#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <regex>

#include "utils/simple_resource.h"

namespace
{
namespace naive_details = utils::memory::details;

template <std::size_t ResourceAlignment, std::size_t BlockCount,
          std::size_t BlockSize>
class naive_resource : public ::testing::Test
{
   protected:
    using simple_resource =
        utils::memory::simple_resource<BlockCount, BlockSize>;
    using BIndex = typename simple_resource::BIndex;
    using void_ptr_t = void *;

    bool is_in_order(BIndex aIndex1, BIndex aIndex2) const noexcept
    {
        const auto &info1 = c_resource.at(aIndex1);
        const auto &info2 = c_resource.at(aIndex2);
        return (info1.next_index() == aIndex2) &&
               (info2.prev_index() == aIndex1);
    }

    ~naive_resource() override { resource.release(); }

    alignas(ResourceAlignment) std::byte data_[BlockSize * BlockCount]{};
    simple_resource resource{data_};
    const simple_resource &c_resource{resource};
    void_ptr_t allocated_pointers[simple_resource::kMaxBlockCount]{};
};

using NaiveMemoryResourceA4N32S8 = naive_resource<4, 32, 8>;
using NaiveMemoryResourceDeathTest = NaiveMemoryResourceA4N32S8;

template <typename Derived>
class FreeRegionBase : public NaiveMemoryResourceA4N32S8
{
   protected:
    FreeRegionBase()
    {
        if (Derived::kFreeCount == simple_resource::kMaxBlockCount)
        {
            return;
        }

        for (BIndex i = 0; i < resource.kMaxBlockCount; ++i)
        {
            allocated_pointers[i] =
                resource.allocate(sizeof(char), alignof(char));
        }

        for (BIndex i = Derived::kFirst; i <= Derived::kLast; ++i)
        {
            resource.deallocate(allocated_pointers[i]);
            allocated_pointers[i] = nullptr;
        }
    }
};

class FreeRegionAtTheBeginningOfTheResource
    : public FreeRegionBase<FreeRegionAtTheBeginningOfTheResource>
{
   protected:
    friend class FreeRegionBase<FreeRegionAtTheBeginningOfTheResource>;
    inline static constexpr BIndex kFirst = 0;
    inline static constexpr BIndex kLast = 14;
    inline static constexpr std::size_t kFreeCount =
                                            std::enable_if_t < (kFirst == 0) &&
                                            (kFirst <= kLast) &&
                                            (kLast <
                                             simple_resource::kMaxBlockCount -
                                                 1),
                                        std::integral_constant < std::size_t,
                                        kLast - kFirst + 1 >> ::value;
};

class FreeRegionInTheMiddleOfTheResource
    : public FreeRegionBase<FreeRegionInTheMiddleOfTheResource>
{
   protected:
    friend class FreeRegionBase<FreeRegionInTheMiddleOfTheResource>;
    inline static constexpr BIndex kFirst = 5;
    inline static constexpr BIndex kLast = 19;
    [[maybe_unused]] inline static constexpr std::size_t
        kFreeCount = std::enable_if_t < (kFirst > 0) && (kFirst <= kLast) &&
                     (kLast < simple_resource::kMaxBlockCount - 1),
        std::integral_constant < std::size_t, kLast - kFirst + 1 >> ::value;
};

class FreeRegionAtTheEndOfTheResource
    : public FreeRegionBase<FreeRegionAtTheEndOfTheResource>
{
   protected:
    friend class FreeRegionBase<FreeRegionAtTheEndOfTheResource>;
    inline static constexpr BIndex kFirst = 10;
    inline static constexpr BIndex kLast = simple_resource::kMaxBlockCount - 1;
    [[maybe_unused]] inline static constexpr std::size_t
        kFreeCount = std::enable_if_t < (kFirst > 0) && (kFirst <= kLast) &&
                     (kLast == simple_resource::kMaxBlockCount - 1),
        std::integral_constant < std::size_t, kLast - kFirst + 1 >> ::value;
};

class FreeRegionIsTheWholeOfResource
    : public FreeRegionBase<FreeRegionIsTheWholeOfResource>
{
   protected:
    friend class FreeRegionBase<FreeRegionIsTheWholeOfResource>;
    inline static constexpr BIndex kFirst = 0;
    inline static constexpr BIndex kLast = simple_resource::kMaxBlockCount - 1;
    [[maybe_unused]] inline static constexpr std::size_t
        kFreeCount = std::enable_if_t < (kFirst == 0) &&
                     (kLast == simple_resource::kMaxBlockCount - 1),
        std::integral_constant < std::size_t, kLast - kFirst + 1 >> ::value;
};

using utils::memory::details::origin_and_block_count;

template <std::size_t ResourceAlignment, std::size_t BlockSize,
          std::size_t MaxBlockCount>
class OriginAndBlockCount : public ::testing::Test
{
   protected:
    constexpr static inline auto kBlockSize = BlockSize;
    constexpr static inline auto kMaxBlockCount = MaxBlockCount;

    alignas(ResourceAlignment) std::byte buffer_[BlockSize * MaxBlockCount]{};
};

using InitBufAlignment4BSize4BCount32 = OriginAndBlockCount<4, 4, 32>;
}  // namespace

TEST_F(InitBufAlignment4BSize4BCount32, InitWithNullptrBufAnd10BytesOfSize)
{
    const auto [first_block_ptr, block_count] =
        origin_and_block_count<kBlockSize, kMaxBlockCount>(nullptr, 10);
    ASSERT_EQ(first_block_ptr, nullptr);
    ASSERT_EQ(block_count, 0);
}

TEST_F(InitBufAlignment4BSize4BCount32, InitWithNonNullptrBufAnd0BytesOfSize)
{
    const auto [first_block_ptr, block_count] =
        origin_and_block_count<kBlockSize, kMaxBlockCount>(buffer_, 0);
    ASSERT_EQ(first_block_ptr, nullptr);
    ASSERT_EQ(block_count, 0);
}

TEST_F(InitBufAlignment4BSize4BCount32, InitWithNullptrBufAnd0BytesOfSize)
{
    const auto [first_block_ptr, block_count] =
        origin_and_block_count<kBlockSize, kMaxBlockCount>(nullptr, 0);
    ASSERT_EQ(first_block_ptr, nullptr);
    ASSERT_EQ(block_count, 0);
}

TEST_F(NaiveMemoryResourceA4N32S8, DefaultConstructed)
{
    ASSERT_EQ(resource.free_count(), 1);
    ASSERT_EQ(resource.busy_count(), 0);
    for (simple_resource::BIndex i = 0; i < resource.kMaxBlockCount; ++i)
    {
        ASSERT_FALSE(resource.in_use(i));
        ASSERT_EQ(c_resource.at(i).prev_index(), 0);
        ASSERT_EQ(c_resource.at(i).next_index(), 0);
    }
    utils::memvcmp(resource.data(), 0, sizeof(resource.data()));
}

TEST_F(NaiveMemoryResourceA4N32S8, AllocateZeroBytes)
{
    ASSERT_EQ(resource.free_count(), 1);
    ASSERT_EQ(resource.busy_count(), 0);

    static_cast<void>(resource.allocate(0, 1));

    ASSERT_EQ(resource.free_count(), 1);
    ASSERT_EQ(resource.busy_count(), 0);
    for (simple_resource::BIndex i = 0; i < resource.kMaxBlockCount; ++i)
    {
        ASSERT_FALSE(resource.in_use(i));
        ASSERT_EQ(c_resource.at(i).prev_index(), 0);
        ASSERT_EQ(c_resource.at(i).next_index(), 0);
    }
    utils::memvcmp(resource.data(), 0, sizeof(resource.data()));
}

TEST_F(NaiveMemoryResourceA4N32S8, AllocateDeallocate1Char)
{
    void *ptr = resource.allocate(sizeof(char), alignof(char));
    ASSERT_TRUE(ptr);
    ASSERT_EQ(resource.free_count(), 1);
    ASSERT_EQ(resource.busy_count(), 1);

    ASSERT_TRUE(resource.in_use(0));
    ASSERT_EQ(c_resource.at(0).prev_index(), 0);
    ASSERT_EQ(c_resource.at(0).next_index(), 1);

    ASSERT_FALSE(resource.in_use(1));
    ASSERT_EQ(c_resource.at(1).prev_index(), 0);
    ASSERT_EQ(c_resource.at(1).next_index(), 0);

    resource.deallocate(ptr);
    ASSERT_EQ(resource.free_count(), 1);
    ASSERT_EQ(resource.busy_count(), 0);
    for (simple_resource::BIndex i = 0; i < resource.kMaxBlockCount; ++i)
    {
        ASSERT_FALSE(resource.in_use(i));
        ASSERT_EQ(c_resource.at(i).prev_index(), 0);
        ASSERT_EQ(c_resource.at(i).next_index(), 0);
    }
    utils::memvcmp(resource.data(), 0, sizeof(resource.data()));
}

TEST_F(NaiveMemoryResourceA4N32S8, AllocateDeallocate32CharsInReverseOrder)
{
    for (BIndex i = 0; i < resource.kMaxBlockCount; ++i)
    {
        ASSERT_FALSE(resource.in_use(i));

        ASSERT_EQ(c_resource.at(0).prev_index(), 0);
        ASSERT_EQ(
            c_resource.at(simple_resource::kMaxBlockCount - 1).next_index(), 0);

        void_ptr_t ptr = resource.allocate(sizeof(char), alignof(char));

        ASSERT_EQ(c_resource.at(0).prev_index(), 0);
        ASSERT_EQ(
            c_resource.at(simple_resource::kMaxBlockCount - 1).next_index(), 0);

        ASSERT_TRUE(ptr);
        allocated_pointers[i] = ptr;
        const std::size_t kFreeCount =
            (i < resource.kMaxBlockCount - 1) ? 1 : 0;
        ASSERT_EQ(resource.free_count(), kFreeCount);
        ASSERT_EQ(resource.busy_count(), i + 1);
        ASSERT_TRUE(resource.in_use(i));
        if (i == 0)
        {
            ASSERT_EQ(c_resource.at(i).prev_index(), 0);
            ASSERT_EQ(c_resource.at(i).next_index(), i + 1);
        }
        else if (i < resource.kMaxBlockCount - 1)
        {
            ASSERT_EQ(c_resource.at(i).prev_index(), i - 1);
            ASSERT_EQ(c_resource.at(i).next_index(), i + 1);
        }
        else
        {
            ASSERT_EQ(c_resource.at(i).prev_index(), i - 1);
            ASSERT_EQ(c_resource.at(i).next_index(), 0);
        }
    }

    for (BIndex i = 0; i < resource.kMaxBlockCount; ++i)
    {
        ASSERT_TRUE(resource.in_use(i));
    }

    for (BIndex j = 0; j < resource.kMaxBlockCount; ++j)
    {
        const BIndex i = resource.kMaxBlockCount - j - 1;
        const std::size_t kFreeCount =
            (i < resource.kMaxBlockCount - 1) ? 1 : 0;
        ASSERT_EQ(resource.free_count(), kFreeCount);
        ASSERT_EQ(resource.busy_count(), i + 1);
        ASSERT_TRUE(resource.in_use(i));

        ASSERT_EQ(c_resource.at(0).prev_index(), 0);
        ASSERT_EQ(
            c_resource.at(simple_resource::kMaxBlockCount - 1).next_index(), 0);

        resource.deallocate(allocated_pointers[i]);

        ASSERT_EQ(c_resource.at(0).prev_index(), 0);
        ASSERT_EQ(
            c_resource.at(simple_resource::kMaxBlockCount - 1).next_index(), 0);

        ASSERT_EQ(resource.free_count(), 1);
        ASSERT_EQ(resource.busy_count(), i);
        ASSERT_FALSE(resource.in_use(i));

        if (i == 0)
        {
            ASSERT_EQ(c_resource.at(i).prev_index(), 0);
            ASSERT_EQ(c_resource.at(i).next_index(), 0);
        }
        else
        {
            ASSERT_EQ(c_resource.at(i).prev_index(), i - 1);
            ASSERT_EQ(c_resource.at(i).next_index(), 0);
        }
    }
    utils::memvcmp(resource.data(), 0, sizeof(resource.data()));
}

TEST_F(NaiveMemoryResourceA4N32S8, AllocateDeallocate32CharsInStraightOrder)
{
    for (BIndex i = 0; i < resource.kMaxBlockCount; ++i)
    {
        ASSERT_FALSE(resource.in_use(i));
        void_ptr_t ptr = resource.allocate(sizeof(char), alignof(char));
        ASSERT_TRUE(ptr);
        allocated_pointers[i] = ptr;
        const std::size_t kFreeCount =
            (i < resource.kMaxBlockCount - 1) ? 1 : 0;
        ASSERT_EQ(resource.free_count(), kFreeCount);
        ASSERT_EQ(resource.busy_count(), i + 1);
        ASSERT_TRUE(resource.in_use(i));
        if (i == 0)
        {
            ASSERT_EQ(c_resource.at(i).prev_index(), 0);
            ASSERT_EQ(c_resource.at(i).next_index(), i + 1);
        }
        else if (i < resource.kMaxBlockCount - 1)
        {
            ASSERT_EQ(c_resource.at(i).prev_index(), i - 1);
            ASSERT_EQ(c_resource.at(i).next_index(), i + 1);
        }
        else
        {
            ASSERT_EQ(c_resource.at(i).prev_index(), i - 1);
            ASSERT_EQ(c_resource.at(i).next_index(), 0);
        }
    }

    for (BIndex i = 0; i < resource.kMaxBlockCount; ++i)
    {
        ASSERT_TRUE(resource.in_use(i));
    }

    for (BIndex i = 0; i < resource.kMaxBlockCount; ++i)
    {
        const std::size_t kFreeCount = i ? 1 : 0;
        ASSERT_EQ(resource.free_count(), kFreeCount);
        ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount - i);
        ASSERT_TRUE(resource.in_use(i));

        ASSERT_EQ(c_resource.at(i).prev_index(), 0);
        if (i < resource.kMaxBlockCount - 1)
        {
            ASSERT_EQ(c_resource.at(i).next_index(), i + 1);
        }
        else
        {
            ASSERT_EQ(c_resource.at(i).next_index(), 0);
        }

        ASSERT_EQ(c_resource.at(0).prev_index(), 0);
        ASSERT_EQ(
            c_resource.at(simple_resource::kMaxBlockCount - 1).next_index(), 0);

        resource.deallocate(allocated_pointers[i]);

        ASSERT_EQ(c_resource.at(0).prev_index(), 0);
        ASSERT_EQ(
            c_resource.at(simple_resource::kMaxBlockCount - 1).next_index(), 0);

        ASSERT_EQ(resource.free_count(), 1);
        ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount - i - 1);
        ASSERT_FALSE(resource.in_use(i));

        ASSERT_EQ(c_resource.at(i).prev_index(), 0);
        if (i < resource.kMaxBlockCount - 1)
        {
            ASSERT_EQ(c_resource.at(i).next_index(), i + 1);
        }
        else
        {
            ASSERT_EQ(c_resource.at(i).next_index(), 0);
        }
    }
    utils::memvcmp(resource.data(), 0, sizeof(resource.data()));
}

TEST_F(NaiveMemoryResourceA4N32S8, Deallocate8CharsInCustomOrder)
{
    for (BIndex i = 0; i < resource.kMaxBlockCount; ++i)
    {
        void_ptr_t ptr = resource.allocate(sizeof(char), alignof(char));
        allocated_pointers[i] = ptr;
    }

    ASSERT_EQ(resource.free_count(), 0);
    ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount);

    resource.deallocate(allocated_pointers[2]);

    ASSERT_TRUE(is_in_order(1, 2));
    ASSERT_TRUE(is_in_order(2, 3));

    ASSERT_EQ(resource.free_count(), 1);
    ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount - 1);

    resource.deallocate(allocated_pointers[4]);

    ASSERT_TRUE(is_in_order(3, 4));
    ASSERT_TRUE(is_in_order(4, 5));

    ASSERT_EQ(resource.free_count(), 2);
    ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount - 2);

    resource.deallocate(allocated_pointers[3]);

    ASSERT_TRUE(is_in_order(1, 2));
    ASSERT_TRUE(is_in_order(2, 5));

    ASSERT_EQ(resource.free_count(), 1);
    ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount - 3);

    resource.deallocate(allocated_pointers[5]);

    ASSERT_EQ(resource.free_count(), 1);
    ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount - 4);

    resource.deallocate(allocated_pointers[7]);

    ASSERT_EQ(resource.free_count(), 2);
    ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount - 5);

    resource.deallocate(allocated_pointers[10]);

    ASSERT_EQ(resource.free_count(), 3);
    ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount - 6);

    resource.deallocate(allocated_pointers[13]);

    ASSERT_EQ(resource.free_count(), 4);
    ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount - 7);

    resource.deallocate(allocated_pointers[16]);

    ASSERT_EQ(resource.free_count(), 5);
    ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount - 8);
}

TEST_F(NaiveMemoryResourceA4N32S8, DeallocateAndAllocateAgainOneChar)
{
    for (BIndex i = 0; i < resource.kMaxBlockCount; ++i)
    {
        void_ptr_t ptr = resource.allocate(sizeof(char), alignof(char));
        allocated_pointers[i] = ptr;
    }

    ASSERT_EQ(resource.free_count(), 0);
    ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount);

    resource.deallocate(allocated_pointers[2]);

    ASSERT_TRUE(is_in_order(1, 2));
    ASSERT_TRUE(is_in_order(2, 3));

    ASSERT_EQ(resource.free_count(), 1);
    ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount - 1);

    void_ptr_t ptr = resource.allocate(sizeof(char), alignof(char));
    ASSERT_TRUE(ptr);

    ASSERT_EQ(resource.free_count(), 0);
    ASSERT_EQ(resource.busy_count(), resource.kMaxBlockCount);
}

TEST_F(FreeRegionAtTheBeginningOfTheResource,
       AllocateAtTheBeginningOfFreeRegion)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();
    const BIndex kLastAllocated = kLast - 3;
    naive_details::allocate_begin_matched(resource, kFirst, kLastAllocated,
                                          kLast);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);
    ASSERT_TRUE(resource.in_use(kFirst));
    ASSERT_FALSE(resource.in_use(kLastAllocated + 1));
    ASSERT_TRUE(is_in_order(kFirst, kLastAllocated + 1));
    ASSERT_TRUE(is_in_order(kLastAllocated + 1, kLast + 1));
    ASSERT_EQ(resource.block_size(kFirst), kFreeCount - 3);
}

TEST_F(FreeRegionAtTheBeginningOfTheResource, AllocateInTheMiddleOfFreeRegion)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();
    const BIndex kFirstAllocated = kFirst + 2;
    const BIndex kLastAllocated = kLast - 3;
    naive_details::allocate_not_matched(resource, kFirst, kFirstAllocated,
                                        kLastAllocated, kLast);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount + 1);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);
    ASSERT_FALSE(resource.in_use(kFirst));
    ASSERT_TRUE(resource.in_use(kFirstAllocated));
    ASSERT_FALSE(resource.in_use(kLastAllocated + 1));
    ASSERT_TRUE(is_in_order(kFirst, kFirstAllocated));
    ASSERT_TRUE(is_in_order(kFirstAllocated, kLastAllocated + 1));
    ASSERT_TRUE(is_in_order(kLastAllocated + 1, kLast + 1));
    ASSERT_EQ(resource.block_size(kFirstAllocated), kFreeCount - 5);
}

TEST_F(FreeRegionAtTheBeginningOfTheResource, AllocateInTheEndOfFreeRegion)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();
    const BIndex kFirstAllocated = kFirst + 2;
    naive_details::allocate_end_matched(resource, kFirst, kFirstAllocated,
                                        kLast);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);

    ASSERT_FALSE(resource.in_use(kFirst));
    ASSERT_TRUE(resource.in_use(kFirstAllocated));

    ASSERT_TRUE(is_in_order(kFirst, kFirstAllocated));
    ASSERT_TRUE(is_in_order(kFirstAllocated, kLast + 1));
    ASSERT_EQ(resource.block_size(kFirstAllocated), kFreeCount - 2);
}

TEST_F(FreeRegionAtTheBeginningOfTheResource, AllocateFreeRegionEntirely)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();

    naive_details::allocate_matched(resource, kFirst);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount - 1);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);

    ASSERT_TRUE(resource.in_use(kFirst));

    ASSERT_TRUE(is_in_order(kFirst, kLast + 1));
    ASSERT_EQ(resource.block_size(kFirst), kFreeCount);
}

TEST_F(FreeRegionInTheMiddleOfTheResource, AllocateAtTheBeginningOfFreeRegion)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();
    const BIndex kLastAllocated = kLast - 3;
    naive_details::allocate_begin_matched(resource, kFirst, kLastAllocated,
                                          kLast);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);
    ASSERT_TRUE(resource.in_use(kFirst));
    ASSERT_FALSE(resource.in_use(kLastAllocated + 1));
    ASSERT_TRUE(is_in_order(kFirst, kLastAllocated + 1));
    ASSERT_TRUE(is_in_order(kLastAllocated + 1, kLast + 1));
    ASSERT_EQ(resource.block_size(kFirst), kFreeCount - 3);
}

TEST_F(FreeRegionInTheMiddleOfTheResource, AllocateInTheMiddleOfFreeRegion)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();
    const BIndex kFirstAllocated = kFirst + 2;
    const BIndex kLastAllocated = kLast - 3;
    naive_details::allocate_not_matched(resource, kFirst, kFirstAllocated,
                                        kLastAllocated, kLast);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount + 1);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);
    ASSERT_FALSE(resource.in_use(kFirst));
    ASSERT_TRUE(resource.in_use(kFirstAllocated));
    ASSERT_FALSE(resource.in_use(kLastAllocated + 1));
    ASSERT_TRUE(is_in_order(kFirst, kFirstAllocated));
    ASSERT_TRUE(is_in_order(kFirstAllocated, kLastAllocated + 1));
    ASSERT_TRUE(is_in_order(kLastAllocated + 1, kLast + 1));
    ASSERT_EQ(resource.block_size(kFirstAllocated), kFreeCount - 5);
}

TEST_F(FreeRegionInTheMiddleOfTheResource, AllocateInTheEndOfFreeRegion)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();
    const BIndex kFirstAllocated = kFirst + 2;
    naive_details::allocate_end_matched(resource, kFirst, kFirstAllocated,
                                        kLast);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);

    ASSERT_FALSE(resource.in_use(kFirst));
    ASSERT_TRUE(resource.in_use(kFirstAllocated));

    ASSERT_TRUE(is_in_order(kFirst, kFirstAllocated));
    ASSERT_TRUE(is_in_order(kFirstAllocated, kLast + 1));
    ASSERT_EQ(resource.block_size(kFirstAllocated), kFreeCount - 2);
}

TEST_F(FreeRegionInTheMiddleOfTheResource, AllocateFreeRegionEntirely)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();

    naive_details::allocate_matched(resource, kFirst);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount - 1);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);

    ASSERT_TRUE(resource.in_use(kFirst));

    ASSERT_TRUE(is_in_order(kFirst, kLast + 1));
    ASSERT_EQ(resource.block_size(kFirst), kFreeCount);
}

TEST_F(FreeRegionAtTheEndOfTheResource, AllocateAtTheBeginningOfFreeRegion)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();
    const BIndex kLastAllocated = kLast - 3;
    naive_details::allocate_begin_matched(resource, kFirst, kLastAllocated,
                                          kLast);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);
    ASSERT_TRUE(resource.in_use(kFirst));
    ASSERT_FALSE(resource.in_use(kLastAllocated + 1));
    ASSERT_TRUE(is_in_order(kFirst, kLastAllocated + 1));
    ASSERT_EQ(resource.block_size(kFirst), kFreeCount - 3);
}

TEST_F(FreeRegionAtTheEndOfTheResource, AllocateInTheMiddleOfFreeRegion)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();
    const BIndex kFirstAllocated = kFirst + 2;
    const BIndex kLastAllocated = kLast - 3;
    naive_details::allocate_not_matched(resource, kFirst, kFirstAllocated,
                                        kLastAllocated, kLast);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount + 1);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);
    ASSERT_FALSE(resource.in_use(kFirst));
    ASSERT_TRUE(resource.in_use(kFirstAllocated));
    ASSERT_FALSE(resource.in_use(kLastAllocated + 1));
    ASSERT_TRUE(is_in_order(kFirst, kFirstAllocated));
    ASSERT_TRUE(is_in_order(kFirstAllocated, kLastAllocated + 1));
    ASSERT_EQ(resource.block_size(kFirstAllocated), kFreeCount - 5);
}

TEST_F(FreeRegionAtTheEndOfTheResource, AllocateInTheEndOfFreeRegion)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();
    const std::size_t kDiff = 2;
    const BIndex kFirstAllocated = kFirst + kDiff;
    const std::size_t kAllocatedBlockCount = kFreeCount - kDiff;
    naive_details::allocate_end_matched(resource, kFirst, kFirstAllocated,
                                        kLast);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);

    ASSERT_FALSE(resource.in_use(kFirst));
    ASSERT_TRUE(resource.in_use(kFirstAllocated));

    ASSERT_TRUE(is_in_order(kFirst, kFirstAllocated));
    ASSERT_EQ(resource.block_size(kFirstAllocated), kAllocatedBlockCount);
}

TEST_F(FreeRegionAtTheEndOfTheResource, AllocateFreeRegionEntirely)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();

    naive_details::allocate_matched(resource, kFirst);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount - 1);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);

    ASSERT_TRUE(resource.in_use(kFirst));
    ASSERT_EQ(resource.block_size(kFirst), kFreeCount);
}

TEST_F(FreeRegionIsTheWholeOfResource, AllocateAtTheBeginningOfFreeRegion)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();
    const BIndex kLastAllocated = kLast - 3;
    naive_details::allocate_begin_matched(resource, kFirst, kLastAllocated,
                                          kLast);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);
    ASSERT_TRUE(resource.in_use(kFirst));
    ASSERT_FALSE(resource.in_use(kLastAllocated + 1));
    ASSERT_TRUE(is_in_order(kFirst, kLastAllocated + 1));
    ASSERT_EQ(resource.block_size(kFirst), kFreeCount - 3);
}

TEST_F(FreeRegionIsTheWholeOfResource, AllocateInTheMiddleOfFreeRegion)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();
    const BIndex kFirstAllocated = kFirst + 2;
    const BIndex kLastAllocated = kLast - 3;
    naive_details::allocate_not_matched(resource, kFirst, kFirstAllocated,
                                        kLastAllocated, kLast);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount + 1);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);
    ASSERT_FALSE(resource.in_use(kFirst));
    ASSERT_TRUE(resource.in_use(kFirstAllocated));
    ASSERT_FALSE(resource.in_use(kLastAllocated + 1));
    ASSERT_TRUE(is_in_order(kFirst, kFirstAllocated));
    ASSERT_TRUE(is_in_order(kFirstAllocated, kLastAllocated + 1));
    ASSERT_EQ(resource.block_size(kFirstAllocated), kFreeCount - 5);
}

TEST_F(FreeRegionIsTheWholeOfResource, AllocateInTheEndOfFreeRegion)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();
    const std::size_t kDiff = 2;
    const BIndex kFirstAllocated = kFirst + kDiff;
    const std::size_t kAllocatedBlockCount = kFreeCount - kDiff;
    naive_details::allocate_end_matched(resource, kFirst, kFirstAllocated,
                                        kLast);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);

    ASSERT_FALSE(resource.in_use(kFirst));
    ASSERT_TRUE(resource.in_use(kFirstAllocated));

    ASSERT_TRUE(is_in_order(kFirst, kFirstAllocated));
    ASSERT_EQ(resource.block_size(kFirstAllocated), kAllocatedBlockCount);
}

TEST_F(FreeRegionIsTheWholeOfResource, AllocateFreeRegionEntirely)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();

    naive_details::allocate_matched(resource, kFirst);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount - 1);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount + 1);

    ASSERT_TRUE(resource.in_use(kFirst));
    ASSERT_EQ(resource.block_size(kFirst), kFreeCount);
}

TEST_F(FreeRegionIsTheWholeOfResource, AllocateTooBigPieceOfMemory)
{
    const auto kOriginFreeCount = resource.free_count();
    const auto kOriginBusyCount = resource.busy_count();

    void *ptr = resource.allocate(resource.size() + 1, 1);
    ASSERT_NE(ptr, nullptr);
    resource.deallocate(ptr);

    ASSERT_EQ(resource.free_count(), kOriginFreeCount);
    ASSERT_EQ(resource.busy_count(), kOriginBusyCount);

    ASSERT_FALSE(resource.in_use(kFirst));
    ASSERT_EQ(resource.block_size(kFirst), kFreeCount);
}

MATCHER(DestructingWhileInUse, "")
{
    std::regex re(
        R"pattern(^\[ERROR\][\s\S]*\nreason: simple_resource is being destroyed while [\d]+ of its regions are in use\.$)pattern");
    std::smatch m;
    return std::regex_match(arg, m, re);
}

TEST_F(NaiveMemoryResourceDeathTest,
       RemainsNotDeallocatedMemoryWhileDestructing)
{
    ASSERT_DEATH({ {simple_resource p(data_); static_cast<void>(p.allocate(sizeof(char), alignof(char)));
}
}, DestructingWhileInUse());
}

MATCHER(PointerIsLessThanLeftBoundary, "")
{
    std::regex re(
        R"pattern(^\[ERROR\][\s\S]*\nreason: deallocating pointer\([a-zA-Z0-9]+\) is less than simple_resource's left boundary\([a-zA-Z0-9]+\)$)pattern");
    std::smatch m;
    return std::regex_match(arg, m, re);
}

TEST_F(NaiveMemoryResourceDeathTest, DeallocatePointerThatLessThanLeftBoundary)
{
    ASSERT_DEATH(
        {
            simple_resource p(data_, nullptr);
            p.deallocate(data_ - simple_resource::kBlockSize);
        },
        PointerIsLessThanLeftBoundary());
}

MATCHER(PointerIsMoreThanRightBoundary, "")
{
    std::regex re(
        R"pattern(^\[ERROR\][\s\S]*\nreason: deallocating pointer\([a-zA-Z0-9]+\) is more than simple_resource's right boundary\([a-zA-Z0-9]+\)$)pattern");
    std::smatch m;
    return std::regex_match(arg, m, re);
}

TEST_F(NaiveMemoryResourceDeathTest, DeallocatePointerThatMoreThanRightBoundary)
{
    ASSERT_DEATH(
        {
            simple_resource p(data_, nullptr);
            p.deallocate(data_ + p.size());
        },
        PointerIsMoreThanRightBoundary());
}

MATCHER(DeallocatingWrongAlignedPointer, "")
{
    std::regex re(
        R"pattern(^\[ERROR\][\s\S]*\nreason: pointer\([a-zA-Z0-9]+\) which is being deallocated must be aligned at least as one block size\([\d]+\)$)pattern");
    std::smatch m;
    return std::regex_match(arg, m, re);
}

TEST_F(NaiveMemoryResourceDeathTest,
       DeallocatePointerWithLessThanBlockSizeAlignment)
{
    ASSERT_DEATH(
        {
            simple_resource p(data_);
            p.deallocate(data_ + 1);
        },
        DeallocatingWrongAlignedPointer());
}

MATCHER(DeallocatingNotAllocatedPointer, "")
{
    std::regex re(
        R"pattern(^\[ERROR\][\s\S]*\nreason: trying to deallocate block\([\d]+\) that was not marked as allocated$)pattern");
    std::smatch m;
    return std::regex_match(arg, m, re);
}

TEST_F(NaiveMemoryResourceDeathTest, DeallocatePointerThatNotInUse)
{
    ASSERT_DEATH(
        {
            simple_resource p(data_, nullptr);
            p.deallocate(data_);
        },
        DeallocatingNotAllocatedPointer());
}
