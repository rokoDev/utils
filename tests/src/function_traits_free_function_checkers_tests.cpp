#include <gtest/gtest.h>
#include <utils/utils.h>

using utils::type_list;
using utils::function_traits::params_list_t;

struct empty
{
};

void useful_func(int) {}

void useful_func(int, bool) noexcept {}

float useful_func(int, bool, char) noexcept { return 1.f; }

void uniq_func_1() {}

void uniq_func_2(int, bool, const int &, float &&, const int,
                 const volatile int)
{
}

template <typename T>
void uniq_func_3(T, int &) noexcept
{
}

template <typename... Ts>
int compute(const bool *)
{
    if (sizeof...(Ts) > 1)
    {
        return sizeof...(Ts) * sizeof(float);
    }
    else
    {
        return sizeof(bool) * sizeof(float);
    }
}

CREATE_FREE_FUNCTION_CHECKERS(useful_func)
CREATE_FREE_FUNCTION_TEMPLATE_CHECKERS(compute)

TEST(FreeFunctionCheckers, ParamsListTest)
{
    static_assert(
        std::is_same_v<params_list_t<decltype(uniq_func_1)>, type_list<>>);
    static_assert(
        std::is_same_v<params_list_t<decltype(uniq_func_2)>,
                       type_list<int, bool, const int &, float &&, int, int>>);
    static_assert(std::is_same_v<params_list_t<decltype(uniq_func_3<float>)>,
                                 type_list<float, int &>>);
}

TEST(FreeFunctionCheckers, HasComputeTemplateFunc)
{
    static_assert(
        is_compute_invocable_template_v<type_list<int>, const bool *>);
    static_assert(
        is_compute_invocable_template_r_v<int, type_list<int>, const bool *>);
    static_assert(
        not is_compute_noexcept_invocable_template_r_v<int, type_list<int>,
                                                       const bool *>);

    static_assert(is_compute_invocable_template_v<type_list<>, const bool *>);

    static_assert(
        is_compute_invocable_template_v<type_list<bool, void>, const bool *>);
    static_assert(
        not is_compute_noexcept_invocable_template_v<type_list<bool, void>,
                                                     const bool *>);
}

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
