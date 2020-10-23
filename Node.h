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
    std::vector<Element<T> *> m_representatives;
    std::vector<int> m_ids;

public:
    Node();
    Node(std::vector<Element<T>*> elements, int size);
    bool isOverflowing();
    void increaseCounter();
    void increaseSize();
    void decreaseSize();
    void increaseWeight();
    int getMin();
    int getMax();
    int getSize();
    int getWeight();
    std::vector<Element<T> *> getRepresentatives();
    bool isAvailableForInsert();
    void insert(Element<T> *element);
    int getChildIndex(int key);
    bool deleteElement(int key);
    T search(int key);
    void print();
};

#endif //IST_NODE_H
#include "Node-inl.h"
