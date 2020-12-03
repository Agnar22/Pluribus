#include <string>
#include <utility>
#include <algorithm>
#include <set>
#include <iostream>
#include <chrono>

class KuhnPoker: public Game {

    public:
        int player_to_move = 0;
        //float money[2] = {0.0f, 0.0f};
        float money_in_hand[2] = {0.0f, 0.0f};
        bool has_folded[2] = {false, false};
        char card_for_player[2] = {'-','-'};
        //char cards[3] = {'A', 'K', 'Q'};
        std::vector<std::string> history;
        std::vector<std::set<std::string>> encountered_infosets;

        KuhnPoker(){initialize_hand();}

        void initialize_hand() {
            money_in_hand[0] = 1.0f;
            money_in_hand[1] = 1.0f;

            has_folded[0] = false;
            has_folded[1] = false;

            history.clear();

            player_to_move = 0;
            draw_cards();
        }

        void reset_game() {
            initialize_hand();

            //money[0] = 0.0f;
            //money[1] = 0.0f;
        }

        bool is_finished() {
            if (has_folded[0] || has_folded[1])
                return true;
            if (history.size()>=2 &&
               ((history.end()[-2]=="r" && history.back()=="c") ||
               (history.end()[-2]=="c" && history.back()=="c")))
                return true;
            return false;
        }

        std::set<std::string> get_encountered_infosets(int player) {
            return encountered_infosets[player];
        }

        void execute(std::string action) {
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

        void undo() {
            player_to_move=(player_to_move+1)%2;
            std::string last_action = history.back();
            history.pop_back();
            if (last_action == "f") {
                has_folded[player_to_move] = false;
            } else {
                money_in_hand[player_to_move] = 1.0f;
            }
        }

        bool is_player_to_move(int player) {
            return player == player_to_move;
        }

        std::vector<std::string> get_actions() {
            if (has_folded[0] || has_folded[1])
                return std::vector<std::string>();
            std::vector<std::string> actions{"f", "c"};
            if (std::find(history.begin(), history.end(), "r") == history.end())
                actions.push_back("r");
            return actions;
        }

        std::string get_random_action() {
            std::vector<std::string> actions = get_actions();
            return actions[rand()%actions.size()];
        }

        std::string get_infoset(int player) {
            return "";
        }

        std::string get_current_infoset() {
            return get_infoset(player_to_move);
        }

        float get_outcome_for_player(int player) {
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

        static std::vector<std::string> get_actions_from_infoset(std::string infoset) {
            if (infoset.back()=='f') {
                return std::vector<std::string>();
            }
            std::vector<std::string> actions{"f", "c"};
            if (infoset.back()!='r')
                actions.push_back("r");
            return actions;
        }

        std::vector<int> get_players() {
            return std::vector<int>{0,1};
        }
    
    private:
        std::pair<char, char> card_combination_to_chars(int card_combination) {
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

        void draw_cards() {
            std::pair<char, char> dCards = card_combination_to_chars(rand()%6);
            card_for_player[0] = dCards.first;
            card_for_player[1] = dCards.second;
        }
};

void play_khun_poker(int iterations, bool verbose){
    KuhnPoker game;
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
        game.initialize_hand();
    }
}

int main(){
    KuhnPoker game;
    
    auto start = std::chrono::high_resolution_clock::now();
    play_khun_poker(10000, false);
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << std::endl;
    //while (true) {
    //    std::cout << std::endl << std::endl << "Starting game" << std::endl;
    //    std::cout << game.card_for_player[0] << " " << game.card_for_player[1] << std::endl;
    //    while (!game.is_finished()) {
    //        std::cout << "Possible actions: ";
    //        for (std::string action:game.get_actions())
    //            std::cout << action << " ";
    //        std::cout << std::endl << "Move:";
    //        std::string move;
    //        std::cin >> move;
    //        game.execute(move);
    //        std::cout << "In for: " << game.money_in_hand[0] << " " << game.money_in_hand[1] << std::endl;
    //    }
    //    std::cout << "Game over. Outcome: " << game.get_outcome_for_player(0) << " " << game.get_outcome_for_player(1) << std::endl;
    //    game.initialize_hand();
    //}
}