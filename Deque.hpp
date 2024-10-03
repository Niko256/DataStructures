#pragma once

#include <stdexcept>
#include <iostream> 
#include "DbList.hpp"


template <typename T>
class Deque {
private:
    DbList<T> list;

public:
    Deque() : list() {}

    ~Deque() {
        list.clear();
    }

    Deque(const DbList<T>& list) : list(list) {}

    Deque(const Deque<T>& other_deque);

    // ----------------------------------------------------------

    T front() const;

    T back() const;

    void push_front(const T& item);

    void push_back(const T& item);

    void pop_front();

    void pop_back();

    size_t size() const;

    bool empty() const;

    Deque<T> operator=(const Deque<T>& other_deque);

    void resize(size_t new_size);

    Deque<T> concat(const Deque<T>& other_deque);

    Deque<T> subdeque(size_t start_index, size_t end_index);

    Deque<T> reverse();

    void print_Deque();
};

// -----------------------------------------------------------

template <typename T>
Deque<T>::Deque(const Deque<T>& other_deque) : Deque<T>() {
    for (size_t i = 0; i < other_deque.list.size(); ++i) {
        this->push_back(other_deque.list.get(i));
    }
}

template <typename T>
T Deque<T>::front() const {
    if (list.empty()) {
        throw std::runtime_error("Deque is empty");
    }
    return list.front();
}

template <typename T>
T Deque<T>::back() const {
    if (list.empty()) {
        throw std::runtime_error("Deque is empty");
    }
    return list.back();
}

template <typename T>
void Deque<T>::push_front(const T& item) {
    list.push_front(item);
}

template <typename T>
void Deque<T>::push_back(const T& item) {
    list.push_back(item);
}

template <typename T>
void Deque<T>::pop_front() {
    if (list.empty()) {
        throw std::runtime_error("Deque is empty");
    }
    list.pop_front();
}

template <typename T>
void Deque<T>::pop_back() {
    if (list.empty()) {
        throw std::runtime_error("Deque is empty");
    }
    list.pop_back();
}

template <typename T>
size_t Deque<T>::size() const {
    return list.size();
}

template <typename T>
bool Deque<T>::empty() const {
    return list.empty();
}

template <typename T>
Deque<T> Deque<T>::operator=(const Deque<T>& other_deque) {
    if (this != &other_deque) {
        list = other_deque.list;
    }
    return *this;
}

template <typename T>
void Deque<T>::resize(size_t new_size) {
    list.resize(new_size);
}

template <typename T>
Deque<T> Deque<T>::concat(const Deque<T>& other_deque) {
    Deque<T> concatenation = *this;
    for (size_t i = 0; i < other_deque.size(); ++i) {
        concatenation.push_back(other_deque.list.get(i));
    }
    return concatenation;
}

template <typename T>
Deque<T> Deque<T>::subdeque(size_t start_index, size_t end_index) {
    if (start_index < 0 || end_index > list.size() || start_index > end_index) {
        throw std::out_of_range("Index out of range");
    }

    Deque<T> subdeque;
    for (size_t i = start_index; i < end_index; ++i) {
        subdeque.push_back(list.get(i));
    }
    return subdeque;
}

template <typename T>
Deque<T> Deque<T>::reverse() {
    Deque<T> reversed_deque;

    for (size_t i = 0; i < list.size(); ++i) {
        reversed_deque.push_front(list.get(i));
    }

    return reversed_deque;
}

template<typename T>
void Deque<T>::print_Deque() {
    for (size_t i = 0; i < list.size(); ++i) {
        std::cout << list.get(i) << " ";
    }
    std::cout << std::endl;
}
