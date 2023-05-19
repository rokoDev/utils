#ifndef utils_h
#define utils_h

#include <array>
#include <cassert>
#include <climits>
#include <cstring>
#include <limits>
#include <string_view>
#include <tuple>
#include <type_traits>

#include "type_list.h"
#include "value_list.h"

namespace utils
{
template <typename T>
struct tag_t
{
};

template <typename T>
constexpr tag_t<T> tag{};

// for details see: https://en.cppreference.com/w/cpp/experimental/is_detected
// and:
// https://stackoverflow.com/questions/257288/templated-check-for-the-existence-of-a-class-member-function

namespace details
{
template <class Default, class AlwaysVoid, template <class...> class Op,
          class... Args>
struct detector
{
    using value_t = std::false_type;
    using type = Default;
};

template <class Default, template <class...> class Op, class... Args>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...>
{
    using value_t = std::true_type;
    using type = Op<Args...>;
};

}  // namespace details

struct nonesuch
{
    ~nonesuch() = delete;
    nonesuch(nonesuch const&) = delete;
    void operator=(nonesuch const&) = delete;
};

template <template <class...> class Op, class... Args>
using is_detected =
    typename details::detector<nonesuch, void, Op, Args...>::value_t;

template <template <class...> class Op, class... Args>
constexpr inline bool is_detected_v = is_detected<Op, Args...>::value;

template <template <class...> class Op, class... Args>
using detected_t =
    typename details::detector<nonesuch, void, Op, Args...>::type;

template <class Default, template <class...> class Op, class... Args>
using detected_or = details::detector<Default, void, Op, Args...>;

template <class Default, template <class...> class Op, class... Args>
using detected_or_t = typename detected_or<Default, Op, Args...>::type;

template <class Expected, template <class...> class Op, class... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

template <class Expected, template <class...> class Op, class... Args>
constexpr inline bool is_detected_exact_v =
    is_detected_exact<Expected, Op, Args...>::value;

template <class To, template <class...> class Op, class... Args>
using is_detected_convertible =
    std::is_convertible<detected_t<Op, Args...>, To>;

template <class To, template <class...> class Op, class... Args>
constexpr inline bool is_detected_convertible_v =
    is_detected_convertible<To, Op, Args...>::value;

template <typename T>
struct is_std_array : std::false_type
{
};

template <typename DataT, std::size_t Size>
struct is_std_array<std::array<DataT, Size>> : std::true_type
{
};

template <class T>
struct remove_cvref
{
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <typename T>
inline constexpr bool is_std_array_v = is_std_array<std::decay_t<T>>::value;

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
    T aValue, std::index_sequence<I...>) noexcept
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
}  //  namespace details

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
constexpr auto make_array(T aValue)
{
    return details::make_array_with_value_impl<T>(
        aValue, std::make_index_sequence<N>{});
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

constexpr uint8_t bits_count(const uint64_t aValue)
{
    return (aValue == 0) ? 0 : 1 + bits_count(aValue >> 1);
}

inline constexpr bool is_power_of_2(std::size_t aValue) noexcept
{
    return aValue && ((aValue & (aValue - 1)) == 0);
}

template <typename T>
static constexpr uintptr_t skip_to_align(void const* aPtr) noexcept
{
    constexpr auto kAlignment = alignof(T);
    static_assert(utils::is_power_of_2(kAlignment));
    const uintptr_t ptrAsUInt = reinterpret_cast<uintptr_t>(aPtr);
    const uintptr_t alignedPtr = (ptrAsUInt + (kAlignment - 1)) & -kAlignment;
    return alignedPtr - ptrAsUInt;
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

template <typename Values1, typename Values2>
struct multiply_2;

template <auto... Values1, auto... Values2>
struct multiply_2<value_list<Values1...>, value_list<Values2...>>
{
    using type = type_list<value_list<Values1..., Values2>...>;
};

template <typename Values1, typename Values2>
using multiply_2_t = typename multiply_2<Values1, Values2>::type;

template <typename TypeList, typename... RestValues>
struct multiply_type_list;

template <typename... Types>
struct multiply_type_list<type_list<Types...>>
{
    using type = type_list<Types...>;
};

template <typename... TypeListValues, typename Values2, typename... RestValues>
struct multiply_type_list<type_list<TypeListValues...>, Values2, RestValues...>
{
    using type = typename multiply_type_list<
        concatenate_t<multiply_2_t<TypeListValues, Values2>...>,
        RestValues...>::type;
};

template <typename Values1, typename Values2>
struct cartesian_product_2;

template <auto... Values1, auto... Values2>
struct cartesian_product_2<value_list<Values1...>, value_list<Values2...>>
{
    using type = concatenate_t<
        multiply_2_t<value_list<Values1>, value_list<Values2...>>...>;
};

template <typename Values1, typename Values2>
using cartesian_product_2_t =
    typename cartesian_product_2<Values1, Values2>::type;

template <typename Values1, typename Values2, typename... RestValues>
struct cartesian_product
{
    using type =
        typename multiply_type_list<cartesian_product_2_t<Values1, Values2>,
                                    RestValues...>::type;
};

template <typename Values1, typename Values2, typename... RestValues>
using cartesian_product_t =
    typename cartesian_product<Values1, Values2, RestValues...>::type;

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
}  // namespace utils

#endif /* utils_h */
