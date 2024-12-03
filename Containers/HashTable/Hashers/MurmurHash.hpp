#include <cstdint>
#include <string>
#include <string>
#include <type_traits>
#include "../../../external/smhasher/src/MurmurHash3.h"

template<typename T>
struct MurmurHash3 {
    size_t operator()(const T& key) const {
        uint64_t hash[2];
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            MurmurHash3_x64_128(&key, sizeof(T), 0, &hash);
        } else if constexpr (std::is_same_v<T, std::string>) {
            MurmurHash3_x64_128(key.c_str(), key.length(), 0, &hash);
        } else if constexpr (std::is_pointer_v<T>) {
            auto ptr_value = reinterpret_cast<std::uintptr_t>(key);
            MurmurHash3_x64_128(&ptr_value, sizeof(std::uintptr_t), 0, hash);
        } else {
            MurmurHash3_x64_128(&key, sizeof(T), 0, hash);
        }
        return hash[0];
    }
};
