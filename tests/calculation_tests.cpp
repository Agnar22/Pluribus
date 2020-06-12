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


