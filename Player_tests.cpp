#include "unit_test_framework.hpp"
#include "Player.hpp"
#include "Card.hpp"
#include <sstream>
#include <iostream>

using namespace std;


// Add more tests here
TEST(check_name){
  // Create a Human player
  Player * person = Player_factory("sumedha", "Human");

  // Print the player using the stream insertion operator
  ostringstream oss1;
  oss1 << * person;

  // Verify that the output is the player's name
  ASSERT_EQUAL(oss1.str(), "sumedha");

  // Create a Simple player
  Player * prisha = Player_factory("prisha", "Simple");

  // Print the player using the stream insertion operator
  ostringstream oss2;
  oss2 << * prisha;
  ASSERT_EQUAL(oss2.str(), "prisha");

  ASSERT_EQUAL(person->get_name(), "sumedha");
  ASSERT_EQUAL(prisha->get_name(), "prisha");

  // Clean up players that were created using Player_factory()
  delete person;
  delete prisha;
}

TEST(trump_round1){
  // Bob's hand
  Player * bob = Player_factory("Bob", "Simple");
  bob->add_card(Card(NINE, CLUBS));
  bob->add_card(Card(TEN, CLUBS));
  bob->add_card(Card(QUEEN, SPADES));
  bob->add_card(Card(JACK, CLUBS));
  bob->add_card(Card(ACE, CLUBS));

  // Bob makes trump
  Card nine_spades(KING, SPADES);
  Suit trump;
  bool orderup = bob->make_trump(
    nine_spades,    // Upcard
    true,           // Bob is also the dealer
    1,              // First round
    trump           // Suit ordered up (if any)
  );

  // Verify Bob's order up and trump suit
  ASSERT_TRUE(orderup);
  ASSERT_EQUAL(trump, SPADES);

  // Bob makes tump
  Card nine_spades2(KING, HEARTS);
  Suit trump2;
  bool orderup2 = bob->make_trump(
    nine_spades2,    // Upcard
    true,           // Bob is also the dealer
    1,              // First round
    trump2           // Suit ordered up (if any)
  );

  // Verify Bob's order up and trump suit
  ASSERT_FALSE(orderup2);

  // Bob makes tump
  Card nine_spades3(KING, CLUBS);
  Suit trump3;
  bool orderup3 = bob->make_trump(
    nine_spades3,    // Upcard
    true,           // Bob is also the dealer
    1,              // First round
    trump3           // Suit ordered up (if any)
  );

  // Verify Bob's order up and trump suit
  ASSERT_TRUE(orderup3);
  ASSERT_EQUAL(trump3, CLUBS);

  delete bob;
}

TEST(trump_round2_dealer){
  // Bob's hand
  Player * bob = Player_factory("Bob", "Simple");
  bob->add_card(Card(NINE, SPADES));
  bob->add_card(Card(TEN, SPADES));
  bob->add_card(Card(QUEEN, CLUBS));
  bob->add_card(Card(JACK, CLUBS));
  bob->add_card(Card(ACE, CLUBS));

  //Nick comment screw the dealer condition
  Card upcard(TWO, DIAMONDS);
  Suit trump0;
  bool orderup0 = bob->make_trump(
    upcard,   //upcard
    true,     //bob is dealer
    2,        //second round
    trump0
  );
  ASSERT_TRUE(orderup0);
  ASSERT_EQUAL(trump0, HEARTS);

  // Bob makes tump
  Card nine_spades(KING, SPADES);
  Suit trump;
  bool orderup = bob->make_trump(
    nine_spades,    // Upcard
    true,           // Bob is also the dealer
    2,              // First round
    trump           // Suit ordered up (if any)
  );

  // Verify Bob's order up and trump suit
  ASSERT_TRUE(orderup);
  ASSERT_EQUAL(trump, CLUBS);

  // Bob makes tump
  Card nine_spades2(KING, HEARTS);
  Suit trump2;
  bool orderup2 = bob->make_trump(
    nine_spades2,    // Upcard
    false,           // Bob is also the dealer
    2,              // First round
    trump2          // Suit ordered up (if any)
  );

  // Verify Bob's order up and trump suit
  ASSERT_FALSE(orderup2);

  // Bob makes tump
  Card nine_spades3(KING, CLUBS);
  Suit trump3;
  bool orderup3 = bob->make_trump(
    nine_spades3,    // Upcard
    false,           // Bob is also the dealer
    2,              // First round
    trump3          // Suit ordered up (if any)
  );

  // Verify Bob's order up and trump suit
  ASSERT_TRUE(orderup3);
  ASSERT_EQUAL(trump3, SPADES);

  delete bob;
}

TEST(Lead_card){
  // Bob's hand
  Player * bob = Player_factory("Bob", "Simple");
  bob->add_card(Card(ACE, SPADES));
  bob->add_card(Card(TEN, CLUBS));
  bob->add_card(Card(QUEEN, DIAMONDS));
  bob->add_card(Card(KING, HEARTS));
  bob->add_card(Card(ACE, CLUBS));

  // Bob adds a card to his hand and discards one card
  bob->add_and_discard(
    Card(NINE, HEARTS) // upcard
  );

  // Bob leads
  Card card_led = bob->lead_card(HEARTS);

  // Verify the card Bob selected to lead
  Card ace_spades(ACE, CLUBS);
  ASSERT_EQUAL(card_led, ace_spades); //check led card

  delete bob;
}

TEST(Lead_card_all_trump_leftbower){
  // Bob's hand
  Player * bob = Player_factory("Bob", "Simple");
  bob->add_card(Card(JACK, DIAMONDS));
  bob->add_card(Card(TEN, HEARTS));
  bob->add_card(Card(QUEEN, HEARTS));
  bob->add_card(Card(KING, HEARTS));
  bob->add_card(Card(ACE, HEARTS));

  // Bob adds a card to his hand and discards one card
  bob->add_and_discard(
    Card(NINE, HEARTS) // upcard
  );

  // Bob leads
  Card card_led = bob->lead_card(HEARTS);

  // Verify the card Bob selected to lead
  Card ace_spades(JACK, DIAMONDS);
  ASSERT_EQUAL(card_led, ace_spades); //check led card

  delete bob;
}

TEST(Lead_card_add_discard){
  // Bob's hand
  Card throwaway(TEN,DIAMONDS);
  Player * bob = Player_factory("Bob", "Simple");
  bob->add_card(Card(TEN, HEARTS));
  bob->add_card(Card(NINE, CLUBS));
  bob->add_card(Card(QUEEN, HEARTS));
  bob->add_card(Card(NINE, HEARTS));
  bob->add_card(Card(KING, HEARTS));

  // Bob adds a card to his hand and discards one card
  bob->add_and_discard(
    Card(ACE, HEARTS) // upcard
  );

  // Bob leads
  Card card_led = bob->lead_card(HEARTS);

  // Verify the card Bob selected to lead
  Card ace_spades(ACE, HEARTS);
 
  ASSERT_EQUAL(card_led, ace_spades); //check led card

  delete bob;
}

TEST(Lead_card_all_trump){
  // Bob's hand
  Player * bob = Player_factory("Bob", "Simple");
  bob->add_card(Card(JACK, HEARTS));
  bob->add_card(Card(TEN, HEARTS));
  bob->add_card(Card(QUEEN, HEARTS));
  bob->add_card(Card(KING, HEARTS));
  bob->add_card(Card(ACE, HEARTS));

  // Bob adds a card to his hand and discards one card
  bob->add_and_discard(
    Card(NINE, HEARTS) // upcard
  );

  // Bob leads
  Card card_led = bob->lead_card(HEARTS);

  // Verify the card Bob selected to lead
  Card ace_spades(JACK, HEARTS);
  ASSERT_EQUAL(card_led, ace_spades); //check led card

  delete bob;
}

TEST(play_card){
  // Bob's hand
  Player * bob = Player_factory("Bob", "Simple");
  bob->add_card(Card(TEN, DIAMONDS));
  bob->add_card(Card(TEN, SPADES));
  bob->add_card(Card(JACK, DIAMONDS));
  bob->add_card(Card(KING, SPADES));
  bob->add_card(Card(ACE, CLUBS));

  // Bob plays a card
  Card nine_diamonds(NINE, DIAMONDS);
  Card card_played = bob->play_card(
    nine_diamonds,  // Nine of Diamonds is led
    HEARTS    // Trump suit
  );

  // Verify the card Bob played
  ASSERT_EQUAL(card_played, Card(TEN, DIAMONDS));



  // Bob plays a card
  Card nine_diamonds2(NINE, DIAMONDS);
  Card card_played2 = bob->play_card(
    nine_diamonds2,  // Nine of Diamonds is led
    DIAMONDS    // Trump suit
  );

  // Verify the card Bob played
  ASSERT_EQUAL(card_played2, Card(JACK, DIAMONDS));

  delete bob;
}

TEST(play_card_cantfollow){
  // Bob's hand
  Player * bob = Player_factory("Bob", "Simple");
  bob->add_card(Card(JACK, CLUBS));
  bob->add_card(Card(JACK, SPADES));
  bob->add_card(Card(QUEEN, HEARTS));
  bob->add_card(Card(KING, SPADES));
  bob->add_card(Card(ACE, CLUBS));

  // Bob plays a card
  Card nine_diamonds(ACE, DIAMONDS);
  Card card_played = bob->play_card(
    nine_diamonds,  // Nine of Diamonds is led
    HEARTS    // Trump suit
  );

  // Verify the card Bob played
  ASSERT_EQUAL(card_played, Card(JACK, SPADES));
 
  Card card_played2 = bob->play_card(
    nine_diamonds,  // Nine of Diamonds is led
    HEARTS    // Trump suit
  );

  // Verify the card Bob played
  ASSERT_EQUAL(card_played2, Card(JACK, CLUBS));

  delete bob;
}

TEST_MAIN()