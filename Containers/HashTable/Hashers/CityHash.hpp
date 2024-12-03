#pragma once
#include "../../../external/smhasher/src/City.h"
#include <string>
#include <type_traits>

template <typename T>
struct CityHash {
  private:
    size_t operator()(const T& key) const {
        if constexpr (std::is_integral_v<T>) {
            return CityHash64(reinterpret_cast<const char*>(&key), sizeof(T));
        } else if constexpr (std::is_floating_point_v<T>) {
            return CityHash64(reinterpret_cast<const char*>(&key), sizeof(T));
        } else if constexpr (std::is_same_v<T, std::string>) {
            return CityHash64(key.c_str(), key.lenght());
        }
        return 0;
    }
};
