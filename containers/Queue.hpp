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
    Queue() = default;

    Queue(const Stack<T>& first_stack, const Stack<T>& second_stack) : stack_1(first_stack), stack_2(second_stack) {}

    Queue(const Queue& other) : stack_1(other.stack_1), stack_2(other.stack_2) {}

    Queue(Queue&& other) noexcept : stack_1(std::move(other.stack_1)), stack_2(std::move(other.stack_2)) {}

    ~Queue() = default;

    Queue& operator=(const Queue& other) {
        if (this != &other) {
            stack_1 = other.stack_1;
            stack_2 = other.stack_2;
        }
        return *this;
    }

    Queue& operator=(Queue&& other) noexcept {
        if (this != &other) {
            stack_1 = std::move(other.stack_1);
            stack_2 = std::move(other.stack_2);
        }
        return *this;
    }

    void enqueue(const T& item) {
        stack_1.push(item);
    }

    void enqueue(T&& item) {
        stack_1.push(std::move(item));
    }

    template <typename... Args>
    void emplace(Args&&... args) {
        stack_1.emplace(std::forward<Args>(args)...);
    }

    void dequeue() {
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

    T front() const {
        if (stack_1.empty() && stack_2.empty()) {
            throw std::runtime_error("Queue is empty");
        }
        if (stack_2.empty()) {
            Stack<T> temp_stack_1 = stack_1;
            while (!temp_stack_1.empty()) {
                stack_2.push(temp_stack_1.top());
                temp_stack_1.pop();
            }
        }
        return stack_2.top();
    }

    size_t size() const {
        return stack_1.size() + stack_2.size();
    }

    bool empty() const {
        return stack_1.empty() && stack_2.empty();
    }

    Queue reverse() const {
        if (stack_1.empty() && stack_2.empty()) {
            throw std::runtime_error("Queue is empty");
        }

        Queue reversed_queue;
        Stack<T> temp_stack = stack_1.reverse();
        while (!temp_stack.empty()) {
            reversed_queue.enqueue(temp_stack.top());
            temp_stack.pop();
        }

        temp_stack = stack_2.reverse();
        while (!temp_stack.empty()) {
            reversed_queue.enqueue(temp_stack.top());
            temp_stack.pop();
        }

        return reversed_queue;
    }

    Queue concat(Queue& other_Q) {
        if (stack_1.empty() && stack_2.empty()) {
            throw std::runtime_error("Queue is empty");
        }

        Queue concatenation = *this;
        Stack<T> temp_stack = other_Q.stack_1.reverse();
        while (!temp_stack.empty()) {
            concatenation.enqueue(temp_stack.top());
            temp_stack.pop();
        }

        temp_stack = other_Q.stack_2.reverse();
        while (!temp_stack.empty()) {
            concatenation.enqueue(temp_stack.top());
            temp_stack.pop();
        }

        return concatenation;
    }

    Queue sub_queue(size_t start_index, size_t end_index) const {
        if (start_index >= end_index || start_index >= this->size() || end_index > this->size()) {
            throw std::out_of_range("Index out of range");
        }

        Queue temp_queue = *this;
        Queue sub;

        for (size_t i = 0; i < start_index; ++i) {
            temp_queue.dequeue();
        }

        for (size_t i = start_index; i < end_index; ++i) {
            sub.enqueue(temp_queue.front());
            temp_queue.dequeue();
        }
        return sub;
    }

    void swap(Queue& other) noexcept {
        stack_1.swap(other.stack_1);
        stack_2.swap(other.stack_2);
    }

    void print_queue() const {
        Stack<T> temp_stack_1 = stack_1;
        Stack<T> temp_stack_2 = stack_2;

        while (!temp_stack_2.empty()) {
            std::cout << temp_stack_2.top() << " ";
            temp_stack_2.pop();
        }

        while (!temp_stack_1.empty()) {
            temp_stack_2.push(temp_stack_1.top());
            temp_stack_1.pop();
        }

        while (!temp_stack_2.empty()) {
            std::cout << temp_stack_2.top() << " ";
            temp_stack_2.pop();
        }

        std::cout << std::endl;
    }
};
