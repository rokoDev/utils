#ifndef utils_memory_h
#define utils_memory_h

#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <functional>

#include "ctz.h"

namespace utils
{
namespace memory
{
[[nodiscard]] inline std::size_t get_alignment(void const *const aPtr) noexcept
{
    assert(aPtr && "invalid pointer");
    const auto ptrAsUInt = reinterpret_cast<std::uintptr_t>(aPtr);
    const auto power = ctz(ptrAsUInt);
    return std::size_t{1} << power;
}

inline void *aligned_malloc(std::size_t aSize, std::size_t aAlignment) noexcept
{
    void *result;
#ifdef _MSC_VER
    result = _aligned_malloc(aSize, aAlignment);
#else
    aAlignment = aAlignment >= sizeof(void *) ? aAlignment : sizeof(void *);
    if (posix_memalign(&result, aAlignment, aSize))
    {
        result = 0;
    }
#endif
    return result;
}

inline void aligned_free(void *aPtr) noexcept
{
#ifdef _MSC_VER
    _aligned_free(aPtr);
#else
    free(aPtr);
#endif
}

class memory_resource
{
   protected:
    using allocate_t = void *(
        memory_resource::*)(std::size_t aSize, std::size_t aAlignment) noexcept;
    using deallocate_t =
        void (memory_resource::*)(void *aPtrToDeallocate) noexcept;

    template <typename Resource>
    void *allocate(std::size_t aSize, std::size_t aAlignment) noexcept
    {
        return static_cast<Resource *>(this)->allocate_impl(aSize, aAlignment);
    }

    template <typename Resource>
    void deallocate(void *aPtr) noexcept
    {
        static_cast<Resource *>(this)->deallocate_impl(aPtr);
    }

    memory_resource(allocate_t aAllocate, deallocate_t aDeallocate) noexcept
        : allocate_(aAllocate), deallocate_(aDeallocate)
    {
    }

   public:
    virtual ~memory_resource() = default;
    [[nodiscard]] void *allocate(
        std::size_t aSize,
        std::size_t aAlignment = alignof(std::max_align_t)) noexcept
    {
        return std::invoke(allocate_, this, aSize, aAlignment);
    }

    template <typename T>
    [[nodiscard]] void *allocate() noexcept
    {
        return allocate(sizeof(T), alignof(T));
    }

    void deallocate(void *aPtr) noexcept
    {
        std::invoke(deallocate_, this, aPtr);
    }

    virtual bool is_equal(const memory_resource &aOther) const noexcept = 0;

    friend bool operator==(const memory_resource &a,
                           const memory_resource &b) noexcept
    {
        return a.is_equal(b);
    }

    friend bool operator!=(const memory_resource &a,
                           const memory_resource &b) noexcept
    {
        return !(a == b);
    }

   protected:
    allocate_t allocate_{};
    deallocate_t deallocate_{};
};

memory_resource *system_memory_resource() noexcept;
memory_resource *null_memory_resource() noexcept;

namespace details
{
inline std::atomic<memory_resource *> default_resource =
    system_memory_resource();

class system_memory_resource : public memory_resource
{
   public:
    system_memory_resource(const system_memory_resource &) = delete;

    bool is_equal(const memory_resource &aOther) const noexcept override
    {
        return &aOther == this;
    }

   private:
    friend class memory_resource;
    friend memory_resource * ::utils::memory::system_memory_resource() noexcept;

    system_memory_resource()
        : memory_resource(
              &system_memory_resource::allocate<system_memory_resource>,
              &system_memory_resource::deallocate<system_memory_resource>)
    {
    }

    [[nodiscard]] void *allocate_impl(std::size_t aSize,
                                      std::size_t aAlignment) noexcept
    {
        return aligned_malloc(aSize, aAlignment);
    }

    void deallocate_impl(void *aPtr) noexcept { aligned_free(aPtr); }
};

class null_memory_resource : public memory_resource
{
   public:
    null_memory_resource(const null_memory_resource &) = delete;

    bool is_equal(const memory_resource &aOther) const noexcept override
    {
        return &aOther == this;
    }

   private:
    friend class memory_resource;
    friend memory_resource * ::utils::memory::null_memory_resource() noexcept;

    null_memory_resource()
        : memory_resource(
              &null_memory_resource::allocate<null_memory_resource>,
              &null_memory_resource::deallocate<null_memory_resource>)
    {
    }

    [[nodiscard]] void *allocate_impl(std::size_t, std::size_t) noexcept
    {
        return nullptr;
    }

    void deallocate_impl(void *) noexcept {}
};
}  // namespace details

memory_resource *system_memory_resource() noexcept
{
    static details::system_memory_resource r;
    return &r;
}

memory_resource *null_memory_resource() noexcept
{
    static details::null_memory_resource r;
    return &r;
}

memory_resource *set_default_resource(memory_resource *aResource) noexcept
{
    return details::default_resource.exchange(aResource);
}

memory_resource *get_default_resource() noexcept
{
    return details::default_resource.load();
}
}  // namespace memory
}  // namespace utils

#endif /* utils_memory_h */
