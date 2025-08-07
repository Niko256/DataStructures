#pragma once

#include "Stack.hpp"
#include <iostream>
#include <stdexcept>

namespace data_structures::containers {

template <typename T>
class Queue {
  private:
    Stack<T> input_stack;
    Stack<T> output_stack;

    void transfer_elements() {
        if (output_stack.empty()) {
            while (!input_stack.empty()) {
                output_stack.push(std::move(input_stack.top()));
                input_stack.pop();
            }
        }
    }

  public:
    Queue() = default;
    ~Queue() = default;

    Queue(const Queue& other) : input_stack(other.input_stack), output_stack(other.output_stack) {}

    Queue(Queue&& other) noexcept
        : input_stack(std::move(other.input_stack)), output_stack(std::move(other.output_stack)) {}

    Queue& operator=(const Queue& other) {
        if (this != &other) {
            input_stack = other.input_stack;
            output_stack = other.output_stack;
        }
        return *this;
    }

    Queue& operator=(Queue&& other) noexcept {
        if (this != &other) {
            input_stack = std::move(other.input_stack);
            output_stack = std::move(other.output_stack);
        }
        return *this;
    }

    void enqueue(const T& item) {
        input_stack.push(item);
    }

    void enqueue(T&& item) {
        input_stack.push(std::move(item));
    }

    template <typename... Args>
    void emplace(Args&&... args) {
        input_stack.emplace(std::forward<Args>(args)...);
    }

    void dequeue() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        transfer_elements();
        output_stack.pop();
    }

    const T front() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        transfer_elements();
        return output_stack.top();
    }

    size_t size() const noexcept {
        return input_stack.size() + output_stack.size();
    }

    bool empty() const noexcept {
        return input_stack.empty() && output_stack.empty();
    }

    Queue reverse() const {
        Queue reversed;
        Stack<T> temp = output_stack;

        for (Stack<T> in_temp = input_stack; !in_temp.empty(); in_temp.pop()) {
            temp.push(in_temp.top());
        }

        while (!temp.empty()) {
            reversed.enqueue(std::move(temp.top()));
            temp.pop();
        }

        return reversed;
    }

    void swap(Queue& other) noexcept {
        input_stack.swap(other.input_stack);
        output_stack.swap(other.output_stack);
    }

    void print_queue() const {
        Stack<T> temp_output = output_stack;
        Stack<T> temp_input = input_stack;
        Stack<T> print_stack;

        while (!temp_output.empty()) {
            std::cout << temp_output.top() << " ";
            temp_output.pop();
        }

        while (!temp_input.empty()) {
            print_stack.push(temp_input.top());
            temp_input.pop();
        }

        while (!print_stack.empty()) {
            std::cout << print_stack.top() << " ";
            print_stack.pop();
        }
        std::cout << std::endl;
    }
};
}  // namespace data_structures::containers
