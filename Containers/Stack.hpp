#pragma once

#include "DynamicArray.hpp"
#include <iostream>
#include <stdexcept>

namespace data_structures::containers {

template <typename T>
class Stack {
  private:
    DynamicArray<T> container_;

  public:
    Stack() = default;
    ~Stack() = default;

    Stack(const DynamicArray<T>& cont) : container_(cont) {}

    Stack(DynamicArray<T>&& cont) noexcept : container_(std::move(cont)) {}

    Stack(const Stack& other) : container_(other.container_) {}

    Stack(Stack&& other) noexcept : container_(std::move(other.container_)) {}

    Stack& operator=(const Stack& other) {
        if (this != &other) {
            container_ = other.container_;
        }
        return *this;
    }

    Stack& operator=(Stack&& other) noexcept {
        if (this != &other) {
            container_ = std::move(other.container_);
        }
        return *this;
    }

    const T& top() const {
        if (container_.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return container_.back();
    }

    T& top() {
        if (container_.empty())
            throw std::runtime_error("Stack is empty");

        return container_.back();
    }

    void pop() {
        if (container_.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        container_.pop_back();
    }

    void push(const T& element) {
        container_.push_back(element);
    }

    void push(T&& element) {
        container_.push_back(std::move(element));
    }

    template <typename... Args>
    void emplace(Args&&... args) {
        container_.emplace_back(std::forward<Args>(args)...);
    }

    Stack reverse() const {
        Stack reversed_stack;
        DynamicArray<T> temp_container = container_;
        while (!temp_container.empty()) {
            reversed_stack.push(temp_container.back());
            temp_container.pop_back();
        }
        return reversed_stack;
    }

    size_t size() const {
        return container_.size();
    }

    Stack concat(const Stack& other) const {
        Stack result = *this;
        DynamicArray<T> temp_container = other.container_;
        while (!temp_container.empty()) {
            result.push(temp_container.back());
            temp_container.pop_back();
        }
        return result;
    }

    Stack substack(size_t start_index, size_t end_index) const {
        if (end_index >= container_.size() || start_index > end_index) {
            throw std::out_of_range("Index out of range");
        }
        Stack temp_stack;
        for (size_t i = start_index; i <= end_index; ++i) {
            temp_stack.push(container_[i]);
        }
        return temp_stack;
    }

    bool empty() const {
        return container_.empty();
    }

    void resize(size_t new_size) {
        if (container_.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        container_.resize(new_size);
    }

    void print_stack() const {
        for (size_t i = 0; i < container_.size(); ++i) {
            std::cout << container_[container_.size() - i - 1] << " ";
        }
        std::cout << std::endl;
    }
};
}  // namespace data_structures::containers
