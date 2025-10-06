#include "Card.hpp"
#include "unit_test_framework.hpp"
#include <iostream>

using namespace std;


TEST(test_card_ctor) {
    Card c(ACE, HEARTS);
    ASSERT_EQUAL(ACE, c.get_rank());
    ASSERT_EQUAL(HEARTS, c.get_suit());
}

// Add more test cases here

TEST(test_face_or_ace) {
    ASSERT_TRUE(Card(JACK, SPADES).is_face_or_ace());
    ASSERT_TRUE(Card(QUEEN, HEARTS).is_face_or_ace());
    ASSERT_TRUE(Card(KING, DIAMONDS).is_face_or_ace());
    ASSERT_TRUE(Card(ACE, CLUBS).is_face_or_ace());
    ASSERT_FALSE(Card(NINE, SPADES).is_face_or_ace());
    ASSERT_FALSE(Card(TEN, HEARTS).is_face_or_ace());
}

TEST(test_right_left_bower_logic) {
    Card right(JACK, HEARTS);
    Card left(JACK, DIAMONDS);
    Suit trump = HEARTS;

    ASSERT_TRUE(right.is_right_bower(trump));
    ASSERT_FALSE(right.is_left_bower(trump));
    ASSERT_FALSE(left.is_right_bower(trump));
    ASSERT_TRUE(left.is_left_bower(trump));

    // Left bower is treated as trump suit
    ASSERT_EQUAL(left.get_suit(trump), trump);
}

TEST(test_is_trump) {
    Suit trump = SPADES;
    Card right(JACK, SPADES); // right bower
    Card left(JACK, CLUBS);  // left bower
    Card normal_trump(ACE, SPADES);
    Card non_trump(ACE, HEARTS);

    ASSERT_TRUE(right.is_trump(trump));
    ASSERT_TRUE(left.is_trump(trump));
    ASSERT_TRUE(normal_trump.is_trump(trump));
    ASSERT_FALSE(non_trump.is_trump(trump));
}

TEST(test_operator_comparisons_no_trump) {
    Card c1(ACE, HEARTS);
    Card c2(ACE, DIAMONDS);
    Card c3(KING, HEARTS);
    Card c4(KING, HEARTS);

    ASSERT_TRUE(c3 < c1); // higher rank wins
    ASSERT_TRUE(c1 < c2); // tie-break by suit: HEARTS < DIAMONDS
    ASSERT_TRUE(c1 != c2);
    ASSERT_TRUE(c1 == Card(ACE, HEARTS));
    ASSERT_TRUE(c3 <= c4);
    ASSERT_TRUE(c1 >= c3);
}

TEST(test_card_less_with_trump) {
    Suit trump = DIAMONDS;
    Card right(JACK, DIAMONDS);  // right bower
    Card left(JACK, HEARTS);     // left bower
    Card trump_ace(ACE, DIAMONDS);
    Card normal_ace(ACE, SPADES);
    Card low_card(NINE, CLUBS);

    ASSERT_FALSE(Card_less(right, left, trump)); // right > left
    ASSERT_TRUE(Card_less(low_card, trump_ace, trump));
    ASSERT_FALSE(Card_less(trump_ace, right, trump));
    ASSERT_TRUE(Card_less(normal_ace, trump_ace, trump));
}

TEST(test_card_less_with_led_suit) {
    Suit trump = HEARTS;
    Card led_ace(ACE, SPADES); // led suit
    Card non_trump_follow(QUEEN, SPADES);
    Card off_suit(ACE, DIAMONDS);
    Card trump_card(JACK, HEARTS); // right bower

    // led suit beats off-suit
    ASSERT_FALSE(Card_less(non_trump_follow, led_ace, led_ace, trump));
    ASSERT_TRUE(Card_less(off_suit, led_ace, led_ace, trump));

    // trump beats everything
    ASSERT_FALSE(Card_less(trump_card, off_suit, led_ace, trump));
}

TEST(test_suit_next_function) {
    ASSERT_EQUAL(Suit_next(SPADES), CLUBS);   // same color
    ASSERT_EQUAL(Suit_next(HEARTS), DIAMONDS);
    ASSERT_EQUAL(Suit_next(CLUBS), SPADES);
    ASSERT_EQUAL(Suit_next(DIAMONDS), HEARTS);
}

TEST(test_stream_operators) {
    stringstream ss;
    Card c1(ACE, HEARTS);
    ss << c1;
    Card c2;
    ss >> c2;
    ASSERT_TRUE(c1.get_rank() == c2.get_rank());
    ASSERT_TRUE(c1.get_suit() == c2.get_suit());
}

TEST(test_all_ranks_trump_logic) {
    Suit trump = CLUBS;
    for (int r = NINE; r <= ACE; ++r) {
        Card c(static_cast<Rank>(r), trump);
        ASSERT_TRUE(c.is_trump(trump));
        if (r >= JACK) {
            ASSERT_TRUE(c.is_face_or_ace());
        } else {
            ASSERT_FALSE(c.is_face_or_ace());
        }
    }
}

TEST_MAIN()
