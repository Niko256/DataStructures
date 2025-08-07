#pragma once

#include <cstddef>
#include <iterator>

namespace data_structures::iterators {

template <typename T>
class RandomAccessIterator {
  private:
    T* current_;

  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
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

    RandomAccessIterator& operator+=(ptrdiff_t n) {
        current_ += n;
        return *this;
    }

    RandomAccessIterator& operator-=(ptrdiff_t n) {
        current_ -= n;
        return *this;
    }

    RandomAccessIterator operator+(ptrdiff_t n) const {
        return RandomAccessIterator(current_ + n);
    }

    RandomAccessIterator operator-(ptrdiff_t n) const {
        return RandomAccessIterator(current_ - n);
    }

    ptrdiff_t operator-(const RandomAccessIterator& other) const {
        return current_ - other.current_;
    }

    reference operator[](ptrdiff_t n) const {
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

    ConstRandomAccessIterator& operator+=(ptrdiff_t n) {
        current_ += n;
        return *this;
    }

    ConstRandomAccessIterator& operator-=(ptrdiff_t n) {
        current_ -= n;
        return *this;
    }

    ConstRandomAccessIterator operator+(ptrdiff_t n) const {
        return ConstRandomAccessIterator(current_ + n);
    }

    ConstRandomAccessIterator operator-(ptrdiff_t n) const {
        return ConstRandomAccessIterator(current_ - n);
    }

    ptrdiff_t operator-(const ConstRandomAccessIterator& other) const {
        return current_ - other.current_;
    }

    reference operator[](ptrdiff_t n) const {
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
}  // namespace data_structures::iterators
