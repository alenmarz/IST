#pragma once
#include "Tree.h"
#include <iostream>

template <typename T>
Tree<T>::Tree() :
    m_root(std::make_shared<Node<T>>()) {
}

template <typename T>
Tree<T>::Tree(std::vector<ElementPtr<T>> elements) :
    m_root(std::make_shared<Node<T>>()) {

    for (auto element: elements) {
        insert(element);
    }
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
bool Tree<T>::insert(ElementPtr<T> element, std::vector<Tree<T>*> *path) {
    ElementPtr<T> elementPtr = m_root->search(element->getKey());
    if (elementPtr != nullptr && !elementPtr->isMarked())
        return false;

    path->push_back(this);

    if (elementPtr != nullptr) {
        elementPtr->unmark();

        for (auto tree: *path) {
            tree->increaseSize();
        }

        return true;
    }

    if (m_root->isAvailableForInsert()) {
        m_root->insert(element);
        createChildren();

        for (auto tree: *path) {
            tree->helpInsert();
        }
        for (auto tree: *path) {
            if (tree->updateTreeState()) {
                return true;
            }
        }
        return true;
    } else {
        int index = m_root->getChildIndex(element->getKey());
        TreePtr<T> child = m_children[index];
        return child->insert(element, path);
    }
}

template <typename T>
bool Tree<T>::insert(ElementPtr<T> element) {
    return insert(element, new std::vector<Tree<T> *>());
}

/*template <typename T>
bool Tree<T>::insert(ElementPtr<T> element) {
    std::vector<Tree<T>*> path;

    Tree<T>* currentTree = this;
    NodePtr<T> currentNode;
    std::vector<TreePtr<T>> currentChildren;

    while (true) {
        currentNode = currentTree->getNode();
        currentChildren = currentTree->getChildren();
        if (currentNode->search(element->getKey()) != nullptr) return false;

        path.push_back(currentTree);

        if (currentNode->isAvailableForInsert()) {
            currentNode->insert(element);
            currentTree->createChildren();

            for (auto tree: path) {
                tree->helpInsert();
            }
            for (auto tree: path) {
                if (tree->updateTreeState()) {
                    return true;
                }
            }
            return true;
        } else {
            int index = currentNode->getChildIndex(element->getKey());
            currentTree = currentChildren[index].get();
        }
    }

    return false;
}*/

template <typename T>
bool Tree<T>::remove(int key, std::vector<Tree<T>*> *path) {
    path->push_back(this);

    bool isElementDeleted = m_root->remove(key);
    if (isElementDeleted) {
        for (auto tree: *path) {
            tree->helpRemove();
        }
        for (auto tree: *path) {
            if (tree->updateTreeState()) {
                return true;
            }
        }
    } else if (!m_children.empty()) {
        int index = m_root->getChildIndex(key);
        TreePtr<T> child = m_children[index];
        return child->remove(key, path);
    }

    return isElementDeleted;
}

template <typename T>
bool Tree<T>::remove(int key) {
    return remove(key, new std::vector<Tree<T> *>());
}

/*template <typename T>
ElementPtr<T> Tree<T>::search(int key) {
    int index = m_root->getChildIndex(key);

    ElementPtr<T> elementPtr = index < m_root->getNodeSize() ? m_root->getByIndex(index) : nullptr;
    if ((elementPtr == nullptr || elementPtr->getKey() != key) && !m_children.empty()) {
        TreePtr<T> child = m_children[index];
        return child->search(key);
    }

    return elementPtr;
}*/

template <typename T>
ElementPtr<T> Tree<T>::search(int key) {
    ElementPtr<T> elementPtr = m_root->search(key);

    if (elementPtr != nullptr && elementPtr->isMarked()) {
        return nullptr;
    }

    if (elementPtr == nullptr && !m_children.empty()) {
        int index = m_root->getChildIndex(key);
        TreePtr<T> child = m_children[index];
        return child->search(key);
    }

    return elementPtr;
}

template <typename T>
bool Tree<T>::contains(int key) {
    ElementPtr<T> a = search(key);
    return a != nullptr;
}

template <typename T>
void Tree<T>::print(const std::string& prefix) {
    std::cout << prefix << "└──";

    m_root->print();
    std::cout << " size: " << getSize();
    std:: cout << std::endl;

    for (auto child: m_children) {
        child->print(prefix + "│   ");
    }
}

template <typename T>
void Tree<T>::helpInsert() {
    m_root->increaseSize();
    m_root->increaseWeight();
}

template <typename T>
void Tree<T>::helpRemove() {
    m_root->decreaseSize();
}

template <typename T>
void Tree<T>::increaseSize() {
    m_root->increaseSize();
}

template <typename T>
void Tree<T>::rebuild() {
    std::vector<ElementPtr<T>> *rebuildingElements = compoundRebuildingVector(); // RIGHT
    rebuild(rebuildingElements);
}

template <typename T>
void Tree<T>::rebuild(std::vector<ElementPtr<T>> *rebuildingElements) {
    if (rebuildingElements->empty()) return;

    std::vector<ElementPtr<T>> newRepresentatives;
    int step = floor(sqrt(rebuildingElements->size()));

    auto childElements = new std::vector<ElementPtr<T>>();
    m_children.clear();

    for (int i = 0, j = step / 2; i < rebuildingElements->size(); i++) {
        if (i == j) {
            newRepresentatives.push_back((*rebuildingElements)[i]);

            auto newChild = std::make_shared<Tree<T>>();
            newChild->rebuild(childElements);
            m_children.push_back(newChild);
            childElements->clear();

            j += step;
        } else {
            childElements->push_back((*rebuildingElements)[i]);
        }
    }

    auto newChild = std::make_shared<Tree<T>>();
    newChild->rebuild(childElements);
    m_children.push_back(newChild);
    childElements->clear();

    int min = (*rebuildingElements)[0]->getKey();
    int max = (*rebuildingElements)[rebuildingElements->size() - 1]->getKey();

    m_root = std::make_shared<Node<T>>(newRepresentatives, rebuildingElements->size(), min, max);
}

template <typename T>
std::vector<ElementPtr<T>> * Tree<T>::compoundRebuildingVector() {
    auto rebuildingElements = new std::vector<ElementPtr<T>>(getSize());
    return compoundRebuildingVector(rebuildingElements, 0);
}

template <typename T>
std::vector<ElementPtr<T>> * Tree<T>::compoundRebuildingVector(
        std::vector<ElementPtr<T>> *rebuildingElements,
        int position) {
    std::vector<ElementPtr<T>> rootRepresentatives = m_root->getRepresentatives();

    std::vector<int> childrenTreesElementsPositions;
    childrenTreesElementsPositions.push_back(position);
    for (int i = 1; i < m_children.size(); i++) {
        int markedBias = rootRepresentatives[i - 1]->isMarked() ? 0 : 1;
        childrenTreesElementsPositions.push_back(childrenTreesElementsPositions[i - 1] + m_children[i - 1]->getSize() + markedBias);
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
bool Tree<T>::updateTreeState() {
    m_root->increaseCounter();

    if (m_root->isOverflowing()) {
        rebuild();
        return true;
    }

    return false;
}

template <typename T>
void Tree<T>::createChildren() {
    if (m_children.empty()) {
        m_children.push_back(std::make_shared<Tree<T>>());
    }
    m_children.push_back(std::make_shared<Tree<T>>());
}

template <typename T>
NodePtr<T> Tree<T>::getNode() {
    return m_root;
}

template <typename T>
std::vector<TreePtr<T>> Tree<T>::getChildren() {
    return m_children;
}