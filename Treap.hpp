#pragma once
#include "Treap.h"
#include <memory>
#include <vector>
#include <spgranularity.hpp>

template <typename T>
Treap<T>::Treap() :
    m_root(nullptr) {
}

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
std::tuple<TreapNodePtr<T>, bool> Treap<T>::insert(TreapNodePtr<T> node, ElementPtr<T> element) {
    auto [less, equal, greater] = Split(std::move(node), element->getKey());
    bool has_element = true;
    if (!equal) {
        equal = std::make_shared<TreapNode<T>>(element);
        has_element = false;
    }
    node = Merge(std::move(less), std::move(equal), std::move(greater));
    return std::make_tuple(std::move(node), !has_element);
}

template <typename T>
std::tuple<TreapNodePtr<T>, std::shared_ptr<std::vector<bool>>> Treap<T>::p_execute(TreapNodePtr<T> root, ActionsPtr<T> actions) {
    if (actions == nullptr || actions->empty()) {
        return std::make_tuple(std::move(root), nullptr);
    }

    auto [v1, mid, v2] = split_vector(std::move(actions));
    auto [less, equal, greater] = Split(std::move(root), mid->at(0)->getKey());

    auto res1 = std::make_shared<std::vector<bool>>();
    auto res2 = std::make_shared<std::vector<bool>>();

    if (v1->size() == 0 && v2->size() == 0) {
        auto mid_type = mid->at(0)->getType();
        if (mid_type == Remove) {
            res1->push_back(equal != nullptr);
            root = Merge(std::move(less), std::move(greater));
        } else if (mid_type == Insert) {
            if (equal != nullptr) {
                res1->push_back(false);
                root = Merge(std::move(less), std::move(equal), std::move(greater));
            } else {
                res1->push_back(true);
                auto newNode = std::make_shared<TreapNode<T>>(mid->at(0)->getElement());
                root = Merge(std::move(less), std::move(newNode), std::move(greater));
            }
        } else if (mid_type == Contains) {
            res1->push_back(equal != nullptr);
            root = Merge(std::move(less), std::move(equal), std::move(greater));
        }

        return std::make_tuple(std::move(root), std::move(res1));
    }

    TreapNodePtr<T> less_;
    TreapNodePtr<T> greater_;

    // send middle element to the right subtree
    if (equal == nullptr) {
        v2->insert(v2->begin(), mid->at(0));
    }

    sptl::fork2([&, v1 = v1, less = less] {
        auto [node, success_vec] = p_execute(std::move(less), std::move(v1));
        less_ = std::move(node);
        res1 = std::move(success_vec);
    }, [&, v2 = v2, greater = greater] {
        auto [node, success_vec] = p_execute(std::move(greater), std::move(v2));
        greater_ = std::move(node);
        res2 = std::move(success_vec);
    });

    less = std::move(less_);
    greater = std::move(greater_);

    if (equal != nullptr) {
        auto mid_type = mid->at(0)->getType();
        if (mid_type == Remove) {
            res1->push_back(true);
            root = Merge(std::move(less), std::move(greater));
        } else if (mid_type == Insert) {
            res1->push_back(false);
            root = Merge(std::move(less), std::move(equal), std::move(greater));
        } else if (mid_type == Contains) {
            res1->push_back(true);
            root = Merge(std::move(less), std::move(equal), std::move(greater));
        }
    } else {
        root = Merge(std::move(less), std::move(equal), std::move(greater));
    }

    //TODO: важно -- исходим из предположения, что средний элемент один!
    // объединяем 2 вектора
    if (res1 == nullptr) {
        res1 = std::move(res2);
    } else if (res2 != nullptr) {
        for (auto elem: *res2) {
            res1->push_back(elem);
        }
    }

    return std::make_tuple(std::move(root), std::move(res1));
}

template <typename T>
std::shared_ptr<std::vector<bool>> Treap<T>::p_execute(ActionsPtr<T> actions) {
    auto [node, result] = p_execute(m_root, std::move(actions));
    m_root = std::move(node);
    return std::move(result);
}

template <typename T>
std::tuple<ActionsPtr<T>, ActionsPtr<T>, ActionsPtr<T>> split_vector(ActionsPtr<T> actions) {
    auto v1 = std::make_shared<Actions<T>>();
    auto v2 = std::make_shared<Actions<T>>();
    auto eq = std::make_shared<Actions<T>>();

    if (actions == nullptr || actions->empty()) {
        return std::make_tuple(std::move(v1), std::move(eq), std::move(v2));
    }

    auto middle_key = actions->at(actions->size() / 2)->getKey();
    for (auto action: *actions) {
        if (action->getKey() < middle_key) {
            v1->push_back(std::move(action));
        } else if (action->getKey() == middle_key) {
            eq->push_back(std::move(action));
        } else {
            v2->push_back(std::move(action));
        }
    }

    return std::make_tuple(std::move(v1), std::move(eq), std::move(v2));
};

/*template <typename T>
std::tuple<TreapNodePtr<T>, std::vector<bool>> Treap<T>::p_insert(TreapNodePtr<T> root, ElementsPtr<T> elements) {
    bool left_is_empty = root->getLeft() == nullptr;
    bool right_is_empty = root->getRight() == nullptr;

    std::vector<bool> res1, res2;
    auto [v1, v2] = split_vector(std::move(elements), root->getRepresentative()->getKey());

    if (left_is_empty && !v1->empty()) {
        // TODO: а если его нельзя вставить?
        size_t i = 0;
        for (; i < v1->size(); i++) {
            if (v1->at(i)->getKey() == root->getRepresentative()->getKey()) {
                res1.push_back(false);
            } else {
                root->setLeft(std::make_shared<TreapNode<T>>(v1->at(i)));
                res1.push_back(true);
                i++;
                break;
            }
        }
        //root->setLeft(std::make_shared<TreapNode<T>>(v1->at(0)));
        //res1.push_back(true);

        for (; i < v1->size(); i++) {
            //res1.push_back(root->getLeft()->insert(v1.at(i)));
            auto [node, success] = insert(std::move(root->getLeft()), v1->at(i));
            if (success) root->setLeft(std::move(node));
            res1.push_back(success);
        }
    }

    if (right_is_empty && !v2->empty()) {
        // TODO: а если его нельзя вставить? или пустой!
        root->setRight(std::make_shared<TreapNode<T>>(v2->at(0)));
        res2.push_back(true);

        for (size_t i = 1; i < v2->size(); i++) {
            auto [node, success] = insert(std::move(root->getRight()), v2->at(i));
            if (success) root->setRight(std::move(node));
            res2.push_back(success);
        }
    }

    if (!left_is_empty && !right_is_empty && !v1->empty() && !v2->empty()) {
        sptl::fork2([&, v1 = v1] {
            auto [node, success_vec] = p_insert(std::move(root->getLeft()), std::move(v1));
            root->setLeft(std::move(node));
            res1 = success_vec;
        }, [&, v2 = v2] {
            auto [node, success_vec] = p_insert(std::move(root->getRight()), std::move(v2));
            root->setRight(std::move(node));
            res2 = success_vec;
        });
    } else if (!left_is_empty && !v1->empty()) {
        // TODO: проверка на то, что v1 не empty
        //res1 = root->getLeft()->p_insert(v1);
        auto [node, success_vec] = p_insert(std::move(root->getLeft()), std::move(v1));
        root->setLeft(std::move(node));
        res1 = success_vec;
    } else if (!right_is_empty && !v2->empty()) {
        //res2 = root->getRight()->p_insert(v2);
        auto [node, success_vec] = p_insert(std::move(root->getRight()), std::move(v2));
        root->setRight(std::move(node));
        res2 = success_vec;
    }

    // восстанавливаем свойства treap
    // ошибка тут! не подтираются right и left

    auto [less, equal, greater] = Split(std::move(root), root->getRepresentative()->getKey());
    root = Merge(std::move(less), std::move(equal), std::move(greater));

    // объединяем 2 вектора
    res1.insert(res1.end(), res2.begin(), res2.end());
    return std::make_tuple(std::move(root), res1);
}

template <typename T>
std::vector<bool> Treap<T>::p_insert(ElementsPtr<T> elements) {
    auto [node, result] = p_insert(m_root, std::move(elements));
    m_root = node;
    return result;
}*/

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