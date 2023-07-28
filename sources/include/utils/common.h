#ifndef utils_common_h
#define utils_common_h

#include <type_traits>

#include "value_list.h"

namespace utils
{
template <typename T>
struct is_uint
    : std::conjunction<std::is_unsigned<T>,
                       std::negation<std::is_same<T, bool>>>
{
};

template <typename T>
inline constexpr bool is_uint_v = is_uint<T>::value;

template <typename... Ts>
struct is_equal_sizes : value_list<sizeof(Ts)...>::is_same
{
};

template <typename... Ts>
inline constexpr bool is_equal_sizes_v = is_equal_sizes<Ts...>::value;
}  // namespace utils

#endif /* utils_common_h */
