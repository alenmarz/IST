#pragma once
#include "Node.h"
#include <cmath>
#include <iostream>

double const REBUILD_THRESHOLD = 0.5;

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
Node<T>::Node(std::vector<ElementPtr<T>> elements, int size, int min, int max) :
    m_representatives(elements),
    m_initSize(size),
    m_size(size),
    m_weight(size),
    m_counter(0),
    m_min(min),
    m_max(max) {

    int test = size;
    int tmp = elements.size();
    int ids_size = std::max(test, tmp);

    m_ids = std::vector<int>(ids_size, 0);
    if (m_max - m_min == 0) return;

    for (int i = 0, j = 0; i < ids_size; i++) {
        int ithSearchPosition = m_min + 1.0 * i * (m_max - m_min) / ids_size;
        while (j < elements.size() - 1 && ithSearchPosition > elements[j]->getKey()) {
            j++;
        }
        m_ids[i] = j;
    }
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
int Node<T>::getMin() {
    return m_min;
}

template <typename T>
int Node<T>::getMax() {
    return m_max;
}

template <typename T>
int Node<T>::getChildIndex(int key) {
    int childIndex = getStartIndexForSearch(key);

    while (childIndex > 0 && key < m_representatives[childIndex]->getKey()) {
        childIndex--;
    }

    while (childIndex < m_representatives.size() && key >= m_representatives[childIndex]->getKey()) {
        childIndex++;
    }

    return childIndex;
}

template <typename T>
ElementPtr<T> Node<T>::search(int key) {
    if (m_representatives.empty()) return nullptr;

    int index = getStartIndexForSearch(key);

    while (index > 0
           && (key < m_representatives[index]->getKey())) {
        index--;
    }

    while (index < m_representatives.size() - 1
           && (key > m_representatives[index]->getKey())) {
        index++;
    }

    if (m_representatives[index]->getKey() == key) {
        return m_representatives[index];
    }

    return nullptr;
}

template <typename T>
std::vector<ElementPtr<T>> Node<T>::getRepresentatives() {
    return m_representatives;
}

template <typename T>
bool Node<T>::isOverflowing() {
    return m_counter > m_initSize / REBUILD_THRESHOLD;
}

template <typename T>
bool Node<T>::isAvailableForInsert() {
    return m_weight <= 1;
}

template <typename T>
void Node<T>::increaseCounter() {
    m_counter++;
}

template <typename T>
void Node<T>::increaseSize() {
    m_size++;
}

template <typename T>
void Node<T>::decreaseSize() {
    m_size--;
}

template <typename T>
void Node<T>::increaseWeight() {
    m_weight++;
}

template <typename T>
void Node<T>::insert(ElementPtr<T> element) {
    if (m_representatives.empty() || m_representatives[0]->getKey() <= element->getKey()) {
        m_representatives.push_back(element);
    } else {
        m_representatives.insert(m_representatives.begin(), element);
    }
}

template <typename T>
bool Node<T>::remove(int key) {
    if (m_representatives.empty()) return false;

    int index = getStartIndexForSearch(key);

    while (index > 0
            && (key < m_representatives[index]->getKey() || m_representatives[index]->isMarked())) {
        index--;
    }

    while (index < m_representatives.size() - 1
            && (key > m_representatives[index]->getKey() || m_representatives[index]->isMarked())) {
        index++;
    }

    if (m_representatives[index]->getKey() == key && !m_representatives[index]->isMarked()) {
        m_representatives[index]->mark();
        return true;
    }

    return false;
}

template <typename T>
void Node<T>::print() {
    for (auto element: m_representatives) {
        char mark = element->isMarked() ? '*' : ' ';
        std::cout << "(" << element->getKey() << ")" << mark;
    }
}

template <typename T>
int Node<T>::getStartIndexForSearch(int key) {
    int a = getMin();
    int b = getMax();

    int idIndex = ((b - a) > 0) && ((key - a) > 0) ? floor(1.0 * (key - a) * m_ids.size() / (b - a)) : 0;

    int last = m_ids.size() - 1;
    idIndex = std::min(idIndex, last);

    int index = 0;

    if (!m_ids.empty() && idIndex < m_ids.size()) {
        index = m_ids[idIndex];
    }

    return index;
}

template <typename T>
ElementPtr<T> Node<T>::getByIndex(int index) {
    return m_representatives[index];
}

template <typename T>
int Node<T>::getNodeSize() {
    return m_representatives.size();
}