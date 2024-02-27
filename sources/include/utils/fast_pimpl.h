#ifndef utils_fast_pimpl_h
#define utils_fast_pimpl_h

#include <new>
#include <type_traits>

namespace utils
{
struct forwarding_constructor
{
};

template <typename T, std::size_t Size, std::size_t Alignment>
class fast_pimpl final
{
   public:
    ~fast_pimpl()
    {
        validate<sizeof(T), alignof(T)>();
        get().~T();
    }

    fast_pimpl() noexcept(std::is_nothrow_constructible_v<T>)
    {
        new (&get()) T();
    }

    template <typename... Args>
    explicit fast_pimpl(forwarding_constructor, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
    {
        new (&get()) T(std::forward<Args>(args)...);
    }

    fast_pimpl(const fast_pimpl &aOther) noexcept(
        std::is_nothrow_copy_constructible_v<T>)
    {
        new (&get()) T(aOther.get());
    }

    fast_pimpl(const T &aOther) noexcept(
        std::is_nothrow_copy_constructible_v<T>)
    {
        new (&get()) T(aOther);
    }

    fast_pimpl(fast_pimpl &&aOther) noexcept(
        std::is_nothrow_move_constructible_v<T>)
    {
        new (&get()) T(std::move(aOther.get()));
    }

    fast_pimpl(T &&aOther) noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        new (&get()) T(std::move(aOther));
    }

    fast_pimpl &operator=(const fast_pimpl &aOther) noexcept(
        std::is_nothrow_copy_assignable_v<T>)
    {
        get() = aOther.get();
        return *this;
    }

    fast_pimpl &operator=(const T &aOther) noexcept(
        std::is_nothrow_copy_assignable_v<T>)
    {
        get() = aOther;
        return *this;
    }

    fast_pimpl &operator=(fast_pimpl &&aOther) noexcept(
        std::is_nothrow_move_assignable_v<T>)
    {
        get() = std::move(aOther.get());
        return *this;
    }

    fast_pimpl &operator=(T &&aOther) noexcept(
        std::is_nothrow_move_assignable_v<T>)
    {
        get() = std::move(aOther);
        return *this;
    }

    T *operator->() noexcept { return &get(); }

    const T *operator->() const noexcept { return &get(); }

    template <std::size_t ActualSize, std::size_t ActualAlignment>
    static void validate() noexcept
    {
        static_assert(Size == ActualSize,
                      "Error: Size and sizeof(T) must be equal");
        static_assert(Alignment == ActualAlignment,
                      "Error: Alignment and alignof(T) must be equal");
    }

    T &get() noexcept { return reinterpret_cast<T &>(*this); }

    const T &get() const noexcept { return reinterpret_cast<const T &>(*this); }

   private:
    alignas(Alignment) std::byte data_[Size]{};
};
}  // namespace utils

#endif /* utils_fast_pimpl_h */
