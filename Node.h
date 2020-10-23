//
// Created by Alena Martsenyuk on 18/10/2020.
//
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
    std::vector<std::shared_ptr<Element<T>>> m_representatives;
    std::vector<int> m_ids;

    int getStartIndexForSearch(int key);

public:
    Node();
    Node(std::vector<std::shared_ptr<Element<T>>> elements, int size);
    int getSize();
    int getWeight();
    int getMin();
    int getMax();
    int getChildIndex(int key);
    std::vector<std::shared_ptr<Element<T>>> getRepresentatives();
    bool isOverflowing();
    bool isAvailableForInsert();
    void increaseCounter();
    void increaseSize();
    void decreaseSize();
    void increaseWeight();
    void insertElement(std::shared_ptr<Element<T>> element);
    bool deleteElement(int key);
    T search(int key);
    void print();
};

#endif //IST_NODE_H
#include "Node-inl.h"
