#include <iostream>
#include "Tree.h"

int main() {
    std::vector<Element<int>*> elements;
    for (int i = 0; i < 200; i++) {
        int digit = rand() % 1000;
        elements.push_back(new Element<int>(digit, digit));
    }
    elements.push_back(new Element<int>(10, 10));
    auto tree = new Tree<int>(elements);

    tree->print("");
    std::cout << "Size: " << tree->getSize() << std::endl;
    std::cout << "Weight: " << tree->getWeight() << std::endl;
    std::cout << "-------------------------------" << std::endl;

    tree->deleteElement(10);

    tree->print("");
    std::cout << "Size: " << tree->getSize() << std::endl;
    std::cout << "Weight: " << tree->getWeight() << std::endl;

    tree->insertElement(new Element<int>(1000, 1001));
    //tree->deleteElement(1000);

    std::cout << "RESULT: " << tree->search(1000) << std::endl;

    return 0;
}