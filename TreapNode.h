#pragma once
#ifndef IST_TREAPNODE_H
#define IST_TREAPNODE_H
#include "Element.h"
#include <string>

template <typename T>
class TreapNode {
    ElementPtr<T> m_representative;
    std::shared_ptr<TreapNode<T>> m_left, m_right;

public:
    explicit TreapNode(ElementPtr<T> element);
    ElementPtr<T> getRepresentative();
    void setLeft(std::shared_ptr<TreapNode<T>> node);
    std::shared_ptr<TreapNode<T>> getLeft();
    void setRight(std::shared_ptr<TreapNode<T>> node);
    std::shared_ptr<TreapNode<T>> getRight();
    void print(const std::string& prefix);
};

template <typename T>
using TreapNodePtr = std::shared_ptr<TreapNode<T>>;

#endif //IST_TREAPNODE_H
#include "TreapNode.hpp"
