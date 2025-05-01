#pragma once
#include "DynamicArray.hpp"
#include <functional>
#include <type_traits>

template <typename T>
class PriorityNode {
public:
    long priority;
    T item;

    PriorityNode() : priority(0), item(T()) {}
    PriorityNode(const long priority, const T& value) : priority(priority), item(value) {}

    bool operator<(const PriorityNode& other) const { return priority < other.priority; }
    bool operator>(const PriorityNode& other) const { return priority > other.priority; }
    bool operator<=(const PriorityNode& other) const { return priority <= other.priority; }
    bool operator>=(const PriorityNode& other) const { return priority >= other.priority; }
    bool operator==(const PriorityNode& other) const { return priority == other.priority; }
};

template<typename T, typename Container = DynamicArray<PriorityNode<T>>,
         typename Compare = std::less<PriorityNode<T>>>
class PriorityQueue {
private:
    Container c;
    Compare comp;

    void heapify_up(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (!comp(c[parent], c[index])) break;
            std::swap(c[index], c[parent]);
            index = parent;
        }
    }

    void heapify_down(size_t index) {
        size_t size = c.size();
        while (true) {
            size_t largest = index;
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;

            if (left < size && comp(c[largest], c[left]))
                largest = left;
            if (right < size && comp(c[largest], c[right]))
                largest = right;

            if (largest == index) break;

            std::swap(c[index], c[largest]);
            index = largest;
        }
    }

public:
    // Type definitions

    using value_type = PriorityNode<T>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using iterator = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;
    using reverse_iterator = typename Container::reverse_iterator;
    using const_reverse_iterator = typename Container::const_reverse_iterator;

    // Constructors
    PriorityQueue() = default;
    explicit PriorityQueue(const Compare& compare) : comp(compare) {}

    template<typename InputIt>
    PriorityQueue(InputIt first, InputIt last, const Compare& compare = Compare()) : comp(compare) {
        while (first != last) {
            push(*first);
            ++first;
        }
    }


    PriorityQueue(std::initializer_list<value_type> init, const Compare& compare = Compare()) : comp(compare) {
        c.reserve(init.size());
        for (const auto& item : init) {
            c.push_back(item);
        }
        for (int i = (c.size() / 2) - 1; i >= 0; --i) {
            heapify_down(i);
        }
    }

    void swap(PriorityQueue& other) noexcept(std::is_nothrow_swappable_v<Container> && std::is_nothrow_swappable_v<Compare>) {
        using std::swap;
        swap(c, other.c);
        swap(comp, other.comp);
    }

    bool operator==(const PriorityQueue& other) const { return c == other.c; }
    bool operator!=(const PriorityQueue& other) const { return !(*this == other); }

    Compare value_comp() const { return comp; }

    // Iterators
    iterator begin() noexcept { return c.begin(); }
    const_iterator begin() const noexcept { return c.begin(); }
    const_iterator cbegin() const noexcept { return c.cbegin(); }
    
    iterator end() noexcept { return c.end(); }
    const_iterator end() const noexcept { return c.end(); }
    const_iterator cend() const noexcept { return c.cend(); }
    
    reverse_iterator rbegin() noexcept { return c.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return c.rbegin(); }
    
    reverse_iterator rend() noexcept { return c.rend(); }
    const_reverse_iterator rend() const noexcept { return c.rend(); }

    // Capacity
    bool empty() const noexcept { return c.empty(); }
    size_type size() const noexcept { return c.size(); }
    
    // Element access
    const_reference top() const {
        if (empty()) throw std::runtime_error("Priority queue is empty");
        return c.front();
    }

    // Modifiers
    void push(const long priority, const T& item) {
        c.push_back(value_type(priority, item));
        heapify_up(c.size() - 1);
    }

    template<typename... Args>
    void emplace(Args&&... args) {
        c.emplace_back(std::forward<Args>(args)...);
        heapify_up(c.size() - 1);
    }

    void pop() {
        if (empty()) throw std::runtime_error("Priority queue is empty");
        c[0] = std::move(c.back());
        c.pop_back();
        if (!empty()) heapify_down(0);
    }

    T extract_min() {
        if (empty()) throw std::runtime_error("Priority queue is empty");
        T result = c.front().item;
        pop();
        return result;
    }

    void clear() noexcept { c.clear(); }

    // Additional operations
    void change_priority(const long old_priority, const long new_priority) {
        for (size_t i = 0; i < c.size(); ++i) {
            if (c[i].priority == old_priority) {
                c[i].priority = new_priority;
                heapify_up(i);
                heapify_down(i);
            }
        }
    }

    void reserve(size_type new_cap) { c.reserve(new_cap); }

    void merge(PriorityQueue& other) {
        if (this == &other) return;
    
        c.reserve(c.size() + other.size());
    
        for (auto& elem : other.c) {
            c.push_back(std::move(elem));
        }
    
        for (int i = (c.size() / 2) - 1; i >= 0; --i) {
            heapify_down(i);
        }
    
        other.clear();
    }
};
