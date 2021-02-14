#pragma once
#include "Treap.h"
#include <memory>

template <typename T>
std::tuple<TreapNodePtr<T>, TreapNodePtr<T>> Treap<T>::SplitBinary(TreapNodePtr<T>&& input, int key) {
    if (!input) {
        return std::make_tuple(nullptr, nullptr);
    } else if (input->getRepresentative()->getKey() < key) {
        auto [less, greater] = SplitBinary(std::move(input->getRight()), key);
        input->setRight(std::move(less));
        return std::make_tuple(std::move(input), std::move(greater));
    } else {
        auto [less, greater] = SplitBinary(std::move(input->getLeft()), key);
        input->setLeft(std::move(greater));
        return std::make_tuple(std::move(less), std::move(input));
    }
}

template <typename T>
std::tuple<TreapNodePtr<T>, TreapNodePtr<T>, TreapNodePtr<T>> Treap<T>::Split(TreapNodePtr<T>&& input, int key) {
    auto [less, greater_or_equal] = SplitBinary(std::move(input), key);
    auto [equal, greater] = SplitBinary(std::move(greater_or_equal), key + 1);
    return std::make_tuple(std::move(less), std::move(equal), std::move(greater));
}

template <typename T>
TreapNodePtr<T>&& Treap<T>::Merge(TreapNodePtr<T>&& less, TreapNodePtr<T>&& greater) {
    if (!less | !greater) {
        return std::move(less ? less : greater);
    } else if (less->getRepresentative()->getPriority() > greater->getRepresentative()->getPriority()) {
        less->setRight(Merge(std::move(less->getRight()), std::move(greater)));
        return std::move(less);
    } else {
        greater->setLeft(Merge(std::move(less), std::move(greater->getLeft())));
        return std::move(greater);
    }
}

template <typename T>
TreapNodePtr<T>&& Treap<T>::Merge(TreapNodePtr<T>&& less, TreapNodePtr<T>&& equal, TreapNodePtr<T>&& greater) {
    return Merge(Merge(std::move(less), std::move(equal)), std::move(greater));
}

template <typename T>
bool Treap<T>::insert(ElementPtr<T> element) {
    auto [less, equal, greater] = Split(std::move(m_root), element->getKey());
    bool has_element = true;
    if (!equal) {
        equal = std::make_shared<TreapNode<T>>(element);
        has_element = false;
    }
    m_root = Merge(std::move(less), std::move(equal), std::move(greater));
    return !has_element;
}

template <typename T>
bool Treap<T>::remove(int key) {
    auto [less, equal, greater] = Split(std::move(m_root), key);
    m_root = Merge(std::move(less), std::move(greater));
    return equal != nullptr;
}

template <typename T>
bool Treap<T>::contains(int key) {
    auto [less, equal, greater] = Split(std::move(m_root), key);
    const bool contains = equal != nullptr;
    m_root = Merge(std::move(less), std::move(equal), std::move(greater));
    return contains;
}

template <typename T>
void Treap<T>::print(const std::string& prefix) {
    m_root->print("");
}