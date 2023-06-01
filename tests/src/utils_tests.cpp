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

TEST(UtilsTest, IsPowerOf2)
{
    struct power
    {
        std::size_t value{};
        bool is_power_of_2{};
    };

    constexpr auto testData = utils::make_array(
        power{0, false}, power{1, true}, power{2, true}, power{3, false},
        power{4, true}, power{5, false}, power{6, false}, power{7, false},
        power{8, true}, power{9, false}, power{11, false}, power{12, false},
        power{13, false}, power{14, false}, power{15, false}, power{16, true},
        power{17, false}, power{143390, false}, power{65536, true});

    for (const auto& [value, state_power_of_2]: testData)
    {
        ASSERT_EQ(utils::is_power_of_2(value), state_power_of_2);
    }
}

TEST(UtilsTest, SkipToAlignForBool)
{
    ASSERT_EQ(utils::skip_to_align<bool>(reinterpret_cast<void*>(0)), 0);
    ASSERT_EQ(utils::skip_to_align<bool>(reinterpret_cast<void*>(1)), 0);
    ASSERT_EQ(utils::skip_to_align<bool>(reinterpret_cast<void*>(2)), 0);
}

TEST(UtilsTest, SkipToAlignForChar)
{
    ASSERT_EQ(utils::skip_to_align<char>(reinterpret_cast<void*>(0)), 0);
    ASSERT_EQ(utils::skip_to_align<char>(reinterpret_cast<void*>(1)), 0);
    ASSERT_EQ(utils::skip_to_align<char>(reinterpret_cast<void*>(2)), 0);
}

TEST(UtilsTest, SkipToAlignForInt16)
{
    ASSERT_EQ(utils::skip_to_align<std::int16_t>(reinterpret_cast<void*>(0)),
              0);
    ASSERT_EQ(utils::skip_to_align<std::int16_t>(reinterpret_cast<void*>(1)),
              1);
    ASSERT_EQ(utils::skip_to_align<std::int16_t>(reinterpret_cast<void*>(2)),
              0);
    ASSERT_EQ(utils::skip_to_align<std::int16_t>(reinterpret_cast<void*>(3)),
              1);
    ASSERT_EQ(utils::skip_to_align<std::int16_t>(reinterpret_cast<void*>(4)),
              0);
}

TEST(UtilsTest, SkipToAlignForInt32)
{
    ASSERT_EQ(utils::skip_to_align<std::int32_t>(reinterpret_cast<void*>(0)),
              0);
    ASSERT_EQ(utils::skip_to_align<std::int32_t>(reinterpret_cast<void*>(1)),
              3);
    ASSERT_EQ(utils::skip_to_align<std::int32_t>(reinterpret_cast<void*>(2)),
              2);
    ASSERT_EQ(utils::skip_to_align<std::int32_t>(reinterpret_cast<void*>(3)),
              1);
    ASSERT_EQ(utils::skip_to_align<std::int32_t>(reinterpret_cast<void*>(4)),
              0);
    ASSERT_EQ(utils::skip_to_align<std::int32_t>(reinterpret_cast<void*>(5)),
              3);
}

TEST(UtilsTest, SkipToAlignForInt64)
{
    ASSERT_EQ(utils::skip_to_align<std::int64_t>(reinterpret_cast<void*>(0)),
              0);
    ASSERT_EQ(utils::skip_to_align<std::int64_t>(reinterpret_cast<void*>(1)),
              7);
    ASSERT_EQ(utils::skip_to_align<std::int64_t>(reinterpret_cast<void*>(2)),
              6);
    ASSERT_EQ(utils::skip_to_align<std::int64_t>(reinterpret_cast<void*>(3)),
              5);
    ASSERT_EQ(utils::skip_to_align<std::int64_t>(reinterpret_cast<void*>(4)),
              4);
    ASSERT_EQ(utils::skip_to_align<std::int64_t>(reinterpret_cast<void*>(5)),
              3);
    ASSERT_EQ(utils::skip_to_align<std::int64_t>(reinterpret_cast<void*>(6)),
              2);
    ASSERT_EQ(utils::skip_to_align<std::int64_t>(reinterpret_cast<void*>(7)),
              1);
    ASSERT_EQ(utils::skip_to_align<std::int64_t>(reinterpret_cast<void*>(8)),
              0);
    ASSERT_EQ(utils::skip_to_align<std::int64_t>(reinterpret_cast<void*>(9)),
              7);
    ASSERT_EQ(utils::skip_to_align<std::int64_t>(reinterpret_cast<void*>(10)),
              6);
    ASSERT_EQ(utils::skip_to_align<std::int64_t>(reinterpret_cast<void*>(11)),
              5);
}

TEST(UtilsTest, IsRefWrapper)
{
    static_assert(not utils::is_ref_wrapper_v<int>);
    static_assert(utils::is_ref_wrapper_v<std::reference_wrapper<char>>);

    int val = 5;
    static_assert(utils::is_ref_wrapper_v<decltype(std::cref(val))>);
}

TEST(UtilsTest, UnwrapReference)
{
    using t1 = int*;
    static_assert(std::is_same_v<t1, utils::unwrap_reference_t<t1>>);

    using t2 = std::reference_wrapper<t1>;
    static_assert(std::is_same_v<t1&, utils::unwrap_reference_t<t2>>);

    static_assert(
        std::is_same_v<const int&, utils::unwrap_reference_t<
                                       std::reference_wrapper<const int>>>);
}
