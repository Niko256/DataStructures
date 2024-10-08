#pragma once

#include "Iterators/Bidirectional.hpp"
#include "Iterators/Reverse.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>


template <typename T, typename Allocator = std::allocator<T>>
class DbList {
private:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        Node(const T& value) : data(value), prev(nullptr), next(nullptr) {}
        Node(T&& value) : data(std::move(value)), prev(nullptr), next(nullptr) {}
    };

    Node* head_;
    Node* tail_;
    size_t size_;
    Allocator allocator_;

    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    NodeAllocator node_allocator_;

public:
    using allocator_type = Allocator;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator = BidirectionalIterator<Node>;
    using const_iterator = ConstBidirectionalIterator<Node>;
    using reverse_iterator = ReverseBidirectionalIterator<iterator>;
    using const_reverse_iterator = ReverseBidirectionalIterator<const_iterator>;

    DbList() : head_(nullptr), tail_(nullptr), size_(0) {}

    DbList(const DbList& other) : head_(nullptr), tail_(nullptr), size_(0) {
        for (const auto& value : other) {
            push_back(value);
        }
    }

    DbList(DbList&& other) noexcept : head_(std::move(other.head_)), tail_(std::move(other.tail_)), size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    ~DbList() {
        clear();
    }

    DbList& operator=(const DbList& other) {
        if (this != &other) {
            clear();
            for (const auto& value : other) {
                push_back(value);
            }
        }
        return *this;
    }

    DbList& operator=(DbList&& other) noexcept {
        if (this != &other) {
            clear();

            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;

            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    iterator begin() noexcept {
        return iterator(head_);
    }

    const_iterator begin() const noexcept {
        return const_iterator(head_);
    }

    iterator end() noexcept {
        return iterator(nullptr);
    }

    const_iterator end() const noexcept {
        return const_iterator(nullptr);
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
        Node* new_node = node_allocator_.allocate(1);
        std::allocator_traits<NodeAllocator>::construct(node_allocator_, new_node, value);
        if (tail_) {
            tail_->next = new_node;
            new_node->prev = tail_;
            tail_ = new_node;
        } else {
            head_ = tail_ = new_node;
        }
        ++size_;
    }

    void push_back(T&& value) {
        Node* new_node = node_allocator_.allocate(1);
        std::allocator_traits<NodeAllocator>::construct(node_allocator_, new_node, std::move(value));
        if (tail_) {
            tail_->next = new_node;
            new_node->prev = tail_;
            tail_ = new_node;
        } else {
            head_ = tail_ = new_node;
        }
        ++size_;
    }

    void pop_back() {
        if (tail_) {
            Node* temp = tail_;
            tail_ = tail_->prev;
            if (tail_) {
                tail_->next = nullptr;
            } else {
                head_ = nullptr;
            }
            std::allocator_traits<NodeAllocator>::destroy(node_allocator_, temp);
            node_allocator_.deallocate(temp, 1);
            --size_;
        }
    }

    void push_front(const T& value) {
        Node* new_node = node_allocator_.allocate(1);
        std::allocator_traits<NodeAllocator>::construct(node_allocator_, new_node, value);
        if (head_) {
            head_->prev = new_node;
            new_node->next = head_;
            head_ = new_node;
        } else {
            head_ = tail_ = new_node;
        }
        ++size_;
    }

    void push_front(T&& value) {
        Node* new_node = node_allocator_.allocate(1);
        std::allocator_traits<NodeAllocator>::construct(node_allocator_, new_node, std::move(value));
        if (head_) {
            head_->prev = new_node;
            new_node->next = head_;
            head_ = new_node;
        } else {
            head_ = tail_ = new_node;
        }
        ++size_;
    }

    void pop_front() {
        if (head_) {
            Node* temp = head_;
            head_ = head_->next;
            if (head_) {
                head_->prev = nullptr;
            } else {
                tail_ = nullptr;
            }
            std::allocator_traits<NodeAllocator>::destroy(node_allocator_, temp);
            node_allocator_.deallocate(temp, 1);
            --size_;
        }
    }

    void clear() noexcept {
        while (head_) {
            Node* temp = head_;
            head_ = head_->next;
            std::allocator_traits<NodeAllocator>::destroy(node_allocator_, temp);
            node_allocator_.deallocate(temp, 1);
        }
        tail_ = nullptr;
        size_ = 0;
    }

    size_t size() const noexcept {
        return size_;
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    T& front() {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return head_->data;
    }

    const T& front() const {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return head_->data;
    }

    T& back() {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return tail_->data;
    }

    const T& back() const {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return tail_->data;
    }

    iterator erase(iterator pos) {
        if (pos == end()) {
            return end();
        }

        Node* node = pos.node_;
        iterator next(node->next);

        if (node->prev) {
            node->prev->next = node->next;
        } else {
            head_ = node->next;
        }

        if (node->next) {
            node->next->prev = node->prev;
        } else {
            tail_ = node->prev;
        }

        std::allocator_traits<NodeAllocator>::destroy(node_allocator_, node);
        node_allocator_.deallocate(node, 1);
        --size_;

        return next;
    }
};
