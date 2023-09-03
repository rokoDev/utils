#ifndef utils_allocator_h
#define utils_allocator_h

#include <type_traits>

#include "memory.h"

namespace utils
{
namespace memory
{
template <typename T>
class allocator
{
   public:
    using value_type = T;

    allocator() noexcept = default;
    allocator(const allocator&) = default;
    allocator& operator=(const allocator&) = delete;

    template <typename U>
    allocator(const allocator<U>& aOther) noexcept : resource_(aOther.resource_)
    {
    }

    allocator(memory_resource* aResource) noexcept : resource_(aResource) {}

    [[nodiscard]] T* allocate(std::size_t aCount) noexcept
    {
        return static_cast<T*>(
            resource_->allocate(aCount * sizeof(T), alignof(T)));
    }

    void deallocate(T* aPtr) noexcept { resource_->deallocate(aPtr); }

    template <typename U, typename... Args>
    void construct(U* aPtr, Args&&... aArgs) noexcept
    {
        new (aPtr) U(std::forward<T>(aArgs)...);
    }

    [[nodiscard]] void* allocate_bytes(
        std::size_t aNBytes,
        std::size_t aAlignment = alignof(std::max_align_t)) noexcept
    {
        return resource_->allocate(aNBytes, aAlignment);
    }

    void deallocate_bytes(void* aPtr) noexcept { resource_->deallocate(aPtr); }

    template <typename U>
    [[nodiscard]] U* allocate_object(std::size_t aCount = 1) noexcept
    {
        return static_cast<U*>(allocate_bytes(aCount * sizeof(U), alignof(U)));
    }

    template <typename U>
    void deallocate_object(U* aPtr) noexcept
    {
        deallocate_bytes(aPtr);
    }

    template <typename U, typename... CtorArgs>
    [[nodiscard]] U* new_object(CtorArgs&&... aArgs) noexcept
    {
        static_assert(std::is_nothrow_constructible_v<U, CtorArgs...>);
        U* ptr = this->allocate_object<U>();
        if (ptr)
        {
            this->construct(ptr, aArgs...);
        }
        return ptr;
    }

    template <typename U>
    void delete_object(U* aPtr) noexcept
    {
        if (aPtr)
        {
            aPtr->~U();
            deallocate_object(aPtr);
        }
    }

    memory_resource* resource() const noexcept { return resource_; }

    friend bool operator==(const allocator& aLhs,
                           const allocator& aRhs) noexcept
    {
        return *aLhs.resource_ == *aRhs.resource_;
    }

    friend bool operator!=(const allocator& aLhs,
                           const allocator& aRhs) noexcept
    {
        return !(aLhs == aRhs);
    }

   private:
    memory_resource* resource_{get_default_resource()};
};

template <class T1, class T2>
bool operator==(const allocator<T1>& aLhs, const allocator<T2>& aRhs) noexcept
{
    return *aLhs.resource_ == *aRhs.resource_;
}

template <class T1, class T2>
bool operator!=(const allocator<T1>& aLhs, const allocator<T2>& aRhs) noexcept
{
    return !(aLhs == aRhs);
}
}  // namespace memory
}  // namespace utils

#endif /* utils_allocator_h */
