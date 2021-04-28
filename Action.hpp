#pragma once
#include "Action.h"

template <typename T>
Action<T>::Action(ElementPtr<T> element, action_type type) : m_element(std::move(element)), m_type(type) {}

template <typename T>
Action<T>::Action(ElementPtr<T> element, action_type type, int position)
    : m_element(std::move(element)),
    m_type(type),
    m_position(position) {}

template <typename T>
action_type Action<T>::getType() {
    return m_type;
}

template <typename T>
ElementPtr<T> Action<T>::getElement() {
    return m_element;
}

template <typename T>
int Action<T>::getKey() {
    return m_element->getKey();
}

template <typename T>
int Action<T>::getPosition() {
    return m_position;
}

