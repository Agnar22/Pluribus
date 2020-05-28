#include "holdem.h"
#include "card_deck.h"
#include <chrono>



Holdem::Holdem(int num_players) {

}

// GetMoves
// ExecuteMove
// GetState
// CalculateWinningProbForEachWinCategory
// IsFinished
bool Holdem::IsFinished() {
    return true;
}
// GetOutCome


/*
    Variables: bigblindsize, smallblindsize, dealer (bigblind and small blind), playerinfor, revealedcards, players.
*/

unsigned long Holdem::CalculateHighestMask(unsigned long long cards, int n) {
    /*
        CalculateHighestMask takes the card hand as input. It ouputs the mask of the top n most valuable cards in the hand.
        It assumes that there are no cards with the same value.
    */
    unsigned long card_mask = 0;

    while (n) {
        int msb = 63 - __builtin_clzll(cards);
        cards ^= 1ULL<<msb;
        card_mask |= 1ULL<<(msb / 4);
        n--;
    }
    return card_mask;
}

unsigned long Holdem::CalculateHandStrength(unsigned long long player_mask) {
    const unsigned long long number_mask = 0x0001111111111111ULL;
    const unsigned long long ace_mask = 0x000F000000000000ULL;

    // __builtin_clzll = number of leading 0s.
    // __builtin_ctzll = number of trailing 0s.
    // __builtin_popcountll = number of set bits.

    // Royal flush.
    if (__builtin_popcountll( 0x0001111100000000ULL & player_mask ) == 5 ||
        __builtin_popcountll( (0x0001111100000000ULL<<1) & player_mask ) == 5 ||
        __builtin_popcountll( (0x0001111100000000ULL<<2) & player_mask ) == 5 ||
        __builtin_popcountll( (0x0001111100000000ULL<<3) & player_mask ) == 5
    ) {
        return 9UL<<26;
    }

    // Straight flush.
    unsigned long long straight_flush = player_mask & (player_mask>>4) & (player_mask>>8) & (player_mask>>12) & (player_mask>>16);
    if (straight_flush) {
        return (8UL<<26) | (1UL<<( (63 - __builtin_clzll(straight_flush)) / 4) );
    }

    // Straight flush with aces at bottom.
    if (ace_mask & player_mask) {
        unsigned long long aces_down = ( (player_mask & ~ace_mask)<<4 ) | ( (player_mask & ace_mask)>>(12*4) );
        if (aces_down & (aces_down>>4) & (aces_down>>8) & (aces_down>>12) & (aces_down>>16)) {
            return (8UL<<26);
        }
    }

    // Four of a kind.
    unsigned long long four_of_a_kind = player_mask & (player_mask>>1) & (player_mask>>2) & (player_mask>>3) & number_mask;
    if (four_of_a_kind) {
        unsigned long long fours_value = __builtin_ctzll(four_of_a_kind) / 4; // The card number of the four of a kind.
        unsigned long fifth_mask = Holdem::CalculateHighestMask(player_mask & ~(0x000000000000000FULL<<(fours_value*4)), 1); // The card number of the last card.
        return (7UL<<26) | (1UL<<(fours_value + 13)) | fifth_mask;
    }

    // Full house.
    unsigned long long twos = ~( player_mask ^ (player_mask>>1) ^ (player_mask>>2) ^ (player_mask>>3) ) & // Ensures that there are 0, 2 or 4 1s.
                               ( player_mask | (player_mask>>1) | (player_mask>>2) | (player_mask>>3) ) & // There is at least one 1.
                               number_mask;
    unsigned long long threes = ( player_mask | (player_mask>>1) ) & ( (player_mask>>2) | (player_mask>>3) ) & number_mask;
    threes ^= threes & twos;
    if (__builtin_popcountll(threes) > 0 && __builtin_popcountll(threes) + __builtin_popcountll(twos) >=2) {
        unsigned long long threes_mask = 1UL<<((63 - __builtin_clzll(threes))/4); // The card number of the three equal cards.
        unsigned long long twos_mask; // The card number of the two equal cards.
        if (twos) { // If there are twos, there can not be an additional three of a kind.
            twos_mask = 1UL<<((63 - __builtin_clzll(twos))/4);
        } else {
            twos_mask = 1UL<<(__builtin_ctzll(threes)/4);
        }
        return (6UL<<26) | (threes_mask<<13) | twos_mask;
    }

    // Flush.
    for (int x = 0; x < 4; ++x) {
        unsigned long long flush = player_mask & (number_mask<<x);
        if (__builtin_popcountll(flush) > 4) {
            // TODO: Speedup possible if explioting that we want 5 of the 7 cards.
            return (5UL<<26) | Holdem::CalculateHighestMask(flush, 5);
        }
    }

    // Straight.
    unsigned long long straight = ( player_mask | (player_mask>>1) | (player_mask>>2) | (player_mask>>3) ) & number_mask;
    straight = straight & (straight>>4) & (straight>>8) & (straight>>12) & (straight>>16);
    if (straight) {
        return (4UL<<26) | ( 1UL<<((63 - __builtin_clzll(straight))/4) );
    }

    // Straight with aces at bottom.
    if (ace_mask & player_mask) {
        unsigned long long aces_down = ( (player_mask & ~ace_mask)<<4 ) | ( (player_mask & ace_mask)>>12*4 );
        aces_down = ( aces_down | (aces_down>>1) | (aces_down>>2) | (aces_down>>3) ) & number_mask;
        if (aces_down & (aces_down>>4) & (aces_down>>8) & (aces_down>>12) & (aces_down>>16)) {
            return (4UL<<26);
        }
    }

    // Three of a kind.
    if (threes) {
        unsigned long threes_mask = 1UL<<( (63 - __builtin_clzll(threes)) /4);
        unsigned long rest_mask = Holdem::CalculateHighestMask(player_mask & ~(0x000000000000000FULL<<(__builtin_ctzll(threes))), 2);
        return (3UL<<26) | (threes_mask<<13) | rest_mask;
    }

    // Two pair.
    if (__builtin_popcountll(twos) > 1) {
        unsigned long twos_mask = Holdem::CalculateHighestMask(twos, 2);
        unsigned long long twos_filter = twos | (twos<<1) | (twos<<2) | (twos<<3);
        if (__builtin_popcountll(twos) == 3) {
            twos_filter &= ~(0x000000000000000FULL<<__builtin_ctzll(twos));
        }
        unsigned long rest_mask = Holdem::CalculateHighestMask(player_mask & ~twos_filter, 1);
        return (2UL<<26) | (twos_mask<<13) | rest_mask;
    }

    // Pair.
    if (twos) {
        unsigned long twos_mask = 1UL<<(__builtin_ctzll(twos)/4);
        unsigned long rest_mask = Holdem::CalculateHighestMask(player_mask & ~(0x000000000000000FULL<<(__builtin_ctzll(twos))), 3);
        return (1UL<<26) | (twos_mask<<13) | rest_mask;
    }

    // High card.
    return Holdem::CalculateHighestMask(player_mask, 5);
}

