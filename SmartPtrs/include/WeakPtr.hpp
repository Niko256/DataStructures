#pragma once
#include <cstddef>
#include "ControlBlock.hpp"
#include "SharedPtr.hpp"

template <typename T>
class WeakPtr {
private:
    T* ptr_;
    ControlBlock* ref_counter_;

public:
    WeakPtr() noexcept : ptr_(nullptr), ref_counter_(nullptr) {}

    WeakPtr(const SharedPtr<T>& shared) noexcept : ptr_(shared.ptr_), ref_counter_(shared.ref_counter_) {
        if (ref_counter_) {
            ref_counter_->IncrementWeak();
        }
    }

    WeakPtr(const WeakPtr& other) noexcept : ptr_(other.ptr_), ref_counter_(other.ref_counter_) {
        if (ref_counter_) {
            ref_counter_->IncrementWeak();
        }
    }

    WeakPtr(WeakPtr&& other) noexcept : ptr_(other.ptr_), ref_counter_(other.ref_counter_) {
        other.ptr_ = nullptr;
        other.ref_counter_ = nullptr;
    }

    ~WeakPtr() {
        release();
    }

    WeakPtr& operator=(const WeakPtr& other) noexcept {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            ref_counter_ = other.ref_counter_;
            if (ref_counter_) {
                ref_counter_->IncrementWeak();
            }
        }
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            ref_counter_ = other.ref_counter_;
            other.ptr_ = nullptr;
            other.ref_counter_ = nullptr;
        }
        return *this;
    }

    WeakPtr& operator=(const SharedPtr<T>& shared) noexcept {
        release();
        ptr_ = shared.ptr_;
        ref_counter_ = shared.ref_counter_;
        if (ref_counter_) {
            ref_counter_->IncrementWeak();
        }
        return *this;
    }

    SharedPtr<T> lock() const noexcept {
        if (ref_counter_ && ref_counter_->SharedCount() > 0) { 
            return SharedPtr<T>(*this);
        }
        return SharedPtr<T>();
    }

    size_t use_count() const noexcept {
        return ref_counter_ ? ref_counter_->SharedCount() : 0;
    }

    bool expired() const noexcept {
        return use_count() == 0;
    }

    void reset() noexcept {
        release();
    }

private:
    void release() {
        if (ref_counter_) {
            ref_counter_->DecrementWeak();
            if (ref_counter_->SharedCount() == 0 && ref_counter_->WeakCount() == 0) {
                delete ref_counter_;
            }
            ptr_ = nullptr;
            ref_counter_ = nullptr;
        }
    }

    friend class SharedPtr<T>;
};
