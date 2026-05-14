#include <gtest/gtest.h>
#include <utils/utils.h>

using utils::type_list;
using utils::function_traits::params_list_t;

void func_1(int) {}

void func_1(int, bool) noexcept {}

float func_1(int, bool, char) noexcept { return 1.f; }

void func_2() {}

void func_3(int, bool, const int &, float &&, const int, const volatile int) {}

template <typename T>
bool func_4(T, int &) noexcept
{
    return (sizeof(T) > 2);
}

template <typename... Ts>
int func_5(bool const *)
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

template <typename T1>
int func_6(int &)
{
    return 0;
}

struct data_t
{
};

template <typename T, typename U>
void func_7(data_t &)
{
}

template <typename T>
auto func_7()
    -> decltype((std::enable_if_t<std::is_same_v<T, float>>(), void()))
{
}

template <typename T, typename... Ts>
int func_7(bool, Ts &&...) noexcept
{
    return sizeof...(Ts) * sizeof(T);
}

CREATE_FREE_FUNCTION_CHECKERS(func_1)
CREATE_FREE_FUNCTION_CHECKERS(func_2)
CREATE_FREE_FUNCTION_CHECKERS(func_3)
CREATE_FREE_FUNCTION_CHECKERS(func_4)
CREATE_FREE_FUNCTION_CHECKERS(func_5)
CREATE_FREE_FUNCTION_CHECKERS(func_6)
CREATE_FREE_FUNCTION_CHECKERS(func_7)
CREATE_FREE_FUNCTION_CHECKERS(non_existent_func)

TEST(FreeFunctionCheckers, IsInvocableTest)
{
    static_assert(is_func_1_invocable_v<int>);
    static_assert(is_func_1_invocable_v<double>);
    static_assert(is_func_1_invocable_v<int, bool>);
    static_assert(is_func_1_invocable_v<int, int>);
    static_assert(is_func_1_invocable_v<int, bool, char>);
    static_assert(is_func_1_invocable_v<int, int, int>);
    static_assert(is_func_1_invocable_v<float, float, float>);
    static_assert(is_func_1_invocable_v<float &, float, int &>);
    static_assert(is_func_1_invocable_v<float &, float *, int &>);
    static_assert(is_func_2_invocable_v<>);
    static_assert(is_func_3_invocable_v<int, bool, const int &, float &&,
                                        const int, const volatile int>);
    static_assert(
        is_func_3_invocable_v<int, bool, const int &, float &&, int, int>);
    static_assert(
        is_func_3_invocable_v<double, char, int &, float &&, bool, float>);
    static_assert(is_func_4_invocable_v<float, int &>);
    static_assert(is_func_4_invocable_v<bool, int &>);
    static_assert(is_func_4_invocable_v<const bool &, int &>);
    static_assert(is_func_4_invocable_v<const int *, int &>);
    static_assert(is_func_5_invocable_v<const bool *>);
    static_assert(is_func_5_invocable_v<bool *>);
    static_assert(is_func_5_invocable_v<type_list<>, bool *>);
    static_assert(is_func_5_invocable_v<type_list<int>, bool *>);
    static_assert(is_func_5_invocable_v<type_list<int, float, bool *>, bool *>);
    static_assert(is_func_6_invocable_v<type_list<float>, int &>);
    static_assert(is_func_6_invocable_v<type_list<float &>, int &>);
    static_assert(is_func_7_invocable_v<type_list<float, int>, data_t &>);
    static_assert(is_func_7_invocable_v<type_list<void, void>, data_t &>);
    static_assert(is_func_7_invocable_v<type_list<float>>);
    static_assert(is_func_7_invocable_v<type_list<float>, bool>);
    static_assert(is_func_7_invocable_v<type_list<float>, bool &>);
    static_assert(is_func_7_invocable_v<type_list<float>, const float &>);
    static_assert(is_func_7_invocable_v<type_list<float>, int>);
    static_assert(
        is_func_7_invocable_v<type_list<float>, int, float &, int &&, data_t>);
}

TEST(FreeFunctionCheckers, IsNotInvocableTest)
{
    static_assert(not is_func_1_invocable_v<int *>);
    static_assert(not is_func_1_invocable_v<float &, data_t, int &>);
    static_assert(not is_func_2_invocable_v<bool>);
    static_assert(not is_func_3_invocable_v<int, bool, const int &, float &,
                                            const int, const volatile int>);
    static_assert(
        not is_func_3_invocable_v<int, bool, const int &, float &, int, int>);
    static_assert(
        not is_func_3_invocable_v<double, char, int &, float &, bool, float>);
    static_assert(not is_func_4_invocable_v<float, const int &>);
    static_assert(not is_func_4_invocable_v<bool, bool &>);
    static_assert(not is_func_4_invocable_v<const bool &, int *>);
    static_assert(not is_func_4_invocable_v<const int *, int>);
    static_assert(not is_func_5_invocable_v<const int *>);
    static_assert(not is_func_5_invocable_v<int *>);
    static_assert(not is_func_5_invocable_v<type_list<>, int>);
    static_assert(not is_func_5_invocable_v<type_list<int>, float>);
    static_assert(
        not is_func_5_invocable_v<type_list<int, float, bool *>, bool &>);
    static_assert(not is_func_6_invocable_v<type_list<float>, const int &>);
    static_assert(not is_func_6_invocable_v<type_list<float &>, bool>);
    static_assert(not is_func_7_invocable_v<type_list<float, int>, int &>);
    static_assert(not is_func_7_invocable_v<type_list<void, void>, bool>);
    static_assert(not is_func_7_invocable_v<type_list<void>>);
    static_assert(not is_func_7_invocable_v<type_list<float, void>, bool, int>);
    static_assert(not is_func_7_invocable_v<type_list<float, float &>, int,
                                            const float &, int &&, data_t>);
}

TEST(FreeFunctionCheckers, IsInvocableRTest)
{
    static_assert(is_func_1_invocable_r_v<void, int>);
    static_assert(is_func_1_invocable_r_v<void, double>);
    static_assert(is_func_1_invocable_r_v<void, int, bool>);
    static_assert(is_func_1_invocable_r_v<void, int, int>);
    static_assert(is_func_1_invocable_r_v<float, int, bool, char>);
    static_assert(is_func_1_invocable_r_v<float, int, int, int>);
    static_assert(is_func_1_invocable_r_v<float, float, float, float>);
    static_assert(is_func_1_invocable_r_v<float, float &, float, int &>);
    static_assert(is_func_1_invocable_r_v<float, float &, float *, int &>);
    static_assert(is_func_2_invocable_r_v<void>);
    static_assert(
        is_func_3_invocable_r_v<void, int, bool, const int &, float &&,
                                const int, const volatile int>);
    static_assert(is_func_3_invocable_r_v<void, int, bool, const int &,
                                          float &&, int, int>);
    static_assert(is_func_3_invocable_r_v<void, double, char, int &, float &&,
                                          bool, float>);
    static_assert(is_func_4_invocable_r_v<bool, float, int &>);
    static_assert(is_func_4_invocable_r_v<bool, bool, int &>);
    static_assert(is_func_4_invocable_r_v<bool, const bool &, int &>);
    static_assert(is_func_4_invocable_r_v<bool, const int *, int &>);
    static_assert(is_func_5_invocable_r_v<int, const bool *>);
    static_assert(is_func_5_invocable_r_v<int, bool *>);
    static_assert(is_func_5_invocable_r_v<int, type_list<>, bool *>);
    static_assert(is_func_5_invocable_r_v<int, type_list<int>, bool *>);
    static_assert(
        is_func_5_invocable_r_v<int, type_list<int, float, bool *>, bool *>);
    static_assert(is_func_6_invocable_r_v<int, type_list<float>, int &>);
    static_assert(is_func_6_invocable_r_v<int, type_list<float &>, int &>);
    static_assert(
        is_func_7_invocable_r_v<void, type_list<float, int>, data_t &>);
    static_assert(
        is_func_7_invocable_r_v<void, type_list<void, void>, data_t &>);
    static_assert(is_func_7_invocable_r_v<void, type_list<float>>);
    static_assert(is_func_7_invocable_r_v<int, type_list<float>, bool>);
    static_assert(is_func_7_invocable_r_v<int, type_list<float>, bool &>);
    static_assert(
        is_func_7_invocable_r_v<int, type_list<float>, const float &>);
    static_assert(is_func_7_invocable_r_v<int, type_list<float>, int>);
    static_assert(is_func_7_invocable_r_v<int, type_list<float>, int, float &,
                                          int &&, data_t>);
}

TEST(FreeFunctionCheckers, IsNotInvocableRTest)
{
    // valid return type but invalid input parameter types
    static_assert(not is_func_1_invocable_r_v<void>);
    static_assert(not is_func_1_invocable_r_v<void, int *>);
    static_assert(not is_func_1_invocable_r_v<float, float &, data_t, int &>);
    static_assert(not is_func_2_invocable_r_v<bool>);
    static_assert(
        not is_func_3_invocable_r_v<void, int, bool, const int &, float &,
                                    const int, const volatile int>);
    static_assert(not is_func_3_invocable_r_v<void, int, bool, const int &,
                                              float &, int, int>);
    static_assert(not is_func_3_invocable_r_v<void, double, char, int &,
                                              float &, bool, float>);
    static_assert(not is_func_4_invocable_r_v<float, const int &>);
    static_assert(not is_func_4_invocable_r_v<bool, bool, bool &>);
    static_assert(not is_func_4_invocable_r_v<bool, const bool &, int *>);
    static_assert(not is_func_4_invocable_r_v<bool, const int *, int>);
    static_assert(not is_func_5_invocable_r_v<int, const int *>);
    static_assert(not is_func_5_invocable_r_v<int, int *>);
    static_assert(not is_func_5_invocable_r_v<int, type_list<>, int>);
    static_assert(not is_func_5_invocable_r_v<int, type_list<int>, float>);
    static_assert(
        not is_func_5_invocable_r_v<int, type_list<int, float, bool *>,
                                    bool &>);
    static_assert(
        not is_func_6_invocable_r_v<int, type_list<float>, const int &>);
    static_assert(not is_func_6_invocable_r_v<int, type_list<float &>, bool>);
    static_assert(
        not is_func_7_invocable_r_v<void, type_list<float, int>, int &>);
    static_assert(
        not is_func_7_invocable_r_v<void, type_list<void, void>, bool>);
    static_assert(not is_func_7_invocable_r_v<void, type_list<void>>);
    static_assert(
        not is_func_7_invocable_r_v<int, type_list<float, void>, bool, int>);
    static_assert(
        not is_func_7_invocable_r_v<int, type_list<float, float &>, int,
                                    const float &, int &&, data_t>);

    // valid input parameter types but invalid return type
    static_assert(not is_func_1_invocable_r_v<bool, int>);
    static_assert(not is_func_1_invocable_r_v<bool, double>);
    static_assert(not is_func_1_invocable_r_v<bool, int, bool>);
    static_assert(not is_func_1_invocable_r_v<bool, int, int>);
    static_assert(not is_func_1_invocable_r_v<double, int, bool, char>);
    static_assert(not is_func_1_invocable_r_v<double, int, int, int>);
    static_assert(not is_func_1_invocable_r_v<double, float, float, float>);
    static_assert(not is_func_1_invocable_r_v<double, float &, float, int &>);
    static_assert(not is_func_1_invocable_r_v<double, float &, float *, int &>);
    static_assert(not is_func_2_invocable_r_v<int>);
    static_assert(
        not is_func_3_invocable_r_v<int, int, bool, const int &, float &&,
                                    const int, const volatile int>);
    static_assert(not is_func_3_invocable_r_v<int, int, bool, const int &,
                                              float &&, int, int>);
    static_assert(not is_func_3_invocable_r_v<int, double, char, int &,
                                              float &&, bool, float>);
    static_assert(not is_func_4_invocable_r_v<void, float, int &>);
    static_assert(not is_func_4_invocable_r_v<void, bool, int &>);
    static_assert(not is_func_4_invocable_r_v<void, const bool &, int &>);
    static_assert(not is_func_4_invocable_r_v<void, const int *, int &>);
    static_assert(not is_func_5_invocable_r_v<char, const bool *>);
    static_assert(not is_func_5_invocable_r_v<char, bool *>);
    static_assert(not is_func_5_invocable_r_v<char, type_list<>, bool *>);
    static_assert(not is_func_5_invocable_r_v<char, type_list<int>, bool *>);
    static_assert(
        not is_func_5_invocable_r_v<char, type_list<int, float, bool *>,
                                    bool *>);
    static_assert(not is_func_6_invocable_r_v<char, type_list<float>, int &>);
    static_assert(not is_func_6_invocable_r_v<char, type_list<float &>, int &>);
    static_assert(
        not is_func_7_invocable_r_v<int, type_list<float, int>, data_t &>);
    static_assert(
        not is_func_7_invocable_r_v<int, type_list<void, void>, data_t &>);
    static_assert(not is_func_7_invocable_r_v<int, type_list<float>>);
    static_assert(not is_func_7_invocable_r_v<void, type_list<float>, bool>);
    static_assert(not is_func_7_invocable_r_v<void, type_list<float>, bool &>);
    static_assert(
        not is_func_7_invocable_r_v<void, type_list<float>, const float &>);
    static_assert(not is_func_7_invocable_r_v<void, type_list<float>, int>);
    static_assert(not is_func_7_invocable_r_v<void, type_list<float>, int,
                                              float &, int &&, data_t>);
}

TEST(FreeFunctionCheckers, IsNoexceptInvocableTest)
{
    static_assert(is_func_1_noexcept_invocable_v<int, bool>);
    static_assert(is_func_1_noexcept_invocable_v<int, int>);
    static_assert(is_func_1_noexcept_invocable_v<int, bool, char>);
    static_assert(is_func_1_noexcept_invocable_v<int, char, bool>);
    static_assert(is_func_1_noexcept_invocable_v<float, float, float>);
    static_assert(is_func_1_noexcept_invocable_v<float &, float, char>);
    static_assert(is_func_1_noexcept_invocable_v<float &, float *, char>);
    static_assert(is_func_4_noexcept_invocable_v<float, int &>);
    static_assert(is_func_4_noexcept_invocable_v<bool, int &>);
    static_assert(is_func_4_noexcept_invocable_v<const bool &, int &>);
    static_assert(is_func_4_noexcept_invocable_v<const int *, int &>);
    static_assert(is_func_7_noexcept_invocable_v<type_list<float>, bool>);
    static_assert(is_func_7_noexcept_invocable_v<type_list<float>, bool &>);
    static_assert(
        is_func_7_noexcept_invocable_v<type_list<float>, const float &>);
    static_assert(is_func_7_noexcept_invocable_v<type_list<float>, int>);
    static_assert(is_func_7_noexcept_invocable_v<type_list<float>, int, float &,
                                                 int &&, data_t>);
}

TEST(FreeFunctionCheckers, IsNotNoexceptInvocableTest)
{
    static_assert(not is_func_1_noexcept_invocable_v<int>);
    static_assert(not is_func_1_noexcept_invocable_v<double>);
    static_assert(not is_func_2_noexcept_invocable_v<>);
    static_assert(
        not is_func_3_noexcept_invocable_v<int, bool, const int &, float &&,
                                           const int, const volatile int>);
    static_assert(not is_func_3_noexcept_invocable_v<int, bool, const int &,
                                                     float &&, int, int>);
    static_assert(not is_func_3_noexcept_invocable_v<double, char, int &,
                                                     float &&, bool, float>);
    static_assert(not is_func_5_noexcept_invocable_v<const bool *>);
    static_assert(not is_func_5_noexcept_invocable_v<bool *>);
    static_assert(not is_func_5_noexcept_invocable_v<type_list<>, bool *>);
    static_assert(not is_func_5_noexcept_invocable_v<type_list<int>, bool *>);
    static_assert(
        not is_func_5_noexcept_invocable_v<type_list<int, float, bool *>,
                                           bool *>);
    static_assert(not is_func_6_noexcept_invocable_v<type_list<float>, int &>);
    static_assert(
        not is_func_6_noexcept_invocable_v<type_list<float &>, int &>);
    static_assert(
        not is_func_7_noexcept_invocable_v<type_list<float, int>, data_t &>);
    static_assert(
        not is_func_7_noexcept_invocable_v<type_list<void, void>, data_t &>);
    static_assert(not is_func_7_noexcept_invocable_v<type_list<float>>);
}

TEST(FreeFunctionCheckers, IsNoexceptInvocableRTest)
{
    static_assert(is_func_1_noexcept_invocable_r_v<void, int, bool>);
    static_assert(is_func_1_noexcept_invocable_r_v<void, int, int>);
    static_assert(is_func_1_noexcept_invocable_r_v<float, int, bool, char>);
    static_assert(is_func_1_noexcept_invocable_r_v<float, int, int, char>);
    static_assert(is_func_1_noexcept_invocable_r_v<float, float, float, float>);
    static_assert(
        is_func_1_noexcept_invocable_r_v<float, float &, float, bool>);
    static_assert(
        is_func_1_noexcept_invocable_r_v<float, float &, float *, char>);
    static_assert(is_func_4_noexcept_invocable_r_v<bool, float, int &>);
    static_assert(is_func_4_noexcept_invocable_r_v<bool, bool, int &>);
    static_assert(is_func_4_noexcept_invocable_r_v<bool, const bool &, int &>);
    static_assert(is_func_4_noexcept_invocable_r_v<bool, const int *, int &>);
    static_assert(
        is_func_7_noexcept_invocable_r_v<int, type_list<float>, bool>);
    static_assert(
        is_func_7_noexcept_invocable_r_v<int, type_list<float>, bool &>);
    static_assert(
        is_func_7_noexcept_invocable_r_v<int, type_list<float>, const float &>);
    static_assert(is_func_7_noexcept_invocable_r_v<int, type_list<float>, int>);
    static_assert(is_func_7_noexcept_invocable_r_v<int, type_list<float>, int,
                                                   float &, int &&, data_t>);
}

TEST(FreeFunctionCheckers, IsNotNoexceptInvocableRTest)
{
    static_assert(not is_func_1_noexcept_invocable_r_v<void, int>);
    static_assert(not is_func_1_noexcept_invocable_r_v<void, double>);
    static_assert(not is_func_2_noexcept_invocable_r_v<void>);
    static_assert(
        not is_func_3_noexcept_invocable_r_v<void, int, bool, const int &,
                                             float &&, const int,
                                             const volatile int>);
    static_assert(
        not is_func_3_noexcept_invocable_r_v<void, int, bool, const int &,
                                             float &&, int, int>);
    static_assert(
        not is_func_3_noexcept_invocable_r_v<void, double, char, int &,
                                             float &&, bool, float>);
    static_assert(not is_func_5_noexcept_invocable_r_v<int, const bool *>);
    static_assert(not is_func_5_noexcept_invocable_r_v<int, bool *>);
    static_assert(
        not is_func_5_noexcept_invocable_r_v<int, type_list<>, bool *>);
    static_assert(
        not is_func_5_noexcept_invocable_r_v<int, type_list<int>, bool *>);
    static_assert(
        not is_func_5_noexcept_invocable_r_v<int, type_list<int, float, bool *>,
                                             bool *>);
    static_assert(
        not is_func_6_noexcept_invocable_r_v<int, type_list<float>, int &>);
    static_assert(
        not is_func_6_noexcept_invocable_r_v<int, type_list<float &>, int &>);
    static_assert(
        not is_func_7_noexcept_invocable_r_v<void, type_list<float, int>,
                                             data_t &>);
    static_assert(
        not is_func_7_noexcept_invocable_r_v<void, type_list<void, void>,
                                             data_t &>);
    static_assert(not is_func_7_noexcept_invocable_r_v<void, type_list<float>>);

    // valid return type but invalid input parameter types
    static_assert(not is_func_1_noexcept_invocable_r_v<void>);
    static_assert(not is_func_1_noexcept_invocable_r_v<void, int *>);
    static_assert(
        not is_func_1_noexcept_invocable_r_v<float, float &, data_t, int &>);
    static_assert(not is_func_2_noexcept_invocable_r_v<bool>);
    static_assert(
        not is_func_3_noexcept_invocable_r_v<void, int, bool, const int &,
                                             float &, const int,
                                             const volatile int>);
    static_assert(
        not is_func_3_noexcept_invocable_r_v<void, int, bool, const int &,
                                             float &, int, int>);
    static_assert(
        not is_func_3_noexcept_invocable_r_v<void, double, char, int &, float &,
                                             bool, float>);
    static_assert(not is_func_4_noexcept_invocable_r_v<float, const int &>);
    static_assert(not is_func_4_noexcept_invocable_r_v<bool, bool, bool &>);
    static_assert(
        not is_func_4_noexcept_invocable_r_v<bool, const bool &, int *>);
    static_assert(not is_func_4_noexcept_invocable_r_v<bool, const int *, int>);
    static_assert(not is_func_5_noexcept_invocable_r_v<int, const int *>);
    static_assert(not is_func_5_noexcept_invocable_r_v<int, int *>);
    static_assert(not is_func_5_noexcept_invocable_r_v<int, type_list<>, int>);
    static_assert(
        not is_func_5_noexcept_invocable_r_v<int, type_list<int>, float>);
    static_assert(
        not is_func_5_noexcept_invocable_r_v<int, type_list<int, float, bool *>,
                                             bool &>);
    static_assert(not is_func_6_noexcept_invocable_r_v<int, type_list<float>,
                                                       const int &>);
    static_assert(
        not is_func_6_noexcept_invocable_r_v<int, type_list<float &>, bool>);
    static_assert(
        not is_func_7_noexcept_invocable_r_v<void, type_list<float, int>,
                                             int &>);
    static_assert(
        not is_func_7_noexcept_invocable_r_v<void, type_list<void, void>,
                                             bool>);
    static_assert(not is_func_7_noexcept_invocable_r_v<void, type_list<void>>);
    static_assert(
        not is_func_7_noexcept_invocable_r_v<int, type_list<float, void>, bool,
                                             int>);
    static_assert(
        not is_func_7_noexcept_invocable_r_v<int, type_list<float, float &>,
                                             int, const float &, int &&,
                                             data_t>);

    // valid input parameter types but invalid return type
    static_assert(not is_func_1_noexcept_invocable_r_v<bool, int>);
    static_assert(not is_func_1_noexcept_invocable_r_v<bool, double>);
    static_assert(not is_func_1_noexcept_invocable_r_v<bool, int, bool>);
    static_assert(not is_func_1_noexcept_invocable_r_v<bool, int, int>);
    static_assert(
        not is_func_1_noexcept_invocable_r_v<double, int, bool, char>);
    static_assert(not is_func_1_noexcept_invocable_r_v<double, int, int, char>);
    static_assert(
        not is_func_1_noexcept_invocable_r_v<double, float, float, float>);
    static_assert(
        not is_func_1_noexcept_invocable_r_v<double, float &, float, bool>);
    static_assert(
        not is_func_1_noexcept_invocable_r_v<double, float &, float *, char>);
    static_assert(not is_func_2_noexcept_invocable_r_v<int>);
    static_assert(
        not is_func_3_noexcept_invocable_r_v<int, int, bool, const int &,
                                             float &&, const int,
                                             const volatile int>);
    static_assert(
        not is_func_3_noexcept_invocable_r_v<int, int, bool, const int &,
                                             float &&, int, int>);
    static_assert(not is_func_3_noexcept_invocable_r_v<int, double, char, int &,
                                                       float &&, bool, float>);
    static_assert(not is_func_4_noexcept_invocable_r_v<void, float, int &>);
    static_assert(not is_func_4_noexcept_invocable_r_v<void, bool, int &>);
    static_assert(
        not is_func_4_noexcept_invocable_r_v<void, const bool &, int &>);
    static_assert(
        not is_func_4_noexcept_invocable_r_v<void, const int *, int &>);
    static_assert(not is_func_5_noexcept_invocable_r_v<char, const bool *>);
    static_assert(not is_func_5_noexcept_invocable_r_v<char, bool *>);
    static_assert(
        not is_func_5_noexcept_invocable_r_v<char, type_list<>, bool *>);
    static_assert(
        not is_func_5_noexcept_invocable_r_v<char, type_list<int>, bool *>);
    static_assert(not is_func_5_noexcept_invocable_r_v<
                  char, type_list<int, float, bool *>, bool *>);
    static_assert(
        not is_func_6_noexcept_invocable_r_v<char, type_list<float>, int &>);
    static_assert(
        not is_func_6_noexcept_invocable_r_v<char, type_list<float &>, int &>);
    static_assert(
        not is_func_7_noexcept_invocable_r_v<int, type_list<float, int>,
                                             data_t &>);
    static_assert(
        not is_func_7_noexcept_invocable_r_v<int, type_list<void, void>,
                                             data_t &>);
    static_assert(not is_func_7_noexcept_invocable_r_v<int, type_list<float>>);
    static_assert(
        not is_func_7_noexcept_invocable_r_v<void, type_list<float>, bool>);
    static_assert(
        not is_func_7_noexcept_invocable_r_v<void, type_list<float>, bool &>);
    static_assert(not is_func_7_noexcept_invocable_r_v<void, type_list<float>,
                                                       const float &>);
    static_assert(
        not is_func_7_noexcept_invocable_r_v<void, type_list<float>, int>);
    static_assert(
        not is_func_7_noexcept_invocable_r_v<void, type_list<float>, int,
                                             float &, int &&, data_t>);
}

TEST(FreeFunctionCheckers, ParamsListTest)
{
    static_assert(std::is_same_v<params_list_t<decltype(func_2)>, type_list<>>);
    static_assert(
        std::is_same_v<params_list_t<decltype(func_3)>,
                       type_list<int, bool, const int &, float &&, int, int>>);
    static_assert(std::is_same_v<params_list_t<decltype(func_4<float>)>,
                                 type_list<float, int &>>);
}
