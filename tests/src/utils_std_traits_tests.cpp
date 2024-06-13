#include <gtest/gtest.h>
#include <utils/utils.h>

#include <tuple>

TEST(StdTraits, IsStdContainer)
{
    static_assert(not utils::is_std_array_v<int>);
    static_assert(not utils::is_std_valarray_v<int>);
    static_assert(not utils::is_std_basic_string_v<int>);
    static_assert(not utils::is_std_basic_string_view_v<int>);
    static_assert(not utils::is_std_vector_v<int>);
    static_assert(not utils::is_std_set_v<int>);
    static_assert(not utils::is_std_multiset_v<int>);
    static_assert(not utils::is_std_map_v<int>);
    static_assert(not utils::is_std_multimap_v<int>);
    static_assert(not utils::is_std_unordered_set_v<int>);
    static_assert(not utils::is_std_unordered_multiset_v<int>);
    static_assert(not utils::is_std_unordered_map_v<int>);
    static_assert(not utils::is_std_unordered_multimap_v<int>);
    static_assert(not utils::is_std_list_v<int>);
    static_assert(not utils::is_std_forward_list_v<int>);
    static_assert(not utils::is_std_deque_v<int>);
    static_assert(not utils::is_std_queue_v<int>);

    static_assert(utils::is_std_array_v<std::array<int, 10>>);
    static_assert(utils::is_std_valarray_v<std::valarray<int>>);
    static_assert(utils::is_std_basic_string_v<std::string>);
    static_assert(utils::is_std_basic_string_view_v<std::string_view>);
    static_assert(utils::is_std_vector_v<std::vector<float>>);
    static_assert(utils::is_std_set_v<std::set<int>>);
    static_assert(utils::is_std_multiset_v<std::multiset<int>>);
    static_assert(utils::is_std_map_v<std::map<int, float>>);
    static_assert(utils::is_std_multimap_v<std::multimap<int, float>>);
    static_assert(utils::is_std_unordered_set_v<std::unordered_set<int>>);
    static_assert(
        utils::is_std_unordered_multiset_v<std::unordered_multiset<int>>);
    static_assert(
        utils::is_std_unordered_map_v<std::unordered_map<int, float>>);
    static_assert(utils::is_std_unordered_multimap_v<
                  std::unordered_multimap<int, float>>);
    static_assert(utils::is_std_list_v<std::list<int>>);
    static_assert(utils::is_std_forward_list_v<std::forward_list<int>>);
    static_assert(utils::is_std_deque_v<std::deque<int>>);
    static_assert(utils::is_std_queue_v<std::queue<int>>);
}

TEST(StlTraits, HasSize)
{
    static_assert(utils::has_invocable_r_size_const_noexcept_v<
                  std::vector<char>::size_type, std::vector<char>>);
    static_assert(
        utils::has_invocable_r_size_const_noexcept_v<std::set<float>::size_type,
                                                     std::set<float>>);
    static_assert(utils::has_invocable_r_size_const_noexcept_v<
                  std::unordered_map<int, std::string>::size_type,
                  std::unordered_map<int, std::string>>);
    static_assert(
        utils::has_invocable_r_size_const_noexcept_v<std::string::size_type,
                                                     std::string>);
    static_assert(
        utils::has_invocable_r_size_const_v<std::size_t, std::valarray<int>>);
    static_assert(utils::has_invocable_r_size_const_noexcept_v<
                  std::array<int, 10>::size_type, std::array<int, 10>>);
    static_assert(not utils::has_invocable_r_size_const_noexcept_v<
                  std::size_t, std::tuple<int, char, float>>);
    static_assert(
        not utils::has_invocable_r_size_const_noexcept_v<std::size_t, int>);
}

TEST(StlTraits, HasBeginMem)
{
    static_assert(utils::has_invocable_begin_v<std::vector<char>>);
    static_assert(utils::has_invocable_begin_v<std::set<float>>);
    static_assert(
        utils::has_invocable_begin_v<std::unordered_map<int, std::string>>);
    static_assert(utils::has_invocable_begin_v<std::string>);
    static_assert(not utils::has_invocable_begin_v<std::valarray<int>>);
    static_assert(utils::has_invocable_begin_v<std::array<int, 10>>);
    static_assert(
        not utils::has_invocable_begin_v<std::tuple<int, char, float>>);
    static_assert(not utils::has_invocable_begin_v<int>);
}

TEST(StlTraits, HasBeginNonMem)
{
    static_assert(utils::is_begin_invocable_v<std::vector<char>>);
    static_assert(utils::is_begin_invocable_v<std::set<float>>);
    static_assert(
        utils::is_begin_invocable_v<std::unordered_map<int, std::string>>);
    static_assert(utils::is_begin_invocable_v<std::string>);
    static_assert(utils::is_begin_invocable_v<std::valarray<int>>);
    static_assert(utils::is_begin_invocable_v<const std::valarray<int>>);
    static_assert(utils::is_begin_invocable_v<std::array<int, 10>>);
    static_assert(
        not utils::is_begin_invocable_v<std::tuple<int, char, float>>);
    static_assert(not utils::is_begin_invocable_v<int>);
}

TEST(StlTraits, HasEndMem)
{
    static_assert(utils::has_invocable_end_v<std::vector<char>>);
    static_assert(utils::has_invocable_end_v<std::set<float>>);
    static_assert(
        utils::has_invocable_end_v<std::unordered_map<int, std::string>>);
    static_assert(utils::has_invocable_end_v<std::string>);
    static_assert(not utils::has_invocable_end_v<std::valarray<int>>);
    static_assert(utils::has_invocable_end_v<std::array<int, 10>>);
    static_assert(not utils::has_invocable_end_v<std::tuple<int, char, float>>);
    static_assert(not utils::has_invocable_end_v<int>);
}

TEST(StlTraits, HasEndNonMem)
{
    static_assert(utils::is_end_invocable_v<std::vector<char>>);
    static_assert(utils::is_end_invocable_v<std::set<float>>);
    static_assert(
        utils::is_end_invocable_v<std::unordered_map<int, std::string>>);
    static_assert(utils::is_end_invocable_v<std::string>);
    static_assert(utils::is_end_invocable_v<std::valarray<int>>);
    static_assert(utils::is_end_invocable_v<const std::valarray<int>>);
    static_assert(utils::is_end_invocable_v<std::array<int, 10>>);
    static_assert(not utils::is_end_invocable_v<std::tuple<int, char, float>>);
    static_assert(not utils::is_end_invocable_v<int>);
}

TEST(StlTraits, HasConstBeginMem)
{
    static_assert(utils::has_invocable_cbegin_v<std::vector<char>>);
    static_assert(utils::has_invocable_cbegin_v<std::set<float>>);
    static_assert(
        utils::has_invocable_cbegin_v<std::unordered_map<int, std::string>>);
    static_assert(utils::has_invocable_cbegin_v<std::string>);
    static_assert(not utils::has_invocable_cbegin_v<std::valarray<int>>);
    static_assert(utils::has_invocable_cbegin_v<std::array<int, 10>>);
    static_assert(
        not utils::has_invocable_cbegin_v<std::tuple<int, char, float>>);
    static_assert(not utils::has_invocable_cbegin_v<int>);
}

TEST(StlTraits, HasConstBeginNonMem)
{
    static_assert(utils::is_cbegin_invocable_v<std::vector<char>>);
    static_assert(utils::is_cbegin_invocable_v<std::set<float>>);
    static_assert(
        utils::is_cbegin_invocable_v<std::unordered_map<int, std::string>>);
    static_assert(utils::is_cbegin_invocable_v<std::string>);
    static_assert(utils::is_cbegin_invocable_v<std::array<int, 10>>);
    static_assert(
        not utils::is_cbegin_invocable_v<std::tuple<int, char, float>>);
    static_assert(not utils::is_cbegin_invocable_v<int>);
}

TEST(StlTraits, HasConstEndMem)
{
    static_assert(utils::has_invocable_cend_v<std::vector<char>>);
    static_assert(utils::has_invocable_cend_v<std::set<float>>);
    static_assert(
        utils::has_invocable_cend_v<std::unordered_map<int, std::string>>);
    static_assert(utils::has_invocable_cend_v<std::string>);
    static_assert(not utils::has_invocable_cend_v<std::valarray<int>>);
    static_assert(utils::has_invocable_cend_v<std::array<int, 10>>);
    static_assert(
        not utils::has_invocable_cend_v<std::tuple<int, char, float>>);
    static_assert(not utils::has_invocable_cend_v<int>);
}

TEST(StlTraits, HasConstEndNonMem)
{
    static_assert(utils::is_cend_invocable_v<std::vector<char>>);
    static_assert(utils::is_cend_invocable_v<std::set<float>>);
    static_assert(
        utils::is_cend_invocable_v<std::unordered_map<int, std::string>>);
    static_assert(utils::is_cend_invocable_v<std::string>);
    static_assert(utils::is_cend_invocable_v<std::array<int, 10>>);
    static_assert(not utils::is_cend_invocable_v<std::tuple<int, char, float>>);
    static_assert(not utils::is_cend_invocable_v<int>);
}

TEST(StlTraits, IsIterable)
{
    static_assert(utils::is_iterable_v<std::vector<char>>);
    static_assert(utils::is_iterable_v<std::set<float>>);
    static_assert(utils::is_iterable_v<std::unordered_map<int, std::string>>);
    static_assert(utils::is_iterable_v<std::string>);
    static_assert(utils::is_iterable_v<std::valarray<int>>);
    static_assert(utils::is_iterable_v<const std::valarray<int>>);
    static_assert(utils::is_iterable_v<std::array<int, 10>>);
    static_assert(not utils::is_iterable_v<std::tuple<int, char, float>>);
    static_assert(not utils::is_iterable_v<int>);
}

TEST(StlTraits, IsConstIterable)
{
    static_assert(utils::is_const_iterable_v<std::vector<char>>);
    static_assert(utils::is_const_iterable_v<std::set<float>>);
    static_assert(
        utils::is_const_iterable_v<std::unordered_map<int, std::string>>);
    static_assert(utils::is_const_iterable_v<std::string>);
    static_assert(utils::is_const_iterable_v<std::array<int, 10>>);
    static_assert(not utils::is_const_iterable_v<std::tuple<int, char, float>>);
    static_assert(not utils::is_const_iterable_v<int>);
}

TEST(StlTraits, HasSizeAndIterableViaBeginEnd)
{
    static_assert(
        utils::has_size_and_iterable_via_begin_end_v<std::vector<char>>);
    static_assert(
        utils::has_size_and_iterable_via_begin_end_v<std::set<float>>);
    static_assert(utils::has_size_and_iterable_via_begin_end_v<
                  std::unordered_map<int, std::string>>);
    static_assert(utils::has_size_and_iterable_via_begin_end_v<std::string>);
    static_assert(
        utils::has_size_and_iterable_via_begin_end_v<std::valarray<int>>);
    static_assert(
        utils::has_size_and_iterable_via_begin_end_v<const std::valarray<int>>);
    static_assert(
        utils::has_size_and_iterable_via_begin_end_v<std::array<int, 10>>);
    static_assert(not utils::is_const_iterable_v<std::tuple<int, char, float>>);
    static_assert(not utils::is_const_iterable_v<int>);
}
