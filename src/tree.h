#ifndef TREE_H
#define TREE_H

#include <type_traits>
#include <vector>
#include "game.h"

typedef std::underlying_type<Cards>::type cards_type;
typedef std::underlying_type<Move>::type move_type;

int num_nodes=0;

struct Tree {
    int num;
    Tree* children[MAX_MOVES];
    float infoset_regret[MAX_CARDS][MAX_MOVES];
    float infoset_strategy[MAX_CARDS][MAX_MOVES];

    Tree() : children() {
        //std::cout << "Initializing tree" << std::endl;
        num = num_nodes++;
    }

    ~Tree() {
        //std::cout << "Destructing tree" << std::endl;
        //for (int x=0; x<MAX_MOVES; ++x) {
            //std::cout << "iterating through children" << x << std::endl;
            //if (children[x] != nullptr) {
                //std::cout << "deleting child" << std::endl;
                //delete children[x];
                //std::cout << "deleted child" << std::endl;
            //}
        //}
    }

    int delete_tree() {
        int num_deleted = delete_tree(0);
        delete this;
        return num_deleted;
    }

    int delete_tree(int depth){
        //std::cout << "Deleting children for " << num << std::endl;
        int deleted_children = 0;
        for (Tree* child:children) {
            //std::cout << "Deleted child for " << num << std::endl;
            if (child != nullptr)
                deleted_children += child->delete_tree(depth+1);
            delete child;
            deleted_children++;
        }
        return deleted_children;
    }


    inline float get_regret(Cards card, Move move) {
        return infoset_regret[static_cast<cards_type>(card)][static_cast<move_type>(move)];
    }

    inline void set_regret(Cards card, Move move, float regret) {
        infoset_regret[static_cast<cards_type>(card)][static_cast<move_type>(move)] = regret;
    }

    inline float get_strategy(Cards card, Move move) {
        return infoset_strategy[static_cast<cards_type>(card)][static_cast<move_type>(move)];
    }

    inline void set_strategy(Cards card, Move move, float strategy) {
        infoset_strategy[static_cast<cards_type>(card)][static_cast<move_type>(move)] = strategy;
    }

    Tree* get_child(int move) {
        if (children[move] == nullptr) {
            children[move] = new Tree();
            //std::cout << "Created ";
            //std::cout << num << std::endl;
        } else {
            //std::cout << "not created" << std::endl;
         }
        return children[move];
    }
/*
    Tree* operator[](int move) {
        if (children[move] != nullptr)
            children[move] = new Tree();
        return children[move];
    }
*/
};

#endif
