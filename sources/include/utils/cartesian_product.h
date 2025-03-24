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
}  // namespace utils

#endif /* utils_cartesian_product_h */
