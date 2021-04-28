#pragma once
#ifndef IST_TREAPNODE_H
#define IST_TREAPNODE_H
#include "Element.h"
#include <string>

template <typename T>
class TreapNode {
    ElementPtr<T> m_representative;
    std::shared_ptr<TreapNode<T>> m_left, m_right;
    size_t m_size;

public:
    TreapNode();
    explicit TreapNode(ElementPtr<T> element);
    ElementPtr<T> getRepresentative();
    void setLeft(std::shared_ptr<TreapNode<T>> node);
    std::shared_ptr<TreapNode<T>> getLeft();
    void setRight(std::shared_ptr<TreapNode<T>> node);
    std::shared_ptr<TreapNode<T>> getRight();
    size_t getSize();
    void setSize(size_t size);
    void print(const std::string& prefix);
};

template <typename T>
using TreapNodePtr = std::shared_ptr<TreapNode<T>>;

#endif //IST_TREAPNODE_H
#include "TreapNode.hpp"
