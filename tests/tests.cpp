#include "tests.h"

namespace hand_test {

    ul find_strength(Wincondition kind, Card value, Card kicker, bool aces_at_bottom, ul hand_mask) {
        /*
            Calculated the strength of a hand based on the following parameters:
                - kind: the kind of hand (royal_flush, straight_flush, etc.).
                - value: the value of the kind (eg. the value of the most dominating feature of the hand).
                - kicker: the kicker of the hand.
                - aces_at_bottom: true if aces at the bottom are used to get a straight or a staight-flush (eg. A-2-3-4-5).
                - hand_mask:
        */

        // Royal flush or straight/stright-flush with aces at the bottom.
        if (kind == Wincondition::royal_flush || aces_at_bottom) {
            return (static_cast<ul>(kind)<<26);
        // Straight flush or straight.
        } else if (kind == Wincondition::straight_flush || kind == Wincondition::straight) {
            return (static_cast<ul>(kind)<<26) | ( 1UL<<int(kicker) );
        // Flush or high card.
        } else if (kind == Wincondition::flush || kind == Wincondition::high_card) {
            return (static_cast<ul>(kind)<<26) | hand_mask;
        // Three of a kind or pair.
        } else if (kind == Wincondition::three_of_a_kind || kind == Wincondition::pair) {
            return (static_cast<ul>(kind)<<26) | ( 1UL<<(13 + int(value)) ) | hand_mask;
        // Two pair.
        } else if (kind == Wincondition::two_pair) {
            return (static_cast<ul>(kind)<<26) | ( 1UL<<(13 + int(value)) ) | ( 1UL<<(13 + int(kicker)) ) | hand_mask;
        // Four of a kind or full house.
        } else {
            return (static_cast<ul>(kind)<<26) | ( 1UL<<(13 + int(value)) ) | ( 1UL<<int(kicker) );
        }
    }

    ull build_hand(std::vector< std::pair<Card, Card> > cards) {
        ull hand = 0;
        for (auto card : cards) {
            hand |= 1ULL<<(int(card.first) + 4*int(card.second));
        }
        return hand;
    }

    ul create_mask(std::vector<Card> cards) {
        ul hand_mask = 0;
        for (auto card : cards)
            hand_mask |= 1<<int(card);

        return hand_mask;
    }

    std::vector< std::pair<Card, Card> > parse_hand_string(std::string hand) {
        std::istringstream s(hand);
        std::vector< std::pair<Card, Card> > card_hand;
        std::map< char, Card > suit = { {'c', Card::c}, {'h', Card::h}, {'s', Card::s}, {'d', Card::d} };
        std::map< std::string, Card > value = {
                                                    {"2", Card::two}, {"3", Card::three}, {"4", Card::four}, {"5", Card::five},
                                                    {"6", Card::six}, {"7", Card::seven}, {"8", Card::eight}, {"9", Card::nine},
                                                    {"10", Card::ten}, {"j", Card::jack}, {"q", Card::queen}, {"k", Card::king},
                                                    {"a", Card::ace}
                                              };
        std::string tmp;
        while (s >> tmp) {
            card_hand.push_back( std::make_pair(suit[tmp[0]], value[tmp.substr(1)]) );
        }
        return card_hand;
    }

    ull hand_from_string(std::string hand) {
        ull hand_ull = 0ULL;
        std::vector< std::pair<Card, Card> > card_pairs = hand_test::parse_hand_string(hand);

        for (auto card_pair : card_pairs)
            hand_ull |= 1ULL<<(int(card_pair.first) + 4 * int(card_pair.second));

        return hand_ull;
    }

    std::vector<Card> extract_unique(std::vector< std::pair<Card, Card> > cards, unsigned int num_extractions) {
            std::vector<Card> hand_values;
            unsigned int pos = cards.size()-1;
            while (hand_values.size() < num_extractions) {
                // Add card if the next and the previous card is not equal.
                if (cards[pos].second != cards[pos-1].second && ((pos == cards.size() - 1) || cards[pos].second != cards[pos+1].second)) {
                    hand_values.push_back(cards[pos].second);
                }
                pos--;
            }
            return hand_values;
    }

    ul find_and_check_strength(Wincondition kind, std::string cards_s, Card value, Card kicker, bool aces_at_bottom) {
        std::vector< std::pair<Card, Card> > cards = hand_test::parse_hand_string(cards_s);
        ull hand = hand_test::build_hand(cards);

        ul hand_mask;
        if (kind == Wincondition::flush) {
            std::vector<int> suit_num = { 0, 0, 0, 0 };
            for (auto card : cards)
                suit_num[int(card.first)]+=1;

            int pos = cards.size()-1;
            std::vector<Card> hand_values;
            while (hand_values.size() < 5) {
                if (suit_num[int(cards[pos].first)] >= 5) {
                    hand_values.push_back(cards[pos].second);
                }
                pos--;
            }
            hand_mask = hand_test::create_mask(hand_values);
        } else if (kind == Wincondition::three_of_a_kind) {
            hand_mask = hand_test::create_mask(extract_unique(cards, 2));
        } else if (kind == Wincondition::two_pair) {
            std::vector<Card> hand_values;
            unsigned int pos = cards.size()-1;
            int skipped = 0;
            while (hand_values.size() < 1) {
                // Add card if the next and the previous card is not equal or if skipped four cards already.
                if ((cards[pos].second != cards[pos-1].second &&
                    ((pos == cards.size() - 1) || cards[pos].second != cards[pos+1].second)) ||
                    skipped == 4) {
                    hand_values.push_back(cards[pos].second);
                } else {
                    skipped++;
                }
                pos--;
            }
            hand_mask = hand_test::create_mask(hand_values);
        } else if (kind == Wincondition::pair) {
            hand_mask = hand_test::create_mask(extract_unique(cards, 3));
        } else if (kind == Wincondition::high_card) {
            hand_mask = hand_test::create_mask(extract_unique(cards, 5));
        } else {
            hand_mask = 0UL;
        }

        ul hand_strength = hand_test::find_strength(kind, value, kicker, aces_at_bottom, hand_mask);
        ul calculated_strength = Holdem::CalculateHandStrength(hand);

        EXPECT_EQ(hand_strength, calculated_strength);
        return calculated_strength;
    }

    void calculate_frequency(std::vector<int> &hand_frequency, int upper_bound, int remaining_cards, ull current_hand) {
        if (remaining_cards > 0)
            for (int x = 0; x < upper_bound; ++x)
                hand_test::calculate_frequency(hand_frequency, x, remaining_cards - 1, current_hand | 1ULL<<x);
        else
            hand_frequency[int(Holdem::CalculateHandStrength(current_hand)>>26)]++;
    }

}
