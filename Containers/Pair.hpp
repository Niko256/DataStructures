#pragma once
#include <utility>

template <typename T1, typename T2>
class Pair {
public:
    T1 first_;
    T2 second_;

    Pair() : first_(), second_() {}

    Pair(const T1& f, const T2& s) : first_(f), second_(s) {}

    Pair(const Pair& other) : first_(other.first_), second_(other.second_) {}

    Pair(Pair&& other) noexcept : first_(std::move(other.first_)), second_(std::move(other.second_)) {}

    template <typename U, typename V>
    Pair(U&& f, V&& s) : first_(std::forward<U>(f)), second_(std::forward<V>(s)) {}

    Pair& operator=(const Pair& other) {
        if (this != &other) {
            first_ = other.first_;
            second_ = other.second_;
        }
        return *this;
    }

    Pair& operator=(Pair&& other) noexcept {
        if (this != &other) {
            first_ = std::move(other.first_);
            second_ = std::move(other.second_);
        }
        return *this;
    }

    bool operator==(const Pair& other) const {
        return first_ == other.first_ && second_ == other.second_;
    }

    bool operator!=(const Pair& other) const {
        return !(*this == other);
    }

    bool operator<(const Pair& other) const {
        return first_ < other.first_ || (first_ == other.first_ && second_ < other.second_);
    }

    bool operator<=(const Pair& other) const {
        return !(other < *this);
    }

    bool operator>(const Pair& other) const {
        return other < *this;
    }

    bool operator>=(const Pair& other) const {
        return !(*this < other);
    }


    template <std::size_t I>
    decltype(auto) get() & {
        if constexpr (I == 0) return first_;
        else if constexpr (I == 1) return second_;
    }

    template <std::size_t I>
    decltype(auto) get() const & {
        if constexpr (I == 0) return first_;
        else if constexpr (I == 1) return second_;
    }

    template <std::size_t I>
    decltype(auto) get() && {
        if constexpr (I == 0) return std::move(first_);
        else if constexpr (I == 1) return std::move(second_);
    }
};

template <typename T1, typename T2>
Pair<T1, T2> make_my_pair(T1&& first_, T2&& second_) {
    return Pair<T1, T2>(std::forward<T1>(first_), std::forward<T2>(second_));
}


namespace std {
    template<typename T1, typename T2>
    struct tuple_size<Pair<T1, T2>> : integral_constant<size_t, 2> {};

    template<size_t I, typename T1, typename T2>
    struct tuple_element<I, Pair<T1, T2>> {
        using type = typename conditional<I == 0, T1, T2>::type;
    };
}
