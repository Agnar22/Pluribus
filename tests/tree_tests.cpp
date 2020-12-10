#include "gtest/gtest.h"
#include "../src/game.h"
#include "../src/tree.h"


TEST(TreeTest, Destruction) {
    int max_depth = 5;
    int deleted_children;

    Tree* tree = new Tree();
    Tree* leaf = tree;
    for (int depth=0; depth<max_depth; ++depth) {
        leaf = leaf->get_child(0);
    }
    
    deleted_children = tree->delete_tree();
    ASSERT_EQ(deleted_children, 12);
}

TEST(TreeTest, RegretUpdate) {
    Tree* tree = new Tree();

    tree->set_regret(Cards::K, Move::C, -1.0f);

    ASSERT_EQ(tree->get_regret(Cards::K, Move::C), -1.0f);
    tree->delete_tree();
}


TEST(TreeTest, StrategyUpdate) {
    Tree* tree = new Tree();

    tree->set_strategy(Cards::K, Move::C, 2.0f);

    ASSERT_EQ(tree->get_strategy(Cards::K, Move::C), 2.0f);
    tree->delete_tree();
}

TEST(TreeTest, ScopedUpdate) {
    Tree* tree = new Tree();

    {
        Tree* leaf = tree->get_child(0);
        leaf->set_strategy(Cards::K, Move::C, 2.0f);
        leaf->set_regret(Cards::K, Move::C, 1.0f);
    }

    ASSERT_EQ(tree->get_child(0)->get_strategy(Cards::K, Move::C), 2.0f);
    ASSERT_EQ(tree->get_child(0)->get_regret(Cards::K, Move::C), 1.0f);
    tree->delete_tree();
}

