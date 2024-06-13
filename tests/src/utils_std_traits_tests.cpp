#include <gtest/gtest.h>
#include <utils/utils.h>

#include <array>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <valarray>
#include <vector>

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
