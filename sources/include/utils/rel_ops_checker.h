#ifndef utils_rel_ops_checker_h
#define utils_rel_ops_checker_h

#include <type_traits>
#include <utility>

#include "detector.h"

namespace utils
{
namespace details
{
template <typename T>
using equality_comparison_result =
    decltype(std::declval<T>() == std::declval<T>());

template <typename T>
using noexcept_equality_comparison_result = decltype(
    std::bool_constant<noexcept(std::declval<T>() == std::declval<T>())>{});

template <typename T>
using less_comparison_result = decltype(std::declval<T>() < std::declval<T>());

template <typename T>
using noexcept_less_comparison_result = decltype(
    std::bool_constant<noexcept(std::declval<T>() < std::declval<T>())>{});
}  // namespace details

template <typename T>
struct is_equality_comparable
    : is_detected_exact<bool, details::equality_comparison_result, T>
{
};

template <typename T>
inline constexpr bool is_equality_comparable_v =
    is_equality_comparable<T>::value;

template <typename T>
struct is_noexcept_equality_comparable
    : is_detected_exact<std::true_type,
                        details::noexcept_equality_comparison_result, T>
{
};

template <typename T>
inline constexpr bool is_noexcept_equality_comparable_v =
    is_noexcept_equality_comparable<T>::value;

template <typename T>
struct is_less_comparable
    : is_detected_exact<bool, details::less_comparison_result, T>
{
};

template <typename T>
inline constexpr bool is_less_comparable_v = is_less_comparable<T>::value;

template <typename T>
struct is_noexcept_less_comparable
    : is_detected_exact<std::true_type,
                        details::noexcept_less_comparison_result, T>
{
};

template <typename T>
inline constexpr bool is_noexcept_less_comparable_v =
    is_noexcept_less_comparable<T>::value;
}  // namespace utils

#endif /* utils_rel_ops_checker_h */
