#ifndef utils_simple_resource_h
#define utils_simple_resource_h

#include <algorithm>
#include <bitset>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <limits>
#include <tuple>

#include "../ctz.h"
#include "memory.h"

namespace utils
{
namespace memory
{
namespace details
{
template <typename Resource>
void allocate_not_matched(Resource &aResource,
                          typename Resource::BIndex aFreeFirst,
                          typename Resource::BIndex aFirst,
                          typename Resource::BIndex aLast,
                          [[maybe_unused]]
                          typename Resource::BIndex aFreeLast) noexcept
{
    assert(aFreeFirst < aFirst);
    assert(aFirst <= aLast);
    assert(aLast < aFreeLast);

    aResource.set_in_use(aFirst);

    using BIndex = typename Resource::BIndex;
    const BIndex next = aLast + 1;
    auto &free_info = aResource.at(aFreeFirst);

    aResource.set_prev_next(aFirst, aFreeFirst, next);

    aResource.set_prev_next(next, aFirst, free_info.next_index());

    aResource.set_prev(free_info.next_index(), next);
    aResource.set_next(aFreeFirst, aFirst);

    ++aResource.free_count_;
    ++aResource.busy_count_;
}

template <typename Resource>
void allocate_begin_matched(Resource &aResource,
                            typename Resource::BIndex aFreeFirst,
                            typename Resource::BIndex aLast,
                            [[maybe_unused]]
                            typename Resource::BIndex aFreeLast) noexcept
{
    assert(aFreeFirst <= aLast);
    assert(aLast < aFreeLast);

    aResource.set_in_use(aFreeFirst);

    using BIndex = typename Resource::BIndex;
    const BIndex next = aLast + 1;
    auto &free_info = aResource.at(aFreeFirst);

    aResource.set_prev_next(next, aFreeFirst, free_info.next_index());

    aResource.set_prev(free_info.next_index(), next);
    aResource.set_next(aFreeFirst, next);

    ++aResource.busy_count_;
}

template <typename Resource>
void allocate_end_matched(Resource &aResource,
                          typename Resource::BIndex aFreeFirst,
                          typename Resource::BIndex aFirst,
                          typename Resource::BIndex aFreeLast) noexcept
{
    assert(aFreeFirst < aFirst);
    assert(aFirst <= aFreeLast);

    aResource.set_in_use(aFirst);

    using BIndex = typename Resource::BIndex;
    const BIndex next =
        (aFreeLast < aResource.block_count_ - 1) ? aFreeLast + 1 : 0;
    auto &free_info = aResource.at(aFreeFirst);

    aResource.set_prev_next(aFirst, aFreeFirst, next);

    aResource.set_prev(free_info.next_index(), aFirst);
    aResource.set_next(aFreeFirst, aFirst);

    ++aResource.busy_count_;
}

template <typename Resource>
void allocate_matched(Resource &aResource,
                      typename Resource::BIndex aFreeFirst) noexcept
{
    assert(aFreeFirst < aResource.block_count_);
    aResource.set_in_use(aFreeFirst);
    --aResource.free_count_;
    ++aResource.busy_count_;
}

template <std::size_t BlockSize, std::size_t MaxBlockCount>
std::tuple<std::byte *, std::size_t> origin_and_block_count(
    std::byte *const aBuffer, const std::size_t aSize) noexcept
{
    static_assert(is_power_of_2(BlockSize));
    static_assert(MaxBlockCount);

    std::byte *first_block_ptr = nullptr;
    std::size_t block_count = 0;

    if (aBuffer && aSize)
    {
        const auto kSkip = skip_to_align(aBuffer, BlockSize);
        if (kSkip < aSize)
        {
            const auto count = (aSize - kSkip) / BlockSize;
            if (count > 0)
            {
                block_count = count;
                first_block_ptr = aBuffer + kSkip;
            }
        }
    }

    return {first_block_ptr, block_count};
}

template <std::size_t BlockSize, std::size_t MaxBlockCount, std::size_t BufSize>
std::tuple<std::byte *, std::size_t> origin_and_block_count(
    std::byte (&aBuffer)[BufSize]) noexcept
{
    return origin_and_block_count<BlockSize, MaxBlockCount>(aBuffer, BufSize);
}
}  // namespace details

template <std::size_t MaxBlockCount, std::size_t BlockSize>
class simple_resource : public memory_resource
{
   public:
    friend class memory_resource;

    using BIndex = uint_from_nbits_t<bits_count(MaxBlockCount - 1)>;
    struct block_info
    {
        friend class simple_resource;

       public:
        inline BIndex prev_index() const noexcept { return prev_index_; }

        inline BIndex next_index() const noexcept { return next_index_; }

       private:
        BIndex prev_index_{};
        BIndex next_index_{};
    };

    static constexpr std::size_t kMaxBlockCount = MaxBlockCount;
    static constexpr std::size_t kBlockSize = BlockSize;
    static constexpr std::size_t kMaxResourceSize = BlockSize * MaxBlockCount;

    ~simple_resource() override
    {
        UTILS_ABORT_IF(busy_count_,
                       "\nreason: simple_resource is being destroyed while %d "
                       "of its regions "
                       "are in use.",
                       busy_count_);
    }

    simple_resource(const simple_resource &) = delete;
    simple_resource() noexcept = delete;
    simple_resource(std::byte *const aBuffer, const std::size_t aSize,
                    memory_resource *aUpstream) noexcept
        : simple_resource(
              details::origin_and_block_count<BlockSize, MaxBlockCount>(aBuffer,
                                                                        aSize),
              aUpstream)
    {
    }
    simple_resource(std::byte *const aBuffer, const std::size_t aSize) noexcept
        : simple_resource(
              details::origin_and_block_count<BlockSize, MaxBlockCount>(aBuffer,
                                                                        aSize),
              get_default_resource())
    {
    }

    template <std::size_t BufSize>
    explicit simple_resource(std::byte (&aBuffer)[BufSize]) noexcept
        : simple_resource(aBuffer, BufSize, get_default_resource())
    {
    }

    template <std::size_t BufSize>
    simple_resource(std::byte (&aBuffer)[BufSize],
                    memory_resource *aUpstream) noexcept
        : simple_resource(aBuffer, BufSize, aUpstream)
    {
    }

    bool is_equal(const memory_resource &aOther) const noexcept override
    {
        return &aOther == this;
    }

    void release() noexcept
    {
        apply([this](BIndex aIndex) noexcept -> bool { return in_use(aIndex); },
              [this](BIndex aIndex) mutable noexcept -> bool
              { return at(deallocate_blocks(aIndex)).next_index(); });
    }

    const block_info &at(BIndex aIndex) const noexcept
    {
        assert(aIndex < block_count_);
        return infos_[aIndex];
    }

    std::byte const *data() const noexcept { return data_; }

    const block_info (&infos() const noexcept)[MaxBlockCount] { return infos_; }

    constexpr bool in_use(BIndex aIndex) const noexcept
    {
        assert(aIndex < block_count_);
        return in_use_[aIndex];
    }

    constexpr std::size_t free_count() const noexcept { return free_count_; }

    constexpr std::size_t busy_count() const noexcept { return busy_count_; }

    constexpr std::size_t block_size(const BIndex aIndex) const noexcept
    {
        std::size_t next_index = at(aIndex).next_index();
        if (!next_index)
        {
            next_index = block_count_;
        }
        return next_index - aIndex;
    }

    constexpr std::size_t block_size_in_bytes(
        const BIndex aIndex) const noexcept
    {
        return block_size(aIndex) * BlockSize;
    }

    std::size_t size() const noexcept { return BlockSize * block_count_; }

   private:
    friend void details::allocate_not_matched<simple_resource>(
        simple_resource &aResource, BIndex aFreeFirst, BIndex aFirst,
        BIndex aLast, BIndex aFreeLast) noexcept;
    friend void details::allocate_begin_matched<simple_resource>(
        simple_resource &aResource, BIndex aFreeFirst, BIndex aLast,
        BIndex aFreeLast) noexcept;
    friend void details::allocate_end_matched<simple_resource>(
        simple_resource &aResource, BIndex aFreeFirst, BIndex aFirst,
        BIndex aFreeLast) noexcept;
    friend void details::allocate_matched<simple_resource>(
        simple_resource &aResource, BIndex aFreeFirst) noexcept;

    simple_resource(std::tuple<std::byte *, std::size_t> aOriginAndBlockCount,
                    memory_resource *aUpstream) noexcept
        : memory_resource(&simple_resource::allocate<simple_resource>,
                          &simple_resource::deallocate<simple_resource>)
        , data_(std::get<0>(aOriginAndBlockCount))
        , block_count_(
              std::min(std::get<1>(aOriginAndBlockCount), MaxBlockCount))
        , upstream_(aUpstream)
    {
    }

    [[nodiscard]] void *allocate_impl(std::size_t aSize,
                                      std::size_t aAlignment) noexcept
    {
        if (!aSize)
        {
            return nullptr;
        }

        assert(is_power_of_2(aAlignment));
        static_assert(is_power_of_2(BlockSize));

        std::byte *pointer = nullptr;
        if (data_)
        {
            const std::size_t kBufferAlignment =
                1 << ctz(reinterpret_cast<uintptr_t>(data_));
            if (aAlignment <= max_alignment_inside_block(
                                  kBufferAlignment, block_count_ * BlockSize))
            {
                apply([this](BIndex aIndex) noexcept -> bool
                      { return not in_use(aIndex); },
                      [this, &pointer, aAlignment,
                       aSize](BIndex aIndex) mutable noexcept -> bool
                      {
                          const auto block_size = block_size_in_bytes(aIndex);
                          std::byte *const block_pointer =
                              block_index_to_pointer(aIndex);
                          const auto skip =
                              skip_to_align(block_pointer, aAlignment);
                          if (skip + aSize <= block_size)
                          {
                              pointer = block_pointer + skip;
                              allocate_blocks(aIndex, pointer, aSize);
                              return 0;
                          }
                          return at(aIndex).next_index();
                      });
            }
        }

        if (!pointer && upstream_)
        {
            pointer = static_cast<std::byte *>(
                upstream_->allocate(aSize, aAlignment));
        }

        assert(is_aligned(pointer, BlockSize));

        return pointer;
    }

    void deallocate_impl(void *aPtr) noexcept
    {
        if (!aPtr)
        {
            return;
        }

        UTILS_ABORT_IF(
            !is_aligned(aPtr, BlockSize),
            "\nreason: pointer(%p) which is being deallocated must be aligned "
            "at least as one block size(%d)",
            aPtr, BlockSize);

        if ((aPtr >= static_cast<void *>(data_)) &&
            (aPtr < static_cast<void *>(data_ + this->size())))
        {
            const BIndex index = pointer_to_block_index(aPtr);
            deallocate_blocks(index);
        }
        else if (upstream_)
        {
            upstream_->deallocate(aPtr);
        }
        else
        {
            UTILS_ABORT_IF(aPtr < static_cast<void *>(data_),
                           "\nreason: deallocating pointer(%p) is less than "
                           "simple_resource's "
                           "left boundary(%p)",
                           aPtr, data_);

            UTILS_ABORT_IF(
                (aPtr >= static_cast<void *>(data_ + this->size())) &&
                    !upstream_,
                "\nreason: deallocating pointer(%p) is more than "
                "simple_resource's "
                "right boundary(%p)",
                aPtr, data_ + this->size());
        }
    }

    BIndex deallocate_blocks(const BIndex aIndex) noexcept
    {
        assert(aIndex < block_count_);
        UTILS_ABORT_IF(not in_use(aIndex),
                       "\nreason: trying to deallocate block(%d) that was not "
                       "marked as allocated",
                       aIndex);

        reset_in_use(aIndex);
        assert(busy_count_ > 0);
        --busy_count_;
        assert(free_count_ < std::numeric_limits<decltype(free_count_)>::max());
        ++free_count_;

        return merge_adjacent_blocks(aIndex);
    }

    template <typename Predicate, typename Action>
    std::enable_if_t<std::is_nothrow_invocable_r_v<bool, Predicate, BIndex> &&
                     std::is_nothrow_invocable_r_v<bool, Action, BIndex>>
    apply(Predicate &&aPredicate, Action &&aAction) noexcept
    {
        BIndex i = 0;
        do
        {
            block_info &info = at(i);
            if (std::invoke(aPredicate, i))
            {
                i = std::invoke(aAction, i);
            }
            else
            {
                i = info.next_index();
            }
        } while (i);
    }

    void allocate_blocks(BIndex aFreeRegion, std::byte *aPtr,
                         std::size_t aSize) noexcept
    {
        const BIndex first = pointer_to_block_index(aPtr);
        const BIndex last = pointer_to_block_index(aPtr + aSize);
        const BIndex after_free = at(aFreeRegion).next_index();
        const BIndex last_of_free =
            after_free ? after_free - 1 : static_cast<BIndex>(block_count_ - 1);
        if (aFreeRegion == first)
        {
            if (last != last_of_free)
            {
                details::allocate_begin_matched(*this, aFreeRegion, last,
                                                last_of_free);
            }
            else
            {
                details::allocate_matched(*this, aFreeRegion);
            }
        }
        else
        {
            if (last != last_of_free)
            {
                details::allocate_not_matched(*this, aFreeRegion, first, last,
                                              last_of_free);
            }
            else
            {
                details::allocate_end_matched(*this, aFreeRegion, first, last);
            }
        }
    }

    inline BIndex pointer_to_block_index(void const *aPtr) const noexcept
    {
        const uintptr_t begin = reinterpret_cast<uintptr_t>(data_);
        const uintptr_t ptr = reinterpret_cast<uintptr_t>(aPtr);

        assert(ptr >= begin);
        assert(ptr < reinterpret_cast<uintptr_t>(data_ + kMaxResourceSize));

        return static_cast<BIndex>((ptr - begin) / BlockSize);
    }

    inline std::byte *block_index_to_pointer(BIndex aIndex) noexcept
    {
        assert(aIndex < block_count_);
        return data_ + aIndex * BlockSize;
    }

    constexpr BIndex merge_if_both_not_in_use(BIndex aLeft,
                                              BIndex aRight) noexcept
    {
        if ((aLeft == aRight) || !aRight)
        {
            return aLeft;
        }
        assert(aLeft < aRight);
        assert(not in_use(aLeft) || not in_use(aRight));

        if (not in_use(aLeft))
        {
            if (not in_use(aRight))
            {
                assert(free_count_ > 0);
                auto &right_info = at(aRight);
                set_prev(right_info.next_index(), aLeft);
                set_next(aLeft, right_info.next_index());
                --free_count_;
            }
            return aLeft;
        }
        return aRight;
    }

    constexpr BIndex merge_adjacent_blocks(BIndex aIndex) noexcept
    {
        block_info &info = at(aIndex);
        assert(not in_use(aIndex));
        const BIndex merged =
            merge_if_both_not_in_use(info.prev_index(), aIndex);
        return merge_if_both_not_in_use(merged, info.next_index());
    }

    void set_prev(const BIndex aIndex, const BIndex aPrev) noexcept
    {
        assert(aIndex < block_count_);
        if ((aPrev < aIndex) || !aPrev)
        {
            at(aIndex).prev_index_ = aPrev;
        }
    }

    void set_next(const BIndex aIndex, const BIndex aNext) noexcept
    {
        assert(aNext < block_count_);
        if ((aIndex < aNext) || !aNext)
        {
            at(aIndex).next_index_ = aNext;
        }
    }

    void set_prev_next(const BIndex aIndex, const BIndex aPrev,
                       const BIndex aNext) noexcept
    {
        assert(aIndex < block_count_);
        assert(aNext < block_count_);
        auto &info = at(aIndex);
        if ((aPrev < aIndex) || !aPrev)
        {
            info.prev_index_ = aPrev;
        }
        if ((aIndex < aNext) || !aNext)
        {
            info.next_index_ = aNext;
        }
    }

    block_info &at(BIndex aIndex) noexcept
    {
        assert(aIndex < block_count_);
        return infos_[aIndex];
    }

    constexpr void set_in_use(BIndex aIndex) noexcept
    {
        assert(aIndex < block_count_);
        in_use_[aIndex] = true;
    }

    constexpr void reset_in_use(BIndex aIndex) noexcept
    {
        assert(aIndex < block_count_);
        in_use_[aIndex] = false;
    }

    std::byte *data_{};
    std::size_t block_count_{};
    memory_resource *upstream_{memory::get_default_resource()};
    block_info infos_[MaxBlockCount]{};
    std::bitset<MaxBlockCount> in_use_{};
    std::size_t free_count_{1};
    std::size_t busy_count_{};
};
}  // namespace memory
}  // namespace utils

#endif /* utils_simple_resource_h */
