#ifndef utils_stl_traits_h
#define utils_stl_traits_h

#include <type_traits>
#include <utility>

namespace utils
{
namespace details
{
template <typename T>
using has_size_impl = decltype(std::declval<T>().size());

template <typename T>
using has_begin_mem_impl = decltype(std::declval<T>().begin());

template <typename T>
using has_end_mem_impl = decltype(std::declval<T>().end());

template <typename T>
using has_cbegin_mem_impl = decltype(std::declval<T>().cbegin());

template <typename T>
using has_cend_mem_impl = decltype(std::declval<T>().cend());

template <typename T>
using has_begin_non_mem_impl = decltype(begin(std::declval<T>()));

template <typename T>
using has_end_non_mem_impl = decltype(end(std::declval<T>()));

template <typename T>
using has_cbegin_non_mem_impl = decltype(cbegin(std::declval<T>()));

template <typename T>
using has_cend_non_mem_impl = decltype(cend(std::declval<T>()));

template <typename T, bool HasIterator>
struct has_begin_mem : std::false_type
{
};

template <typename T>
struct has_begin_mem<T, true>
    : is_detected_exact<typename T::iterator, has_begin_mem_impl, T>
{
};

template <typename T, bool HasIterator>
struct has_end_mem : std::false_type
{
};

template <typename T>
struct has_end_mem<T, true>
    : is_detected_exact<typename T::iterator, has_end_mem_impl, T>
{
};

template <typename T, bool HasConstIterator>
struct has_cbegin_mem : std::false_type
{
};

template <typename T>
struct has_cbegin_mem<T, true>
    : is_detected_exact<typename T::const_iterator, has_cbegin_mem_impl, T>
{
};

template <typename T, bool HasConstIterator>
struct has_cend_mem : std::false_type
{
};

template <typename T>
struct has_cend_mem<T, true>
    : is_detected_exact<typename T::const_iterator, has_cend_mem_impl, T>
{
};
}  // namespace details

template <typename T>
struct has_size
    : utils::is_detected_exact<std::size_t, details::has_size_impl, T>
{
};

template <typename T>
inline constexpr bool has_size_v = has_size<T>::value;

template <typename T>
struct has_iterator
{
    template <typename U>
    static std::true_type test(typename U::iterator *);

    template <typename U>
    static std::false_type test(...);

    static constexpr bool value = decltype(test<T>(nullptr))::value;
};

template <typename T>
inline constexpr bool has_iterator_v = has_iterator<T>::value;

template <typename T>
struct has_const_iterator
{
    template <typename U>
    static std::true_type test(typename U::const_iterator *);

    template <typename U>
    static std::false_type test(...);

    static constexpr bool value = decltype(test<T>(nullptr))::value;
};

template <typename T>
inline constexpr bool has_const_iterator_v = has_const_iterator<T>::value;

template <typename T>
struct has_begin_mem : details::has_begin_mem<T, has_iterator_v<T>>
{
};

template <typename T>
inline constexpr bool has_begin_mem_v = has_begin_mem<T>::value;

template <typename T>
struct has_end_mem : details::has_end_mem<T, has_iterator_v<T>>
{
};

template <typename T>
inline constexpr bool has_end_mem_v = has_end_mem<T>::value;

template <typename T>
struct has_cbegin_mem : details::has_cbegin_mem<T, has_const_iterator_v<T>>
{
};

template <typename T>
inline constexpr bool has_cbegin_mem_v = has_cbegin_mem<T>::value;

template <typename T>
struct has_cend_mem : details::has_cend_mem<T, has_const_iterator_v<T>>
{
};

template <typename T>
inline constexpr bool has_cend_mem_v = has_cend_mem<T>::value;

template <typename T>
struct has_begin_non_mem : is_detected<details::has_begin_non_mem_impl, T>
{
};

template <typename T>
inline constexpr bool has_begin_non_mem_v = has_begin_non_mem<T>::value;

template <typename T>
struct has_end_non_mem : is_detected<details::has_end_non_mem_impl, T>
{
};

template <typename T>
inline constexpr bool has_end_non_mem_v = has_end_non_mem<T>::value;

template <typename T>
struct has_cbegin_non_mem : is_detected<details::has_cbegin_non_mem_impl, T>
{
};

template <typename T>
inline constexpr bool has_cbegin_non_mem_v = has_cbegin_non_mem<T>::value;

template <typename T>
struct has_cend_non_mem : is_detected<details::has_cend_non_mem_impl, T>
{
};

template <typename T>
inline constexpr bool has_cend_non_mem_v = has_cend_non_mem<T>::value;

template <typename T>
struct is_iterable
    : std::disjunction<
          std::conjunction<has_begin_mem<T>, has_end_mem<T>>,
          std::conjunction<has_begin_non_mem<T>, has_end_non_mem<T>>>
{
};

template <typename T>
inline constexpr bool is_iterable_v = is_iterable<T>::value;

template <typename T>
struct is_const_iterable
    : std::disjunction<
          std::conjunction<has_cbegin_mem<T>, has_cend_mem<T>>,
          std::conjunction<has_cbegin_non_mem<T>, has_cend_non_mem<T>>>
{
};

template <typename T>
inline constexpr bool is_const_iterable_v = is_const_iterable<T>::value;

template <typename T>
struct has_size_and_iterable_via_begin_end
    : std::conjunction<has_size<T>, is_iterable<T>>
{
};

template <typename T>
inline constexpr bool has_size_and_iterable_via_begin_end_v =
    has_size_and_iterable_via_begin_end<T>::value;
}  // namespace utils

#endif /* utils_stl_traits_h */
