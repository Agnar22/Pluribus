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

