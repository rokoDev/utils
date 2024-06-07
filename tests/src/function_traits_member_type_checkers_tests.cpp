#include <gtest/gtest.h>
#include <utils/utils.h>

struct empty
{
};

struct s1
{
    using mem_type = int;
};

struct s2
{
    static inline int mem_type = 0;
};

struct s3
{
    int mem_type() { return 0; }
};

struct s4
{
    static int mem_type() { return 0; }
};

CREATE_MEMBER_TYPE_CHECKERS(mem_type)

TEST(MemberTypeCheckers, HasType)
{
    static_assert(not has_type_mem_type_v<empty>);
    static_assert(has_type_mem_type_v<s1>);
    static_assert(not has_type_mem_type_v<s2>);
    static_assert(not has_type_mem_type_v<s3>);
    static_assert(not has_type_mem_type_v<s4>);
}

TEST(MemberTypeCheckers, HasConvertibleType)
{
    static_assert(not has_convertible_type_mem_type_v<empty, int>);
    static_assert(has_convertible_type_mem_type_v<s1, int>);
    static_assert(has_convertible_type_mem_type_v<s1, bool>);
    static_assert(not has_convertible_type_mem_type_v<s2, int>);
    static_assert(not has_convertible_type_mem_type_v<s3, int>);
    static_assert(not has_convertible_type_mem_type_v<s4, int>);
}

TEST(MemberTypeCheckers, HasExactType)
{
    static_assert(not has_exact_type_mem_type_v<empty, int>);
    static_assert(has_exact_type_mem_type_v<s1, int>);
    static_assert(not has_exact_type_mem_type_v<s1, bool>);
    static_assert(not has_exact_type_mem_type_v<s2, int>);
    static_assert(not has_exact_type_mem_type_v<s3, int>);
    static_assert(not has_exact_type_mem_type_v<s3, int (s3::*)()>);
    static_assert(not has_exact_type_mem_type_v<s4, int()>);
}
