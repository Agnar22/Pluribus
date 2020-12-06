#include "kuhn_poker.h"
#include <string>
#include <utility>
#include <algorithm>
#include <numeric>
#include <set>
#include <iostream>
#include <chrono>
#include <cassert>

#define assertm(exp, msg) assert(((void)msg, exp))

KuhnPoker::KuhnPoker() {
    KuhnPoker(2);
}

KuhnPoker::KuhnPoker(int num_players) {
    assertm(num_players == 2 || num_players == 3, "There are two or three players.");
    players=num_players;
    encountered_infosets.resize(num_players, std::set<std::string>());
    card_for_player.resize(num_players);
    cards = {'A', 'K', 'Q'};
    if (num_players==3)
        cards.push_back('J');
    initialize_hand();
}

void KuhnPoker::initialize_hand() {
    money_in_hand.assign(players, 1.0f);
    has_folded.assign(players, false);
    history.clear();
    player_to_move = 0;
    draw_cards();
}

void KuhnPoker::reset_game() {
    initialize_hand();
}

bool KuhnPoker::is_finished() {
    if (std::accumulate(has_folded.begin(), has_folded.end(),0) == players - 1)
        return true;
    if (history.size()>=players && history[history.size()-players] == "r")
        return true;
    if (history.size()>=players && std::find(history.begin(), history.end(), "r") == history.end())
        return true;
    return false;
}

bool KuhnPoker::is_chance_node() {
    return false;
}

int KuhnPoker::betting_round() {
    return 0;
}

std::string KuhnPoker::sample_action() {
    throw std::runtime_error("Kuhn poker cannot sample action.");
}

bool KuhnPoker::is_player_in_hand(int player) {
    return !has_folded[player];
}

int KuhnPoker::get_player_to_move() {
    return player_to_move;
}

std::set<std::string> KuhnPoker::get_encountered_infosets(int player) {
    return encountered_infosets[player];
}

void KuhnPoker::execute(std::string action) {
    encountered_infosets[player_to_move].insert(get_infoset(player_to_move));
    history.push_back(action);
    if (action=="f")
        has_folded[player_to_move]=true;
    else if (action=="c") {
        money_in_hand[player_to_move] = *std::max_element(money_in_hand.begin(), money_in_hand.end());
    }
    else
        money_in_hand[player_to_move]+=1;
    player_to_move = (player_to_move+1)%players;
}

void KuhnPoker::undo() {
    player_to_move=(player_to_move-1+players)%players;
    std::string last_action = history.back();
    history.pop_back();
    if (last_action == "f") {
        has_folded[player_to_move] = false;
    } else {
        money_in_hand[player_to_move] = 1.0f;
    }
}

bool KuhnPoker::is_player_to_move(int player) {
    return player == player_to_move;
}

std::vector<std::string> KuhnPoker::get_actions() {
    if (std::accumulate(has_folded.begin(), has_folded.end(),0) == players - 1)
        return std::vector<std::string>();
    std::vector<std::string> actions{"c"};
    if (std::find(history.begin(), history.end(), "r") == history.end())
        actions.push_back("r");
    else
        actions.push_back("f");
    return actions;
}

std::string KuhnPoker::get_random_action() {
    std::vector<std::string> actions = get_actions();
    return actions[rand()%actions.size()];
}

std::string KuhnPoker::get_infoset(int player) {
    std::string concat_hist;
    for (const auto &piece : history) concat_hist += piece;
    std::string infoset = std::to_string(player) + "|" + std::string(1, card_for_player[player]) + "|" + concat_hist;
    if (actions_for_infoset.find(infoset) == actions_for_infoset.end())
        actions_for_infoset[infoset] = get_actions();
    return infoset;
}

std::string KuhnPoker::get_current_infoset() {
    return get_infoset(player_to_move);
}

float KuhnPoker::get_outcome_for_player(int player) {
    assertm(is_finished(), "The game is finished.");
    if (has_folded[player])
        return -money_in_hand[player];
    if (std::accumulate(has_folded.begin(), has_folded.end(),0) == players - 1)
        return std::accumulate(money_in_hand.begin(), money_in_hand.end(),0.0f) - money_in_hand[player];
    if (card_for_player[player] != find_best_remaining_hand()) {
        return -money_in_hand[player];
    }
    return std::accumulate(money_in_hand.begin(), money_in_hand.end(),0.0f) - money_in_hand[player];
};

std::vector<std::string> KuhnPoker::get_actions_from_infoset(std::string infoset) {
    assertm(actions_for_infoset.find(infoset) != actions_for_infoset.end(), "Infoset is encountered before.");
    return actions_for_infoset[infoset];
}

std::vector<int> KuhnPoker::get_players() {
    std::vector<int> all_players(players);
    for (int x=0; x<players; ++x)
        all_players[x] = x;
    return all_players;
}

char KuhnPoker::find_best_remaining_hand() {
    int best_card_pos = -1;
    for (int player:get_players()) {
        if (!is_player_in_hand(player))
            continue;
        int curr_card_pos = std::find(cards.begin(), cards.end(), card_for_player[player]) - cards.begin();
        // Expoiting the fact that cards are in descending order.
        best_card_pos = (best_card_pos==-1) ? curr_card_pos:std::min(curr_card_pos, best_card_pos);
    }
    return cards[best_card_pos];
}

void KuhnPoker::draw_cards() {
    std::vector<char> remaining_cards = cards;
    for (int x=0; x<players; ++x) {
        int card_pos = rand()%remaining_cards.size();
        card_for_player[x] = remaining_cards[card_pos];
        remaining_cards.erase(remaining_cards.begin()+card_pos);
    }
}

void play_khun_poker(int iterations, bool verbose){
    KuhnPoker game;
    int num_won = 0;
    for (int x=0; x<iterations; ++x) {
        if (verbose) {
            std::cout << std::endl << std::endl << "Starting game" << std::endl;
            std::cout << game.card_for_player[0] << " " << game.card_for_player[1] << std::endl;
        }
        while (!game.is_finished()) {
            if (verbose) {
                std::cout << "Possible actions: ";
                for (std::string action:game.get_actions())
                    std::cout << action << " ";
                std::cout << std::endl << "Move:";
                std::string move;
                std::cin >> move;
                game.execute(move);
                std::cout << "In for: " << game.money_in_hand[0] << " " << game.money_in_hand[1] << std::endl;
            } else {
                game.execute(game.get_random_action());
            }

        }
        if (verbose)
            std::cout << "Game over. Outcome: " << game.get_outcome_for_player(0) << " " << game.get_outcome_for_player(1) << std::endl;
        num_won += game.get_outcome_for_player(0) > 0.0f ? 1:0;
        game.initialize_hand();
    }
    std::cout << "Won: " << num_won << std::endl;
}

/*
int main(){
    KuhnPoker game;
    
    auto start = std::chrono::high_resolution_clock::now();
    play_khun_poker(10000, false);
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << std::endl;
}
*/