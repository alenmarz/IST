#include <iostream>
#include <set>
#include <chrono>
#include "Tree.h"

void test() {
    std::set<int> set;
    Tree<int> tree;

    for (int i = 0; i < 10000; i++) {
        int digit = rand();
        auto start = std::chrono::high_resolution_clock::now();
        tree.insert(std::make_shared<Element<int>>(digit, digit));
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;

        set.insert(digit);

        std::cout << "Time: "  << elapsed.count() << " N: " << tree.getSize() << std::endl;
    }

    while (true) {
        int digit = rand();
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

int main() {
    test();
    return 0;
}