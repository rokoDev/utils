#ifndef utils_type_list_h
#define utils_type_list_h

namespace utils
{
namespace details
{
template <typename... Ts>
struct type_list
{
    template <std::size_t Index, typename... U>
    struct at_impl;

    template <typename U, typename... V>
    struct at_impl<0, U, V...>
    {
        using type = U;
    };

    template <std::size_t Index, typename U, typename... V>
    struct at_impl<Index, U, V...>
    {
        using type = typename at_impl<Index - 1, V...>::type;
    };

    template <std::size_t I, template <typename> typename Predicate,
              typename... V>
    struct at_with_predicate;

    template <bool, std::size_t I, template <typename T> typename Predicate,
              typename U, typename... V>
    struct at_with_predicate_impl;

    template <std::size_t I, template <typename T> typename Predicate,
              typename U, typename... V>
    struct at_with_predicate_impl<true, I, Predicate, U, V...>
    {
        using type = typename at_with_predicate<I - 1, Predicate, V...>::type;
    };

    template <std::size_t I, template <typename T> typename Predicate,
              typename U, typename... V>
    struct at_with_predicate_impl<false, I, Predicate, U, V...>
    {
        using type = typename at_with_predicate<I, Predicate, V...>::type;
    };

    template <template <typename T> typename Predicate, typename U,
              typename... V>
    struct at_with_predicate_impl<false, 0, Predicate, U, V...>
    {
        using type = typename at_with_predicate<0, Predicate, V...>::type;
    };

    template <template <typename T> typename Predicate, typename U,
              typename... V>
    struct at_with_predicate_impl<true, 0, Predicate, U, V...>
    {
        using type = U;
    };

    template <std::size_t Index, template <typename T> typename Predicate,
              typename U, typename... V>
    struct at_with_predicate<Index, Predicate, U, V...>
    {
        using type = typename at_with_predicate_impl<Predicate<U>::value, Index,
                                                     Predicate, U, V...>::type;
    };

    template <typename... Types>
    struct is_same_impl : std::true_type
    {
    };

    template <typename T, typename... Types>
    struct is_same_impl<T, Types...>
        : std::conjunction<std::is_same<T, Types>...>
    {
    };

    template <typename T, typename... Types>
    struct count_of_impl
        : std::integral_constant<std::size_t, (0 + ... +
                                               static_cast<std::size_t>(
                                                   std::is_same_v<T, Types>))>
    {
    };

    template <typename T, typename... Types>
    struct more_than_once
        : std::bool_constant<(count_of_impl<T, Types...>::value > 1)>
    {
    };

    template <template <typename> typename Predicate, typename... Types>
    struct count_of_predicate_compliant_impl
        : std::integral_constant<std::size_t, (0 + ... +
                                               static_cast<std::size_t>(
                                                   Predicate<Types>::value))>
    {
    };

    template <std::size_t Index, template <typename> typename Predicate,
              typename... U>
    struct index_of_impl;

    template <std::size_t Index, template <typename T> typename Predicate>
    struct index_of_impl<Index, Predicate>
        : std::integral_constant<std::size_t, Index>
    {
    };

    template <std::size_t Index, template <typename T> typename Predicate,
              typename U, typename... V>
    struct index_of_impl<Index, Predicate, U, V...>
        : std::conditional_t<Predicate<U>::value,
                             std::integral_constant<std::size_t, Index>,
                             index_of_impl<Index + 1, Predicate, V...>>
    {
    };

    template <typename T, typename... U, std::size_t... I>
    constexpr static std::size_t get_first_index_of(
        std::index_sequence<I...>) noexcept
    {
        std::size_t k{};
        (void)(... || (k = std::is_same_v<T, U> * (I + 1)));
        return k ? k - 1 : sizeof...(U);
    }

    template <std::size_t I, template <typename> typename... Predicate>
    struct at;

    template <std::size_t I>
    struct at<I>
    {
        using type = typename at_impl<I, Ts...>::type;
    };

    template <std::size_t I, template <typename> typename Predicate>
    struct at<I, Predicate>
    {
        using type = typename at_with_predicate<I, Predicate, Ts...>::type;
    };

    template <template <typename...> typename R,
              template <typename...> typename Concat,
              template <typename> typename Predicate, std::size_t TypeCount>
    struct list_of_predicate_compliant_impl
    {
        using type = typename Concat<
            std::conditional_t<Predicate<Ts>::value, R<Ts>, R<>>...>::type;
    };

    template <template <typename...> typename R,
              template <typename...> typename Concat,
              template <typename> typename Predicate>
    struct list_of_predicate_compliant_impl<R, Concat, Predicate, 0>
    {
        using type = R<>;
    };
};
}  // namespace details

template <typename... Ts>
struct type_list;

template <auto... Values>
struct value_list;

template <typename... Ts>
struct concatenate;

template <auto... V>
struct concatenate<value_list<V...>>
{
    using type = value_list<V...>;
};

template <auto... V1, auto... V2, typename... Ts>
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

template <typename... Ts>
struct type_list
{
   private:
    using impl = details::type_list<Ts...>;

   public:
    static constexpr std::size_t size = sizeof...(Ts);

    template <std::size_t I, template <typename> typename... Predicate>
    using at = typename impl::template at<I, Predicate...>::type;

    template <template <typename> typename Predicate>
    static constexpr std::size_t index_of =
        impl::template index_of_impl<0, Predicate, Ts...>::value;

    template <typename T>
    static constexpr std::size_t first_index_of_type =
        impl::template get_first_index_of<T, Ts...>(
            std::make_index_sequence<sizeof...(Ts)>{});

    static constexpr bool is_same = impl::template is_same_impl<Ts...>::value;

    template <typename T>
    static constexpr std::size_t count_of =
        impl::template count_of_impl<T, Ts...>::value;

    template <template <typename> typename Predicate>
    static constexpr std::size_t count_of_predicate_compliant =
        impl::template count_of_predicate_compliant_impl<Predicate,
                                                         Ts...>::value;

    template <typename T>
    struct contains : std::bool_constant<static_cast<bool>(count_of<T>)>
    {
    };

    template <typename... Us>
    struct contains<type_list<Us...>>
        : std::bool_constant<std::conjunction_v<contains<Us>...>>
    {
    };

    template <typename T>
    static constexpr bool contains_v = contains<T>::value;

    template <template <typename> typename Predicate>
    static constexpr bool contains_predicate_compliant =
        count_of_predicate_compliant<Predicate>;

    static constexpr bool contains_copies = std::disjunction_v<
        typename impl::template more_than_once<Ts, Ts...>...>;

    template <template <typename T> typename Predicate>
    struct list_of_predicate_compliant
    {
        using type = typename impl::template list_of_predicate_compliant_impl<
            type_list, concatenate, Predicate, sizeof...(Ts)>::type;
    };

    template <template <typename T> typename Predicate>
    using list_of_predicate_compliant_t =
        typename list_of_predicate_compliant<Predicate>::type;
};

template <typename... Ts>
struct is_type_list : std::false_type
{
};

template <typename... Ts>
struct is_type_list<type_list<Ts...>> : std::true_type
{
};

template <typename... Ts>
inline constexpr bool is_type_list_v = is_type_list<Ts...>::value;
}  // namespace utils

#endif /* utils_type_list_h */
