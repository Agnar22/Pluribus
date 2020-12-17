#include "optimal_strategies_tests.h"

bool test_two_player_kuhn_poker(float error_treshold, robin_hood::unordered_map<uint64_t, robin_hood::unordered_map<Move, float>> strategy) {
    // The optimal strategy for two player Kuhn poker is described here: https://en.wikipedia.org/wiki/Kuhn_poker#Optimal_strategy
    float strategy_alpha = strategy[to_infoset("", Cards::Q)][Move::R];

    assert(to_infoset("", Cards::K) == 1);
    assert(to_infoset("R", Cards::K) == 25);
    assert(infoset_to_string(to_infoset("R", Cards::K)) == "RK");

    // QK
    assert(strategy_alpha - 1.0f/3.0f <  0.0f);
    assert(std::abs(strategy[to_infoset("R", Cards::K)][Move::F] - 2.0f/3.0f) < error_treshold);
    assert(std::abs(strategy[to_infoset("C", Cards::K)][Move::C] - 1.0f) < error_treshold);

    // QA
    assert(std::abs(strategy[to_infoset("R", Cards::A)][Move::C] - 1.0f) < error_treshold);
    assert(std::abs(strategy[to_infoset("C", Cards::A)][Move::R] - 1.0f) < error_treshold);
    assert(std::abs(strategy[to_infoset("CR", Cards::Q)][Move::F] - 1.0f) < error_treshold);

    // KQ
    assert(std::abs(strategy[to_infoset("", Cards::K)][Move::C] - 1.0f) < error_treshold);
    assert(std::abs(strategy[to_infoset("C", Cards::Q)][Move::R] - 1.0f/3.0f) < error_treshold);
    assert(std::abs(strategy[to_infoset("CR", Cards::K)][Move::F] - (2.0f/3.0f - strategy_alpha)) < error_treshold);

    // KA
    // All situations covered above.

    // AQ
    assert(std::abs(strategy[to_infoset("", Cards::A)][Move::R] - 3.0f*strategy_alpha) < error_treshold);
    assert(std::abs(strategy[to_infoset("R", Cards::Q)][Move::F] - 1.0f) < error_treshold);
    assert(std::abs(strategy[to_infoset("CR", Cards::A)][Move::C] - 1.0f) < error_treshold);

    // AK
    // All situations covered above.
    return true;
}

robin_hood::unordered_map<std::string, float> change_notation(robin_hood::unordered_map<uint64_t, robin_hood::unordered_map<Move, float>> strategy) {
    std::vector<Cards> cards = {Cards::J, Cards::Q, Cards::K, Cards::A};
    std::vector<std::vector<std::string>> player_paths = {
        std::vector<std::string>{"", "CCR", "CRF", "CRR"},
        std::vector<std::string>{"C", "R", "CCRF", "CCRC"},
        std::vector<std::string>{"CC", "CR", "RF", "RC"}
    };
    robin_hood::unordered_map<std::string, float> infosets;
    for (int player=0; player<3; ++player)
        for (int x=0; x<player_paths[player].size(); ++x)
            for (int y=0; y<cards.size(); ++y) {
                Move action = player_paths[player][x].find("R") == std::string::npos ? Move::R : Move::C;
                infosets[std::string(1, 'a' + player) + std::to_string(y+1) + std::to_string(x+1)] = strategy[to_infoset(player_paths[player][x], cards[y])][action];
            }
    return infosets;
}


bool test_three_player_kuhn_poker(float error_treshold, robin_hood::unordered_map<std::string, float> strategy) {
    // The optimal strategy for three player Kuhn poker is described here: https://poker.cs.ualberta.ca/publications/AAMAS13-3pkuhn.pdf
    // The variables are using the notation from the paper.
    // Nash equilibrium for a (player 0).
    assert(strategy["a11"] < error_treshold);
    assert(strategy["a21"] < error_treshold);
    assert(strategy["a22"] < error_treshold);
    assert(strategy["a23"] < error_treshold);
    assert(strategy["a31"] < error_treshold);
    assert(strategy["a32"] < error_treshold);
    assert(std::abs(strategy["a33"] - 0.5f) < error_treshold);
    assert(strategy["a34"] < error_treshold);
    assert(strategy["a41"] < error_treshold);

    // Nash equilibrium for b (player 1).
    float strategy_treshold = 0.02f;
    float beta = std::max(strategy["b11"], strategy["b21"]);
    if (strategy["c11"] < strategy_treshold) {
        assert(strategy["b11"] < strategy["b21"] + error_treshold);
        assert(strategy["b21"] < 0.25f + error_treshold);
    } else if (strategy["c11"] > 0.5f - strategy_treshold) {
        assert(strategy["b11"] < 0.25f + error_treshold);
        assert(strategy["b21"] < std::min(strategy["b11"], 0.5f - 2.0f*strategy["b11"]) + error_treshold);
    } else {
        assert(strategy["b11"] < 0.25f + error_treshold);
        assert(std::abs(strategy["b21"] - strategy["b11"]) < error_treshold);
    }
    assert(strategy["b22"] < error_treshold);
    assert(strategy["b23"] < std::max(0.0f, (strategy["b11"]-strategy["b21"]) / (2.0f*(1.0f-strategy["b21"]))) + error_treshold);
    assert(strategy["b31"] < error_treshold);
    assert(strategy["b32"] < 0.5 + 0.75f * (strategy["b11"] + strategy["b21"]) + beta/4.0f + error_treshold);
    assert(std::abs(
            strategy["b33"] - (
                0.5 + 0.5f * (strategy["b11"] + strategy["b21"]) + beta/2.0f - strategy["b23"] * (1 - strategy["b21"])
            )
        ) < error_treshold
    );
    assert(strategy["b34"] < error_treshold);
    assert(strategy["b41"] - 2*(strategy["b11"] + strategy["b21"]) < error_treshold);

    // Nash equilibrium for c (player 2).
    assert(strategy["c11"] < std::min(0.5f, (2.0f-strategy["b11"])/(3.0f + 2.0f * (strategy["b11"] + strategy["b21"]))) + error_treshold);
    assert(std::abs(strategy["c21"] - (0.5f - strategy["c11"])) < error_treshold);
    assert(strategy["c22"] < error_treshold);
    assert(strategy["c23"] < error_treshold);
    assert(strategy["c31"] < error_treshold);
    assert(strategy["c32"] < error_treshold);
    float lower = 0.5f - strategy["b32"];
    assert(strategy["c33"] > lower - error_treshold);
    float upper = 0.5f - strategy["b32"] + 0.75f * (strategy["b11"] + strategy["b21"]) + beta/4.0f;
    assert(strategy["c33"] < upper + error_treshold);
    assert(std::abs(strategy["c41"] - 1.0f) < error_treshold);

    return true;
}
