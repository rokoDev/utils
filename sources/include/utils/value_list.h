#ifndef utils_value_list_h
#define utils_value_list_h

#include <cstddef>
#include <type_traits>

#include "type_list.h"

namespace utils
{
namespace details
{
template <auto... Values>
struct value_list
{
    template <std::size_t Index, auto... U>
    struct at_impl;

    template <auto U, auto... V>
    struct at_impl<0, U, V...>
    {
        static constexpr auto value = U;
    };

    template <std::size_t Index, auto U, auto... V>
    struct at_impl<Index, U, V...>
    {
        static constexpr auto value = at_impl<Index - 1, V...>::value;
    };

    template <typename U, typename V, U u, V v>
    struct is_same_impl_impl : std::false_type
    {
    };

    template <typename T, T v1, T v2>
    struct is_same_impl_impl<T, T, v1, v2> : std::bool_constant<v1 == v2>
    {
    };

    template <auto V1, auto V2>
    struct is_same_impl : is_same_impl_impl<decltype(V1), decltype(V2), V1, V2>
    {
    };

    template <auto V1, auto V2>
    static constexpr std::size_t is_same_impl_v = is_same_impl<V1, V2>::value;

    template <auto Value, auto... Vs>
    struct count_of_impl
        : std::integral_constant<std::size_t, (0 + ... +
                                               static_cast<std::size_t>(
                                                   is_same_impl_v<Value, Vs>))>
    {
    };

    template <auto... Vs>
    struct is_all_same_impl : std::true_type
    {
    };

    template <auto Value, auto... Vs>
    struct is_all_same_impl<Value, Vs...>
        : std::conjunction<is_same_impl<Value, Vs>...>
    {
    };

    template <auto Value, auto... Vs>
    struct more_than_once
        : std::bool_constant<(count_of_impl<Value, Vs...>::value > 1)>
    {
    };

    template <auto Value, auto... Vs>
    struct contains_impl : std::disjunction<is_same_impl<Value, Vs>...>
    {
    };

    template <std::size_t I, auto U, auto... V>
    struct index_of_impl : std::integral_constant<std::size_t, I>
    {
    };

    template <std::size_t I, auto U, auto V1, auto... V>
    struct index_of_impl<I, U, V1, V...>
        : std::conditional_t<is_same_impl_v<U, V1>,
                             std::integral_constant<std::size_t, I>,
                             index_of_impl<I + 1, U, V...>>
    {
    };

    template <typename T, T Min, auto... V>
    struct min_impl;

    template <typename T, T Min, T V1, auto... V>
    struct min_impl<T, Min, V1, V...>
        : std::conditional_t<(type_list<decltype(V)...>::template count_of_impl<
                                  T, decltype(V)...>::value > 0),
                             min_impl<T, std::min(Min, V1), V...>,
                             std::integral_constant<T, std::min(Min, V1)>>
    {
    };

    template <typename T, typename U, T Min, U V1, auto... V>
    struct min_impl<T, Min, V1, V...>
        : std::conditional_t<(type_list<decltype(V)...>::template count_of_impl<
                                  T, decltype(V)...>::value > 0),
                             min_impl<T, Min, V...>,
                             std::integral_constant<T, Min>>
    {
    };

    template <typename T, T Max, auto... V>
    struct max_impl;

    template <typename T, T Max, T V1, auto... V>
    struct max_impl<T, Max, V1, V...>
        : std::conditional_t<(type_list<decltype(V)...>::template count_of_impl<
                                  T, decltype(V)...>::value > 0),
                             max_impl<T, std::max(Max, V1), V...>,
                             std::integral_constant<T, std::max(Max, V1)>>
    {
    };

    template <typename T, typename U, T Max, U V1, auto... V>
    struct max_impl<T, Max, V1, V...>
        : std::conditional_t<(type_list<decltype(V)...>::template count_of_impl<
                                  T, decltype(V)...>::value > 0),
                             max_impl<T, Max, V...>,
                             std::integral_constant<T, Max>>
    {
    };
};
}  // namespace details

template <auto... Values>
struct value_list
{
   private:
    using impl = details::value_list<Values...>;

   public:
    static constexpr std::size_t size = sizeof...(Values);

    template <std::size_t I>
    static constexpr auto at = impl::template at_impl<I, Values...>::value;

    template <auto Value>
    static constexpr std::size_t index_of =
        impl::template index_of_impl<0, Value, Values...>::value;

    struct is_same : impl::template is_all_same_impl<Values...>
    {
    };
    static constexpr bool is_same_v = is_same::value;

    template <auto Value>
    static constexpr std::size_t count_of =
        impl::template count_of_impl<Value, Values...>::value;

    template <auto Value>
    using contains = typename impl::template contains_impl<Value, Values...>;

    template <auto Value>
    static constexpr bool contains_v = contains<Value>::value;

    static constexpr bool contains_copies = std::disjunction_v<
        typename impl::template more_than_once<Values, Values...>...>;

    template <typename T>
    static constexpr bool contains_type =
        type_list<decltype(Values)...>::template contains_v<T>;

    template <typename T>
    static constexpr T min = impl::template min_impl<
        T, at<type_list<decltype(Values)...>::template first_index_of_type<T>>,
        Values...>::value;

    template <typename T>
    static constexpr auto max = impl::template max_impl<
        T, at<type_list<decltype(Values)...>::template first_index_of_type<T>>,
        Values...>::value;
};

template <typename T>
struct is_value_list : std::false_type
{
};

template <auto... Values>
struct is_value_list<value_list<Values...>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_value_list_v = is_value_list<T>::value;
}  // namespace utils

#endif /* utils_value_list_h */
