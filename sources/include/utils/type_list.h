#ifndef utils_type_list_h
#define utils_type_list_h

#include <cstddef>
#include <type_traits>
#include <utility>

namespace utils
{
template <typename... Ts>
struct type_list;

namespace details
{
template <typename... Ts>
struct type_list_impl
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

    template <std::size_t Index, template <typename...> typename Predicate,
              typename TypeList, typename... RestPredArgs>
    struct index_of_impl;

    template <std::size_t Index, template <typename... T> typename Predicate,
              typename... RestPredArgs>
    struct index_of_impl<Index, Predicate, ::utils::type_list<>,
                         RestPredArgs...>
        : std::integral_constant<std::size_t, Index>
    {
    };

    template <std::size_t Index, template <typename... T> typename Predicate,
              typename U, typename... V, typename... RestPredArgs>
    struct index_of_impl<Index, Predicate, ::utils::type_list<U, V...>,
                         RestPredArgs...>
        : std::conditional_t<
              Predicate<U, RestPredArgs...>::value,
              std::integral_constant<std::size_t, Index>,
              index_of_impl<Index + 1, Predicate, ::utils::type_list<V...>,
                            RestPredArgs...>>
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
              template <typename...> typename Predicate, std::size_t TypeCount,
              typename... RestPredArgs>
    struct list_of_predicate_compliant_impl
    {
        using type = typename Concat<std::conditional_t<
            Predicate<Ts, RestPredArgs...>::value, R<Ts>, R<>>...>::type;
    };

    template <template <typename...> typename R,
              template <typename...> typename Concat,
              template <typename...> typename Predicate,
              typename... RestPredArgs>
    struct list_of_predicate_compliant_impl<R, Concat, Predicate, 0,
                                            RestPredArgs...>
    {
        using type = R<>;
    };
};
}  // namespace details

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

template <typename... Ts>
struct type_list
{
   private:
    using impl = details::type_list_impl<Ts...>;

   public:
    static constexpr std::size_t size = sizeof...(Ts);

    template <std::size_t I, template <typename> typename... Predicate>
    using at = typename impl::template at<I, Predicate...>::type;

    template <template <typename...> typename Predicate,
              typename... RestPredArgs>
    static constexpr std::size_t index_of =
        impl::template index_of_impl<0, Predicate, type_list<Ts...>,
                                     RestPredArgs...>::value;

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

    template <template <typename... T> typename Predicate,
              typename... RestPredArgs>
    struct list_of_predicate_compliant
    {
        using type = typename impl::template list_of_predicate_compliant_impl<
            type_list, concatenate, Predicate, sizeof...(Ts),
            RestPredArgs...>::type;
    };

    template <template <typename... T> typename Predicate,
              typename... RestPredArgs>
    using list_of_predicate_compliant_t =
        typename list_of_predicate_compliant<Predicate, RestPredArgs...>::type;
};

namespace details
{
template <typename T, typename... Rest, std::size_t... I>
constexpr std::size_t min_sizeof_index_impl(std::index_sequence<I...>) noexcept
{
    std::size_t index = 0;
    [[maybe_unused]] std::size_t min_sizeof = sizeof(T);

    (...,
     [&index, &min_sizeof]()
     {
         if (min_sizeof > sizeof(Rest))
         {
             index = I + 1;
             min_sizeof = sizeof(Rest);
         }
     }());

    return index;
}

template <typename T, typename... Rest, std::size_t... I>
constexpr std::size_t max_sizeof_index_impl(std::index_sequence<I...>) noexcept
{
    std::size_t index = 0;
    [[maybe_unused]] std::size_t max_sizeof = sizeof(T);

    (...,
     [&index, &max_sizeof]()
     {
         if (max_sizeof < sizeof(Rest))
         {
             index = I + 1;
             max_sizeof = sizeof(Rest);
         }
     }());

    return index;
}

template <typename TypeList, std::size_t First, std::size_t... I>
struct sublist_impl;

template <typename... Ts, std::size_t First, std::size_t... I>
struct sublist_impl<utils::type_list<Ts...>, First, I...>
{
    using type = utils::type_list<
        typename utils::type_list<Ts...>::template at<First + I>...>;
};

template <typename TypeList, std::size_t First, std::size_t... I>
constexpr sublist_impl<TypeList, First, I...> sublist_helper(
    std::index_sequence<I...>) noexcept;
}  // namespace details

template <typename TypeList>
struct min_sizeof_index;

template <typename T, typename... Rest>
struct min_sizeof_index<type_list<T, Rest...>>
    : std::integral_constant<std::size_t,
                             details::min_sizeof_index_impl<T, Rest...>(
                                 std::make_index_sequence<sizeof...(Rest)>{})>
{
};

template <typename TypeList>
inline constexpr std::size_t min_sizeof_index_v =
    min_sizeof_index<TypeList>::value;

template <typename TypeList>
struct max_sizeof_index;

template <typename T, typename... Rest>
struct max_sizeof_index<type_list<T, Rest...>>
    : std::integral_constant<std::size_t,
                             details::max_sizeof_index_impl<T, Rest...>(
                                 std::make_index_sequence<sizeof...(Rest)>{})>
{
};

template <typename TypeList>
inline constexpr std::size_t max_sizeof_index_v =
    max_sizeof_index<TypeList>::value;

template <typename TypeList, std::size_t First, std::size_t Count>
struct sublist;

template <>
struct sublist<type_list<>, 0, 0>
{
    using type = type_list<>;
};

template <typename... Ts, std::size_t First>
struct sublist<type_list<Ts...>, First, 0>
{
    using type = std::enable_if_t < First<sizeof...(Ts), type_list<>>;
};

template <typename... Ts, std::size_t First, std::size_t Count>
struct sublist<type_list<Ts...>, First, Count>
{
    using type = std::enable_if_t<
        First + Count <= sizeof...(Ts),
        typename decltype(details::sublist_helper<type_list<Ts...>, First>(
            std::make_index_sequence<Count>{}))::type>;
};

template <typename TypeList, std::size_t First, std::size_t Count>
using sublist_t = typename sublist<TypeList, First, Count>::type;

namespace details
{
template <typename TypeList, std::size_t I>
struct exclude_at_impl;

template <typename T, typename... Rest>
struct exclude_at_impl<utils::type_list<T, Rest...>, 0>
{
    using type = utils::type_list<Rest...>;
};

template <typename T1, typename T2, typename... Rest>
struct exclude_at_impl<utils::type_list<T1, T2, Rest...>, sizeof...(Rest) + 1>
{
    using type =
        sublist_t<utils::type_list<T1, T2, Rest...>, 0, sizeof...(Rest) + 1>;
};

template <typename... Ts, std::size_t I>
struct exclude_at_impl<utils::type_list<Ts...>, I>
{
    using type = concatenate_t<
        sublist_t<utils::type_list<Ts...>, 0, I>,
        sublist_t<utils::type_list<Ts...>, I + 1, sizeof...(Ts) - I - 1>>;
};

template <typename TypeList, std::size_t I, bool>
struct exclude_at_helper;

template <typename... Ts, std::size_t I>
struct exclude_at_helper<utils::type_list<Ts...>, I, true>
{
    using type = typename exclude_at_impl<utils::type_list<Ts...>, I>::type;
};
}  // namespace details

template <typename TypeList, std::size_t I>
struct exclude_at
{
    using type =
        typename details::exclude_at_helper<TypeList, I,
                                            (I < TypeList::size)>::type;
};

template <typename TypeList, std::size_t I>
using exclude_at_t = typename exclude_at<TypeList, I>::type;

template <typename TypeList>
struct sort_sizeof_ascending;

template <>
struct sort_sizeof_ascending<type_list<>>
{
    using type = type_list<>;
};

template <typename T>
struct sort_sizeof_ascending<type_list<T>>
{
    using type = type_list<T>;
};

template <typename... Ts>
struct sort_sizeof_ascending<type_list<Ts...>>
{
    using type = concatenate_t<
        type_list<typename type_list<Ts...>::template at<
            min_sizeof_index_v<type_list<Ts...>>>>,
        typename sort_sizeof_ascending<exclude_at_t<
            type_list<Ts...>, min_sizeof_index_v<type_list<Ts...>>>>::type>;
};

template <typename TypeList>
using sort_sizeof_ascending_t = typename sort_sizeof_ascending<TypeList>::type;

template <typename TypeList>
struct sort_sizeof_descending;

template <>
struct sort_sizeof_descending<type_list<>>
{
    using type = type_list<>;
};

template <typename T>
struct sort_sizeof_descending<type_list<T>>
{
    using type = type_list<T>;
};

template <typename... Ts>
struct sort_sizeof_descending<type_list<Ts...>>
{
    using type = concatenate_t<
        type_list<typename type_list<Ts...>::template at<
            max_sizeof_index_v<type_list<Ts...>>>>,
        typename sort_sizeof_descending<exclude_at_t<
            type_list<Ts...>, max_sizeof_index_v<type_list<Ts...>>>>::type>;
};

template <typename TypeList>
using sort_sizeof_descending_t =
    typename sort_sizeof_descending<TypeList>::type;

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
