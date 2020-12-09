#ifndef GAME_H
#define GAME_H

#include <chrono>
#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <bits/stdc++.h>
#include <cassert>

#define assertm(exp, msg) assert(((void)msg, exp))

constexpr int MAX_MOVE_SIZE = 2;
constexpr int MAX_CARD_SIZE = 3;
constexpr int MAX_MOVES = 2;
constexpr uint32_t MOVE_MASK = 3;
constexpr uint32_t CARDS_MASK = 7;

enum class Cards : char {
    A, K, Q, J, NONE
};

enum class Move : uint32_t {
    NONE=0, F, C, R
};

extern std::unordered_map<char, Move> char_to_move;
extern std::unordered_map<Move, char> move_to_char;
extern std::unordered_map<Cards, char> cards_to_char;

extern std::string infoset_to_string(uint64_t infoset);
/*
std::string infoset_to_string(uint64_t infoset) {
    int pos = 0;
    std::string readable_infoset;
    while (infoset & (MOVE_MASK << (MAX_CARD_SIZE + pos * MAX_MOVE_SIZE)))
        readable_infoset += move_to_char[static_cast<Move>(infoset & MOVE_MASK)];
    std::reverse(readable_infoset.begin(), readable_infoset.end());
    readable_infoset += cards_to_char[static_cast<Cards>(infoset & MOVE_MASK)];
    return readable_infoset;
}
*/

struct History {
    uint32_t history;
    int length;

    History() : history{0}, length{0}{};

    History(std::string& history) : history{0}, length{0} {
        for (char& move:history) {
            this->history = (this->history << MAX_MOVE_SIZE) | static_cast<uint32_t>(char_to_move[move]);
            length++;
        }
        assertm(length <= 8*sizeof(this->history) / MAX_MOVE_SIZE, "Not overflow.");
    };

    History(std::string&& history) : history{0},length{0} {
        for (char& move:history) {
            this->history = (this->history << MAX_MOVE_SIZE) | static_cast<uint32_t>(char_to_move[move]);
            length++;
        }
        assertm(length <= 8*sizeof(this->history) / MAX_MOVE_SIZE, "Not overflow.");
    };

    inline void clear() {
        history=0;
        length=0;
    };

    inline int get_length() {
        return this->length;
    };

    int get_first_occurence(Move& move) {
        for (int x=0; x<length; ++x)
            if ((*this)[x] == move)
                return x;
        return -1;
    };

    int get_first_occurence(Move&& move) {
        for (int x=0; x<length; ++x)
            if ((*this)[x] == move)
                return x;
        return -1;
    };

    History operator--(int times) {
        assertm(length > 0, "Not underflow.");
        int dec = times + 1;
        history = history >> (MAX_MOVE_SIZE * dec);
        length-=dec;
        return *this;
    };

    History& operator+=(const Move& move) {
        assertm(length < 8*sizeof(history) / MAX_MOVE_SIZE, "Not overflow.");
        history = ((history << MAX_MOVE_SIZE) | static_cast<uint32_t>(move));
        length++;
        return *this;
    };

    Move operator[](const int pos) {
        assertm(pos >= 0, "Position is positive.");
        assertm(length > pos, "Valid position.");
        return static_cast<Move>(history >> (MAX_MOVE_SIZE * pos) & MOVE_MASK);
    };
};

inline uint64_t create_infoset(History& hist, Cards player_card) {
    return (((uint64_t)hist.history) << MAX_CARD_SIZE) | (uint64_t)player_card;
};

inline uint64_t to_infoset(std::string history, Cards player_card) {
    return (((uint64_t)History(history).history) << MAX_CARD_SIZE) | (uint64_t)player_card;
};

class Game {
    public:
        virtual std::set<uint64_t> get_encountered_infosets(int) {};
        virtual void execute(Move&) {};
        virtual void undo() {};
        virtual void reset_game() {};
        virtual bool is_finished() {};
        virtual bool is_player_in_hand(int) {};
        virtual bool is_chance_node() {};
        virtual int betting_round() {};
        virtual Move sample_action() {};
        virtual bool is_player_to_move(int) {};
        virtual int get_player_to_move() {};
        virtual std::vector<Move>& get_actions(std::vector<Move>&) {};
        virtual Move get_random_action() {};
        virtual uint64_t get_infoset(int) {};
        virtual uint64_t get_current_infoset() {};
        virtual float get_outcome_for_player(int) {};
        virtual std::vector<Move> get_actions_from_infoset(uint64_t) {};
        virtual int get_num_players() {};
};

#endif
