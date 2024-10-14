#include <gtest/gtest.h>
#include "../include/UniquePtr.hpp"

class TestClass {
public:
    static int counter;
    TestClass() { 
        counter++; 
    }

    ~TestClass() { 
        counter--;
    }
};


int TestClass::counter = 0;


TEST(UniquePtrTest, DefaultConstructor) {
    UniquePtr<int> ptr;
    EXPECT_EQ(ptr.get(), nullptr);
}


TEST(UniquePtrTest, PointerConstructor) {
    int* raw_ptr = new int(42);
    UniquePtr<int> ptr(raw_ptr);
    EXPECT_EQ(ptr.get(), raw_ptr);
    EXPECT_EQ(*ptr, 42);
}


TEST(UniquePtrTest, Destructor) {
    {
        UniquePtr<TestClass> ptr(new TestClass());
        EXPECT_EQ(TestClass::counter, 1);
    }
    EXPECT_EQ(TestClass::counter, 0);
}


TEST(UniquePtrTest, MoveConstructor) {
    UniquePtr<int> ptr1(new int(42));
    UniquePtr<int> ptr2(std::move(ptr1));
    EXPECT_EQ(ptr1.get(), nullptr);
    EXPECT_EQ(*ptr2, 42);
}


TEST(UniquePtrTest, MoveAssignmentOperator) {
    UniquePtr<int> ptr1(new int(42));
    UniquePtr<int> ptr2;
    ptr2 = std::move(ptr1);
    EXPECT_EQ(ptr1.get(), nullptr);
    EXPECT_EQ(*ptr2, 42);
}


TEST(UniquePtrTest, DereferenceOperator) {
    UniquePtr<int> ptr(new int(42));
    EXPECT_EQ(*ptr, 42);
}


TEST(UniquePtrTest, ArrowOperator) {
    struct TestStruct { int value = 42; };
    UniquePtr<TestStruct> ptr(new TestStruct());
    EXPECT_EQ(ptr->value, 42);
}


TEST(UniquePtrTest, GetMethod) {
    int* raw_ptr = new int(42);
    UniquePtr<int> ptr(raw_ptr);
    EXPECT_EQ(ptr.get(), raw_ptr);
}


TEST(UniquePtrTest, ReleaseMethod) {
    int* raw_ptr = new int(42);
    UniquePtr<int> ptr(raw_ptr);
    int* released_ptr = ptr.release();
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_EQ(released_ptr, raw_ptr);
    delete released_ptr; 
}


TEST(UniquePtrTest, ResetMethod) {
    UniquePtr<int> ptr(new int(42));
    ptr.reset(new int(10));
    EXPECT_EQ(*ptr, 10);
}


TEST(UniquePtrTest, ComparisonOperators) {
    UniquePtr<int> ptr1(new int(42));
    UniquePtr<int> ptr2(new int(42));
    UniquePtr<int> ptr3;

    EXPECT_NE(ptr1, ptr2);
    EXPECT_EQ(ptr1, ptr1);
    EXPECT_NE(ptr1, ptr3);
    EXPECT_EQ(ptr3, nullptr);
    EXPECT_NE(ptr1, nullptr);
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
