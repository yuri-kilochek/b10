#ifndef B10_INCLUDED_DETAIL_DOUBLE_UINT_HPP
#define B10_INCLUDED_DETAIL_DOUBLE_UINT_HPP

#include <b10/detail/width_of.hpp>

#include <b10/detail/thirdparty/hedley.h>

#include <bit>
#include <concepts>

namespace b10::detail {

template <typename Half>
struct double_width_uint;

namespace double_width_uint_detail {

template <typename T>
constexpr bool is = false;

template <typename Half>
constexpr bool is<double_width_uint<Half>> = true;

template <typename Half, std::endian = std::endian::native>
struct storage;

template <typename Half>
struct storage<Half, std::endian::little> {
    Half lo;
    Half hi;
};

template <typename Half>
struct storage<Half, std::endian::big> {
    Half hi;
    Half lo;
};

} // namespace double_width_uint_detail

template <typename T>
concept double_width_unsigned_integral = double_width_uint_detail::is<T>;

template <typename Half>
struct double_width_uint
: double_width_uint_detail::storage<Half>
{
    double_width_uint()
    = default;

    template <typename T>
    HEDLEY_ALWAYS_INLINE constexpr
    double_width_uint(T x)
    requires { *this = x; }
    { *this = x; }

    template <typename OtherHalf>
    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(double_width_uint<OtherHalf> x)
    -> double_width_uint<Half>&
    {
        if constexpr(width_of<OtherHalf> < width_of<Half>) {
            lo = x;
            hi = 0;
        } else {
            *this = x.lo;
        }
        return *this;
    }

    template <std::convertible_to<Half> T>
    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(T x)
    -> double_width_uint<Width>&
    {
        lo = x;
        hi = -(x < 0);
        return *this;
    }

    template <typename T>
    requires std::convertible_to<Half, T>
    HEDLEY_ALWAYS_INLINE constexpr
    operator T() const {
        return lo;
    }
};

} // namespace b10::detail

#endif
