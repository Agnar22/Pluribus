#ifndef KUHN_POKER_H
#define KUHN_POKER_H

#include "game.h"
#include <unordered_map>


class KuhnPoker: public Game {

    public:

        KuhnPoker();
        KuhnPoker(int);
        void reset_game();
        inline int get_num_players();
        inline int get_player_to_move();
        inline uint64_t get_infoset(int);
        inline uint64_t get_current_infoset();
        inline bool is_player_to_move(int);
        inline bool is_player_in_hand(int);
        inline bool is_chance_node();
        inline int betting_round();
        void execute(Move&);
        void undo();
        bool is_finished();
        float get_outcome_for_player(int);
        std::vector<Move>& get_actions(std::vector<Move>&);
        Move get_random_action();
        inline Move sample_action();
        std::set<uint64_t> get_encountered_infosets(int);
        std::vector<Move> get_actions_from_infoset(uint64_t);

    private:
        int players;
        int player_to_move;
        History history;
        std::vector<Cards> cards;
        std::vector<Cards> card_for_player;
        std::vector<float> money_in_hand;
        std::vector<bool> has_folded;
        std::vector<std::set<uint64_t>> encountered_infosets;
        std::unordered_map<uint64_t, std::vector<Move>> actions_for_infoset;

        void initialize_hand();
        void draw_cards();
        Cards find_best_remaining_hand();
};


#endif
