#include <gtest/gtest.h>
#include <utils/utils.h>

struct empty
{
};

struct s1
{
    static int sfunc() { return 0; }

    static int sfunc(float) { return 0; }

    static int sfunc(bool) noexcept { return 0; }
};

struct s2
{
    int sfunc() { return 0; }
};

struct s3
{
    static inline int sfunc = 0;
};

CREATE_STATIC_METHOD_CHECKERS(sfunc)

TEST(StaticMethodCheckers, HasStaticFunc)
{
    static_assert(not has_static_invocable_sfunc_v<empty>);
    static_assert(not has_static_invocable_sfunc_v<void, int, float>);

    static_assert(has_static_invocable_sfunc_v<s1>);
    static_assert(not has_static_invocable_sfunc_noexcept_v<s1>);

    static_assert(has_static_invocable_sfunc_v<s1, float>);
    static_assert(not has_static_invocable_sfunc_noexcept_v<s1, float>);

    static_assert(has_static_invocable_sfunc_v<s1, bool>);
    static_assert(has_static_invocable_sfunc_noexcept_v<s1, bool>);

    static_assert(not has_static_invocable_sfunc_v<s2>);
    static_assert(not has_static_invocable_sfunc_noexcept_v<s2>);
    static_assert(not has_static_invocable_sfunc_v<s2, s2*>);
    static_assert(not has_static_invocable_sfunc_noexcept_v<s2, s2&>);

    static_assert(not has_static_invocable_sfunc_v<s3>);
    static_assert(not has_static_invocable_sfunc_noexcept_v<s3>);
}

TEST(StaticMethodCheckers, HasStaticFuncR)
{
    static_assert(not has_static_invocable_r_sfunc_v<void, empty>);
    static_assert(not has_static_invocable_r_sfunc_v<void, int, float>);
    static_assert(
        not has_static_invocable_r_sfunc_noexcept_v<void, int, float>);
    static_assert(has_static_invocable_r_sfunc_v<int, s1>);
    static_assert(has_static_invocable_r_sfunc_v<int, s1, float>);
    static_assert(not has_static_invocable_r_sfunc_noexcept_v<int, s1>);
    static_assert(not has_static_invocable_r_sfunc_noexcept_v<int, s1, float>);
    static_assert(has_static_invocable_r_sfunc_noexcept_v<int, s1, bool>);

    static_assert(not has_static_invocable_r_sfunc_v<int, s2>);
    static_assert(not has_static_invocable_r_sfunc_v<int, s2, s2&>);

    static_assert(not has_static_invocable_r_sfunc_v<int, s3>);
}
