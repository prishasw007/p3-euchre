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
    Player *p = Player_factory("Frank", "Simple");

    p->add_card(Card(NINE, CLUBS));
    p->add_card(Card(KING, CLUBS));
    p->add_card(Card(TEN, HEARTS));
    p->add_card(Card(ACE, SPADES));
    p->add_card(Card(JACK, DIAMONDS));

    Card led = Card(NINE, CLUBS);
    Suit trump = HEARTS;

    // Should play highest following-suit card (King of Clubs)
    Card c1 = p->play_card(led, trump);
    ASSERT_EQUAL(c1.get_rank(), KING);
    ASSERT_EQUAL(c1.get_suit(), CLUBS);

    delete p;
}

TEST(test_simpleplayer_make_trump_dealer) {
    Player *p = Player_factory("Dealer", "Simple");
    p->add_card(Card(NINE, SPADES));
    p->add_card(Card(TEN, SPADES));
    p->add_card(Card(QUEEN, DIAMONDS));
    p->add_card(Card(KING, CLUBS));
    p->add_card(Card(ACE, CLUBS));

    Suit order_up;
    Card upcard(JACK, HEARTS);
    // Round 2, dealer must order up to same color suit (Hearts → Diamonds)
    bool called = p->make_trump(upcard, true, 2, order_up);

    ASSERT_TRUE(called);
    ASSERT_EQUAL(order_up, DIAMONDS);
    delete p;
}

TEST(test_simpleplayer_make_trump_round1_pass) {
    Player *p = Player_factory("Passer", "Simple");
    p->add_card(Card(NINE, CLUBS));
    p->add_card(Card(TEN, DIAMONDS));
    p->add_card(Card(QUEEN, HEARTS));
    p->add_card(Card(KING, CLUBS));
    p->add_card(Card(ACE, SPADES));

    Suit order_up;
    Card upcard(JACK, HEARTS);
    bool called = p->make_trump(upcard, false, 1, order_up);

    ASSERT_FALSE(called);
    delete p;
}

TEST(test_simpleplayer_lead_card_all_trump) {
    Player *p = Player_factory("TrumpLeader", "Simple");
    p->add_card(Card(NINE, SPADES));
    p->add_card(Card(JACK, SPADES));
    p->add_card(Card(ACE, SPADES));
    p->add_card(Card(QUEEN, SPADES));
    p->add_card(Card(KING, SPADES));

    Suit trump = SPADES;
    Card led = p->lead_card(trump);
    ASSERT_EQUAL(led.get_rank(), ACE);
    ASSERT_EQUAL(led.get_suit(), SPADES);
    delete p;
}

TEST(test_simpleplayer_play_card_cannot_follow) {
    Player *p = Player_factory("Follower", "Simple");
    p->add_card(Card(NINE, SPADES));
    p->add_card(Card(TEN, SPADES));
    p->add_card(Card(QUEEN, HEARTS));
    p->add_card(Card(KING, HEARTS));
    p->add_card(Card(ACE, DIAMONDS));

    Card led(TEN, CLUBS);
    Suit trump = SPADES;

    Card played = p->play_card(led, trump);
    // Should play lowest card overall (NINE of SPADES)
    ASSERT_EQUAL(played.get_rank(), NINE);
    ASSERT_EQUAL(played.get_suit(), SPADES);
    delete p;
}

TEST(test_player_factory_invalid_strategy) {
    Player* p = Player_factory("Invalid", "NotARealStrategy");
    ASSERT_TRUE(p == nullptr);
}

TEST(test_simpleplayer_make_trump_round2_pass) {
    Player* p = Player_factory("NoTrump", "Simple");
    p->add_card(Card(NINE, CLUBS));
    p->add_card(Card(TEN, CLUBS));
    p->add_card(Card(NINE, DIAMONDS));
    p->add_card(Card(TEN, SPADES));
    p->add_card(Card(QUEEN, CLUBS));

    Suit order_up;
    Card upcard(JACK, HEARTS);
    bool called = p->make_trump(upcard, false, 2, order_up);
    ASSERT_FALSE(called);
    delete p;
}

TEST(test_simpleplayer_add_and_discard_removes_lowest) {
    Player* p = Player_factory("Dealer", "Simple");
    p->add_card(Card(NINE, CLUBS));
    p->add_card(Card(TEN, CLUBS));
    p->add_card(Card(JACK, HEARTS));
    p->add_card(Card(QUEEN, HEARTS));
    p->add_card(Card(KING, HEARTS));

    p->add_and_discard(Card(ACE, CLUBS));

    // Lowest (9C) should be gone
    for (int i = 0; i < 5; i++) {
        Card c = p->lead_card(SPADES);
        ASSERT_TRUE(!(c == Card(NINE, CLUBS)));
    }
    delete p;
}

TEST(test_simpleplayer_lead_card_removes_card) {
    Player* p = Player_factory("Leader", "Simple");
    p->add_card(Card(ACE, SPADES));
    p->add_card(Card(KING, HEARTS));
    p->add_card(Card(QUEEN, CLUBS));
    p->add_card(Card(JACK, DIAMONDS));
    p->add_card(Card(TEN, SPADES));

    Card led = p->lead_card(HEARTS);
    ASSERT_EQUAL(led, Card(ACE, SPADES));
    // Next lead should return next-highest
    Card next = p->lead_card(HEARTS);
    ASSERT_TRUE(!(next == led));
    delete p;
}

TEST(test_simpleplayer_play_card_removes_card) {
    Player* p = Player_factory("Follower", "Simple");
    p->add_card(Card(NINE, HEARTS));
    p->add_card(Card(TEN, HEARTS));
    p->add_card(Card(QUEEN, CLUBS));
    p->add_card(Card(KING, CLUBS));
    p->add_card(Card(ACE, DIAMONDS));

    Card led(NINE, HEARTS);
    Suit trump = CLUBS;
    Card first_play = p->play_card(led, trump);
    ASSERT_EQUAL(first_play.get_suit(), HEARTS);

    Card second_play = p->play_card(led, trump);
    ASSERT_TRUE(!(second_play == first_play));
    delete p;
}

TEST(test_simpleplayer_play_card_prefers_follow_suit) {
    Player* p = Player_factory("Smart", "Simple");
    p->add_card(Card(NINE, CLUBS));
    p->add_card(Card(TEN, SPADES));
    p->add_card(Card(QUEEN, CLUBS));
    p->add_card(Card(KING, HEARTS));
    p->add_card(Card(ACE, DIAMONDS));

    Card led(QUEEN, CLUBS);
    Suit trump = HEARTS;
    Card played = p->play_card(led, trump);

    // Should follow Clubs (highest in that suit)
    ASSERT_EQUAL(played.get_suit(), CLUBS);
    delete p;
}

TEST(test_simpleplayer_round_flow) {
    Player* leader = Player_factory("Lead", "Simple");
    Player* follower = Player_factory("Follow", "Simple");

    leader->add_card(Card(ACE, SPADES));
    leader->add_card(Card(KING, DIAMONDS));
    leader->add_card(Card(QUEEN, HEARTS));
    leader->add_card(Card(JACK, CLUBS));
    leader->add_card(Card(TEN, CLUBS));

    follower->add_card(Card(ACE, HEARTS));
    follower->add_card(Card(KING, HEARTS));
    follower->add_card(Card(JACK, SPADES));
    follower->add_card(Card(QUEEN, DIAMONDS));
    follower->add_card(Card(TEN, CLUBS));

    Suit trump = HEARTS;
    Card led = leader->lead_card(trump);
    Card played = follower->play_card(led, trump);

    ASSERT_FALSE(led < played || !(played < led)); // just ensure valid play
    delete leader;
    delete follower;
}



TEST_MAIN()
