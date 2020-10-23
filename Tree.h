#pragma once
#ifndef IST_TREE_H
#define IST_TREE_H

#include "Node.h"

template <typename T>
class Tree {
private:
    NodePtr<T> m_root;
    std::vector<std::shared_ptr<Tree>> m_children;

    void insertElement(ElementPtr<T> element, std::vector<Tree<T>*> *path);
    void deleteElement(int key, std::vector<Tree<T>*> *path);
    void helpInsert();
    void helpDelete();
    void rebuild();
    void rebuild(std::vector<ElementPtr<T>> *rebuildingElements);
    std::vector<ElementPtr<T>> *compoundRebuildingVector();
    std::vector<ElementPtr<T>> *compoundRebuildingVector(std::vector<ElementPtr<T>> *rebuildingElements, int position);
    bool updateTreeState();
    void createChildren();

public:
    Tree();
    explicit Tree(std::vector<ElementPtr<T>> elements);
    int getSize();
    int getWeight();
    void insertElement(ElementPtr<T> element);
    void deleteElement(int key);
    T search(int key);
    void print(const std::string& prefix);
};

template <typename T>
using TreePtr = std::shared_ptr<Tree<T>>;

#endif //IST_TREE_H

#include "Tree-inl.h"
