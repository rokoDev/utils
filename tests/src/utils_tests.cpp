#include <gtest/gtest.h>

#include "utils/utils.h"

TEST(UtilsTest, MakeEmptyArray)
{
    constexpr auto emptyArr = utils::make_array<std::uint8_t>();
    static_assert(
        std::is_same_v<decltype(emptyArr), const std::array<std::uint8_t, 0>>,
        "Invalid type.");
}

TEST(UtilsTest, MakeArrayWithEqValues1)
{
    using ElementT = std::uint8_t;
    constexpr std::size_t kNumElements{0};
    constexpr ElementT value{10};

    constexpr auto arr = utils::make_array<ElementT, kNumElements>(value);
    static_assert(
        std::is_same_v<decltype(arr), const std::array<ElementT, kNumElements>>,
        "Invalid type.");
}

TEST(UtilsTest, MakeArrayWithEqValues2)
{
    using ElementT = std::uint8_t;
    constexpr std::size_t kNumElements{5};
    constexpr ElementT value{10};

    constexpr auto arr = utils::make_array<ElementT, kNumElements>(value);
    static_assert(
        std::is_same_v<decltype(arr), const std::array<ElementT, kNumElements>>,
        "Invalid type.");
    for (const auto element: arr)
    {
        ASSERT_EQ(element, value);
    }
}

TEST(UtilsTest, MakeZeroArray)
{
    using ElementT = std::uint16_t;
    constexpr std::size_t kNumElements{10};

    constexpr auto zeroArr = utils::make_zero_array<ElementT, kNumElements>();
    static_assert(std::is_same_v<decltype(zeroArr),
                                 const std::array<ElementT, kNumElements>>,
                  "Invalid type.");
    for (const auto element: zeroArr)
    {
        ASSERT_EQ(element, ElementT{});
    }
}
