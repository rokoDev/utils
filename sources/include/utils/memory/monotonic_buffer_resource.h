#ifndef utils_monotonic_buffer_resource_h
#define utils_monotonic_buffer_resource_h

#include <functional>

#include "memory.h"

namespace utils
{
namespace memory
{
class monotonic_buffer_resource : public memory_resource
{
    friend class memory_resource;

   public:
    monotonic_buffer_resource(const monotonic_buffer_resource &) = delete;
    monotonic_buffer_resource(std::byte *const aBuffer, const std::size_t aSize,
                              memory_resource *aUpstream) noexcept
        : memory_resource(
              &monotonic_buffer_resource::allocate<monotonic_buffer_resource>,
              &monotonic_buffer_resource::deallocate<monotonic_buffer_resource>)
        , data_(aSize ? aBuffer : nullptr)
        , size_(aBuffer ? aSize : 0)
        , upstream_(aUpstream)
        , next_(data_)
    {
    }

    monotonic_buffer_resource(std::byte *const aBuffer,
                              const std::size_t aSize) noexcept
        : monotonic_buffer_resource(aBuffer, aSize, get_default_resource())
    {
    }

    template <std::size_t BufSize>
    monotonic_buffer_resource(std::byte (&aBuffer)[BufSize],
                              memory_resource *aUpstream) noexcept
        : monotonic_buffer_resource(aBuffer, BufSize, aUpstream)
    {
    }

    template <std::size_t BufSize>
    monotonic_buffer_resource(std::byte (&aBuffer)[BufSize]) noexcept
        : monotonic_buffer_resource(aBuffer, BufSize, get_default_resource())
    {
    }

    bool is_equal(const memory_resource &aOther) const noexcept override
    {
        return &aOther == this;
    }

    std::byte const *data() const noexcept { return data_; }

    std::size_t size() const noexcept { return size_; }

    void release() noexcept { next_ = data_; }

   private:
    [[nodiscard]] void *allocate_impl(std::size_t aSize,
                                      std::size_t aAlignment) noexcept
    {
        if (!aSize)
        {
            return nullptr;
        }

        assert(is_power_of_2(aAlignment));

        std::byte *pointer{};
        if (data_)
        {
            const auto skip = skip_to_align(next_, aAlignment);
            std::byte *supposed_result = next_ + skip;
            if (std::less_equal<std::byte *>{}(supposed_result + aSize,
                                               data_ + size_))
            {
                next_ = supposed_result + aSize;
                pointer = supposed_result;
            }
        }

        if (!pointer && upstream_)
        {
            pointer = static_cast<std::byte *>(
                upstream_->allocate(aSize, aAlignment));
        }

        return pointer;
    }

    void deallocate_impl(void *) noexcept {}

    std::byte *data_{};
    std::size_t size_{};
    memory_resource *upstream_{memory::get_default_resource()};
    std::byte *next_{};
};
}  // namespace memory
}  // namespace utils

#endif /* utils_monotonic_buffer_resource_h */
