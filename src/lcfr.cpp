#include "game.h"
#include "kuhn_poker.h"
#include <vector>
#include <algorithm>
#include "../lib/robin_hood.h"

namespace lcfr {

    robin_hood::unordered_map<uint64_t, robin_hood::unordered_map<Move, float>> strategy_profile, regret, cumulative_strategy;

    robin_hood::unordered_map<uint64_t, robin_hood::unordered_map<Move, float>> calculate_cumulative_strategy() {
        robin_hood::unordered_map<uint64_t, robin_hood::unordered_map<Move, float>> strategy;
        for (auto const [infoset, infoset_cumulative_strategies] : cumulative_strategy) {
            float sum = 0.0f;
            for (auto const [action, action_cumulative_strategy] : infoset_cumulative_strategies)
                sum += action_cumulative_strategy;
            for (auto const [action, action_cumulative_strategy] : infoset_cumulative_strategies)
                strategy[infoset][action] = action_cumulative_strategy/sum;
        }
        return strategy;
    }

    void calculate_strategy(Game& game, uint64_t infoset, std::vector<float>& probabilities) {
        float sum=0.0f;
        std::vector<Move> actions;
        actions.reserve(MAX_MOVES);
        game.get_actions(actions);
        for (int x=0; x<actions.size(); ++x)
            sum += std::max(regret[infoset][actions[x]], 0.0f);
        for (int x=0; x<actions.size(); ++x)
            //strategy_profile[infoset][actions[x]] = (sum>0.0f) ? regret[infoset][actions[x]]/sum : 1.0f/static_cast<float>(actions.size());
            probabilities[x] = (sum>0.0f) ? std::max(regret[infoset][actions[x]], 0.0f)/sum : 1.0f/static_cast<float>(actions.size());
    }

    float lcfr (Game& game, int player, int timestep, std::vector<float>& player_reach_prob) {
        if (game.is_finished()) {
            //std::cout << infoset_to_string(game.get_infoset(0)) << " " << infoset_to_string(game.get_infoset(1)) << " " << player << " " << game.get_outcome_for_player(player) << std::endl;
            return game.get_outcome_for_player(player);
        } else if (game.is_chance_node()) {
            Move action = game.sample_action();
            game.execute(action);
            float outcome = lcfr(game, player, timestep, player_reach_prob);
            game.undo();
            return outcome;
        }
        float expected_value = 0.0f;
        int current_player = game.get_player_to_move();
        uint64_t infoset = game.get_infoset(current_player);
        std::vector<Move> actions;
        actions.reserve(MAX_MOVES);
        game.get_actions(actions);
        std::vector<float> lcfr_value(actions.size(), 0.0f);
        std::vector<float> infoset_strategy(actions.size(), 0.0f);
        calculate_strategy(game, infoset, infoset_strategy);
        for (int x=0; x<actions.size(); ++x) {
            float prior_player_reach_prob = player_reach_prob[current_player];
            player_reach_prob[current_player] *= infoset_strategy[x];
            game.execute(actions[x]);
            float outcome = lcfr(game, player, timestep, player_reach_prob);
            game.undo();
            player_reach_prob[current_player] = prior_player_reach_prob;
            lcfr_value[x] = outcome;
            expected_value += infoset_strategy[x] * outcome;
        }
        if (player == current_player) {
            float reach_prob = 1.0f;
            for (int x=0; x<game.get_num_players(); ++x)
                if (x!=player)
                    reach_prob *= player_reach_prob[x];
            float disc = ((float)timestep + 1)/(float)(timestep + 2);
            for (int x=0; x<actions.size(); ++x) {
                regret[game.get_current_infoset()][actions[x]] += reach_prob * (lcfr_value[x] - expected_value);
                regret[game.get_current_infoset()][actions[x]] *= disc;
                cumulative_strategy[game.get_current_infoset()][actions[x]] += player_reach_prob[player] * infoset_strategy[x];
                cumulative_strategy[game.get_current_infoset()][actions[x]] *= disc;
            }

            std::vector<float> updated_strategy(actions.size(), 0.0f);
            calculate_strategy(game, infoset, updated_strategy);
            for (int x=0; x<actions.size(); ++x)
                strategy_profile[infoset][actions[x]] = updated_strategy[x];
        }
        return expected_value;
    }

    void search(int timesteps, Game& game) {
        for (int timestep=0; timestep<timesteps; ++timestep) {
            for (int player=0; player<game.get_num_players(); ++player) {
                game.reset_game();
                std::vector<float> player_reach_prob(game.get_num_players(), 1.0f);
                lcfr(game, player, timestep, player_reach_prob);
            }
        }
    }
}
