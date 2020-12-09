#include "holdem.h"
#include "calculations.h"


unsigned long long compress_hand_lossless(unsigned long long hand) {
    /*
        A lossless compression of a poker hand exploits the fact that suits are
        indifferent; the only thing that is important is to keep the same cards in the
        same groups.
    */
    unsigned long long compressed_hand = 0ULL;
    unsigned long long value_mask = 0x0001111111111111;
    std::vector< std::pair<unsigned long long, int> > suit_value = {
        std::make_pair(0ULL, 0),
        std::make_pair(0ULL, 1),
        std::make_pair(0ULL, 2),
        std::make_pair(0ULL, 3)
    };

    for (int suit = 0; suit < 4; ++suit)
        suit_value[suit].first = (hand>>suit) & value_mask;

    sort(
        suit_value.begin(),
        suit_value.end(),
        [](const std::pair<unsigned long long, int> &l, std::pair<unsigned long long, int> &r) -> bool {
            return l.first > r.first;
        }
    );

    for (int suit = 0; suit < 4; ++suit) {
        if (suit - suit_value[suit].second >= 0)
            compressed_hand |= (hand<<(suit - suit_value[suit].second)) & (value_mask<<suit);
        else
            compressed_hand |= (hand>>(suit_value[suit].second - suit)) & (value_mask<<suit);
    }

    return compressed_hand;
}

namespace calculations {

    std::unordered_map< unsigned long long, int > suit_permutations;
    std::unordered_map< int, std::vector<int> > hand_frequencies;

    void calculate_suit_permutations(unsigned long long current_hand, int upper_bound, int max_cards) {
        if (max_cards == 0) {
            unsigned long long compressed_hand = compress_hand_lossless(current_hand);
            if (calculations::suit_permutations.find(compressed_hand) != calculations::suit_permutations.end())
                calculations::suit_permutations[compressed_hand]++;
            else
                calculations::suit_permutations[compressed_hand] = 1;
        } else {
            for (int x = 0; x < upper_bound; x++) {
                if (current_hand & 1ULL<<x)
                    continue;
                calculations::calculate_suit_permutations(current_hand | 1ULL<<x, x, max_cards - 1);
            }
        }
    }

    void load_suit_permutations() {
        std::string file_name = "../../files/suit_permutations.txt";

        if (!calculations::suit_permutations.empty())
            return;

        if (std::filesystem::exists(file_name)) {
            std::ifstream ifs(file_name);
            boost::archive::text_iarchive ia(ifs);
            ia >> calculations::suit_permutations;
            ifs.close();
        } else {
            for (int max_cards = 1; max_cards <= 7; ++max_cards)
                calculations::calculate_suit_permutations(0ULL, 52, max_cards);
            std::ofstream ofs(file_name);
            boost::archive::text_oarchive oa(ofs);
            oa << calculations::suit_permutations;
            ofs.close();
        }
    }

    int num_suit_permutations(unsigned long long hand) {
        if (calculations::suit_permutations.empty())
            calculations::load_suit_permutations();

        return calculations::suit_permutations[compress_hand_lossless(hand)];
    }

    void hand_frequency_(std::vector<int> &frequencies, unsigned long long current_hand, int upper_bound) {
        if (__builtin_popcountll(current_hand) == 7) {
            frequencies[Holdem::CalculateHandStrength(current_hand)>>26]++;
        } else {
            for (int x = 0; x < upper_bound; x++) {
                if (current_hand & 1ULL<<x)
                    continue;
                calculations::hand_frequency_(frequencies, current_hand | 1ULL<<x, x);
            }
        }
    }

    std::vector<int> hand_frequency(unsigned long long player_mask) {
        unsigned long long player_mask_compressed = compress_hand_lossless(player_mask);

        if (calculations::hand_frequencies.find(player_mask_compressed) != calculations::hand_frequencies.end())
            return calculations::hand_frequencies[player_mask_compressed];

        std::vector<int> frequencies(10,0);
        calculations::hand_frequency_(frequencies, player_mask_compressed, 52);

        calculations::hand_frequencies[player_mask_compressed] = frequencies;
        return frequencies;
    }

    void headsup_tabled_outcomes(unsigned long long player_cards, unsigned long long opponent_cards, unsigned long long board_cards, int upper_bound, std::vector<unsigned long> &outcomes) {
        if (__builtin_popcountll(board_cards) == 5) {
            unsigned long our_strength = Holdem::CalculateHandStrength(player_cards | board_cards);
            unsigned long opponent_strength = Holdem::CalculateHandStrength(opponent_cards | board_cards);
            if (our_strength > opponent_strength)
                outcomes[2]++;
            else if (our_strength == opponent_strength)
                outcomes[1]++;
            else
                outcomes[0]++;
        } else {
            for (int x = 0; x < upper_bound; ++x) {
                if ( (player_cards | opponent_cards | board_cards) & (1ULL<<x) )
                    continue;
                headsup_tabled_outcomes(player_cards, opponent_cards, board_cards | (1ULL<<x), x, outcomes);
            }
        }
    }

    std::vector<unsigned long> headsup_outcomes(unsigned long long player_cards, unsigned long long board_cards) {
        std::vector<unsigned long> outcomes(3,0);
        for (int x = 0; x < 52; ++x) {
            for (int y = 0; y < x; ++y) {
                if ( (player_cards | board_cards) & ((1ULL<<x) | (1ULL<<y)) )
                    continue;
                headsup_tabled_outcomes(player_cards, (1ULL<<x) | (1ULL<<y), board_cards, 52, outcomes);
            }
        }
        return outcomes;
    }
} // namespace calculations

