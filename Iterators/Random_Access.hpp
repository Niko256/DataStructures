#pragma once

#include "Categories.hpp" 
#include <cstddef>

template <typename T>
class RandomAccessIterator {
private:
    T* current_;

public:
    using iterator_category = RandomAccessIteratorTag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    RandomAccessIterator(T* ptr) : current_(ptr) {}

    reference operator*() const {
        return *current_;
    }

    pointer operator->() const {
        return current_;
    }

    RandomAccessIterator& operator++() {
        ++current_;
        return *this;
    }

    RandomAccessIterator operator++(int) {
        RandomAccessIterator tmp = *this;
        ++current_;
        return tmp;
    }

    RandomAccessIterator& operator--() {
        --current_;
        return *this;
    }

    RandomAccessIterator operator--(int) {
        RandomAccessIterator tmp = *this;
        --current_;
        return tmp;
    }

    RandomAccessIterator& operator+=(difference_type n) {
        current_ += n;
        return *this;
    }

    RandomAccessIterator& operator-=(difference_type n) {
        current_ -= n;
        return *this;
    }

    RandomAccessIterator operator+(difference_type n) const {
        return RandomAccessIterator(current_ + n);
    }

    RandomAccessIterator operator-(difference_type n) const {
        return RandomAccessIterator(current_ - n);
    }

    difference_type operator-(const RandomAccessIterator& other) const {
        return current_ - other.current_;
    }

    reference operator[](difference_type n) const {
        return current_[n];
    }

    bool operator==(const RandomAccessIterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const RandomAccessIterator& other) const {
        return current_ != other.current_;
    }

    bool operator<(const RandomAccessIterator& other) const {
        return current_ < other.current_;
    }

    bool operator<=(const RandomAccessIterator& other) const {
        return current_ <= other.current_;
    }

    bool operator>(const RandomAccessIterator& other) const {
        return current_ > other.current_;
    }

    bool operator>=(const RandomAccessIterator& other) const {
        return current_ >= other.current_;
    }
};



template <typename T>
class ConstRandomAccessIterator {
private:
    const T* current_;

public:
    using iterator_category = RandomAccessIteratorTag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    ConstRandomAccessIterator(const T* ptr) : current_(ptr) {}

    reference operator*() const {
        return *current_;
    }

    pointer operator->() const {
        return current_;
    }

    ConstRandomAccessIterator& operator++() {
        ++current_;
        return *this;
    }

    ConstRandomAccessIterator operator++(int) {
        ConstRandomAccessIterator tmp = *this;
        ++current_;
        return tmp;
    }

    ConstRandomAccessIterator& operator--() {
        --current_;
        return *this;
    }

    ConstRandomAccessIterator operator--(int) {
        ConstRandomAccessIterator tmp = *this;
        --current_;
        return tmp;
    }

    ConstRandomAccessIterator& operator+=(difference_type n) {
        current_ += n;
        return *this;
    }

    ConstRandomAccessIterator& operator-=(difference_type n) {
        current_ -= n;
        return *this;
    }

    ConstRandomAccessIterator operator+(difference_type n) const {
        return ConstRandomAccessIterator(current_ + n);
    }

    ConstRandomAccessIterator operator-(difference_type n) const {
        return ConstRandomAccessIterator(current_ - n);
    }

    difference_type operator-(const ConstRandomAccessIterator& other) const {
        return current_ - other.current_;
    }

    reference operator[](difference_type n) const {
        return current_[n];
    }

    bool operator==(const ConstRandomAccessIterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const ConstRandomAccessIterator& other) const {
        return current_ != other.current_;
    }

    bool operator<(const ConstRandomAccessIterator& other) const {
        return current_ < other.current_;
    }

    bool operator<=(const ConstRandomAccessIterator& other) const {
        return current_ <= other.current_;
    }

    bool operator>(const ConstRandomAccessIterator& other) const {
        return current_ > other.current_;
    }

    bool operator>=(const ConstRandomAccessIterator& other) const {
        return current_ >= other.current_;
    }
};
