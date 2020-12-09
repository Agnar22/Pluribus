#include "gtest/gtest.h"
#include "../src/game.h"


TEST(History, AddMove) {
    History hist;
    Move move = Move::C;
    
    hist += move;

    ASSERT_EQ(hist.history, static_cast<uint32_t>(Move::C));
    ASSERT_EQ(hist.get_length(), 1);
}


TEST(History, AddTwoMoves) {
    History hist;
    Move first_move = Move::C;
    Move second_move = Move::R;
    
    hist += first_move;
    hist += second_move;

    ASSERT_EQ(hist.get_length(), 2);
    ASSERT_EQ(hist[0], Move::R);
    ASSERT_EQ(hist[1], Move::C);
}

TEST(History, Defaultconstructor) {
    History hist;

    ASSERT_EQ(hist.get_length(), 0);
    ASSERT_EQ(hist.history, 0);
}


TEST(History, lStringconstructor) {
    std::string pos = "CCR";
    History hist(pos);

    ASSERT_EQ(hist.get_length(), 3);
    ASSERT_EQ(hist[0], Move::R);
    ASSERT_EQ(hist[1], Move::C);
    ASSERT_EQ(hist[2], Move::C);
}

TEST(History, rStringconstructor) {
    History hist("RF");

    ASSERT_EQ(hist.get_length(), 2);
    ASSERT_EQ(hist[0], Move::F);
    ASSERT_EQ(hist[1], Move::R);
}

TEST(History, Clear) {
    History hist("CCC");
    
    hist.clear();

    ASSERT_EQ(hist.get_length(), 0);
    ASSERT_EQ(hist.history, 0);
}

TEST(History, Decrement) {
    History hist("RF");
    
    hist--;

    ASSERT_EQ(hist.get_length(), 1);
    ASSERT_EQ(hist[0], Move::R);
}

TEST(History, Assertions) {
    // The length of the string is 17, as the maximum length allowed is 16.
    std::string moves = "CCCCCCCCCCCCCCCCC";
    ASSERT_DEATH(new History(moves), "Not overflow.");
    ASSERT_DEATH(new History("CCCCCCCCCCCCCCCCC"), "Not overflow.");

    History hist;
    ASSERT_DEATH(hist--, "Not underflow.");
    ASSERT_DEATH(hist[-1], "Position is positive.");
    ASSERT_DEATH(hist[0], "Valid position.");

    for (int x=0; x<16; ++x)
        hist+=Move::C;
    ASSERT_DEATH(hist+=Move::C, "Not overflow.");
}
