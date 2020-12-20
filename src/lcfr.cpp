#include "game.h"
#include "kuhn_poker.h"
#include <vector>
#include <algorithm>
#include "../lib/robin_hood.h"

namespace lcfr {

    robin_hood::unordered_map<uint64_t, robin_hood::unordered_map<Move, float>> regret, cumulative_strategy;

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

    void calculate_strategy(Game& game, std::vector<Move>& actions, uint64_t infoset, std::vector<float>& probabilities) {
        float sum=0.0f;
        for (int x=0; x<actions.size(); ++x)
            sum += std::max(regret[infoset][actions[x]], 0.0f);
        for (int x=0; x<actions.size(); ++x)
            //strategy_profile[infoset][actions[x]] = (sum>0.0f) ? regret[infoset][actions[x]]/sum : 1.0f/static_cast<float>(actions.size());
            probabilities[x] = (sum>0.0f) ? std::max(regret[infoset][actions[x]], 0.0f)/sum : 1.0f/static_cast<float>(actions.size());
    }

    float lcfr (Game& game, int player, int timestep, std::vector<float>& player_reach_prob, double pos_pow, double neg_pow, double strat_pow) {
        if (game.is_finished()) {
            //std::cout << infoset_to_string(game.get_infoset(0)) << " " << infoset_to_string(game.get_infoset(1)) << " " << player << " " << game.get_outcome_for_player(player) << std::endl;
            return game.get_outcome_for_player(player);
        } else if (game.is_chance_node()) {
            Move action = game.sample_action();
            game.execute(action);
            float outcome = lcfr(game, player, timestep, player_reach_prob, pos_pow, neg_pow, strat_pow);
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
        calculate_strategy(game, actions, infoset, infoset_strategy);
        for (int x=0; x<actions.size(); ++x) {
            float prior_player_reach_prob = player_reach_prob[current_player];
            player_reach_prob[current_player] *= infoset_strategy[x];
            game.execute(actions[x]);
            float outcome = lcfr(game, player, timestep, player_reach_prob, pos_pow, neg_pow, strat_pow);
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
            double pos_disc = std::pow((double)(timestep + 1), pos_pow)/(std::pow((double)(timestep + 1), pos_pow) + 1.0);
            double neg_disc = std::pow((double)(timestep + 1), neg_pow)/(std::pow((double)(timestep + 1), neg_pow) + 1.0);
            double strat_disc = std::pow((double)(timestep + 1)/(double)(timestep + 2), strat_pow);
            for (int x=0; x<actions.size(); ++x) {
                regret[infoset][actions[x]] += reach_prob * (lcfr_value[x] - expected_value);
                regret[infoset][actions[x]]*= regret[infoset][actions[x]] > 0.0f ? pos_disc:neg_disc;
                cumulative_strategy[infoset][actions[x]] += player_reach_prob[player] * infoset_strategy[x];
                cumulative_strategy[infoset][actions[x]] *= strat_disc;
            }
        }
        return expected_value;
    }

    void search(int timesteps, Game& game) {
        for (int timestep=0; timestep<timesteps; ++timestep) {
            for (int player=0; player<game.get_num_players(); ++player) {
                game.reset_game();
                std::vector<float> player_reach_prob(game.get_num_players(), 1.0f);
                lcfr(game, player, timestep, player_reach_prob, 1.5, 0.5, 3.0);
            }
        }
    }
}
