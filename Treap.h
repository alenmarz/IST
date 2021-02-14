#pragma once
#ifndef IST_TREAP_H
#define IST_TREAP_H

#include "TreapNode.h"
#include <tuple>
#include <utility>
#include <iostream>

template <typename T>
class Treap {
    TreapNodePtr<T> m_root;

    std::tuple<TreapNodePtr<T>, TreapNodePtr<T>> SplitBinary(TreapNodePtr<T>&& input, int key);
    std::tuple<TreapNodePtr<T>, TreapNodePtr<T>, TreapNodePtr<T>> Split(TreapNodePtr<T>&& input, int key);
    TreapNodePtr<T>&& Merge(TreapNodePtr<T>&& less, TreapNodePtr<T>&& greater);
    TreapNodePtr<T>&& Merge(TreapNodePtr<T>&& less, TreapNodePtr<T>&& equal, TreapNodePtr<T>&& greater);

public:
    bool insert(ElementPtr<T> element);
    bool remove(int key);
    bool contains(int key);
    void print(const std::string& prefix);
};

template <typename T>
using TreapPtr = std::shared_ptr<Treap<T>>;

#endif //IST_TREAP_H
#include "Treap.hpp"