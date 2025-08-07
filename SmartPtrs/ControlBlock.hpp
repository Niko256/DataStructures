#pragma once

#include <atomic>
#include <functional>
#include <new>
#include <type_traits>
#include <utility>

namespace data_structures::smart_ptrs {

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

// Abstract base class for control block
class BaseControlBlock {
  protected:
    std::atomic<int> shared_count_;  // Number of strong references
    std::atomic<int> weak_count_;    // Number of weak references

  public:
    // Constructor initialise counts
    // for the first shared_ptr, shared_count is 1 (the pointer itself)
    // and weak count is 1 too, (the control block exists as long as there's shared_ptr or weak_ptr)
    BaseControlBlock() noexcept : shared_count_(1), weak_count_(1) {}

    // Delete copy behaviour for safety bc control blocks are managed internally and should not be copied explicitly
    BaseControlBlock(const BaseControlBlock& other) = delete;
    BaseControlBlock& operator=(const BaseControlBlock& other) = delete;

    virtual ~BaseControlBlock() = default;

    void increment_shared() noexcept {
        shared_count_.fetch_add(1, std::memory_order_relaxed);
    }

    // Decrement shared counter using acquire-release memory order
    // This function returns true if the count __was__ 1 before decrement
    // (hence it became 0 after the subtraction)
    // This one sync with memory operations that lead to object destruction
    bool decrement_shared() noexcept {
        return shared_count_.fetch_sub(1, std::memory_order_acq_rel) == 1;
    }

    // Read shared count (acquire memory order to ensure the latest value visibility)
    size_t use_count() const noexcept {
        return static_cast<size_t>(shared_count_.load(std::memory_order_acquire));
    }

    // Attempt to increment count atomically
    // Returns true if successfull => if shared_count_ was > 0 and was incremented
    bool try_increment_shared() noexcept {
        int old_count = shared_count_.load(std::memory_order_relaxed);

        for (;;) {
            if (old_count == 0) {  // hence object is already destroyed
                return false;
            }

            // atomically try to increment and if old_count has changed => loop again
            if (shared_count_.compare_exchange_weak(old_count, old_count + 1, std::memory_order_acquire, std::memory_order_relaxed)) {
                return true;
            }
        }
    }

    void increment_weak() noexcept {
        weak_count_.fetch_add(1, std::memory_order_relaxed);
    }

    // Works similarly as for shared counter
    // => returns true if weak counter was 1 __before__ decrement
    bool decrement_weak() noexcept {
        return weak_count_.fetch_sub(1, std::memory_order_acq_rel) == 1;
    }

    size_t weak_use_count() const noexcept {
        return static_cast<size_t>(weak_count_.load(std::memory_order_acquire));
    }

    // Abstract methods to destroy the managed object and the control blovk itself
    virtual void destroy_obj() noexcept = 0;   // deletes the object (e.g., delete ptr_)
    virtual void destroy_self() noexcept = 0;  // deletes control block instance (e.g., delete __this__)
};

template <typename T>
class DefaultControlBlock : public BaseControlBlock {
  private:
    T* ptr_;

  public:
    explicit DefaultControlBlock(T* ptr) noexcept : ptr_(ptr) {}

    void destroy_obj() noexcept override {
        delete ptr_;
    }

    void destroy_self() noexcept override {
        delete this;
    }
};

template <typename T>
class MakeSharedControlBlock : public BaseControlBlock {
  private:
    // storage for the object. i use aligned storage here to ensure proper alignment and size
    // the object itself is constructed in this storage using __placement new__
    typename std::aligned_storage_t<sizeof(T), alignof(T)>::type obj_storage_;

    // raw ptr to the object within object storage for convenience
    // (it's the same memory as obj_storage_, but cast to T*)
    T* ptr_;

  public:
    template <typename... Args>
    explicit MakeSharedControlBlock(Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...))) {
        // construct T in allocated storage using placement new
        ptr_ = new (&obj_storage_) T(std::forward<Args>(args)...);
    }

    // so, the memory itself will be freed when destroy_self() is called for the whole control block
    void destroy_obj() noexcept override {
        if (ptr_ != nullptr) {
            ptr_->~T();
            ptr_ = nullptr;
        }
    }

    // destroys th control block itself
    void destroy_self() noexcept override {
        // deallocates the whole block : MakeSharedControlBlock + T's storage
        delete this;
    }

    T* get_object() const noexcept {
        return ptr_;
    }
};
}  // namespace data_structures::smart_ptrs
