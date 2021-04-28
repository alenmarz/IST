#pragma once
#include "Tree.h"
#include <iostream>
#include <functional>
#include <chrono>
#include <map>

#include <stdlib.h>
#include "bench.hpp"
#include "samplesort.hpp"
#include "loaders.hpp"
#include "sampleSort.h"

#undef parallel_for

using namespace pasl::pctl;
granularity::control_by_prediction p_exec_tree("p_exec_tree");
granularity::control_by_prediction p_exec_tree1("p_exec_tree1");

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
    if (elementPtr != nullptr && !elementPtr->isMarked()) {
        delete path;
        return false;
    }

    path->push_back(this);

    if (elementPtr != nullptr) {
        elementPtr->unmark();

        for (auto tree: *path) {
            tree->increaseSize();
        }

        delete path;
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

template <typename T>
bool Tree<T>::insert(ElementPtr<T> element) {
    return insert(element, new std::vector<Tree<T> *>());
}

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

template <typename T>
bool Tree<T>::remove(int key) {
    return remove(key, new std::vector<Tree<T> *>());
}

template <typename T>
ElementPtr<T> Tree<T>::search(int key) {
    int index = m_root->getChildIndex(key);

    ElementPtr<T> elementPtr = index < m_root->getNodeSize() ? m_root->getByIndex(index) : nullptr;

    if (elementPtr != nullptr && elementPtr->getKey() == key && elementPtr->isMarked()) {
        return nullptr;
    }

    if ((elementPtr == nullptr || elementPtr->getKey() != key) && !m_children.empty()) {
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
std::tuple<int, int> Tree<T>::p_execute(ActionsPtr<T> actions, std::shared_ptr<std::vector<int>> sum_v, std::shared_ptr<std::vector<bool>> res) {
    if (actions == nullptr || actions->empty()) {
        return std::make_tuple(0, 0);
    }

    /*for (auto action: *actions) {
    	std::cout << action->getKey() << " ";    
    }

    std::cout << std::endl;*/

    //print("");


    int inserted = 0, removed = 0;

    /*if (!m_children.size()) {
        for (auto action: *actions) {
            res->at(action->getPosition()) = false;
        }
        return std::make_tuple(0, 0);
    }*/

    auto first_sum = actions->at(0)->getPosition() > 0 ? sum_v->at(actions->at(0)->getPosition() - 1) : 0;
    auto modifying_elem_count = sum_v->at((actions->at(actions->size() - 1))->getPosition()) - first_sum;
    //std::cout << "modifying_elem_count: " << modifying_elem_count << std::endl;
    if (!m_children.size() || m_root->isOverflowing(modifying_elem_count) || modifying_elem_count > 0 && m_root->getSize() <= 2) {
        int old_size = m_root->getSize();
        rebuild(actions, res);
        return std::make_tuple(0, old_size - m_root->getSize());
    }

    auto current_child_index = -1;
    auto child_action_map = std::map<int, ActionsPtr<T>>();
    auto child_indexes = std::vector<int>();

    for (auto action: *actions) {
        ElementPtr<T> elementPtr = m_root->search(action->getElement()->getKey());
        if (elementPtr) {
            auto marked = elementPtr->isMarked();
            if (action->getType() == Insert) {
		//std::cout << "in insert " << action->getKey() << " " << marked  << std::endl;
                if (marked) {
                    elementPtr->unmark();
                    helpInsert();
                    inserted++;
                }
                res->at(action->getPosition()) = marked;
            } else if (action->getType() == Remove) {
                if (!marked) {
		//	std::cout << "in remove " << action->getKey() << std::endl;
                    elementPtr->mark();
                    helpRemove();
                    removed++;
                }
                res->at(action->getPosition()) = !marked;
            } else if (action->getType() == Contains) {
                res->at(action->getPosition()) = !marked;
            }
        } else {
            int index = m_root->getChildIndex(action->getKey());
            if (current_child_index != index) {
                current_child_index = index;
                child_indexes.push_back(index);
                child_action_map[index] = std::make_shared<Actions<int>>();
            }
            child_action_map[index]->push_back(action);
        }
    }

    auto comp = [&, child_action_map = child_action_map, m_children = m_children, child_indexes = child_indexes] (int i) {
   	 return child_action_map.at(child_indexes[i])->size() * std::log(std::log(m_children[child_indexes[i]]->getSize()));
  	};

    // Параллельная часть
    if (current_child_index >= 0) {
        auto child_results = std::make_shared<std::vector<std::tuple<int, int>>>(child_indexes.size());
	granularity::cstmt(p_exec_tree, [&, actions = actions, child_results = child_results, m_children = m_children, child_indexes = child_indexes, sum_v = sum_v, res = res, child_action_map = child_action_map] {
        	return actions->size() * std::log(std::log(getSize()));
	}, [&, actions = actions, child_results = child_results, m_children = m_children, child_indexes = child_indexes, sum_v = sum_v, res = res, child_action_map = child_action_map] {
	
	//std::cout << actions->size() * std::log(std::log(getSize())) << std::endl;
	//if (getSize() > 0 && actions->size() * std::log(std::log(getSize())) > 40) {	
	parallel_for(0, static_cast<int>(child_indexes.size()), comp, [&, child_results = child_results, m_children = m_children, child_indexes = child_indexes, sum_v = sum_v, res = res, child_action_map = child_action_map] (int i) {
	    		(*child_results)[i] = m_children[child_indexes[i]]->p_execute(child_action_map.at(child_indexes[i]), sum_v, res);
        });//} else {
	}, [&, actions = actions, child_results = child_results, m_children = m_children, child_indexes = child_indexes, sum_v = sum_v, res = res, child_action_map = child_action_map] {
	//std::cout << "a size: " << actions->size() << " t size:" << getSize() << std::endl;
	
	for (int i = 0; i < child_indexes.size(); i++) {
		auto child = m_children[child_indexes[i]];
			for (auto action: *child_action_map.at(child_indexes[i])) {
			bool success;
			int cur_inserted, cur_removed;
            if (action->getType() == Insert) {

                success = child->insert(action->getElement());
			cur_inserted = (success ? 1 : 0) + std::get<0>((*child_results)[i]);
            		cur_removed = std::get<1>((*child_results)[i]);
			(*child_results)[i] = std::make_tuple(cur_inserted, cur_removed);
            } else if (action->getType() == Remove) {
                success = child->remove(action->getElement()->getKey());
                        cur_inserted = std::get<0>((*child_results)[i]);
                        cur_removed = (success ? 1 : 0) + std::get<1>((*child_results)[i]);
			(*child_results)[i] = std::make_tuple(cur_inserted, cur_removed);
            } else if (action->getType() == Contains) {
                success = child->contains(action->getElement()->getKey());
                        (*child_results)[i] = std::make_tuple(0, 0);
            }
	    res->at(action->getPosition()) = success;
        }
		}//}
	});
        for (auto result: *child_results) {
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
    //std::cout << "a size: " << actions->size() << std::endl;
    //std::cout << "res size: " << res->size() << std::endl;
    p_execute(actions, sum_v, res);
    return std::move(res);
}

template <typename T>
std::shared_ptr<std::vector<int>> build_modifying_sum_vector(ActionsPtr<T> actions) {
    auto sum = std::make_shared<std::vector<int>>();
    int current_sum = 0;
    for (auto action: *actions) {
        if (action->getType() != Contains && action->getType() != Remove) {
            current_sum++;
        }
	sum->push_back(current_sum);
    }
    return std::move(sum);
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
    //m_root->increaseCounter();
}

template <typename T>
void Tree<T>::helpRemove() {
    m_root->decreaseSize();
    //m_root->increaseCounter();
}

template <typename T>
void Tree<T>::increaseSize() {
    m_root->increaseSize();
}

template <typename T>
void Tree<T>::rebuild(ActionsPtr<T> actions, std::shared_ptr<std::vector<bool>> res) {
    std::vector<ElementPtr<T>> *rebuildingElements = compoundRebuildingVector(); // RIGHT
    auto result_elements = new std::vector<ElementPtr<T>>();

    int i = 0, j = 0;

        while (i < rebuildingElements->size() && j < actions->size()) {
            auto element = rebuildingElements->at(i);
            auto action = actions->at(j);

            if (action->getKey() > element->getKey()) {
                result_elements->push_back(element);
                i++;
            } else if (action->getKey() == element->getKey()) {
                if (action->getType() == Insert) {
                    // т.к. если в rebuildingElements все элементы unmarked
                    result_elements->push_back(element);
		  //  std::cout << "rebuild insert false " << action->getKey() << std::endl;
                    res->at(action->getPosition()) = false;
                    i++; j++;
                } else if (action->getType() == Remove) {
		//	std::cout << "rebuild remove " << action->getKey() << std::endl;
			res->at(action->getPosition()) = true;
                    i++; j++;
                } else if (action->getType() == Contains) {
                    result_elements->push_back(element);
                    res->at(action->getPosition()) = true;
                    i++; j++;
                }
            } else {
                if (action->getType() == Insert) {
                    result_elements->push_back(action->getElement());
		  //  std::cout << "rebuild insert true " << action->getKey() << std::endl;
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

	while (j < actions->size()) {
		auto action = actions->at(j);
		if (action->getType() == Insert) {
                    result_elements->push_back(action->getElement());
                    res->at(action->getPosition()) = true;
		    //std::cout << "rebuild insert true! " << action->getKey() << std::endl;
                } else if (action->getType() == Remove) {
                    res->at(action->getPosition()) = false;
                } else if (action->getType() == Contains) {
                    res->at(action->getPosition()) = false;
                }
		j++;
	}

	/*for (auto a: *result_elements) {
		std::cout << a->getKey() << std::endl;
	}*/

	//std::cout << "before -------------------" << std::endl;
	//print("");
    rebuild(result_elements);
	//std::cout << "after -------------------" << std::endl;
	//print("");
	delete rebuildingElements;
}

template <typename T>
void Tree<T>::rebuild() {
	//std::cout << "before -------------------" << std::endl;
	//print("");
    std::vector<ElementPtr<T>> *rebuildingElements = compoundRebuildingVector(); // RIGHT
    rebuild(rebuildingElements);
	//std::cout << "after -------------------" << std::endl;
	//print("");
}

template <typename T>
void Tree<T>::rebuild(std::vector<ElementPtr<T>> *rebuildingElements) {
	if (rebuildingElements->empty()) return;
    
    std::vector<ElementPtr<T>> newRepresentatives;
    int step = floor(sqrt(rebuildingElements->size()));

    auto childElements2 = std::make_shared<std::vector<std::vector<ElementPtr<T>>*>>();
    //auto childElements = std::make_shared<std::vector<ElementPtr<T>>>();

    m_children.clear();
    childElements2->push_back(new std::vector<ElementPtr<T>>());
    int child_id = 0;

    for (int i = 0, j = step / 2; i < rebuildingElements->size(); i++) {
        if (i == j) {
            newRepresentatives.push_back((*rebuildingElements)[i]);

	    //childElements.push_back();

            //auto newChild = std::make_shared<Tree<T>>();
            //newChild->rebuild(childElements);
            //m_children.push_back(newChild);
	    //childElements2->push_back(childElements);
		child_id++;
		childElements2->push_back(new std::vector<ElementPtr<T>>());
            //childElements->clear();

            j += step;
        } else {
		(*childElements2)[child_id]->push_back((*rebuildingElements)[i]);
            //childElements->push_back((*rebuildingElements)[i]);
        }
    }

    /*auto newChild = std::make_shared<Tree<T>>();
    newChild->rebuild(childElements);
    m_children.push_back(newChild);
    delete childElements;*/

    auto children_tmp = std::make_shared<std::vector<TreePtr<T>>>(childElements2->size());

	auto comp = [&, childElements2 = childElements2] (int i) {
         return (*childElements2)[i]->size();
        };

granularity::cstmt(p_exec_tree1, [&, rebuildingElements = rebuildingElements, childElements2 = childElements2] {
                return rebuildingElements->size() * childElements2->size();
        }, [&, childElements2 = childElements2] {
    
	//if (childElements2->size() > 100) {
	parallel_for(0, static_cast<int>(childElements2->size()), /*comp,*/ [&, childElements2 = childElements2] (int i) {
	    auto newChild = std::make_shared<Tree<T>>();
	    newChild->rebuild((*childElements2)[i]);
	    delete (*childElements2)[i];
	    (*children_tmp)[i] = newChild;
	});// } else {
    }, [&, childElements2 = childElements2] {
    for (int i = 0; i < childElements2->size(); i++) {
            auto newChild = std::make_shared<Tree<T>>();
            newChild->rebuild((*childElements2)[i]);
            delete (*childElements2)[i];
            (*children_tmp)[i] = newChild;
        }
    });

	for (auto child: *children_tmp) {
		m_children.push_back(child);
	}

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
