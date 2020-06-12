#include "../src/holdem.h"
#include "../src/card_deck.h"
#include "tests.h"

TEST(CalculateHandStrength, RoyalFlush) {
    ul c = hand_test::find_and_check_strength(Wincondition::royal_flush, "c2 c10 cj cq ck ca sa", Card::ten, Card::ten, false);
    ul s = hand_test::find_and_check_strength(Wincondition::royal_flush, "s2 s10 sj sq sk sa ca", Card::ten, Card::ten, false);
    ul d = hand_test::find_and_check_strength(Wincondition::royal_flush, "d2 d10 dj dq dk da ha", Card::ten, Card::ten, false);
    ul h = hand_test::find_and_check_strength(Wincondition::royal_flush, "h2 h10 hj hq hk ha ca", Card::ten, Card::ten, false);

    ASSERT_EQ(c, s);
    ASSERT_EQ(c, d);
    ASSERT_EQ(c, h);
}


TEST(CalculateHandStrength, StraightFlush) {
    ul c_a2345 = hand_test::find_and_check_strength(Wincondition::straight_flush, "c2 c3 c4 c5 hj dq ca", Card::ace, Card::ace, true);
    ul s_a2345 = hand_test::find_and_check_strength(Wincondition::straight_flush, "s2 s3 s4 s5 sj cq sa", Card::ace, Card::ace, true);
    ul s_a2345_6 = hand_test::find_and_check_strength(Wincondition::straight_flush, "s2 s3 s4 s5 c6 sq sa", Card::ace, Card::ace, true);
    ul c_23456 = hand_test::find_and_check_strength(Wincondition::straight_flush, "c2 c3 c4 c5 c6 hq ca", Card::two, Card::two, false);
    ul c_34567_2 = hand_test::find_and_check_strength(Wincondition::straight_flush, "c2 c3 c4 c5 c6 c7 sa", Card::three, Card::three, false);
    ul h_34567 = hand_test::find_and_check_strength(Wincondition::straight_flush, "s2 h3 h4 h5 h6 h7 ca", Card::three, Card::three, false);
    ul c_910jqk = hand_test::find_and_check_strength(Wincondition::straight_flush, "c2 s9 c9 c10 cj cq ck", Card::nine, Card::nine, false);
    ul c_910jqk_a = hand_test::find_and_check_strength(Wincondition::straight_flush, "c2 c9 c10 cj cq ck sa", Card::nine, Card::nine, false);

    ASSERT_EQ(c_a2345, s_a2345);
    ASSERT_EQ(c_a2345, s_a2345_6);
    ASSERT_LT(c_a2345, c_23456);
    ASSERT_LT(c_23456, c_910jqk);
    ASSERT_EQ(h_34567, c_34567_2);
    ASSERT_EQ(c_910jqk, c_910jqk_a);
}


TEST(CalculateHandStrength, FourOfAKind) {
    ul k_3_32 = hand_test::find_and_check_strength(Wincondition::four_of_a_kind, "c2 d3 c3 sk dk hk ck", Card::king, Card::three, false);
    ul k_3_33 = hand_test::find_and_check_strength(Wincondition::four_of_a_kind, "h3 d3 c3 sk dk hk ck", Card::king, Card::three, false);
    ul k_4_43 = hand_test::find_and_check_strength(Wincondition::four_of_a_kind, "h3 d4 c4 sk dk hk ck", Card::king, Card::four, false);
    ul k_4_44 = hand_test::find_and_check_strength(Wincondition::four_of_a_kind, "h4 d4 c4 sk dk hk ck", Card::king, Card::four, false);
    ul q_a_ak = hand_test::find_and_check_strength(Wincondition::four_of_a_kind, "hq dq cq sq dk sa ca", Card::queen, Card::ace, false);
    ul q_a_j9 = hand_test::find_and_check_strength(Wincondition::four_of_a_kind, "s9 dj hq dq cq sq ca", Card::queen, Card::ace, false);
    ul q_4_44 = hand_test::find_and_check_strength(Wincondition::four_of_a_kind, "h4 d4 c4 sq dq hq cq", Card::queen, Card::four, false);

    ASSERT_EQ(k_3_32, k_3_33);
    ASSERT_EQ(k_4_43, k_4_44);
    ASSERT_LT(k_3_32, k_4_43);
    ASSERT_EQ(q_a_ak, q_a_j9);
    ASSERT_LT(q_a_ak, k_3_32);
    ASSERT_LT(q_4_44, q_a_ak);
}

TEST(CalculateHandStrength, FullHouse) {
    ul k_q_22 = hand_test::find_and_check_strength(Wincondition::full_house, "c2 d2 cq sq dk hk ck", Card::king, Card::queen, false);
    ul k_q_aj = hand_test::find_and_check_strength(Wincondition::full_house, "cj dq cq sk dk hk ca", Card::king, Card::queen, false);
    ul k_q_jj = hand_test::find_and_check_strength(Wincondition::full_house, "cj dj cq sq dk hk ck", Card::king, Card::queen, false);
    ul k_j_99 = hand_test::find_and_check_strength(Wincondition::full_house, "c9 d9 cj sj dk hk ck", Card::king, Card::jack, false);
    ul k_j_j9 = hand_test::find_and_check_strength(Wincondition::full_house, "c9 dj cj sj dk hk ck", Card::king, Card::jack, false);
    ul q_k_22 = hand_test::find_and_check_strength(Wincondition::full_house, "c2 d2 hq sq dq hk ck", Card::queen, Card::king, false);

    ASSERT_EQ(k_q_22, k_q_aj);
    ASSERT_EQ(k_q_22, k_q_jj);
    ASSERT_EQ(k_j_99, k_j_j9);
    ASSERT_LT(k_j_j9, k_q_22);
    ASSERT_LT(q_k_22, k_j_j9);
}


TEST(CalculateHandStrength, Flush) {
    ul s_aqj95 = hand_test::find_and_check_strength(Wincondition::flush, "s2 s5 s5 s9 sj sq sa", Card::ace, Card::five, false);
    ul c_kqj96 = hand_test::find_and_check_strength(Wincondition::flush, "c2 c5 c6 c9 cj cq ck", Card::king, Card::six, false);
    ul s_kqj96 = hand_test::find_and_check_strength(Wincondition::flush, "s2 c5 s6 s9 sj sq sk", Card::king, Card::six, false);
    ul s_kqj95 = hand_test::find_and_check_strength(Wincondition::flush, "s2 s5 s5 s9 sj sq sk", Card::king, Card::five, false);
    ul d_87653 = hand_test::find_and_check_strength(Wincondition::flush, "s2 d3 d5 d6 d7 d8 sk", Card::king, Card::four, false);
    ul d_87653_q = hand_test::find_and_check_strength(Wincondition::flush, "s2 d3 d5 d6 d7 d8 sq", Card::queen, Card::four, false);

    ASSERT_LT(c_kqj96, s_aqj95);
    ASSERT_EQ(c_kqj96, s_kqj96);
    ASSERT_LT(s_kqj95, c_kqj96);
    ASSERT_EQ(d_87653, d_87653_q);
    ASSERT_LT(d_87653, s_kqj95);
}

TEST(CalculateHandStrength, Straight) {
    ul nine = hand_test::find_and_check_strength(Wincondition::straight, "s2 h5 d9 s10 hj dq sk", Card::king, Card::nine, false);
    ul eight = hand_test::find_and_check_strength(Wincondition::straight, "s2 h5 d8 d9 s10 hj dq", Card::queen, Card::eight, false);
    ul eight_longer = hand_test::find_and_check_strength(Wincondition::straight, "s6 h7 d8 d9 s10 hj dq", Card::queen, Card::eight, false);
    ul ace = hand_test::find_and_check_strength(Wincondition::straight, "s2 h3 d4 d5 s10 hj da", Card::ace, Card::ace, true);
    ul ten = hand_test::find_and_check_strength(Wincondition::straight, "s2 h3 d10 dj sq hk da", Card::ace, Card::ten, false);

    ASSERT_LT(eight, nine);
    ASSERT_EQ(eight, eight_longer);
    ASSERT_LT(ace, eight);
    ASSERT_LT(nine, ten);
}

TEST(CalculateHandStrength, ThreeOfAKind) {
    ul k_aj = hand_test::find_and_check_strength(Wincondition::three_of_a_kind, "s2 h4 dj sk hk dk sa", Card::king, Card::king, false);
    ul k_85 = hand_test::find_and_check_strength(Wincondition::three_of_a_kind, "s2 h4 d5 s8 hk dk sk", Card::king, Card::king, false);
    ul k_76 = hand_test::find_and_check_strength(Wincondition::three_of_a_kind, "s2 h4 d6 s7 hk dk sk", Card::king, Card::king, false);
    ul k_75 = hand_test::find_and_check_strength(Wincondition::three_of_a_kind, "s2 h4 d5 s7 hk dk sk", Card::king, Card::king, false);
    ul k_75_3 = hand_test::find_and_check_strength(Wincondition::three_of_a_kind, "s3 c4 h5 s7 hk dk sk", Card::king, Card::king, false);
    ul q_ak = hand_test::find_and_check_strength(Wincondition::three_of_a_kind, "s2 c7 cq sq hq dk sa", Card::queen, Card::queen, false);
    ul q_75 = hand_test::find_and_check_strength(Wincondition::three_of_a_kind, "s2 c4 h5 s7 hq dq sq", Card::queen, Card::queen, false);

    ASSERT_LT(k_85, k_aj);
    ASSERT_LT(k_76, k_85);
    ASSERT_LT(k_75, k_76);
    ASSERT_EQ(k_75, k_75_3);
    ASSERT_LT(q_ak, k_75);
    ASSERT_LT(q_75, q_ak);
}

TEST(CalculateHandStrength, TwoPair) {
    ul k_7_a55 = hand_test::find_and_check_strength(Wincondition::two_pair, "h5 d5 d7 c7 hk sk da", Card::king, Card::seven, false);
    ul k_7_8 = hand_test::find_and_check_strength(Wincondition::two_pair, "s2 h4 d7 s7 h8 dk sk", Card::king, Card::seven, false);
    ul k_7_844 = hand_test::find_and_check_strength(Wincondition::two_pair, "s4 h4 d7 s7 h8 dk sk", Card::king, Card::seven, false);
    ul k_7_5 = hand_test::find_and_check_strength(Wincondition::two_pair, "s2 h4 d5 s7 h7 dk sk", Card::king, Card::seven, false);
    ul k_7_55 = hand_test::find_and_check_strength(Wincondition::two_pair, "s2 h5 d5 d7 c7 hk sk", Card::king, Card::seven, false);
    ul q_j_a = hand_test::find_and_check_strength(Wincondition::two_pair, "s2 h4 cj sj hq dq sa", Card::queen, Card::jack, false);
    ul q_j_k = hand_test::find_and_check_strength(Wincondition::two_pair, "s2 h4 cj sj hq dq sk", Card::queen, Card::jack, false);

    ASSERT_LT(k_7_8, k_7_a55);
    ASSERT_EQ(k_7_8, k_7_844);
    ASSERT_LT(k_7_5, k_7_8);
    ASSERT_EQ(k_7_5, k_7_55);
    ASSERT_LT(q_j_a, k_7_5);
    ASSERT_LT(q_j_k, q_j_a);
}

TEST(CalculateHandStrength, Pair) {
    ul k_876 = hand_test::find_and_check_strength(Wincondition::pair, "s2 h4 d6 s7 h8 dk sk", Card::king, Card::king, false);
    ul k_865 = hand_test::find_and_check_strength(Wincondition::pair, "s2 h4 h5 d6 h8 dk sk", Card::king, Card::king, false);
    ul j_akq = hand_test::find_and_check_strength(Wincondition::pair, "s2 h4 hj dj hq dk sa", Card::jack, Card::jack, false);
    ul j_ak9 = hand_test::find_and_check_strength(Wincondition::pair, "s2 h4 c9 hj dj dk sa", Card::jack, Card::jack, false);
    ul j_ak9_7 = hand_test::find_and_check_strength(Wincondition::pair, "s2 h7 c9 hj dj dk sa", Card::jack, Card::jack, false);

    ASSERT_LT(k_865, k_876);
    ASSERT_LT(j_akq, k_865);
    ASSERT_LT(j_ak9, j_akq);
    ASSERT_EQ(j_ak9, j_ak9_7);
}


TEST(CalculateHandStrength, HighCard) {
    ul a8765 = hand_test::find_and_check_strength(Wincondition::high_card, "s2 h3 d5 s6 h7 d8 sa", Card::ace, Card::five, false);
    ul k9875 = hand_test::find_and_check_strength(Wincondition::high_card, "s2 h3 d5 s7 h8 d9 sk", Card::king, Card::five, false);
    ul k9865 = hand_test::find_and_check_strength(Wincondition::high_card, "s2 h3 d5 s6 h8 d9 sk", Card::king, Card::five, false);
    ul k9865_4 = hand_test::find_and_check_strength(Wincondition::high_card, "s2 h4 d5 s6 h8 d9 sk", Card::king, Card::five, false);

    ASSERT_LT(k9875, a8765);
    ASSERT_LT(k9865, k9875);
    ASSERT_EQ(k9865, k9865_4);
}


TEST(CalculateHandStrength, Ordering) {
    // TODO: Migth test that hand_strength matches a specific hard coded number.
    ul royal_flush = hand_test::find_and_check_strength(Wincondition::royal_flush, "c2 d3 s10 sj sq sk sa", Card::ace, Card::ten, false);
    ul straight_flush_high = hand_test::find_and_check_strength(Wincondition::straight_flush, "c2 d3 s9 s10 sj sq sk", Card::nine, Card::nine, false);
    ul straight_flush_low = hand_test::find_and_check_strength(Wincondition::straight_flush, "c2 c3 c4 c5 sj sq ca", Card::ace, Card::ace, true);
    ul four_of_a_kind_high = hand_test::find_and_check_strength(Wincondition::four_of_a_kind, "c2 c3 ck sa ha da ca", Card::ace, Card::king, false);
    ul four_of_a_kind_low = hand_test::find_and_check_strength(Wincondition::four_of_a_kind, "c2 d2 h2 s2 h3 d3 c3", Card::two, Card::three, false);
    ul full_house_high = hand_test::find_and_check_strength(Wincondition::full_house, "cq dk hk sk ha da ca", Card::ace, Card::king, false);
    ul full_house_low = hand_test::find_and_check_strength(Wincondition::full_house, "c2 d2 h2 s3 h3 d4 c5", Card::two, Card::three, false);
    ul flush_high = hand_test::find_and_check_strength(Wincondition::flush, "c7 c8 c9 cj cq ck ca", Card::ace, Card::nine, false);
    ul flush_low = hand_test::find_and_check_strength(Wincondition::flush, "c2 c4 c5 c6 c7 s9 sa", Card::seven, Card::two, false);
    ul straight_high = hand_test::find_and_check_strength(Wincondition::straight, "c8 d9 c10 cj cq sk sa", Card::ace, Card::ten, false);
    ul straight_low = hand_test::find_and_check_strength(Wincondition::straight, "c2 d3 c4 c5 c7 s8 sa", Card::ace, Card::ace, true);
    ul three_of_a_kind_high = hand_test::find_and_check_strength(Wincondition::three_of_a_kind, "c9 dj cq ck ca da sa", Card::ace, Card::ace, false);
    ul three_of_a_kind_low = hand_test::find_and_check_strength(Wincondition::three_of_a_kind, "c2 d2 s2 c3 c4 d6 s7", Card::two, Card::two, false);
    ul two_pair_high = hand_test::find_and_check_strength(Wincondition::two_pair, "cj dq sq dk ck da sa", Card::ace, Card::king, false);
    ul two_pair_low = hand_test::find_and_check_strength(Wincondition::two_pair, "c2 d2 s3 d3 c4 d5 s7", Card::three, Card::two, false);
    ul pair_high = hand_test::find_and_check_strength(Wincondition::pair, "c8 d9 sj dq ck da sa", Card::ace, Card::ace, false);
    ul pair_low = hand_test::find_and_check_strength(Wincondition::pair, "c2 d2 s3 d4 c5 d7 s8", Card::two, Card::two, false);
    ul high_card_high = hand_test::find_and_check_strength(Wincondition::high_card, "c7 d8 s9 dj cq dk sa", Card::ace, Card::nine, false);
    ul high_card_low = hand_test::find_and_check_strength(Wincondition::high_card, "c2 d3 s4 d5 c7 d8 s9", Card::nine, Card::four, false);

    ASSERT_LT(straight_flush_high, royal_flush);
    ASSERT_LT(straight_flush_low, straight_flush_high);
    ASSERT_LT(four_of_a_kind_high, straight_flush_low);
    ASSERT_LT(four_of_a_kind_low, four_of_a_kind_high);
    ASSERT_LT(full_house_high, four_of_a_kind_low);
    ASSERT_LT(full_house_low, full_house_high);
    ASSERT_LT(flush_high, full_house_low);
    ASSERT_LT(flush_low, flush_high);
    ASSERT_LT(straight_high, flush_low);
    ASSERT_LT(straight_low, straight_high);
    ASSERT_LT(three_of_a_kind_high, straight_low);
    ASSERT_LT(three_of_a_kind_low, three_of_a_kind_high);
    ASSERT_LT(two_pair_high, three_of_a_kind_low);
    ASSERT_LT(two_pair_low, two_pair_high);
    ASSERT_LT(pair_high, two_pair_low);
    ASSERT_LT(pair_low, pair_high);
    ASSERT_LT(high_card_high, pair_low);
    ASSERT_LT(high_card_low, high_card_high);
}


TEST(CalculateHandStrength, HandFrequency) {
    std::vector<int> hand_frequency(10, 0);
    std::vector<int> correct_hand_frequency = { 23294460, 58627800, 31433400, 6461620, 6180020, 4047644, 3473184, 224848, 37260, 4324};
    hand_test::calculate_frequency(hand_frequency, 52, 7, 0ULL);

    for (unsigned int x = 0; x < correct_hand_frequency.size(); x++)
        ASSERT_EQ(hand_frequency[x], correct_hand_frequency[x]);
}

