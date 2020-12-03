#include <iostream>
#include <set>
#include <chrono>
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

void test() {
    std::set<int> set;
    Tree<int> tree;

    for (int digit = 0; digit < 5000000; digit++) {
        if (rand() % 2) {
            tree.insert(std::make_shared<Element<int>>(digit, digit));
            set.insert(digit);
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
        srand(i);
        int digit = rand() % 5000000;
        tree.contains(digit);
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

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
        srand(i);
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

int main() {
    test();
    //correctnessTest();
    return 0;
}