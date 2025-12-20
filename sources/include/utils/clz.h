#ifndef utils_clz_h
#define utils_clz_h

#include <type_traits>

#include "common.h"
#include "details/cxz.h"
#include "type_list.h"

namespace utils
{
namespace details
{
template <typename T>
constexpr decltype(auto) calc_clz(T aValue) noexcept;

template <typename T, typename = std::enable_if_t<is_uint_v<T>>>
constexpr unsigned long crafted_clz(T aValue) noexcept
{
    assert(aValue > 0 && "Invalid aValue");
    unsigned long clz{};
    for (T to_test = static_cast<T>(1) << (sizeof(T) * CHAR_BIT - 1);
         (clz < sizeof(T) * CHAR_BIT - 1) && !(aValue & to_test);
         to_test >>= 1, ++clz)
        ;
    return clz;
}
}  // namespace details
#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(_BitScanReverse)
constexpr unsigned long clz_ul(unsigned long aValue) noexcept
{
    assert(aValue > 0 && "Invalid aValue");
    if (__builtin_is_constant_evaluated())
    {
        return details::crafted_clz(aValue);
    }
    else
    {
        unsigned long clz = 0;
        _BitScanReverse(&clz, aValue);
        clz = static_cast<unsigned long>(sizeof(aValue) * CHAR_BIT - clz - 1);
        return clz;
    }
}
namespace details
{
template <>
constexpr decltype(auto) calc_clz<unsigned long>(unsigned long aValue) noexcept
{
    return clz_ul(aValue);
}
}  // namespace details
#if UINTPTR_MAX == UINT64_MAX
#pragma intrinsic(_BitScanReverse)
constexpr unsigned long clz_ull(unsigned __int64 aValue) noexcept
{
    assert(aValue > 0 && "Invalid aValue");
    if (__builtin_is_constant_evaluated())
    {
        return details::crafted_clz(aValue);
    }
    else
    {
        unsigned long clz = 0;
        _BitScanReverse64(&clz, aValue);
        clz = static_cast<unsigned long>(sizeof(aValue) * CHAR_BIT - clz - 1);
        return clz;
    }
}

namespace details
{
using supported_clz_types = ::utils::type_list<unsigned long, unsigned __int64>;

template <>
constexpr decltype(auto) calc_clz<unsigned __int64>(
    unsigned __int64 aValue) noexcept
{
    return clz_ull(aValue);
}
}  // namespace details
#else

namespace details
{
using supported_clz_types = ::utils::type_list<unsigned long>;
}  // namespace details

#endif
#else
constexpr decltype(auto) clz_ui(unsigned int aValue) noexcept
{
    assert(aValue > 0 && "Invalid aValue");
    return __builtin_clz(aValue);
}

constexpr decltype(auto) clz_ul(unsigned long aValue) noexcept
{
    assert(aValue > 0 && "Invalid aValue");
    return __builtin_clzl(aValue);
}

constexpr decltype(auto) clz_ull(unsigned long long aValue) noexcept
{
    assert(aValue > 0 && "Invalid aValue");
    return __builtin_clzll(aValue);
}

namespace details
{
using supported_clz_types =
    ::utils::type_list<unsigned int, unsigned long, unsigned long long>;

template <>
constexpr decltype(auto) calc_clz<unsigned int>(unsigned int aValue) noexcept
{
    return clz_ui(aValue);
}

template <>
constexpr decltype(auto) calc_clz<unsigned long>(unsigned long aValue) noexcept
{
    return clz_ul(aValue);
}

template <>
constexpr decltype(auto) calc_clz<unsigned long long>(
    unsigned long long aValue) noexcept
{
    return clz_ull(aValue);
}
}  // namespace details

#endif

template <typename T, typename = std::enable_if_t<is_uint_v<T>>>
constexpr decltype(auto) clz(T aValue) noexcept
{
    using sorted_clz_types =
        sort_sizeof_ascending_t<details::supported_clz_types>;
    using equal_size_list =
        typename sorted_clz_types::list_of_predicate_compliant_t<is_equal_sizes,
                                                                 T>;

    if constexpr (equal_size_list::size)
    {  // there is intrinsic that support type T
        using equal_size_t = typename equal_size_list::template at<0>;
        static_assert(sizeof(equal_size_t) == sizeof(T));
        return details::calc_clz<equal_size_t>(
            static_cast<equal_size_t>(aValue));
    }
    else if constexpr (constexpr std::size_t i =
                           sorted_clz_types::index_of_v<details::less_eq_pred,
                                                        T>;
                       i < sorted_clz_types::size)
    {  // there is intrinsic that support type bigger than type T
        using supported_t = typename sorted_clz_types::at<i>;
        using ret_type = decltype(details::calc_clz<supported_t>(
            std::declval<supported_t>()));
        const auto bitsize_diff =
            static_cast<ret_type>((sizeof(supported_t) - sizeof(T)) * CHAR_BIT);
        return details::calc_clz<supported_t>(
                   static_cast<supported_t>(aValue)) -
               bitsize_diff;
    }
    else
    {
        static_assert(
            i < sorted_clz_types::size,
            "intrinsics support only types with size less than sizeof(T)");
        // intrinsics support only types with size less than sizeof(T). Fallback
        // to handmade clz. return details::crafted_clz(aValue);
    }
}
}  // namespace utils

#endif /* utils_clz_h */
