#pragma once
#include <stdexcept>
#include <type_traits>

template <typename T>
class UniquePtr {
private:
    T* ptr_;
    
public:
    UniquePtr(T* ptr = nullptr) : ptr_(ptr) {}

    ~UniquePtr() {
        if (ptr_) {
            delete ptr_;
        }
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // SFINAE
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    UniquePtr(UniquePtr<U>&& other_ptr) noexcept : ptr_(other_ptr.release()) {
        other_ptr.ptr_ = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other_ptr) noexcept {
        if (this != &other_ptr) {
            reset(other_ptr.release());
        }
        return *this;
    }

    T& operator*() const { 
        if (ptr_ == nullptr) {
            throw std::runtime_error("Dereferencing a nullptr");
        }
        return *ptr_; 
    }
    
    const T* operator->() const noexcept { return ptr_; }
    
    const T* get() const { return ptr_; }

    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T* new_ptr = nullptr) noexcept {
        if (ptr_) {
            delete ptr_;
        }
        ptr_ = new_ptr;
    }

    bool operator==(const UniquePtr& other) const noexcept {
        return ptr_ == other.ptr_;
    }

    bool operator!=(const UniquePtr& other) const noexcept {
        return ptr_ != other.ptr_;
    }

    // Friend declaration to allow access to private members
    template <typename U>
    friend class UniquePtr;
};

// Implementation of make_unique for types with constructor parameters
template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

// Implementation of make_unique for arrays
template <typename T>
UniquePtr<T[]> make_unique_array(std::size_t size) {
    return UniquePtr<T[]>(new T[size]());
}

