#include "gtest/gtest.h"
#include "../src/game.h"
#include "../src/kuhn_poker.h"
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
    float error_treshold = 0.03f;
    KuhnPoker kuhn_poker(2);

    mccfr::mccfr_p(100000, 1, 20000, 1000, 20, kuhn_poker);
    auto strategy = mccfr::calculate_probabilities();
    float strategy_alpha = strategy["0|Q|"]["r"];

    ASSERT_EQ(to_infoset("", Cards::K), 1);
    ASSERT_EQ(to_infoset("R", Cards::K), 25);
    ASSERT_EQ(infoset_to_string(to_infoset("R", Cards::K)), "RK");

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

TEST_F(MCCFRTest, ThreePlayerKuhnPokerOptimalStrategy) {
    // The optimal strategy for three player Kuhn poker is described here: https://poker.cs.ualberta.ca/publications/AAMAS13-3pkuhn.pdf
    // The variables are using the notation from the paper.
    float error_treshold = 0.05f;
    KuhnPoker kuhn_poker(3);


    mccfr::mccfr_p(1000000, 500, 2000000, 100000, 5000, kuhn_poker);
    auto strategy = mccfr::calculate_probabilities();

    std::vector<char> cards = {'J', 'Q', 'K', 'A'};
    std::vector<std::vector<std::string>> player_paths = {
        std::vector<std::string>{"", "ccr", "crf", "crc"},
        std::vector<std::string>{"c", "r", "ccrf", "ccrc"},
        std::vector<std::string>{"cc", "cr", "rf", "rc"}
    };
    std::unordered_map<std::string, float> infosets;

    for (int player=0; player<3; ++player)
        for (int x=0; x<player_paths[player].size(); ++x)
            for (int y=0; y<cards.size(); ++y) {
                std::string action = player_paths[player][x].find("r") == std::string::npos ? "r":"c";
                std::string infoset_name = std::to_string(player) + "|" + std::string(1, cards[y]) + "|" + player_paths[player][x];
                infosets[std::string(1, 'a' + player) + std::to_string(y+1) + std::to_string(x+1)] = strategy[infoset_name][action];
            }


    // Nash equilibrium for a (player 0).
    ASSERT_LT(infosets["a11"], error_treshold);
    ASSERT_LT(infosets["a21"], error_treshold);
    ASSERT_LT(infosets["a22"], error_treshold);
    ASSERT_LT(infosets["a23"], error_treshold);
    ASSERT_LT(infosets["a31"], error_treshold);
    ASSERT_LT(infosets["a32"], error_treshold);
    ASSERT_LT(std::abs(infosets["a33"] - 0.5f), error_treshold);
    ASSERT_LT(infosets["a34"], error_treshold);
    ASSERT_LT(infosets["a41"], error_treshold);

    // Nash equilibrium for b (player 1).
    float beta = std::max(infosets["b11"], infosets["b21"]);
    if (infosets["c11"] <= error_treshold) {
        ASSERT_LT(infosets["b11"], infosets["b21"] + error_treshold);
        ASSERT_LT(infosets["b21"], 0.25f + error_treshold);
    } else if (infosets["c11"] >= 0.5f - error_treshold){
        ASSERT_LT(std::abs(infosets["b11"] - infosets["b21"]), error_treshold);
        ASSERT_LT(infosets["b21"], 0.25 + error_treshold);
    } else {
        ASSERT_LT(infosets["b11"], 0.25f + error_treshold);
        ASSERT_LT(infosets["b21"], std::min(infosets["b11"], 0.5f - 2.0f*infosets["b11"]) + error_treshold);
    }
    ASSERT_LT(infosets["b22"], error_treshold);
    ASSERT_LT(infosets["b23"], std::max(0.0f, (infosets["b11"]-infosets["b21"]) / (2.0f*(1.0f-infosets["b21"]))) + error_treshold);
    ASSERT_LT(infosets["b31"], error_treshold);
    ASSERT_LT(infosets["b32"], 0.5 + 0.75f * (infosets["b11"] + infosets["b21"]) + beta/4.0f + error_treshold);
    ASSERT_LT(std::abs(
            infosets["b33"] - (
                0.5 + 0.5f * (infosets["b11"] + infosets["b21"]) + beta/2.0f - infosets["b23"] * (1 - infosets["b21"])
            )
        ),
        error_treshold
    );
    ASSERT_LT(infosets["b34"], error_treshold);
    ASSERT_LT(infosets["b41"] - 2*(infosets["b11"] + infosets["b21"]), error_treshold);

    // Nash equilibrium for c (player 2).
    ASSERT_LT(infosets["c11"], std::min(0.5f, (2.0f-infosets["b11"])/(3.0f + 2.0f * (infosets["b11"] + infosets["b21"]))) + error_treshold);
    ASSERT_LT(std::abs(infosets["c21"] - (0.5f - infosets["c11"])), error_treshold);
    ASSERT_LT(infosets["c22"], error_treshold);
    ASSERT_LT(infosets["c23"], error_treshold);
    ASSERT_LT(infosets["c31"], error_treshold);
    ASSERT_LT(infosets["c32"], error_treshold);
    float lower = 0.5f - infosets["b32"];
    ASSERT_GT(infosets["c33"], lower - error_treshold);
    float upper = 0.5f - infosets["b32"] + 0.75f * (infosets["b11"] + infosets["b21"]) + beta/4.0f;
    ASSERT_LT(infosets["c33"], upper + error_treshold);
    ASSERT_LT(std::abs(infosets["c41"] - 1.0f), error_treshold);
}
