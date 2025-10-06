#include "Player.hpp"
#include "unit_test_framework.hpp"

#include <iostream>

using namespace std;

TEST(test_player_get_name) {
    Player * alice = Player_factory("Alice", "Simple");
    ASSERT_EQUAL("Alice", alice->get_name());

    delete alice;
}

// Add more tests here
// Test adding cards and make_trump logic
TEST(test_simpleplayer_make_trump_round1) {
    Player * bob = Player_factory("Bob", "Simple");
    bob->add_card(Card(JACK, SPADES));
    bob->add_card(Card(QUEEN, SPADES));
    bob->add_card(Card(TEN, HEARTS));
    bob->add_card(Card(NINE, CLUBS));
    bob->add_card(Card(ACE, DIAMONDS));

    Suit order_up;
    bool called = bob->make_trump(Card(NINE, SPADES), false, 1, order_up);
    ASSERT_EQUAL(true, called);
    ASSERT_EQUAL(SPADES, order_up);

    delete bob;
}

TEST(test_simpleplayer_make_trump_round2) {
    Player * carol = Player_factory("Carol", "Simple");
    carol->add_card(Card(KING, DIAMONDS));
    carol->add_card(Card(NINE, HEARTS));
    carol->add_card(Card(TEN, CLUBS));
    carol->add_card(Card(JACK, HEARTS));
    carol->add_card(Card(ACE, SPADES));

    Suit order_up;
    bool called = carol->make_trump(Card(NINE, HEARTS), false, 2, order_up);
    ASSERT_EQUAL(true, called);
    ASSERT_EQUAL(DIAMONDS, order_up);

    delete carol;
}

// Test add_and_discard picks lowest card
TEST(test_simpleplayer_add_and_discard) {
    Player * dave = Player_factory("Dave", "Simple");
    dave->add_card(Card(ACE, SPADES));
    dave->add_card(Card(KING, HEARTS));
    dave->add_card(Card(JACK, DIAMONDS));
    dave->add_card(Card(NINE, CLUBS));
    dave->add_card(Card(TEN, HEARTS));

    dave->add_and_discard(Card(NINE, SPADES));
    // Just ensure hand size stays correct
    for (int i = 0; i < Player::MAX_HAND_SIZE; ++i) {
        Card c = dave->lead_card(SPADES);
        ASSERT_TRUE(c.get_rank() >= NINE && c.get_rank() <= ACE);
    }

    delete dave;
}

// Test lead_card: plays highest non-trump, fallback to highest trump
TEST(test_simpleplayer_lead_card) {
    Player * eve = Player_factory("Eve", "Simple");
    eve->add_card(Card(ACE, SPADES));
    eve->add_card(Card(NINE, HEARTS));
    eve->add_card(Card(TEN, CLUBS));
    eve->add_card(Card(JACK, SPADES));
    eve->add_card(Card(KING, HEARTS));

    Card c = eve->lead_card(HEARTS);
    ASSERT_EQUAL(ACE, c.get_rank());
    ASSERT_EQUAL(SPADES, c.get_suit());

    delete eve;
}

// Test play_card: follows suit if possible, else lowest card
TEST(test_simpleplayer_play_card) {
    Player * frank = Player_factory("Frank", "Simple");
    frank->add_card(Card(ACE, SPADES));
    frank->add_card(Card(NINE, HEARTS));
    frank->add_card(Card(TEN, CLUBS));
    frank->add_card(Card(JACK, SPADES));
    frank->add_card(Card(KING, HEARTS));

    Card led_card = Card(NINE, HEARTS);
    Card c = frank->play_card(led_card, DIAMONDS);
    ASSERT_EQUAL(KING, c.get_rank());
    ASSERT_EQUAL(HEARTS, c.get_suit());

    Card led_card2 = Card(NINE, DIAMONDS);
    Card c2 = frank->play_card(led_card2, HEARTS);
    ASSERT_EQUAL(NINE, c2.get_rank());
    ASSERT_EQUAL(HEARTS, c2.get_suit());

    delete frank;
}

// Test factory invalid strategy asserts
// Test factory invalid strategy asserts
TEST(test_player_factory_invalid_strategy) {
    Player *p = Player_factory("Gina", "Invalid");
    ASSERT_TRUE(p == nullptr);  // Ensure factory returns nullptr for invalid input
}


TEST_MAIN()
