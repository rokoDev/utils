#ifndef utils_padding_checker_h
#define utils_padding_checker_h

#include <boost/pfr.hpp>
#include <type_traits>
#include <utility>

namespace utils
{
namespace pfr = boost::pfr;

namespace details
{
template <typename T, bool IsAggregate>
struct sum_size_of_all_members;

template <typename T, std::size_t... I>
std::integral_constant<
    std::size_t,
    (... + sum_size_of_all_members<
               pfr::tuple_element_t<I, T>,
               std::is_aggregate_v<pfr::tuple_element_t<I, T>>>::value)>
    sum(std::index_sequence<I...>);

template <typename T, bool IsAggregate>
struct sum_size_of_all_members
    : std::integral_constant<
          std::size_t, decltype(sum<T>(std::declval<std::make_index_sequence<
                                           pfr::tuple_size_v<T>>>()))::value>
{
};

template <typename T>
struct sum_size_of_all_members<T, false>
    : std::integral_constant<std::size_t, sizeof(T)>
{
};

template <typename T>
struct has_padding_impl
    : std::bool_constant<sizeof(T) != sum_size_of_all_members<
                                          T, std::is_aggregate_v<T>>::value>
{
};
}  // namespace details

template <typename T>
struct has_padding
    : std::disjunction<std::negation<std::is_trivially_copyable<T>>,
                       details::has_padding_impl<T>>
{
};

template <typename T>
inline constexpr bool has_padding_v = has_padding<T>::value;
}  // namespace utils

#endif /* utils_padding_checker_h */
