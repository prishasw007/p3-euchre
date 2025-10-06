#include "Pack.hpp"
#include <algorithm> 
#include <cassert>
#include <iostream>

using namespace std;

// Default constructor: initialize in standard pack order
Pack::Pack() : next(0) {
    int index = 0;
    for (int s = SPADES; s <= DIAMONDS; ++s) {
        Suit suit = static_cast<Suit>(s);
        for (int r = NINE; r <= ACE; ++r) {  // Euchre uses 9 to Ace
            Rank rank = static_cast<Rank>(r);
            cards[index++] = Card(rank, suit);
        }
    }
}

// Stream constructor: read pack from input
Pack::Pack(istream& pack_input) : next(0) {
    for (int i = 0; i < PACK_SIZE; ++i) {
        pack_input >> cards[i];
    }
}

// Return next card and increment
Card Pack::deal_one() {
     if (empty()) throw out_of_range("No cards left in pack"); 
    return cards[next++];
}

// Reset next index
void Pack::reset() {
    next = 0;
}

// Check if pack is empty
bool Pack::empty() const {
    return next >= PACK_SIZE;
}

// Shuffle using in-shuffle 7 times
void Pack::shuffle() {
    for (int shuffle_count = 0; shuffle_count < 7; ++shuffle_count) {
        array<Card, PACK_SIZE> shuffled;
        int mid = PACK_SIZE / 2;  // 12
        int i = 0;
        for (int k = 0; k < mid; ++k) {
            shuffled[i++] = cards[mid + k]; // bottom half first
            shuffled[i++] = cards[k];       // then top half
        }
        cards = shuffled;
    }
    reset();
}


