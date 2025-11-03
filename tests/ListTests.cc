#include "../src/Containers/List.hpp"
#include <gtest/gtest.h>
#include <string>

template <typename T>
using List = data_structures::containers::List<T>;

class ListTest : public ::testing::Test {
  protected:
    List<int> list;
    List<std::string> str_list;
};

TEST_F(ListTest, Constructors) {
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());

    list.push_back(1);
    list.push_back(2);
    List<int> copy_list(list);
    EXPECT_EQ(copy_list.size(), 2);
    EXPECT_EQ(copy_list.front(), 1);
    EXPECT_EQ(copy_list.back(), 2);

    List<int> moved_list(std::move(copy_list));
    EXPECT_EQ(moved_list.size(), 2);
    EXPECT_EQ(copy_list.size(), 0);
}

TEST_F(ListTest, AssignmentOperators) {
    list.push_back(1);
    list.push_back(2);

    List<int> copy_list;
    copy_list = list;
    EXPECT_EQ(copy_list.size(), 2);

    List<int> moved_list;
    moved_list = std::move(copy_list);
    EXPECT_EQ(moved_list.size(), 2);
    EXPECT_EQ(copy_list.size(), 0);
}

TEST_F(ListTest, PushAndPopBack) {
    // push_back
    list.push_back(1);
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.back(), 1);

    list.push_back(2);
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.back(), 2);

    list.pop_back();
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.back(), 1);
}

TEST_F(ListTest, PushAndPopFront) {
    list.push_front(1);
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.front(), 1);

    list.push_front(2);
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.front(), 2);

    list.pop_front();
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.front(), 1);
}

TEST_F(ListTest, Iterators) {
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    int expected = 1;
    for (auto it = list.begin(); it != list.end(); ++it) {
        EXPECT_EQ(*it, expected++);
    }

    const List<int>& const_list = list;
    expected = 1;
    for (auto it = const_list.begin(); it != const_list.end(); ++it) {
        EXPECT_EQ(*it, expected++);
    }

    expected = 3;
    for (auto it = list.rbegin(); it != list.rend(); ++it) {
        EXPECT_EQ(*it, expected--);
    }
}

TEST_F(ListTest, FrontAndBack) {
    EXPECT_THROW(list.front(), std::out_of_range);
    EXPECT_THROW(list.back(), std::out_of_range);

    list.push_back(1);
    list.push_back(2);

    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.back(), 2);
}

TEST_F(ListTest, Clear) {
    list.push_back(1);
    list.push_back(2);
    list.clear();
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());
}

TEST_F(ListTest, Insert) {
    list.push_back(1);
    list.push_back(3);

    auto it = ++list.begin();
    list.insert(it, 2);

    EXPECT_EQ(list.size(), 3);
    it = list.begin();
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(*++it, 2);
    EXPECT_EQ(*++it, 3);
}

TEST_F(ListTest, Erase) {
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    auto it = ++list.begin();
    it = list.erase(it);

    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.back(), 3);
}

TEST_F(ListTest, Emplace) {
    auto it = list.emplace(list.begin(), 1);
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(list.size(), 1);

    str_list.emplace(str_list.begin(), "test");
    EXPECT_EQ(str_list.front(), "test");
}

TEST_F(ListTest, ExceptionSafety) {
    struct ThrowingCopy {
        ThrowingCopy() = default;

        ThrowingCopy(const ThrowingCopy&) { throw std::runtime_error("copy error"); }
    };

    List<ThrowingCopy> throwing_list;
    ThrowingCopy obj;
    EXPECT_THROW(throwing_list.push_back(obj), std::runtime_error);
    EXPECT_EQ(throwing_list.size(), 0);
}

TEST_F(ListTest, EmplaceExtended) {
    auto it1 = list.emplace(list.begin(), 1);
    EXPECT_EQ(*it1, 1);
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.back(), 1);

    auto it2 = list.emplace(list.begin(), 2);
    EXPECT_EQ(*it2, 2);
    EXPECT_EQ(list.front(), 2);

    auto it3 = list.emplace(list.end(), 3);
    EXPECT_EQ(*it3, 3);
    EXPECT_EQ(list.back(), 3);

    auto middle = ++list.begin();
    auto it4 = list.emplace(middle, 4);
    EXPECT_EQ(*it4, 4);

    std::vector<int> expected = {2, 4, 1, 3};
    int idx = 0;
    for (const auto& val : list) {
        EXPECT_EQ(val, expected[idx++]);
    }

    struct Complex {
        int x, y;
        std::string name;

        Complex(int x_, int y_, std::string name_) : x(x_), y(y_), name(std::move(name_)) {}
    };

    List<Complex> complex_list;
    auto it5 = complex_list.emplace(complex_list.begin(), 1, 2, "test");
    EXPECT_EQ(it5->x, 1);
    EXPECT_EQ(it5->y, 2);
    EXPECT_EQ(it5->name, "test");
}

TEST_F(ListTest, EraseExtended) {
    for (int i = 1; i <= 5; ++i) {
        list.push_back(i);
    }

    auto it1 = list.erase(list.begin());
    EXPECT_EQ(*it1, 2);
    EXPECT_EQ(list.front(), 2);

    auto it2 = list.erase(--list.end());
    EXPECT_EQ(it2, list.end());
    EXPECT_EQ(list.back(), 4);

    auto middle = ++list.begin();
    auto it3 = list.erase(middle);
    EXPECT_EQ(*it3, 4);

    EXPECT_EQ(list.size(), 2);
    std::vector<int> expected = {2, 4};
    int idx = 0;
    for (const auto& val : list) {
        EXPECT_EQ(val, expected[idx++]);
    }

    while (!list.empty()) {
        list.erase(list.begin());
    }
    EXPECT_TRUE(list.empty());

    auto it = list.erase(list.end());
    EXPECT_EQ(it, list.end());
}

TEST_F(ListTest, EmplaceEraseInteraction) {
    auto it1 = list.emplace(list.begin(), 1);
    auto it2 = list.emplace(list.end(), 2);
    auto it3 = list.emplace(++list.begin(), 3);

    EXPECT_EQ(list.size(), 3);

    it2 = list.erase(it2);
    EXPECT_EQ(it2, list.end());

    auto new_it = list.emplace(it1, 4);
    EXPECT_EQ(*new_it, 4);

    std::vector<int> expected = {4, 1, 3};
    int idx = 0;
    for (const auto& val : list) {
        EXPECT_EQ(val, expected[idx++]);
    }
}

TEST_F(ListTest, MovedListOperations) {
    list.push_back(1);
    list.push_back(2);
    List<int> moved_list(std::move(list));

    EXPECT_NO_THROW(list.clear());
    EXPECT_NO_THROW(list.begin());
    EXPECT_NO_THROW(list.end());
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST_F(ListTest, LargeListOperations) {
    const size_t large_size = 100000;

    EXPECT_NO_THROW({
        for (size_t i = 0; i < large_size; ++i) {
            list.push_back(static_cast<int>(i));
        }
    });

    EXPECT_NO_THROW({
        while (!list.empty()) {
            list.pop_front();
        }
    });
}

TEST_F(ListTest, MultipleMoveCopyOperations) {
    for (int i = 0; i < 100; ++i) {
        list.push_back(i);
    }

    List<int> copy1(list);
    List<int> copy2(copy1);
    List<int> copy3(copy2);

    List<int> moved1(std::move(copy1));
    List<int> moved2(std::move(copy2));
    List<int> moved3(std::move(copy3));

    EXPECT_EQ(moved1.size(), 100);
    EXPECT_TRUE(copy1.empty());
}

TEST_F(ListTest, NonCopyableTypes) {
    struct NonCopyable {
        NonCopyable() = default;
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
        NonCopyable(NonCopyable&&) = default;
        NonCopyable& operator=(NonCopyable&&) = default;
    };

    List<NonCopyable> non_copyable_list;
    EXPECT_NO_THROW(non_copyable_list.emplace(non_copyable_list.begin()));
    EXPECT_NO_THROW(non_copyable_list.emplace(non_copyable_list.end()));
}
