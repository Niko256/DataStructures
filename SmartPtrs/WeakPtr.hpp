#pragma once
#include "ControlBlock.hpp"
#include "SharedPtr.hpp"
#include <cstddef>
#include <utility>

template <typename T>
class WeakPtr {
  private:
    T* ptr_;
    BaseControlBlock* ctr_block_;

    // friend shared_ptr so it can access private members for construction from weak_ptr
    friend class SharedPtr<T>;

  public:
    // creates an empty weak_ptr
    constexpr WeakPtr() noexcept : ptr_(nullptr), ctr_block_(nullptr) {}

    // constructor from shared_ptr (observes its state)
    WeakPtr(const SharedPtr<T>& shared) noexcept : ptr_(shared.ptr_), ctr_block_(shared.ctr_block_) {
        if (ctr_block_) {
            ctr_block_->increment_weak();
        }
    }

    WeakPtr(const WeakPtr& other) : ptr_(other.ptr_), ctr_block_(other.ctr_block_) {
        if (ctr_block_) {
            ctr_block_->increment_weak();
        }
    }

    WeakPtr(WeakPtr&& other) noexcept : ptr_(std::exchange(other.ptr_, nullptr)), ctr_block_(std::exchange(other.ctr_block_, nullptr)) {}

    ~WeakPtr() noexcept {
        release();
    }

    WeakPtr& operator=(const WeakPtr& other) noexcept {
        if (this != &other) {
            WeakPtr tmp(other);  // exeption-safety guarantee for swap

            std::swap(ptr_, tmp.ptr_);
            std::swap(ctr_block_, tmp.ctr_block_);
        }
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        if (this != &other) {
            release();

            ptr_ = std::exchange(other.ptr_, nullptr);
            ctr_block_ = std::exchange(other.ctr_block_, nullptr);
        }
        return *this;
    }

    WeakPtr& operator=(const SharedPtr<T>& shared) noexcept {
        release();

        ptr_ = shared.ptr_;
        ctr_block_ = shared.ctr_block_;

        if (ctr_block_) {
            ctr_block_->increment_weak();
        }

        return *this;
    }

    // Attempt to acquire a shared_ptr from this weak_ptr
    // returns a valid shared_ptr if the obj is still alive, otherwise an empty shared_ptr
    SharedPtr<T> lock() const noexcept {
        if (ctr_block_ == nullptr) {
            return SharedPtr<T>();  // empty weak_ptr cannot be locked
        }

        // use try_increment_shared from control block to atomically acquire the ownership
        if (ctr_block_->try_increment_shared()) {
            return SharedPtr<T>(ptr_, ctr_block_);
        } else {
            return SharedPtr<T>();  // object has expired (shared_count became 0)
        }
    }

    size_t use_count() const noexcept {
        return ctr_block_ ? ctr_block_->use_count() : 0;
    }

    bool expired() const noexcept {
        return use_count() == 0;
    }

    // release weak_ptr to an empty state
    void reset() noexcept {
        release();
    }

  private:
    // helprr function to release observed resources during destruction or assignment
    void release() noexcept {
        if (ctr_block_ == nullptr) {
            return;
        }

        // decrement weak count. if it was the last weak reference => consider destroying the control block
        bool weak_cnt = ctr_block_->decrement_weak();

        if (weak_cnt && ctr_block_->use_count() == 0) {
            ctr_block_->destroy_self();
        }

        ptr_ = nullptr;
        ctr_block_ = nullptr;
    }
};
