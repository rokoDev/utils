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

TEST(UtilsTest, NBitsOccopied1)
{
    constexpr uint8_t kValue{0};
    constexpr std::size_t bitsToSave = utils::nbits_occupied(kValue);
    static_assert(bitsToSave == 0, "Invalid bitsToSave");
}

TEST(UtilsTest, NBitsOccopied2)
{
    constexpr uint8_t kValue{3};
    constexpr std::size_t bitsToSave = utils::nbits_occupied(kValue);
    static_assert(bitsToSave == 2, "Invalid bitsToSave");
}

TEST(UtilsTest, NBitsOccopied3)
{
    constexpr uint8_t kValue{35};
    constexpr std::size_t bitsToSave = utils::nbits_occupied(kValue);
    static_assert(bitsToSave == 6, "Invalid bitsToSave");
}

TEST(UtilsTest, NBitsOccopied4)
{
    constexpr uint8_t kValue{250};
    constexpr std::size_t bitsToSave = utils::nbits_occupied(kValue);
    static_assert(bitsToSave == 8, "Invalid bitsToSave");
}

TEST(UtilsTest, NBitsOccopied5)
{
    constexpr uint32_t kValue{2505};
    constexpr std::size_t bitsToSave = utils::nbits_occupied(kValue);
    static_assert(bitsToSave == 12, "Invalid bitsToSave");
}

TEST(UtilsTest, NBitsOccopied6)
{
    constexpr uint32_t kValue{0b00000000000000000010100111001001};
    constexpr std::size_t bitsToSave = utils::nbits_occupied(kValue);
    static_assert(bitsToSave == 14, "Invalid bitsToSave");
}

TEST(UtilsTest, NBitsOccopied7)
{
    constexpr uint32_t kValue{0b00000001111111111000000000000000};
    constexpr std::size_t bitsToSave = utils::nbits_occupied(kValue);
    static_assert(bitsToSave == 25, "Invalid bitsToSave");
}
