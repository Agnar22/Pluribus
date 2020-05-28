
#ifndef CARD_DECK_H
#define CARD_DECK_H

#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>

enum class Card {
    c, d, h, s,
    two=0, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace
};


class CardDeck {
    public:
        CardDeck();
        unsigned long long PickTop();
        void PutBack();
        void PutBackAll();
        void Shuffle();
    private:
        std::vector< unsigned long long > card_deck;
        int top;
};

#endif
