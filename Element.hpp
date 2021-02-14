#pragma once
#include "Element.h"

template <typename T>
Element<T>::Element(int key, int value) : m_key(key), m_value(value), mb_marked(false) {
    m_priority = rand() % 100;
}

template <typename T>
int Element<T>::getKey() {
    return m_key;
}

template <typename T>
T Element<T>::getValue() {
    return m_value;
}

template <typename T>
int Element<T>::getPriority() {
    return m_priority;
}

template <typename T>
void Element<T>::mark() {
    mb_marked = true;
}

template <typename T>
void Element<T>::unmark() {
    mb_marked = false;
}

template <typename T>
bool Element<T>::isMarked() {
    return mb_marked;
}