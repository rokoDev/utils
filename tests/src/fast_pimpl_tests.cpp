#include <gtest/gtest.h>
#include <utils/fast_pimpl.h>

struct my_type
{
    char c;
    float f;
    int i;
};

TEST(FastPimpl, DefaultConstructor)
{
    utils::fast_pimpl<my_type, sizeof(my_type), alignof(my_type)> k;
}
//  TODO: right unit tests
