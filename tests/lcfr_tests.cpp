#include "optimal_strategies_tests.h"
#include "../src/lcfr.cpp"

struct LCFRTest: public testing::Test {
    LCFRTest() {
        srand(42);
    };
    ~LCFRTest() {
        lcfr::regret.clear();
        lcfr::strategy_profile.clear();
        lcfr::cumulative_strategy.clear();
    };
};

TEST_F(LCFRTest, TwoPlayerKuhnPokerOptimalStrategy) {
    // The optimal strategy for two player Kuhn poker is described here: https://en.wikipedia.org/wiki/Kuhn_poker#Optimal_strategy
    KuhnPoker kuhn_poker(2);
    int timesteps = 200000;
    float error_treshold = 0.03f;

    lcfr::search(timesteps, kuhn_poker);
    auto strategy = lcfr::calculate_cumulative_strategy();

    ASSERT_NO_THROW(test_two_player_kuhn_poker(error_treshold, strategy));
}

TEST_F(LCFRTest, ThreePlayerKuhnPokerOptimalStrategy) {
    // The optimal strategy for three player Kuhn poker is described here: https://poker.cs.ualberta.ca/publications/AAMAS13-3pkuhn.pdf
    // The variables are using the notation from the paper.
    KuhnPoker kuhn_poker(3);
    int timesteps = 10000000;
    float error_treshold = 0.03f;

    lcfr::search(timesteps, kuhn_poker);
    auto strategy = change_notation(lcfr::calculate_cumulative_strategy());

    ASSERT_NO_THROW(test_three_player_kuhn_poker(error_treshold, strategy));
}