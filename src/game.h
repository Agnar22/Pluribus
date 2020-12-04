#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <set>

class Game {
    public:
        virtual std::set<std::string> get_encountered_infosets(int) = 0;
        virtual void execute(std::string) = 0;
        virtual void undo() = 0;
        virtual bool is_finished() = 0;
        virtual bool is_player_in_hand(int) = 0;
        virtual bool is_chance_node() = 0;
        virtual int betting_round() = 0;
        virtual std::string sample_action() = 0;
        virtual bool is_player_to_move(int) = 0;
        virtual int get_player_to_move() = 0;
        virtual std::vector<std::string> get_actions() = 0;
        virtual std::string get_random_action() = 0;
        virtual std::string get_infoset(int) = 0;
        virtual std::string get_current_infoset() = 0;
        virtual float get_outcome_for_player(int) = 0;
        virtual std::vector<std::string> get_actions_from_infoset(std::string) = 0;
        virtual std::vector<int> get_players() = 0;
};

#endif
