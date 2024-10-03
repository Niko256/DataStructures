#pragma once

#include "Dynamic_Array.hpp"
#include <stdexcept>
#include <iostream> 

template <typename T>
class Stack {
private:
    DynamicArray<T> array;

public:
    Stack() : array() {}
    
    ~Stack() {
        array.clear();
    }

    Stack(DynamicArray<T>& arr) : array(arr) {}

    Stack(const Stack<T>& other_stack) : array(other_stack.array) {}

    T top() const;

    void pop();

    void push(const T& element);

    Stack<T> reverse();

    size_t size() const;

    Stack<T> concat(Stack<T>& other_stack);

    Stack<T> substack(size_t start_index, size_t end_index);

    bool empty() const;
    
    Stack<T>& operator=(const Stack<T>& other_stack);

    void resize(size_t new_size);

    void print_stack();
};

// ---------------------------------------------------------
template <typename T>
T Stack<T>::top() const {
    if (array.size() == 0) {
        throw std::runtime_error("Stack is empty");
    }
    return array[array.size() - 1];
}

template <typename T>
void Stack<T>::pop() {
    if (array.size() == 0) {
        throw std::runtime_error("Stack is empty");
    }
    array.pop_back();
}

template <typename T>
void Stack<T>::push(const T& element) {
    array.push_back(element);
}

template <typename T>
Stack<T> Stack<T>::reverse() {
    Stack<T> reversed_stack;
    Stack<T> temp_stack = *this;
    while (!temp_stack.empty()) {
        reversed_stack.push(temp_stack.top());
        temp_stack.pop();
    }
    return reversed_stack;
}

template <typename T>
size_t Stack<T>::size() const {
    return array.size();
}

template <typename T>
Stack<T> Stack<T>::concat(Stack<T>& other_stack) {
    if (array.size() == 0) {
        throw std::runtime_error("Stack is empty");
    }

    Stack<T> result = *this;
    while (!other_stack.empty()) {
        result.push(other_stack.top());
        other_stack.pop();
    }
    return result;
}

template <typename T>
Stack<T> Stack<T>::substack(size_t start_index, size_t end_index) {
    if (end_index > array.size() || start_index > end_index) {
        throw std::out_of_range("Index out of range");
    }
    Stack<T> temp_stack;
    for (size_t i = start_index; i <= end_index; ++i) {
        temp_stack.push(array[i]);
    }
    return temp_stack;
}

template <typename T>
bool Stack<T>::empty() const {
    return array.size() == 0;
}

template <typename T>
Stack<T>& Stack<T>::operator=(const Stack<T>& other_stack) {
    if (this != &other_stack) {
        array = other_stack.array;
    }
    return *this;
}

template <typename T>
void Stack<T>::resize(size_t new_size) {
    if (array.size() == 0) {
        throw std::runtime_error("Stack is empty");
    }
    array.resize(new_size);
}

template<typename T>
void Stack<T>::print_stack() {
    for (size_t i = 0; i < array.size(); ++i) {
        std::cout << array[array.size() - i - 1] << " ";
    }
    std::cout << std::endl;
}
