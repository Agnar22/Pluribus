#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <set>

class Game {
    public:
        std::set<std::string> get_encountered_infosets(int);
        void execute(std::string);
        void undo();
        //GameState get_state();
        bool is_finished();
        bool is_player_in_hand(int);
        bool is_chance_node();
        int betting_round();
        std::string sample_action();
        bool is_player_to_move(int);
        std::vector<std::string> get_actions();
        std::string get_random_action();
        std::string get_infoset(int);
        std::string get_current_infoset();
        float get_outcome_for_player(int);
        static std::vector<std::string> get_actions_from_infoset(std::string);
        std::vector<int> get_players();
};

#endif