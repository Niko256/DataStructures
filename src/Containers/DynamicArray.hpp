#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

namespace data_structures::containers {

template <typename T, typename Allocator = std::allocator<T>>
class DynamicArray {
  private:

    T* data_;
    size_t size_;
    size_t capacity_;
    Allocator allocator_;

  public:
  
    using allocator_type = Allocator;
    using value_type = T;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    using reference = T&;
    using const_reference = const T&;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    DynamicArray() : data_(nullptr), size_(0), capacity_(0) {}

    template<typename U>
    DynamicArray(const DynamicArray<U>& other) : data_(nullptr), size_(0), capacity_(0) {
        reserve(other.size());
        for (const auto& item : other) {
            push_back(T(item));
        }
    }

    explicit DynamicArray(size_t n, const Allocator& alloc = Allocator()) : allocator_(alloc) {
        if (n > std::numeric_limits<size_t>::max() / sizeof(T)) {
            throw std::bad_alloc();
        }
        try {
            data_ = allocator_.allocate(n);
            size_ = n;
            capacity_ = n;
            for (size_t i = 0; i < n; ++i) {
                std::allocator_traits<Allocator>::construct(allocator_, data_ + i);
            }
        }
        catch (...) {
            throw std::bad_alloc();
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

    DynamicArray(DynamicArray&& other) noexcept : data_(std::move(other.data_)), size_(other.size_), capacity_(other.capacity_), allocator_(std::move(other.allocator_)) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    DynamicArray(size_t n, const T& value, const Allocator& alloc = Allocator()) : allocator_(alloc) {
        data_ = allocator_.allocate(n);
        size_ = n;
        capacity_ = n;
        for (size_t i = 0; i < n; ++i) {
            std::allocator_traits<Allocator>::construct(allocator_, data_ + i, value);
        }
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

    void assign(size_t n, const T& value) {
        if (n > capacity_) {
            reserve(n);
        }

        for (size_t i = 0; i < n; ++i) {
            std::allocator_traits<Allocator>::construct(allocator_, data_ + i, value);
        }
        size_ = n;
    }

    template <typename InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        size_t count = std::distance(first, last);
        
        if (count > capacity_) {
            reserve(count);
        }

        for (size_t i = 0; first != last; ++first, ++i) {
            std::allocator_traits<Allocator>::construct(allocator_, data_ + i, *first);
        }
        size_ = count;
    }

    void assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
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

    const_iterator cbegin() const noexcept {
        return data_;
    }

    iterator end() noexcept {
        return data_ + size_;
    }

    const_iterator cend() const noexcept {
        return data_ + size_;
    }
    
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
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

    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        std::allocator_traits<Allocator>::construct(allocator_, data_ + size_, std::forward<Args>(args)...);
        ++size_;
    }

    template <typename U>
    void insert(size_t index, U&& value) {
        if (index > size_) {
            throw std::out_of_range("Index out of range (insert)");
        }

        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        for (size_t i = size_; i > index; --i) {
            std::allocator_traits<Allocator>::construct(allocator_, data_ + i, std::move(data_[i - 1]));
            std::allocator_traits<Allocator>::destroy(allocator_, data_ + i - 1);
        }

        std::allocator_traits<Allocator>::construct(allocator_, data_ + index, std::forward<U>(value));
        ++size_;
    }

    template <typename U>
    iterator insert(iterator pos, U&& value) {
        size_t index = pos - begin();  
        insert(index, std::forward<U>(value));
        return begin() + index;
    }

    template <typename InputIt>
    void insert(size_t index, InputIt first, InputIt last) {
        if (index > size_) {
            throw std::out_of_range("Index out of range");
        }

        size_t count = std::distance(first, last);

        if (size_ + count > capacity_) {
            reserve(std::max(capacity_ * 2, size_ + count));
        }

        for (size_t i = size_ + count - 1; i >= index + count; --i) {
            std::allocator_traits<Allocator>::construct(allocator_, data_ + i, std::move(data_[i - count]));
            std::allocator_traits<Allocator>::destroy(allocator_, data_ + i - count);
        }

        for (size_t i = 0; i < count; ++i, ++first) {
            std::allocator_traits<Allocator>::construct(allocator_, data_ + index + i, *first);
        }

        size_ += count;
    }


    template <typename InputIt>
    iterator insert(iterator position, InputIt first, InputIt last) {
        size_t index = position - begin();
        size_t count = std::distance(first, last);

        if (size_ + count > capacity_) {
            reserve(std::max(capacity_ * 2, size_ + count));
        }

        for (size_t i = size_ + count - 1; i >= index + count && i < size_ + count; --i) {
            
            std::allocator_traits<Allocator>::construct(allocator_, data_ + i, std::move(data_[i - count]));

            std::allocator_traits<Allocator>::destroy(allocator_, data_ + i - count);
        }

        size_t i = index;
        for (; first != last; ++first, ++i) {
            std::allocator_traits<Allocator>::construct(allocator_, data_ + i, *first);
        }

        size_ += count;
        return begin() + index;
    }
    
    void insert(size_t index, std::initializer_list<T> ilist) {
        insert(index, ilist.begin(), ilist.end());
    }

    void pop_back() {
        if (size_ > 0) {
            --size_;
            std::allocator_traits<Allocator>::destroy(allocator_, data_ + size_);
        }
    }

    void erase(size_t index) {
        erase_at_index(index);
    }

    void erase(const T& value) {
        size_t i = 0;
        while (i < size_) {
            if (data_[i] == value) {
                std::allocator_traits<Allocator>::destroy(allocator_, data_ + i);
                for (size_t j = i; j < size_ - 1; ++j) {
                    std::allocator_traits<Allocator>::construct(allocator_, data_ + j, std::move(data_[j + 1]));
                    std::allocator_traits<Allocator>::destroy(allocator_, data_ + j + 1);
                }
                --size_;
            } else {
                ++i;
            }
        }
    }

    iterator erase(iterator pos) {
        const size_t index = static_cast<size_t>(pos - begin());
        erase_at_index(index);
        return begin() + index;
    }


    void erase_at_index(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }

        std::allocator_traits<Allocator>::destroy(allocator_, data_ + index);

        for (size_t i = index; i < size_ - 1; ++i) {
            std::allocator_traits<Allocator>::construct(allocator_, data_ + i, std::move(data_[i + 1]));
            std::allocator_traits<Allocator>::destroy(allocator_, data_ + i + 1);
        }

        --size_;
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
}
