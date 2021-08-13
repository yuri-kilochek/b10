#ifndef B10_INCLUDED_DETAIL_STACKED_UINT_HPP
#define B10_INCLUDED_DETAIL_STACKED_UINT_HPP

#include <b10/detail/width_of.hpp>
#include <b10/detail/builtin_uint.hpp>
#include <b10/detail/have_builtin_add_overflow.hpp>
#include <b10/detail/have_builtin_sub_overflow.hpp>

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
    stacked_uint(stacked_uint<U> x) {
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

template <builtin_unsigned_integral T>
HEDLEY_ALWAYS_INLINE constexpr
auto add(T x, T y, bool& c)
-> T
{
    #if B10_DETAIL_HAVE_BUILTIN_ADD_OVERFLOW
        if (!std::is_constant_evaluated()) {
            T t;
            bool c1 = __builtin_add_overflow(x, y, &t);
            T r;
            bool c2 = __builtin_add_overflow(t, c, &r);
            c = c1 | c2;
            return r;
        }
    #endif

    T t = x + y;
    bool c1 = t < x;
    T r = t + c;
    bool c2 = r < t;
    c = c1 | c2;
    return r;
}

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto add(stacked_uint<T> x, stacked_uint<T> y, bool& c)
-> stacked_uint<T>
{
    stacked_uint<T> r;

    r.lo = add(x.lo, y.lo, c);
    r.hi = add(x.hi, y.hi, c);

    return r;
}

} // namespace stacked_uint_detail

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator+(stacked_uint<T> x, stacked_uint<T> y)
-> stacked_uint<T>
{
    bool c = 0;
    return stacked_uint_detail::add(x, y, c);
}

namespace stacked_uint_detail {

template <builtin_unsigned_integral T>
HEDLEY_ALWAYS_INLINE constexpr
auto sub(T x, T y, bool& b)
-> T
{
    #if B10_DETAIL_HAVE_BUILTIN_SUB_OVERFLOW
        if (!std::is_constant_evaluated()) {
            T t;
            bool b1 = __builtin_sub_overflow(x, y, &t);
            T r;
            bool b2 = __builtin_sub_overflow(t, b, &r);
            b = b1 | b2;
            return r;
        }
    #endif

    T t = x - y;
    bool b1 = t > x;
    T r = t - b;
    bool b2 = r > t;
    b = b1 | b2;
    return r;
}

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto sub(stacked_uint<T> x, stacked_uint<T> y, bool& b)
-> stacked_uint<T>
{
    stacked_uint<T> r;

    r.lo = sub(x.lo, y.lo, c);
    r.hi = sub(x.hi, y.hi, c);

    return r;
}

} // namespace stacked_uint_detail

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto operator-(stacked_uint<T> x, stacked_uint<T> y)
-> stacked_uint<T>
{
    bool b = 0;
    return stacked_uint_detail::sub(x, y, b);
}

namespace stacked_uint_detail {

template <builtin_unsigned_integral T>
HEDLEY_ALWAYS_INLINE constexpr
auto lo(T x)
-> builtin_uint<width_of<T> / 2>
{ return x; }

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto lo(stacked_uint<T> x)
-> T
{ return x.lo; }

template <builtin_unsigned_integral T>
HEDLEY_ALWAYS_INLINE constexpr
auto hi(T x)
-> builtin_uint<width_of<T> / 2>
{ return x >> (width_of<T> / 2); }

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto hi(stacked_uint<T> x)
-> T
{ return x.hi; }

template <builtin_unsigned_integral T>
HEDLEY_ALWAYS_INLINE constexpr
auto cat(T hi, T lo)
-> builtin_uint<width_of<T> * 2>
{ return (builtin_uint<width_of<T> * 2>(hi) << width_of<T>) | lo; }

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto cat(T hi, T lo)
-> stacked_uint<T>
{
    stacked_uint<T> r;

    r.lo = lo;
    r.hi = hi;

    return r;
}

} // namespace stacked_uint_detail

template <typename T>
HEDLEY_ALWAYS_INLINE constexpr
auto mul(T x, T y)
-> stacked_uint<T>
{
    using namespace stacked_uint_detail;

    auto x_lo = lo(x);
    auto x_hi = hi(x);
    auto y_lo = lo(y);
    auto y_hi = hi(y);

    T a = mul(x_lo, y_lo);
    T b = mul(x_lo, y_hi);
    T c = mul(x_hi, y_lo) + hi(a) + lo(b);
    T d = mul(x_hi, y_hi) + hi(b) + hi(c);

    return cat(d, cat(lo(c), lo(a)));
}

} // namespace b10::detail

#endif
