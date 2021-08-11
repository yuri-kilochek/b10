#ifndef B10_INCLUDED_DETAIL_STACKED_UINT_HPP
#define B10_INCLUDED_DETAIL_STACKED_UINT_HPP

#include <b10/detail/width_of.hpp>
#include <b10/detail/builtin_uint.hpp>
#include <b10/detail/have_builtin_add_overflow.hpp>

#include <b10/detail/thirdparty/hedley.h>

#include <bit>
#include <type_traits>

namespace b10::detail {
namespace stacked_uint_detail {

template <typename T, std::endian = std::endian::native>
struct storage;

template <typename T>
struct storage<T, std::endian::little> {
    T lo;
    T hi;
};

template <typename T>
struct storage<T, std::endian::big> {
    T hi;
    T lo;
};

} // namespace stacked_uint_detail

template <typename T>
struct stacked_uint
: private stacked_uint_detail::storage<T>
{
    using stacked_uint_detail::storage<T>::hi;
    using stacked_uint_detail::storage<T>::lo;

    stacked_uint()
    = default;

    HEDLEY_ALWAYS_INLINE constexpr
    stacked_uint(builtin_unsigned_integral auto x) {
        lo = x;
        hi = 0;
    }

    template <typename U>
    requires (width_of<U> < width_of<T>)
    HEDLEY_ALWAYS_INLINE constexpr
    stacked_uint(unsigned_uint<U> x) {
        lo = x;
        hi = 0;
    }

    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(builtin_unsigned_integral auto x)
    -> stacked_uint<T>&
    {
        lo = x;
        hi = 0;

        return *this;
    }

    template <typename U>
    requires (width_of<U> < width_of<T>)
    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(stacked_uint<U> x)
    -> stacked_uint<T>&
    {
        lo = x;
        hi = 0;

        return *this;
    }

    template <builtin_unsigned_integral U>
    HEDLEY_ALWAYS_INLINE constexpr
    operator U() const {
        return lo;
    }

    template <typename U>
    requires (width_of<U> < width_of<T>)
    HEDLEY_ALWAYS_INLINE constexpr
    operator stacked_uint<U>() const {
        return lo;
    }
};

namespace stacked_uint_detail {

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto lo(T x) {
    if constexpr(builtin_unsigned_integral<T>) {
        return builtin_uint<width_of<T> / 2>(x);
    } else {
        return x.lo;
    }
}

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto hi(T x) {
    if constexpr(builtin_unsigned_integral<T>) {
        return builtin_uint<width_of<T> / 2>(x >> (width_of<T> / 2));
    } else {
        return x.hi;
    }
}

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto cat(T hi, T lo) {
    if constexpr(requires { typename builtin_uint<width_of<T> * 2>; }) {
        return (builtin_uint<width_of<T> * 2>(hi) << width_of<T>) | lo;
    } else {
        stacked_uint<T> r;

        r.lo = lo;
        r.hi = hi;

        return r;
    }
}

} // namespace stacked_uint_detail

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator==(stacked_uint<T> x, stacked_uint<T> y)
-> bool
{ return x.hi == y.hi && x.lo == y.lo; }

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator!=(stacked_uint<T> x, stacked_uint<T> y)
-> bool
{ return x.hi != y.hi || x.lo != y.lo; }

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator<(stacked_uint<T> x, stacked_uint<T> y)
-> bool
{ return x.hi < y.hi || x.hi == y.hi && x.lo < y.lo; }

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator>(stacked_uint<T> x, stacked_uint<T> y)
-> bool
{ return x.hi > y.hi || x.hi == y.hi && x.lo > y.lo; }

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator<=(stacked_uint<T> x, stacked_uint<T> y)
-> bool
{ return x.hi < y.hi || x.hi == y.hi && x.lo <= y.lo; }

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator>=(stacked_uint<T> x, stacked_uint<T> y)
-> bool
{ return x.hi > y.hi || x.hi == y.hi && x.lo >= y.lo; }

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator~(stacked_uint<T> x)
-> stacked_uint<T>
{
    stacked_uint<T> r;

    r.lo = ~x.lo;
    r.hi = ~x.hi;

    return r;
}

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator&(stacked_uint<T> x, stacked_uint<T> y)
-> stacked_uint<T>
{
    stacked_uint<T> r;

    r.lo = x.lo & y.lo;
    r.hi = x.hi & y.hi;

    return r;
}

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator|(stacked_uint<T> x, stacked_uint<T> y)
-> stacked_uint<T>
{
    stacked_uint<T> r;

    r.lo = x.lo | y.lo;
    r.hi = x.hi | y.hi;

    return r;
}

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator<<(stacked_uint<T> x, std::size_t y)
-> stacked_uint<T>
{
    stacked_uint<T> r;

    if (y < width_of<T>) {
        r.lo = x.lo << y;
        r.hi = (x.hi << y) | (x.lo >> (width_of<T> - y));
    } else {
        r.lo = 0;
        r.hi = x.lo << (y - width_of<T>);
    }

    return r;
}

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator>>(stacked_uint<T> x, std::size_t y)
-> stacked_uint<T>
{
    stacked_uint<T> r;

    if (y < width_of<T>) {
        r.lo = (r.hi << (width_of<T> - y)) | (x.lo >> y);
        r.hi = x.hi >> y;
    } else {
        r.lo = r.hi >> (y - width_of<T>);
        r.hi = 0;
    }

    return r;
}

namespace stacked_uint_detail {

template <typename T>
struct addc_result {
    T value;
    bool carry;
};

template <builtin_unsigned_integral T>
HEDLEY_ALWAYS_INLINE constexpr
auto addc(T x, T y)
-> addc_result<T>
{
    #if B10_DETAIL_HAVE_BUILTIN_ADD_OVERFLOW
        if (!std::is_constant_evaluated()) {
            T r;
            bool c = __builtin_add_overflow(x, y, &r);
            return {r, c};
        }
    #endif

    auto r = x + y;
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

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto addc(stacked_uint<T> x, stacked_uint<T> y)
-> addc_result<T>
{
    auto [r_lo, c1] = addc(x.lo, y.lo);
    auto [r_hi, c2] = addc(x.hi, y.hi, c1);
    return {cat(r_hi, r_lo), c2};
}

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto addc(stacked_uint<T> x, stacked_uint<T> y, bool c)
-> addc_result<T>
{
    auto [r_lo, c1] = addc(x.lo, y.lo, c);
    auto [r_hi, c2] = addc(x.hi, y.hi, c1);
    return {cat(r_hi, r_lo), c2};
}

} // namespace stacked_uint_detail

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator+(stacked_uint<T> x, stacked_uint<T> y)
-> stacked_uint<T>
{
    using namespace stacked_uint_detail;

    auto [r_lo, c] = addc(x.lo, y.lo);
    auto r_hi = x.hi + y.hi + c;
    return cat(r_hi, r_lo);
}

} // namespace b10::detail

#endif
