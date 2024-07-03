#include <gtest/gtest.h>
#include <type_name/type_name.h>
#include <utils/timer.h>

using timer = utils::timer;

void print_duration(const timer &) noexcept {}

TEST(Timer, DefaultConstructor)
{
    static_assert(not std::is_nothrow_default_constructible_v<timer>);
}

TEST(Timer, CopyConstructor)
{
    static_assert(not std::is_nothrow_copy_constructible_v<timer>);
}

TEST(Timer, MoveConstructor)
{
    static_assert(std::is_nothrow_move_constructible_v<timer>);
    timer t(print_duration);
    const auto tp = t.start_tp();
    timer t2(std::move(t));
    ASSERT_EQ(tp, t2.start_tp());
}

TEST(Timer, CopyAssignment)
{
    static_assert(not std::is_nothrow_copy_assignable_v<timer>);
}

TEST(Timer, MoveAssignment)
{
    static_assert(std::is_nothrow_move_assignable_v<timer>);
    timer t1(print_duration);
    timer t2 = std::move(t1);
    ASSERT_EQ(t1.start_tp(), t2.start_tp());
}

TEST(Timer, ConstructWithFunctionRef)
{
    [[maybe_unused]] timer t(print_duration);
}

TEST(Timer, ConstructWithFunctionPointer)
{
    auto ptr = print_duration;
    [[maybe_unused]] timer t(ptr);
}

TEST(Timer, ConstructWithFunctor)
{
    auto functor = [](const timer &) noexcept {};
    [[maybe_unused]] timer t(functor);
}
