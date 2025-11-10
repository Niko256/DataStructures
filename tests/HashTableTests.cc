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

    // std::string generateRandomString(size_t length) {
    //     const std::string chars =
    //         "0123456789"
    //         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    //         "abcdefghijklmnopqrstuvwxyz";
    //     std::random_device rd;
    //     std::mt19937 generator(rd());
    //     std::uniform_int_distribution<> distribution(0, chars.size() - 1);
    //     std::string result;
    //     result.reserve(length);
    //     for (size_t i = 0; i < length; ++i) {
    //         result += chars[distribution(generator)];
    //     }
    //     return result;
    // }
};

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
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
