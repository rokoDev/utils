#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <utils/utils.h>

#include <tuple>

struct with_padding
{
    char c;
    int i;
    char d;
};

struct no_padding
{
    int a;
    int b;
    int c;
};

struct nested_with_padding
{
    int a;
    int b;
    int c;
    with_padding wp;
};

struct nested_no_padding
{
    int a;
    no_padding np;
    int b;
    int c;
};

TEST(PaddingCheckerTest, Padding)
{
    static_assert(not utils::has_padding_v<float>);
    static_assert(utils::has_padding_v<with_padding>);
    static_assert(not utils::has_padding_v<no_padding>);
    static_assert(utils::has_padding_v<nested_with_padding>);
    static_assert(not utils::has_padding_v<nested_no_padding>);
    static_assert(
        utils::has_padding_v<
            std::tuple<float, char, int>>);  // detected padding because it is
                                             // not trivially copyable
    static_assert(
        utils::has_padding_v<
            std::tuple<char, char, char>>);  // detected padding because it is
                                             // not trivially copyable
}
