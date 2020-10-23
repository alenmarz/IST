//
// Created by Alena Martsenyuk on 19/10/2020.
//
#pragma once
#ifndef IST_TREE_H
#define IST_TREE_H

#include "Node.h"

template <typename T>
class Tree {
private:
    std::shared_ptr<Node<T>> m_root;
    std::vector<std::shared_ptr<Tree>> m_children;

    void insertElement(std::shared_ptr<Element<T>> element, std::vector<Tree<T>*> *path);
    void deleteElement(int key, std::vector<Tree<T>*> *path);
    void helpDelete();
    void helpInsert();
    void rebuild();
    void rebuild(std::vector<std::shared_ptr<Element<T>>> *rebuildingElements);
    std::vector<std::shared_ptr<Element<T>>> *compoundRebuildingVector();
    std::vector<std::shared_ptr<Element<T>>> *compoundRebuildingVector(std::vector<std::shared_ptr<Element<T>>> *rebuildingElements, int position);
    bool updateTreeState();
    void createChildren();

public:
    Tree();
    explicit Tree(std::vector<std::shared_ptr<Element<T>>> elements);
    int getSize();
    int getWeight();
    void insertElement(std::shared_ptr<Element<T>> element);
    void deleteElement(int key);
    T search(int key);
    void print(const std::string& prefix);
};


#endif //IST_TREE_H

#include "Tree-inl.h"
