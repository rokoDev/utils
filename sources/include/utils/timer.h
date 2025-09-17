#ifndef utils_timer_h
#define utils_timer_h

#include <chrono>
#include <type_traits>
#include <utility>

namespace utils
{
namespace details
{
template <typename Clock, typename Timer>
struct timer_impl
{
    using clock = Clock;
    using time_point = std::chrono::time_point<clock>;
    using duration =
        decltype(std::declval<time_point>() - std::declval<time_point>());
    using callback_ptr_t = void (*)(const Timer &) noexcept;
    using callback_ref_t = void (&)(const Timer &) noexcept;

    using callback_t = void (*)(std::byte *, const Timer &) noexcept;

    template <typename Callable>
    struct storable
    {
        using type = std::conditional_t<
            std::is_same_v<Callable, callback_ref_t>, callback_ptr_t,
            std::enable_if_t<
                std::is_invocable_r_v<void, Callable, duration>,
                std::remove_cv_t<std::remove_reference_t<Callable>>>>;
    };

    template <typename Callable>
    using storable_t = typename storable<Callable>::type;

    template <typename FinishCallback>
    static void call_finish_callback(std::byte *aData,
                                     const Timer &aTimer) noexcept
    {
        callback<FinishCallback>(aData)(aTimer);
    }

    template <typename T>
    static T &callback(std::byte *aData) noexcept
    {
        return *reinterpret_cast<T *>(aData);
    }

    template <std::size_t... I>
    static constexpr void copy_impl(std::byte (&aDst)[sizeof...(I)],
                                    std::byte (&aSrc)[sizeof...(I)],
                                    std::index_sequence<I...>) noexcept
    {
        (..., (aDst[I] = aSrc[I]));
    }

    template <std::size_t Size>
    static constexpr void copy(std::byte (&aDst)[Size],
                               std::byte (&aSrc)[Size]) noexcept
    {
        copy_impl(aDst, aSrc, std::make_index_sequence<Size>{});
    }
};

template <std::size_t MaxCallbackAlignment, std::size_t MaxCallbackSize,
          typename Clock>
class timer
{
   private:
    using impl = timer_impl<Clock, timer>;

   public:
    static constexpr auto kMaxCallbackAlignment = MaxCallbackAlignment;
    static constexpr auto kMaxCallbackSize = MaxCallbackSize;
    using clock = typename impl::clock;
    using duration = typename impl::duration;
    using time_point = typename impl::time_point;

    ~timer()
    {
        if (callback_)
        {
            std::invoke(callback_, data_, *this);
        }
    }

    template <
        typename FinishCallback,
        typename = std::enable_if_t<not std::is_same_v<
            timer, std::remove_cv_t<std::remove_reference_t<FinishCallback>>>>>
    timer(FinishCallback &&aCallback) noexcept
        : start_{impl::clock::now()}
        , callback_{&impl::template call_finish_callback<
              typename impl::template storable_t<FinishCallback>>}
    {
        using stored_callback_t =
            typename impl::template storable_t<FinishCallback>;
        using callback_ref_t = typename impl::callback_ref_t;
        static_assert(alignof(stored_callback_t) <= MaxCallbackAlignment);
        static_assert(sizeof(stored_callback_t) <= MaxCallbackSize);
        static_assert(std::is_invocable_r_v<void, FinishCallback, duration>);
        if constexpr (std::is_same_v<FinishCallback, callback_ref_t>)
        {
            new (&impl::template callback<stored_callback_t>(data_))
                stored_callback_t(aCallback);
        }
        else
        {
            new (&impl::template callback<stored_callback_t>(data_))
                stored_callback_t(std::forward<FinishCallback>(aCallback));
        }
    }

    timer() = delete;
    timer(const timer &) = delete;
    timer(timer &&aOther) noexcept
        : start_(aOther.start_), callback_(aOther.callback_)
    {
        impl::template copy<MaxCallbackSize>(data_, aOther.data_);
        aOther.callback_ = nullptr;
    }
    timer &operator=(const timer &) = delete;
    timer &operator=(timer &&aOther) noexcept
    {
        start_ = aOther.start_;
        callback_ = aOther.callback_;
        aOther.callback_ = nullptr;
        impl::template copy<MaxCallbackSize>(data_, aOther.data_);
        return *this;
    }

    duration leaked() const noexcept { return clock::now() - start_; }

    auto leaked_sec() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::seconds>(leaked());
    }

    auto leaked_milli_sec() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(leaked());
    }

    auto leaked_micro_sec() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(leaked());
    }

    auto leaked_nano_sec() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(leaked());
    }

    time_point start_tp() const noexcept { return start_; }

    void reset() noexcept { start_ = clock::now(); }

   private:
    time_point start_ = clock::now();
    typename impl::callback_t callback_{};
    alignas(MaxCallbackAlignment) std::byte data_[MaxCallbackSize]{};
};
}  // namespace details

using timer = details::timer<alignof(void (*)(int &)), sizeof(void (*)(int &)),
                             std::chrono::steady_clock>;
}  // namespace utils

#endif /* utils_timer_h */
