#ifndef utils_value_list_h
#define utils_value_list_h

#include <cstddef>
#include <tuple>
#include <type_traits>

#include "type_list.h"

namespace utils
{
namespace details
{
template <decltype(auto)... Values>
struct value_list
{
    template <std::size_t I>
    using at_t = std::tuple_element_t<I, std::tuple<decltype(Values)...>>;

    template <std::size_t I>
    static constexpr at_t<I> at_v =
        std::get<I>(std::tuple<decltype(Values)...>(Values...));

    template <typename U, typename V, U u, V v>
    struct is_same_impl_impl : std::false_type
    {
    };

    template <typename T, T v1, T v2>
    struct is_same_impl_impl<T, T, v1, v2> : std::bool_constant<v1 == v2>
    {
    };

    template <decltype(auto) V1, decltype(auto) V2>
    struct is_same_impl : is_same_impl_impl<decltype(V1), decltype(V2), V1, V2>
    {
    };

    template <decltype(auto) V1, decltype(auto) V2>
    static constexpr std::size_t is_same_impl_v = is_same_impl<V1, V2>::value;

    template <decltype(auto) Value, decltype(auto)... Vs>
    struct count_of_impl
        : std::integral_constant<std::size_t, (0 + ... +
                                               static_cast<std::size_t>(
                                                   is_same_impl_v<Value, Vs>))>
    {
    };

    template <decltype(auto)... Vs>
    struct is_all_same_impl : std::true_type
    {
    };

    template <decltype(auto) Value, decltype(auto)... Vs>
    struct is_all_same_impl<Value, Vs...>
        : std::conjunction<is_same_impl<Value, Vs>...>
    {
    };

    template <decltype(auto) Value, decltype(auto)... Vs>
    struct more_than_once
        : std::bool_constant<(count_of_impl<Value, Vs...>::value > 1)>
    {
    };

    template <decltype(auto) Value, decltype(auto)... Vs>
    struct contains_impl : std::disjunction<is_same_impl<Value, Vs>...>
    {
    };

    template <std::size_t I, decltype(auto) U, decltype(auto)... V>
    struct index_of_impl : std::integral_constant<std::size_t, I>
    {
    };

    template <std::size_t I, decltype(auto) U, decltype(auto) V1,
              decltype(auto)... V>
    struct index_of_impl<I, U, V1, V...>
        : std::conditional_t<is_same_impl_v<U, V1>,
                             std::integral_constant<std::size_t, I>,
                             index_of_impl<I + 1, U, V...>>
    {
    };

    template <typename T, decltype(auto) V>
    struct all_of_impl
    {
        using type = ::utils::value_list<>;
    };

    template <typename T, T V>
    struct all_of_impl<T, V>
    {
        using type = ::utils::value_list<V>;
    };

    template <typename T, decltype(auto) V>
    using all_of_impl_t = typename all_of_impl<T, V>::type;

    template <typename T, decltype(auto)... Vs>
    struct all_of
    {
        using type = concatenate_t<all_of_impl_t<T, Vs>...>;
    };

    template <typename T>
    struct all_of<T>
    {
        using type = ::utils::value_list<>;
    };

    template <typename T, decltype(auto)... Vs>
    struct min_impl
    {
        using list_of_T = typename all_of<T, Vs...>::type;
        template <typename ValueList, std::size_t... I>
        static constexpr std::integral_constant<
            T, std::min({ValueList::template at<I>...})>
            calc_min(std::index_sequence<I...>) noexcept;
        static constexpr T value = decltype(calc_min<list_of_T>(
            std::make_index_sequence<list_of_T::size>{}))::value;
    };

    template <typename T, decltype(auto)... Vs>
    struct max_impl
    {
        using list_of_T = typename all_of<T, Vs...>::type;
        template <typename ValueList, std::size_t... I>
        static constexpr std::integral_constant<
            T, std::max({ValueList::template at<I>...})>
            calc_min(std::index_sequence<I...>) noexcept;
        static constexpr T value = decltype(calc_min<list_of_T>(
            std::make_index_sequence<list_of_T::size>{}))::value;
    };
};
}  // namespace details

template <decltype(auto)... Values>
struct value_list
{
   private:
    using impl = details::value_list<Values...>;

   public:
    static constexpr std::size_t size = sizeof...(Values);

    template <std::size_t I>
    static constexpr decltype(auto) at = impl::template at_v<I>;

    template <std::size_t I>
    using at_t = typename impl::template at_t<I>;

    template <decltype(auto) Value>
    static constexpr std::size_t index_of =
        impl::template index_of_impl<0, Value, Values...>::value;

    struct is_same : impl::template is_all_same_impl<Values...>
    {
    };
    static constexpr bool is_same_v = is_same::value;

    template <decltype(auto) Value>
    static constexpr std::size_t count_of =
        impl::template count_of_impl<Value, Values...>::value;

    template <decltype(auto) Value>
    using contains = typename impl::template contains_impl<Value, Values...>;

    template <decltype(auto) Value>
    static constexpr bool contains_v = contains<Value>::value;

    static constexpr bool contains_copies = std::disjunction_v<
        typename impl::template more_than_once<Values, Values...>...>;

    template <typename T>
    static constexpr bool contains_type =
        type_list<decltype(Values)...>::template contains_v<T>;

    template <typename T>
    static constexpr T min = impl::template min_impl<T, Values...>::value;

    template <typename T>
    static constexpr T max = impl::template max_impl<T, Values...>::value;

    template <typename T>
    struct list_all_of
    {
        using type = typename impl::template all_of<T, Values...>::type;
    };

    template <typename T>
    using list_all_of_t = typename list_all_of<T>::type;
};

template <typename T>
struct is_value_list : std::false_type
{
};

template <decltype(auto)... Values>
struct is_value_list<value_list<Values...>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_value_list_v = is_value_list<T>::value;
}  // namespace utils

#endif /* utils_value_list_h */
