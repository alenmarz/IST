#include <iostream>
#include <set>
#include <chrono>
#include <memory>
#include "Treap.h"
#include "Tree.h"

void correctnessTest() {
    std::set<int> set;
    Tree<int> tree;

    for (int digit = 0; digit < 100000; digit++) {
        if (rand() % 2) {
            tree.insert(std::make_shared<Element<int>>(digit, digit));
            set.insert(digit);
        }
    }

    tree.rebuild();
    tree.print("");


    while (true) {
        int digit = rand() % 100000;
        switch (rand() % 3) {
            case 0:
                if (set.insert(digit).second != tree.insert(std::make_shared<Element<int>>(digit, digit))) {
                    std::cout << "Insert Error!" << std::endl;
                    tree.getSize();
                }
                break;
            case 1:
                if (set.erase(digit) != tree.remove(digit)) {
                    std::cout << "Remove Error!" << std::endl;
                    tree.getSize();
                }
                break;
            case 2:
                bool setContains = set.find(digit) != set.end();
                if (setContains != tree.contains(digit)) {
                    std::cout << "Contains Error!" << tree.getSize() << std::endl;
                }
                break;
        }
    }
}

void correctnessTest1() {
    std::set<int> set;
    Treap<int> tree;

    for (int digit = 0; digit < 15; digit++) {
        if (rand() % 2) {
            tree.insert(std::make_shared<Element<int>>(digit, digit));
            set.insert(digit);
        }
    }

    tree.print("");
    std::cout << "---------" << std::endl;

    auto vect = std::make_shared<Actions<int>>();
    vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(0, 0), Contains));
    vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(1, 1), Remove));
    vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(2, 2), Contains));
    vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(8, 8), Remove));
    vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(10, 10), Insert));
    vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(20, 20), Insert));

    auto result = tree.p_execute(vect);
    tree.print("");
    for (auto a: *result) {
        std::cout << a << " ";
    }
    std::cout << std::endl;

    /*while (int i = 0; i < 1000; i++) {
        int digit = rand() % 100;
        switch (rand() % 3) {
            case 0:
                if (set.insert(digit).second != tree.insert(std::make_shared<Element<int>>(digit, digit))) {
                    std::cout << "Insert Error!" << std::endl;
                }
                break;
            case 1:
                if (set.erase(digit) != tree.remove(digit)) {
                    std::cout << "Remove Error!" << std::endl;
                }
                break;
            case 2:
                bool setContains = set.find(digit) != set.end();
                if (setContains != tree.contains(digit)) {
                    std::cout << "Contains Error!" << std::endl;
                }
                break;
        }
    }*/
}

void test1() {
    Treap<int> treap;

    for (int digit = 0; digit < 100; digit++) {
        std::cout << digit << " " << treap.insert(std::make_shared<Element<int>>(digit, digit)) << std::endl;
    }

    treap.print("");
}

void test() {
    std::set<int> set;
    Tree<int> tree;

    for (int digit = 0; digit < 5000000; digit++) {
        if (rand() % 2) {
            tree.insert(std::make_shared<Element<int>>(digit, digit));
            set.insert(digit);
        }
    }

    //tree.rebuild();
    int max_depth = 0;
    long total_depth = 0;

    srand(1);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000000; i++) {
        int digit = rand() % 5000000;
        //int depth = 0;
        //tree.contains(digit, &depth);
        tree.contains(digit);
        //total_depth += depth;
        //max_depth = std::max(max_depth, depth);
        /*switch (rand() % 3) {
            case 0:
                tree.insert(std::make_shared<Element<int>>(digit, digit));
                //std::cout<<"insert" <<std::endl;
                break;
            case 1:
                tree.remove(digit);
                //std::cout<<"remove" <<std::endl;
                break;
            case 2:
                tree.contains(digit);
                //std::cout<<"contains" <<std::endl;
                break;
        }*/
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Time for tree: " << elapsed.count() << std::endl;
    //std::cout << "max depth: " << max_depth << std::endl;
    //std::cout << "avg: " << total_depth / 10000000 << std::endl;

    //tree.print("");

    srand(1);
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000000; i++) {
        int digit = rand() % 5000000;
        set.find(digit);
        /*switch (rand() % 3) {
        //switch (0) {
            case 0:
                set.insert(digit);
                break;
            case 1:
                set.erase(digit);
                break;
            case 2:
                set.find(digit);
                break;
        }*/
    }

    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - start;
    std::cout << "Time for set: " << elapsed.count() << std::endl;
}

void test2() {
    Treap<int> treap;
    Tree<int> tree;

    for (int digit = 0; digit < 5000000; digit++) {
        if (rand() % 2) {
            tree.insert(std::make_shared<Element<int>>(digit, digit));
            treap.insert(std::make_shared<Element<int>>(digit, digit));
        }
    }

    //tree.rebuild();
    int max_depth = 0;
    long total_depth = 0;

    srand(1);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000000; i++) {
        int digit = rand() % 5000000;
        //int depth = 0;
        //tree.contains(digit, &depth);
        //tree.contains(digit);
        //total_depth += depth;
        //max_depth = std::max(max_depth, depth);
        switch (rand() % 3) {
            case 0:
                tree.insert(std::make_shared<Element<int>>(digit, digit));
                //std::cout<<"insert" <<std::endl;
                break;
            case 1:
                tree.remove(digit);
                //std::cout<<"remove" <<std::endl;
                break;
            case 2:
                tree.contains(digit);
                //std::cout<<"contains" <<std::endl;
                break;
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Time for tree: " << elapsed.count() << std::endl;
    //std::cout << "max depth: " << max_depth << std::endl;
    //std::cout << "avg: " << total_depth / 10000000 << std::endl;

    //tree.print("");

    srand(1);
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000000; i++) {
        int digit = rand() % 5000000;
        //treap.contains(digit);
        switch (rand() % 3) {
            case 0:
                treap.insert(std::make_shared<Element<int>>(digit, digit));
                //std::cout<<"insert" <<std::endl;
                break;
            case 1:
                treap.remove(digit);
                //std::cout<<"remove" <<std::endl;
                break;
            case 2:
                treap.contains(digit);
                //std::cout<<"contains" <<std::endl;
                break;
        }
    }

    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - start;
    std::cout << "Time for treap: " << elapsed.count() << std::endl;
}

int main() {
    //test2();
    correctnessTest1();
    return 0;
}