#ifndef utils_stl_traits_h
#define utils_stl_traits_h

#include <map>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include "function_traits.h"

namespace utils
{
CREATE_R_METHOD_CHECKERS(size)

CREATE_MEMBER_TYPE_CHECKERS(iterator)
CREATE_METHOD_CHECKERS(begin)
CREATE_METHOD_CHECKERS(end)

CREATE_MEMBER_TYPE_CHECKERS(const_iterator)
CREATE_METHOD_CHECKERS(cbegin)
CREATE_METHOD_CHECKERS(cend)

CREATE_FREE_FUNCTION_CHECKERS(begin)
CREATE_FREE_FUNCTION_CHECKERS(end)

CREATE_FREE_FUNCTION_CHECKERS(cbegin)
CREATE_FREE_FUNCTION_CHECKERS(cend)

template <typename T>
struct is_std_map : std::false_type
{
};

template <class Key, class T, class Compare, class Allocator>
struct is_std_map<std::map<Key, T, Compare, Allocator>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_map_v = is_std_map<T>::value;

template <typename T>
struct is_std_multimap : std::false_type
{
};

template <class Key, class T, class Compare, class Allocator>
struct is_std_multimap<std::multimap<Key, T, Compare, Allocator>>
    : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_multimap_v = is_std_multimap<T>::value;

template <typename T>
struct is_std_unordered_map : std::false_type
{
};

template <class Key, class T, class Hash, class KeyEqual, class Allocator>
struct is_std_unordered_map<
    std::unordered_map<Key, T, Hash, KeyEqual, Allocator>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_unordered_map_v = is_std_unordered_map<T>::value;

template <typename T>
struct is_std_unordered_multimap : std::false_type
{
};

template <class Key, class T, class Hash, class KeyEqual, class Allocator>
struct is_std_unordered_multimap<
    std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_unordered_multimap_v =
    is_std_unordered_multimap<T>::value;

template <typename T>
struct is_associative_std_container
    : std::disjunction<is_std_map<T>, is_std_multimap<T>,
                       is_std_unordered_map<T>, is_std_unordered_multimap<T>>
{
};

template <typename T>
inline constexpr bool is_associative_std_container_v =
    is_associative_std_container<T>::value;

template <typename T>
struct is_iterable
    : std::disjunction<
          std::conjunction<has_type_iterator<T>, has_invocable_begin<T>,
                           has_invocable_end<T>>,
          std::conjunction<is_begin_invocable<T>, is_end_invocable<T>>>
{
};

template <typename T>
inline constexpr bool is_iterable_v = is_iterable<T>::value;

template <typename T>
struct is_const_iterable
    : std::disjunction<
          std::conjunction<has_type_const_iterator<T>, has_invocable_cbegin<T>,
                           has_invocable_cend<T>>,
          std::conjunction<is_cbegin_invocable<T>, is_cend_invocable<T>>>
{
};

template <typename T>
inline constexpr bool is_const_iterable_v = is_const_iterable<T>::value;

template <typename T>
struct has_size_and_iterable_via_begin_end
    : std::conjunction<
          std::disjunction<has_invocable_r_size_const_noexcept<std::size_t, T>,
                           has_invocable_r_size_const<std::size_t, T>>,
          is_iterable<T>>
{
};

template <typename T>
inline constexpr bool has_size_and_iterable_via_begin_end_v =
    has_size_and_iterable_via_begin_end<T>::value;
}  // namespace utils

#endif /* utils_stl_traits_h */
