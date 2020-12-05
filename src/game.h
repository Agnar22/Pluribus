#ifndef GAME_H
#define GAME_H

#include <chrono>
#include <vector>
#include <string>
#include <set>
#include <iostream>

class Game {
    public:
        virtual std::set<std::string> get_encountered_infosets(int) {};
        virtual void execute(std::string) {};
        virtual void undo() {};
        virtual void reset_game() {};
        virtual bool is_finished() {};
        virtual bool is_player_in_hand(int) {};
        virtual bool is_chance_node() {};
        virtual int betting_round() {};
        virtual std::string sample_action() {};
        virtual bool is_player_to_move(int) {};
        virtual int get_player_to_move() {};
        virtual std::vector<std::string> get_actions() {};
        virtual std::string get_random_action() {};
        virtual std::string get_infoset(int) {};
        virtual std::string get_current_infoset() {};
        virtual float get_outcome_for_player(int) {};
        virtual std::vector<std::string> get_actions_from_infoset(std::string) {};
        virtual std::vector<int> get_players() {};
};

#endif
