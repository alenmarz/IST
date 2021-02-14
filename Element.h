#pragma once
#ifndef IST_ELEMENT_H
#define IST_ELEMENT_H

#include <memory>

template <typename T>
class Element {
    int m_key;
    T m_value;
    bool mb_marked;
    int m_priority;

public:
    Element(int key, int value);
    int getKey();
    T getValue();
    int getPriority();
    void mark();
    void unmark();
    bool isMarked();
};

template <typename T>
using ElementPtr = std::shared_ptr<Element<T>>;

#endif //IST_ELEMENT_H

#include "Element.hpp"
