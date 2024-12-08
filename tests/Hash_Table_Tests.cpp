#include <gtest/gtest.h>
#include "../Containers/HashTable/Hash_Table.hpp"
#include <string>
#include <vector>

TEST(HashTableTest, DefaultConstructor) {
    HashTable<int, std::string> ht;
    EXPECT_EQ(ht.size(), 0);
    EXPECT_EQ(ht.bucket_count(), 7);
    EXPECT_TRUE(ht.empty());
}

TEST(HashTableTest, ConstructorWithBucketCount) {
    HashTable<int, std::string> ht(10);
    EXPECT_EQ(ht.size(), 0);
    EXPECT_EQ(ht.bucket_count(), 10);
    EXPECT_TRUE(ht.empty());
}

TEST(HashTableTest, CopyConstructor) {
    HashTable<int, std::string> ht;
    ht.emplace(1, "one");
    ht.emplace(2, "two");

    HashTable<int, std::string> ht_copy(ht);
    EXPECT_EQ(ht_copy.size(), 2);
    EXPECT_EQ(ht_copy.bucket_count(), ht.bucket_count());
    EXPECT_EQ(ht_copy.at(1), "one");
    EXPECT_EQ(ht_copy.at(2), "two");
}

TEST(HashTableTest, MoveConstructor) {
    HashTable<int, std::string> ht;
    ht.emplace(1, "one");
    ht.emplace(2, "two");

    HashTable<int, std::string> ht_move(std::move(ht));
    EXPECT_EQ(ht_move.size(), 2);
    EXPECT_EQ(ht_move.at(1), "one");
    EXPECT_EQ(ht_move.at(2), "two");
    EXPECT_TRUE(ht.empty());
}

// Тестирование методов вставки и поиска
TEST(HashTableTest, InsertAndFind) {
    HashTable<int, std::string> ht;
    ht.emplace(1, "one");
    ht.emplace(2, "two");

    EXPECT_EQ(ht.size(), 2);
    EXPECT_EQ(ht.at(1), "one");
    EXPECT_EQ(ht.at(2), "two");

    auto it = ht.find(1);
    EXPECT_NE(it, ht.end());
    EXPECT_EQ(it->get_value(), "one");

    it = ht.find(3);
    EXPECT_EQ(it, ht.end());
}

TEST(HashTableTest, InsertDuplicate) {
    HashTable<int, std::string> ht;
    ht.emplace(1, "one");
    auto [it, inserted] = ht.emplace(1, "another one");

    EXPECT_EQ(ht.size(), 1);
    EXPECT_FALSE(inserted);
    EXPECT_EQ(it->get_value(), "one");
}

TEST(HashTableTest, EraseByKey) {
    HashTable<int, std::string> ht;
    ht.emplace(1, "one");
    ht.emplace(2, "two");

    ht.erase(1);
    EXPECT_EQ(ht.size(), 1);
    EXPECT_EQ(ht.find(1), ht.end());
    EXPECT_NE(ht.find(2), ht.end());
}

TEST(HashTableTest, EraseByIterator) {
    HashTable<int, std::string> ht;
    ht.emplace(1, "one");
    ht.emplace(2, "two");

    auto it = ht.find(1);
    ht.erase(it);
    EXPECT_EQ(ht.size(), 1);
    EXPECT_EQ(ht.find(1), ht.end());
    EXPECT_NE(ht.find(2), ht.end());
}

TEST(HashTableTest, EraseRange) {
    HashTable<int, std::string> ht;
    ht.emplace(1, "one");
    ht.emplace(2, "two");
    ht.emplace(3, "three");

    auto first = ht.find(1);
    auto last = ht.find(3);
    ht.erase(first, last);
    EXPECT_EQ(ht.size(), 1);
    EXPECT_EQ(ht.find(1), ht.end());
    EXPECT_EQ(ht.find(2), ht.end());
    EXPECT_NE(ht.find(3), ht.end());
}

TEST(HashTableTest, Iterator) {
    HashTable<int, std::string> ht;
    ht.emplace(1, "one");
    ht.emplace(2, "two");

    auto it = ht.begin();
    EXPECT_NE(it, ht.end());
    EXPECT_EQ(it->get_value(), "one");
    ++it;
    EXPECT_NE(it, ht.end());
    EXPECT_EQ(it->get_value(), "two");
    ++it;
    EXPECT_EQ(it, ht.end());
}

TEST(HashTableTest, BucketSize) {
    HashTable<int, std::string> ht;
    ht.emplace(1, "one");
    ht.emplace(2, "two");
    ht.emplace(3, "three");

    EXPECT_EQ(ht.bucket_size(1), 1);
    EXPECT_EQ(ht.bucket_size(2), 1);
    EXPECT_EQ(ht.bucket_size(3), 1);
}

TEST(HashTableTest, LoadFactor) {
    HashTable<int, std::string> ht;
    ht.emplace(1, "one");
    ht.emplace(2, "two");
    ht.emplace(3, "three");

    EXPECT_FLOAT_EQ(ht.load_factor(), 3.0 / 7.0);
}

TEST(HashTableTest, Reserve) {
    HashTable<int, std::string> ht;
    ht.reserve(10);
    EXPECT_EQ(ht.bucket_count(), 10);
}

TEST(HashTableTest, Swap) {
    HashTable<int, std::string> ht1;
    ht1.emplace(1, "one");
    HashTable<int, std::string> ht2;
    ht2.emplace(2, "two");

    ht1.swap(ht2);
    EXPECT_EQ(ht1.size(), 1);
    EXPECT_EQ(ht1.at(2), "two");
    EXPECT_EQ(ht2.size(), 1);
    EXPECT_EQ(ht2.at(1), "one");
}

TEST(HashTableTest, Contains) {
    HashTable<int, std::string> ht;
    ht.emplace(1, "one");

    EXPECT_TRUE(ht.contains(1));
    EXPECT_FALSE(ht.contains(2));
}

TEST(HashTableTest, OperatorBracket) {
    HashTable<int, std::string> ht;
    ht[1] = "one";
    ht[2] = "two";

    EXPECT_EQ(ht.size(), 2);
    EXPECT_EQ(ht[1], "one");
    EXPECT_EQ(ht[2], "two");
}

TEST(HashTableTest, AtWithException) {
    HashTable<int, std::string> ht;
    ht.emplace(1, "one");

    EXPECT_THROW(ht.at(2), std::out_of_range);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
