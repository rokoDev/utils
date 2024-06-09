#include <gtest/gtest.h>
#include <utils/utils.h>

struct empty
{
};

struct s1
{
    int mfunc() { return 0; }

    int mfunc(float) { return 0; }

    int mfunc(bool) noexcept { return 0; }
};

struct s2
{
    static int mfunc() { return 0; }

    void mfunc(int, float) const && {}

    void mfunc(bool) &noexcept {}
};

CREATE_R_METHOD_CHECKERS(mfunc)
CREATE_METHOD_CHECKERS(mfunc)

TEST(MethodRCheckers, HasFuncWithReturnType)
{
    static_assert(not has_invocable_r_mfunc_v<void, empty>);
    static_assert(not has_invocable_r_mfunc_v<void, int, float>);

    static_assert(has_invocable_r_mfunc_v<int, s1>);
    static_assert(has_invocable_r_mfunc_v<int, s1, float>);
    static_assert(not has_invocable_r_mfunc_noexcept_v<int, s1>);
    static_assert(not has_invocable_r_mfunc_noexcept_v<int, s1, float>);
    static_assert(not has_invocable_r_mfunc_ref_v<int, s1>);
    static_assert(not has_invocable_r_mfunc_rref_v<int, s1>);
    static_assert(not has_invocable_r_mfunc_const_v<int, s1>);

    static_assert(has_invocable_r_mfunc_v<int, s1, bool>);
    static_assert(has_invocable_r_mfunc_noexcept_v<int, s1, bool>);

    static_assert(not has_invocable_r_mfunc_v<int, s2>);
    static_assert(has_invocable_r_mfunc_const_rref_v<void, s2, int, float>);
    static_assert(has_invocable_r_mfunc_ref_noexcept_v<void, s2, bool>);
    static_assert(
        not has_invocable_r_mfunc_const_rref_noexcept_v<void, s2, int, float>);
    static_assert(has_invocable_r_mfunc_ref_v<void, s2, bool>);
    static_assert(not has_invocable_r_mfunc_volatile_ref_v<void, s2, bool>);
}

TEST(MethodCheckers, HasFunc)
{
    static_assert(not has_invocable_r_mfunc_v<void, empty>);
    static_assert(not has_invocable_r_mfunc_v<void, int, float>);

    static_assert(has_invocable_mfunc_v<s1>);
    static_assert(has_invocable_mfunc_v<s1, float>);
    static_assert(has_invocable_mfunc_v<s1, bool>);

    static_assert(not has_invocable_mfunc_noexcept_v<s1>);
    static_assert(not has_invocable_mfunc_noexcept_v<s1, float>);
    static_assert(has_invocable_mfunc_noexcept_v<s1, bool>);

    static_assert(has_invocable_mfunc_v<s2, int, float>);
    static_assert(not has_invocable_mfunc_v<s2, bool>);
    static_assert(has_invocable_mfunc_v<s2 &, bool>);

    static_assert(not has_invocable_mfunc_noexcept_v<s2, int, float>);
    static_assert(not has_invocable_mfunc_noexcept_v<s2, bool>);
    static_assert(has_invocable_mfunc_noexcept_v<s2 &, bool>);
}
