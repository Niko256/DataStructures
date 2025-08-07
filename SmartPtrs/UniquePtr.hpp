#pragma once

#include <functional>
#include <type_traits>
#include <utility>

namespace data_structures::smart_ptrs {

template <typename T>
struct DefaultDelete {
    void operator()(T* ptr) const {
        static_assert(sizeof(T) > 0, "can't delete an incomplete type");
        delete ptr;
    }
};

template <typename T, typename Deleter = DefaultDelete<T>>
class UniquePtr {
  private:
    T* ptr_;
    Deleter deleter_;

    void release_and_delete() noexcept {
        if (ptr_ != nullptr) {
            deleter_(ptr_);
            ptr_ = nullptr;
        }
    }

  public:
    constexpr UniquePtr() noexcept : ptr_(nullptr), deleter_() {
        static_assert(std::is_default_constructible_v<Deleter>, "Deleter must be default-constructible for df constructor");
    }

    constexpr UniquePtr(std::nullptr_t) noexcept : ptr_(nullptr), deleter_() {
        static_assert(std::is_default_constructible_v<Deleter>, "Deleter must be default constructible for nullptr constructor.");
    }

    explicit UniquePtr(T* ptr) noexcept : ptr_(ptr), deleter_() {
        static_assert(std::is_default_constructible_v<Deleter>, "Deleter must be default constructible for raw pointer constructor.");
    }

    UniquePtr(T* ptr, Deleter del) noexcept : ptr_(ptr), deleter_(std::move(del)) {}

    UniquePtr(T* ptr, const Deleter& del) noexcept(std::is_copy_constructible_v<Deleter>) : ptr_(ptr), deleter_(del) {}

    UniquePtr(const UniquePtr& other) = delete;
    UniquePtr& operator=(const UniquePtr& other) = delete;

    UniquePtr(UniquePtr&& other) noexcept : ptr_(std::exchange(other.ptr_, nullptr)), deleter_(std::move(other.deleter_)) {}

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            release_and_delete();
            ptr_ = std::exchange(other.ptr_, nullptr);
            deleter_ = std::move(other.deleter_);
        }
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        release_and_delete();  // Delete current owned resource (if any)
        // ptr_ is already nullified by release_and_delete()
        return *this;
    }

    ~UniquePtr() noexcept {
        release_and_delete();
    }

    T* get() const noexcept {
        return ptr_;
    }

    T* release() noexcept {
        return std::exchange(ptr_, nullptr);
    }

    void reset(T* ptr = nullptr) noexcept {
        if (ptr_ != ptr) {
            release_and_delete();
            ptr_ = ptr;
        }
    }

    Deleter& get_deleter() const noexcept {
        return deleter_;
    }

    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    T& operator*() const noexcept {
        return *ptr_;
    }

    T* operator->() const noexcept {
        return ptr_;
    }

    bool operator==(std::nullptr_t) noexcept {
        return get() == nullptr;
    }

    bool operator!=(std::nullptr_t) noexcept {
        return get() != nullptr;
    }

    void swap(UniquePtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(deleter_, other.deleter_);
    }
};

template <typename T, typename D>
bool operator==(const UniquePtr<T, D>& p1, const UniquePtr<T, D>& p2) {
    return p1.get() == p2.get();
}

template <typename T, typename D>
bool operator!=(const UniquePtr<T, D>& p1, const UniquePtr<T, D>& p2) {
    return p1.get() != p2.get();
}

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}
}  // namespace data_structures::smart_ptrs
