#pragma once

#include "Categories.hpp"

template <typename T>
class BidirectionalIterator {
private:
    T* current_;

public:
    using iterator_category = BidirectionalIteratorTag;
    using pointer = T*;
    using reference = T&;

    BidirectionalIterator(T* ptr) : current_(ptr) {}

    reference operator*() const {
        return *current_;
    }

    pointer operator->() const {
        return current_;
    }

    BidirectionalIterator& operator++() {
        ++current_;
        return *this;
    }

    BidirectionalIterator operator++(int) {
        BidirectionalIterator tmp = *this;
        ++current_;
        return tmp;
    }

    BidirectionalIterator& operator--() {
        --current_;
        return *this;
    }

    BidirectionalIterator operator--(int) {
        BidirectionalIterator tmp = *this;
        --current_;
        return tmp;
    }

    bool operator==(const BidirectionalIterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const BidirectionalIterator& other) const {
        return current_ != other.current_;
    }
};

template <typename T>
class ConstBidirectionalIterator {
private:
    const T* current_;

public:
    using iterator_category = BidirectionalIteratorTag;
    using pointer = const T*;
    using reference = const T&;

    ConstBidirectionalIterator(const T* ptr) : current_(ptr) {}

    reference operator*() const {
        return *current_;
    }

    pointer operator->() const {
        return current_;
    }

    ConstBidirectionalIterator& operator++() {
        ++current_;
        return *this;
    }

    ConstBidirectionalIterator operator++(int) {
        ConstBidirectionalIterator tmp = *this;
        ++current_;
        return tmp;
    }

    ConstBidirectionalIterator& operator--() {
        --current_;
        return *this;
    }

    ConstBidirectionalIterator operator--(int) {
        ConstBidirectionalIterator tmp = *this;
        --current_;
        return tmp;
    }

    bool operator==(const ConstBidirectionalIterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const ConstBidirectionalIterator& other) const {
        return current_ != other.current_;
    }
};
