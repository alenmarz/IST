//
// Created by Alena Martsenyuk on 18/10/2020.
//
#pragma once
#include "Node.h"
#include <cmath>
#include <iostream>

int const REBUILD_THRESHOLD = 4;

template <typename T>
Node<T>::Node() :
    m_initSize(0),
    m_size(0),
    m_weight(0),
    m_counter(0),
    m_max(0),
    m_min(0) {
}

template <typename T>
Node<T>::Node(std::vector<std::shared_ptr<Element<T>>> elements, int size) :
    m_representatives(elements),
    m_initSize(size),
    m_size(size),
    m_weight(size),
    m_counter(0),
    m_min(elements.empty() ? 0 : elements[0]->getKey()),
    m_max(elements.empty() ? 0 : elements[elements.size() - 1]->getKey()) {

    int ids_size = elements.size(); // Для простоты
    m_ids = std::vector<int>(ids_size, 0);
    if (m_max - m_min == 0) return;

    for (int i = 0, j = 0; i < ids_size; i++) {
        int ithSearchPosition = m_min + i * (m_max - m_min) / ids_size;
        while (ithSearchPosition > elements[j]->getKey() && j < elements.size() - 1) {
            j++;
        }
        m_ids[i] = j;
    }
}

template <typename T>
bool Node<T>::isOverflowing() {
    return m_counter > m_initSize / REBUILD_THRESHOLD;
}

template <typename T>
void Node<T>::increaseCounter() {
    m_counter++;
}

template <typename T>
void Node<T>::decreaseSize() {
    m_size--;
}

template <typename T>
void Node<T>::increaseSize() {
    m_size++;
}

template <typename T>
void Node<T>::increaseWeight() {
    m_weight++;
}

template <typename T>
int Node<T>::getMin() {
    return m_min;
}

template <typename T>
int Node<T>::getMax() {
    return m_max;
}

template <typename T>
bool Node<T>::isAvailableForInsert() {
    return m_weight <= 1;
}

template <typename T>
void Node<T>::insertElement(std::shared_ptr<Element<T>> element) {
    if (m_representatives.empty() || m_representatives[0]->getKey() <= element->getKey()) {
        m_representatives.push_back(element);
    } else {
        m_representatives.insert(m_representatives.begin(), element);
    }
}

template <typename T>
int Node<T>::getChildIndex(int key) {
    int childIndex = getStartIndexForSearch(key);

    while (childIndex < m_representatives.size() && key >= m_representatives[childIndex]->getKey()) {
        childIndex++;
    }

    return childIndex;
}

template <typename T>
bool Node<T>::deleteElement(int key) {
    if (m_representatives.empty()) return false;

    int index = getStartIndexForSearch(key);
    while (key > m_representatives[index]->getKey() && index < m_representatives.size() - 1) {
        index++;
    }

    if (m_representatives[index]->getKey() == key) {
        m_representatives[index]->mark();
        return true;
    }

    return false;
}

template <typename T>
int Node<T>::getSize() {
    return m_size;
}

template <typename T>
int Node<T>::getWeight() {
    return m_weight;
}

template <typename T>
std::vector<std::shared_ptr<Element<T>>> Node<T>::getRepresentatives() {
    auto copy = m_representatives;
    return copy;
}

template <typename T>
T Node<T>::search(int key) {
    if (m_representatives.empty()) return T{};

    int index = getStartIndexForSearch(key);

    while ((key > m_representatives[index]->getKey() && index < m_representatives.size() - 1)
            || m_representatives[index]->isMarked()) {
        index++;
    }

    if (m_representatives[index]->getKey() == key) {
        return m_representatives[index]->getValue();
    }

    return T{};
}

template <typename T>
int Node<T>::getStartIndexForSearch(int key) {
    int a = getMin();
    int b = getMax();
    int idIndex = (b - a) * (key - a) > 0 ? floor((key - a) / (b - a) * m_ids.size()) : 0;

    int index = 0;
    if (!m_ids.empty() && idIndex < m_ids.size()) {
        index = m_ids[idIndex];
    }

    return index;
}

template <typename T>
void Node<T>::print() {
    for (auto element: m_representatives) {
        char mark = element->isMarked() ? '*' : ' ';
        std::cout << "(" << element->getKey() << ")" << mark;
    }
}