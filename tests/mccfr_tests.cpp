#include "optimal_strategies_tests.h"
#include "../src/mccfr.cpp"

struct MCCFRTest: public testing::Test {
    MCCFRTest() {
        srand(42);
    };
    ~MCCFRTest() {
        mccfr::regret.clear();
        mccfr::strategy.clear();
    };
};

TEST_F(MCCFRTest, TwoPlayerKuhnPokerOptimalStrategy) {
    // The optimal strategy for two player Kuhn poker is described here: https://en.wikipedia.org/wiki/Kuhn_poker#Optimal_strategy
    KuhnPoker kuhn_poker(2);
    float error_treshold = 0.03f;

    mccfr::mccfr_p(1000000, 1, 20000, 1000, 20, kuhn_poker);
    auto strategy = mccfr::calculate_probabilities();

    ASSERT_NO_THROW(test_two_player_kuhn_poker(error_treshold, strategy));
}

TEST_F(MCCFRTest, ThreePlayerKuhnPokerOptimalStrategy) {
    // The optimal strategy for three player Kuhn poker is described here: https://poker.cs.ualberta.ca/publications/AAMAS13-3pkuhn.pdf
    // The variables are using the notation from the paper.
    KuhnPoker kuhn_poker(3);
    float error_treshold = 0.03f;

    mccfr::mccfr_p(3000000, 1000, 20000000, 20000000, 10000000, kuhn_poker);
    auto strategy = change_notation(mccfr::calculate_probabilities());

    ASSERT_NO_THROW(test_three_player_kuhn_poker(error_treshold, strategy));
}
