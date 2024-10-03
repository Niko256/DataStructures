#pragma once

#include "Dynamic_Array.hpp"
#include <stdexcept>
#include <iostream> 
#include "Binary_heap.hpp"

template <typename T>
class PriorityNode {
public:
    long priority;
    T item;

    PriorityNode() : priority(0), item(T()) {}
    PriorityNode(const long priority, const T& value) : priority(priority), item(value) {}
    PriorityNode(const PriorityNode& other_Node) : priority(other_Node.priority), item(other_Node.item) {}
    ~PriorityNode() = default;

    void change_priority(const long new_priority);
    void change_value(const T& new_value);

    bool operator > (const PriorityNode& node) const;
    bool operator >= (const PriorityNode& node) const;
    bool operator < (const PriorityNode& node) const;
    bool operator <= (const PriorityNode& node) const;
    bool operator == (const PriorityNode& node) const;
};

template <typename T>
void PriorityNode<T>::change_priority(const long new_priority) {
    this->priority = new_priority;
}

template <typename T>
void PriorityNode<T>::change_value(const T& new_value) {
    this->item = new_value;
}

template <typename T>
bool PriorityNode<T>::operator>(const PriorityNode& node) const {
    return this->priority > node.priority;
}

template <typename T>
bool PriorityNode<T>::operator>=(const PriorityNode& node) const {
    return this->priority >= node.priority;
}

template <typename T>
bool PriorityNode<T>::operator<(const PriorityNode& node) const {
    return this->priority < node.priority;
}

template <typename T>
bool PriorityNode<T>::operator<=(const PriorityNode& node) const {
    return this->priority <= node.priority;
}

template <typename T>
bool PriorityNode<T>::operator==(const PriorityNode& node) const {
    return this->priority == node.priority;
}

// ----------------------------------------------------------------------------------

template <typename T>
class PriorityQueue {
private:
    BinaryHeap<PriorityNode<T>> queue;

public:
    PriorityQueue() = default;

    void push(const long priority, const T& item);
    
    T extract_min();
    
    void Concat(const PriorityQueue<T>& other_q);
    
    DynamicArray<PriorityNode<T>> get_subqueue(size_t start_index, size_t end_index);
    
    void change_priority(const long old_priority, const long new_priority);
    
    bool empty() const;
    
    size_t size() const;

    void print_PriorityQueue();
};

template <typename T>
size_t PriorityQueue<T>::size() const {
    return this->queue.size();
}

template <typename T>
void PriorityQueue<T>::push(const long priority, const T& item) {
    this->queue.insert(PriorityNode<T>(priority, item));
}

template <typename T>
T PriorityQueue<T>::extract_min() {
    if (queue.empty()) {
        throw std::runtime_error("Queue is empty");
    }
    return this->queue.extract_min().item;
}

template <typename T>
void PriorityQueue<T>::Concat(const PriorityQueue<T>& other_q) {
    for (size_t i = 0; i < other_q.queue.size(); ++i) {
        this->queue.insert(other_q.queue.get_element(i));
    }
}

template <typename T>
DynamicArray<PriorityNode<T>> PriorityQueue<T>::get_subqueue(size_t start_index, size_t end_index) {
    if (start_index < 0 || end_index > queue.size() || start_index > end_index) {
        throw std::out_of_range("Index out of range");
    }

    DynamicArray<PriorityNode<T>> result;
    for (size_t i = start_index; i < end_index; ++i) {
        result.push_back(queue.get_element(i));
    }
    return result;
}

template <typename T>
bool PriorityQueue<T>::empty() const {
    return this->queue.empty();
}

template<typename T>
void PriorityQueue<T>::print_PriorityQueue() {
    DynamicArray<PriorityNode<T>> elements = queue.get_elements();
    for (const auto& elem : elements) {
        std::cout << elem.item << " ";
    }
    std::cout << std::endl;
}
