#include <gtest/gtest.h>
#include <stdexcept>
#include "../include/SharedPtr.hpp"


TEST(SharedPtrTest, DefaultConstructor) {
    SharedPtr<int> ptr_;
    EXPECT_EQ(ptr_.get(), nullptr);
    EXPECT_EQ(ptr_.use_count(), 0);
}

TEST(SharedPtrTest, PtrConstructor) {
    int* raw_ptr_ = new int(1999);
    SharedPtr<int> ptr_(raw_ptr_);

    EXPECT_EQ(ptr_.get(), raw_ptr_);
    EXPECT_EQ(ptr_.use_count(), 1);
    EXPECT_EQ(*ptr_.get(), 1999);
}


TEST(SharedPtrTest, CopyConstructor) {
    double* raw_ptr_ = new double(3.14);
    
    SharedPtr<double> ptr_1_(raw_ptr_);
    SharedPtr<double> ptr_2_(ptr_1_);

    EXPECT_EQ(ptr_1_.get(), raw_ptr_);
    EXPECT_EQ(ptr_2_.get(), raw_ptr_);
    EXPECT_EQ(ptr_1_.use_count(), 2);
    EXPECT_EQ(ptr_2_.use_count(), 2);
}


TEST(SharedPtrTest, MoveConstructor) {
    char* raw_ptr_ = new char('R');

    SharedPtr<char> ptr_1_(raw_ptr_);
    SharedPtr<char> ptr_2_(std::move(ptr_1_));

    EXPECT_EQ(ptr_1_.get(), nullptr);
    EXPECT_EQ(ptr_2_.get(), raw_ptr_);
    EXPECT_EQ(ptr_1_.use_count(), 0);
    EXPECT_EQ(ptr_2_.use_count(), 1);
}


TEST(SharedPtrTest, CopyAssignment) {
    int* raw_ptr_1 = new int(42);
    int* raw_ptr_2_ = new int(24);
    
    SharedPtr<int> ptr_1_(raw_ptr_1);
    SharedPtr<int> ptr_2_(raw_ptr_2_);
    ptr_2_ = ptr_1_;
    
    EXPECT_EQ(ptr_1_.get(), raw_ptr_1);
    EXPECT_EQ(ptr_2_.get(), raw_ptr_1);
    EXPECT_EQ(ptr_1_.use_count(), 2);
    EXPECT_EQ(ptr_2_.use_count(), 2);
}


TEST(SharedPtrTest, MoveAssignment) {
    int* raw_ptr_1_ = new int(42);
    int* raw_ptr_2_ = new int(24);
    
    SharedPtr<int> ptr_1_(raw_ptr_1_);
    SharedPtr<int> ptr_2_(raw_ptr_2_);
    ptr_2_ = std::move(ptr_1_);
    
    EXPECT_EQ(ptr_1_.get(), nullptr);
    EXPECT_EQ(ptr_2_.get(), raw_ptr_1_);
    EXPECT_EQ(ptr_1_.use_count(), 0);
    EXPECT_EQ(ptr_2_.use_count(), 1);
}


TEST(SharedPtrTest, DereferenceOperator) {
    int* raw_ptr_ = new int(42);
    SharedPtr<int> ptr_(raw_ptr_);
    EXPECT_EQ(*ptr_, 42);

    SharedPtr<int> null_;
    EXPECT_THROW(*null_, std::runtime_error);
}


TEST(SharedPtrTest, UniqueMethod) {
    int* raw_ptr_ = new int(42);
    SharedPtr<int> ptr_1_(raw_ptr_);
    
    EXPECT_TRUE(ptr_1_.unique());
    
    SharedPtr<int> ptr_2_(ptr_1_);
    
    EXPECT_FALSE(ptr_1_.unique());
    EXPECT_FALSE(ptr_2_.unique());
}


TEST(SharedPtrTest, ResetMethod) {
    int* raw_ptr_1_ = new int(42);
    int* raw_ptr_2_ = new int(24);
    
    SharedPtr<int> ptr(raw_ptr_1_);
    ptr.reset(raw_ptr_2_);
    
    EXPECT_EQ(ptr.get(), raw_ptr_2_);
    EXPECT_EQ(ptr.use_count(), 1);
}



TEST(SharedPtrTest, Destructor) {
    int* raw_ptr_ = new int(42);
    
    {
        SharedPtr<int> ptr(raw_ptr_);
        EXPECT_EQ(ptr.get(), raw_ptr_);
        EXPECT_EQ(ptr.use_count(), 1);
    }

    EXPECT_NE(raw_ptr_, nullptr); 
}


TEST(SharedPtrTest, ReferenceCounting) {
    int* raw_ptr_ = new int(42);
    SharedPtr<int> ptr_1_(raw_ptr_);
    
    EXPECT_EQ(ptr_1_.use_count(), 1);
    
    {
        SharedPtr<int> ptr_2_(ptr_1_);
        EXPECT_EQ(ptr_1_.use_count(), 2);
        EXPECT_EQ(ptr_2_.use_count(), 2);
    }

    EXPECT_EQ(ptr_1_.use_count(), 1);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
