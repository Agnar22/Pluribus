#include "optimal_strategies_tests.h"
#include "../src/cfr.cpp"

struct CFRTest: public testing::Test {
    CFRTest() {
        srand(42);
    };
    ~CFRTest() {
        cfr::regret.clear();
        cfr::strategy_profile.clear();
        cfr::cumulative_strategy.clear();
    };
};

TEST_F(CFRTest, TwoPlayerKuhnPokerOptimalStrategy) {
    // The optimal strategy for two player Kuhn poker is described here: https://en.wikipedia.org/wiki/Kuhn_poker#Optimal_strategy
    KuhnPoker kuhn_poker(2);
    int timesteps = 1000000;
    float error_treshold = 0.03f;

    cfr::search(timesteps, kuhn_poker);
    auto strategy = cfr::calculate_cumulative_strategy();

    ASSERT_NO_THROW(test_two_player_kuhn_poker(error_treshold, strategy));
}

TEST_F(CFRTest, ThreePlayerKuhnPokerOptimalStrategy) {
    // The optimal strategy for three player Kuhn poker is described here: https://poker.cs.ualberta.ca/publications/AAMAS13-3pkuhn.pdf
    // The variables are using the notation from the paper.
    KuhnPoker kuhn_poker(3);
    int timesteps = 1000000;
    float error_treshold = 0.03f;

    cfr::search(3000000, kuhn_poker);
    auto strategy = change_notation(cfr::calculate_cumulative_strategy());

    ASSERT_NO_THROW(test_three_player_kuhn_poker(error_treshold, strategy));
}
