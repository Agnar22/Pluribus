#include "../src/card_deck.h"
#include "tests.h"


TEST(CardDeck, PutBackAll) {
    CardDeck card_deck;
    std::vector<unsigned long long> cards;

    for (int x = 0; x < 52; x++)
        cards.push_back(card_deck.PickTop());

    card_deck.PutBackAll();
     for (int x = 0; x < 52; x++)
        ASSERT_EQ(cards[x], card_deck.PickTop());
}

TEST(CardDeck, PickTop) {
    CardDeck card_deck;
    std::vector<unsigned long long> cards;

    for (int x = 0; x < 5; x++)
        cards.push_back(card_deck.PickTop());

    for (int x = 0; x < 5; x++)
        card_deck.PutBack();

    for (int x = 0; x < 5; x++)
        ASSERT_EQ(cards[x], card_deck.PickTop());
}


TEST(CardDeck, Shuffle) {
    CardDeck card_deck;
    std::vector<unsigned long long> cards;

    for (int x = 0; x < 52; x++)
        cards.push_back(card_deck.PickTop());

    for (int x = 0; x < 20; x++)
        card_deck.PutBack();

    card_deck.Shuffle();
    card_deck.PutBackAll();

    for (int x = 0; x < 32; x++)
        ASSERT_EQ(cards[x], card_deck.PickTop());

    // There is a very slim chance that they are in the same order (1 in 20!).
    bool equal = true;
    for (int x = 0; x < 20; x++)
        if(cards[x + 32] != card_deck.PickTop())
            equal = false;
    ASSERT_FALSE(equal);
}
