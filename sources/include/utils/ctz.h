#ifndef utils_ctz_h
#define utils_ctz_h

#include <type_traits>

#include "common.h"
#include "details/cxz.h"
#include "type_list.h"

namespace utils
{
namespace details
{
template <typename T>
constexpr decltype(auto) calc_ctz(T aValue) noexcept;

template <typename T>
constexpr unsigned long crafted_ctz(T aValue) noexcept
{
    assert(aValue > 0 && "Invalid aValue");
    unsigned long ctz{};
    for (; (ctz < sizeof(T) * CHAR_BIT - 1) && !(aValue & 1);
         aValue >>= 1, ++ctz)
        ;
    return ctz;
}
}  // namespace details
#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(_BitScanForward)
constexpr unsigned long ctz_ul(unsigned long aValue) noexcept
{
    assert(aValue > 0 && "Invalid aValue");
    if (__builtin_is_constant_evaluated())
    {
        return details::crafted_ctz(aValue);
    }
    else
    {
        unsigned long ctz = 0;
        _BitScanForward(&ctz, aValue);
        return ctz;
    }
}
namespace details
{
template <>
constexpr decltype(auto) calc_ctz<unsigned long>(unsigned long aValue) noexcept
{
    return ctz_ul(aValue);
}
}  // namespace details
#if UINTPTR_MAX == UINT64_MAX
#pragma intrinsic(_BitScanForward)
constexpr unsigned long ctz_ull(unsigned __int64 aValue) noexcept
{
    assert(aValue > 0 && "Invalid aValue");
    if (__builtin_is_constant_evaluated())
    {
        return details::crafted_ctz(aValue);
    }
    else
    {
        unsigned long ctz = 0;
        _BitScanForward64(&ctz, aValue);
        return ctz;
    }
}

namespace details
{
using supported_ctz_types = ::utils::type_list<unsigned long, unsigned __int64>;

template <>
constexpr decltype(auto) calc_ctz<unsigned __int64>(
    unsigned __int64 aValue) noexcept
{
    return ctz_ull(aValue);
}
}  // namespace details
#else

namespace details
{
using supported_ctz_types = ::utils::type_list<unsigned long>;
}  // namespace details

#endif
#else
constexpr decltype(auto) ctz_ui(unsigned int aValue) noexcept
{
    assert(aValue > 0 && "Invalid aValue");
    return __builtin_ctz(aValue);
}

constexpr decltype(auto) ctz_ul(unsigned long aValue) noexcept
{
    assert(aValue > 0 && "Invalid aValue");
    return __builtin_ctzl(aValue);
}

constexpr decltype(auto) ctz_ull(unsigned long long aValue) noexcept
{
    assert(aValue > 0 && "Invalid aValue");
    return __builtin_ctzll(aValue);
}

namespace details
{
using supported_ctz_types =
    ::utils::type_list<unsigned int, unsigned long, unsigned long long>;

template <>
constexpr decltype(auto) calc_ctz<unsigned int>(unsigned int aValue) noexcept
{
    return ctz_ui(aValue);
}

template <>
constexpr decltype(auto) calc_ctz<unsigned long>(unsigned long aValue) noexcept
{
    return ctz_ul(aValue);
}

template <>
constexpr decltype(auto) calc_ctz<unsigned long long>(
    unsigned long long aValue) noexcept
{
    return ctz_ull(aValue);
}
}  // namespace details

#endif

template <typename T, typename = std::enable_if_t<is_uint_v<T>>>
constexpr decltype(auto) ctz(T aValue) noexcept
{
    using sorted_ctz_types =
        sort_sizeof_ascending_t<details::supported_ctz_types>;
    using equal_size_list =
        typename sorted_ctz_types::list_of_predicate_compliant_t<is_equal_sizes,
                                                                 T>;

    if constexpr (equal_size_list::size)
    {  // there is intrinsic that support type T
        using equal_size_t = typename equal_size_list::template at<0>;
        static_assert(sizeof(equal_size_t) == sizeof(T));
        return details::calc_ctz<equal_size_t>(
            static_cast<equal_size_t>(aValue));
    }
    else if constexpr (constexpr std::size_t i =
                           sorted_ctz_types::index_of<details::less_eq_pred, T>;
                       i < sorted_ctz_types::size)
    {  // there is intrinsic that support type bigger than type T
        using supported_t = typename sorted_ctz_types::at<i>;
        return details::calc_ctz<supported_t>(static_cast<supported_t>(aValue));
    }
    else
    {
        static_assert(
            i < sorted_ctz_types::size,
            "intrinsics support only types with size less than sizeof(T)");
        // intrinsics support only types with size less than sizeof(T). Fallback
        // to handmade ctz. return details::crafted_ctz(aValue);
    }
}
}  // namespace utils

#endif /* utils_ctz_h */
