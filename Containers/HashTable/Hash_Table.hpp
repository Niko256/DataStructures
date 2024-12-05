#pragma once

#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <cmath>
#include <stdexcept>
#include <utility>
#include "../Pair.hpp"
#include "../List.hpp"
#include "../Dynamic_Array.hpp"
#include "Hashers/CityHash.hpp"
#include "Hashers/MurmurHash.hpp"


template <typename Key,
         typename Value,
         typename Hash = std::hash<Key>,
         typename KeyEqual = std::equal_to<Key>,
         typename Allocator = std::allocator<Pair<Key, Value>>>
class HashTable {
  private:

    using NodeType = Pair<const Key, Value>;
    using AllocTraits = std::allocator_traits<Allocator>;
    using ListType = List<NodeType, typename AllocTraits::template rebind_alloc<NodeType>>;
    using ListIterator = typename ListType::iterator;
    using ConstListIterator = typename ListType::const_iterator;
    using ListIteratorAlloc = typename AllocTraits::template rebind_alloc<ListIterator>;

    using iterator = typename ListType::template Iterator<false>;
    using const_iterator = typename ListType::template Iterator<true>;

  private:
    
    Hash hash_; 
    KeyEqual equal_; // comparator
    Allocator allocator_;
    DynamicArray<ListIterator> hash_table_;
    ListType elements_;

    size_t size_{0};
    size_t bucket_count_{MIN_BUCKET_COUNT};
    size_t rehash_threshold_;

    static constexpr float MAX_LOAD_FACTOR = 0.8f;
    static constexpr size_t MIN_BUCKET_COUNT = 8;

    void rehash(size_t count) {
        try {
            if (count == bucket_count_) return;

            count = std::max(count, MIN_BUCKET_COUNT);

            count = std::max(count, static_cast<size_t>(std::ceil(size_ / MAX_LOAD_FACTOR)));

            DynamicArray<ListIterator> new_table(count);

            for (auto it = elements_.begin(); it != elements_.end(); ++it) {
                size_t index = hash_(it->first_) % count;
                new_table[index] = it;
            }

            hash_table_ = std::move(new_table);
            bucket_count_ = count;
            rehash_threshold_ = static_cast<size_t>(bucket_count_ * MAX_LOAD_FACTOR);
        
        } catch(const std::exception& e) {
            throw;    
        }
    }

    void clear() {
        elements_.clear();
        hash_table_.clear();
        hash_table_.resize(MIN_BUCKET_COUNT);
        size_ = 0;
        bucket_count_ = MIN_BUCKET_COUNT;
        rehash_threshold_ = static_cast<size_t>(bucket_count_ * MAX_LOAD_FACTOR);
    }

  public:

//
    HashTable() : 
        hash_table_(bucket_count_), 
        rehash_threshold_(static_cast<size_t>(bucket_count_ * MAX_LOAD_FACTOR)) {}

    explicit HashTable(
            size_t bucket_count, const Hash& hash = Hash(),
            const KeyEqual& equal = KeyEqual(),
            const Allocator& alloc = Allocator()) : 
        hash_(hash), equal_(equal), allocator_(alloc), hash_table_(bucket_count),
            bucket_count_(std::max(bucket_count, MIN_BUCKET_COUNT)),
            rehash_threshold_(static_cast<size_t>(bucket_count_ * MAX_LOAD_FACTOR)) {}


    HashTable(const HashTable& other) : hash_(other.hash_), equal_(other.equal_), 
        allocator_(other.allocator_), hash_table_(other.bucket_count()),
        rehash_threshold_(other.rehash_threshold_), elements_(other.elements_) {
            rehash(other.bucket_count_);
    }

    HashTable(HashTable&& other) noexcept : 
        hash_(std::move(other.hash_)),
        equal_(std::move(other.equal_)),
        allocator_(std::move(other.allocator_)),
        hash_table_(std::move(other.hash_table_)), 
        bucket_count_(other.bucket_count_),
        rehash_threshold_(other.rehash_threshold_), 
        elements_(std::move(other.elements_)) {
        
            other.bucket_count_ = MIN_BUCKET_COUNT;
            other.rehash_threshold_ = static_cast<size_t>(other.bucket_count_ * MAX_LOAD_FACTOR);
    } 

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
            bucket_count_ = other.bucket_count_;
            rehash_threshold_ = other.rehash_threshold_;
            elements_ = std::move(other.elements_);

            other.elements_.clear();
            other.hash_table_.clear();
            other.bucket_count_ = MIN_BUCKET_COUNT;
            other.rehash_threshold_ = static_cast<size_t>(MIN_BUCKET_COUNT * MAX_LOAD_FACTOR);
            other.hash_table_.resize(MIN_BUCKET_COUNT);
        }
        return *this;
    }

    ~HashTable() = default;

//
    iterator begin() { return elements_.begin(); }

    iterator end() { return elements_.end(); }

    const_iterator cbegin() const { return elements_.begin(); }

    const_iterator cend() const { return elements_.end(); }

//
    void reserve(size_t sz) {
        if (sz > hash_table_.size()) {
            hash_table_.resize(sz);
            bucket_count_ = sz;
            rehash_threshold_ = static_cast<size_t>(bucket_count_ * MAX_LOAD_FACTOR);
            rehash(sz);
        }
    }

    void insert(const NodeType& node) {
        auto [it, inserted] = emplace(node); 
    }

    void insert(NodeType&& node) {
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
        auto it = elements_.emplace(elements_.end(), std::forward<Args>(args)...);
        const auto& key = it->first_;

        size_t bucket_index = hash_(key) % bucket_count_;

        auto bucket_it = hash_table_[bucket_index];
        while (bucket_it != elements_.end()) {
            if (equal_(bucket_it->first_, key)) {
                elements_.erase(it);
                return { bucket_it, false };
            }
            ++bucket_it;
        }


        if (size_ + 1 > rehash_threshold_) {
            size_t new_count = bucket_index * 2;
            rehash(new_count);

            bucket_index = hash_(key) % bucket_count_;
        }

        hash_table_[bucket_index] = it;
        ++size_;

        return {it, true};
    }

    void erase(iterator position) {
        if (position == elements_.end()) {
            return;
        }

        size_t bucket_index = hash_(position->first_) % bucket_count_;

        if (hash_table_[bucket_index] == position) {
            hash_table_[bucket_index] = elements_.end();
        }

        elements_.erase(position);
        --size_;
    }

    void erase(const Key& key) {
        auto it = find(key);

        if (it != elements_.end()) {
            erase(it);
        }
    }

    void erase(iterator first, iterator second) {
        for (auto it = first; it != second;) {
            auto current = it++;
            erase(current);
        }
    }

//
    iterator find(const Key& key) {
        size_t bucket_index = hash_(key) % bucket_count_;

        for (auto bucket_it = hash_table_[bucket_index]; bucket_it != elements_.end(); ++bucket_it) {
            if (equal_(bucket_it->first_, key)) {
                return bucket_it;
            }
        }
        return elements_.end();
    }

    Value& operator[](const Key& key) {
        auto it = find(key);

        if (it != elements_.end()) {
            return it->second_;
        }

        auto [inserted_it, success] = emplace(key, Value{});
        return inserted_it->second_;
    }

    Value& at(const Key& key) {
        auto it = find(key);

        if (it == elements_.end()) throw std::out_of_range("Key not found");

        return it->second_;
    }

    bool contains(const Key& key) const {
        return find(key) != elements_.end();
    }

//
    
    float load_factor() const noexcept { return static_cast<float>(size_) / bucket_count_; }

    size_t size() const noexcept { return size_; }

    size_t bucket_count() const { return bucket_count_; }

    size_t get_bucket(const Key& key) const noexcept { return hash_(key) % bucket_count_; }

    size_t bucket_size(size_t hash_index) const {
        size_t count = 0;

        for (auto it = hash_table_[hash_index]; it != elements_.end(); ++it) {
            if (get_bucket(it->first_) == hash_index) {
                ++count;
            }
        }
        return count;
    } 

    bool empty() const { return size_ == 0; }


    void swap(HashTable& other) {
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
};
