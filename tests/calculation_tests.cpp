#include "../src/calculations.h"
#include "tests.h"


TEST(Calculations, LosslessCompression) {
    ull sc = compress_hand_lossless(hand_test::hand_from_string("c2"));
    ASSERT_EQ(sc, 1ULL<<0);

    ull sc_mv = compress_hand_lossless(hand_test::hand_from_string("c2 c5 c7 c9"));
    ASSERT_EQ(sc_mv, (1ULL<<0) | (1ULL<<(3 * 4)) | (1ULL<<(5 * 4)) | (1ULL<<(7 * 4)) );

    ull mc_mv = compress_hand_lossless(hand_test::hand_from_string("c2 s5 s7 d9 hj"));
    ASSERT_EQ(mc_mv, (1ULL<<3) | (1ULL<<(2 + 3 * 4)) | (1ULL<<(2 + 5 * 4)) | (1ULL<<(1 + 7 * 4)) | (1ULL<<(0 + 9 * 4)));
}


TEST(Calculations, SuitPermutations) {
    ull single = hand_test::hand_from_string("c2");
    ASSERT_EQ(4, calculations::num_suit_permutations(single));

    ull pair = hand_test::hand_from_string("c2 s2");
    ASSERT_EQ(6, calculations::num_suit_permutations(pair));

    ull two_pair_no_overlap = hand_test::hand_from_string("c2 s2 h3 d3");
    ASSERT_EQ(6, calculations::num_suit_permutations(two_pair_no_overlap));

    ull two_pair_partial_overlap = hand_test::hand_from_string("c2 s2 s3 d3");
    ASSERT_EQ(24, calculations::num_suit_permutations(two_pair_partial_overlap));

    ull two_pair_complete_overlap = hand_test::hand_from_string("c2 s2 s3 c3");
    ASSERT_EQ(6, calculations::num_suit_permutations(two_pair_complete_overlap));

    ull three = hand_test::hand_from_string("c2 s2 d2");
    ASSERT_EQ(4, calculations::num_suit_permutations(three));

    ull three_and_single_no_overlap = hand_test::hand_from_string("c2 s2 d2 h3");
    ASSERT_EQ(4, calculations::num_suit_permutations(three_and_single_no_overlap));

    ull three_and_single_overlap = hand_test::hand_from_string("c2 s2 d2 d3");
    ASSERT_EQ(12, calculations::num_suit_permutations(three_and_single_overlap));

    ull house_partial_overlap = hand_test::hand_from_string("c2 s2 d2 h3 d3");
    ASSERT_EQ(12, calculations::num_suit_permutations(house_partial_overlap));

    ull house_complete_overlap = hand_test::hand_from_string("c2 s2 d2 s3 d3");
    ASSERT_EQ(12, calculations::num_suit_permutations(house_complete_overlap));

    ull four_of_a_kind = hand_test::hand_from_string("c2 s2 d2 h2");
    ASSERT_EQ(1, calculations::num_suit_permutations(four_of_a_kind));
}

TEST(Calculations, TotalSuitPermutations) {
    calculations::load_suit_permutations();

    std::vector<int> compression_permutations = { 0, 0, 0, 0, 0, 0, 0 };
    std::vector<int> ans = { 52, 1326, 22100, 270725, 2598960, 20358520, 133784560 };

    for (auto hand : calculations::suit_permutations) {
        compression_permutations[__builtin_popcountll(hand.first) - 1] += calculations::num_suit_permutations(hand.first);
    }

    for (int pos = 0; pos < 7; ++pos)
        ASSERT_EQ(ans[pos], compression_permutations[pos]);
}

