#include <cstdint>
#include <string>
#include <string>
#include <type_traits>

// TODO - add all types and add as submodule https://github.com/aappleby/smhasher


template<typename T>
struct MurmurHash3 {
    size_t operator()(const T& key) const {
        uint32_t hash;
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            MurmurHash3_x64_128(&key, sizeof(T), 0, &hash);
        } else if constexpr (std::is_same_v<T, std::string>) {
            MurmurHash3_x64_128(key.c_str(), key.length(), 0, &hash);
        }
        return hash;
    }
};
