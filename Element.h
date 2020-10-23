//
// Created by Alena Martsenyuk on 18/10/2020.
//
#pragma once
#ifndef IST_ELEMENT_H
#define IST_ELEMENT_H

template <typename T>
class Element {
    int m_key;
    T m_value;
    bool mb_marked;

public:
    Element(int key, int value);
    int getKey();
    T getValue();
    void mark();
    bool isMarked();
};

#endif //IST_ELEMENT_H

#include "Element-inl.h"
