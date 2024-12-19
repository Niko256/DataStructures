#include <gtest/gtest.h>
#include <chrono>
#include <random>
#include <string>
#include "../Containers/HashTable/Hash_Table.hpp"
#include "../Containers/HashTable/Hashers/CityHash.hpp"
#include "../Containers/HashTable/Hashers/MurmurHash.hpp"

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
    for(int i = 0; i < NUM_ELEMENTS; ++i) {
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
    
    for(int i = 0; i < NUM_ELEMENTS; ++i) {
        test_data.push_back(generateRandomString(20));
    }
    
    {
        HashTable<std::string, int, std::hash<std::string>> std_hash_table;
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < NUM_ELEMENTS; ++i) {
            std_hash_table.emplace(test_data[i], i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "std::hash insertion time: " << duration.count() << "µs" << std::endl;
    }
    
    {
        HashTable<std::string, int, CityHash<std::string>> city_hash_table;
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < NUM_ELEMENTS; ++i) {
            city_hash_table.emplace(test_data[i], i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "CityHash insertion time: " << duration.count() << "µs" << std::endl;
    }
    
    {
        HashTable<std::string, int, MurmurHash3<std::string>> murmur_hash_table;
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < NUM_ELEMENTS; ++i) {
            murmur_hash_table.emplace(test_data[i], i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "MurmurHash3 insertion time: " << duration.count() << "µs" << std::endl;
    }
}


TEST_F(HashTableTest, BucketOperations) {
    const int NUM_ELEMENTS = 100;
    
    for(int i = 0; i < NUM_ELEMENTS; ++i) {
        table.emplace(i, std::to_string(i));
    }
    
    size_t total_elements = 0;
    for(size_t i = 0; i < table.bucket_count(); ++i) {
        total_elements += table.bucket_size(i);
    }
    
    EXPECT_EQ(total_elements, NUM_ELEMENTS);
    
    double average_bucket_size = static_cast<double>(NUM_ELEMENTS) / table.bucket_count();
    double variance = 0;
    
    for(size_t i = 0; i < table.bucket_count(); ++i) {
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
    
    for(int i = 0; i < 10; ++i) {
        table.emplace(i, std::to_string(i));
    }
    
    EXPECT_GT(table.bucket_count(), 7);
    EXPECT_LE(table.load_factor(), 0.8f);
}


TEST_F(HashTableTest, Iterators) {
    table.emplace(1, "one");
    table.emplace(2, "two");
    
    size_t count = 0;
    for(auto it = table.begin(); it != table.end(); ++it) {
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
