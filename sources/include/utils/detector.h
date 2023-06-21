#ifndef utils_detector_h
#define utils_detector_h

#include <type_traits>

#ifndef UTILS_TO_STR
#define UTILS_TO_STR(x) #x
#endif

namespace utils
{
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
}  // namespace utils

#endif /* utils_detector_h */
