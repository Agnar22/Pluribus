#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <experimental/filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>


unsigned long long compress_hand_lossless(unsigned long long);

namespace calculations {

    extern std::unordered_map< unsigned long long, int > suit_permutations;
    extern std::unordered_map< int, std::vector<int> > hand_frequencies;
    void load_suit_permutations();
    int num_suit_permutations(unsigned long long);
    void hand_frequency_(std::vector<int>&, unsigned long long, int);
    std::vector<int> hand_frequency(unsigned long long);

}

#endif
