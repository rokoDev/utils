#ifndef utils_stl_traits_h
#define utils_stl_traits_h

#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <vector>

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
struct is_std_vector : std::false_type
{
};

template <typename T, typename Allocator>
struct is_std_vector<std::vector<T, Allocator>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_vector_v = is_std_vector<T>::value;

template <typename T>
struct is_std_set : std::false_type
{
};

template <typename Key, typename Compare, typename Allocator>
struct is_std_set<std::set<Key, Compare, Allocator>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_set_v = is_std_set<T>::value;

template <typename T>
struct is_std_multiset : std::false_type
{
};

template <typename Key, typename Compare, typename Allocator>
struct is_std_multiset<std::multiset<Key, Compare, Allocator>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_multiset_v = is_std_multiset<T>::value;

template <typename T>
struct is_std_unordered_set : std::false_type
{
};

template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
struct is_std_unordered_set<std::unordered_set<Key, Hash, KeyEqual, Allocator>>
    : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_unordered_set_v = is_std_unordered_set<T>::value;

template <typename T>
struct is_std_unordered_multiset : std::false_type
{
};

template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
struct is_std_unordered_multiset<
    std::unordered_multiset<Key, Hash, KeyEqual, Allocator>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_unordered_multiset_v =
    is_std_unordered_multiset<T>::value;

template <typename T>
struct is_std_list : std::false_type
{
};

template <typename T, typename Allocator>
struct is_std_list<std::list<T, Allocator>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_list_v = is_std_list<T>::value;

template <typename T>
struct is_std_basic_string : std::false_type
{
};

template <typename CharT, typename Traits, typename Allocator>
struct is_std_basic_string<std::basic_string<CharT, Traits, Allocator>>
    : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_basic_string_v = is_std_basic_string<T>::value;

template <typename T>
struct is_std_valarray : std::false_type
{
};

template <typename T>
struct is_std_valarray<std::valarray<T>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_valarray_v = is_std_valarray<T>::value;

template <typename T>
struct is_std_basic_string_view : std::false_type
{
};

template <typename CharT, typename Traits>
struct is_std_basic_string_view<std::basic_string_view<CharT, Traits>>
    : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_basic_string_view_v =
    is_std_basic_string_view<T>::value;

template <typename T>
struct is_std_forward_list : std::false_type
{
};

template <typename T, typename Allocator>
struct is_std_forward_list<std::forward_list<T, Allocator>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_forward_list_v = is_std_forward_list<T>::value;

template <typename T>
struct is_std_deque : std::false_type
{
};

template <typename T, typename Allocator>
struct is_std_deque<std::deque<T, Allocator>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_deque_v = is_std_deque<T>::value;

template <typename T>
struct is_std_queue : std::false_type
{
};

template <typename T, typename Container>
struct is_std_queue<std::queue<T, Container>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_queue_v = is_std_queue<T>::value;

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

template <typename T>
struct has_contiguous_storage
    : std::disjunction<is_std_vector<T>, is_std_basic_string<T>,
                       is_std_array<T>, is_std_basic_string_view<T>>
{
};

template <typename T>
inline constexpr bool has_contiguous_storage_v =
    has_contiguous_storage<T>::value;
}  // namespace utils

#endif /* utils_stl_traits_h */
