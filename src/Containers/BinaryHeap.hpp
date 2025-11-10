#pragma once

#include "DynamicArray.hpp"
#include <stdexcept>

namespace ds::containers {

template <typename T>
class BinaryHeap {
  private:
    DynamicArray<T> data;

  public:
    BinaryHeap() = default;

    BinaryHeap(const BinaryHeap& other) : data(other.data) {}

    BinaryHeap(BinaryHeap&& other) noexcept : data(std::move(other.data)) {}

    ~BinaryHeap() = default;

    BinaryHeap& operator=(const BinaryHeap& other) {
        if (this != &other) {
            data = other.data;
        }
        return *this;
    }

    BinaryHeap& operator=(BinaryHeap&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
        }
        return *this;
    }

    void sift_down(size_t index);

    void sift_up(size_t index);

    void insert(const T& item);

    T extract_min();

    bool empty() const;

    size_t size() const;

    const T& get_element(size_t index) const;

    void set_element(size_t index, const T& value);

    DynamicArray<T> get_elements() const;
};

template <typename T>
void BinaryHeap<T>::sift_down(size_t index) {
    size_t left = 2 * index + 1;
    size_t right = 2 * index + 2;
    size_t smallest = index;

    if (left < data.size() && data[left] < data[smallest]) {
        smallest = left;
    }
    if (right < data.size() && data[right] < data[smallest]) {
        smallest = right;
    }
    if (smallest != index) {
        std::swap(data[index], data[smallest]);
        sift_down(smallest);
    }
}

template <typename T>
void BinaryHeap<T>::sift_up(size_t index) {
    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (data[index] >= data[parent]) {
            break;
        }
        std::swap(data[index], data[parent]);
        index = parent;
    }
}

template <typename T>
void BinaryHeap<T>::insert(const T& item) {
    data.push_back(item);
    sift_up(data.size() - 1);
}

template <typename T>
T BinaryHeap<T>::extract_min() {
    if (data.empty()) {
        throw std::runtime_error("Heap is empty");
    }
    T min_item = data[0];
    data[0] = data[data.size() - 1];
    data.pop_back();
    if (!data.empty()) {
        sift_down(0);
    }
    return min_item;
}

template <typename T>
const T& BinaryHeap<T>::get_element(size_t index) const {
    if (index >= data.size()) {
        throw std::out_of_range("Index out of range");
    }
    return data[index];
}

template <typename T>
void BinaryHeap<T>::set_element(size_t index, const T& value) {
    if (index >= data.size()) {
        throw std::out_of_range("Index out of range");
    }
    data[index] = value;
    sift_up(index);
    sift_down(index);
}

template <typename T>
bool BinaryHeap<T>::empty() const {
    return data.empty();
}

template <typename T>
size_t BinaryHeap<T>::size() const {
    return data.size();
}

template <typename T>
DynamicArray<T> BinaryHeap<T>::get_elements() const {
    DynamicArray<T> elements;
    for (size_t i = 0; i < data.size(); ++i) {
        elements.push_back(data[i]);
    }
    return elements;
}
}  // namespace ds::containers
