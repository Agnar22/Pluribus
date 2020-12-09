#ifndef KUHN_POKER_H
#define KUHN_POKER_H

#include "game.h"
#include <unordered_map>


class KuhnPoker: public Game {

    public:
        int player_to_move;
        int players;
        std::vector<Cards> cards;
        std::vector<float> money_in_hand;
        std::vector<bool> has_folded;
        std::vector<Cards> card_for_player;
        History history;
        std::vector<std::set<uint64_t>> encountered_infosets;

        KuhnPoker();
        KuhnPoker(int);
        void initialize_hand();
        void reset_game();
        bool is_finished();
        std::set<uint64_t> get_encountered_infosets(int);
        void execute(Move&);
        void undo();
        bool is_player_to_move(int);
        bool is_player_in_hand(int);
        bool is_chance_node();
        int betting_round();
        int get_player_to_move();
        Move sample_action();
        std::vector<Move>& get_actions(std::vector<Move>&);
        Move get_random_action();
        uint64_t get_infoset(int);
        uint64_t get_current_infoset();
        float get_outcome_for_player(int);
        std::vector<Move> get_actions_from_infoset(uint64_t);
        int get_num_players();

    private:
        Cards find_best_remaining_hand();
        void draw_cards();
        std::unordered_map<uint64_t, std::vector<Move>> actions_for_infoset;
};


#endif
