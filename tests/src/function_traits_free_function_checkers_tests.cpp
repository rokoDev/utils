#include <gtest/gtest.h>
#include <utils/utils.h>

struct empty
{
};

void useful_func(int) {}

void useful_func(int, bool) noexcept {}

float useful_func(int, bool, char) noexcept { return 1.f; }

CREATE_FREE_FUNCTION_CHECKERS(useful_func)

TEST(FreeFunctionCheckers, HasUsefulFunc)
{
    static_assert(is_useful_func_invocable_v<int>);
    static_assert(is_useful_func_invocable_r_v<void, int>);
    static_assert(not is_useful_func_noexcept_invocable_v<int>);
    static_assert(not is_useful_func_noexcept_invocable_r_v<void, int>);

    static_assert(is_useful_func_invocable_v<int, bool>);
    static_assert(is_useful_func_invocable_r_v<void, int, bool>);
    static_assert(is_useful_func_noexcept_invocable_v<int, bool>);
    static_assert(is_useful_func_noexcept_invocable_r_v<void, int, bool>);

    static_assert(is_useful_func_invocable_v<int, bool, char>);
    static_assert(is_useful_func_invocable_r_v<float, int, bool, char>);
    static_assert(is_useful_func_noexcept_invocable_v<int, bool, char>);
    static_assert(
        is_useful_func_noexcept_invocable_r_v<float, int, bool, char>);

    static_assert(not is_useful_func_invocable_v<double, empty>);
    static_assert(not is_useful_func_invocable_r_v<void>);
    static_assert(not is_useful_func_noexcept_invocable_v<>);
    static_assert(not is_useful_func_noexcept_invocable_r_v<empty, int, bool>);
}
