#include "game.h"

std::unordered_map<char, Move> char_to_move = {
    {'F', Move::F},
    {'C', Move::C},
    {'R', Move::R}
};

std::unordered_map<Move, char> move_to_char = {
    {Move::F, 'F'},
    {Move::C, 'C'},
    {Move::R, 'R'}
};

std::unordered_map<Cards, char> cards_to_char = {
    {Cards::A, 'A'},
    {Cards::K, 'K'},
    {Cards::Q, 'Q'},
    {Cards::J, 'J'}
};

std::string infoset_to_string(uint64_t infoset) {
    int pos = 0;
    std::string readable_infoset;
    while ((infoset >> (MAX_CARD_SIZE + pos * MAX_MOVE_SIZE)) & MOVE_MASK) {
        Move curr_move = static_cast<Move>((infoset >> (MAX_CARD_SIZE + pos * MAX_MOVE_SIZE)) & MOVE_MASK);
        readable_infoset += move_to_char[curr_move];
        ++pos;
    }
    std::reverse(readable_infoset.begin(), readable_infoset.end());
    readable_infoset += cards_to_char[static_cast<Cards>(infoset & CARDS_MASK)];
    return readable_infoset;
}

