#include "kuhn_poker.h"
#include <string>
#include <utility>
#include <algorithm>
#include <set>
#include <iostream>
#include <chrono>


KuhnPoker::KuhnPoker() {
    encountered_infosets = {std::set<std::string>(), std::set<std::string>()};
    initialize_hand();
}

void KuhnPoker::initialize_hand() {
    money_in_hand[0] = 1.0f;
    money_in_hand[1] = 1.0f;

    has_folded[0] = false;
    has_folded[1] = false;

    history.clear();

    player_to_move = 0;
    draw_cards();
}

void KuhnPoker::reset_game() {
    initialize_hand();
}

bool KuhnPoker::is_finished() {
    if (has_folded[0] || has_folded[1])
        return true;
    if (history.size()>=2 &&
        ((history.end()[-2]=="r" && history.back()=="c") ||
        (history.end()[-2]=="c" && history.back()=="c")))
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
    int opponent = (player_to_move+1)%2;
    if (action=="f")
        has_folded[player_to_move]=true;
    else if (action=="c")
        money_in_hand[player_to_move] = money_in_hand[opponent];
    else
        money_in_hand[player_to_move]+=1;
    player_to_move = opponent;
}

void KuhnPoker::undo() {
    player_to_move=(player_to_move+1)%2;
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
    if (has_folded[0] || has_folded[1])
        return std::vector<std::string>();
    std::vector<std::string> actions{"c"};
    if (history.size()>0 && history.back() == "r")
        actions.push_back("f");
    if (std::find(history.begin(), history.end(), "r") == history.end())
        actions.push_back("r");
    return actions;
}

std::string KuhnPoker::get_random_action() {
    std::vector<std::string> actions = get_actions();
    return actions[rand()%actions.size()];
}

std::string KuhnPoker::get_infoset(int player) {
    std::string concat_hist;
    for (const auto &piece : history) concat_hist += piece;
    return std::to_string(player) + "|" + std::string(1, card_for_player[player]) + "|" + (has_folded[0]?"t":"f") + "|" + (has_folded[1]?"t":"f") + "|" + concat_hist;
}

std::string KuhnPoker::get_current_infoset() {
    return get_infoset(player_to_move);
}

float KuhnPoker::get_outcome_for_player(int player) {
    if (has_folded[player])
        return -money_in_hand[player];
    int opponent = (player+1)%2;
    if (has_folded[opponent])
        return money_in_hand[opponent];
    if (card_for_player[player] != 'K') {
        return card_for_player[player] == 'A' ? money_in_hand[opponent] : -money_in_hand[player];
    }
    return card_for_player[opponent] == 'A' ? -money_in_hand[player] : money_in_hand[opponent];
};

std::vector<std::string> KuhnPoker::get_actions_from_infoset(std::string infoset) {
    if (infoset.back()=='f') {
        return std::vector<std::string>();
    }
    std::vector<std::string> actions{"c"};
    if (infoset.back()=='r')
        actions.push_back("f");
    if (infoset.back()!='r')
        actions.push_back("r");
    return actions;
}

std::vector<int> KuhnPoker::get_players() {
    return std::vector<int>{0,1};
}

std::pair<char, char> KuhnPoker::card_combination_to_chars(int card_combination) {
    //               Player 2
    //              A   K   Q
    //          A   -   0   1
    // Player 1 K   3   -   2
    //          Q   4   5   -
    //
    // I.e Player 1 wins if number<=2

    char one, two;
    if (card_combination < 2)
        one = 'A';
    else
        one = card_combination < 4 ? 'K' : 'Q';
    if (card_combination==0 || card_combination==5)
        two = 'K';
    else
        two = card_combination > 2 ? 'A' : 'Q';
    return std::make_pair(one, two);
}

void KuhnPoker::draw_cards() {
    std::pair<char, char> dCards = card_combination_to_chars(rand()%6);
    card_for_player[0] = dCards.first;
    card_for_player[1] = dCards.second;
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