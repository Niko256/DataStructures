#pragma once

#include <functional>
#include <memory>
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

    void rehash();

    void clear();

  public:

//
    HashTable();

    explicit HashTable(
            size_t bucket_count, const Hash& hash = Hash(),
            const KeyEqual& equal = KeyEqual(),
            const Allocator& alloc = Allocator());

    HashTable(const HashTable& other);

    HashTable(HashTable&& other);

    HashTable& operator=(const HashTable& other);

    HashTable& operator=(HashTable&& other);

    ~HashTable();

//
    iterator begin() { return elements_.begin(); }

    iterator end() { return elements_.end(); }

    const_iterator cbegin() const { return elements_.begin(); }

    const_iterator cend() const { return elements_.end(); }

//
    void reserve(size_t chunks) {
        if (chunks > hash_table_.size()) {
            hash_table_.resize(chunks);
            rehash();
        }
    }

    void insert(const NodeType& node);

    void insert(NodeType&& node);

    template <typename InputIt>
    void insert(const InputIt& first, const InputIt& second);

    template <typename... Args>
    Pair<iterator, bool> emplace(Args&&... args);

    void erase(iterator position);

    void erase(const Key& key);

    void erase(iterator first, iterator second);

//
    iterator find(const Key& key);

    Value& operator[](const Key& key);

    Value& at(const Key& key);

    bool contains(const Key& key) const;

//
    
    float load_factor() const noexcept { return static_cast<float>(size_) / bucket_count_; }

    size_t size() const noexcept { return size_; }

    size_t bucket_count() const { return bucket_count_; }

    size_t get_bucket(const Key& key) const noexcept { return hash_(key) % bucket_count_; }

    size_t bucket_size() const;

    bool empty() const { return size_ == 0; }


    void swap(const HashTable& other) {
        std::swap(hash_table_, other.hash_table_);
        std::swap(elements_, other.elements_);
        std::swap(hash_, other.hash_);
        std::swap(equal_, other.equal_);

        if (AllocTraits::propagate_on_container_copy_assignment::value) {
            allocator_ = other.allocator_;
        }
    }
};

#include "Hash_Table.tpp"
