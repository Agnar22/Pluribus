#include "game.h"
#include "kuhn_poker.h"
#include "tree.h"
#include <vector>
#include <stdlib.h>
#include <unordered_map>
#include <utility>
#include <string>
#include <algorithm>
#include "../lib/robin_hood.h"


namespace mccfr {

    //std::unordered_map<uint64_t, std::unordered_map<Move, float>> regret, strategy;
    Tree* tree_root;

    /*
    std::unordered_map<uint64_t, std::unordered_map<Move, float>> calculate_probabilities() {
        std::unordered_map<uint64_t, std::unordered_map<Move, float>> probabilities;
        for (auto const [infoset, infoset_regret]:regret) {
            float sum = 0;
            for (auto const [action, action_regret]:infoset_regret) {
                sum += strategy[infoset][action];
            }
            for (auto const [action, action_regret]:infoset_regret) {
                probabilities[infoset][action] = strategy[infoset][action] / sum;
            }
        }
        return probabilities;
    };
    */

    Move sample_action(std::vector<Move>& actions, std::vector<float>& probabilities) {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float r_copy = r;
        for (int x=0; x<actions.size(); ++x) {
            r-=probabilities[x];
            if (r<=1.0e-7f)
                return actions[x];
        }
        throw std::runtime_error("Could not decide upon an action. the sum of strategies is lower than 1.0.");
    }

    void calculate_strategy(Tree* tree, Cards card, Game& game, int player, std::vector<Move>& actions, std::vector<float>& probabilities) {
        if (!game.is_player_to_move(player))
            throw std::runtime_error("Calculating strategy for wrong player.");

        float sum = 0;
        for (Move action:actions)
            sum += std::max(tree->get_regret(card, action), 0.0f);
        for (Move action:actions)
            probabilities.emplace_back(sum > 0 ? std::max(tree->get_regret(card, action), 0.0f)/sum : 1.0f/static_cast<float>(actions.size()));
    }

    void update_strategy(Tree* tree, Game& game, int player) {
        if (game.is_finished() || !game.is_player_in_hand(player) || game.betting_round() > 0) {
            return;
        } else if (game.is_chance_node()) {
            Move action = game.sample_action();
            game.execute(action);
            update_strategy(tree->get_child(action), game, player);
            game.undo();
        } else if (game.is_player_to_move(player)) {
            uint64_t infoset = game.get_infoset(player);
            std::vector<Move> actions;
            actions.reserve(MAX_MOVES);
            actions = game.get_actions(actions);
            std::vector<float> probabilities;
            probabilities.reserve(MAX_MOVES);
            calculate_strategy(tree, game.get_card(player), game, player, actions, probabilities);
            Move action = sample_action(actions, probabilities);
            tree->set_strategy(game.get_card(player), action, tree->get_strategy(game.get_card(player), action)+1);

            game.execute(action);
            update_strategy(tree->get_child(action), game, player);
            game.undo();
        } else {
            std::vector<Move> actions;
            actions.reserve(MAX_MOVES);
            actions = game.get_actions(actions);
            for (Move action:actions) {
                game.execute(action);
                update_strategy(tree->get_child(action), game, player);
                game.undo();
            }
        }
    }

    float traverse_mccfr(Tree* tree, Game& game, int player, bool prune) {
        if (game.is_finished()) {
            return game.get_outcome_for_player(player);
        } else if (!game.is_player_in_hand(player)) {
            // TODO: It is possible that we can skip the recursion.
            Move action = game.get_random_action();

            game.execute(action);
            float outcome = traverse_mccfr(tree->get_child(action), game, player, prune);
            game.undo();
            return outcome;
        } else if (game.is_chance_node()) {
            Move action = game.sample_action();

            game.execute(action);
            float outcome = traverse_mccfr(tree->get_child(action), game, player, prune);
            game.undo();
            return outcome;
        } else if (game.is_player_to_move(player)) {
            uint64_t infoset = game.get_infoset(player);
            std::vector<Move> actions;
            actions.reserve(MAX_MOVES);
            actions = game.get_actions(actions);
            std::vector<float> probabilities;
            probabilities.reserve(MAX_MOVES);
            calculate_strategy(tree, game.get_card(player), game, player, actions, probabilities);

            float expected_value = 0;
            std::vector<float> outcomes(actions.size(), 0.0f);
            std::vector<bool> explored(actions.size(), false);
            for (int x=0; x<actions.size(); ++x) {
                if (!prune || tree->get_regret(game.get_card(player), actions[x]) > -300000000.0f) {
                    explored[actions[x]] = true;
                    game.execute(actions[x]);
                    float outcome = traverse_mccfr(tree->get_child(actions[x]), game, player, prune);
                    game.undo();
                    expected_value += probabilities[x] * outcome;
                    outcomes[x] = outcome;
                }
            }
            for (Move action:actions) {
                if (!prune || explored[action])
                    tree->set_regret(game.get_card(player), action, tree->get_regret(game.get_card(player), action) + outcomes[action] - expected_value);
            }
            return expected_value;
        } else {
            std::vector<Move> actions;
            actions.reserve(MAX_MOVES);
            actions = game.get_actions(actions);
            std::vector<float> probabilities;
            probabilities.reserve(MAX_MOVES);
            calculate_strategy(tree, game.get_card(game.get_player_to_move()), game, game.get_player_to_move(), actions, probabilities);
            Move action = sample_action(actions, probabilities);

            game.execute(action);
            float outcome = traverse_mccfr(tree->get_child(action), game, player, prune);
            game.undo();
            return outcome;
        }
    }


    void mccfr_p(int timesteps, int strategy_interval, int prune_treshold, int lcfr_treshold, int disc_interval, Game& game) {
        // Optional: set all strategies and rewares to zero.
        Tree* tree = new Tree();
        int num_players = game.get_num_players();
        for (int timestep = 0; timestep < timesteps; ++timestep){
            for (int player=0; player<num_players; ++player) {
                game.reset_game();
                if (timestep%strategy_interval==0) {
                    update_strategy(tree, game, player);
                }
                if (timestep>prune_treshold) {
                    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    if (r < 0.05) {
                        traverse_mccfr(tree, game, player, false);
                    } else {
                        traverse_mccfr(tree, game, player, true);
                    }
                } else {
                    traverse_mccfr(tree, game, player, false);
                }
            }
            /*
            if (timestep < lcfr_treshold && timestep%disc_interval==0) {
                float d = (static_cast <float> (timestep)/static_cast <float> (disc_interval) ) / ((static_cast <float> (timestep)/static_cast <float> (disc_interval) + 1.0f));
                for (int player=0; player<num_players; ++player) {
                    // Info: only getting encountered infosets where it is the player to act.
                    std::vector<Tree*> encountered_infosets=game.get_encountered_infosets(player);
                    
                    //for (int x=0; x<
                    for (uint64_t infoset:game.get_encountered_infosets(player)) {
                        for (Move action:game.get_actions_from_infoset(infoset)) {
                            tree->set_regret(
                            regret[infoset][action] = regret[infoset][action] * d;
                            strategy[infoset][action] = strategy[infoset][action] * d;
                        }
                    }
                }
            }
            */
        }
    }
}
/*
void print_strategy() {
    for (auto const [infoset, infoset_regret]:mccfr::regret) {
        std::cout << infoset_to_string(infoset) << ": ";
        for (auto const [action, regret]:infoset_regret) {
            std::cout << move_to_char[action] << " " << regret << " " << std::to_string(mccfr::strategy[infoset][action]) << " ";
        }
        std::cout << std::endl;
    }
}
*/



/*
int main() {
    KuhnPoker khun_poker;

    auto start = std::chrono::high_resolution_clock::now();
    mccfr::mccfr_p(100000, 5, 200, 3, 2, khun_poker);
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << std::endl;
    print_strategy();
}
*/
