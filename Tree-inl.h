//
// Created by Alena Martsenyuk on 19/10/2020.
//
#pragma once
#include "Tree.h"
#include <iostream>

template <typename T>
Tree<T>::Tree() :
    m_root(new Node<T>()) {
}

template <typename T>
Tree<T>::Tree(std::vector<Element<T>*> elements) :
    m_root(new Node<T>()) {

    for (auto element: elements) {
        insertElement(element);
    }
}

template <typename T>
void Tree<T>::insertElement(Element<T> *element, std::vector<Tree<T>*> *path) {
    path->push_back(this);

    if (m_root->isAvailableForInsert()) {
        m_root->insert(element);
        createChildren();

        for (auto tree: *path) {
            tree->helpInsert();
        }
        for (auto tree: *path) {
            if (tree->updateTreeState()) {
                return;
            }
        }
    } else {
        int index = m_root->getChildIndex(element->getKey());
        Tree<T>* child = m_children[index];
        return child->insertElement(element, path);
    }
}

template <typename T>
void Tree<T>::insertElement(Element<T> *element) {
    insertElement(element, new std::vector<Tree<T>*>());
}

template <typename T>
void Tree<T>::deleteElement(int key, std::vector<Tree<T>*> *path) {
    path->push_back(this);

    bool isElementDeleted = m_root->deleteElement(key);
    if (isElementDeleted) {
        for (auto tree: *path) {
            tree->helpDelete();
        }
        for (auto tree: *path) {
            if (tree->updateTreeState()) {
                return;
            }
        }
    } else if (!m_children.empty()) {
        int index = m_root->getChildIndex(key);
        Tree<T>* child = m_children[index];
        return child->deleteElement(key, path);
    }
}

template <typename T>
void Tree<T>::deleteElement(int key) {
    deleteElement(key, new std::vector<Tree<T>*>());
}

template <typename T>
T Tree<T>::search(int key) {
    T value = m_root->search(key);

    if (!value && !m_children.empty()) {
        int index = m_root->getChildIndex(key);
        Tree<T>* child = m_children[index];

        return child->search(key);
    }

    return value;
}

template <typename T>
void Tree<T>::helpDelete() {
    m_root->decreaseSize();
}

template <typename T>
void Tree<T>::helpInsert() {
    m_root->increaseSize();
    m_root->increaseWeight();
}

template <typename T>
bool Tree<T>::updateTreeState() {
    m_root->increaseCounter();

    if (m_root->isOverflowing()) {
        rebuild();
        return true;
    }

    return false;
}

template <typename T>
void Tree<T>::rebuild(std::vector<Element<T>*> *rebuildingElements) {
    if (rebuildingElements->empty()) return;

    std::vector<Element<T>*> newRepresentatives;
    int step = floor(sqrt(rebuildingElements->size()));

    auto childElements = new std::vector<Element<T>*>();
    m_children.clear();

    for (int i = 0, j = step / 2; i < rebuildingElements->size(); i++) {
        if (i == j) {
            newRepresentatives.push_back((*rebuildingElements)[i]);

            auto newChild = new Tree<T>();
            newChild->rebuild(childElements);
            m_children.push_back(newChild);
            childElements->clear();
        } else {
            childElements->push_back((*rebuildingElements)[i]);
        }

        if (i == j) {
            j += step;
        }
    }

    auto newChild = new Tree<T>();
    newChild->rebuild(childElements);
    m_children.push_back(newChild);
    childElements->clear();

    m_root = new Node<T>(newRepresentatives, rebuildingElements->size());
}

template <typename T>
void Tree<T>::rebuild() {
    std::vector<Element<T>*> *rebuildingElements = compoundRebuildingVector(); // RIGHT
    rebuild(rebuildingElements);
}

template <typename T>
std::vector<Element<T>*> * Tree<T>::compoundRebuildingVector(std::vector<Element<T>*> *rebuildingElements, int position) {
    std::vector<Element<T> *> rootRepresentatives = m_root->getRepresentatives();

    std::vector<int> childrenTreesElementsPositions;
    childrenTreesElementsPositions.push_back(position);
    for (int i = 1; i < m_children.size(); i++) {
        childrenTreesElementsPositions.push_back(childrenTreesElementsPositions[i - 1] + m_children[i - 1]->getSize() + 1);
    }

    for (int i = 0; i < rootRepresentatives.size(); i++) {
        if (!rootRepresentatives[i]->isMarked()) {
            (*rebuildingElements)[childrenTreesElementsPositions[i + 1] - 1] = rootRepresentatives[i];
        }
    }

    for (int i = 0; i < m_children.size(); i++) {
        m_children[i]->compoundRebuildingVector(rebuildingElements, childrenTreesElementsPositions[i]);
    }

    return rebuildingElements;
}

template <typename T>
std::vector<Element<T>*> * Tree<T>::compoundRebuildingVector() {
    auto rebuildingElements = new std::vector<Element<T>*>(getSize());
    return compoundRebuildingVector(rebuildingElements, 0);
}

template <typename T>
void Tree<T>::createChildren() {
    if (m_children.empty()) {
        m_children.push_back(new Tree<T>);
    }
    m_children.push_back(new Tree<T>);
}

template <typename T>
int Tree<T>::getSize() {
    return m_root->getSize();
}

template <typename T>
int Tree<T>::getWeight() {
    return m_root->getWeight();
}

template <typename T>
void Tree<T>::print(const std::string& prefix) {
    std::cout << prefix << "└──";

    m_root->print();
    std:: cout << std::endl;

    for (auto child: m_children) {
        child->print(prefix + "│   ");
    }
}