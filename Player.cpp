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

    bool make_trump(const Card &upcard, bool is_dealer, int round, 
                    Suit &order_up_suit) const override {
        Suit up_suit = upcard.get_suit();
        Suit next_suit = Suit_next(up_suit);

        if (round == 1) {
            // First round: count only cards of the upcard's printed suit
            int suit_count = 0;
            for (const auto &c : hand) {
                if (c.get_suit() == up_suit) ++suit_count;
            }
            // Eldest/non-dealer needs >=2; dealer needs >=3 to order up
            const int needed = is_dealer ? 3 : 2;
            if (suit_count >= needed) {
                order_up_suit = up_suit;
                return true;
            }
            return false;
        }

        // Second round: order up next suit if holding at least one card of that
        // OR if dealer (screw the dealer rule)
        if (is_dealer) {
            order_up_suit = next_suit;
            return true;
        }
        
        // Non-dealer: order up next suit if holding at least one card of that
        for (const auto &c : hand) {
            if (c.get_suit() == next_suit) {
                order_up_suit = next_suit;
                return true;
            }
        }
        
        return false;
    }

    void add_and_discard(const Card &upcard) override {
        hand.push_back(upcard);
        // Discard preference: lowest non-trump if any; otherwise lowest trump,
        // avoiding right bower if possible
        Suit trump = upcard.get_suit();
        // Try to find lowest non-trump
        auto best_it = hand.end();
        for (auto it = hand.begin(); it != hand.end(); ++it) {
            if (!it->is_trump(trump)) {
                if (best_it == hand.end() || Card_less(*it, *best_it, trump)) best_it = it;
            }
        }
        if (best_it == hand.end()) {
            // All trumps: discard lowest trump, but avoid right bower if possible
            // First find any non-right-bower trump as candidate
            for (auto it = hand.begin(); it != hand.end(); ++it) {
                if (it->is_trump(trump) && !it->is_right_bower(trump)) {
                    if (best_it == hand.end() || 
                        Card_less(*it, *best_it, trump)) best_it = it;
                }
            }
            // If all are right bower (only possible if exactly one trump), 
            // fallback to absolute lowest
            if (best_it == hand.end()) {
                best_it = hand.begin();
                for (auto it = hand.begin(); it != hand.end(); ++it) {
                    if (Card_less(*it, *best_it, trump)) best_it = it;
                }
            }
        }
        hand.erase(best_it);
    }

    Card lead_card(Suit trump) override {
        // Lead highest non-trump if any; else highest trump
        auto pick_it = hand.end();
        for (auto it = hand.begin(); it != hand.end(); ++it) {
            if (!it->is_trump(trump)) {
                if (pick_it == hand.end() || 
                    Card_less(*pick_it, *it, trump)) pick_it = it;
            }
        }
        if (pick_it == hand.end()) {
            for (auto it = hand.begin(); it != hand.end(); ++it) {
                if (pick_it == hand.end() || 
                    Card_less(*pick_it, *it, trump)) pick_it = it;
            }
        }
        Card led = *pick_it;
        hand.erase(pick_it);
        return led;
    }

    Card play_card(const Card &led_card, Suit trump) override {
        Suit led_suit = led_card.get_suit(trump);

        // Try to follow suit: play highest card of led suit
        auto it_to_play = hand.end();
        for (auto it = hand.begin(); it != hand.end(); ++it) {
            if (it->get_suit(trump) == led_suit) {
                if (it_to_play == hand.end() || 
                    Card_less(*it_to_play, *it, trump)) {
                    it_to_play = it;
                }
            }
        }

        if (it_to_play != hand.end()) {
            Card played = *it_to_play;
            hand.erase(it_to_play);
            return played;
        }

        // Cannot follow: discard lowest non-trump if any; otherwise lowest trump
        auto pick_it = hand.end();
        for (auto it = hand.begin(); it != hand.end(); ++it) {
            if (!it->is_trump(trump)) {
                if (pick_it == hand.end() || 
                    Card_less(*it, *pick_it, trump)) pick_it = it;
            }
        }
        if (pick_it == hand.end()) {
            pick_it = hand.begin();
            for (auto it = hand.begin(); it != hand.end(); ++it) {
                if (Card_less(*it, *pick_it, trump)) pick_it = it;
            }
        }
        Card played = *pick_it;
        hand.erase(pick_it);
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