#include "tests.h"
#include "../src/game.h"
#include "../src/kuhn_poker.h"
#include "../src/mccfr.cpp"

struct MCCFRTest: public testing::Test {
    MCCFRTest() {}
    ~MCCFRTest() {
        mccfr::regret.clear();
        mccfr::strategy.clear();
    };
};

TEST_F(MCCFRTest, TwoPlayerKuhnPokerOptimalStrategy) {
    // The optimal strategy for two player Kuhn poker is described here: https://en.wikipedia.org/wiki/Kuhn_poker#Optimal_strategy
    float error_treshold = 0.02f;
    KuhnPoker kuhn_poker(2);

    mccfr::mccfr_p(100000, 20, 20000, 1000, 20, kuhn_poker);
    auto strategy = mccfr::calculate_probabilities();
    float strategy_alpha = strategy["0|Q|"]["r"];

    // QK
    ASSERT_LT(strategy_alpha - 1.0f/3.0f,  0.0f);
    ASSERT_LT(std::abs(strategy["1|K|r"]["f"] - 2.0f/3.0f), error_treshold);
    ASSERT_LT(std::abs(strategy["1|K|c"]["c"] - 1.0f), error_treshold);

    // QA
    ASSERT_LT(std::abs(strategy["1|A|r"]["c"] - 1.0f), error_treshold);
    ASSERT_LT(std::abs(strategy["1|A|c"]["r"] - 1.0f), error_treshold);
    ASSERT_LT(std::abs(strategy["0|Q|cr"]["f"] - 1.0f), error_treshold);

    // KQ
    ASSERT_LT(std::abs(strategy["0|K|"]["c"] - 1.0f), error_treshold);
    ASSERT_LT(std::abs(strategy["1|Q|c"]["r"] - 1.0f/3.0f), error_treshold);
    ASSERT_LT(std::abs(strategy["0|K|cr"]["f"] - (2.0f/3.0f - strategy_alpha)), error_treshold);

    // KA
    // All situations covered above.

    // AQ
    ASSERT_LT(std::abs(strategy["0|A|"]["r"] - 3.0f*strategy_alpha), error_treshold);
    ASSERT_LT(std::abs(strategy["1|Q|r"]["f"] - 1.0f), error_treshold);
    ASSERT_LT(std::abs(strategy["0|A|cr"]["c"] - 1.0f), error_treshold);

    // AK
    // All situations covered above.
}
