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

