#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "utils/utils.h"

namespace
{
using testing::TestParamInfo;
using ::testing::ValuesIn;
using utils::memory::get_alignment;
static inline constexpr auto PointerValues =
    utils::make_array_from_range<std::uintptr_t{1}, std::uintptr_t{150},
                                 std::uintptr_t{1}>();

std::size_t actual_alignment(void const* const aPtr) noexcept
{
    assert(aPtr);
    const auto ptr_as_uint = reinterpret_cast<std::uintptr_t>(aPtr);
    return 1 << utils::details::crafted_ctz(ptr_as_uint);
}

class Validate : public testing::TestWithParam<std::uintptr_t>
{
};

}  // namespace

TEST_P(Validate, For)
{
    void* ptr = reinterpret_cast<void*>(GetParam());
    const auto kAlignment = get_alignment(ptr);
    const auto kActualAlignment = actual_alignment(ptr);
    ASSERT_EQ(kAlignment, kActualAlignment);
}

INSTANTIATE_TEST_SUITE_P(GetAlignment, Validate, ValuesIn(PointerValues),
                         [](const TestParamInfo<Validate::ParamType>& aInfo)
                         { return std::to_string(aInfo.param); });
