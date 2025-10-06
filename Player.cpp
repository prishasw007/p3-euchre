#include "Player.hpp"
#include "Card.hpp"
#include <iostream>
#include <algorithm>
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

    bool make_trump(const Card &upcard, bool, int round, Suit &order_up_suit) const override {
        int count = 0;
        Suit suit_to_consider = upcard.get_suit();
        if (round == 2) suit_to_consider = Suit_next(upcard.get_suit());

        for (auto &c : hand) {
            if (c.is_trump(suit_to_consider) && c.is_face_or_ace()) ++count;
        }

        if ((round == 1 && count >= 2) || (round == 2 && count >= 1)) {
            order_up_suit = suit_to_consider;
            return true;
        }
        return false;
    }

    void add_and_discard(const Card &upcard) override {
        hand.push_back(upcard);
        auto min_it = min_element(hand.begin(), hand.end(),
                                  [](const Card &a, const Card &b){ return a < b; });
        hand.erase(min_it);
    }

    Card lead_card(Suit trump) override {
        auto it = max_element(hand.begin(), hand.end(),
                              [trump](const Card &a, const Card &b){
                                  if (a.is_trump(trump) && !b.is_trump(trump)) return true;
                                  if (!a.is_trump(trump) && b.is_trump(trump)) return false;
                                  return a < b;
                              });
        Card c = *it;
        hand.erase(it);
        return c;
    }

    Card play_card(const Card &led_card, Suit trump) override {
        Suit led_suit = led_card.get_suit(trump);
        auto it = hand.end();
        for (size_t i = 0; i < hand.size(); ++i) {
            if (hand[i].get_suit(trump) == led_suit) {
                if (it == hand.end() || hand[i] > *it) it = hand.begin() + i;
            }
        }
        if (it == hand.end()) {
            it = min_element(hand.begin(), hand.end());
        }
        Card c = *it;
        hand.erase(it);
        return c;
    }

private:
    string name;
    vector<Card> hand;
};


class Human : public Player {
public:
    explicit Human(const string &name_in) : name(name_in) {}

    const string & get_name() const override {
        return name;
    }

    void add_card(const Card &c) override {
        hand.push_back(c);
    }

    bool make_trump(const Card &upcard, bool, int round, Suit &order_up_suit) const override {
        // Implementation left for Human input
        assert(false);
        return false;
    }

    void add_and_discard(const Card &upcard) override {
        assert(false);
    }

    Card lead_card(Suit trump) override {
        assert(false);
        return Card();
    }

    Card play_card(const Card &led_card, Suit trump) override {
        assert(false);
        return Card();
    }

private:
    string name;
    vector<Card> hand;
};


Player * Player_factory(const std::string &name, const std::string &strategy) {
    if (strategy == "Simple") return new SimplePlayer(name);
    if (strategy == "Human") return new Human(name);
    return nullptr;  // invalid strategy
}


ostream & operator<<(ostream &os, const Player &p) {
    os << p.get_name();
    return os;
}
