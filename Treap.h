#pragma once
#ifndef IST_TREAP_H
#define IST_TREAP_H

#include "TreapNode.h"
#include <tuple>
#include <utility>
#include <iostream>
#include "Element.h"
#include "Action.h"

template <typename T>
class Treap {
    TreapNodePtr<T> m_root;

    std::tuple<TreapNodePtr<T>, TreapNodePtr<T>> SplitBinary(TreapNodePtr<T>&& input, int key);
    std::tuple<TreapNodePtr<T>, TreapNodePtr<T>, TreapNodePtr<T>> Split(TreapNodePtr<T>&& input, int key);
    TreapNodePtr<T>&& Merge(TreapNodePtr<T>&& less, TreapNodePtr<T>&& greater);
    TreapNodePtr<T>&& Merge(TreapNodePtr<T>&& less, TreapNodePtr<T>&& equal, TreapNodePtr<T>&& greater);
    std::tuple<TreapNodePtr<T>, bool> insert(TreapNodePtr<T> node, ElementPtr<T> element);
    std::tuple<TreapNodePtr<T>, std::shared_ptr<std::vector<bool>>> p_execute(TreapNodePtr<T> root, ActionsPtr<T> actions);

    std::tuple<TreapNodePtr<T>, bool> contains(TreapNodePtr<T> root, int key);
    std::tuple<TreapNodePtr<T>, bool> remove(TreapNodePtr<T> root, int key);

public:
    Treap();
    std::shared_ptr<std::vector<bool>> p_execute(ActionsPtr<T> actions);
    bool insert(ElementPtr<T> element);
    bool remove(int key);
    bool contains(int key);
    void print(const std::string& prefix);
};

template <typename T>
using TreapPtr = std::shared_ptr<Treap<T>>;

#endif //IST_TREAP_H
#include "Treap.hpp"