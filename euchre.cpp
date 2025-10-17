#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include "Pack.hpp"
#include "Player.hpp"
using namespace std;

class Game {
public:
  Game(Pack &pack_in, bool shuffle_in, int points_to_win_in,
       vector<Player*> &players_in)
      : pack(pack_in), shuffle(shuffle_in), points_to_win(points_to_win_in),
        players(players_in), dealer(0), hand_number(0) {}

  void play() {
    int team0_points = 0; // players 0 and 2
    int team1_points = 0; // players 1 and 3

    while (team0_points < points_to_win && team1_points < points_to_win) {
      start_hand_announcement();
      shuffle_or_reset();
      deal();

      Card upcard = turn_up();

      // Make trump
      Suit trump_suit = SPADES; // will be set by make_trump
      int maker_index = -1;
      make_trump(upcard, dealer, trump_suit, maker_index);

      cout << endl;

      // Play hand
      pair<int,int> tricks = play_hand((dealer + 1) % 4, trump_suit);

      // Scoring
      apply_scoring(maker_index, tricks.first, tricks.second,
                    team0_points, team1_points);

      print_scores(team0_points, team1_points);

      dealer = (dealer + 1) % 4;
      hand_number++;
    }

    announce_winner(team0_points, team1_points);
  }

private:
  Pack &pack;
  bool shuffle;
  int points_to_win;
  vector<Player*> players;
  int dealer;
  int hand_number;

  void deal() {
    // Deal in two rounds starting left of dealer: 3-2-3-2 then 2-3-2-3
    const int first_round[4] = {3, 2, 3, 2};
    const int second_round[4] = {2, 3, 2, 3};
    for (int i = 1; i <= 4; ++i) {
      int player_index = (dealer + i) % 4;
      for (int c = 0; c < first_round[i - 1]; ++c) {
        players[player_index]->add_card(pack.deal_one());
      }
    }
    for (int i = 1; i <= 4; ++i) {
      int player_index = (dealer + i) % 4;
      for (int c = 0; c < second_round[i - 1]; ++c) {
        players[player_index]->add_card(pack.deal_one());
      }
    }
  }

  bool make_trump(const Card &upcard, int dealer_index, Suit &trump_suit,
                  int &maker_index) {
    // First round: players may order up the upcard suit; dealer then adds and discards
    for (int i = 1; i <= 4; ++i) {
      int idx = (dealer_index + i) % 4;
      bool is_dealer = (idx == dealer_index);
      if (players[idx]->make_trump(upcard, is_dealer, 1, trump_suit)) {
        cout << players[idx]->get_name() << " orders up " << trump_suit << endl;
        maker_index = idx;
        players[dealer_index]->add_and_discard(upcard);
        return true;
      } else {
        cout << players[idx]->get_name() << " passes" << endl;
      }
    }

    // Second round: players may call a different suit; no adding/discarding
    for (int i = 1; i <= 4; ++i) {
      int idx = (dealer_index + i) % 4;
      bool is_dealer = (idx == dealer_index);
      if (players[idx]->make_trump(upcard, is_dealer, 2, trump_suit)) {
        cout << players[idx]->get_name() << " orders up " << trump_suit << endl;
        maker_index = idx;
        return true;
      } else {
        cout << players[idx]->get_name() << " passes" << endl;
      }
    }
    return false;
  }

  pair<int,int> play_hand(int leader, Suit trump) {
    int team0_tricks = 0;
    int team1_tricks = 0;

    for (int trick = 0; trick < 5; trick++) {
      int winning_index = play_trick_and_get_winner(leader, trump);
      if (winning_index % 2 == 0) team0_tricks++; else team1_tricks++;
      leader = winning_index;
    }

    announce_hand_winner(team0_tricks, team1_tricks);
    return {team0_tricks, team1_tricks};
  }

  int play_trick_and_get_winner(int leader, Suit trump) {
    Card led_card = players[leader]->lead_card(trump);
    cout << led_card << " led by " << players[leader]->get_name() << endl;
    int winning_index = leader;
    Card winning_card = led_card;

    for (int i = 1; i <= 3; i++) {
      int idx = (leader + i) % 4;
      Card played = players[idx]->play_card(led_card, trump);
      cout << played << " played by " << players[idx]->get_name() << endl;
      if (Card_less(winning_card, played, led_card, trump)) {
        winning_card = played;
        winning_index = idx;
      }
    }

    cout << players[winning_index]->get_name() << " takes the trick" << endl;
    cout << endl;
    return winning_index;
  }

  void start_hand_announcement() const {
    cout << "Hand " << hand_number << endl;
    cout << players[dealer]->get_name() << " deals" << endl;
  }

  void shuffle_or_reset() {
    if (shuffle) pack.shuffle(); else pack.reset();
  }

  Card turn_up() {
    Card upcard = pack.deal_one();
    cout << upcard << " turned up" << endl;
    return upcard;
  }

  void apply_scoring(int maker_index,
                     int team0_tricks,
                     int team1_tricks,
                     int &team0_points,
                     int &team1_points) {
    bool maker_team0 = (maker_index % 2 == 0);
    if (maker_team0) {
      if (team0_tricks >= 3) {
        if (team0_tricks == 5) { cout << "march!" << endl; }
        team0_points += (team0_tricks == 5 ? 2 : 1);
      } else {
        team1_points += 2; cout << "euchred!" << endl;
      }
    } else {
      if (team1_tricks >= 3) {
        if (team1_tricks == 5) { cout << "march!" << endl; }
        team1_points += (team1_tricks == 5 ? 2 : 1);
      } else {
        team0_points += 2; cout << "euchred!" << endl;
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

  void announce_hand_winner(int team0_tricks, int team1_tricks) const {
    if (team0_tricks > team1_tricks) {
      cout << players[0]->get_name() << " and " << players[2]->get_name()
           << " win the hand" << endl;
    } else {
      cout << players[1]->get_name() << " and " << players[3]->get_name()
           << " win the hand" << endl;
    }
  }

  void announce_winner(int team0_points, int team1_points) const {
    if (team0_points >= points_to_win) {
      cout << players[0]->get_name() << " and " << players[2]->get_name()
           << " win!" << endl;
    } else {
      cout << players[1]->get_name() << " and " << players[3]->get_name()
           << " win!" << endl;
    }
  }
};

int main(int argc, char **argv) {
  if (argc != 12) {
    cout << "Usage: euchre.exe PACK_FILENAME [shuffle|noshuffle] "
         << "POINTS_TO_WIN NAME1 TYPE1 NAME2 TYPE2 NAME3 TYPE3 "
         << "NAME4 TYPE4" << endl;
    return 1;
  }

  string pack_filename = argv[1];
  string shuffle_arg = argv[2];
  int points_to_win = atoi(argv[3]);
  if (points_to_win < 1 || points_to_win > 100 ||
      (shuffle_arg != "shuffle" && shuffle_arg != "noshuffle")) {
    cout << "Usage: euchre.exe PACK_FILENAME [shuffle|noshuffle] "
         << "POINTS_TO_WIN NAME1 TYPE1 NAME2 TYPE2 NAME3 TYPE3 "
         << "NAME4 TYPE4" << endl;
    return 1;
  }

  ifstream pack_file(pack_filename);
  if (!pack_file.is_open()) {
    cout << "Error opening " << pack_filename << endl;
    return 1;
  }

  // Print all arguments
  for (int i = 0; i < argc; ++i)
    cout << argv[i] << " ";
  cout << endl;

  Pack pack(pack_file);
  bool shuffle = (shuffle_arg == "shuffle");

  vector<Player*> players;
  for (int i = 0; i < 4; i++) {
    string name = argv[4 + i * 2];
    string type = argv[5 + i * 2];
    if (type != "Simple" && type != "Human") {
      cout << "Usage: euchre.exe PACK_FILENAME [shuffle|noshuffle] "
           << "POINTS_TO_WIN NAME1 TYPE1 NAME2 TYPE2 NAME3 TYPE3 "
           << "NAME4 TYPE4" << endl;
      return 1;
    }
    players.push_back(Player_factory(name, type));
  }

  Game game(pack, shuffle, points_to_win, players);
  game.play();

  for (Player* p : players)
    delete p;

  return 0;
}
