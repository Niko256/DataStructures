#pragma once

#include <cstddef>
#include <exception>
#include <functional>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <utility>
#include "../Pair.hpp"
#include "../List.hpp"
#include "../Dynamic_Array.hpp"
#include "Hashers/CityHash.hpp"
#include "Hashers/MurmurHash.hpp"


template <typename Key, typename Value,
          typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>,
          typename Allocator = std::allocator<Pair<Key, Value>>>

class HashTable {
  private:
    struct HashNode {
        Pair<const Key, Value> data_;
        size_t cached_hash_; // Cached hash value for perfomance

        HashNode(size_t hash, Key& k, Value& v) : 
            cached_hash_(hash), data_(k, v) {}

        HashNode(size_t hash, Key&& k, Value&& v) : cached_hash_(hash), data_(std::move(k), std::move(v)) {}

        Value& get_value() { return data_.second_; }

        const Key& get_key() const { return data_.first_; }

        const Value& get_value() const { return data_.second_; }

        HashNode(const HashNode& other) : data_(other.data_), cached_hash_(other.cached_hash_) {}

        HashNode(HashNode&& other) noexcept : cached_hash_(other.cached_hash_), data_(std::move(other.data_)) {}
    };

    
    struct HashTableRef {
        const Key& first;
        Value& second;
    
        HashTableRef(const Key& k, Value& v) : first(k), second(v) {}
    };

    using BaseNodeType = Pair<const Key, Value>;
    using AllocTraits = std::allocator_traits<Allocator>;
    using ListType = List<HashNode, typename AllocTraits::template rebind_alloc<BaseNodeType>>;
    using ListIterator = typename ListType::iterator;
    using ConstListIterator = typename ListType::const_iterator;
    using ListIteratorAlloc = typename AllocTraits::template rebind_alloc<ListIterator>;

    struct iterator {
        typename ListType::iterator it_;

        using iterator_category = std::forward_iterator_tag;
        using value_type = HashNode;
        using difference_type = std::ptrdiff_t;
        using pointer = HashNode*;
        using reference = HashNode&;

        iterator(typename ListType::iterator it) : it_(it) {}

        HashTableRef operator*() && {
            return HashTableRef(it_->data_.first_, it_->data_.second_);
        }

        const HashTableRef operator*() const && {
            return HashTableRef(it_->data_.first_, it_->data_.second_);
        }

        HashTableRef& operator*() & {
            return *reinterpret_cast<HashTableRef*>(&(it_->data_));
        }

        const HashTable& operator*() const & {
            return *reinterpret_cast<const HashTableRef*>(&(it_->data_));
        }

        iterator& operator++() {
            ++it_;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return it_ == other.it_;
        }

        bool operator!=(const iterator& other) const {
            return it_ != other.it_;
        }

        HashNode* operator->() {
            return &(*it_);
        }

        const HashNode* operator->() const {
            return &(*it_);
        }

    };

    struct const_iterator {

        typename ListType::const_iterator it_;

        using iterator_category = std::forward_iterator_tag;
        using value_type = const HashNode;
        using difference_type = std::ptrdiff_t;
        using pointer = const HashNode*;
        using reference = const HashNode&;

        const_iterator(typename ListType::const_iterator it) : it_(it) {}

        const HashTableRef operator*() const {
            return HashTableRef(it_->data_.first_, it_->data_.second_);
        }

        const_iterator& operator++() {
            ++it_;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const const_iterator& other) const {
            return it_ == other.it_;
        }

        bool operator!=(const const_iterator& other) const {
            return it_ != other.it_;
        }

        
        const HashNode* operator->() const {
            return &(*it_);
        }
    };

  private:

// ----------------------------------------------

    Hash hash_; 
    KeyEqual equal_; // comparator
    Allocator allocator_;
    ListType elements_;
    DynamicArray<ListIterator> hash_table_;

    size_t size_; // Number of elements
    size_t bucket_count_{MIN_BUCKET_COUNT}; // Number of buckets 
    size_t rehash_threshold_; // Threshold for rehashing

    static constexpr float MAX_LOAD_FACTOR = 0.8f;
    static constexpr size_t MIN_BUCKET_COUNT = 7;

// -----------------------------------------------

    void rehash(size_t count) {
        // Ensure minimum bucket count and proper sizing based on load factor
        count = std::max(count, MIN_BUCKET_COUNT);
        count = std::max(count, static_cast<size_t>(std::ceil(size_ / MAX_LOAD_FACTOR)));
    
        // Early return if no resizing needed
        if (count == bucket_count_) return;

        try {
            // Create new hash table with desired size, initialized with end iterators
            DynamicArray<ListIterator> new_table(count, elements_.end());
        
            // Redistribute existing elements into new table
            for (auto it = elements_.begin(); it != elements_.end(); ++it) {
                // Calculate new index using cached hash value
                size_t new_index = it->cached_hash_ % count;

                // If bucket is empty (contains end iterator), place element
                if (new_table[new_index] == elements_.end()) {
                    new_table[new_index] = it;
                }
            }

            // Move new table into place
            hash_table_ = std::move(new_table);
            bucket_count_ = count;
            // Recalculate rehashing threshold
            rehash_threshold_ = static_cast<size_t>(bucket_count_ * MAX_LOAD_FACTOR);

        } catch(const std::bad_alloc& e) {
            // Propagate memory allocation failures
            throw;
        }
    }

  public:

    void clear() {
        elements_.clear();
        hash_table_ = DynamicArray<ListIterator>(MIN_BUCKET_COUNT, elements_.end());
        size_ = 0;
        bucket_count_ = MIN_BUCKET_COUNT;
        rehash_threshold_ = static_cast<size_t>(bucket_count_ * MAX_LOAD_FACTOR);
    }

    HashTable() : 
        bucket_count_(MIN_BUCKET_COUNT),
        size_(0), 
        rehash_threshold_(static_cast<size_t>(bucket_count_ * MAX_LOAD_FACTOR)) {
            
            hash_table_ = DynamicArray<ListIterator>(bucket_count_, elements_.end());
    }

    explicit HashTable(
            size_t bucket_count, const Hash& hash = Hash(),
            const KeyEqual& equal = KeyEqual(),
            const Allocator& alloc = Allocator()) : 
            hash_(hash), equal_(equal), allocator_(alloc),
            bucket_count_(std::max(bucket_count, MIN_BUCKET_COUNT)),
            hash_table_(bucket_count, elements_.end()), size_(0),
            rehash_threshold_(static_cast<size_t>(bucket_count_ * MAX_LOAD_FACTOR)) {}

    HashTable& operator=(const HashTable& other) {
        if (this != &other) {
            HashTable tmp(other);
            swap(tmp);
        }
        return *this;
    }

    HashTable& operator=(HashTable&& other) noexcept {
        if (this != &other) {
            hash_ = std::move(other.hash_);
            equal_ = std::move(other.equal_);
            allocator_ = std::move(other.allocator_);
            hash_table_ = std::move(other.hash_table_);
            elements_ = std::move(other.elements_);
            bucket_count_ = other.bucket_count_;
            rehash_threshold_ = other.rehash_threshold_;

            other.elements_.clear();
            other.hash_table_.clear();
            other.size_ = 0;
            other.bucket_count_ = MIN_BUCKET_COUNT;
            other.rehash_threshold_ = static_cast<size_t>(MIN_BUCKET_COUNT * MAX_LOAD_FACTOR);
            other.hash_table_.resize(MIN_BUCKET_COUNT);
        }
        return *this;
    }

    HashTable(HashTable&& other) noexcept :
        hash_(std::move(other.hash_)),
        equal_(std::move(other.equal_)),
        allocator_(std::move(other.allocator_)),
        elements_(std::move(other.elements_)),
        hash_table_(std::move(other.hash_table_)),
        size_(other.size_),
        bucket_count_(other.bucket_count_),
        rehash_threshold_(other.rehash_threshold_) {
    
            other.size_ = 0;
            other.bucket_count_ = MIN_BUCKET_COUNT;
            other.rehash_threshold_ = static_cast<size_t>(MIN_BUCKET_COUNT * MAX_LOAD_FACTOR);
    }

    HashTable(const HashTable& other) :
        hash_(other.hash_),
        equal_(other.equal_),
        allocator_(AllocTraits::select_on_container_copy_construction(other.allocator_)),
        elements_(), hash_table_(other.bucket_count_, elements_.end()),
        size_(0), bucket_count_(other.bucket_count_),
        rehash_threshold_(other.rehash_threshold_) {
    
        for (const auto& elem : other.elements_) {
            insert(elem);
        }
    }

    HashTable(const HashTable& other, const Allocator& alloc) :
        hash_(other.hash_),
        equal_(other.equal_),
        allocator_(alloc),
        elements_(), 
        hash_table_(other.bucket_count_, elements_.end()),
        size_(0),
        bucket_count_(other.bucket_count_),
        rehash_threshold_(other.rehash_threshold_) {
            for (const auto& elem : other.elements_) {
            insert(elem);
        }
    }

    HashTable(std::initializer_list<Pair<const Key, Value>> init) : HashTable() {
        for (const auto& item : init) {
            insert(item);
        }
    }

    ~HashTable() = default;

    iterator begin() { return iterator(elements_.begin()); }

    iterator end() { return iterator(elements_.end()); }

    const_iterator begin() const { return const_iterator(elements_.begin()); }

    const_iterator end() const { return const_iterator(elements_.end()); }

    void reserve(size_t sz) {
        if (sz > hash_table_.size()) {
            hash_table_.resize(sz);
            bucket_count_ = sz;
            rehash_threshold_ = static_cast<size_t>(bucket_count_ * MAX_LOAD_FACTOR);
            rehash(sz);
        }
    }

    void insert(const HashNode& node) {
        auto [it, inserted] = emplace(node); 
    }

    void insert(HashNode&& node) {
        auto [it, inserted] = emplace(std::move(node));
    }

    template <typename InputIt>
    void insert(InputIt first, InputIt second) {
        for (auto it = first; it != second; ++it) {
            insert(*(it));
        }
    }

    template <typename... Args>
    Pair<iterator, bool> emplace(Args&&... args) {
        try {
            
            // Create a temporary pair from forwarded arguments
            BaseNodeType tmp_pair(std::forward<Args>(args)...); 
            
            // Calculate hash value for the key
            const size_t hash_value = hash_(tmp_pair.first_);
            
            // Determine bucket index using modulo
            size_t bucket_index = hash_value % bucket_count_;

            // Check if key already exists in the bucket
            auto current = hash_table_[bucket_index];

            while (current != elements_.end() && current->cached_hash_ % bucket_count_ == bucket_index) {
                if (equal_(current->data_.first_, tmp_pair.first_)) {
                    return {iterator(current), false};  // Key exists, return false
                }
                ++current;
            }

            // Check if rehashing is needed (load factor exceeded)
            if (size_ + 1 > rehash_threshold_) {
                try {
                    size_t new_count = next_prime(bucket_count_ * 2);
                    rehash(new_count);
                    bucket_index = hash_value % bucket_count_;
                }
                catch (const std::bad_alloc& e) {
                    if (size_ >= bucket_count_) throw; // Rethrow if critical
                }
            }

            // Create new node with the hash value and moved data
            HashNode node(hash_value, 
                         std::move(const_cast<Key&>(tmp_pair.first_)), 
                         std::move(tmp_pair.second_));

            // Find insertion position in the bucket
            auto inserted_position = hash_table_[bucket_index];
            while (inserted_position != elements_.end() && inserted_position->cached_hash_ % bucket_count_ == bucket_index) {
                ++inserted_position;
            }

            // Insert the node into the list
            auto inserted_it = elements_.emplace(inserted_position, std::move(node));
        
            // Update bucket head if needed
            if (hash_table_[bucket_index] == elements_.end()) {
                hash_table_[bucket_index] = inserted_it;
            }

            ++size_;
            return {iterator(inserted_it), true}; // Successfully inserted

        }
        catch (const std::bad_alloc& e) {
            std::cout << "Bad alloc caught at: " << __LINE__ << std::endl;
            throw; 
        }
        catch (const std::exception& e) {
            std::cout << "Exception caught at: " << __LINE__ << std::endl;
            throw;
        }
    }

    void erase(iterator position) {
        if (position == end()) {
            return;
        }
        
        size_t hash_value = position.it_->cached_hash_;
        size_t bucket_index = hash_value % bucket_count_;

        if (hash_table_[bucket_index] == position.it_) {
            auto next = position.it_;
            ++next;

            if (next != elements_.end() && next->cached_hash_ % bucket_count_ == bucket_index) {
                hash_table_[bucket_index] = next;
            } else {
                hash_table_[bucket_index] = elements_.end();
            }
        }

        elements_.erase(position.it_);
        --size_;
    }

    void erase(const Key& key) {
        auto it = find(key);

        if (it != end()) {
            erase(it);
        }
    }

    void erase(iterator first, iterator second) {
        for (auto it = first; it != second;) {
            auto current = it++;
            erase(current);
        }
    }

    iterator find(const Key& key) {
        const size_t hash_value = hash_(key);
        const size_t bucket_index = hash_value % bucket_count_;

        auto current = hash_table_[bucket_index];
        while (current != elements_.end() && current->cached_hash_ % bucket_count_ == bucket_index) {
            if (equal_(current->data_.first_, key)) {
                return iterator(current);
            }
            ++current;
        }
        return end();
    }

    const_iterator find(const Key& key) const {
        const size_t hash_value = hash_(key);
        const size_t bucket_index = hash_value % bucket_count_;

        
        auto current = hash_table_[bucket_index];
        while (current != elements_.end() && current->cached_hash_ % bucket_count_ == bucket_index) {
            if (equal_(current->data_.first_, key)) {
                return const_iterator(current);
            }
            ++current;
        }
        return end();
    }

    Value& operator[](const Key& key) {
        auto it = find(key);

        if (it != end()) {
            return it.it_->data_.second_;
        }

        auto [inserted_it, success] = emplace(key, Value{});
        return inserted_it.it_->data_.second_;
    }

    const Value& operator[](const Key& key) const {
        auto it = find(key);

        if (it == end()) throw std::out_of_range("Key not found!");

        return it.it_->data_.second_;
    }

    Value& at(const Key& key) const {
        auto it = find(key);

        if (it == end()) throw std::out_of_range("Key not found");

        return it.it_->data_.second_;
    }

    bool contains(const Key& key) const {
        return find(key) != end();
    }

    float load_factor() const noexcept { return static_cast<float>(size_) / bucket_count_; }

    float max_load_factor() const noexcept { return MAX_LOAD_FACTOR; }

    size_t size() const noexcept { return size_; }

    size_t bucket_count() const { return bucket_count_; }

    size_t bucket_size(size_t hash_index) const {
        if (hash_index >= bucket_count_) {
            throw std::out_of_range("Invalid bucket index");
        }
        
        auto it  = hash_table_[hash_index];
        size_t count = 0;

        if (it == elements_.end()) return 0;

        while (it != elements_.end() && it->cached_hash_ % bucket_count_ == hash_index) {
            ++count;
            ++it;
        }
        return count;
    } 

    bool empty() const noexcept { return size_ == 0; }

    void swap(HashTable& other) noexcept {
        std::swap(hash_table_, other.hash_table_);
        std::swap(elements_, other.elements_);
        std::swap(hash_, other.hash_);
        std::swap(size_, other.size_);
        std::swap(bucket_count_, other.bucket_count_);
        std::swap(rehash_threshold_, other.rehash_threshold_);
        std::swap(equal_, other.equal_);

        if (AllocTraits::propagate_on_container_swap::value) {
            std::swap(allocator_,other.allocator_);
        }
    }

private:
    bool is_prime(size_t n) const noexcept {
        if (n <= 1) return false;
        if (n <= 3) return true;
        if (n % 2 == 0 || n % 3 == 0) return false;

        for (size_t i = 5; i * i <= n; i += 6) {
            if (n % i == 0 || n % (i + 2) == 0) {
                return false;
            }
        }
        return true;
    }

    size_t next_prime(size_t n) const noexcept {
        if (n <= 2) return 2;

        size_t prime = n;
        bool found = false;

        while(!found) {
            ++prime;
            if (is_prime(prime)) {
                found = true;
            }
        }
        return prime;
    }
};

