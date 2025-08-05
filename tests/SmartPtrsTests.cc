#include <gtest/gtest.h>
#include "../SmartPtrs/SharedPtr.hpp"
#include "../SmartPtrs/WeakPtr.hpp"
#include "../SmartPtrs/ControlBlock.hpp"
#include "../SmartPtrs/UniquePtr.hpp"

struct DestructionTracker {
    static int instances_destroyed;
    DestructionTracker() = default;
    ~DestructionTracker() {
        instances_destroyed++;
    }
};
int DestructionTracker::instances_destroyed = 0;

class SmartPtrTest : public ::testing::Test {
protected:
    void SetUp() override {
        DestructionTracker::instances_destroyed = 0;
    }
};


TEST_F(SmartPtrTest, UniquePtr_DefaultConstructor) {
    UniquePtr<int> ptr;
    ASSERT_EQ(ptr.get(), nullptr);
    ASSERT_FALSE(ptr);
}

TEST_F(SmartPtrTest, UniquePtr_RawPointerConstructorAndDestruction) {
    {
        UniquePtr<DestructionTracker> ptr(new DestructionTracker());
        ASSERT_NE(ptr.get(), nullptr);
        ASSERT_TRUE(ptr);
    }
    ASSERT_EQ(DestructionTracker::instances_destroyed, 1);
}

TEST_F(SmartPtrTest, UniquePtr_Release) {
    DestructionTracker* raw_ptr = new DestructionTracker();
    UniquePtr<DestructionTracker> ptr(raw_ptr);
    
    
    auto released_ptr = ptr.release(); 
    ASSERT_EQ(released_ptr, raw_ptr);
    ASSERT_EQ(ptr.get(), nullptr);
    ASSERT_EQ(DestructionTracker::instances_destroyed, 0);

    delete released_ptr;
    ASSERT_EQ(DestructionTracker::instances_destroyed, 1);
}

TEST_F(SmartPtrTest, UniquePtr_Reset) {
    UniquePtr<DestructionTracker> ptr(new DestructionTracker());
    ASSERT_EQ(DestructionTracker::instances_destroyed, 0);

    ptr.reset(new DestructionTracker());
    ASSERT_EQ(DestructionTracker::instances_destroyed, 1);

    ptr.reset(); // reset(nullptr)
    ASSERT_EQ(DestructionTracker::instances_destroyed, 2);
    ASSERT_EQ(ptr.get(), nullptr);
}

TEST_F(SmartPtrTest, UniquePtr_MoveConstructor) {
    UniquePtr<DestructionTracker> ptr1(new DestructionTracker());
    DestructionTracker* raw_ptr = ptr1.get();

    UniquePtr<DestructionTracker> ptr2(std::move(ptr1));

    ASSERT_EQ(ptr1.get(), nullptr);
    ASSERT_EQ(ptr2.get(), raw_ptr);
    ASSERT_EQ(DestructionTracker::instances_destroyed, 0);
}

TEST_F(SmartPtrTest, UniquePtr_MoveAssignment) {
    UniquePtr<DestructionTracker> ptr1(new DestructionTracker());
    DestructionTracker* raw_ptr1 = ptr1.get();
    UniquePtr<DestructionTracker> ptr2(new DestructionTracker());
    
    ASSERT_EQ(DestructionTracker::instances_destroyed, 0);
    ptr2 = std::move(ptr1);

    ASSERT_EQ(DestructionTracker::instances_destroyed, 1);
    ASSERT_EQ(ptr1.get(), nullptr);
    ASSERT_EQ(ptr2.get(), raw_ptr1);
}

TEST_F(SmartPtrTest, UniquePtr_MakeUnique) {
    auto ptr = make_unique<DestructionTracker>();
    ASSERT_NE(ptr.get(), nullptr);
}



TEST_F(SmartPtrTest, SharedPtr_DefaultConstructor) {
    SharedPtr<int> sp;
    EXPECT_EQ(sp.get(), nullptr);
    EXPECT_EQ(sp.use_count(), 0);
    EXPECT_FALSE(sp);
}

TEST_F(SmartPtrTest, SharedPtr_RawPointerConstructor) {
    SharedPtr<DestructionTracker> sp(new DestructionTracker());
    EXPECT_NE(sp.get(), nullptr);
    EXPECT_EQ(sp.use_count(), 1);
    EXPECT_TRUE(sp.unique());
}

TEST_F(SmartPtrTest, SharedPtr_CopyConstructor) {
    SharedPtr<DestructionTracker> sp1(new DestructionTracker());
    EXPECT_EQ(sp1.use_count(), 1);

    SharedPtr<DestructionTracker> sp2(sp1);
    EXPECT_EQ(sp1.get(), sp2.get());
    EXPECT_EQ(sp1.use_count(), 2);
    EXPECT_EQ(sp2.use_count(), 2);
    EXPECT_FALSE(sp1.unique());
    EXPECT_FALSE(sp2.unique());
}

TEST_F(SmartPtrTest, SharedPtr_CopyAssignment) {
    SharedPtr<DestructionTracker> sp1(new DestructionTracker());
    SharedPtr<DestructionTracker> sp2(new DestructionTracker());
    
    ASSERT_EQ(DestructionTracker::instances_destroyed, 0);

    sp1 = sp2;
    EXPECT_EQ(DestructionTracker::instances_destroyed, 1);
    EXPECT_EQ(sp1.get(), sp2.get());
    EXPECT_EQ(sp1.use_count(), 2);
    EXPECT_EQ(sp2.use_count(), 2);
}

TEST_F(SmartPtrTest, SharedPtr_MoveConstructor) {
    auto* raw_ptr = new DestructionTracker();
    SharedPtr<DestructionTracker> sp1(raw_ptr);

    SharedPtr<DestructionTracker> sp2(std::move(sp1));
    EXPECT_EQ(sp1.get(), nullptr);
    EXPECT_EQ(sp1.use_count(), 0);
    EXPECT_EQ(sp2.get(), raw_ptr);
    EXPECT_EQ(sp2.use_count(), 1);
    EXPECT_EQ(DestructionTracker::instances_destroyed, 0);
}

TEST_F(SmartPtrTest, SharedPtr_MoveAssignment) {
    SharedPtr<DestructionTracker> sp1(new DestructionTracker());
    SharedPtr<DestructionTracker> sp2(new DestructionTracker());
    auto* raw_ptr2 = sp2.get();

    sp1 = std::move(sp2);

    EXPECT_EQ(DestructionTracker::instances_destroyed, 1);
    EXPECT_EQ(sp2.get(), nullptr);
    EXPECT_EQ(sp2.use_count(), 0);
    EXPECT_EQ(sp1.get(), raw_ptr2);
    EXPECT_EQ(sp1.use_count(), 1);
}

TEST_F(SmartPtrTest, SharedPtr_Destruction) {
    SharedPtr<DestructionTracker> sp1(new DestructionTracker());
    EXPECT_EQ(sp1.use_count(), 1);
    
    {
        SharedPtr<DestructionTracker> sp2 = sp1;
        SharedPtr<DestructionTracker> sp3 = sp1;
        EXPECT_EQ(sp1.use_count(), 3);
        EXPECT_EQ(DestructionTracker::instances_destroyed, 0);
    } 

    EXPECT_EQ(sp1.use_count(), 1);
    EXPECT_EQ(DestructionTracker::instances_destroyed, 0);

    sp1.reset();
    EXPECT_EQ(sp1.get(), nullptr);
    EXPECT_EQ(sp1.use_count(), 0);
    EXPECT_EQ(DestructionTracker::instances_destroyed, 1);
}

TEST_F(SmartPtrTest, SharedPtr_MakeShared) {
    SharedPtr<DestructionTracker> sp;
    {
        sp = make_shared<DestructionTracker>();
        EXPECT_NE(sp.get(), nullptr);
        EXPECT_EQ(sp.use_count(), 1);
        EXPECT_EQ(DestructionTracker::instances_destroyed, 0);
    }
    EXPECT_EQ(DestructionTracker::instances_destroyed, 0);
    
    sp.reset();
    EXPECT_EQ(DestructionTracker::instances_destroyed, 1);
}


TEST_F(SmartPtrTest, WeakPtr_DefaultConstructor) {
    WeakPtr<int> wp;
    EXPECT_TRUE(wp.expired());
    EXPECT_EQ(wp.use_count(), 0);
}

TEST_F(SmartPtrTest, WeakPtr_ConstructFromSharedPtr) {
    SharedPtr<DestructionTracker> sp(new DestructionTracker());
    WeakPtr<DestructionTracker> wp(sp);

    EXPECT_FALSE(wp.expired());
    EXPECT_EQ(wp.use_count(), 1);
    
    EXPECT_EQ(sp.use_count(), 1);
}

TEST_F(SmartPtrTest, WeakPtr_LockSuccess) {
    SharedPtr<DestructionTracker> sp1(new DestructionTracker());
    WeakPtr<DestructionTracker> wp(sp1);

    EXPECT_EQ(sp1.use_count(), 1);

    SharedPtr<DestructionTracker> sp2 = wp.lock();
    ASSERT_NE(sp2.get(), nullptr);
    EXPECT_EQ(sp1.get(), sp2.get());
    EXPECT_EQ(sp1.use_count(), 2);
    EXPECT_EQ(sp2.use_count(), 2);
    EXPECT_EQ(DestructionTracker::instances_destroyed, 0);
}

TEST_F(SmartPtrTest, WeakPtr_LockFailureAfterExpired) {
    WeakPtr<DestructionTracker> wp;
    {
        SharedPtr<DestructionTracker> sp(new DestructionTracker());
        wp = sp; 
        EXPECT_FALSE(wp.expired());
    } 

    EXPECT_EQ(DestructionTracker::instances_destroyed, 1);
    EXPECT_TRUE(wp.expired());

    SharedPtr<DestructionTracker> sp_locked = wp.lock();
    EXPECT_EQ(sp_locked.get(), nullptr);
    EXPECT_EQ(sp_locked.use_count(), 0);
}


struct NodeA;
struct NodeB;

struct NodeA {
    ~NodeA() { DestructionTracker::instances_destroyed++; }
    SharedPtr<NodeB> ptr_to_b;
};

struct NodeB {
    ~NodeB() { DestructionTracker::instances_destroyed++; }
    WeakPtr<NodeA> ptr_to_a; 
};

TEST_F(SmartPtrTest, HandlesCyclicDependency) {
    SharedPtr<NodeA> a = make_shared<NodeA>();
    SharedPtr<NodeB> b = make_shared<NodeB>();

    a->ptr_to_b = b;
    b->ptr_to_a = a;

    EXPECT_EQ(a.use_count(), 1);
    EXPECT_EQ(b.use_count(), 2);
    
    a.reset();
    EXPECT_EQ(DestructionTracker::instances_destroyed, 1);
    EXPECT_EQ(b.use_count(), 1); 
    
    b.reset();
    EXPECT_EQ(DestructionTracker::instances_destroyed, 2);
}
