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


template <typename Key,
         typename Value,
         typename Hash = std::hash<Key>,
         typename KeyEqual = std::equal_to<Key>,
         typename Allocator = std::allocator<Pair<Key, Value>>>
class HashTable {
  private:
    
    struct HashNode {
        Pair<const Key, Value> data_;
        size_t cached_hash_;

        HashNode(size_t hash, Key& k, Value& v) : 
            cached_hash_(hash), data_(k, v) {}

        HashNode(size_t hash, Key&& k, Value&& v) : cached_hash_(hash), data_(std::move(k), std::move(v)) {}

        Value& get_value() { return data_.second_; }

        const Value& get_value() const { return data_.second_; }

        HashNode(const HashNode& other) : data_(other.data_), cached_hash_(other.cached_hash_) {}

        HashNode(HashNode&& other) noexcept : cached_hash_(other.cached_hash_), data_(std::move(other.data_)) {}
    };

    using BaseNodeType = Pair<const Key, Value>;
    using AllocTraits = std::allocator_traits<Allocator>;
    using ListType = List<HashNode, typename AllocTraits::template rebind_alloc<BaseNodeType>>;
    using ListIterator = typename ListType::iterator;
    using ConstListIterator = typename ListType::const_iterator;
    using ListIteratorAlloc = typename AllocTraits::template rebind_alloc<ListIterator>;

    using iterator = typename ListType::template Iterator<false>;
    using const_iterator = typename ListType::template Iterator<true>;

  private:
    
//-----------------------------------------
    
    Hash hash_; 
    KeyEqual equal_; // comparator
    Allocator allocator_;
    ListType elements_;
    DynamicArray<ListIterator> hash_table_;


    size_t size_;
    size_t bucket_count_{MIN_BUCKET_COUNT};
    size_t rehash_threshold_;

//-----------------------------------------

    static constexpr float MAX_LOAD_FACTOR = 0.8f;
    static constexpr size_t MIN_BUCKET_COUNT = 7;

    void rehash(size_t count) {

        count = std::max(count, MIN_BUCKET_COUNT);
        count = std::max(count, static_cast<size_t>(std::ceil(size_ / MAX_LOAD_FACTOR)));
    
        if (count == bucket_count_) return;

        try {
            DynamicArray<ListIterator> new_table(count, elements_.end());
        
            for (auto it = elements_.begin(); it != elements_.end(); ++it) {
                size_t new_index = it->cached_hash_ % count;

                if (new_table[new_index] == elements_.end()) {
                    new_table[new_index] = it;
                }
            }

            hash_table_ = std::move(new_table);
            bucket_count_ = count;
            rehash_threshold_ = static_cast<size_t>(bucket_count_ * MAX_LOAD_FACTOR);

        } catch(const std::bad_alloc& e) {
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

//
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

    ~HashTable() = default;

//
    iterator begin() { return elements_.begin(); }

    iterator end() { return elements_.end(); }

    const_iterator begin() const { return elements_.begin(); }

    const_iterator end() const { return elements_.end(); }

//
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

            BaseNodeType tmp_pair(std::forward<Args>(args)...); 
            const size_t hash_value = hash_(tmp_pair.first_);
            size_t bucket_index = hash_value % bucket_count_;

            auto current = hash_table_[bucket_index];
            while (current != elements_.end() && current->cached_hash_ % bucket_count_ == bucket_index) {
                if (equal_(current->data_.first_, tmp_pair.first_)) {
                    return {current, false};  
                }
                ++current;
            }

            if (size_ + 1 > rehash_threshold_) {
                try {
                    size_t new_count = next_prime(bucket_count_ * 2);
                    rehash(new_count);
                    bucket_index = hash_value % bucket_count_;
                }
                catch (const std::bad_alloc& e) {
                    if (size_ >= bucket_count_) throw; 
                }
            }

            HashNode node(hash_value, 
                     std::move(const_cast<Key&>(tmp_pair.first_)), 
                     std::move(tmp_pair.second_));

            auto inserted_position = hash_table_[bucket_index];
            while (inserted_position != elements_.end() && inserted_position->cached_hash_ % bucket_count_ == bucket_index) {
                ++inserted_position;
            }


            auto inserted_it = elements_.emplace(inserted_position, std::move(node));
        
            if (hash_table_[bucket_index] == elements_.end()) {
                hash_table_[bucket_index] = inserted_it;
            }

            ++size_;
            return {inserted_it, true};
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
        if (position == elements_.end()) {
            return;
        }
        
        size_t hash_value = position->cached_hash_;
        size_t bucket_index = hash_value % bucket_count_;

        if (hash_table_[bucket_index] == position) {
            auto next = position;
            ++next;

            if (next != elements_.end() && next->cached_hash_ % bucket_count_ == bucket_index) {
                hash_table_[bucket_index] = next;
            } else {
                hash_table_[bucket_index] = elements_.end();
            }
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

    iterator find(const Key& key) const {
        const size_t hash_value = hash_(key);
        const size_t bucket_index = hash_value % bucket_count_;

        auto current = hash_table_[bucket_index];
        while (current != elements_.end() && current->cached_hash_ % bucket_count_ == bucket_index) {
            if (equal_(current->data_.first_, key)) {
                return current;
            }
            ++current;
        }
        return elements_.end();
    }

    Value& operator[](const Key& key) {
        auto it = find(key);

        if (it != elements_.end()) {
            return it->get_value();
        }

        auto [inserted_it, success] = emplace(key, Value{});
        return inserted_it->get_value();
    }

    Value& at(const Key& key) {
        auto it = find(key);

        if (it == elements_.end()) throw std::out_of_range("Key not found");

        return it->get_value();
    }

    bool contains(const Key& key) const {
        return find(key) != elements_.end();
    }

//
    
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
