
#ifndef HOLDEM_H
#define HOLDEM_H

#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>

struct Player {
    int chips;
    std::vector< std::pair<int, int> > hand;
    std::vector<int> mapping;
    int colours = 0;
};

class Holdem {
    public:
        Holdem(int);
        bool IsFinished();
        int big_blind = 10, small_blind = 5, dealer = 0;
        std::vector<int>  hand_bet;
        unsigned long long board_mask = 0;
        static unsigned long CalculateHighestMask(unsigned long long, int);
        static unsigned long CalculateHandStrength(unsigned long long);
};

#endif
