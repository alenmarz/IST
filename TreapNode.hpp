#pragma once

#include <iostream>
#include "TreapNode.h"

template <typename T>
TreapNode<T>::TreapNode() :
        m_representative(nullptr),
        m_left(nullptr),
        m_right(nullptr),
        m_size(0) {
}

template <typename T>
TreapNode<T>::TreapNode(ElementPtr<T> element) :
        m_representative(element),
        m_left(nullptr),
        m_right(nullptr),
        m_size(1) {
}

template <typename T>
ElementPtr<T> TreapNode<T>::getRepresentative() {
    return m_representative;
}

template <typename T>
void TreapNode<T>::setLeft(TreapNodePtr<T> node) {
    m_left = node;
}

template <typename T>
TreapNodePtr<T> TreapNode<T>::getLeft() {
    return m_left;
}

template <typename T>
void TreapNode<T>::setRight(TreapNodePtr<T> node) {
    m_right = node;
}

template <typename T>
TreapNodePtr<T> TreapNode<T>::getRight() {
    return m_right;
}

template <typename T>
size_t TreapNode<T>::getSize() {
    return m_size;
}

template <typename T>
void TreapNode<T>::setSize(size_t size) {
    m_size = size;
}

template <typename T>
void TreapNode<T>::print(const std::string& prefix) {
    std::cout << prefix << "└──";

    std::cout << "(" << m_representative->getPriority() << "|" << m_representative->getKey() << ")";
    std::cout << std::endl;

    if (m_left) m_left->print(prefix + "l│   ");
    if (m_right) m_right->print(prefix + "r│   ");
}