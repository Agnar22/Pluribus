#include "game.h"
#include "kuhn_poker.h"
#include <vector>
#include <stdlib.h>
#include <unordered_map>
#include <utility>
#include <string>
#include <algorithm>


namespace mccfr {

    std::unordered_map<std::string, std::unordered_map<std::string, float>> regret, strategy;

    std::string sample_action(std::unordered_map<std::string, float>& temp_strategy) {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float r_copy = r;
        for (auto kv:temp_strategy) {
            r-=kv.second;
            if (r<=1.0e-7f)
                return kv.first;
        }
        throw std::runtime_error("Could not decide upon an action. the sum of strategies is lower than 1.0.");
    }

    std::unordered_map<std::string, float> calculate_strategy(Game& game, int player) {
        std::unordered_map<std::string, float> updated_strategy;
        std::string infoset = game.get_infoset(player);

        if (!game.is_player_to_move(player)) {
            throw std::runtime_error("Calculating strategy for wrong player.");
        }

        float sum = 0;
        for (std::string action:game.get_actions())
            sum+=std::max(regret[infoset][action], 0.0f);
        for (std::string action:game.get_actions())
            updated_strategy[action] = sum > 0 ? std::max(regret[infoset][action], 0.0f)/sum : 1.0f/static_cast <float>(regret[infoset].size());
        return updated_strategy;
    }

    void update_strategy(Game& game, int player) {
        if (game.is_finished() || !game.is_player_in_hand(player) || game.betting_round() > 0) {
            return;
        } else if (game.is_chance_node()) {
            std::string action = game.sample_action();
            game.execute(action);
            update_strategy(game, player);
            game.undo();
        } else if (game.is_player_to_move(player)) {
            std::string infoset = game.get_infoset(player);
            auto curr_strategy = calculate_strategy(game, player);
            std::string action = sample_action(curr_strategy);
            strategy[infoset][action] = strategy[infoset][action] + 1;

            game.execute(action);
            update_strategy(game, player);
            game.undo();
        } else {
            for (std::string action:game.get_actions()) {
                game.execute(action);
                update_strategy(game, player);
                game.undo();
            }
        }
    }

    float traverse_mccfr(Game& game, int player, bool prune) {
        if (game.is_finished()) {
            return game.get_outcome_for_player(player);
        } else if (!game.is_player_in_hand(player)) {
            // TODO: it is possible that we can skip the recursion.
            std::string action = game.get_random_action();

            game.execute(action);
            float outcome = traverse_mccfr(game, player, prune);
            game.undo();
            return outcome;
        } else if (game.is_chance_node()) {
            std::string action = game.sample_action();

            game.execute(action);
            float outcome = traverse_mccfr(game, player, prune);
            game.undo();
            return outcome;
        } else if (game.is_player_to_move(player)) {
            std::string infoset = game.get_infoset(player);
            auto curr_strategy = calculate_strategy(game, player);
            float expected_value = 0;
            std::unordered_map<std::string, float> outcomes;
            std::unordered_map<std::string, bool> explored;
            for (std::string action:game.get_actions()) {
                if (!prune || regret[infoset][action] > -300000000.0f) {
                    explored[action] = true;
                    game.execute(action);
                    float outcome = traverse_mccfr(game, player, prune);
                    game.undo();
                    expected_value += curr_strategy[action] * outcome;
                    outcomes[action] = outcome;
                } else {
                    explored[action] = false;
                }
            }
            for (std::string action:game.get_actions()) {
                if (!prune || explored[action])
                    regret[infoset][action] = regret[infoset][action] + outcomes[action] - expected_value;
            }
            return expected_value;
        } else {
            auto curr_strategy = calculate_strategy(game, game.get_player_to_move());
            std::string action = sample_action(curr_strategy);

            game.execute(action);
            float outcome = traverse_mccfr(game, player, prune);
            game.undo();
            return outcome;
        }
    }


    void mccfr_p(int timesteps, int strategy_interval, int prune_treshold, int lcfr_treshold, int disc_interval, Game& game) {
        // Optional: set all strategies and rewares to zero.
        std::vector<int> players = game.get_players();
        for (int timestep = 0; timestep < timesteps; ++timestep){
            for (int player:players) {
                game.reset_game();
                if (timestep%strategy_interval==0)
                    update_strategy(game, player);
                if (timestep>prune_treshold) {
                    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    if (r < 0.05) {
                        traverse_mccfr(game, player, false);
                    } else {
                        traverse_mccfr(game, player, true);
                    }
                } else {
                    traverse_mccfr(game, player, false);
                }
            }
            if (timestep < lcfr_treshold && timestep%disc_interval==0) {
                float d = (static_cast <float> (timestep)/static_cast <float> (disc_interval) ) / ((static_cast <float> (timestep)/static_cast <float> (disc_interval) + 1.0f));
                for (int player:players) {
                    // Info: only getting encountered infosets where it is the player to act.
                    for (std::string infoset:game.get_encountered_infosets(player)) {
                        for (std::string action:game.get_actions_from_infoset(infoset)) {
                            regret[infoset][action] = regret[infoset][action] * d;
                            strategy[infoset][action] = strategy[infoset][action] * d;
                        }
                    }
                }
            }
        }
    }
}

void print_strategy() {
    for (auto const [infoset, infoset_regret]:mccfr::regret) {
        std::cout << infoset << ": ";
        for (auto const [action, regret]:infoset_regret) {
            std::cout << action << " " << regret << " " << std::to_string(mccfr::strategy[infoset][action]) << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    KuhnPoker khun_poker;

    auto start = std::chrono::high_resolution_clock::now();
    mccfr::mccfr_p(100000, 5, 200, 3, 2, khun_poker);
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << std::endl;
    print_strategy();
}
