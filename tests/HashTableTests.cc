#include "../src/Containers/HashTable/HashTable.hpp"
#include "../src/Containers/HashTable/Hashers/CityHash.hpp"
#include "../src/Containers/HashTable/Hashers/MurmurHash.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <random>
#include <string>

template <typename Key, typename Value, typename Hash = std::hash<Key>>
using HashTable = ds::containers::HashTable<Key, Value, Hash>;

class HashTableTest : public ::testing::Test {
  protected:
    HashTable<int, std::string> table;

    std::string generateRandomString(size_t length) {
        const std::string chars =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distribution(0, chars.size() - 1);
        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            result += chars[distribution(generator)];
        }
        return result;
    }
};

struct CollisionHash {
    size_t operator()(int) const {
        return 42;
    }
};

TEST_F(HashTableTest, CollisionHandling) {
    HashTable<int, std::string, CollisionHash> collision_table;

    collision_table.emplace(1, "one");
    collision_table.emplace(2, "two");
    collision_table.emplace(3, "three");

    EXPECT_EQ(collision_table.size(), 3);

    EXPECT_EQ(collision_table.at(1), "one");
    EXPECT_EQ(collision_table.at(2), "two");
    EXPECT_EQ(collision_table.at(3), "three");

    collision_table.erase(2);
    EXPECT_EQ(collision_table.size(), 2);
    EXPECT_THROW(collision_table.at(2), std::out_of_range);

    HashTable<int, std::string> hash_table;
    hash_table[1] = "one";
    hash_table[2] = "two";
    hash_table[3] = "three";

    for (const auto& [key, value] : hash_table) {
        std::cout << "Key: " << key << ", Value: " << value << std::endl;
    }
}

TEST_F(HashTableTest, RehashOnCollision) {
    HashTable<int, std::string, CollisionHash> collision_table;

    for (int i = 0; i < 10; ++i) {
        collision_table.emplace(i, "value" + std::to_string(i));
    }

    EXPECT_GT(collision_table.bucket_count(), 7);

    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(collision_table.at(i), "value" + std::to_string(i));
    }
}

TEST_F(HashTableTest, EraseWithCollision) {
    HashTable<int, std::string, CollisionHash> collision_table;

    collision_table.emplace(1, "one");
    collision_table.emplace(2, "two");
    collision_table.emplace(3, "three");

    collision_table.erase(2);

    EXPECT_EQ(collision_table.size(), 2);
    EXPECT_THROW(collision_table.at(2), std::out_of_range);

    EXPECT_EQ(collision_table.at(1), "one");
    EXPECT_EQ(collision_table.at(3), "three");
}

TEST_F(HashTableTest, StressTest) {
    const int NUM_ELEMENTS = 100000;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        table.emplace(i, generateRandomString(10));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Insertion of " << NUM_ELEMENTS << " elements took: "
              << duration.count() << "ms" << std::endl;

    EXPECT_EQ(table.size(), NUM_ELEMENTS);
    EXPECT_LE(table.load_factor(), table.max_load_factor());
}

TEST_F(HashTableTest, HashFunctionComparison) {
    const int NUM_ELEMENTS = 50000;
    std::vector<std::string> test_data;
    test_data.reserve(NUM_ELEMENTS);

    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        test_data.push_back(generateRandomString(20));
    }

    {
        HashTable<std::string, int, std::hash<std::string>> std_hash_table;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            std_hash_table.emplace(test_data[i], i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "std::hash insertion time: " << duration.count() << "µs" << std::endl;
    }

    {
        HashTable<std::string, int, CityHash<std::string>> city_hash_table;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            city_hash_table.emplace(test_data[i], i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "CityHash insertion time: " << duration.count() << "µs" << std::endl;
    }

    {
        HashTable<std::string, int, MurmurHash3<std::string>> murmur_hash_table;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            murmur_hash_table.emplace(test_data[i], i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "MurmurHash3 insertion time: " << duration.count() << "µs" << std::endl;
    }
}

TEST_F(HashTableTest, BucketOperations) {
    const int NUM_ELEMENTS = 100;

    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        table.emplace(i, std::to_string(i));
    }

    size_t total_elements = 0;
    for (size_t i = 0; i < table.bucket_count(); ++i) {
        total_elements += table.bucket_size(i);
    }

    EXPECT_EQ(total_elements, NUM_ELEMENTS);

    double average_bucket_size = static_cast<double>(NUM_ELEMENTS) / table.bucket_count();
    double variance = 0;

    for (size_t i = 0; i < table.bucket_count(); ++i) {
        double diff = table.bucket_size(i) - average_bucket_size;
        variance += diff * diff;
    }
    variance /= table.bucket_count();

    std::cout << "Average bucket size: " << average_bucket_size << std::endl;
    std::cout << "Variance: " << variance << std::endl;
}

TEST_F(HashTableTest, DefaultConstructor) {
    EXPECT_EQ(table.size(), 0);
    EXPECT_EQ(table.bucket_count(), 7);
    EXPECT_TRUE(table.empty());
}

TEST_F(HashTableTest, ConstructorWithBucketCount) {
    HashTable<int, std::string> custom_table(16);
    EXPECT_EQ(custom_table.bucket_count(), 16);
    EXPECT_TRUE(custom_table.empty());
}

TEST_F(HashTableTest, EmplaceAndAccess) {
    auto [it1, inserted1] = table.emplace(1, "one");
    EXPECT_TRUE(inserted1);
    EXPECT_EQ(it1->data_.second_, "one");
    EXPECT_EQ(table.size(), 1);

    auto [it2, inserted2] = table.emplace(1, "another one");
    EXPECT_FALSE(inserted2);
    EXPECT_EQ(it2->data_.second_, "one");
    EXPECT_EQ(table.size(), 1);
}

TEST_F(HashTableTest, OperatorBrackets) {
    table[1] = "one";
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table[1], "one");

    table[1] = "new one";
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table[1], "new one");
}

TEST_F(HashTableTest, AtMethod) {
    table.emplace(1, "one");
    EXPECT_EQ(table.at(1), "one");

    EXPECT_THROW(table.at(2), std::out_of_range);
}

TEST_F(HashTableTest, EraseByIterator) {
    table.emplace(1, "one");
    table.emplace(2, "two");

    auto it = table.find(1);
    table.erase(it);

    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.find(1), table.end());
    EXPECT_NE(table.find(2), table.end());
}

TEST_F(HashTableTest, Rehashing) {
    float initial_load_factor = table.load_factor();

    for (int i = 0; i < 10; ++i) {
        table.emplace(i, std::to_string(i));
    }

    EXPECT_GT(table.bucket_count(), 7);
    EXPECT_LE(table.load_factor(), 0.8f);
}

TEST_F(HashTableTest, Iterators) {
    table.emplace(1, "one");
    table.emplace(2, "two");

    size_t count = 0;
    for (auto it = table.begin(); it != table.end(); ++it) {
        EXPECT_NE(it->data_.second_, "");
        ++count;
    }
    EXPECT_EQ(count, 2);
}

TEST_F(HashTableTest, LoadFactor) {
    EXPECT_FLOAT_EQ(table.load_factor(), 0.0f);

    table.emplace(1, "one");
    EXPECT_FLOAT_EQ(table.load_factor(), 1.0f / table.bucket_count());
}

TEST_F(HashTableTest, MoveConstructor) {
    table.emplace(1, "one");
    table.emplace(2, "two");

    HashTable<int, std::string> moved_table(std::move(table));

    EXPECT_EQ(moved_table.size(), 2);
    EXPECT_EQ(moved_table.at(1), "one");
    EXPECT_TRUE(table.empty());
}

TEST_F(HashTableTest, RehashStress) {
    const size_t iterations = 1000;
    std::unordered_map<int, std::string> reference;

    for (size_t i = 0; i < iterations; ++i) {
        int key = static_cast<int>(i);
        std::string value = std::to_string(i);
        table.emplace(key, value);
        reference[key] = value;

        if (i % 100 == 0) {
            table.rehash(table.bucket_count() + 1);
            for (const auto& [k, v] : reference) {
                auto it = table.find(k);
                ASSERT_NE(it, table.end());
                EXPECT_EQ(it->data_.second_, v);
            }
        }
    }
}

TEST_F(HashTableTest, CopyConstructor) {
    table.emplace(1, "one");
    table.emplace(2, "two");

    HashTable<int, std::string> copied_table(table);

    EXPECT_EQ(copied_table.size(), table.size());
    EXPECT_EQ(copied_table.at(1), table.at(1));
    EXPECT_EQ(copied_table.at(2), table.at(2));

    copied_table[1] = "modified";
    EXPECT_EQ(table.at(1), "one");
}

TEST_F(HashTableTest, MoveAssignment) {
    table.emplace(1, "one");
    HashTable<int, std::string> other_table;
    other_table = std::move(table);

    EXPECT_EQ(other_table.at(1), "one");
    EXPECT_TRUE(table.empty());
}

TEST_F(HashTableTest, EraseRange) {
    for (int i = 0; i < 10; ++i) {
        table.emplace(i, std::to_string(i));
    }

    auto start = table.find(3);
    auto end = table.find(7);
    table.erase(start, end);

    EXPECT_FALSE(table.contains(3));
    EXPECT_FALSE(table.contains(4));
    EXPECT_FALSE(table.contains(5));
    EXPECT_FALSE(table.contains(6));
    EXPECT_TRUE(table.contains(7));
}

TEST_F(HashTableTest, SearchPerformance) {
    const int NUM_ELEMENTS = 100000;
    std::vector<int> keys;

    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        keys.push_back(i);
        table.emplace(i, std::to_string(i));
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(keys.begin(), keys.end(), g);

    auto start = std::chrono::high_resolution_clock::now();
    for (int key : keys) {
        auto it = table.find(key);
        EXPECT_NE(it, table.end());
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Search of " << NUM_ELEMENTS << " elements took: "
              << duration.count() << "ms" << std::endl;
}

TEST_F(HashTableTest, LargeScaleTest) {
    const int NUM_ELEMENTS = 1000000;

    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        table.emplace(i, std::to_string(i));
    }

    EXPECT_EQ(table.size(), NUM_ELEMENTS);
    EXPECT_LE(table.load_factor(), table.max_load_factor());

    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        EXPECT_EQ(table.at(i), std::to_string(i));
    }
}

TEST_F(HashTableTest, DuplicateInsertion) {
    table.emplace(1, "one");
    table.emplace(1, "another one");

    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.at(1), "one");
}

TEST_F(HashTableTest, ClearTable) {
    for (int i = 0; i < 10; ++i) {
        table.emplace(i, std::to_string(i));
    }

    EXPECT_EQ(table.size(), 10);

    table.clear();
    EXPECT_EQ(table.size(), 0);
    EXPECT_TRUE(table.empty());
}

TEST_F(HashTableTest, RehashWithLargeBucketCount) {
    table.rehash(10000);

    EXPECT_GE(table.bucket_count(), 10000);

    for (int i = 0; i < 1000; ++i) {
        table.emplace(i, std::to_string(i));
    }

    EXPECT_LE(table.load_factor(), table.max_load_factor());
}

TEST_F(HashTableTest, EmptyIterator) {
    EXPECT_EQ(table.begin(), table.end());
}

TEST_F(HashTableTest, ModifyValueWithBrackets) {
    table[1] = "one";
    table[1] = "new one";

    EXPECT_EQ(table[1], "new one");
}

TEST_F(HashTableTest, CompareWithStdUnorderedMap) {
    const int NUM_ELEMENTS = 100000;

    std::unordered_map<int, std::string> std_map;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        std_map.emplace(i, std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto std_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        table.emplace(i, std::to_string(i));
    }
    end = std::chrono::high_resolution_clock::now();
    auto custom_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "std::unordered_map insertion time: " << std_duration.count() << "ms" << std::endl;
    std::cout << "Custom hash table insertion time: " << custom_duration.count() << "ms" << std::endl;
}

struct CustomKey {
    int id;
    std::string name;

    bool operator==(const CustomKey& other) const {
        return id == other.id && name == other.name;
    }
};

struct CustomKeyHash {
    size_t operator()(const CustomKey& key) const {
        return std::hash<int>{}(key.id) ^ std::hash<std::string>{}(key.name);
    }
};

TEST_F(HashTableTest, CustomKeyType) {
    HashTable<CustomKey, std::string, CustomKeyHash> custom_table;

    CustomKey key1{1, "one"};
    CustomKey key2{2, "two"};

    custom_table.emplace(key1, "value1");
    custom_table.emplace(key2, "value2");

    EXPECT_EQ(custom_table.at(key1), "value1");
    EXPECT_EQ(custom_table.at(key2), "value2");
}

TEST_F(HashTableTest, HashDistributionTest) {
    const int NUM_KEYS = 100;

    HashTable<int, std::string, CityHash<int>> hash_table;

    for (int i = 0; i < NUM_KEYS; ++i) {
        hash_table.emplace(i, "value" + std::to_string(i));
    }

    std::cout << "Hash values for keys:" << std::endl;
    for (int i = 0; i < NUM_KEYS; ++i) {
        size_t hash_value = hash_table.hash_function()(i);
        std::cout << "Key: " << i << ", Hash: " << hash_value << std::endl;
    }

    std::unordered_map<size_t, int> bucket_counts;
    for (int i = 0; i < NUM_KEYS; ++i) {
        size_t bucket_index = hash_table.bucket(i);
        bucket_counts[bucket_index]++;
    }

    std::cout << "Bucket distribution:" << std::endl;
    for (const auto& [bucket, count] : bucket_counts) {
        std::cout << "Bucket " << bucket << ": " << count << " keys" << std::endl;
    }

    int total_buckets = hash_table.bucket_count();
    double average_keys_per_bucket = static_cast<double>(NUM_KEYS) / total_buckets;
    double variance = 0.0;

    for (const auto& [bucket, count] : bucket_counts) {
        double diff = count - average_keys_per_bucket;
        variance += diff * diff;
    }
    variance /= total_buckets;

    std::cout << "Average keys per bucket: " << average_keys_per_bucket << std::endl;
    std::cout << "Variance: " << variance << std::endl;

    EXPECT_LT(variance, average_keys_per_bucket * average_keys_per_bucket * 4);
}
