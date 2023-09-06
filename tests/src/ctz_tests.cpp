#include <gtest/gtest.h>
#include <utils/utils.h>

TEST(UtilsCtzTest, Ctz8)
{
    using utils::ctz;
    using int_t = std::uint8_t;

    {
        const int_t value{0b00000101};
        static_assert(ctz(value) == 0);
        ASSERT_EQ(ctz(value), 0);
    }

    {
        const int_t value{0b00000100};
        static_assert(ctz(value) == 2);
        ASSERT_EQ(ctz(value), 2);
    }

    {
        const int_t value{0b10001000};
        static_assert(ctz(value) == 3);
        ASSERT_EQ(ctz(value), 3);
    }

    {
        const int_t value{0b10000000};
        static_assert(ctz(value) == 7);
        ASSERT_EQ(ctz(value), 7);
    }
}

TEST(UtilsCtzTest, Ctz16)
{
    using utils::ctz;
    using int_t = std::uint16_t;

    {
        const int_t value{0b0000010100000101};
        static_assert(ctz(value) == 0);
        ASSERT_EQ(ctz(value), 0);
    }

    {
        const int_t value{0b0000010000000100};
        static_assert(ctz(value) == 2);
        ASSERT_EQ(ctz(value), 2);
    }

    {
        const int_t value{0b1000100010001000};
        static_assert(ctz(value) == 3);
        ASSERT_EQ(ctz(value), 3);
    }

    {
        const int_t value{0b1000000010000000};
        static_assert(ctz(value) == 7);
        ASSERT_EQ(ctz(value), 7);
    }

    {
        const int_t value{0b1000000000000000};
        static_assert(ctz(value) == 15);
        ASSERT_EQ(ctz(value), 15);
    }
}

TEST(UtilsCtzTest, Ctz32)
{
    using utils::ctz;
    using int_t = std::uint32_t;

    {
        const int_t value{0b00000101000001010000010100000101};
        static_assert(ctz(value) == 0);
        ASSERT_EQ(ctz(value), 0);
    }

    {
        const int_t value{0b00000100000001000000010000000100};
        static_assert(ctz(value) == 2);
        ASSERT_EQ(ctz(value), 2);
    }

    {
        const int_t value{0b10001000100010001000100010001000};
        static_assert(ctz(value) == 3);
        ASSERT_EQ(ctz(value), 3);
    }

    {
        const int_t value{0b10000000100000001000000010000000};
        static_assert(ctz(value) == 7);
        ASSERT_EQ(ctz(value), 7);
    }

    {
        const int_t value{0b10000000000000001000000000000000};
        static_assert(ctz(value) == 15);
        ASSERT_EQ(ctz(value), 15);
    }

    {
        const int_t value{0b10000000000000000000000000000000};
        static_assert(ctz(value) == 31);
        ASSERT_EQ(ctz(value), 31);
    }
}

TEST(UtilsCtzTest, Ctz64)
{
    using utils::ctz;
    using int_t = std::uint64_t;

    {
        const int_t value{
            0b0000010100000101000001010000010100000101000001010000010100000101};
        static_assert(ctz(value) == 0);
        ASSERT_EQ(ctz(value), 0);
    }

    {
        const int_t value{
            0b0000010000000100000001000000010000000100000001000000010000000100};
        static_assert(ctz(value) == 2);
        ASSERT_EQ(ctz(value), 2);
    }

    {
        const int_t value{
            0b1000100010001000100010001000100010001000100010001000100010001000};
        static_assert(ctz(value) == 3);
        ASSERT_EQ(ctz(value), 3);
    }

    {
        const int_t value{
            0b1000000010000000100000001000000010000000100000001000000010000000};
        static_assert(ctz(value) == 7);
        ASSERT_EQ(ctz(value), 7);
    }

    {
        const int_t value{
            0b1000000000000000100000000000000010000000000000001000000000000000};
        static_assert(ctz(value) == 15);
        ASSERT_EQ(ctz(value), 15);
    }

    {
        const int_t value{
            0b1000000000000000000000000000000010000000000000000000000000000000};
        static_assert(ctz(value) == 31);
        ASSERT_EQ(ctz(value), 31);
    }

    {
        const int_t value{
            0b1000000000000000000000000000000000000000000000000000000000000000};
        static_assert(ctz(value) == 63);
        ASSERT_EQ(ctz(value), 63);
    }

    {
        const int_t value{
            0b1000000010000000000000000000000000000000000000000000000000000000};
        static_assert(ctz(value) == 55);
        ASSERT_EQ(ctz(value), 55);
    }
}

TEST(UtilsCtzTest, CraftedCtz8)
{
    using utils::details::crafted_ctz;
    using int_t = std::uint8_t;

    {
        const int_t value{0b00000101};
        static_assert(crafted_ctz(value) == 0);
    }

    {
        const int_t value{0b00000100};
        static_assert(crafted_ctz(value) == 2);
    }

    {
        const int_t value{0b10001000};
        static_assert(crafted_ctz(value) == 3);
    }

    {
        const int_t value{0b10000000};
        static_assert(crafted_ctz(value) == 7);
    }
}

TEST(UtilsCtzTest, CraftedCtz16)
{
    using utils::details::crafted_ctz;
    using int_t = std::uint16_t;

    {
        const int_t value{0b0000010100000101};
        static_assert(crafted_ctz(value) == 0);
    }

    {
        const int_t value{0b0000010000000100};
        static_assert(crafted_ctz(value) == 2);
    }

    {
        const int_t value{0b1000100010001000};
        static_assert(crafted_ctz(value) == 3);
    }

    {
        const int_t value{0b1000000010000000};
        static_assert(crafted_ctz(value) == 7);
    }

    {
        const int_t value{0b1000000000000000};
        static_assert(crafted_ctz(value) == 15);
    }
}

TEST(UtilsCtzTest, CraftedCtz32)
{
    using utils::details::crafted_ctz;
    using int_t = std::uint32_t;

    {
        const int_t value{0b00000101000001010000010100000101};
        static_assert(crafted_ctz(value) == 0);
    }

    {
        const int_t value{0b00000100000001000000010000000100};
        static_assert(crafted_ctz(value) == 2);
    }

    {
        const int_t value{0b10001000100010001000100010001000};
        static_assert(crafted_ctz(value) == 3);
    }

    {
        const int_t value{0b10000000100000001000000010000000};
        static_assert(crafted_ctz(value) == 7);
    }

    {
        const int_t value{0b10000000000000001000000000000000};
        static_assert(crafted_ctz(value) == 15);
    }

    {
        const int_t value{0b10000000000000000000000000000000};
        static_assert(crafted_ctz(value) == 31);
    }
}

TEST(UtilsCtzTest, CraftedCtz64)
{
    using utils::details::crafted_ctz;
    using int_t = std::uint64_t;

    {
        const int_t value{
            0b0000010100000101000001010000010100000101000001010000010100000101};
        static_assert(crafted_ctz(value) == 0);
    }

    {
        const int_t value{
            0b0000010000000100000001000000010000000100000001000000010000000100};
        static_assert(crafted_ctz(value) == 2);
    }

    {
        const int_t value{
            0b1000100010001000100010001000100010001000100010001000100010001000};
        static_assert(crafted_ctz(value) == 3);
    }

    {
        const int_t value{
            0b1000000010000000100000001000000010000000100000001000000010000000};
        static_assert(crafted_ctz(value) == 7);
    }

    {
        const int_t value{
            0b1000000000000000100000000000000010000000000000001000000000000000};
        static_assert(crafted_ctz(value) == 15);
    }

    {
        const int_t value{
            0b1000000000000000000000000000000010000000000000000000000000000000};
        static_assert(crafted_ctz(value) == 31);
    }

    {
        const int_t value{
            0b1000000000000000000000000000000000000000000000000000000000000000};
        static_assert(crafted_ctz(value) == 63);
    }

    {
        const int_t value{
            0b1000000010000000000000000000000000000000000000000000000000000000};
        static_assert(crafted_ctz(value) == 55);
    }
}
