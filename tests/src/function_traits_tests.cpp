#include <gtest/gtest.h>
#include <utils/function_traits.h>

#include <functional>
#include <iomanip>
#include <iostream>
#include <string>

namespace
{
struct qualifier
{
    template <std::size_t N>
    qualifier &add(const char (&aStr)[N])
    {
        if constexpr (N > ::utils::function_traits::details::qualifier_size_v<
                              decltype("")>)
        {
            if (str.size())
            {
                str.push_back(' ');
            }
            str.append(aStr, N - 1);
        }
        return *this;
    }

    template <std::size_t... N>
    qualifier &add(const char (&...aArgs)[N])
    {
        (..., add(aArgs));
        return *this;
    }

    std::string str;
};

#define CONCAT4(x, y, z, t)                                     \
    qualifier()                                                 \
        .add(UTILS_TO_STR(x), UTILS_TO_STR(y), UTILS_TO_STR(z), \
             UTILS_TO_STR(t))                                   \
        .str

#define X(c, v, r, n) CONCAT4(c, v, r, n),
std::string spec_strings[] = {QUALIFIERS};
#undef X

#define X(c, v, r, n)                                          \
    ::utils::function_traits::details::has_rvalue_reference_v< \
        decltype(UTILS_TO_STR(r))>,
[[maybe_unused]] const bool has_rvalue_reference_qualifier[] = {QUALIFIERS};
#undef X

template <typename T, std::size_t N, std::size_t NameLen>
void print_array_values(const char (&aName)[NameLen], T (&aData)[N],
                        const bool (&aValues)[N])
{
    std::cout << "\n"
              << std::setfill('*') << std::setw(20) << std::right << ' '
              << aName << std::setw(20) << std::left << ' ';
    std::cout << std::boolalpha;
    std::cout << '\n';
    for (std::size_t i = 0; i < N; ++i)
    {
        std::string element =
            std::string("case[") + std::to_string(i) + std::string("] = ");
        std::cout << std::setfill(' ') << std::setw(15) << std::right << element
                  << std::setfill('.') << std::setw(30) << std::right
                  << aData[i] << ": " << aValues[i] << '\n';
    }
    std::cout << '\n';
}
// usage example: print_array_values("HAS RVALUE REFERENCE", spec_strings,
// has_rvalue_reference_qualifier);

class Functor
{
   public:
    void operator()(int) const &noexcept {}
};

class PrivateFunctor
{
   private:
    void operator()(int) {}
};

class DualFunctor
{
   public:
    void operator()() {}
    void operator()(int) {}
};

class TemplatedFunctor
{
   public:
    template <typename T>
    void operator()(T)
    {
    }
};

void func1() {}

void func2() noexcept {}

void func3(int, bool, float &) {}

class Methods
{
   public:
    void m() {}
    void m_const() const {}
    void m_volatile() volatile {}
    void m_const_volatile() const volatile {}
    void m_lvalue_ref() & {}
    void m_const_lvalue_ref() const & {}
    void m_volatile_lvalue_ref() volatile & {}
    void m_const_volatile_lvalue_ref() const volatile & {}
    void m_rvalue_ref() && {}
    void m_const_rvalue_ref() const && {}
    void m_volatile_rvalue_ref() volatile && {}
    void m_const_volatile_rvalue_ref() const volatile && {}
    void m_noexcept() noexcept {}
    void m_const_noexcept() const noexcept {}
    void m_volatile_noexcept() volatile noexcept {}
    void m_const_volatile_noexcept() const volatile noexcept {}
    void m_lvalue_ref_noexcept() &noexcept {}
    void m_const_lvalue_ref_noexcept() const &noexcept {}
    void m_volatile_lvalue_ref_noexcept() volatile &noexcept {}
    void m_const_volatile_lvalue_ref_noexcept() const volatile &noexcept {}
    void m_rvalue_ref_noexcept() &&noexcept {}
    void m_const_rvalue_ref_noexcept() const &&noexcept {}
    void m_volatile_rvalue_ref_noexcept() volatile &&noexcept {}
    void m_const_volatile_rvalue_ref_noexcept() const volatile &&noexcept {}

    void m_volatile_const() volatile const {}
};
}  // namespace

TEST(UtilsCallable, IsSimpleFunctor)
{
    using utils::function_traits::is_simple_functor_v;
    static_assert(is_simple_functor_v<Functor>);
    static_assert(not is_simple_functor_v<PrivateFunctor>);
    static_assert(not is_simple_functor_v<DualFunctor>);
    static_assert(not is_simple_functor_v<TemplatedFunctor>);
}

TEST(UtilsCallable, FunctorCallableTypeMustBeTreatedAsFunction)
{
    func1();
    func2();
    auto f = 1.f;
    func3(5, true, f);

    using utils::function_traits::functor_signature_t;

    static_assert(std::is_function_v<functor_signature_t<Functor>>);
    static_assert(std::is_function_v<decltype(func1)>);
    static_assert(std::is_function_v<decltype(func2)>);
    static_assert(std::is_function_v<decltype(func3)>);
}

TEST(UtilsCallable, IsInvocable)
{
    using utils::function_traits::is_invocable_v;
    static_assert(is_invocable_v<Functor>);
    static_assert(is_invocable_v<decltype(func1)>);
    static_assert(is_invocable_v<decltype(func2)>);
    static_assert(is_invocable_v<decltype(func3)>);
}

TEST(UtilsCallable, RemoveAllQualifiers)
{
    using utils::function_traits::member_signature_t;
    using utils::function_traits::remove_all_qualifiers_t;
    using utils::function_traits::signature_t;

    static_assert(std::is_same_v<remove_all_qualifiers_t<signature_t<Functor>>,
                                 void(int)>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<signature_t<decltype(func1)>>,
                       void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<signature_t<decltype(func2)>>,
                       void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<signature_t<decltype(func3)>>,
                       void(int, bool, float &)>);

    static_assert(
        std::is_same_v<
            remove_all_qualifiers_t<member_signature_t<decltype(&Methods::m)>>,
            void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<
                           member_signature_t<decltype(&Methods::m_const)>>,
                       void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<
                           member_signature_t<decltype(&Methods::m_volatile)>>,
                       void()>);
    static_assert(std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                                     decltype(&Methods::m_const_volatile)>>,
                                 void()>);
    static_assert(std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                                     decltype(&Methods::m_lvalue_ref)>>,
                                 void()>);
    static_assert(std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                                     decltype(&Methods::m_const_lvalue_ref)>>,
                                 void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                           decltype(&Methods::m_volatile_lvalue_ref)>>,
                       void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                           decltype(&Methods::m_const_volatile_lvalue_ref)>>,
                       void()>);
    static_assert(std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                                     decltype(&Methods::m_rvalue_ref)>>,
                                 void()>);
    static_assert(std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                                     decltype(&Methods::m_const_rvalue_ref)>>,
                                 void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                           decltype(&Methods::m_volatile_rvalue_ref)>>,
                       void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                           decltype(&Methods::m_const_volatile_rvalue_ref)>>,
                       void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<
                           member_signature_t<decltype(&Methods::m_noexcept)>>,
                       void()>);
    static_assert(std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                                     decltype(&Methods::m_const_noexcept)>>,
                                 void()>);
    static_assert(std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                                     decltype(&Methods::m_volatile_noexcept)>>,
                                 void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                           decltype(&Methods::m_const_volatile_noexcept)>>,
                       void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                           decltype(&Methods::m_lvalue_ref_noexcept)>>,
                       void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                           decltype(&Methods::m_const_lvalue_ref_noexcept)>>,
                       void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                           decltype(&Methods::m_volatile_lvalue_ref_noexcept)>>,
                       void()>);
    static_assert(
        std::is_same_v<
            remove_all_qualifiers_t<member_signature_t<
                decltype(&Methods::m_const_volatile_lvalue_ref_noexcept)>>,
            void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                           decltype(&Methods::m_rvalue_ref_noexcept)>>,
                       void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                           decltype(&Methods::m_const_rvalue_ref_noexcept)>>,
                       void()>);
    static_assert(
        std::is_same_v<remove_all_qualifiers_t<member_signature_t<
                           decltype(&Methods::m_volatile_rvalue_ref_noexcept)>>,
                       void()>);
    static_assert(
        std::is_same_v<
            remove_all_qualifiers_t<member_signature_t<
                decltype(&Methods::m_const_volatile_rvalue_ref_noexcept)>>,
            void()>);
}

TEST(UtilsCallable, HasConst)
{
    using utils::function_traits::has_const_qualifier_v;
    using utils::function_traits::member_signature_t;

    static_assert(
        !has_const_qualifier_v<member_signature_t<decltype(&Methods::m)>>);
    static_assert(
        has_const_qualifier_v<member_signature_t<decltype(&Methods::m_const)>>);
    static_assert(!has_const_qualifier_v<
                  member_signature_t<decltype(&Methods::m_volatile)>>);
    static_assert(has_const_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_volatile)>>);
    static_assert(!has_const_qualifier_v<
                  member_signature_t<decltype(&Methods::m_lvalue_ref)>>);
    static_assert(has_const_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_lvalue_ref)>>);
    static_assert(
        !has_const_qualifier_v<
            member_signature_t<decltype(&Methods::m_volatile_lvalue_ref)>>);
    static_assert(has_const_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_volatile_lvalue_ref)>>);
    static_assert(!has_const_qualifier_v<
                  member_signature_t<decltype(&Methods::m_rvalue_ref)>>);
    static_assert(has_const_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_rvalue_ref)>>);
    static_assert(
        !has_const_qualifier_v<
            member_signature_t<decltype(&Methods::m_volatile_rvalue_ref)>>);
    static_assert(has_const_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_volatile_rvalue_ref)>>);
    static_assert(!has_const_qualifier_v<
                  member_signature_t<decltype(&Methods::m_noexcept)>>);
    static_assert(has_const_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_noexcept)>>);
    static_assert(!has_const_qualifier_v<
                  member_signature_t<decltype(&Methods::m_volatile_noexcept)>>);
    static_assert(
        has_const_qualifier_v<
            member_signature_t<decltype(&Methods::m_const_volatile_noexcept)>>);
    static_assert(
        !has_const_qualifier_v<
            member_signature_t<decltype(&Methods::m_lvalue_ref_noexcept)>>);
    static_assert(has_const_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_lvalue_ref_noexcept)>>);
    static_assert(!has_const_qualifier_v<member_signature_t<
                      decltype(&Methods::m_volatile_lvalue_ref_noexcept)>>);
    static_assert(
        has_const_qualifier_v<member_signature_t<
            decltype(&Methods::m_const_volatile_lvalue_ref_noexcept)>>);
    static_assert(
        !has_const_qualifier_v<
            member_signature_t<decltype(&Methods::m_rvalue_ref_noexcept)>>);
    static_assert(has_const_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_rvalue_ref_noexcept)>>);
    static_assert(!has_const_qualifier_v<member_signature_t<
                      decltype(&Methods::m_volatile_rvalue_ref_noexcept)>>);
    static_assert(
        has_const_qualifier_v<member_signature_t<
            decltype(&Methods::m_const_volatile_rvalue_ref_noexcept)>>);

    static_assert(has_const_qualifier_v<
                  member_signature_t<decltype(&Methods::m_volatile_const)>>);
}

TEST(UtilsCallable, HasVolatile)
{
    using utils::function_traits::has_volatile_qualifier_v;
    using utils::function_traits::member_signature_t;

    static_assert(
        !has_volatile_qualifier_v<member_signature_t<decltype(&Methods::m)>>);
    static_assert(!has_volatile_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const)>>);
    static_assert(has_volatile_qualifier_v<
                  member_signature_t<decltype(&Methods::m_volatile)>>);
    static_assert(has_volatile_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_volatile)>>);
    static_assert(!has_volatile_qualifier_v<
                  member_signature_t<decltype(&Methods::m_lvalue_ref)>>);
    static_assert(!has_volatile_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_lvalue_ref)>>);
    static_assert(
        has_volatile_qualifier_v<
            member_signature_t<decltype(&Methods::m_volatile_lvalue_ref)>>);
    static_assert(has_volatile_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_volatile_lvalue_ref)>>);
    static_assert(!has_volatile_qualifier_v<
                  member_signature_t<decltype(&Methods::m_rvalue_ref)>>);
    static_assert(!has_volatile_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_rvalue_ref)>>);
    static_assert(
        has_volatile_qualifier_v<
            member_signature_t<decltype(&Methods::m_volatile_rvalue_ref)>>);
    static_assert(has_volatile_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_volatile_rvalue_ref)>>);
    static_assert(!has_volatile_qualifier_v<
                  member_signature_t<decltype(&Methods::m_noexcept)>>);
    static_assert(!has_volatile_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_noexcept)>>);
    static_assert(has_volatile_qualifier_v<
                  member_signature_t<decltype(&Methods::m_volatile_noexcept)>>);
    static_assert(
        has_volatile_qualifier_v<
            member_signature_t<decltype(&Methods::m_const_volatile_noexcept)>>);
    static_assert(
        !has_volatile_qualifier_v<
            member_signature_t<decltype(&Methods::m_lvalue_ref_noexcept)>>);
    static_assert(!has_volatile_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_lvalue_ref_noexcept)>>);
    static_assert(has_volatile_qualifier_v<member_signature_t<
                      decltype(&Methods::m_volatile_lvalue_ref_noexcept)>>);
    static_assert(
        has_volatile_qualifier_v<member_signature_t<
            decltype(&Methods::m_const_volatile_lvalue_ref_noexcept)>>);
    static_assert(
        !has_volatile_qualifier_v<
            member_signature_t<decltype(&Methods::m_rvalue_ref_noexcept)>>);
    static_assert(!has_volatile_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_rvalue_ref_noexcept)>>);
    static_assert(has_volatile_qualifier_v<member_signature_t<
                      decltype(&Methods::m_volatile_rvalue_ref_noexcept)>>);
    static_assert(
        has_volatile_qualifier_v<member_signature_t<
            decltype(&Methods::m_const_volatile_rvalue_ref_noexcept)>>);
}

TEST(UtilsCallable, HasLValueRef)
{
    using utils::function_traits::has_lvalue_reference_qualifier_v;
    using utils::function_traits::member_signature_t;

    static_assert(!has_lvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m)>>);
    static_assert(!has_lvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const)>>);
    static_assert(!has_lvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_volatile)>>);
    static_assert(!has_lvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_volatile)>>);
    static_assert(has_lvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_lvalue_ref)>>);
    static_assert(has_lvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_lvalue_ref)>>);
    static_assert(
        has_lvalue_reference_qualifier_v<
            member_signature_t<decltype(&Methods::m_volatile_lvalue_ref)>>);
    static_assert(has_lvalue_reference_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_volatile_lvalue_ref)>>);
    static_assert(!has_lvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_rvalue_ref)>>);
    static_assert(!has_lvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_rvalue_ref)>>);
    static_assert(
        !has_lvalue_reference_qualifier_v<
            member_signature_t<decltype(&Methods::m_volatile_rvalue_ref)>>);
    static_assert(!has_lvalue_reference_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_volatile_rvalue_ref)>>);
    static_assert(!has_lvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_noexcept)>>);
    static_assert(!has_lvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_noexcept)>>);
    static_assert(!has_lvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_volatile_noexcept)>>);
    static_assert(
        !has_lvalue_reference_qualifier_v<
            member_signature_t<decltype(&Methods::m_const_volatile_noexcept)>>);
    static_assert(
        has_lvalue_reference_qualifier_v<
            member_signature_t<decltype(&Methods::m_lvalue_ref_noexcept)>>);
    static_assert(has_lvalue_reference_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_lvalue_ref_noexcept)>>);
    static_assert(has_lvalue_reference_qualifier_v<member_signature_t<
                      decltype(&Methods::m_volatile_lvalue_ref_noexcept)>>);
    static_assert(
        has_lvalue_reference_qualifier_v<member_signature_t<
            decltype(&Methods::m_const_volatile_lvalue_ref_noexcept)>>);
    static_assert(
        !has_lvalue_reference_qualifier_v<
            member_signature_t<decltype(&Methods::m_rvalue_ref_noexcept)>>);
    static_assert(!has_lvalue_reference_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_rvalue_ref_noexcept)>>);
    static_assert(!has_lvalue_reference_qualifier_v<member_signature_t<
                      decltype(&Methods::m_volatile_rvalue_ref_noexcept)>>);
    static_assert(
        !has_lvalue_reference_qualifier_v<member_signature_t<
            decltype(&Methods::m_const_volatile_rvalue_ref_noexcept)>>);
}

TEST(UtilsCallable, HasRValueRef)
{
    using utils::function_traits::has_rvalue_reference_qualifier_v;
    using utils::function_traits::member_signature_t;

    static_assert(!has_rvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m)>>);
    static_assert(!has_rvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const)>>);
    static_assert(!has_rvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_volatile)>>);
    static_assert(!has_rvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_volatile)>>);
    static_assert(!has_rvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_lvalue_ref)>>);
    static_assert(!has_rvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_lvalue_ref)>>);
    static_assert(
        !has_rvalue_reference_qualifier_v<
            member_signature_t<decltype(&Methods::m_volatile_lvalue_ref)>>);
    static_assert(!has_rvalue_reference_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_volatile_lvalue_ref)>>);
    static_assert(has_rvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_rvalue_ref)>>);
    static_assert(has_rvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_rvalue_ref)>>);
    static_assert(
        has_rvalue_reference_qualifier_v<
            member_signature_t<decltype(&Methods::m_volatile_rvalue_ref)>>);
    static_assert(has_rvalue_reference_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_volatile_rvalue_ref)>>);
    static_assert(!has_rvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_noexcept)>>);
    static_assert(!has_rvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_noexcept)>>);
    static_assert(!has_rvalue_reference_qualifier_v<
                  member_signature_t<decltype(&Methods::m_volatile_noexcept)>>);
    static_assert(
        !has_rvalue_reference_qualifier_v<
            member_signature_t<decltype(&Methods::m_const_volatile_noexcept)>>);
    static_assert(
        !has_rvalue_reference_qualifier_v<
            member_signature_t<decltype(&Methods::m_lvalue_ref_noexcept)>>);
    static_assert(!has_rvalue_reference_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_lvalue_ref_noexcept)>>);
    static_assert(!has_rvalue_reference_qualifier_v<member_signature_t<
                      decltype(&Methods::m_volatile_lvalue_ref_noexcept)>>);
    static_assert(
        !has_rvalue_reference_qualifier_v<member_signature_t<
            decltype(&Methods::m_const_volatile_lvalue_ref_noexcept)>>);
    static_assert(
        has_rvalue_reference_qualifier_v<
            member_signature_t<decltype(&Methods::m_rvalue_ref_noexcept)>>);
    static_assert(has_rvalue_reference_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_rvalue_ref_noexcept)>>);
    static_assert(has_rvalue_reference_qualifier_v<member_signature_t<
                      decltype(&Methods::m_volatile_rvalue_ref_noexcept)>>);
    static_assert(
        has_rvalue_reference_qualifier_v<member_signature_t<
            decltype(&Methods::m_const_volatile_rvalue_ref_noexcept)>>);
}

TEST(UtilsCallable, HasNoexcept)
{
    using utils::function_traits::has_noexcept_qualifier_v;
    using utils::function_traits::member_signature_t;

    static_assert(
        !has_noexcept_qualifier_v<member_signature_t<decltype(&Methods::m)>>);
    static_assert(!has_noexcept_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const)>>);
    static_assert(!has_noexcept_qualifier_v<
                  member_signature_t<decltype(&Methods::m_volatile)>>);
    static_assert(!has_noexcept_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_volatile)>>);
    static_assert(!has_noexcept_qualifier_v<
                  member_signature_t<decltype(&Methods::m_lvalue_ref)>>);
    static_assert(!has_noexcept_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_lvalue_ref)>>);
    static_assert(
        !has_noexcept_qualifier_v<
            member_signature_t<decltype(&Methods::m_volatile_lvalue_ref)>>);
    static_assert(!has_noexcept_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_volatile_lvalue_ref)>>);
    static_assert(!has_noexcept_qualifier_v<
                  member_signature_t<decltype(&Methods::m_rvalue_ref)>>);
    static_assert(!has_noexcept_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_rvalue_ref)>>);
    static_assert(
        !has_noexcept_qualifier_v<
            member_signature_t<decltype(&Methods::m_volatile_rvalue_ref)>>);
    static_assert(!has_noexcept_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_volatile_rvalue_ref)>>);
    static_assert(has_noexcept_qualifier_v<
                  member_signature_t<decltype(&Methods::m_noexcept)>>);
    static_assert(has_noexcept_qualifier_v<
                  member_signature_t<decltype(&Methods::m_const_noexcept)>>);
    static_assert(has_noexcept_qualifier_v<
                  member_signature_t<decltype(&Methods::m_volatile_noexcept)>>);
    static_assert(
        has_noexcept_qualifier_v<
            member_signature_t<decltype(&Methods::m_const_volatile_noexcept)>>);
    static_assert(
        has_noexcept_qualifier_v<
            member_signature_t<decltype(&Methods::m_lvalue_ref_noexcept)>>);
    static_assert(has_noexcept_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_lvalue_ref_noexcept)>>);
    static_assert(has_noexcept_qualifier_v<member_signature_t<
                      decltype(&Methods::m_volatile_lvalue_ref_noexcept)>>);
    static_assert(
        has_noexcept_qualifier_v<member_signature_t<
            decltype(&Methods::m_const_volatile_lvalue_ref_noexcept)>>);
    static_assert(
        has_noexcept_qualifier_v<
            member_signature_t<decltype(&Methods::m_rvalue_ref_noexcept)>>);
    static_assert(has_noexcept_qualifier_v<member_signature_t<
                      decltype(&Methods::m_const_rvalue_ref_noexcept)>>);
    static_assert(has_noexcept_qualifier_v<member_signature_t<
                      decltype(&Methods::m_volatile_rvalue_ref_noexcept)>>);
    static_assert(
        has_noexcept_qualifier_v<member_signature_t<
            decltype(&Methods::m_const_volatile_rvalue_ref_noexcept)>>);
}
