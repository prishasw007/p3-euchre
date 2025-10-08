#include "Player.hpp"
#include "Card.hpp"
#include <iostream>
#include <algorithm>
#include <cassert>
#include <vector>

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

    bool make_trump(const Card &upcard, bool is_dealer, int round, Suit &order_up_suit) const override {
        Suit up_suit = upcard.get_suit();
        Suit next_suit = Suit_next(up_suit);

        if (round == 1) {
            int trump_count = 0;
            for (const auto &c : hand) {
                if (c.is_trump(up_suit)) ++trump_count;
            }
            if (trump_count >= 2) {
                order_up_suit = up_suit;
                return true;
            }
            return false;
        }

        if (is_dealer) {
            order_up_suit = next_suit;
            return true;
        }

        int good_cards = 0;
        for (const auto &c : hand) {
            if (c.is_trump(next_suit) && c.is_face_or_ace()) ++good_cards;
        }
        if (good_cards >= 1) {
            order_up_suit = next_suit;
            return true;
        }

        return false;
    }

    void add_and_discard(const Card &upcard) override {
        hand.push_back(upcard);
        // Discard the lowest rank card (ignore trump rules)
        auto min_it = min_element(hand.begin(), hand.end(),
                                  [](const Card &a, const Card &b){
                                      return a.get_rank() < b.get_rank();
                                  });
        hand.erase(min_it);
    }

    Card lead_card(Suit trump) override {
        vector<Card> non_trumps;
        for (const auto &c : hand) {
            if (!c.is_trump(trump)) {
                non_trumps.push_back(c);
            }
        }

        Card led;
        if (!non_trumps.empty()) {
            // Lead highest non-trump
            led = *max_element(non_trumps.begin(), non_trumps.end(),
                               [trump](const Card &a, const Card &b) {
                                   return Card_less(a, b, trump);
                               });
        } else {
            // All cards are trump, lead highest trump
            led = *max_element(hand.begin(), hand.end(),
                               [trump](const Card &a, const Card &b) {
                                   return Card_less(a, b, trump);
                               });
        }

        auto it = find(hand.begin(), hand.end(), led);
        hand.erase(it);
        return led;
    }

    Card play_card(const Card &led_card, Suit trump) override {
        Suit led_suit = led_card.get_suit(trump);
        vector<Card> following;
        for (const auto &c : hand) {
            if (c.get_suit(trump) == led_suit) {
                following.push_back(c);
            }
        }

        Card played;
        if (!following.empty()) {
            // Follow suit with highest card of that suit
            played = *max_element(following.begin(), following.end(),
                                [trump](const Card &a, const Card &b){ return Card_less(a,b,trump); });
        } else {    
            // Cannot follow suit: play the card with lowest rank (ignore trump)
            played = *min_element(hand.begin(), hand.end(),
                                [](const Card &a, const Card &b){ return a.get_rank() < b.get_rank(); });
        }   

        auto it = find(hand.begin(), hand.end(), played);
        hand.erase(it);
        return played;
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

    bool make_trump(const Card &, bool, int, Suit &) const override {
        assert(false);
        return false;
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
    if (strategy == "Human") return new Human(name);
    return nullptr;
}

ostream & operator<<(ostream &os, const Player &p) {
    os << p.get_name();
    return os;
}
