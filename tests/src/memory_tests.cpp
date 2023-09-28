#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <utils/utils.h>

#include <array>
#include <string>

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

template <std::size_t N>
constexpr std::array<std::byte, N> zero_memory(std::byte aInitValue) noexcept
{
    std::array<std::byte, N> buf{};
    for (auto& b: buf)
    {
        b = aInitValue;
    }
    utils::memory::secure_zero_memory(buf.data(), N);
    return buf;
}

template <std::size_t N>
constexpr bool is_zero_buf(const std::array<std::byte, N>& aBuf) noexcept
{
    for (auto& b: aBuf)
    {
        if (std::to_integer<bool>(b))
        {
            return false;
        }
    }
    return true;
}

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

TEST(MemoryTests, SecureZeroMem)
{
    const std::size_t kBufSize = 256;
    std::array<std::byte, kBufSize> buf;
    for (auto& b: buf)
    {
        b = std::byte{10};
    }
    utils::memory::secure_zero_memory(buf.data(), kBufSize);

    bool isZeroBuf = is_zero_buf(buf);

    ASSERT_TRUE(isZeroBuf);
}

TEST(MemoryTests, SecureZeroMemConstexpr)
{
    constexpr auto buf = zero_memory<64>(std::byte{25});

    bool isZeroBuf = is_zero_buf(buf);

    ASSERT_TRUE(isZeroBuf);
}
