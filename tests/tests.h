#ifndef TESTS_H
#define TESTS_H

#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <map>
#include "../src/card_deck.h"
#include "../src/holdem.h"

typedef unsigned long ul;
typedef unsigned long long ull;

enum class Wincondition : unsigned long long {
    high_card, pair, two_pair, three_of_a_kind, straight, flush, full_house, four_of_a_kind, straight_flush, royal_flush
};

namespace hand_test {

    ul find_strength(Wincondition kind, Card value, Card kicker, bool aces_at_bottom, ul hand_mask);
    ull build_hand(std::vector< std::pair<Card, Card> > cards);
    ul create_mask(std::vector<Card> cards);
    std::vector< std::pair<Card, Card> > parse_hand_string(std::string hand);
    ull hand_from_string(std::string hand);
    std::vector<Card> extract_unique(std::vector< std::pair<Card, Card> > cards, unsigned int num_extractions);
    ul find_and_check_strength(Wincondition kind, std::string cards_s, Card value, Card kicker, bool aces_at_bottom);
    void calculate_frequency(std::vector<int>&, int, int, ull);

}

#endif
