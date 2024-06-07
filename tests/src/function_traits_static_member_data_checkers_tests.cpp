#include <gtest/gtest.h>
#include <utils/utils.h>

struct empty
{
};

struct s1
{
    static inline int mem_data = 0;
};

struct s2
{
    int mem_data;
};

struct s3
{
    int mem_data() { return 0; }
};

struct s4
{
    static int mem_data() { return 0; }
};

CREATE_STATIC_MEMBER_DATA_CHECKERS(mem_data)

TEST(StaticMemberDataCheckers, HasData)
{
    static_assert(not has_static_data_mem_data_v<empty>);
    static_assert(not has_static_data_mem_data_v<void>);
    static_assert(has_static_data_mem_data_v<s1>);
    static_assert(not has_static_data_mem_data_v<s2>);
    static_assert(not has_static_data_mem_data_v<s3>);
    static_assert(not has_static_data_mem_data_v<s4>);
}
