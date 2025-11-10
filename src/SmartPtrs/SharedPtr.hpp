#pragma once
#include "ControlBlock.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

namespace ds::smart_ptrs {

template <typename T>
class WeakPtr;

template <typename T>
class SharedPtr {
  private:
    T* ptr_;
    BaseControlBlock* ctr_block_;

  public:
    explicit SharedPtr() noexcept : ptr_(nullptr), ctr_block_(nullptr) {}

    explicit SharedPtr(std::nullptr_t) noexcept : ptr_(nullptr), ctr_block_(nullptr) {}

    explicit SharedPtr(T* ptr) : ptr_(ptr), ctr_block_(nullptr) {
        if (ptr == nullptr) {  // handles nullptr explicitly without creating control block
            return;
        }

        try {
            ctr_block_ = new DefaultControlBlock<T>(ptr_);
        } catch (...) {
            delete ptr_;  // preventing leaks if control block alloc fails
            throw;
        }
    }

    SharedPtr(const SharedPtr& other) noexcept : ptr_(other.ptr_), ctr_block_(other.ctr_block_) {
        if (ctr_block_) {
            ctr_block_->try_increment_shared();
        }
    }

    SharedPtr(SharedPtr&& other) : ptr_(std::exchange(other.ptr_, nullptr)),
                                   ctr_block_(std::exchange(other.ctr_block_, nullptr)) {}

    // Constructor from WeakPtr::lock()
    // Tries to acquire a shared ownership from a weak_ptr
    // and if successfull => construct a valid SharedPtr. otherwise, an empty one
    SharedPtr(const WeakPtr<T>& weak) : ptr_(weak.ptr_), ctr_block_(weak.ctr_block_) {
        // if weak_ptr is null or expired => ctr_block might be nullptr
        // or if it points to a control block  but the obj is already deleted (shared_count_ == 0)
        if (ctr_block_ && ctr_block_->try_increment_shared()) {
            // successfully locked (shared_count_ was > 0 and incremented)
        } else {
            // failed to lock : object already destroyed or weak_ptr was null
            ptr_ = nullptr;
            ctr_block_ = nullptr;
        }
    }

    ~SharedPtr() noexcept {
        release();
    }

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        SharedPtr(other).swap(*this);
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        SharedPtr(std::move(other)).swap(*this);
        return *this;
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(ctr_block_, other.ctr_block_);
    }

    T* get() const noexcept {
        return ptr_;
    }

    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    size_t use_count() const noexcept {
        return ctr_block_ ? ctr_block_->use_count() : 0;
    }

    bool unique() const noexcept {
        return use_count() == 1;
    }

    T& operator*() const {
        if (!ptr_) {
            throw std::runtime_error("Attempt to deref a nullptr");
        }
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    bool operator==(std::nullptr_t) const noexcept {
        return ptr_ == nullptr;
    }

    bool operator!=(std::nullptr_t) const noexcept {
        return ptr_ != nullptr;
    }

    void reset(T* new_ptr = nullptr) {
        if (ptr_ == nullptr && (ptr_ != nullptr || new_ptr == nullptr)) {
            return;
        }

        SharedPtr temp(new_ptr);
        swap(temp);
    }

    friend class WeakPtr<T>;

    template <typename U, typename... Args>
    friend SharedPtr<U> make_shared(Args&&... args);

  private:
    void release() noexcept {
        if (ctr_block_ == nullptr) {
            return;
        }

        if (ctr_block_->decrement_shared()) {
            ctr_block_->destroy_obj();

            if (ctr_block_->decrement_weak()) {
                ctr_block_->destroy_self();
            }
        }
    }

    SharedPtr(T* ptr, BaseControlBlock* cb) noexcept : ptr_(ptr), ctr_block_(cb) {}
};

template <typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
    MakeSharedControlBlock<T>* cb = nullptr;  // the object T will be constructed inside it

    try {
        cb = new MakeSharedControlBlock<T>(std::forward<Args>(args)...);

        return SharedPtr<T>(cb->get_object(), cb);
    } catch (...) {
        delete cb;
        throw;
    }
}

template <typename T>
bool operator==(std::nullptr_t, const SharedPtr<T>& ptr) noexcept {
    return ptr == nullptr;
}

template <typename T>
bool operator!=(std::nullptr_t, const SharedPtr<T>& ptr) noexcept {
    return ptr != nullptr;
}
}  // namespace ds::smart_ptrs
