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
    static_assert(utils::has_size_v<std::vector<char>>);
    static_assert(utils::has_size_v<std::set<float>>);
    static_assert(utils::has_size_v<std::unordered_map<int, std::string>>);
    static_assert(utils::has_size_v<std::string>);
    static_assert(utils::has_size_v<std::valarray<int>>);
    static_assert(utils::has_size_v<std::array<int, 10>>);
    static_assert(not utils::has_size_v<std::tuple<int, char, float>>);
    static_assert(not utils::has_size_v<int>);
}

TEST(StlTraits, HasBeginMem)
{
    static_assert(utils::has_begin_mem_v<std::vector<char>>);
    static_assert(utils::has_begin_mem_v<std::set<float>>);
    static_assert(utils::has_begin_mem_v<std::unordered_map<int, std::string>>);
    static_assert(utils::has_begin_mem_v<std::string>);
    static_assert(not utils::has_begin_mem_v<std::valarray<int>>);
    static_assert(utils::has_begin_mem_v<std::array<int, 10>>);
    static_assert(not utils::has_begin_mem_v<std::tuple<int, char, float>>);
    static_assert(not utils::has_begin_mem_v<int>);
}

TEST(StlTraits, HasBeginNonMem)
{
    static_assert(utils::has_begin_non_mem_v<std::vector<char>>);
    static_assert(utils::has_begin_non_mem_v<std::set<float>>);
    static_assert(
        utils::has_begin_non_mem_v<std::unordered_map<int, std::string>>);
    static_assert(utils::has_begin_non_mem_v<std::string>);
    static_assert(utils::has_begin_non_mem_v<std::valarray<int>>);
    static_assert(utils::has_begin_non_mem_v<const std::valarray<int>>);
    static_assert(utils::has_begin_non_mem_v<std::array<int, 10>>);
    static_assert(not utils::has_begin_non_mem_v<std::tuple<int, char, float>>);
    static_assert(not utils::has_begin_non_mem_v<int>);
}

TEST(StlTraits, HasEndMem)
{
    static_assert(utils::has_end_mem_v<std::vector<char>>);
    static_assert(utils::has_end_mem_v<std::set<float>>);
    static_assert(utils::has_end_mem_v<std::unordered_map<int, std::string>>);
    static_assert(utils::has_end_mem_v<std::string>);
    static_assert(not utils::has_end_mem_v<std::valarray<int>>);
    static_assert(utils::has_end_mem_v<std::array<int, 10>>);
    static_assert(not utils::has_end_mem_v<std::tuple<int, char, float>>);
    static_assert(not utils::has_end_mem_v<int>);
}

TEST(StlTraits, HasEndNonMem)
{
    static_assert(utils::has_end_non_mem_v<std::vector<char>>);
    static_assert(utils::has_end_non_mem_v<std::set<float>>);
    static_assert(
        utils::has_end_non_mem_v<std::unordered_map<int, std::string>>);
    static_assert(utils::has_end_non_mem_v<std::string>);
    static_assert(utils::has_end_non_mem_v<std::valarray<int>>);
    static_assert(utils::has_end_non_mem_v<const std::valarray<int>>);
    static_assert(utils::has_end_non_mem_v<std::array<int, 10>>);
    static_assert(not utils::has_end_non_mem_v<std::tuple<int, char, float>>);
    static_assert(not utils::has_end_non_mem_v<int>);
}

TEST(StlTraits, HasConstBeginMem)
{
    static_assert(utils::has_cbegin_mem_v<std::vector<char>>);
    static_assert(utils::has_cbegin_mem_v<std::set<float>>);
    static_assert(
        utils::has_cbegin_mem_v<std::unordered_map<int, std::string>>);
    static_assert(utils::has_cbegin_mem_v<std::string>);
    static_assert(not utils::has_cbegin_mem_v<std::valarray<int>>);
    static_assert(utils::has_cbegin_mem_v<std::array<int, 10>>);
    static_assert(not utils::has_cbegin_mem_v<std::tuple<int, char, float>>);
    static_assert(not utils::has_cbegin_mem_v<int>);
}

TEST(StlTraits, HasConstBeginNonMem)
{
    static_assert(utils::has_cbegin_non_mem_v<std::vector<char>>);
    static_assert(utils::has_cbegin_non_mem_v<std::set<float>>);
    static_assert(
        utils::has_cbegin_non_mem_v<std::unordered_map<int, std::string>>);
    static_assert(utils::has_cbegin_non_mem_v<std::string>);
    static_assert(utils::has_cbegin_non_mem_v<std::array<int, 10>>);
    static_assert(
        not utils::has_cbegin_non_mem_v<std::tuple<int, char, float>>);
    static_assert(not utils::has_cbegin_non_mem_v<int>);
}

TEST(StlTraits, HasConstEndMem)
{
    static_assert(utils::has_cend_mem_v<std::vector<char>>);
    static_assert(utils::has_cend_mem_v<std::set<float>>);
    static_assert(utils::has_cend_mem_v<std::unordered_map<int, std::string>>);
    static_assert(utils::has_cend_mem_v<std::string>);
    static_assert(not utils::has_cend_mem_v<std::valarray<int>>);
    static_assert(utils::has_cend_mem_v<std::array<int, 10>>);
    static_assert(not utils::has_cend_mem_v<std::tuple<int, char, float>>);
    static_assert(not utils::has_cend_mem_v<int>);
}

TEST(StlTraits, HasConstEndNonMem)
{
    static_assert(utils::has_cend_non_mem_v<std::vector<char>>);
    static_assert(utils::has_cend_non_mem_v<std::set<float>>);
    static_assert(
        utils::has_cend_non_mem_v<std::unordered_map<int, std::string>>);
    static_assert(utils::has_cend_non_mem_v<std::string>);
    static_assert(utils::has_cend_non_mem_v<std::array<int, 10>>);
    static_assert(not utils::has_cend_non_mem_v<std::tuple<int, char, float>>);
    static_assert(not utils::has_cend_non_mem_v<int>);
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
