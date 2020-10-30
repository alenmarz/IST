#pragma once
#ifndef IST_NODE_H
#define IST_NODE_H


#include <vector>
#include "Element.h"

template <typename T>
class Node {
    int m_weight;
    int m_initSize;
    int m_size;
    int m_counter;
    int m_max, m_min;
    std::vector<ElementPtr<T>> m_representatives;
    std::vector<int> m_ids;

    int getStartIndexForSearch(int key);

public:
    Node();
    Node(std::vector<ElementPtr<T>> elements, int size);
    int getSize();
    int getWeight();
    int getMin();
    int getMax();
    int getChildIndex(int key);
    std::vector<ElementPtr<T>> getRepresentatives();
    bool isOverflowing();
    bool isAvailableForInsert();
    void increaseCounter();
    void increaseSize();
    void decreaseSize();
    void increaseWeight();
    void insert(ElementPtr<T> element);
    bool remove(int key);
    T search(int key);
    void print();
};

template <typename T>
using NodePtr = std::shared_ptr<Node<T>>;

#endif //IST_NODE_H
#include "Node.hpp"
