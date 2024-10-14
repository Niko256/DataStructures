#pragma once
#include <cstddef>

class ControlBlock {
private:
    size_t shared_counter_;
    size_t weak_counter_;

public:
    ControlBlock() : shared_counter_(0), weak_counter_(0) {}

    ControlBlock(bool is_shared_) : shared_counter_(is_shared_ ? 1 : 0), weak_counter_(is_shared_ ? 0 : 1) {}

    void IncrementShared() noexcept {
        ++shared_counter_;
    }

    void DecrementShared() noexcept {
        if (shared_counter_ > 0) {
            --shared_counter_;
        }
    }

    size_t SharedCount() const noexcept {
        return shared_counter_;
    }

    void IncrementWeak() noexcept {
        ++weak_counter_;
    }

    void DecrementWeak() noexcept {
        if (weak_counter_ > 0) {
            --weak_counter_;
        }
    }

    size_t WeakCount() const noexcept {
        return weak_counter_;
    }
};
