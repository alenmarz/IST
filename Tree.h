#pragma once
#ifndef IST_TREE_H
#define IST_TREE_H

#include "Node.h"

template <typename T>
class Tree {
private:
    NodePtr<T> m_root;
    std::vector<std::shared_ptr<Tree>> m_children;

    bool insert(ElementPtr<T> element, std::vector<Tree<T>*> *path);
    bool remove(int key, std::vector<Tree<T>*> *path);
    void helpInsert();
    void helpRemove();
    void rebuild(std::vector<ElementPtr<T>> *rebuildingElements);
    std::vector<ElementPtr<T>> *compoundRebuildingVector();
    std::vector<ElementPtr<T>> *compoundRebuildingVector(std::vector<ElementPtr<T>> *rebuildingElements, int position);
    bool updateTreeState();
    void createChildren();
    void increaseSize();

public:
    Tree();
    explicit Tree(std::vector<ElementPtr<T>> elements);
    int getSize();
    int getWeight();
    bool insert(ElementPtr<T> element);
    bool remove(int key);
    void rebuild();
    bool contains(int key);
    ElementPtr<T> search(int key);
    void print(const std::string& prefix);
    NodePtr<T> getNode();
    std::vector<std::shared_ptr<Tree<T>>> getChildren();
};

template <typename T>
using TreePtr = std::shared_ptr<Tree<T>>;

#endif //IST_TREE_H

#include "Tree.hpp"
