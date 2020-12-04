#ifndef KUHN_POKER_H
#define KUHN_POKER_H

#include "game.h"


class KuhnPoker: public Game {

    public:
        int player_to_move;
        float money_in_hand[2];
        bool has_folded[2];
        char card_for_player[2];
        std::vector<std::string> history;
        std::vector<std::set<std::string>> encountered_infosets;

        KuhnPoker();
        void initialize_hand();
        void reset_game();
        bool is_finished();
        std::set<std::string> get_encountered_infosets(int);
        void execute(std::string);
        void undo();
        bool is_player_to_move(int);
        bool is_player_in_hand(int);
        bool is_chance_node();
        int betting_round();
        int get_player_to_move();
        std::string sample_action();
        std::vector<std::string> get_actions();
        std::string get_random_action();
        std::string get_infoset(int);
        std::string get_current_infoset();
        float get_outcome_for_player(int);
        std::vector<std::string> get_actions_from_infoset(std::string);
        std::vector<int> get_players();

    private:
        std::pair<char, char> card_combination_to_chars(int);
        void draw_cards();
};


#endif