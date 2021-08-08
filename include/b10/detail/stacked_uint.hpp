#ifndef B10_INCLUDED_DETAIL_STACKED_UINT_HPP
#define B10_INCLUDED_DETAIL_STACKED_UINT_HPP

#include <b10/detail/width_of.hpp>
#include <b10/detail/builtin_uint.hpp>

#include <b10/detail/thirdparty/hedley.h>

#include <bit>

namespace b10::detail {
namespace stacked_uint_detail {

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

} // namespace stacked_uint_detail

template <typename Half>
struct stacked_uint
: private stacked_uint_detail::storage<Half>
{
    using stacked_uint_detail::storage<Half>::hi;
    using stacked_uint_detail::storage<Half>::lo;

    stacked_uint()
    = default;

    template <typename T>
    HEDLEY_ALWAYS_INLINE constexpr
    stacked_uint(T x)
    requires { *this = x; }
    { *this = x; }

    template <typename XHalf>
    requires (width_of<XHalf> < width_of<Half>)
    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(stacked_uint<XHalf> x)
    -> stacked_uint<Half>&
    {
        lo = x;
        hi = 0;

        return *this;
    }

    template <builtin_unsigned_integral T>
    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(T x)
    -> stacked_uint<Width>&
    {
        lo = x;
        hi = 0;

        return *this;
    }

    template <typename RHalf>
    requires (width_of<RHalf> < width_of<Half>)
    HEDLEY_ALWAYS_INLINE constexpr
    operator stacked_uint<RHalf>() const {
        return lo;
    }

    template <builtin_unsigned_integral T>
    HEDLEY_ALWAYS_INLINE constexpr
    operator T() const {
        return lo;
    }
};

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto operator==(stacked_uint<Half> x, stacked_uint<Half> y)
-> bool
{ return x.hi == y.hi && x.lo == y.lo; }

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto operator!=(stacked_uint<Half> x, stacked_uint<Half> y)
-> bool
{ return x.hi != y.hi || x.lo != y.lo; }

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto operator<(stacked_uint<Half> x, stacked_uint<Half> y)
-> bool
{ return x.hi < y.hi || x.hi == y.hi && x.lo < y.lo; }

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto operator>(stacked_uint<Half> x, stacked_uint<Half> y)
-> bool
{ return x.hi > y.hi || x.hi == y.hi && x.lo > y.lo; }

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto operator<=(stacked_uint<Half> x, stacked_uint<Half> y)
-> bool
{ return x.hi < y.hi || x.hi == y.hi && x.lo <= y.lo; }

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto operator>=(stacked_uint<Half> x, stacked_uint<Half> y)
-> bool
{ return x.hi > y.hi || x.hi == y.hi && x.lo >= y.lo; }

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto operator~(stacked_uint<Half> x)
-> stacked_uint<Half>
{
    stacked_uint<Half> r;

    r.lo = ~x.lo;
    r.hi = ~x.hi;

    return r;
}

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto operator&(stacked_uint<Half> x, stacked_uint<Half> y)
-> stacked_uint<Half>
{
    stacked_uint<Half> r;

    r.lo = x.lo & y.lo;
    r.hi = x.hi & y.hi;

    return r;
}

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto operator|(stacked_uint<Half> x, stacked_uint<Half> y)
-> stacked_uint<Half>
{
    stacked_uint<Half> r;

    r.lo = x.lo | y.lo;
    r.hi = x.hi | y.hi;

    return r;
}

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto operator<<(stacked_uint<Half> x, std::size_t y)
-> stacked_uint<Half>
{
    stacked_uint<Half> r;

    if (y < width_of<Half>) {
        r.lo = x.lo << y;
        r.hi = (x.hi << y) | (x.lo >> (width_of<Half> - y));
    } else {
        r.lo = 0;
        r.hi = x.lo << (y - width_of<Half>);
    }

    return r;
}

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto operator>>(stacked_uint<Half> x, std::size_t y)
-> stacked_uint<Half>
{
    stacked_uint<Half> r;

    if (y < width_of<Half>) {
        r.lo = (r.hi << (width_of<Half> - y)) | (x.lo >> y);
        r.hi = x.hi >> y;
    } else {
        r.lo = r.hi >> (y - width_of<Half>);
        r.hi = 0;
    }

    return r;
}

namespace stacked_uint_detail {

template <builtin_unsigned_integral T>
HEDLEY_ALWAYS_INLINE constexpr
auto addc(T x, T y)
-> addc_result<T>
{
    T r = x + y;
    return {r, r < x};
}

template <builtin_unsigned_integral T>
HEDLEY_ALWAYS_INLINE constexpr
auto addc(T x, T y, bool c)
-> addc_result<T>
{
    auto [t, c1] = addc(x, y);
    auto [r, c2] = addc(t, c);
    return {r, c1 | c2};
}

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto addc(stacked_uint<Half> x, stacked_uint<Half> y)
-> addc_result<T>
{
    auto [r_lo, c1] = addc(x.lo, y.lo);
    auto [r_hi, c2] = addc(x.hi, y.hi, c1);
    return {cat(r_hi, r_lo), c2};
}

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto addc(stacked_uint<Half> x, stacked_uint<Half> y, bool c)
-> addc_result<T>
{
    auto [r_lo, c1] = addc(x.lo, y.lo, c);
    auto [r_hi, c2] = addc(x.hi, y.hi, c1);
    return {cat(r_hi, r_lo), c2};
}

} // namespace stacked_uint_detail

template <typename Half>
HEDLEY_ALWAYS_INLINE constexpr
auto operator+(stacked_uint<Half> x, stacked_uint<Half> y)
-> stacked_uint<Half>
{
    auto [r_lo, c] = addc(x.lo, y.lo);
    auto r_hi = x.hi + y.hi + c;
    return cat(r_hi, r_lo);
}

} // namespace b10::detail

#endif
