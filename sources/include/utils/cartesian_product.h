#ifndef utils_cartesian_product_h
#define utils_cartesian_product_h

namespace utils
{
template <typename... Ts>
struct type_list;

template <decltype(auto)... Values>
struct value_list;

template <typename... Ts>
struct concatenate;

template <decltype(auto)... V>
struct concatenate<value_list<V...>>
{
    using type = value_list<V...>;
};

template <decltype(auto)... V1, decltype(auto)... V2, typename... Ts>
struct concatenate<value_list<V1...>, value_list<V2...>, Ts...>
{
    using type = typename concatenate<value_list<V1..., V2...>, Ts...>::type;
};

template <typename... Ts>
struct concatenate<type_list<Ts...>>
{
    using type = type_list<Ts...>;
};

template <typename... Ts1, typename... Ts2, typename... Ts>
struct concatenate<type_list<Ts1...>, type_list<Ts2...>, Ts...>
{
    using type = typename concatenate<type_list<Ts1..., Ts2...>, Ts...>::type;
};

template <typename... Ts>
using concatenate_t = typename concatenate<Ts...>::type;

namespace details
{
template <bool IsFirstPair, typename Set1, typename Set2>
struct one_element_mult;

template <decltype(auto) Value, decltype(auto)... Values>
struct one_element_mult<true, value_list<Value>, value_list<Values...>>
{
    using type = type_list<value_list<Value, Values>...>;
};

template <decltype(auto) Value, typename... Ts>
struct one_element_mult<false, value_list<Value>, type_list<Ts...>>
{
    using type = type_list<concatenate_t<value_list<Value>, Ts>...>;
};

template <typename T, typename... Ts>
struct one_element_mult<true, type_list<T>, type_list<Ts...>>
{
    using type = type_list<type_list<T, Ts>...>;
};

template <typename T, typename... Ts>
struct one_element_mult<false, type_list<T>, type_list<Ts...>>
{
    using type = type_list<concatenate_t<type_list<T>, Ts>...>;
};

template <std::size_t I, typename List1, typename List2, typename... Lists>
struct cartesian_product_impl;

template <std::size_t I, typename List1, typename List2, typename List3,
          typename... Lists>
struct cartesian_product_impl<I, List1, List2, List3, Lists...>
{
    using type = typename cartesian_product_impl<
        I, List1,
        typename cartesian_product_impl<I - 1, List2, List3,
                                        Lists...>::type>::type;
};

template <std::size_t I, decltype(auto)... Values, typename List>
struct cartesian_product_impl<I, value_list<Values...>, List>
{
    using type = concatenate_t<
        typename one_element_mult<I == 0, value_list<Values>, List>::type...>;
};

template <std::size_t I, typename... Ts, typename List>
struct cartesian_product_impl<I, type_list<Ts...>, List>
{
    using type = concatenate_t<
        typename one_element_mult<I == 0, type_list<Ts>, List>::type...>;
};
}  // namespace details

template <typename List1, typename List2, typename... Lists>
struct cartesian_product
{
    using type =
        typename details::cartesian_product_impl<sizeof...(Lists), List1, List2,
                                                 Lists...>::type;
};

template <typename List1, typename List2>
struct cartesian_product<List1, List2>
{
    using type =
        typename details::cartesian_product_impl<0, List1, List2>::type;
};

template <typename List, typename... Lists>
using cartesian_product_t = typename cartesian_product<List, Lists...>::type;
}  // namespace utils

#endif /* utils_cartesian_product_h */
