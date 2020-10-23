#include <iostream>
#include "Tree.h"

int main() {
    std::vector<std::shared_ptr<Element<int>>> elements;
    for (int i = 0; i < 20000; i++) {
        int digit = rand() % 1000;
        elements.push_back(std::make_shared<Element<int>>(digit, digit));
    }
    elements.push_back(std::make_shared<Element<int>>(10, 10));
    auto tree = std::make_shared<Tree<int>>(elements);

    tree->print("");
    std::cout << "Size: " << tree->getSize() << std::endl;
    std::cout << "Weight: " << tree->getWeight() << std::endl;
    std::cout << "-------------------------------" << std::endl;

    tree->deleteElement(10);

    tree->print("");
    std::cout << "Size: " << tree->getSize() << std::endl;
    std::cout << "Weight: " << tree->getWeight() << std::endl;

    tree->insertElement(std::make_shared<Element<int>>(1000, 1001));

    std::cout << "RESULT: " << tree->search(1000) << std::endl;

    return 0;
}