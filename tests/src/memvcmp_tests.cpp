#include <gtest/gtest.h>
#include <utils/utils.h>

namespace
{
using utils::memvcmp;
}

TEST(UtilsTests, MemvcmpNonEmptyMemTest)
{
    const char arr[10] = {0};
    const auto result = memvcmp(static_cast<const void *>(arr), 0, sizeof(arr));
    ASSERT_EQ(result, true);
}

TEST(UtilsTests, MemvcmpNullptrMemTestZeroSize)
{
    const char *arr = nullptr;
    const auto result = memvcmp(static_cast<const void *>(arr), 0, 0);
    ASSERT_EQ(result, false);
}

TEST(UtilsTests, MemvcmpNullptrMemTestNonZeroSize)
{
    const char *arr = nullptr;
    const auto result = memvcmp(static_cast<const void *>(arr), 0, 10);
    ASSERT_EQ(result, false);
}

TEST(UtilsTests, MemvcmpNonNullptrMemTestOneSize)
{
    const char arr[1] = {0};
    const auto result = memvcmp(static_cast<const void *>(arr), 0, sizeof(arr));
    ASSERT_EQ(result, true);
}
