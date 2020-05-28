#include "card_deck.h"


CardDeck::CardDeck() {
    top = 0;

    card_deck.assign( 52, 0ULL );
    for (int x = 0; x < 52; x++)
        card_deck[x] = 1ULL<<x;
}

unsigned long long CardDeck::PickTop() {
    // Pick the current card on top and "pop".
    return card_deck[top++];
}

void CardDeck::PutBack() {
    // Put the last picked card back on top.
    top--;
}

void CardDeck::PutBackAll() {
    // Put all cards back in the deck in the right order.
    top = 0;
}

void CardDeck::Shuffle() {
    // Shuffle the remaining deck.
    std::random_shuffle( card_deck.begin() + top, card_deck.end() );
}

