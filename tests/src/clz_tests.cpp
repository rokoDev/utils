#include <gtest/gtest.h>
#include <utils/utils.h>

TEST(UtilsClzTest, Clz8)
{
    using utils::clz;
    using int_t = std::uint8_t;

    {
        const int_t value{0b10000101};
        static_assert(clz(value) == 0);
        ASSERT_EQ(clz(value), 0);
    }

    {
        const int_t value{0b00100100};
        static_assert(clz(value) == 2);
        ASSERT_EQ(clz(value), 2);
    }

    {
        const int_t value{0b00011000};
        static_assert(clz(value) == 3);
        ASSERT_EQ(clz(value), 3);
    }

    {
        const int_t value{0b00000001};
        static_assert(clz(value) == 7);
        ASSERT_EQ(clz(value), 7);
    }
}

TEST(UtilsClzTest, Clz16)
{
    using utils::clz;
    using int_t = std::uint16_t;

    {
        const int_t value{0b1000010100000101};
        static_assert(clz(value) == 0);
        ASSERT_EQ(clz(value), 0);
    }

    {
        const int_t value{0b0010010000000100};
        static_assert(clz(value) == 2);
        ASSERT_EQ(clz(value), 2);
    }

    {
        const int_t value{0b0001100010001000};
        static_assert(clz(value) == 3);
        ASSERT_EQ(clz(value), 3);
    }

    {
        const int_t value{0b0000000110000000};
        static_assert(clz(value) == 7);
        ASSERT_EQ(clz(value), 7);
    }

    {
        const int_t value{0b0000000000000001};
        static_assert(clz(value) == 15);
        ASSERT_EQ(clz(value), 15);
    }
}

TEST(UtilsClzTest, Clz32)
{
    using utils::clz;
    using int_t = std::uint32_t;

    {
        const int_t value{0b10000101000001010000010100000101};
        static_assert(clz(value) == 0);
        ASSERT_EQ(clz(value), 0);
    }

    {
        const int_t value{0b00100100000001000000010000000100};
        static_assert(clz(value) == 2);
        ASSERT_EQ(clz(value), 2);
    }

    {
        const int_t value{0b00011000100010001000100010001000};
        static_assert(clz(value) == 3);
        ASSERT_EQ(clz(value), 3);
    }

    {
        const int_t value{0b00000001100000001000000010000000};
        static_assert(clz(value) == 7);
        ASSERT_EQ(clz(value), 7);
    }

    {
        const int_t value{0b00000000000000011000000000000000};
        static_assert(clz(value) == 15);
        ASSERT_EQ(clz(value), 15);
    }

    {
        const int_t value{0b00000000000000000000000000000001};
        static_assert(clz(value) == 31);
        ASSERT_EQ(clz(value), 31);
    }
}

TEST(UtilsClzTest, Clz64)
{
    using utils::clz;
    using int_t = std::uint64_t;

    {
        const int_t value{
            0b1000010100000101000001010000010100000101000001010000010100000101};
        static_assert(clz(value) == 0);
        ASSERT_EQ(clz(value), 0);
    }

    {
        const int_t value{
            0b0010010000000100000001000000010000000100000001000000010000000100};
        static_assert(clz(value) == 2);
        ASSERT_EQ(clz(value), 2);
    }

    {
        const int_t value{
            0b0001100010001000100010001000100010001000100010001000100010001000};
        static_assert(clz(value) == 3);
        ASSERT_EQ(clz(value), 3);
    }

    {
        const int_t value{
            0b0000000110000000100000001000000010000000100000001000000010000000};
        static_assert(clz(value) == 7);
        ASSERT_EQ(clz(value), 7);
    }

    {
        const int_t value{
            0b0000000000000001100000000000000010000000000000001000000000000000};
        static_assert(clz(value) == 15);
        ASSERT_EQ(clz(value), 15);
    }

    {
        const int_t value{
            0b0000000000000000000000000000000110000000000000000000000000000000};
        static_assert(clz(value) == 31);
        ASSERT_EQ(clz(value), 31);
    }

    {
        const int_t value{
            0b0000000000000000000000000000000000000000000000000000000000000001};
        static_assert(clz(value) == 63);
        ASSERT_EQ(clz(value), 63);
    }

    {
        const int_t value{
            0b0000000000000000000000000000000000000000000000000000000100000000};
        static_assert(clz(value) == 55);
        ASSERT_EQ(clz(value), 55);
    }
}

TEST(UtilsClzTest, CraftedClz8)
{
    using utils::details::crafted_clz;
    using int_t = std::uint8_t;

    {
        const int_t value{0b10000101};
        static_assert(crafted_clz(value) == 0);
    }

    {
        const int_t value{0b00100100};
        static_assert(crafted_clz(value) == 2);
    }

    {
        const int_t value{0b00011000};
        static_assert(crafted_clz(value) == 3);
    }

    {
        const int_t value{0b00000001};
        static_assert(crafted_clz(value) == 7);
    }
}

TEST(UtilsClzTest, CraftedClz16)
{
    using utils::details::crafted_clz;
    using int_t = std::uint16_t;

    {
        const int_t value{0b1000010100000101};
        static_assert(crafted_clz(value) == 0);
    }

    {
        const int_t value{0b0010010000000100};
        static_assert(crafted_clz(value) == 2);
    }

    {
        const int_t value{0b0001100010001000};
        static_assert(crafted_clz(value) == 3);
    }

    {
        const int_t value{0b0000000110000000};
        static_assert(crafted_clz(value) == 7);
    }

    {
        const int_t value{0b0000000000000001};
        static_assert(crafted_clz(value) == 15);
    }
}

TEST(UtilsClzTest, CraftedClz32)
{
    using utils::details::crafted_clz;
    using int_t = std::uint32_t;

    {
        const int_t value{0b10000101000001010000010100000101};
        static_assert(crafted_clz(value) == 0);
    }

    {
        const int_t value{0b00100100000001000000010000000100};
        static_assert(crafted_clz(value) == 2);
    }

    {
        const int_t value{0b00011000100010001000100010001000};
        static_assert(crafted_clz(value) == 3);
    }

    {
        const int_t value{0b00000001100000001000000010000000};
        static_assert(crafted_clz(value) == 7);
    }

    {
        const int_t value{0b00000000000000011000000000000000};
        static_assert(crafted_clz(value) == 15);
    }

    {
        const int_t value{0b00000000000000000000000000000001};
        static_assert(crafted_clz(value) == 31);
    }
}

TEST(UtilsClzTest, CraftedClz64)
{
    using utils::details::crafted_clz;
    using int_t = std::uint64_t;

    {
        const int_t value{
            0b1000010100000101000001010000010100000101000001010000010100000101};
        static_assert(crafted_clz(value) == 0);
    }

    {
        const int_t value{
            0b0010010000000100000001000000010000000100000001000000010000000100};
        static_assert(crafted_clz(value) == 2);
    }

    {
        const int_t value{
            0b0001100010001000100010001000100010001000100010001000100010001000};
        static_assert(crafted_clz(value) == 3);
    }

    {
        const int_t value{
            0b0000000110000000100000001000000010000000100000001000000010000000};
        static_assert(crafted_clz(value) == 7);
    }

    {
        const int_t value{
            0b0000000000000001100000000000000010000000000000001000000000000000};
        static_assert(crafted_clz(value) == 15);
    }

    {
        const int_t value{
            0b0000000000000000000000000000000110000000000000000000000000000000};
        static_assert(crafted_clz(value) == 31);
    }

    {
        const int_t value{
            0b0000000000000000000000000000000000000000000000000000000000000001};
        static_assert(crafted_clz(value) == 63);
    }

    {
        const int_t value{
            0b0000000000000000000000000000000000000000000000000000000100000000};
        static_assert(crafted_clz(value) == 55);
    }
}
