//
// Created by Alena Martsenyuk on 19/10/2020.
//
#pragma once
#ifndef IST_TREE_H
#define IST_TREE_H

#include "Node.h"

template <typename T>
class Tree {
    Node<T>* m_root;
    std::vector<Tree *> m_children;

public:
    Tree();
    explicit Tree(std::vector<Element<T>*> elements);
    void insertElement(Element<T> *element, std::vector<Tree<T>*> *path);
    void insertElement(Element<T> *element);
    void deleteElement(int key, std::vector<Tree<T>*> *path);
    void deleteElement(int key);
    void helpDelete();
    void helpInsert();
    void rebuild();
    void rebuild(std::vector<Element<T>*> *rebuildingElements);
    std::vector<Element<T>*> * compoundRebuildingVector();
    std::vector<Element<T>*> * compoundRebuildingVector(std::vector<Element<T>*> *rebuildingElements, int position);
    bool updateTreeState();
    T search(int key);
    void createChildren();
    int getSize();
    int getWeight();
    void print(const std::string& prefix);
};


#endif //IST_TREE_H

#include "Tree-inl.h"
