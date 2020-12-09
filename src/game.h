#ifndef GAME_H
#define GAME_H

#include <chrono>
#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <cassert>

#define assertm(exp, msg) assert(((void)msg, exp))

constexpr int MAX_MOVE_SIZE = 2;
constexpr int MAX_CARD_SIZE = 3;
constexpr int MAX_MOVES = 2;
constexpr uint32_t MOVE_MASK = 3;

enum class Cards : char {
    A, K, Q, J
};

enum class Move : uint32_t {
    NONE=0, F, C, R
};

extern std::unordered_map<char, Move> char_to_move;

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
