#pragma once
#include "Tree.h"
#include <chrono>
#include <functional>
#include <iostream>
#include <map>

#include "bench.hpp"
#include "loaders.hpp"
#include "sampleSort.h"
#include "samplesort.hpp"
#include <stdlib.h>

#undef parallel_for

using namespace pasl::pctl;
granularity::control_by_prediction p_exec_tree("p_exec_tree");

template <typename T> Tree<T>::Tree() : m_root(std::make_shared<Node<T>>()) {}

template <typename T>
Tree<T>::Tree(std::vector<ElementPtr<T>> elements)
    : m_root(std::make_shared<Node<T>>()) {

  for (auto element : elements) {
    insert(element);
  }
}

template <typename T> int Tree<T>::getSize() { return m_root->getSize(); }

template <typename T> int Tree<T>::getWeight() { return m_root->getWeight(); }

template <typename T>
bool Tree<T>::insert(ElementPtr<T> element, std::vector<Tree<T> *> *path) {
  ElementPtr<T> elementPtr = m_root->search(element->getKey());
  if (elementPtr != nullptr && !elementPtr->isMarked()) {
    delete path;
    return false;
  }

  path->push_back(this);

  if (elementPtr != nullptr) {
    elementPtr->unmark();

    for (auto tree : *path) {
      tree->increaseSize();
    }

    delete path;
    return true;
  }

  if (m_root->isAvailableForInsert()) {
    m_root->insert(element);
    createChildren();

    for (auto tree : *path) {
      tree->helpInsert();
    }
    for (auto tree : *path) {
      if (tree->updateTreeState()) {
        delete path;
        return true;
      }
    }
  } else {
    int index = m_root->getChildIndex(element->getKey());
    TreePtr<T> child = m_children[index];
    return child->insert(element, path);
  }

  delete path;
  return true;
}

template <typename T> bool Tree<T>::insert(ElementPtr<T> element) {
  return insert(element, new std::vector<Tree<T> *>());
}

template <typename T>
bool Tree<T>::remove(int key, std::vector<Tree<T> *> *path) {
  path->push_back(this);

  bool isElementDeleted = m_root->remove(key);
  if (isElementDeleted) {
    for (auto tree : *path) {
      tree->helpRemove();
    }
    for (auto tree : *path) {
      if (tree->updateTreeState()) {
        delete path;
        return true;
      }
    }
  } else if (!m_children.empty()) {
    int index = m_root->getChildIndex(key);
    TreePtr<T> child = m_children[index];
    return child->remove(key, path);
  }

  delete path;
  return isElementDeleted;
}

template <typename T> bool Tree<T>::remove(int key) {
  return remove(key, new std::vector<Tree<T> *>());
}

template <typename T> ElementPtr<T> Tree<T>::search(int key) {
  int index = m_root->getChildIndex(key);

  ElementPtr<T> elementPtr =
      index < m_root->getNodeSize() ? m_root->getByIndex(index) : nullptr;

  if (elementPtr != nullptr && elementPtr->getKey() == key &&
      elementPtr->isMarked()) {
    return nullptr;
  }

  if ((elementPtr == nullptr || elementPtr->getKey() != key) &&
      !m_children.empty()) {
    TreePtr<T> child = m_children[index];
    return child->search(key);
  }

  return elementPtr;
}

template <typename T> bool Tree<T>::contains(int key) {
  ElementPtr<T> a = search(key);
  return a != nullptr;
}

template <typename T>
std::tuple<int, int>
Tree<T>::p_execute(ActionsPtr<T> actions, int start, int end,
                   std::shared_ptr<std::vector<int>> sum_v,
                   std::shared_ptr<std::vector<bool>> res) {
  if (actions == nullptr || actions->empty()) {
    return std::make_tuple(0, 0);
  }

  int inserted = 0, removed = 0;

  auto first_sum = actions->at(start)->getPosition() > 0
                       ? sum_v->at(actions->at(start)->getPosition() - 1)
                       : 0;
  auto modifying_elem_count =
      sum_v->at((actions->at(end))->getPosition()) - first_sum;

  if (!m_children.size() || m_root->isOverflowing(modifying_elem_count) ||
      modifying_elem_count > 0 && m_root->getSize() <= 2) {
    int old_size = m_root->getSize();
    rebuild(actions, start, end, res);
    return std::make_tuple(0, old_size - m_root->getSize());
  }

  auto child_indexes = std::vector<int>(actions->size(), -1);
  parallel_for(start, end + 1, [&](int i) {
    auto action = actions->at(i);
    ElementPtr<T> elementPtr = m_root->search(action->getElement()->getKey());
    if (elementPtr) {
      auto marked = elementPtr->isMarked();
      if (action->getType() == Insert) {
        if (marked) {
          elementPtr->unmark();
          helpInsert();
          inserted++;
        }
        res->at(action->getPosition()) = marked;
      } else if (action->getType() == Remove) {
        if (!marked) {
          elementPtr->mark();
          helpRemove();
          removed++;
        }
        res->at(action->getPosition()) = !marked;
      } else if (action->getType() == Contains) {
        res->at(action->getPosition()) = !marked;
      }
    } else {
      child_indexes[i] = m_root->getChildIndex(action->getKey());
    }
  });

  auto starts = std::vector<int>(actions->size(), -1);
  auto ends = std::vector<int>(actions->size(), -1);
  parallel_for(start, end + 1, [&](int i) {
    if (child_indexes[i] != -1 &&
        (i == start || child_indexes[i] != child_indexes[i - 1])) {
      starts[i] = i;
    }

    if (child_indexes[i] != -1 &&
        (i == end || child_indexes[i] != child_indexes[i + 1])) {
      ends[i] = i;
    }
  });

  parray<intT> child_pos_start_ =
      filter(starts.begin() + start, starts.begin() + end + 1,
             [&](intT elem) { return elem > -1; });

  parray<intT> child_pos_end_ =
      filter(ends.begin() + start, ends.begin() + end + 1,
             [&](intT elem) { return elem > -1; });

  auto comp = [&, child_pos_start_ = child_pos_start_,
               child_pos_end_ = child_pos_end_, m_children = m_children,
               child_indexes = child_indexes](int i) {
    return (child_pos_start_[i] - child_pos_end_[i] + 1) *
           std::log(std::log(
               m_children[child_indexes[child_pos_start_[i]]]->getSize()));
  };

  if (child_pos_start_.size() > 0) {
    auto child_results = std::make_shared<std::vector<std::tuple<int, int>>>(
        child_pos_start_.size());

    granularity::cstmt(
        p_exec_tree,
        [&, actions = actions, child_results = child_results,
         m_children = m_children, child_indexes = child_indexes, sum_v = sum_v,
         res = res] {
          return (end - start + 1) * std::log(std::log(getSize()));
        },
        [&, child_pos_start_ = child_pos_start_,
         child_pos_end_ = child_pos_end_, actions = actions,
         child_results = child_results, m_children = m_children,
         child_indexes = child_indexes, sum_v = sum_v, res = res] {
          parallel_for(
              0, static_cast<int>(child_pos_start_.size()), /*comp,*/
              [&, child_results = child_results, m_children = m_children,
               child_indexes = child_indexes, sum_v = sum_v, res = res](int i) {
                (*child_results)[i] =
                    m_children[child_indexes[child_pos_start_[i]]]->p_execute(
                        actions, child_pos_start_[i], child_pos_end_[i], sum_v,
                        res);
              });
        },
        [&, child_pos_start_ = child_pos_start_,
         child_pos_end_ = child_pos_end_, actions = actions,
         child_results = child_results, m_children = m_children,
         child_indexes = child_indexes, sum_v = sum_v, res = res] {
          for (int i = 0; i < child_pos_start_.size(); i++) {
            for (int j = child_pos_start_[i]; j <= child_pos_end_[i]; j++) {
              auto child = m_children[child_indexes[j]];
              auto action = actions->at(j);
              bool success;
              int cur_inserted, cur_removed;
              if (action->getType() == Insert) {
                success = child->insert(action->getElement());
                cur_inserted =
                    (success ? 1 : 0) + std::get<0>((*child_results)[i]);
                cur_removed = std::get<1>((*child_results)[i]);
                (*child_results)[i] =
                    std::make_tuple(cur_inserted, cur_removed);
              } else if (action->getType() == Remove) {
                success = child->remove(action->getElement()->getKey());
                cur_inserted = std::get<0>((*child_results)[i]);
                cur_removed =
                    (success ? 1 : 0) + std::get<1>((*child_results)[i]);
                (*child_results)[i] =
                    std::make_tuple(cur_inserted, cur_removed);
              } else if (action->getType() == Contains) {
                success = child->contains(action->getElement()->getKey());
                (*child_results)[i] = std::make_tuple(0, 0);
              }
              res->at(action->getPosition()) = success;
            }
          }
        });

    for (auto result : *child_results) {
      inserted += std::get<0>(result);
      removed += std::get<1>(result);
      m_root->increaseSize(std::get<0>(result) - std::get<1>(result));
      m_root->increaseWeight(std::get<0>(result));
    }
  }

  return std::make_tuple(inserted, removed);
}

template <typename T>
std::shared_ptr<std::vector<bool>> Tree<T>::p_execute(ActionsPtr<T> actions) {
  auto sum_v = build_modifying_sum_vector(actions);
  auto res = std::make_shared<std::vector<bool>>(actions->size());

  p_execute(actions, 0, actions->size() - 1, sum_v, res);
  return std::move(res);
}

template <typename T>
std::shared_ptr<std::vector<int>>
build_modifying_sum_vector(ActionsPtr<T> actions) {
  auto sum = std::make_shared<std::vector<int>>();
  int current_sum = 0;
  for (auto action : *actions) {
    if (action->getType() != Contains && action->getType() != Remove) {
      current_sum++;
    }
    sum->push_back(current_sum);
  }
  return std::move(sum);
}

template <typename T> void Tree<T>::print(const std::string &prefix) {
  std::cout << prefix << "└──";

  m_root->print();
  std::cout << " size: " << getSize();
  std::cout << std::endl;

  for (auto child : m_children) {
    child->print(prefix + "│   ");
  }
}

template <typename T> void Tree<T>::helpInsert() {
  m_root->increaseSize();
  m_root->increaseWeight();
}

template <typename T> void Tree<T>::helpRemove() { m_root->decreaseSize(); }

template <typename T> void Tree<T>::increaseSize() { m_root->increaseSize(); }

template <typename T>
void Tree<T>::rebuild(ActionsPtr<T> actions, int start, int end,
                      std::shared_ptr<std::vector<bool>> res) {
  std::vector<ElementPtr<T>> *rebuildingElements = compoundRebuildingVector();
  auto result_elements = new std::vector<ElementPtr<T>>();

  int i = 0, j = start;

  while (i < rebuildingElements->size() && j <= end) {
    auto element = rebuildingElements->at(i);
    auto action = actions->at(j);

    if (action->getKey() > element->getKey()) {
      result_elements->push_back(element);
      i++;
    } else if (action->getKey() == element->getKey()) {
      if (action->getType() == Insert) {
        // т.к. если в rebuildingElements все элементы unmarked
        result_elements->push_back(element);
        res->at(action->getPosition()) = false;
        i++;
        j++;
      } else if (action->getType() == Remove) {
        res->at(action->getPosition()) = true;
        i++;
        j++;
      } else if (action->getType() == Contains) {
        result_elements->push_back(element);
        res->at(action->getPosition()) = true;
        i++;
        j++;
      }
    } else {
      if (action->getType() == Insert) {
        result_elements->push_back(action->getElement());
        res->at(action->getPosition()) = true;
      } else if (action->getType() == Remove) {
        res->at(action->getPosition()) = false;
      } else if (action->getType() == Contains) {
        res->at(action->getPosition()) = false;
      }
      j++;
    }
  }

  while (i < rebuildingElements->size()) {
    result_elements->push_back(rebuildingElements->at(i));
    i++;
  }

  while (j <= end) {
    auto action = actions->at(j);
    if (action->getType() == Insert) {
      result_elements->push_back(action->getElement());
      res->at(action->getPosition()) = true;
    } else if (action->getType() == Remove) {
      res->at(action->getPosition()) = false;
    } else if (action->getType() == Contains) {
      res->at(action->getPosition()) = false;
    }
    j++;
  }

  rebuild(result_elements, 0, result_elements->size() - 1);
  delete rebuildingElements;
}

template <typename T> void Tree<T>::rebuild() {
  std::vector<ElementPtr<T>> *rebuildingElements =
      compoundRebuildingVector(); // RIGHT
  rebuild(rebuildingElements, 0, rebuildingElements->size() - 1);
}

template <typename T>
void Tree<T>::rebuild(std::vector<ElementPtr<T>> *rebuildingElements, int start,
                      int end) {
  if (rebuildingElements->empty())
    return;

  int step = floor(sqrt(end - start + 1));

  m_children.clear();

  if (end - start + 1 <= 3) {
    auto newRepresentatives = std::vector<ElementPtr<T>>(end - start + 1);

    for (int i = 0; i < end - start + 1; i++) {
      m_children.push_back(std::make_shared<Tree<T>>());
      newRepresentatives[i] = (*rebuildingElements)[start + i];
    }
    m_children.push_back(std::make_shared<Tree<T>>());

    int min = (*rebuildingElements)[start]->getKey();
    int max = (*rebuildingElements)[end]->getKey();

    m_root = std::make_shared<Node<T>>(newRepresentatives, end - start + 1, min,
                                       max);

    return;
  }

  auto flag_start = false;
  auto flag_end = false;

  auto y = std::vector<int>(rebuildingElements->size(), -1);
  parallel_for(start, end + 1, [&](int i) {
    if (start == end || i == start + step / 2 ||
        i >= start + step / 2 && (i - start - step / 2) % step == 0) {
      y[i] = -1 * i - 1;
      flag_start = (i == start);
      flag_end = (i == end);
    } else {
      y[i] = i;
    }
  });

  parray<intT> newRepresentativesIds =
      filter(y.begin() + start, y.begin() + end + 1,
             [&](intT elem) { return elem < 0; });

  auto newRepresentatives =
      std::vector<ElementPtr<T>>(newRepresentativesIds.size());
  parallel_for(0, static_cast<int>(newRepresentativesIds.size()), [&](int i) {
    newRepresentatives[i] =
        (*rebuildingElements)[(-1 * (newRepresentativesIds[i] + 1))];
  });

  auto starts = std::vector<int>(rebuildingElements->size(), -1);
  auto ends = std::vector<int>(rebuildingElements->size(), -1);

  parallel_for(start, end + 1, [&](int i) {
    if (y[i] >= 0 && (i == start || y[i - 1] < 0)) {
      starts[i] = i;
    }

    if (y[i] >= 0 && (i == end || y[i + 1] < 0)) {
      ends[i] = i;
    }
  });

  parray<intT> child_pos_start_ =
      filter(starts.begin() + start, starts.begin() + end + 1,
             [&](intT elem) { return elem > -1; });

  parray<intT> child_pos_end_ =
      filter(ends.begin() + start, ends.begin() + end + 1,
             [&](intT elem) { return elem > -1; });

  if (flag_start)
    m_children.push_back(std::make_shared<Tree<T>>());

  if (child_pos_start_.size() > 0) {
    auto children_tmp =
        std::make_shared<std::vector<TreePtr<T>>>(child_pos_start_.size());

    parallel_for(0, static_cast<int>(child_pos_start_.size()),
                 /*comp,*/ [&](int i) {
                   auto newChild = std::make_shared<Tree<T>>();
                   newChild->rebuild(rebuildingElements, child_pos_start_[i],
                                     child_pos_end_[i]);
                   (*children_tmp)[i] = newChild;
                 });

    for (auto child : *children_tmp) {
      m_children.push_back(child);
    }
  }

  if (flag_end)
    m_children.push_back(std::make_shared<Tree<T>>());

  int min = (*rebuildingElements)[start]->getKey();
  int max = (*rebuildingElements)[end]->getKey();

  m_root =
      std::make_shared<Node<T>>(newRepresentatives, end - start + 1, min, max);
}

template <typename T>
std::vector<ElementPtr<T>> *Tree<T>::compoundRebuildingVector() {
  auto rebuildingElements = new std::vector<ElementPtr<T>>(getSize());
  return compoundRebuildingVector(rebuildingElements, 0);
}

template <typename T>
std::vector<ElementPtr<T>> *Tree<T>::compoundRebuildingVector(
    std::vector<ElementPtr<T>> *rebuildingElements, int position) {
  std::vector<ElementPtr<T>> rootRepresentatives = m_root->getRepresentatives();

  std::vector<int> childrenTreesElementsPositions;
  childrenTreesElementsPositions.push_back(position);
  for (int i = 1; i < m_children.size(); i++) {
    int markedBias = rootRepresentatives[i - 1]->isMarked() ? 0 : 1;
    childrenTreesElementsPositions.push_back(
        childrenTreesElementsPositions[i - 1] + m_children[i - 1]->getSize() +
        markedBias);
  }

  for (int i = 0; i < rootRepresentatives.size(); i++) {
    if (!rootRepresentatives[i]->isMarked()) {
      (*rebuildingElements)[childrenTreesElementsPositions[i + 1] - 1] =
          rootRepresentatives[i];
    }
  }

  for (int i = 0; i < m_children.size(); i++) {
    m_children[i]->compoundRebuildingVector(rebuildingElements,
                                            childrenTreesElementsPositions[i]);
  }

  return rebuildingElements;
}

template <typename T> bool Tree<T>::updateTreeState() {
  m_root->increaseCounter();

  if (m_root->isOverflowing()) {
    rebuild();
    return true;
  }

  return false;
}

template <typename T> void Tree<T>::createChildren() {
  if (m_children.empty()) {
    m_children.push_back(std::make_shared<Tree<T>>());
  }
  m_children.push_back(std::make_shared<Tree<T>>());
}

template <typename T> NodePtr<T> Tree<T>::getNode() { return m_root; }

template <typename T> std::vector<TreePtr<T>> Tree<T>::getChildren() {
  return m_children;
}
