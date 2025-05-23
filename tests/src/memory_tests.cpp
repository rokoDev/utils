#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <utils/utils.h>

#include <array>
#include <string>

namespace
{
using ::testing::AssertionFailure;
using ::testing::AssertionResult;
using ::testing::AssertionSuccess;
using ::testing::TestParamInfo;
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

template <std::size_t N>
constexpr decltype(auto) constexpr_memmove() noexcept
{
    const std::size_t kBufSize = 16;
    std::array<std::byte, kBufSize> buf{};
    for (std::size_t i = 0; i < kBufSize; ++i)
    {
        buf[i] = std::byte{static_cast<uint8_t>(i)};
    }

    utils::memory::memmove(buf.data(), buf.data() + 5, 10);
    return buf;
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

TEST(MemoryTests, SecureZeroMemSafeWithNullBuf)
{
    const auto result = utils::memory::secure_zero_memory_safe(nullptr, 0);
    ASSERT_EQ(result, -1);
}

TEST(MemoryTests, SecureZeroMemSafeWithZeroSize)
{
    const std::size_t kBufSize = 16;
    std::array<std::byte, kBufSize> buf;
    for (auto& b: buf)
    {
        b = std::byte{10};
    }
    const auto result = utils::memory::secure_zero_memory_safe(buf.data(), 0);
    ASSERT_EQ(result, -2);
}

TEST(MemoryTests, SecureZeroMemSafe)
{
    const std::size_t kBufSize = 256;
    std::array<std::byte, kBufSize> buf;
    for (auto& b: buf)
    {
        b = std::byte{10};
    }
    const auto result =
        utils::memory::secure_zero_memory_safe(buf.data(), kBufSize);
    ASSERT_EQ(result, 0);

    bool isZeroBuf = is_zero_buf(buf);

    ASSERT_TRUE(isZeroBuf);
}

TEST(MemoryTests, SecureZeroMemConstexpr)
{
    constexpr auto buf = zero_memory<64>(std::byte{25});

    bool isZeroBuf = is_zero_buf(buf);

    ASSERT_TRUE(isZeroBuf);
}

TEST(MemoryTests, Memmove)
{
    const std::size_t kBufSize = 16;
    std::array<std::byte, kBufSize> buf;
    for (std::size_t i = 0; i < kBufSize; ++i)
    {
        buf[i] = std::byte{static_cast<uint8_t>(i)};
    }

    utils::memory::memmove(buf.data(), buf.data() + 5, 10);

    constexpr std::array<std::byte, kBufSize> expected{
        std::byte{5},  std::byte{6},  std::byte{7},  std::byte{8},
        std::byte{9},  std::byte{10}, std::byte{11}, std::byte{12},
        std::byte{13}, std::byte{14}, std::byte{10}, std::byte{11},
        std::byte{12}, std::byte{13}, std::byte{14}, std::byte{15}};

    ASSERT_EQ(buf, expected);
}

TEST(MemoryTests, MemmoveConstexpr)
{
    const std::size_t kBufSize = 16;
    constexpr auto buf = constexpr_memmove<kBufSize>();

    constexpr std::array<std::byte, kBufSize> expected{
        std::byte{5},  std::byte{6},  std::byte{7},  std::byte{8},
        std::byte{9},  std::byte{10}, std::byte{11}, std::byte{12},
        std::byte{13}, std::byte{14}, std::byte{10}, std::byte{11},
        std::byte{12}, std::byte{13}, std::byte{14}, std::byte{15}};

    ASSERT_EQ(buf, expected);
}

template <typename Dst, typename Src>
AssertionResult AssertEqualBytes(const char* i_expr, const char* dst_expr,
                                 const char* src_expr, std::size_t i, Dst dst,
                                 Src src)
{
    if (dst == static_cast<Dst>(src))
    {
        return AssertionSuccess();
    }
    else
    {
        return AssertionFailure()
               << "for " << i_expr << " = " << i << '\n'
               << dst_expr << " = "
               << std::bitset<CHAR_BIT>(std::to_integer<std::size_t>(dst))
               << '\n'
               << src_expr << " = "
               << std::bitset<CHAR_BIT>(
                      std::to_integer<std::size_t>(static_cast<Dst>(src)))
               << "\nwhich means they are not equal.";
    }
}

template <typename Dst, typename Src, std::size_t BufSize = 16>
constexpr auto make_result() noexcept
{
    using dst_array_t = std::array<Dst, BufSize>;
    using src_array_t = std::array<Src, BufSize>;
    src_array_t src{};
    for (std::size_t i = 0; i < BufSize; ++i)
    {
        src[i] = static_cast<Src>(i + 1);
    }
    dst_array_t dst{};

    auto* dst_buf = dst.data();
    const auto* src_buf = src.data();

    utils::memory::memcpy(dst_buf, src_buf, BufSize);
    return std::make_tuple(dst, src);
};

TEST(MemoryTests, MemcpyByteByte)
{
    const std::size_t kBufSize = 16;
    std::byte src_buf[kBufSize]{};
    for (std::size_t i = 0; i < kBufSize; ++i)
    {
        src_buf[i] = static_cast<std::byte>(i + 1);
    }

    std::byte dst_buf[kBufSize]{};

    utils::memory::memcpy(dst_buf, src_buf, kBufSize);

    for (std::size_t i = 0; i < kBufSize; ++i)
    {
        EXPECT_PRED_FORMAT3(AssertEqualBytes, i, dst_buf[i], src_buf[i]);
    }
}

TEST(MemoryTests, MemcpyByteByteConstexpr)
{
    constexpr auto result = make_result<std::byte, std::byte>();
    const auto& dst_buf = std::get<0>(result);
    const auto& src_buf = std::get<1>(result);

    for (std::size_t i = 0; i < dst_buf.size(); ++i)
    {
        EXPECT_PRED_FORMAT3(AssertEqualBytes, i, dst_buf[i], src_buf[i]);
    }
}

TEST(MemoryTests, MemcpyByteChar)
{
    const std::size_t kBufSize = 16;
    char src_buf[kBufSize]{};
    for (std::size_t i = 0; i < kBufSize; ++i)
    {
        src_buf[i] = static_cast<char>(i + 1);
    }

    std::byte dst_buf[kBufSize]{};

    utils::memory::memcpy(dst_buf, src_buf, kBufSize);

    for (std::size_t i = 0; i < kBufSize; ++i)
    {
        EXPECT_PRED_FORMAT3(AssertEqualBytes, i, dst_buf[i],
                            static_cast<std::byte>(src_buf[i]));
    }
}

TEST(MemoryTests, MemcpyByteVoid)
{
    using dst_t = std::byte;
    using src_t = void;
    const std::size_t kBufSize = 16;
    std::byte buf[kBufSize]{};
    for (std::size_t i = 0; i < kBufSize; ++i)
    {
        *(buf + i) = static_cast<std::byte>(i + 1);
    }

    src_t* src_buf = buf;
    dst_t dst_buf[kBufSize]{};

    utils::memory::memcpy(dst_buf, src_buf, kBufSize);

    for (std::size_t i = 0; i < kBufSize; ++i)
    {
        EXPECT_PRED_FORMAT3(AssertEqualBytes, i, dst_buf[i],
                            *(static_cast<const std::byte*>(src_buf) + i));
    }
}

TEST(MemoryTests, MemcpyVoidByte)
{
    using dst_t = void;
    using src_t = std::byte;
    const std::size_t kBufSize = 16;
    std::byte src[kBufSize]{};
    for (std::size_t i = 0; i < kBufSize; ++i)
    {
        *(src + i) = static_cast<std::byte>(i + 1);
    }

    src_t* src_buf = src;

    std::byte dst[kBufSize]{};
    dst_t* dst_buf = dst;

    utils::memory::memcpy(dst_buf, src_buf, kBufSize);

    for (std::size_t i = 0; i < kBufSize; ++i)
    {
        EXPECT_PRED_FORMAT3(AssertEqualBytes, i,
                            *(static_cast<std::byte*>(dst_buf) + i),
                            *(static_cast<const std::byte*>(src_buf) + i));
    }
}

TEST(MemoryTests, MemcpyVoidVoid)
{
    using dst_t = void;
    using src_t = void;
    const std::size_t kBufSize = 16;
    std::byte src[kBufSize]{};
    for (std::size_t i = 0; i < kBufSize; ++i)
    {
        *(src + i) = static_cast<std::byte>(i + 1);
    }

    src_t* src_buf = src;

    std::byte dst[kBufSize]{};
    dst_t* dst_buf = dst;

    utils::memory::memcpy(dst_buf, src_buf, kBufSize);

    for (std::size_t i = 0; i < kBufSize; ++i)
    {
        EXPECT_PRED_FORMAT3(AssertEqualBytes, i,
                            *(static_cast<std::byte*>(dst_buf) + i),
                            *(static_cast<const std::byte*>(src_buf) + i));
    }
}

TEST(MemoryTests, MemcpyVoidVoidConstexpr)
{
    constexpr auto result = make_result<std::byte, unsigned char>();
    const auto& dst_buf = std::get<0>(result);
    const auto& src_buf = std::get<1>(result);

    for (std::size_t i = 0; i < dst_buf.size(); ++i)
    {
        EXPECT_PRED_FORMAT3(AssertEqualBytes, i, dst_buf[i], src_buf[i]);
    }
}
