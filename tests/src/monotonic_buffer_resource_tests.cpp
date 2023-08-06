#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "utils/monotonic_buffer_resource.h"

namespace
{
template <std::size_t BufferAlignment, std::size_t BufferSize>
class monotonic_buffer_resource_test : public ::testing::Test
{
   protected:
    using monotonic_buffer_resource = utils::memory::monotonic_buffer_resource;
    using void_ptr_t = void *;

    ~monotonic_buffer_resource_test() override { resource.release(); }

    alignas(BufferAlignment) std::byte data_[BufferSize]{};
    monotonic_buffer_resource resource{data_};
    const monotonic_buffer_resource &c_resource{resource};
};

using MonotonicBufferResource = monotonic_buffer_resource_test<16, 256>;
}  // namespace

TEST_F(MonotonicBufferResource, Construct) { ASSERT_TRUE(true); }
