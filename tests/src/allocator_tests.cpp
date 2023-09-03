#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "utils/allocator.h"

namespace
{
using namespace utils::memory;
}

TEST(Allocator, DefaultConstructor)
{
    allocator<int> myallocator;
    ASSERT_EQ(myallocator.resource(), get_default_resource());
}
