#ifndef utils_function_traits_h
#define utils_function_traits_h

#include <type_traits>

#include "detector.h"
#include "type_list.h"

#define QUALIFIERS                  \
    X(, , , )                       \
    X(const, , , )                  \
    X(, volatile, , )               \
    X(const, volatile, , )          \
    X(, , &, )                      \
    X(const, , &, )                 \
    X(, volatile, &, )              \
    X(const, volatile, &, )         \
    X(, , &&, )                     \
    X(const, , &&, )                \
    X(, volatile, &&, )             \
    X(const, volatile, &&, )        \
    X(, , , noexcept)               \
    X(const, , , noexcept)          \
    X(, volatile, , noexcept)       \
    X(const, volatile, , noexcept)  \
    X(, , &, noexcept)              \
    X(const, , &, noexcept)         \
    X(, volatile, &, noexcept)      \
    X(const, volatile, &, noexcept) \
    X(, , &&, noexcept)             \
    X(const, , &&, noexcept)        \
    X(, volatile, &&, noexcept)     \
    X(const, volatile, &&, noexcept)

#define CREATE_MEMBER_TYPE_CHECKERS(member_type)                            \
    namespace details                                                       \
    {                                                                       \
    template <typename T>                                                   \
    using has_type_##member_type##_helper = typename T::member_type;        \
    }                                                                       \
                                                                            \
    template <typename T>                                                   \
    struct has_type_##member_type                                           \
        : ::utils::is_detected<details::has_type_##member_type##_helper, T> \
    {                                                                       \
    };                                                                      \
                                                                            \
    template <typename T>                                                   \
    inline constexpr bool has_type_##member_type##_v =                      \
        has_type_##member_type<T>::value;                                   \
                                                                            \
    template <typename T, typename Expected>                                \
    struct has_exact_type_##member_type                                     \
        : ::utils::is_detected_exact<                                       \
              Expected, details::has_type_##member_type##_helper, T>        \
    {                                                                       \
    };                                                                      \
                                                                            \
    template <typename T, typename Expected>                                \
    inline constexpr bool has_exact_type_##member_type##_v =                \
        has_exact_type_##member_type<T, Expected>::value;                   \
                                                                            \
    template <typename T, typename To>                                      \
    struct has_convertible_type_##member_type                               \
        : ::utils::is_detected_convertible<                                 \
              To, details::has_type_##member_type##_helper, T>              \
    {                                                                       \
    };                                                                      \
                                                                            \
    template <typename T, typename To>                                      \
    inline constexpr bool has_convertible_type_##member_type##_v =          \
        has_convertible_type_##member_type<T, To>::value;

#define CREATE_MEMBER_DATA_CHECKERS(member_data)                       \
    namespace details                                                  \
    {                                                                  \
    template <typename T>                                              \
    using not_static_##member_data##_type = decltype(&T::member_data); \
                                                                       \
    template <typename T, bool>                                        \
    struct has_data_##member_data : std::false_type                    \
    {                                                                  \
    };                                                                 \
                                                                       \
    template <typename T>                                              \
    struct has_data_##member_data<T, true>                             \
        : ::utils::details::is_member_data_pointer<                    \
              not_static_##member_data##_type<T>>                      \
    {                                                                  \
    };                                                                 \
    }                                                                  \
                                                                       \
    template <typename T>                                              \
    struct has_data_##member_data                                      \
        : details::has_data_##member_data<                             \
              T, ::utils::is_detected_v<                               \
                     details::not_static_##member_data##_type, T>>     \
    {                                                                  \
    };                                                                 \
                                                                       \
    template <typename T>                                              \
    inline constexpr bool has_data_##member_data##_v =                 \
        has_data_##member_data<T>::value;

#define CREATE_STATIC_MEMBER_DATA_CHECKERS(static_member_data)        \
    namespace details                                                 \
    {                                                                 \
    template <typename T>                                             \
    using static_##static_member_data##_type =                        \
        decltype(&T::static_member_data);                             \
                                                                      \
    template <typename T, bool>                                       \
    struct has_static_data_##static_member_data : std::false_type     \
    {                                                                 \
    };                                                                \
                                                                      \
    template <typename T>                                             \
    struct has_static_data_##static_member_data<T, true>              \
        : ::utils::details::is_data_pointer<                          \
              static_##static_member_data##_type<T>>                  \
    {                                                                 \
    };                                                                \
    }                                                                 \
                                                                      \
    template <typename T>                                             \
    struct has_static_data_##static_member_data                       \
        : details::has_static_data_##static_member_data<              \
              T, ::utils::is_detected_v<                              \
                     details::static_##static_member_data##_type, T>> \
    {                                                                 \
    };                                                                \
                                                                      \
    template <typename T>                                             \
    inline constexpr bool has_static_data_##static_member_data##_v =  \
        has_static_data_##static_member_data<T>::value;

#define SYMBOLIZED_QUALIFIERS(member_name)                                     \
    X_IMPL(member_name, , , , , , , , )                                        \
    X_IMPL(member_name, _const, , , , const, , , )                             \
    X_IMPL(member_name, , _volatile, , , , volatile, , )                       \
    X_IMPL(member_name, _const, _volatile, , , const, volatile, , )            \
    X_IMPL(member_name, , , _ref, , , , &, )                                   \
    X_IMPL(member_name, _const, , _ref, , const, , &, )                        \
    X_IMPL(member_name, , _volatile, _ref, , , volatile, &, )                  \
    X_IMPL(member_name, _const, _volatile, _ref, , const, volatile, &, )       \
    X_IMPL(member_name, , , _rref, , , , &&, )                                 \
    X_IMPL(member_name, _const, , _rref, , const, , &&, )                      \
    X_IMPL(member_name, , _volatile, _rref, , , volatile, &&, )                \
    X_IMPL(member_name, _const, _volatile, _rref, , const, volatile, &&, )     \
    X_IMPL(member_name, , , , _noexcept, , , , noexcept)                       \
    X_IMPL(member_name, _const, , , _noexcept, const, , , noexcept)            \
    X_IMPL(member_name, , _volatile, , _noexcept, , volatile, , noexcept)      \
    X_IMPL(member_name, _const, _volatile, , _noexcept, const, volatile, ,     \
           noexcept)                                                           \
    X_IMPL(member_name, , , _ref, _noexcept, , , &, noexcept)                  \
    X_IMPL(member_name, _const, , _ref, _noexcept, const, , &, noexcept)       \
    X_IMPL(member_name, , _volatile, _ref, _noexcept, , volatile, &, noexcept) \
    X_IMPL(member_name, _const, _volatile, _ref, _noexcept, const,             \
           volatile, &, noexcept)                                              \
    X_IMPL(member_name, , , _rref, _noexcept, , , &&, noexcept)                \
    X_IMPL(member_name, _const, , _rref, _noexcept, const, , &&, noexcept)     \
    X_IMPL(member_name, , _volatile, _rref, _noexcept, , volatile, &&,         \
           noexcept)                                                           \
    X_IMPL(member_name, _const, _volatile, _rref, _noexcept, const, volatile,  \
           &&, noexcept)

#define SYMBOLIZED_STATIC_METHOD_QUALIFIERS(member_name) \
    S_IMPL(member_name, , )                              \
    S_IMPL(member_name, _noexcept, noexcept)

namespace utils
{
namespace function_traits
{
#define X_IMPL(member_name, sc, sv, sr, sn, c, v, r, n) \
    template <typename R, typename T, typename... Args> \
    using method_pointer##sc##sv##sr##sn = R (T::*)(Args...) c v r n;
SYMBOLIZED_QUALIFIERS(unused)
#undef X_IMPL

#define S_IMPL(member_name, sn, n)          \
    template <typename R, typename... Args> \
    using static_method_pointer##sn = R (*)(Args...) n;
SYMBOLIZED_STATIC_METHOD_QUALIFIERS(unused)
#undef S_IMPL
}  // namespace function_traits
}  // namespace utils

#define X_IMPL(method_name, sc, sv, sr, sn, c, v, r, n)                       \
    namespace details                                                         \
    {                                                                         \
    template <typename R, typename T, typename... Args>                       \
    using method_##method_name##sc##sv##sr##sn##_ptr_t =                      \
        decltype(::utils::function_traits::method_pointer##sc##sv##sr##sn<    \
                 R, T, Args...>{&T::method_name});                            \
    }                                                                         \
                                                                              \
    template <typename R, typename T, typename... Args>                       \
    struct has_invocable_r_##method_name##sc##sv##sr##sn                      \
        : ::utils::is_detected<                                               \
              details::method_##method_name##sc##sv##sr##sn##_ptr_t, R, T,    \
              Args...>                                                        \
    {                                                                         \
    };                                                                        \
                                                                              \
    template <typename R, typename T, typename... Args>                       \
    inline constexpr bool has_invocable_r_##method_name##sc##sv##sr##sn##_v = \
        has_invocable_r_##method_name##sc##sv##sr##sn<R, T, Args...>::value;

#define CREATE_R_METHOD_CHECKERS(method_name) SYMBOLIZED_QUALIFIERS(method_name)

#define CREATE_METHOD_CHECKERS(method_name)                                 \
    namespace details                                                       \
    {                                                                       \
    template <typename T, typename... Args>                                 \
    using is_invocable_##method_name##_t =                                  \
        decltype(std::declval<T>().method_name(std::declval<Args>()...));   \
                                                                            \
    template <typename T, typename... Args>                                 \
    using is_invocable_##method_name##_noexcept_t =                         \
        std::bool_constant<noexcept(                                        \
            std::declval<T>().method_name(std::declval<Args>()...))>;       \
    }                                                                       \
                                                                            \
    template <typename T, typename... Args>                                 \
    struct has_invocable_##method_name                                      \
        : ::utils::is_detected<details::is_invocable_##method_name##_t, T,  \
                               Args...>                                     \
    {                                                                       \
    };                                                                      \
                                                                            \
    template <typename T, typename... Args>                                 \
    inline constexpr bool has_invocable_##method_name##_v =                 \
        has_invocable_##method_name<T, Args...>::value;                     \
                                                                            \
    template <typename T, typename... Args>                                 \
    struct has_invocable_##method_name##_noexcept                           \
        : ::utils::is_detected_exact<                                       \
              std::true_type,                                               \
              details::is_invocable_##method_name##_noexcept_t, T, Args...> \
    {                                                                       \
    };                                                                      \
                                                                            \
    template <typename T, typename... Args>                                 \
    inline constexpr bool has_invocable_##method_name##_noexcept_v =        \
        has_invocable_##method_name##_noexcept<T, Args...>::value;

#define S_IMPL(method_name, sn, n)                                             \
    namespace details                                                          \
    {                                                                          \
    template <typename T, typename... Args>                                    \
    using static_method_result_##method_name##sn##_t = std::conditional_t<     \
        ::utils::function_traits::details::has_noexcept_v<                     \
            decltype(UTILS_TO_STR(n))>,                                        \
        std::bool_constant<noexcept(T::method_name(std::declval<Args>()...))>, \
        std::true_type>;                                                       \
                                                                               \
    template <typename R, typename T, typename... Args>                        \
    using static_method_r_##method_name##sn##_ptr_t = decltype(                \
        ::utils::function_traits::static_method_pointer##sn<R, Args...>{       \
            &T::method_name});                                                 \
    }                                                                          \
                                                                               \
    template <typename T, typename... Args>                                    \
    struct has_static_invocable_##method_name##sn                              \
        : ::utils::is_detected_convertible<                                    \
              std::true_type,                                                  \
              details::static_method_result_##method_name##sn##_t, T, Args...> \
    {                                                                          \
    };                                                                         \
                                                                               \
    template <typename T, typename... Args>                                    \
    inline constexpr bool has_static_invocable_##method_name##sn##_v =         \
        has_static_invocable_##method_name##sn<T, Args...>::value;             \
                                                                               \
    template <typename R, typename T, typename... Args>                        \
    struct has_static_invocable_r_##method_name##sn                            \
        : ::utils::is_detected<                                                \
              details::static_method_r_##method_name##sn##_ptr_t, R, T,        \
              Args...>                                                         \
    {                                                                          \
    };                                                                         \
                                                                               \
    template <typename R, typename T, typename... Args>                        \
    inline constexpr bool has_static_invocable_r_##method_name##sn##_v =       \
        has_static_invocable_r_##method_name##sn<R, T, Args...>::value;

#define CREATE_STATIC_METHOD_CHECKERS(method_name) \
    SYMBOLIZED_STATIC_METHOD_QUALIFIERS(method_name)

#define CREATE_FREE_FUNCTION_CHECKERS(function_name)                        \
    namespace details                                                       \
    {                                                                       \
    template <typename... Args>                                             \
    using function_name##_return_t =                                        \
        decltype(function_name(std::declval<Args>()...));                   \
                                                                            \
    template <typename... Args>                                             \
    using function_name##_noexcept_return_t =                               \
        std::enable_if_t<noexcept(function_name(std::declval<Args>()...)),  \
                         decltype(function_name(std::declval<Args>()...))>; \
    }                                                                       \
                                                                            \
    template <typename... Args>                                             \
    struct is_##function_name##_invocable                                   \
        : ::utils::is_detected<details::function_name##_return_t, Args...>  \
    {                                                                       \
    };                                                                      \
                                                                            \
    template <typename... Args>                                             \
    inline constexpr bool is_##function_name##_invocable_v =                \
        is_##function_name##_invocable<Args...>::value;                     \
                                                                            \
    template <typename... Args>                                             \
    struct is_##function_name##_noexcept_invocable                          \
        : ::utils::is_detected<details::function_name##_noexcept_return_t,  \
                               Args...>                                     \
    {                                                                       \
    };                                                                      \
                                                                            \
    template <typename... Args>                                             \
    inline constexpr bool is_##function_name##_noexcept_invocable_v =       \
        is_##function_name##_noexcept_invocable<Args...>::value;            \
                                                                            \
    template <typename R, typename... Args>                                 \
    struct is_##function_name##_invocable_r                                 \
        : ::utils::is_detected_exact<R, details::function_name##_return_t,  \
                                     Args...>                               \
    {                                                                       \
    };                                                                      \
                                                                            \
    template <typename R, typename... Args>                                 \
    inline constexpr bool is_##function_name##_invocable_r_v =              \
        is_##function_name##_invocable_r<R, Args...>::value;                \
                                                                            \
    template <typename R, typename... Args>                                 \
    struct is_##function_name##_noexcept_invocable_r                        \
        : ::utils::is_detected_exact<                                       \
              R, details::function_name##_noexcept_return_t, Args...>       \
    {                                                                       \
    };                                                                      \
                                                                            \
    template <typename R, typename... Args>                                 \
    inline constexpr bool is_##function_name##_noexcept_invocable_r_v =     \
        is_##function_name##_noexcept_invocable_r<R, Args...>::value;

namespace utils
{
namespace details
{
template <typename T>
struct is_data_pointer : std::false_type
{
};

template <typename MemT>
struct is_data_pointer<MemT *> : std::negation<std::is_function<MemT>>
{
};

template <typename T>
struct is_member_data_pointer : std::false_type
{
};

template <typename MemT, typename T>
struct is_member_data_pointer<MemT T::*> : std::negation<std::is_function<MemT>>
{
};
}  // namespace details
namespace function_traits
{
namespace details
{
template <typename T>
using is_simple_functor_impl = decltype(&T::operator());

template <typename T>
struct member_signature
{
};

template <class T, class U>
struct member_signature<U T::*>
{
    using type = U;
};

template <typename T>
struct qualifier_size;

template <std::size_t N>
struct qualifier_size<const char (&)[N]>
    : std::integral_constant<std::size_t, N>
{
};

template <typename T>
constexpr inline std::size_t qualifier_size_v = qualifier_size<T>::value;

template <typename T>
struct has_const : std::false_type
{
};

template <std::size_t N>
struct has_const<const char (&)[N]>
    : std::bool_constant<N == qualifier_size_v<decltype("const")>>
{
};

template <typename T>
constexpr inline bool has_const_v = has_const<T>::value;

template <typename T>
struct has_volatile : std::false_type
{
};

template <std::size_t N>
struct has_volatile<const char (&)[N]>
    : std::bool_constant<N == qualifier_size_v<decltype("volatile")>>
{
};

template <typename T>
constexpr inline bool has_volatile_v = has_volatile<T>::value;

template <typename T>
struct has_lvalue_reference : std::false_type
{
};

template <std::size_t N>
struct has_lvalue_reference<const char (&)[N]>
    : std::bool_constant<N == qualifier_size_v<decltype("&")>>
{
};

template <typename T>
constexpr inline bool has_lvalue_reference_v = has_lvalue_reference<T>::value;

template <typename T>
struct has_rvalue_reference : std::false_type
{
};

template <std::size_t N>
struct has_rvalue_reference<const char (&)[N]>
    : std::bool_constant<N == qualifier_size_v<decltype("&&")>>
{
};

template <typename T>
constexpr inline bool has_rvalue_reference_v = has_rvalue_reference<T>::value;

template <typename T>
struct has_noexcept : std::false_type
{
};

template <std::size_t N>
struct has_noexcept<const char (&)[N]>
    : std::bool_constant<N == qualifier_size_v<decltype("noexcept")>>
{
};

template <typename T>
constexpr inline bool has_noexcept_v = has_noexcept<T>::value;

template <typename T, bool U>
struct signature_impl;

template <typename T>
struct signature_impl<T, true>
{
    using type = T;
};

template <typename T>
struct signature_impl<T, false>
{
    using type = typename member_signature<decltype(&T::operator())>::type;
};
}  // namespace details

template <typename T>
using is_simple_functor = is_detected<details::is_simple_functor_impl, T>;

template <typename T>
constexpr inline bool is_simple_functor_v = is_simple_functor<T>::value;

template <typename T>
struct member_signature
{
    using type = typename details::member_signature<T>::type;
};

template <typename T>
using member_signature_t = typename member_signature<T>::type;

template <typename T>
struct functor_signature
{
    using type = member_signature_t<decltype(&T::operator())>;
};

template <typename T>
using functor_signature_t = typename functor_signature<T>::type;

template <typename T>
struct is_invocable
    : std::disjunction<std::is_function<T>, is_simple_functor<T>>
{
};

template <typename T, typename U>
struct is_invocable<U T::*> : std::is_function<U>
{
};

template <typename R, typename... Args>
struct is_invocable<R (*)(Args...)> : std::true_type
{
};

template <typename R, typename... Args>
struct is_invocable<R (*)(Args...) noexcept> : std::true_type
{
};

template <typename T>
constexpr inline bool is_invocable_v = is_invocable<T>::value;

template <typename T>
struct signature
{
    using type =
        typename details::signature_impl<T, std::is_function_v<T>>::type;
};

template <typename T>
using signature_t = typename signature<T>::type;

template <typename T>
struct remove_const_qualifier;

#define X(c, v, r, n)                                 \
    template <typename R, typename... Args>           \
    struct remove_const_qualifier<R(Args...) c v r n> \
    {                                                 \
        using type = R(Args...) v r n;                \
    };
QUALIFIERS
#undef X

template <typename T>
struct remove_volatile_qualifier;

#define X(c, v, r, n)                                    \
    template <typename R, typename... Args>              \
    struct remove_volatile_qualifier<R(Args...) c v r n> \
    {                                                    \
        using type = R(Args...) c r n;                   \
    };
QUALIFIERS
#undef X

template <typename T>
struct remove_reference_qualifier;

#define X(c, v, r, n)                                     \
    template <typename R, typename... Args>               \
    struct remove_reference_qualifier<R(Args...) c v r n> \
    {                                                     \
        using type = R(Args...) c v n;                    \
    };
QUALIFIERS
#undef X

template <typename T>
struct remove_noexcept_qualifier;

#define X(c, v, r, n)                                    \
    template <typename R, typename... Args>              \
    struct remove_noexcept_qualifier<R(Args...) c v r n> \
    {                                                    \
        using type = R(Args...) c v r;                   \
    };
QUALIFIERS
#undef X

template <typename T>
struct remove_all_qualifiers;

#define X(c, v, r, n)                                \
    template <typename R, typename... Args>          \
    struct remove_all_qualifiers<R(Args...) c v r n> \
    {                                                \
        using type = R(Args...);                     \
    };
QUALIFIERS
#undef X

template <typename T>
using remove_all_qualifiers_t = typename remove_all_qualifiers<T>::type;

template <typename T>
struct has_const_qualifier;

#define X(c, v, r, n)                                   \
    template <typename R, typename... Args>             \
    struct has_const_qualifier<R(Args...) c v r n>      \
        : details::has_const<decltype(UTILS_TO_STR(c))> \
    {                                                   \
    };
QUALIFIERS
#undef X

template <typename T>
inline constexpr bool has_const_qualifier_v = has_const_qualifier<T>::value;

template <typename T>
struct has_volatile_qualifier;

#define X(c, v, r, n)                                      \
    template <typename R, typename... Args>                \
    struct has_volatile_qualifier<R(Args...) c v r n>      \
        : details::has_volatile<decltype(UTILS_TO_STR(v))> \
    {                                                      \
    };
QUALIFIERS
#undef X

template <typename T>
inline constexpr bool has_volatile_qualifier_v =
    has_volatile_qualifier<T>::value;

template <typename T>
struct has_lvalue_reference_qualifier;

#define X(c, v, r, n)                                              \
    template <typename R, typename... Args>                        \
    struct has_lvalue_reference_qualifier<R(Args...) c v r n>      \
        : details::has_lvalue_reference<decltype(UTILS_TO_STR(r))> \
    {                                                              \
    };
QUALIFIERS
#undef X

template <typename T>
inline constexpr bool has_lvalue_reference_qualifier_v =
    has_lvalue_reference_qualifier<T>::value;

template <typename T>
struct has_rvalue_reference_qualifier;

#define X(c, v, r, n)                                              \
    template <typename R, typename... Args>                        \
    struct has_rvalue_reference_qualifier<R(Args...) c v r n>      \
        : details::has_rvalue_reference<decltype(UTILS_TO_STR(r))> \
    {                                                              \
    };
QUALIFIERS
#undef X

template <typename T>
inline constexpr bool has_rvalue_reference_qualifier_v =
    has_rvalue_reference_qualifier<T>::value;

template <typename T>
struct has_noexcept_qualifier;

#define X(c, v, r, n)                                      \
    template <typename R, typename... Args>                \
    struct has_noexcept_qualifier<R(Args...) c v r n>      \
        : details::has_noexcept<decltype(UTILS_TO_STR(n))> \
    {                                                      \
    };
QUALIFIERS
#undef X

template <typename T>
inline constexpr bool has_noexcept_qualifier_v =
    has_noexcept_qualifier<T>::value;

template <typename T>
struct return_type;

#define X(c, v, r, n)                       \
    template <typename R, typename... Args> \
    struct return_type<R(Args...) c v r n>  \
    {                                       \
        using type = R;                     \
    };
QUALIFIERS
#undef X

template <typename T>
using return_type_t = typename return_type<T>::type;

template <typename T>
struct params_list;

#define X(c, v, r, n)                       \
    template <typename R, typename... Args> \
    struct params_list<R(Args...) c v r n>  \
    {                                       \
        using type = type_list<Args...>;    \
    };
QUALIFIERS
#undef X

template <typename T>
using params_list_t = typename params_list<T>::type;
}  // namespace function_traits
}  // namespace utils

#endif /* utils_function_traits_h */
