#pragma once

#include "Stack.hpp" 
#include <stdexcept>
#include <iostream>

template <typename T>
class Queue {
private:
    Stack<T> stack_1;
    Stack<T> stack_2;

public:
    Queue() {}

    Queue(const Stack<T>& first_stack, const Stack<T>& second_stack) : stack_1(first_stack), stack_2(second_stack) {}

    Queue(const Queue& other) : stack_1(other.stack_1), stack_2(other.stack_2) {}

    ~Queue();

    void enqueue(const T& item);

    void dequeue();

    T front() const;

    size_t size() const;

    bool empty() const;

    Queue<T> reverse();

    Queue<T> concat(Queue<T>& other_Q);

    Queue<T> sub_queue(size_t start_index, size_t end_index);

    Queue<T>& operator=(const Queue<T>& other_Q);

    void print_queue();
};

// ------------------------------------------------------

template <typename T>
Queue<T>::~Queue() {
    while (!stack_1.empty()) {
        stack_1.pop();
    }

    while (!stack_2.empty()) {
        stack_2.pop();
    }
}

template <typename T>
void Queue<T>::enqueue(const T& item) {
    stack_1.push(item);
}

template <typename T>
void Queue<T>::dequeue() {
    if (stack_1.empty() && stack_2.empty()) {
        throw std::runtime_error("Queue is empty");
    }
    if (stack_2.empty()) {
        while (!stack_1.empty()) {
            stack_2.push(stack_1.top());
            stack_1.pop();
        }
    }
    stack_2.pop();
}

template <typename T>
T Queue<T>::front() const {
    if (stack_1.empty() && stack_2.empty()) {
        throw std::runtime_error("Queue is empty");
    }

    Stack<T> temp_stack_1 = stack_1;
    Stack<T> temp_stack_2 = stack_2;

    if (temp_stack_2.empty()) {
        while (!temp_stack_1.empty()) {
            temp_stack_2.push(temp_stack_1.top());
            temp_stack_1.pop();
        }
    }
    return temp_stack_2.top();
}

template <typename T>
size_t Queue<T>::size() const {
    return stack_1.size() + stack_2.size();
}

template <typename T>
bool Queue<T>::empty() const {
    return stack_1.empty() && stack_2.empty();
}

template <typename T>
Queue<T> Queue<T>::reverse() {
    if (stack_1.empty() && stack_2.empty()) {
        throw std::runtime_error("Queue is empty");
    }

    Queue<T> reversed_queue;
    Stack<T> temp_stack;

    while (!stack_1.empty()) {
        temp_stack.push(stack_1.top());
        stack_1.pop();
    }

    while (!stack_2.empty()) {
        temp_stack.push(stack_2.top());
        stack_2.pop();
    }

    while (!temp_stack.empty()) {
        reversed_queue.enqueue(temp_stack.top());
        temp_stack.pop();
    }

    return reversed_queue;
}

template <typename T>
Queue<T> Queue<T>::concat(Queue<T>& other_Q) {
    if (stack_1.empty()) {
        throw std::runtime_error("Queue is empty");
    }

    Queue<T> concatenation;
    Stack<T> temp_stack;

    while (!stack_1.empty()) {
        temp_stack.push(stack_1.top());
        stack_1.pop();
    }

    while (!temp_stack.empty()) {
        concatenation.enqueue(temp_stack.top());
        temp_stack.pop();
    }

    while (!other_Q.stack_1.empty()) {
        temp_stack.push(other_Q.stack_1.top());
        other_Q.stack_1.pop();
    }

    while (!temp_stack.empty()) {
        concatenation.enqueue(temp_stack.top());
        temp_stack.pop();
    }

    return concatenation;
}

template <typename T>
Queue<T> Queue<T>::sub_queue(size_t start_index, size_t end_index) {
    if (stack_1.empty()) {
        throw std::runtime_error("Queue is empty");
    }

    if (start_index >= end_index || start_index >= this->size() || end_index > this->size()) {
        throw std::out_of_range("Index out of range");
    }

    Queue<T> temp_queue(*this);
    Queue<T> sub;

    for (size_t i = 0; i < start_index; ++i) {
        temp_queue.dequeue();
    }

    for (size_t i = start_index; i < end_index; ++i) {
        sub.enqueue(temp_queue.front());
        temp_queue.dequeue();
    }
    return sub;
}

template <typename T>
Queue<T>& Queue<T>::operator=(const Queue<T>& other_Q) {
    if (this != &other_Q) {
        this->stack_1 = other_Q.stack_1;
        this->stack_2 = other_Q.stack_2;
    }
    return *this;
}

template<typename T>
void Queue<T>::print_queue() {
    Stack<T> tempStack = stack_1;
    Stack<T> temp_stack_2;
    while (!tempStack.empty()) {
        temp_stack_2.push(tempStack.top());
        tempStack.pop();
    }

    while (!temp_stack_2.empty()) {
        std::cout << temp_stack_2.top() << " ";
        temp_stack_2.pop();
    }
}
