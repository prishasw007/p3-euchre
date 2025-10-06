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
    // If any non-trump cards exist, lead the highest non-trump
    vector<Card> non_trumps;
    for (const auto &c : hand) {
        if (!c.is_trump(trump)) {
            non_trumps.push_back(c);
        }
    }

    Card led;
    if (!non_trumps.empty()) {
        led = *max_element(non_trumps.begin(), non_trumps.end(),
            [](const Card &a, const Card &b) { return a < b; });
    } else {
        // All trumps → highest by normal rank
        led = *max_element(hand.begin(), hand.end(),
            [](const Card &a, const Card &b) { return a < b; });
    }

    auto it = find(hand.begin(), hand.end(), led);
    hand.erase(it);
    return led;
}


Card play_card(const Card &led_card, Suit trump) override {
    Suit led_suit = led_card.get_suit(trump);

    // Collect cards that follow suit
    vector<Card> following;
    for (const auto &c : hand) {
        if (c.get_suit(trump) == led_suit) {
            following.push_back(c);
        }
    }

    Card played;
    if (!following.empty()) {
        // Follow suit → play highest following-suit card
        played = *max_element(following.begin(), following.end(),
            [trump](const Card &a, const Card &b) {
                return Card_less(a, b, trump);
            });
    } else {
        // Can't follow suit → play lowest *by rank*, not by trump order
        played = *min_element(hand.begin(), hand.end(),
            [](const Card &a, const Card &b) {
                return a < b;  // normal rank comparison
            });
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
