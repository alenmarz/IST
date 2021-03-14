#pragma once
#ifndef IST_ACTION_H
#define IST_ACTION_H

#include <memory>

enum action_type { Insert, Remove, Contains };

template <typename T>
class Action {
    ElementPtr<T> m_element;
    action_type m_type;

public:
    Action(ElementPtr<T> element, action_type type);
    ElementPtr<T> getElement();
    action_type getType();
    int getKey();
};

template <typename T>
using ActionPtr = std::shared_ptr<Action<T>>;

template <typename T>
using Actions = std::vector<std::shared_ptr<Action<T>>>;

template <typename T>
using ActionsPtr = std::shared_ptr<std::vector<std::shared_ptr<Action<T>>>>;

#endif //IST_ACTION_H

#include "Action.hpp"