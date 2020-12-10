#ifndef TREE_H
#define TREE_H

#include <type_traits>
#include <vector>
#include "game.h"

typedef std::underlying_type<Cards>::type cards_type;
typedef std::underlying_type<Move>::type move_type;

struct Tree {
    int num;
    Tree* children[MAX_MOVES];
    float infoset_regret[MAX_CARDS][MAX_MOVES];
    float infoset_strategy[MAX_CARDS][MAX_MOVES];

    Tree() : children() {}

    int delete_tree() {
        int num_deleted = delete_tree(0);
        delete this;
        return num_deleted;
    }

    int delete_tree(int depth){
        int deleted_children = 0;
        for (Tree* child:children) {
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
        if (children[move] == nullptr)
            children[move] = new Tree();
        return children[move];
    }
};

#endif
