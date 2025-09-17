#include <gtest/gtest.h>
#include <utils/utils.h>

struct equality_comparable
{
    friend constexpr bool operator==(const equality_comparable &lhs,
                                     const equality_comparable &rhs)
    {
        return lhs.a == rhs.a;
    }

    int a;
};

struct noexcept_equality_comparable
{
    friend constexpr bool operator==(
        const noexcept_equality_comparable &lhs,
        const noexcept_equality_comparable &rhs) noexcept
    {
        return lhs.a == rhs.a;
    }

    int a;
};

struct noexcept_equality_comparable_empty
{
    friend constexpr bool operator==(
        const noexcept_equality_comparable_empty &,
        const noexcept_equality_comparable_empty &) noexcept
    {
        return true;
    }
};

struct less_comparable
{
    friend constexpr bool operator<(const less_comparable &lhs,
                                    const less_comparable &rhs)
    {
        return lhs.a < rhs.a;
    }

    int a;
};

struct noexcept_less_comparable
{
    friend constexpr bool operator<(
        const noexcept_less_comparable &lhs,
        const noexcept_less_comparable &rhs) noexcept
    {
        return lhs.a < rhs.a;
    }

    int a;
};

struct noexcept_less_comparable_empty
{
    friend constexpr bool operator<(
        const noexcept_less_comparable_empty &,
        const noexcept_less_comparable_empty &) noexcept
    {
        return true;
    }
};

struct not_empty
{
    int a;
};

struct empty
{
};

TEST(RelOpsCheckerTest, EqualityComparable)
{
    using utils::is_equality_comparable_v;

    static_assert(is_equality_comparable_v<equality_comparable>);
    static_assert(is_equality_comparable_v<noexcept_equality_comparable>);
    static_assert(is_equality_comparable_v<noexcept_equality_comparable_empty>);
    static_assert(not is_equality_comparable_v<not_empty>);
    static_assert(not is_equality_comparable_v<empty>);
}

TEST(RelOpsCheckerTest, NoexceptEqualityComparable)
{
    using utils::is_noexcept_equality_comparable_v;

    static_assert(not is_noexcept_equality_comparable_v<equality_comparable>);
    static_assert(
        is_noexcept_equality_comparable_v<noexcept_equality_comparable>);
    static_assert(
        is_noexcept_equality_comparable_v<noexcept_equality_comparable_empty>);
    static_assert(not is_noexcept_equality_comparable_v<not_empty>);
    static_assert(not is_noexcept_equality_comparable_v<empty>);
}

TEST(RelOpsCheckerTest, LessComparable)
{
    using utils::is_less_comparable_v;

    static_assert(is_less_comparable_v<less_comparable>);
    static_assert(is_less_comparable_v<noexcept_less_comparable>);
    static_assert(is_less_comparable_v<noexcept_less_comparable_empty>);
    static_assert(not is_less_comparable_v<not_empty>);
    static_assert(not is_less_comparable_v<empty>);
}

TEST(RelOpsCheckerTest, NoexceptLessComparable)
{
    using utils::is_noexcept_less_comparable_v;

    static_assert(not is_noexcept_less_comparable_v<less_comparable>);
    static_assert(is_noexcept_less_comparable_v<noexcept_less_comparable>);
    static_assert(
        is_noexcept_less_comparable_v<noexcept_less_comparable_empty>);
    static_assert(not is_noexcept_less_comparable_v<not_empty>);
    static_assert(not is_noexcept_less_comparable_v<empty>);
}
