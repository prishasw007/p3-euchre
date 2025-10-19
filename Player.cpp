// Player.cpp
#include "Player.hpp"
#include "Card.hpp"
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

class SimplePlayer : public Player {
public:
  explicit SimplePlayer(const string &name_in) : name(name_in) {}

  const string & get_name() const override {
    return name;
  }

  void add_card(const Card &c) override {
    hand.push_back(c);
  }

  // Round 1: order up if you have >= 2 trump (including left bower).
  // Round 2: if dealer => must choose next suit; else choose next suit iff you have >= 1 in that suit.
  bool make_trump(const Card &upcard, bool is_dealer, int round,
                  Suit &order_up_suit) const override {
    const Suit up_suit   = upcard.get_suit();
    const Suit next_suit = Suit_next(up_suit);

    if (round == 1) {
  int trump_faces = 0;
  for (const auto &c : hand) {
    if (c.is_trump(up_suit) && c.is_face_or_ace()) {
      ++trump_faces;
    }
  }
  if (trump_faces >= 2) {
    order_up_suit = up_suit;
    return true;
  }
  return false;
}

    // round == 2
    if (is_dealer) {
      order_up_suit = next_suit; // screw-the-dealer
      return true;
    } else {
      for (const auto &c : hand) {
        if (c.is_trump(next_suit)) {
          order_up_suit = next_suit;
          return true;
        }
      }
      return false;
    }
  }

  // Dealer picks up upcard and discards the LOWEST by Card_less with trump = upcard suit
  void add_and_discard(const Card &upcard) override {
    hand.push_back(upcard);
    const Suit trump = upcard.get_suit();
    auto min_it = hand.begin();
    for (auto it = hand.begin(); it != hand.end(); ++it) {
      if (Card_less(*it, *min_it, trump)) min_it = it;
    }
    hand.erase(min_it);
  }

  // Lead highest non-trump by operator< (rank/suit tie: D>C>H>S).
  // If no non-trump, lead highest trump by Card_less (trump-aware).
  Card lead_card(Suit trump) override {
    auto best = hand.end();

    // Highest non-trump using operator<
    for (auto it = hand.begin(); it != hand.end(); ++it) {
      if (!it->is_trump(trump)) {
        if (best == hand.end() || (*best < *it)) best = it;
      }
    }

    // If none, highest overall (trump) using Card_less
    if (best == hand.end()) {
      for (auto it = hand.begin(); it != hand.end(); ++it) {
        if (best == hand.end() || Card_less(*best, *it, trump)) best = it;
      }
    }

    Card led = *best;
    hand.erase(best);
    return led;
  }

  // If can follow suit: play the HIGHEST of the led suit (Card_less).
  // Else: prefer to slough the LOWEST non-trump using operator<; if all cards
  // are trump, play the LOWEST trump using Card_less (trump-aware).
  Card play_card(const Card &led_card, Suit trump) override {
    const Suit led_suit = led_card.get_suit(trump);

    // Try to follow suit: play highest of led suit
    auto candidate = hand.end();
    for (auto it = hand.begin(); it != hand.end(); ++it) {
      if (it->get_suit(trump) == led_suit) {
        if (candidate == hand.end() || Card_less(*candidate, *it, trump)) {
          candidate = it;
        }
      }
    }
    if (candidate != hand.end()) {
      Card played = *candidate;
      hand.erase(candidate);
      return played;
    }

    // Can't follow: play lowest non-trump if possible
    auto lowest_non_trump = hand.end();
    for (auto it = hand.begin(); it != hand.end(); ++it) {
      if (!it->is_trump(trump)) {
        if (lowest_non_trump == hand.end() || (*it < *lowest_non_trump)) {
          lowest_non_trump = it;
        }
      }
    }
    if (lowest_non_trump != hand.end()) {
      Card played = *lowest_non_trump;
      hand.erase(lowest_non_trump);
      return played;
    }

    // All remaining cards are trump: play the lowest trump by Card_less
    auto lowest_trump = hand.begin();
    for (auto it = hand.begin(); it != hand.end(); ++it) {
      if (Card_less(*it, *lowest_trump, trump)) {
        lowest_trump = it;
      }
    }
    Card played = *lowest_trump;
    hand.erase(lowest_trump);
    return played;
  }

private:
  string name;
  vector<Card> hand;
};

class Human : public Player {
public:
  explicit Human(const string &name_in) : name(name_in) {}

  const string & get_name() const override { return name; }
  void add_card(const Card &c) override { hand.push_back(c); }

  bool make_trump(const Card &, bool, int, Suit &) const override {
    assert(false); return false;
  }
  void add_and_discard(const Card &) override { assert(false); }
  Card lead_card(Suit) override { assert(false); return Card(); }
  Card play_card(const Card &, Suit) override { assert(false); return Card(); }

private:
  string name;
  vector<Card> hand;
};

Player * Player_factory(const std::string &name, const std::string &strategy) {
  if (strategy == "Simple") return new SimplePlayer(name);
  if (strategy == "Human")  return new Human(name);
  return nullptr;
}

ostream & operator<<(ostream &os, const Player &p) {
  os << p.get_name();
  return os;
}
