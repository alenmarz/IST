#pragma once
#ifndef IST_TREE_H
#define IST_TREE_H

#include "Node.h"

template <typename T> class Tree {
private:
  NodePtr<T> m_root;
  std::vector<std::shared_ptr<Tree>> m_children;

  bool insert(ElementPtr<T> element, std::vector<Tree<T> *> *path);
  bool remove(int key, std::vector<Tree<T> *> *path);
  void helpInsert();
  void helpRemove();
  void rebuild(std::vector<ElementPtr<T>> *rebuildingElements, int start,
               int end, std::shared_ptr<std::vector<int>> y,
							 std::shared_ptr<std::vector<int>> starts,
							 std::shared_ptr<std::vector<int>> ends);
  void rebuild(ActionsPtr<T> actions, int start, int end,
               std::shared_ptr<std::vector<bool>> res);
  std::vector<ElementPtr<T>> *compoundRebuildingVector();
  std::vector<ElementPtr<T>> *
  compoundRebuildingVector(std::vector<ElementPtr<T>> *rebuildingElements,
                           int position);
  std::tuple<int, int>
  p_execute(ActionsPtr<T> actions, int start, int end,
            std::shared_ptr<std::vector<int>> child_indexes,
            std::shared_ptr<std::vector<int>> starts,
            std::shared_ptr<std::vector<int>> ends,
            std::shared_ptr<std::vector<int>> sum_v,
            std::shared_ptr<std::vector<bool>> res);
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
  std::shared_ptr<std::vector<bool>> p_execute(ActionsPtr<T> actions);
  ElementPtr<T> search(int key);
  void print(const std::string &prefix);
  NodePtr<T> getNode();
  std::vector<std::shared_ptr<Tree<T>>> getChildren();
};

template <typename T> using TreePtr = std::shared_ptr<Tree<T>>;

#endif // IST_TREE_H

#include "Tree.hpp"
