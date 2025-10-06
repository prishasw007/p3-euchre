#include "Pack.hpp"
#include "unit_test_framework.hpp"

#include <iostream>

#include <sstream>
#include <set>



using namespace std;

TEST(test_pack_default_ctor) {
    Pack pack;
    Card first = pack.deal_one();
    ASSERT_EQUAL(NINE, first.get_rank());
    ASSERT_EQUAL(SPADES, first.get_suit());
}

// Add more tests here

TEST(test_pack_empty) {
    Pack pack;
    for (int i = 0; i < 24; ++i) {
        ASSERT_FALSE(pack.empty());
        pack.deal_one();
    }
    ASSERT_TRUE(pack.empty());
}

TEST(test_pack_reset) {
    Pack pack;
    for (int i = 0; i < 5; ++i)
        pack.deal_one();
    pack.reset();
    Card first = pack.deal_one();
    ASSERT_EQUAL(first.get_rank(), NINE);
    ASSERT_EQUAL(first.get_suit(), SPADES);
}

TEST(test_pack_order) {
    Pack pack;
    Card c1 = pack.deal_one(); // Should be Nine of Spades
    ASSERT_EQUAL(c1, Card(NINE, SPADES));
    Card c2 = pack.deal_one(); // Should be Ten of Spades
    ASSERT_EQUAL(c2, Card(TEN, SPADES));
}

TEST(test_pack_shuffle) {
    Pack pack;
    Card before_first = pack.deal_one();
    pack.reset();
    pack.shuffle();
    Card after_first = pack.deal_one();
    ASSERT_TRUE(before_first.get_rank() >= NINE && before_first.get_rank() <= ACE);
    ASSERT_TRUE(after_first.get_rank() >= NINE && after_first.get_rank() <= ACE);
}


TEST(test_pack_deal_all_cards) {
    Pack pack;
    const int TOTAL = 24;
    Card all_cards[TOTAL];
    for (int i = 0; i < TOTAL; ++i) {
        ASSERT_FALSE(pack.empty());
        all_cards[i] = pack.deal_one();
        // Check rank and suit are valid
        ASSERT_TRUE(all_cards[i].get_rank() >= NINE && all_cards[i].get_rank() <= ACE);
    }
    ASSERT_TRUE(pack.empty());

    // Check for duplicates manually
    for (int i = 0; i < TOTAL; ++i) {
        for (int j = i + 1; j < TOTAL; ++j) {
            ASSERT_FALSE(all_cards[i] == all_cards[j]);
        }
    }
}

TEST(test_pack_deal_past_end) {
    Pack pack;
    for (int i = 0; i < 24; ++i)
        pack.deal_one();

    bool threw = false;
    try {
        pack.deal_one(); // should throw or assert
    } catch (...) {
        threw = true;
    }
    ASSERT_TRUE(threw);
}

TEST(test_pack_shuffle_after_partial_deal) {
    Pack pack;
    for (int i = 0; i < 5; ++i)
        pack.deal_one();
    pack.shuffle();
    ASSERT_FALSE(pack.empty());
    pack.reset();
    ASSERT_FALSE(pack.empty());
}

TEST_MAIN()