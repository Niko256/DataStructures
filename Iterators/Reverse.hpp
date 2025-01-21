#pragma once

#include <cstddef>
#include <iterator>

template <typename Iterator>
class ReverseRandomAccessIterator {
private:
    Iterator current_;

public:
    using iterator_category = std::reverse_iterator<Iterator>;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;

    ReverseRandomAccessIterator(Iterator it) : current_(it) {}

    reference operator*() const {
        Iterator tmp = current_;
        return *--tmp;
    }

    pointer operator->() const {
        Iterator tmp = current_;
        return &*--tmp;
    }

    ReverseRandomAccessIterator& operator++() {
        --current_;
        return *this;
    }

    ReverseRandomAccessIterator operator++(int) {
        ReverseRandomAccessIterator tmp = *this;
        --current_;
        return tmp;
    }

    ReverseRandomAccessIterator& operator--() {
        ++current_;
        return *this;
    }

    ReverseRandomAccessIterator operator--(int) {
        ReverseRandomAccessIterator tmp = *this;
        ++current_;
        return tmp;
    }


    ptrdiff_t operator-(const ReverseRandomAccessIterator& other) const {
        return other.current_ - current_;
    }


    bool operator==(const ReverseRandomAccessIterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const ReverseRandomAccessIterator& other) const {
        return current_ != other.current_;
    }

    bool operator<(const ReverseRandomAccessIterator& other) const {
        return current_ > other.current_;
    }

    bool operator<=(const ReverseRandomAccessIterator& other) const {
        return current_ >= other.current_;
    }

    bool operator>(const ReverseRandomAccessIterator& other) const {
        return current_ < other.current_;
    }

    bool operator>=(const ReverseRandomAccessIterator& other) const {
        return current_ <= other.current_;
    }
};

template <typename Iterator>
class ReverseBidirectionalIterator {
private:
    Iterator current_;

public:
    using iterator_category = BidirectionalIteratorTag;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;

    ReverseBidirectionalIterator(Iterator it) : current_(it) {}

    reference operator*() const {
        Iterator tmp = current_;
        return *--tmp;
    }

    pointer operator->() const {
        Iterator tmp = current_;
        return &*--tmp;
    }

    ReverseBidirectionalIterator& operator++() {
        --current_;
        return *this;
    }

    ReverseBidirectionalIterator operator++(int) {
        ReverseBidirectionalIterator tmp = *this;
        --current_;
        return tmp;
    }

    ReverseBidirectionalIterator& operator--() {
        ++current_;
        return *this;
    }

    ReverseBidirectionalIterator operator--(int) {
        ReverseBidirectionalIterator tmp = *this;
        ++current_;
        return tmp;
    }

    bool operator==(const ReverseBidirectionalIterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const ReverseBidirectionalIterator& other) const {
        return current_ != other.current_;
    }
};
