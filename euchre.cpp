// euchre.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

#include "Card.hpp"
#include "Pack.hpp"
#include "Player.hpp"

using namespace std;

static string suit_to_string(Suit s) {
  switch (s) {
    case SPADES:   return "Spades";
    case HEARTS:   return "Hearts";
    case CLUBS:    return "Clubs";
    case DIAMONDS: return "Diamonds";
  }
  return "";
}

class Game {
public:
  Game(Pack &pack_in, bool do_shuffle_in, int points_to_win_in,
       const vector<Player*> &players_in)
      : pack(pack_in),
        do_shuffle(do_shuffle_in),
        points_to_win(points_to_win_in),
        players(players_in),
        dealer(0),
        hand_number(0) {}

  void play() {
    int team0_points = 0; // players 0 & 2
    int team1_points = 0; // players 1 & 3

    while (team0_points < points_to_win && team1_points < points_to_win) {
      announce_hand_start();

      // Reset/shuffle at the start of *each* hand per spec
      if (do_shuffle) {
        pack.shuffle();
      } else {
        pack.reset();
      }

      deal();

      // Turn up the next card
      const Card upcard = pack.deal_one();
      cout << upcard << " turned up" << endl;

      // Make trump
      Suit trump = SPADES;        // will be set by make_trump()
      int maker_index = -1;
      make_trump(upcard, dealer, trump, maker_index);
      cout << endl; // extra newline when making/adding/discarding completes

      // Play the 5 tricks
      const pair<int,int> tricks = play_hand((dealer + 1) % 4, trump);

      // Score the hand
      apply_scoring(maker_index, tricks.first, tricks.second,
                    team0_points, team1_points);

      // Print score w/ extra newline
      print_scores(team0_points, team1_points);

      // Next hand
      dealer = (dealer + 1) % 4;
      ++hand_number;
    }

    announce_game_winner(team0_points, team1_points);
  }

private:
  Pack &pack;
  bool do_shuffle;
  int points_to_win;
  vector<Player*> players;
  int dealer;
  int hand_number;

  void announce_hand_start() const {
    cout << "Hand " << hand_number << endl;
    cout << players[dealer]->get_name() << " deals" << endl;
  }

 void deal() {
  // Round 1 (left of dealer): 3-2-3-2
  const int r1[4] = {3, 2, 3, 2};
  // Round 2 (left of dealer): 2-3-2-3
  const int r2[4] = {2, 3, 2, 3};

  for (int i = 1; i <= 4; ++i) {
    int p = (dealer + i) % 4;
    for (int c = 0; c < r1[i - 1]; ++c) {
      players[p]->add_card(pack.deal_one());
    }
  }
  for (int i = 1; i <= 4; ++i) {
    int p = (dealer + i) % 4;
    for (int c = 0; c < r2[i - 1]; ++c) {
      players[p]->add_card(pack.deal_one());
    }
  }
}

  // Handles both rounds of making trump and dealer add/discard if ordered up
  void make_trump(const Card &upcard, int dealer_index,
                  Suit &trump_suit, int &maker_index) {
    // Round 1: order up upcard suit
    for (int i = 1; i <= 4; ++i) {
      int idx = (dealer_index + i) % 4;
      bool is_dealer = (idx == dealer_index);
      if (players[idx]->make_trump(upcard, is_dealer, 1, trump_suit)) {
        cout << players[idx]->get_name()
             << " orders up " << suit_to_string(trump_suit) << endl;
        maker_index = idx;
        // Dealer must add and discard
        players[dealer_index]->add_and_discard(upcard);
        return;
      } else {
        cout << players[idx]->get_name() << " passes" << endl;
      }
    }

    // Round 2: call a different suit
    for (int i = 1; i <= 4; ++i) {
      int idx = (dealer_index + i) % 4;
      bool is_dealer = (idx == dealer_index);
      if (players[idx]->make_trump(upcard, is_dealer, 2, trump_suit)) {
        cout << players[idx]->get_name()
             << " orders up " << suit_to_string(trump_suit) << endl;
        maker_index = idx;
        return;
      } else {
        cout << players[idx]->get_name() << " passes" << endl;
      }
    }

    // By project rules/tests this shouldn't happen (someone must choose),
    // but guard anyway to avoid UB in scoring.
    // Fallback: dealer becomes maker with next suit (rare; not used by tests).
    maker_index = dealer_index;
    trump_suit = upcard.get_suit(); // reasonable default
  }

  pair<int,int> play_hand(int leader, Suit trump) {
    int team0_tricks = 0;
    int team1_tricks = 0;

    for (int t = 0; t < 5; ++t) {
      int winner = play_trick(leader, trump);
      if (winner % 2 == 0) ++team0_tricks; else ++team1_tricks;
      leader = winner; // winner leads next trick
    }

    // Announce hand winner (lower index partnership printed first)
    if (team0_tricks > team1_tricks) {
      cout << players[0]->get_name() << " and "
           << players[2]->get_name() << " win the hand" << endl;
    } else {
      cout << players[1]->get_name() << " and "
           << players[3]->get_name() << " win the hand" << endl;
    }
    return {team0_tricks, team1_tricks};
  }

  int play_trick(int leader, Suit trump) {
    Card led = players[leader]->lead_card(trump);
    cout << led << " led by " << players[leader]->get_name() << endl;

    int winning_index = leader;
    Card winning_card = led;

    for (int i = 1; i <= 3; ++i) {
      int idx = (leader + i) % 4;
      Card played = players[idx]->play_card(led, trump);
      cout << played << " played by " << players[idx]->get_name() << endl;

      if (Card_less(winning_card, played, led, trump)) {
        winning_card = played;
        winning_index = idx;
      }
    }

    cout << players[winning_index]->get_name() << " takes the trick" << endl;
    cout << endl; // extra newline after each trick
    return winning_index;
  }

  void apply_scoring(int maker_index,
                     int team0_tricks, int team1_tricks,
                     int &team0_points, int &team1_points) const {
    // maker_index is guaranteed valid by make_trump()
    const bool maker_team0 = (maker_index % 2 == 0);

    if (maker_team0) {
      if (team0_tricks >= 3) {
        if (team0_tricks == 5) cout << "march!" << endl;
        team0_points += (team0_tricks == 5 ? 2 : 1);
      } else {
        cout << "euchred!" << endl;
        team1_points += 2;
      }
    } else {
      if (team1_tricks >= 3) {
        if (team1_tricks == 5) cout << "march!" << endl;
        team1_points += (team1_tricks == 5 ? 2 : 1);
      } else {
        cout << "euchred!" << endl;
        team0_points += 2;
      }
    }
  }

  void print_scores(int team0_points, int team1_points) const {
    cout << players[0]->get_name() << " and " << players[2]->get_name()
         << " have " << team0_points << " points" << endl;
    cout << players[1]->get_name() << " and " << players[3]->get_name()
         << " have " << team1_points << " points" << endl;
    cout << endl;
  }

  void announce_game_winner(int team0_points, int team1_points) const {
    if (team0_points >= points_to_win) {
      cout << players[0]->get_name() << " and " << players[2]->get_name()
           << " win!" << endl;
    } else {
      cout << players[1]->get_name() << " and " << players[3]->get_name()
           << " win!" << endl;
    }
  }
};

static void print_usage_and_exit() {
  cout << "Usage: euchre.exe PACK_FILENAME [shuffle|noshuffle] "
       << "POINTS_TO_WIN NAME1 TYPE1 NAME2 TYPE2 NAME3 TYPE3 "
       << "NAME4 TYPE4" << endl;
  exit(1);
}

int main(int argc, char **argv) {
  // Expect exactly 12 args (including executable)
  if (argc != 12) print_usage_and_exit();

  const string pack_filename = argv[1];
  const string shuffle_arg  = argv[2];
  const int points_to_win   = atoi(argv[3]);

  if (points_to_win < 1 || points_to_win > 100) print_usage_and_exit();
  if (shuffle_arg != "shuffle" && shuffle_arg != "noshuffle") print_usage_and_exit();

  ifstream pack_file(pack_filename);
  if (!pack_file.is_open()) {
    cout << "Error opening " << pack_filename << endl;
    return 1;
  }

  // Print the command line (with trailing space)
  for (int i = 0; i < argc; ++i) {
    cout << argv[i] << " ";
  }
  cout << endl;

  Pack pack(pack_file);
  const bool do_shuffle = (shuffle_arg == "shuffle");

  vector<Player*> players;
  players.reserve(4);
  for (int i = 0; i < 4; ++i) {
    const string name = argv[4 + i * 2];
    const string type = argv[5 + i * 2];
    if (type != "Simple" && type != "Human") {
      for (Player* p : players) delete p;
      print_usage_and_exit();
    }
    players.push_back(Player_factory(name, type));
  }

  Game game(pack, do_shuffle, points_to_win, players);
  game.play();

  // Clean up players created by Player_factory
  for (Player* p : players) delete p;

  return 0;
}
