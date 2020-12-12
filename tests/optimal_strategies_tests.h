#include <assert.h>
#include "gtest/gtest.h"
#include "../src/game.h"
#include "../src/kuhn_poker.h"
#include "../lib/robin_hood.h"

bool test_two_player_kuhn_poker(float, robin_hood::unordered_map<uint64_t, robin_hood::unordered_map<Move, float>>);
robin_hood::unordered_map<std::string, float> change_notation(robin_hood::unordered_map<uint64_t, robin_hood::unordered_map<Move, float>>);
bool test_three_player_kuhn_poker(float, robin_hood::unordered_map<std::string, float>);
