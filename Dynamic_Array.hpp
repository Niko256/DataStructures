#pragma once
#include <cstddef>
#include <algorithm>
#include <memory>
#include <stdexcept>

template <typename T, typename Allocator = std::allocator<T>>
class DynamicArray {
private:
    T* data_;
    size_t size_;
    size_t capacity_;
    Allocator allocator_;

public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    DynamicArray() : data_(nullptr), size_(0), capacity_(0) {}

    explicit DynamicArray(size_t n, const Allocator& alloc = Allocator()) : allocator_(alloc) {
        data_ = allocator_.allocate(n);
        size_ = n;
        capacity_ = n;
        for (size_t i = 0; i < n; ++i) {
            std::allocator_traits<Allocator>::construct(allocator_, data_ + i);
        }
    }

    DynamicArray(const DynamicArray& other) : allocator_(std::allocator_traits<Allocator>::select_on_container_copy_construction(other.allocator_)) {
        data_ = allocator_.allocate(other.capacity_);
        size_ = other.size_;
        capacity_ = other.capacity_;
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<Allocator>::construct(allocator_, data_ + i, other.data_[i]);
        }
    }

    DynamicArray(DynamicArray&& other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_), allocator_(std::move(other.allocator_)) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~DynamicArray() {
        clear();
        allocator_.deallocate(data_, capacity_);
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            clear();
            allocator_ = std::allocator_traits<Allocator>::select_on_container_copy_construction(other.allocator_);
            if (capacity_ < other.size_) {
                allocator_.deallocate(data_, capacity_);
                data_ = allocator_.allocate(other.capacity_);
                capacity_ = other.capacity_;
            }
            size_ = other.size_;
            for (size_t i = 0; i < size_; ++i) {
                std::allocator_traits<Allocator>::construct(allocator_, data_ + i, other.data_[i]);
            }
        }
        return *this;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            clear();

            allocator_.deallocate(data_, capacity_);
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            allocator_ = std::move(other.allocator_);

            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    T& operator[](size_t index) {
        return data_[index];
    }

    const T& operator[](size_t index) const {
        return data_[index];
    }

    T& at(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    const T& at(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    T& front() {
        return data_[0];
    }

    const T& front() const {
        return data_[0];
    }

    T& back() {
        return data_[size_ - 1];
    }

    const T& back() const {
        return data_[size_ - 1];
    }

    iterator begin() noexcept {
        return data_;
    }

    const_iterator begin() const noexcept {
        return data_;
    }

    iterator end() noexcept {
        return data_ + size_;
    }

    const_iterator end() const noexcept {
        return data_ + size_;
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    void push_back(const T& value) {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        std::allocator_traits<Allocator>::construct(allocator_, data_ + size_, value);
        ++size_;
    }

    void push_back(T&& value) {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        std::allocator_traits<Allocator>::construct(allocator_, data_ + size_, std::move(value));
        ++size_;
    }

    void pop_back() {
        if (size_ > 0) {
            --size_;
            std::allocator_traits<Allocator>::destroy(allocator_, data_ + size_);
        }
    }

    void reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            T* new_data = allocator_.allocate(new_capacity);

            for (size_t i = 0; i < size_; ++i) {
                std::allocator_traits<Allocator>::construct(allocator_, new_data + i, std::move(data_[i]));
                std::allocator_traits<Allocator>::destroy(allocator_, data_ + i);
            }

            allocator_.deallocate(data_, capacity_);
            data_ = new_data;
            capacity_ = new_capacity;
        }
    }

    void resize(size_t new_size) {
        if (new_size < size_) {
            for (size_t i = new_size; i < size_; ++i) {
                std::allocator_traits<Allocator>::destroy(allocator_, data_ + i);
            }
        } else if (new_size > size_) {
            if (new_size > capacity_) {
                reserve(new_size);
            }
            for (size_t i = size_; i < new_size; ++i) {
                std::allocator_traits<Allocator>::construct(allocator_, data_ + i);
            }
        }
        size_ = new_size;
    }

    void clear() noexcept {
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<Allocator>::destroy(allocator_, data_ + i);
        }
        size_ = 0;
    }

    size_t size() const noexcept {
        return size_;
    }

    size_t capacity() const noexcept {
        return capacity_;
    }

    bool empty() const noexcept {
        return size_ == 0;
    }
};
