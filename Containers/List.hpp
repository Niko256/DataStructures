#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>


template <typename T, typename Allocator = std::allocator<T>>
class List {
private:
    
    struct BaseNode {
        BaseNode* prev = nullptr;
        BaseNode* next = nullptr;
    };

    struct Node : BaseNode {
        T data_;
        Node(const T& value) : data_(value) {}
        Node(T&& value) : data_(std::move(value)) {}
    };

    BaseNode* head_;
    BaseNode* tail_;
    size_t size_;
    Allocator allocator_;

    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    NodeAllocator node_allocator_;

public:
    using allocator_type = Allocator;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    
    template <bool is_const>
    class Iterator {
      private:
        BaseNode* node_;

      public:
        using value_type = std::conditional_t<is_const, const T, T>;
        using ref = std::conditional_t<is_const, const T& , T&>;
        using ptr = std::conditional_t<is_const, const T*, T*>;
        using iterator_category = std::bidirectional_iterator_tag;
        using diff_type = std::ptrdiff_t;

        Iterator() : node_(nullptr) {}

        Iterator(BaseNode* node) : node_(node) {}

        ref operator*() const { return static_cast<Node*>(node_)->data_; }

        pointer operator->() const {return &static_cast<Node*>(node_)->data_; }

        Iterator& operator++() {
            node_ = node_->next;
            return *this;
        }

        Iterator& operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator& operator--() {
            node_ = node_->prev;
            return *(this);
        }

        Iterator& operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return this->node_ == *other.node_;
        }

        bool operator!=(const Iterator& other) const {
            return this->node_ != other.node_;
        }

        operator Iterator<true>() const { return Iterator<true>(node_); }

    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() noexcept { return iterator(head_->next); }
    iterator begin() const noexcept {return const_iterator(head_->next); }
    iterator end() noexcept {return iterator(head_); }
    iterator end() const noexcept { return const_iterator(head_); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    List() : head_(nullptr), tail_(nullptr), size_(0) {
        head_ = tail_ = new BaseNode();
        head_->next = head_;
        head_->prev = head_;
    }

    List(const List& other) : List() { // invoke constructor by default firstly and then copy all elements
        for (const auto& value : other) {
            push_back(value);
        }
    }

    List(List&& other) noexcept : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = other.tail_ = nullptr;
        other.size_ = 0;
    }

    ~List() {
        clear();
        delete head_;
    }

    List& operator=(const List& other) {
        if (this != &other) {
            clear();
            for (const auto& value : other) {
                push_back(value);
            }
        }
        return *this;
    }

    List& operator=(List&& other) noexcept {
        if (*this != other) {
            clear();
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;

            other.head_ = other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }


    void push_back(const T& value) {
        insert(end(), value);
    }

    void push_back(T&& value) {
        insert(end(), std::move(value));
    }

    void pop_back() {
        erase(--end());
    }

    void push_front(const T& value) {
        insert(begin(), value);
    }

    void push_front(T&& value) {
        insert(begin(), std::move(value));
    }

    void pop_front() {
        erase(begin());
    }

    void clear() noexcept {
        while (!empty()) {
            pop_front();
        }
    }

    size_t size() const noexcept { return size_; }

    bool empty() const noexcept { return size_ == 0; }

    T& front() {
        if (empty()) {
            throw std::out_of_range("List is empty!");
        }
        return static_cast<Node*>(head_->next)->data_;
    }

    const T& front() const {
        if (empty()) {
            throw std::out_of_range("List is empty!");
        }
        return static_cast<Node*>(head_->next)->data_;
    }
    
    
    T& back() {
        if (empty()) {
            throw std::out_of_range("List is empty!");
        }
        return static_cast<Node*>(tail_->prev)->data_;
                
    } 

    const T& back() const {
        if (empty()) {
            throw std::out_of_range("List is empty!");
        }
        return static_cast<Node*>(tail_->prev)->data_;
    }


    iterator erase(iterator position) {
        if (position == end()) return end();

        BaseNode* node = position.node_;
        iterator next = node->next;

        node->prev->next = node->next;
        node->next->prev = node->prev;

        std::allocator_traits<NodeAllocator>::destroy(node_allocator_, static_cast<Node*>(node));
        node_allocator_.deallocate(static_cast<Node*>(node), 1);
        --size_;

        return next;
    }

    template <typename... Args>
    iterator emplace(iterator position, Args&&... args){
        BaseNode* new_node = node_allocator_.allocate(1);
        std::allocator_traits<NodeAllocator>::construct(node_allocator_, new_node, std::forward<Args>(args)...);

        BaseNode* node = position.node_;
        new_node->next = node;
        new_node->prev = node->prev;
        node->prev->next = new_node;
        node->prev = new_node;

        ++size_;
        return iterator(new_node);
    }


    iterator insert(iterator position, const T& value) {
        return emplace(position, value);
    }

    iterator insert(iterator position, T&& value) {
        return emplace(position, std::move(value));
    }
};
