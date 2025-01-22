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
    ControlBlock* ref_counter_;

public:
    constexpr SharedPtr() noexcept : ptr_(nullptr), ref_counter_(new ControlBlock(false)) {}
    
    explicit SharedPtr(T* ptr) : ptr_(ptr), ref_counter_(new ControlBlock(true)) {}
   
    constexpr SharedPtr(std::nullptr_t) noexcept : ptr_(nullptr), ref_counter_(new ControlBlock(false)) {}

    SharedPtr(T* ptr, ControlBlock* rc) : ptr_(ptr), ref_counter_(std::move(rc)) {
        if (ref_counter_) {
            ref_counter_->IncrementShared();
        }
    }

    SharedPtr(const SharedPtr& other) : ptr_(other.ptr_), ref_counter_(other.ref_counter_) {
        if (ref_counter_) {
            ref_counter_->IncrementShared();
        }
    }

    SharedPtr(SharedPtr&& other) noexcept : ptr_(other.ptr_), ref_counter_(other.ref_counter_) {
        other.ptr_ = nullptr;
        other.ref_counter_ = nullptr;
    }

    ~SharedPtr() {
        release();
    }


    SharedPtr(const WeakPtr<T>& weak) : ptr_(weak.ptr_), ref_counter_(weak.ref_counter_) {
        if (ref_counter_ && ref_counter_->SharedCount() > 0) {
            ref_counter_->IncrementShared();
        }
        else {
            ptr_ = nullptr;
            ref_counter_ = nullptr;
        }
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            ref_counter_ = other.ref_counter_;
            if (ref_counter_) {
                ref_counter_->IncrementShared();
            }
        }
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            ref_counter_ = other.ref_counter_;
            other.ptr_ = nullptr;
            other.ref_counter_ = nullptr;
        }
        return *this;
    }

    const T* get() const noexcept {
        return ptr_;
    }

    explicit operator bool() const noexcept {
        return this->get() != nullptr;
    }

    size_t use_count() const noexcept {
        return ref_counter_ ? ref_counter_->SharedCount() : 0;
    }

    T& operator*() const {
        if (ptr_ == nullptr) {
            throw std::runtime_error("Dereferencing a nullptr");
        }
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    bool unique() const noexcept {
        return use_count() == 1;
    }

    bool operator==(const SharedPtr<T>& other) const {
        return ptr_ == other.ptr_;
    }

    
    bool operator==(std::nullptr_t) const noexcept {
        return ptr_ == nullptr;
    }

    bool operator!=(std::nullptr_t) const noexcept {
        return ptr_ != nullptr;
    }

    void reset(T* new_ptr = nullptr) {
        if (ptr_ != new_ptr) {

            ptr_ = new_ptr; 
            
            if (new_ptr) {
                ref_counter_ = new ControlBlock(true);
            }
            else {
                ref_counter_ = nullptr;
            }
        }
    }


private:
    void release() {
        if (ref_counter_) {
            ref_counter_->DecrementShared();
            if (ref_counter_->SharedCount() == 0) {
                delete ptr_;
                if (ref_counter_->WeakCount() == 0) {
                    delete ref_counter_;
                }
            }
            ptr_ = nullptr;
            ref_counter_ = nullptr;
        }
    }


    friend class WeakPtr<T>;
};


template<typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}


template <typename T>
bool operator==(std::nullptr_t, const SharedPtr<T>& ptr) noexcept {
    return ptr == nullptr;
}

template <typename T>
bool operator!=(std::nullptr_t, const SharedPtr<T>& ptr) noexcept {
    return ptr != nullptr;
}
