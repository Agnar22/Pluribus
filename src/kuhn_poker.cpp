#include "kuhn_poker.h"
#include <string>
#include <utility>
#include <algorithm>
#include <random>
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
    encountered_infosets.resize(num_players, std::set<uint64_t>());
    card_for_player.reserve(num_players);
    cards = {Cards::A, Cards::K, Cards::Q};
    if (num_players==3)
        cards.emplace_back(Cards::J);
    initialize_hand();
}

void KuhnPoker::reset_game() {
    initialize_hand();
}

void KuhnPoker::initialize_hand() {
    money_in_hand.assign(players, 1.0f);
    has_folded.assign(players, false);
    history.clear();
    player_to_move = 0;
    draw_cards();
}

void KuhnPoker::draw_cards() {
    std::vector<Cards> shuffled_cards = cards;
    std::random_shuffle(shuffled_cards.begin(), shuffled_cards.end());

    for (int x=0; x<players; ++x)
        card_for_player[x] = shuffled_cards[x];
}

inline int KuhnPoker::get_num_players() {
    return players;
}

inline int KuhnPoker::get_player_to_move() {
    return player_to_move;
}

inline uint64_t KuhnPoker::get_current_infoset() {
    return get_infoset(player_to_move);
}

inline bool KuhnPoker::is_player_to_move(int player) {
    return player == player_to_move;
}

inline uint64_t KuhnPoker::get_infoset(int player) {
    return create_infoset(history, card_for_player[player]);
}

inline bool KuhnPoker::is_player_in_hand(int player) {
    return !has_folded[player];
}

inline bool KuhnPoker::is_chance_node() {
    return false;
}

inline int KuhnPoker::betting_round() {
    return 0;
}

void KuhnPoker::execute(Move& action) {
    encountered_infosets[player_to_move].insert(get_infoset(player_to_move));
    history+=action;
    if (action==Move::F)
        has_folded[player_to_move]=true;
    else if (action==Move::C)
        money_in_hand[player_to_move] = *std::max_element(money_in_hand.begin(), money_in_hand.end());
    else
        money_in_hand[player_to_move]+=1;
    player_to_move = (player_to_move+1)%players;
}

void KuhnPoker::undo() {
    player_to_move=(player_to_move-1+players)%players;
    Move last_action = history[0];
    history--;
    if (last_action == Move::F)
        has_folded[player_to_move] = false;
    else
        money_in_hand[player_to_move] = 1.0f;
}

bool KuhnPoker::is_finished() {
    if (std::accumulate(has_folded.begin(), has_folded.end(),0) == players - 1)
        return true;
    if (history.get_length()>=players && history[players-1] == Move::R)
        return true;
    if (history.get_length()>=players && history.get_first_occurence(Move::R) == -1)
        return true;
    return false;
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
}

Cards KuhnPoker::find_best_remaining_hand() {
    Cards best_card = Cards::NONE;
    for (int player=0; player<players; ++player)
        if (is_player_in_hand(player))
            best_card = card_for_player[player] < best_card ? card_for_player[player] : best_card;
    return best_card;
}

std::vector<Move>& KuhnPoker::get_actions(std::vector<Move>& actions) {
    if (std::accumulate(has_folded.begin(), has_folded.end(),0) == players - 1)
        return actions;
    actions.emplace_back(Move::C);
    if (history.get_first_occurence(Move::R) == -1)
        actions.emplace_back(Move::R);
    else
        actions.emplace_back(Move::F);
    actions_for_infoset[get_current_infoset()] = actions;
    return actions;
}

Move KuhnPoker::get_random_action() {
    std::vector<Move> actions;
    actions.reserve(MAX_MOVES);
    actions = get_actions(actions);
    return actions[rand()%actions.size()];
}

inline Move KuhnPoker::sample_action() {
    throw std::runtime_error("Kuhn poker cannot sample action.");
}

inline std::set<uint64_t> KuhnPoker::get_encountered_infosets(int player) {
    return encountered_infosets[player];
}

std::vector<Move> KuhnPoker::get_actions_from_infoset(uint64_t infoset) {
    assertm(actions_for_infoset.find(infoset) != actions_for_infoset.end(), "Infoset is encountered before.");
    return actions_for_infoset[infoset];
}

