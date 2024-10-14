#include <gtest/gtest.h>
#include "../include/WeakPtr.hpp"
#include "../include/SharedPtr.hpp"


TEST(WeakPtrTest, DefaultConstructor) {
    WeakPtr<int> weakPtr;
    EXPECT_EQ(weakPtr.use_count(), 0);
    EXPECT_TRUE(weakPtr.expired());
}

TEST(WeakPtrTest, ConstructorFromSharedPtr) {
    SharedPtr<int> sharedPtr(new int(10));
    WeakPtr<int> weakPtr(sharedPtr);
    EXPECT_EQ(weakPtr.use_count(), 1);
    EXPECT_FALSE(weakPtr.expired());
}

TEST(WeakPtrTest, CopyConstructor) {
    SharedPtr<int> sharedPtr(new int(10));
    WeakPtr<int> weakPtr1(sharedPtr);
    WeakPtr<int> weakPtr2(weakPtr1);
    EXPECT_EQ(weakPtr2.use_count(), 1);
    EXPECT_FALSE(weakPtr2.expired());
}

TEST(WeakPtrTest, MoveConstructor) {
    SharedPtr<int> sharedPtr(new int(10));
    WeakPtr<int> weakPtr1(sharedPtr);
    WeakPtr<int> weakPtr2(std::move(weakPtr1));
    EXPECT_EQ(weakPtr2.use_count(), 1);
    EXPECT_FALSE(weakPtr2.expired());
    EXPECT_EQ(weakPtr1.use_count(), 0);
    EXPECT_TRUE(weakPtr1.expired());
}

TEST(WeakPtrTest, CopyAssignment) {
    SharedPtr<int> sharedPtr(new int(10));
    WeakPtr<int> weakPtr1(sharedPtr);
    WeakPtr<int> weakPtr2;
    weakPtr2 = weakPtr1;
    EXPECT_EQ(weakPtr2.use_count(), 1);
    EXPECT_FALSE(weakPtr2.expired());
}

TEST(WeakPtrTest, MoveAssignment) {
    SharedPtr<int> sharedPtr(new int(10));
    WeakPtr<int> weakPtr1(sharedPtr);
    WeakPtr<int> weakPtr2;
    weakPtr2 = std::move(weakPtr1);
    EXPECT_EQ(weakPtr2.use_count(), 1);
    EXPECT_FALSE(weakPtr2.expired());
    EXPECT_EQ(weakPtr1.use_count(), 0);
    EXPECT_TRUE(weakPtr1.expired());
}

TEST(WeakPtrTest, AssignmentFromSharedPtr) {
    SharedPtr<int> sharedPtr(new int(10));
    WeakPtr<int> weakPtr;
    weakPtr = sharedPtr;
    EXPECT_EQ(weakPtr.use_count(), 1);
    EXPECT_FALSE(weakPtr.expired());
}

TEST(WeakPtrTest, Lock) {
    SharedPtr<int> sharedPtr(new int(10));
    WeakPtr<int> weakPtr(sharedPtr);
    SharedPtr<int> lockedPtr = weakPtr.lock();
    EXPECT_EQ(lockedPtr.use_count(), 2);
    EXPECT_EQ(*lockedPtr, 10);
}

TEST(WeakPtrTest, UseCount) {
    SharedPtr<int> sharedPtr(new int(10));
    WeakPtr<int> weakPtr(sharedPtr);
    EXPECT_EQ(weakPtr.use_count(), 1);
}

TEST(WeakPtrTest, Expired) {
    SharedPtr<int> sharedPtr(new int(10));
    WeakPtr<int> weakPtr(sharedPtr);
    sharedPtr.reset();
    EXPECT_FALSE(weakPtr.expired());
}

TEST(WeakPtrTest, Reset) {
    SharedPtr<int> sharedPtr(new int(10));
    WeakPtr<int> weakPtr(sharedPtr);
    weakPtr.reset();
    EXPECT_EQ(weakPtr.use_count(), 0);
    EXPECT_TRUE(weakPtr.expired());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
