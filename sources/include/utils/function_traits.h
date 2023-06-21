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

namespace utils
{
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
    : std::bool_constant<
          std::disjunction_v<std::is_function<T>, is_simple_functor<T>>>
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
