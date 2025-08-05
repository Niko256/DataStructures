#pragma once
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>
#include "ControlBlock.hpp"

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
        if (ptr == nullptr) { // handles nullptr explicitly without creating control block
            return;
        }

        try {
            ctr_block_ = new DefaultControlBlock<T>(ptr_);
        } catch (...) {
            delete ptr_; // preventing leaks if control block alloc fails
            throw;
        }
    }


    SharedPtr(const SharedPtr& other) noexcept : ptr_(other.ptr_), ctr_block_(other.ctr_block_) {
        if (ctr_block_) {
            ctr_block_->try_increment_shared();
        }
    }

    SharedPtr(SharedPtr&& other) : 
        ptr_(std::exchange(other.ptr_, nullptr)), 
        ctr_block_(std::exchange(other.ctr_block_, nullptr)) {} 
};
