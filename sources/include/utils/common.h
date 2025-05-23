#ifndef utils_common_h
#define utils_common_h

#include <array>
#include <cassert>
#include <climits>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <limits>
#include <string_view>
#include <tuple>
#include <type_traits>

#include "value_list.h"

#ifdef __clang__
#define UTILS_FUNC __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define UTILS_FUNC __FUNCSIG__
#elif defined(__GNUC__)
#define UTILS_FUNC __PRETTY_FUNCTION__
#else
#error "Compiler unrecognized"
#endif

#ifndef UTILS_STRINGIFY
#define UTILS_STRINGIFY(x) #x
#else
#error "UTILS_STRINGIFY already defined somewhere"
#endif

#ifndef UTILS_STR
#define UTILS_STR(x) UTILS_STRINGIFY(x)
#else
#error "UTILS_STR already defined somewhere"
#endif

#ifndef UTILS_FILE_LINE
#define UTILS_FILE_LINE "file: " __FILE__ "\nline: " UTILS_STR(__LINE__)
#else
#error "UTILS_FILE_LINE already defined somewhere"
#endif

#ifndef UTILS_ABORT_IF
#define UTILS_ABORT_IF_HELPER(condition, format_str, ...)                \
    utils::abort_if(condition,                                           \
                    "[ERROR]\n" UTILS_FILE_LINE "\nfunc: %s" format_str, \
                    UTILS_FUNC, __VA_ARGS__)
#define UTILS_ABORT_IF(...) UTILS_ABORT_IF_HELPER(__VA_ARGS__, "", "")
#else
#error "UTILS_ABORT_IF already defined somewhere"
#endif

#ifdef NDEBUG
#define UTILS_DEBUG_ABORT_IF(...)
#else
#define UTILS_DEBUG_ABORT_IF(...) UTILS_ABORT_IF(__VA_ARGS__)
#endif

namespace utils
{
template <typename T>
struct is_uint
    : std::conjunction<std::is_unsigned<T>,
                       std::negation<std::is_same<T, bool>>>
{
};

template <typename T>
inline constexpr bool is_uint_v = is_uint<T>::value;

template <typename T>
struct is_uint_or_byte
    : std::disjunction<is_uint<T>, std::is_same<T, std::byte>>
{
};

template <typename T>
inline constexpr bool is_uint_or_byte_v = is_uint_or_byte<T>::value;

template <typename... Ts>
struct is_equal_sizes : value_list<sizeof(Ts)...>::is_same
{
};

template <typename... Ts>
inline constexpr bool is_equal_sizes_v = is_equal_sizes<Ts...>::value;

template <typename T>
struct tag_t
{
};

template <typename T>
constexpr tag_t<T> tag{};

template <typename T>
struct is_std_array : std::false_type
{
};

template <typename DataT, std::size_t Size>
struct is_std_array<std::array<DataT, Size>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_std_array_v = is_std_array<std::decay_t<T>>::value;

template <class T>
struct remove_cvref
{
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <typename T>
inline constexpr std::size_t std_array_size_v =
    std::tuple_size_v<std::decay_t<T>>;

template <typename T>
using std_array_data_t = typename std::decay_t<T>::value_type;

template <typename A1, typename A2>
inline constexpr bool is_same_data_std_arrays =
    is_std_array_v<A1>&& is_std_array_v<A2>&&
        std::is_same_v<std_array_data_t<A1>, std_array_data_t<A2>>;

template <typename... Arrays>
inline constexpr std::size_t std_array_sizes_sum = (0 + ... +
                                                    std_array_size_v<Arrays>);

namespace details
{
template <typename A1, typename A2, std::size_t... i1, std::size_t... i2,
          typename = std::enable_if_t<is_same_data_std_arrays<A1, A2>>>
static constexpr auto concatenate_2_arrays(A1&& aArray1, A2&& aArray2,
                                           std::index_sequence<i1...>,
                                           std::index_sequence<i2...>) noexcept
    -> std::array<std_array_data_t<A1>, std_array_sizes_sum<A1, A2>>
{
    return {std::get<i1>(std::forward<A1>(aArray1))...,
            std::get<i2>(std::forward<A2>(aArray2))...};
}

template <typename T, typename = std::enable_if_t<is_std_array_v<T>>>
constexpr decltype(auto) concatenate_arrays(T&& aArray) noexcept
{
    return std::forward<T>(aArray);
}

template <typename A1, typename A2, typename... RestArrays>
constexpr auto concatenate_arrays(A1&& aArray1, A2&& aArray2,
                                  RestArrays&&... aArrays) noexcept
    -> std::array<std_array_data_t<A1>,
                  std_array_sizes_sum<A1, A2, RestArrays...>>
{
    using Indices1 = std::make_index_sequence<std_array_size_v<A1>>;
    using Indices2 = std::make_index_sequence<std_array_size_v<A2>>;
    return concatenate_arrays(
        concatenate_2_arrays(std::forward<A1>(aArray1),
                             std::forward<A2>(aArray2), Indices1{}, Indices2{}),
        std::forward<RestArrays>(aArrays)...);
}

template <typename T, std::size_t I>
constexpr decltype(auto) make_element_for_index(T&& aValue) noexcept
{
    return std::forward<T>(aValue);
}

template <typename T, std::size_t... I>
constexpr std::array<T, sizeof...(I)> make_array_with_value_impl(
    [[maybe_unused]] T aValue, std::index_sequence<I...>) noexcept
{
    return {make_element_for_index<T, I>(std::move(aValue))...};
}

template <std::size_t... I>
constexpr uint8_t numBitSet(uint8_t aValue, std::index_sequence<I...>) noexcept
{
    return static_cast<uint8_t>((... + ((aValue >> I) & 1)));
}

constexpr uint8_t getNumBits(uint8_t aValue)
{
    using Indices = std::make_index_sequence<CHAR_BIT>;
    return numBitSet(aValue, Indices{});
}

template <std::size_t... I>
constexpr std::array<uint8_t, sizeof...(I)> make_num_bits_table_impl(
    std::index_sequence<I...>) noexcept
{
    return {(getNumBits(I))...};
}

constexpr auto make_num_bits_table() noexcept
{
    using Indices =
        std::make_index_sequence<std::numeric_limits<uint8_t>::max() + 1>;
    return make_num_bits_table_impl(Indices{});
}
}  // namespace details

template <typename Array, typename... RestArrays>
constexpr auto concatenate_arrays(Array&& aArray,
                                  RestArrays&&... aArrays) noexcept
    -> std::array<std_array_data_t<Array>,
                  std_array_sizes_sum<Array, RestArrays...>>
{
    return details::concatenate_arrays(std::forward<Array>(aArray),
                                       std::forward<RestArrays>(aArrays)...);
}

template <typename... T,
          typename CommonT = std::decay_t<std::common_type_t<T...>>>
constexpr auto make_array(T&&... aArgs) -> std::array<CommonT, sizeof...(T)>
{
    return {std::forward<T>(aArgs)...};
}

template <typename CommonT>
constexpr auto make_array()
{
    return std::array<CommonT, 0>{};
}

template <typename T, std::size_t N>
constexpr auto make_array(T aValue = {})
{
    return details::make_array_with_value_impl<T>(
        aValue, std::make_index_sequence<N>{});
}

namespace details
{
template <typename ValueType, typename CharT, typename Traits, std::size_t... I>
constexpr auto make_array_from_sv(std::basic_string_view<CharT, Traits> aSV,
                                  std::index_sequence<I...>) noexcept
{
    if constexpr (sizeof...(I))
    {
        return make_array(static_cast<ValueType>(aSV[I])...);
    }
    else
    {
        return std::array<ValueType, 0>{};
    }
}
}  // namespace details

template <std::size_t N, typename ValueType, typename CharT, typename Traits>
constexpr auto make_array(std::basic_string_view<CharT, Traits> aSV) noexcept
{
    assert(N <= aSV.size());
    return details::make_array_from_sv<ValueType>(
        aSV, std::make_index_sequence<N>{});
}

template <const std::string_view& Sv>
constexpr auto make_array() noexcept
{
    return make_array<Sv.size(), std::string_view::value_type>(Sv);
}

template <auto Begin, auto End, auto Step = 1,
          typename CommonT =
              std::decay_t<std::common_type_t<decltype(Begin), decltype(End)>>>
struct is_valid_range
    : std::integral_constant<bool, ((Begin < End) && (Begin < Begin + Step)) ||
                                       (Begin == End)>
{
};

template <auto Begin, auto End, auto Step = 1,
          typename CommonT =
              std::decay_t<std::common_type_t<decltype(Begin), decltype(End)>>>
inline constexpr bool is_valid_range_v =
    is_valid_range<Begin, End, Step, CommonT>::value;

template <auto Begin, auto End, auto Step = 1,
          typename CommonT =
              std::decay_t<std::common_type_t<decltype(Begin), decltype(End)>>>
constexpr auto make_array_from_range() noexcept
{
    constexpr auto kMin = std::min(Begin, End);
    constexpr auto kMax = std::max(Begin, End);
    static_assert(is_valid_range_v<kMin, kMax, Step, CommonT>,
                  "Invalid range arguments.");

    auto step_count = [](auto aBegin, auto aEnd, auto aStep) constexpr
    {
        std::size_t count{};
        while (aBegin <= aEnd)
        {
            ++count;
            aBegin += aStep;
        }
        return count;
    };

    constexpr auto kCount = step_count(kMin, kMax, Step);

    std::array<CommonT, kCount> result{};
    auto currentValue = Begin;
    for (auto& v: result)
    {
        v = CommonT{currentValue};
        currentValue = currentValue + Step;
    }
    return result;
}

template <std::size_t... Len>
constexpr decltype(auto) sv_to_array(const std::string_view aSV) noexcept
{
    if constexpr (sizeof...(Len))
    {
        return make_array(aSV[Len]...);
    }
    else
    {
        return make_array<decltype(aSV)::value_type>();
    }
}

template <typename T, std::size_t N>
constexpr std::array<T, N> make_zero_array()
{
    return details::make_array_with_value_impl<T>(
        T{}, std::make_index_sequence<N>{});
}

template <class T, std::size_t N>
constexpr bool is_equal(const std::array<T, N>& lhs,
                        const std::array<T, N>& rhs,
                        const std::size_t kCount = N)
{
    assert(kCount <= N && "Invalid args.");
    bool isEqual = true;
    for (std::size_t i = 0; i < kCount; ++i)
    {
        if (lhs[i] != rhs[i])
        {
            isEqual = false;
            break;
        }
    }
    return isEqual;
}

inline constexpr auto kNumBitsTable = details::make_num_bits_table();

constexpr uint8_t bits_count(const uint64_t aValue) noexcept
{
    return (aValue == 0) ? 0 : 1 + bits_count(aValue >> 1);
}

[[maybe_unused]] static bool memvcmp(const void* memptr, unsigned char val,
                                     const std::size_t size)
{
    if ((0 == size) || (nullptr == memptr))
    {
        return false;
    }
    const unsigned char* mm = static_cast<const unsigned char*>(memptr);
    return (*mm == val) && (memcmp(mm, mm + 1, size - 1) == 0);
}

template <typename T>
constexpr std::enable_if_t<std::is_unsigned_v<T>, T>
greatest_divisor_that_is_power_of_2(T aValue) noexcept
{
    return aValue ? (aValue & (~(aValue - 1)))
                  : static_cast<T>(1) << (sizeof(T) * CHAR_BIT - 1);
}

inline constexpr bool is_power_of_2(std::size_t aValue) noexcept
{
    return aValue && ((aValue & (aValue - 1)) == 0);
}

inline constexpr std::uintptr_t next_multiple_of(std::uintptr_t aMultiple,
                                                 std::uintptr_t aValue) noexcept
{
    assert(utils::is_power_of_2(aMultiple));
    return (aValue + aMultiple - 1) & -aMultiple;
}

template <auto PowOf2, typename U>
inline constexpr std::enable_if_t<utils::is_power_of_2(PowOf2), U>
next_multiple_of(U aValue) noexcept
{
    return (aValue + static_cast<U>(PowOf2 - 1)) & -static_cast<U>(PowOf2);
}

[[maybe_unused]] static inline std::uintptr_t skip_to_align(
    void const* aPtr, const std::size_t aAlignment) noexcept
{
    assert(utils::is_power_of_2(aAlignment));
    const std::uintptr_t ptrAsUInt = reinterpret_cast<std::uintptr_t>(aPtr);
    return next_multiple_of(aAlignment, ptrAsUInt) - ptrAsUInt;
}

template <typename T>
static std::uintptr_t skip_to_align(void const* aPtr) noexcept
{
    constexpr auto kAlignment = alignof(T);
    static_assert(utils::is_power_of_2(kAlignment));
    return skip_to_align(aPtr, kAlignment);
}

constexpr std::size_t max_alignment_inside_block(
    std::size_t aBlockAlignment, std::size_t aBlockSize) noexcept
{
    assert(is_power_of_2(aBlockAlignment));
    assert(aBlockSize > 0);

    const std::size_t value = aBlockAlignment + aBlockSize - 1;

    // 1. msb = index of most significant bit set in binary representation of
    // value: BlockAlignment + BlockSize - 1
    const std::size_t msb = bits_count(value) - 1;

    // 2. return 2^msb
    const std::size_t max_alignment = 1 << msb;

    return max_alignment;
    ;
};

inline bool is_aligned(void const* aPtr, std::size_t aAlignment) noexcept
{
    assert(utils::is_power_of_2(aAlignment));
    return ((reinterpret_cast<std::uintptr_t>(aPtr) & (aAlignment - 1)) == 0);
}

template <typename T>
inline bool is_aligned(void const* aPtr) noexcept
{
    constexpr auto type_alignment = alignof(T);
    return is_aligned(aPtr, type_alignment);
}

[[maybe_unused]] static void abort_if(const bool aCondition,
                                      char const* aFormat = nullptr,
                                      ...) noexcept
{
    if (aCondition)
    {
        if (aFormat)
        {
            std::va_list arglist;
            va_start(arglist, aFormat);
            vfprintf(stderr, aFormat, arglist);
            va_end(arglist);
        }
        std::abort();
    }
}

template <uint8_t BitsCount>
struct uint_from_nbits
{
   private:
    static constexpr auto getType()
    {
        if constexpr ((BitsCount > 0) && (BitsCount <= CHAR_BIT))
        {
            return uint8_t();
        }
        else if constexpr ((BitsCount > CHAR_BIT) &&
                           (BitsCount <= CHAR_BIT * sizeof(uint16_t)))
        {
            return uint16_t();
        }
        else if constexpr ((BitsCount > CHAR_BIT * sizeof(uint16_t)) &&
                           (BitsCount <= CHAR_BIT * sizeof(uint32_t)))
        {
            return uint32_t();
        }
        else if constexpr ((BitsCount > CHAR_BIT * sizeof(uint32_t)) &&
                           (BitsCount <= CHAR_BIT * sizeof(uint64_t)))
        {
            return uint64_t();
        }
        else
        {
            static_assert(BitsCount > 0, "BitsCount must be more than zero");
            static_assert(BitsCount < sizeof(uint64_t) * CHAR_BIT,
                          "BitsCount count can't be more than 64");
            return uint8_t();
        }
    }

   public:
    using type = decltype(getType());
};

template <uint8_t BitsCount>
using uint_from_nbits_t = typename uint_from_nbits<BitsCount>::type;

template <uint8_t BytesCount>
using uint_from_nbytes_t = uint_from_nbits_t<BytesCount * CHAR_BIT>;

template <typename T>
struct enum_properties
{
    static constexpr uint8_t numBits =
        bits_count(static_cast<uint64_t>(T::Count));
    using SerializeT = uint_from_nbits_t<numBits>;
};

template <uint8_t BitsCount>
struct fast_uint_from_nbits
{
   private:
    static constexpr decltype(auto) getType()
    {
        if constexpr ((BitsCount > 0) && (BitsCount <= CHAR_BIT))
        {
            return uint_fast8_t{};
        }
        else if constexpr ((BitsCount > CHAR_BIT) &&
                           (BitsCount <= CHAR_BIT * sizeof(uint16_t)))
        {
            return uint_fast16_t{};
        }
        else if constexpr ((BitsCount > CHAR_BIT * sizeof(uint16_t)) &&
                           (BitsCount <= CHAR_BIT * sizeof(uint32_t)))
        {
            return uint_fast32_t{};
        }
        else if constexpr ((BitsCount > CHAR_BIT * sizeof(uint32_t)) &&
                           (BitsCount <= CHAR_BIT * sizeof(uint64_t)))
        {
            return uint_fast64_t{};
        }
        else
        {
            static_assert(BitsCount > 0, "BitsCount must be more than zero");
            static_assert(BitsCount < sizeof(uint64_t) * CHAR_BIT,
                          "BitsCount count can't be more than 64");
            return uint8_t{};
        }
    }

   public:
    using type = decltype(getType());
};

template <uint8_t BitsCount>
using fast_uint_from_nbits_t = typename fast_uint_from_nbits<BitsCount>::type;

template <uint8_t BytesCount>
using fast_uint_from_nbytes_t =
    typename fast_uint_from_nbits<BytesCount * CHAR_BIT>::type;

template <typename T>
constexpr std::size_t num_bits()
{
    return CHAR_BIT * sizeof(T);
}

template <>
constexpr std::size_t num_bits<bool>()
{
    return 1;
}

template <typename T>
struct div_t
{
    using value_type = T;
    T quot;
    T rem;
};

template <typename T>
div_t(const T&, const T&) -> div_t<std::enable_if_t<std::is_integral_v<T>, T>>;

template <typename T, typename U>
div_t(T, U) -> div_t<std::enable_if_t<
    std::conjunction_v<std::is_integral<T>, std::is_integral<U>>,
    std::common_type_t<T, U>>>;

template <typename T, typename U>
constexpr decltype(auto) div(T aNumerator, U aDenominator) noexcept
{
    assert(aDenominator != 0);
    static_assert(std::is_integral_v<T>);
    static_assert(std::is_integral_v<U>);
    using type = std::common_type_t<remove_cvref_t<T>, remove_cvref_t<U>>;
    if (__builtin_is_constant_evaluated())
    {
        return div_t<type>{static_cast<type>(static_cast<type>(aNumerator) /
                                             static_cast<type>(aDenominator)),
                           static_cast<type>(static_cast<type>(aNumerator) %
                                             static_cast<type>(aDenominator))};
    }
    else
    {
        using types_with_builtin_impl = type_list<int, long, long long>;
        using common_t = std::common_type_t<type, int, long, long long>;
        using work_t =
            std::conditional_t<types_with_builtin_impl::contains_v<common_t>,
                               common_t, long long>;

        const auto result = std::div(static_cast<work_t>(aNumerator),
                                     static_cast<work_t>(aDenominator));
        return div_t<type>{static_cast<type>(result.quot),
                           static_cast<type>(result.rem)};
    }
}

template <typename R, typename T, typename U>
constexpr div_t<R> div_as(T aNumerator, U aDenominator) noexcept
{
    static_assert(std::is_integral_v<R>);
    static_assert(std::is_integral_v<T>);
    static_assert(std::is_integral_v<U>);
    return div(static_cast<R>(aNumerator), static_cast<R>(aDenominator));
}

template <typename... Ts>
constexpr std::size_t sum_size()
{
    constexpr std::size_t bits_size = (num_bits<Ts>() + ...);
    return bits_size / CHAR_BIT + static_cast<bool>(bits_size % CHAR_BIT);
}

template <typename T>
constexpr std::size_t nbits_occupied(T&& aValue) noexcept
{
    using ValueT = remove_cvref_t<T>;
    static_assert(std::is_unsigned_v<ValueT> && !std::is_same_v<ValueT, bool>,
                  "ValueT must be unsigned and not bool.");
    ValueT vCopy = aValue;
    std::size_t nBitsToSave{};
    while (vCopy)
    {
        vCopy >>= 1;
        ++nBitsToSave;
    }
    return nBitsToSave;
}

template <typename F, typename... Ts>
void for_each_arg(F&& f, Ts&&... ts)
{
    using I = std::initializer_list<int>;
    (void)I{(std::forward<F>(f)(std::forward<Ts>(ts)), 0)...};
}

template <typename F, typename T>
void for_each_in_tuple(F&& f, T&& t)
{
    std::apply([&f](auto&&... xs)
               { for_each_arg(f, std::forward<decltype(xs)>(xs)...); },
               std::forward<T>(t));
}

template <class To, class From>
typename std::enable_if_t<sizeof(To) == sizeof(From) &&
                              std::is_trivially_copyable_v<From> &&
                              std::is_trivially_copyable_v<To>,
                          To>
bit_cast(const From& src) noexcept
{
    static_assert(std::is_trivially_constructible_v<To>,
                  "This implementation additionally requires destination type "
                  "to be trivially constructible");

    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<typename std::underlying_type_t<E>>(e);
}

template <uint8_t NBytes>
using UInt = uint_from_nbytes_t<NBytes>;

template <std::size_t NBytes>
using FastUInt = fast_uint_from_nbytes_t<NBytes>;

template <typename Seq, auto N>
struct shifted_sequence;

template <typename SeqT, SeqT... I, auto N>
struct shifted_sequence<std::integer_sequence<SeqT, I...>, N>
{
    using type = std::integer_sequence<SeqT, I + N...>;
};
template <typename Seq, auto N>
using shifted_sequence_t = typename shifted_sequence<Seq, N>::type;

template <auto First, auto Last, typename = std::enable_if_t<Last >= First>>
struct values_in_range
{
   private:
    using SeqT = std::common_type_t<decltype(First), decltype(Last)>;
    using Indices = shifted_sequence_t<
        std::make_integer_sequence<SeqT, static_cast<SeqT>(Last + 1 - First)>,
        static_cast<SeqT>(First)>;

    template <auto... I>
    static value_list<I...> values_in_range_impl(
        std::integer_sequence<SeqT, I...>) noexcept;

   public:
    using type = decltype(values_in_range_impl(Indices{}));
};

template <auto First, auto Last>
using values_in_range_t = typename values_in_range<First, Last>::type;

template <typename... Ts>
struct is_tuple : std::false_type
{
};

template <typename... Ts>
struct is_tuple<std::tuple<Ts...>> : std::true_type
{
};

template <typename... Ts>
inline constexpr bool is_tuple_v = is_tuple<Ts...>::value;

template <typename T>
struct is_ref_wrapper : public std::false_type
{
};

template <typename T>
struct is_ref_wrapper<std::reference_wrapper<T>> : public std::true_type
{
};

template <typename T>
inline constexpr bool is_ref_wrapper_v = is_ref_wrapper<T>::value;

template <typename T>
struct unwrap_reference
{
    using type = T;
};

template <typename T>
struct unwrap_reference<std::reference_wrapper<T>>
{
    using type = T&;
};

template <typename T>
using unwrap_reference_t = typename unwrap_reference<T>::type;

template <typename T>
inline constexpr decltype(auto) shift_right(T&& aValue,
                                            const std::size_t aNumBits) noexcept
{
    using ValueT = remove_cvref_t<T>;
    static_assert(
        std::conjunction_v<std::is_integral<ValueT>,
                           std::negation<std::is_same<ValueT, bool>>>);
    assert(aNumBits < sizeof(ValueT) * CHAR_BIT);
    return static_cast<ValueT>(aValue >> aNumBits);
}

template <typename T>
inline constexpr decltype(auto) shift_left(T&& aValue,
                                           const std::size_t aNumBits) noexcept
{
    using ValueT = remove_cvref_t<T>;
    static_assert(
        std::conjunction_v<std::is_integral<ValueT>,
                           std::negation<std::is_same<ValueT, bool>>>);
    assert(aNumBits < sizeof(ValueT) * CHAR_BIT);
    return static_cast<ValueT>(aValue << aNumBits);
}
}  // namespace utils

#endif /* utils_common_h */
