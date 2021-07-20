#ifndef B10_INCLUDED_DETAIL_UINT_HPP
#define B10_INCLUDED_DETAIL_UINT_HPP

#include <b10/detail/have_int128.hpp>
#include <b10/detail/have_ext_int.hpp>
#include <b10/detail/wide_uint.hpp>
#include <b10/detail/width_of.hpp>
#include <b10/detail/have_builtin_add_overflow.hpp>
#include <b10/detail/have_addcarry_u64.hpp>
#include <b10/detail/addc_result.hpp>
#include <b10/detail/have_builtin_sub_overflow.hpp>
#include <b10/detail/have_subborrow_u64.hpp>
#include <b10/detail/subb_result.hpp>
#include <b10/detail/divmod_result.hpp>

#include <b10/detail/thirdparty/hedley.h>

#include <cstddef>
#include <cstdint>

namespace b10::detail {
namespace uint_detail {

template <std::size_t Width>
struct selector;

} // namespace uint_detail

template <std::size_t Width>
using uint = uint_detail::selector<Width>::type;

namespace uint_detail {

template <typename T>
struct wide;

template <>
struct selector<32> {
    using type = std::uint32_t;
};

template <>
struct selector<64> {
    using type = std::uint64_t;
};

#if B10_DETAIL_HAVE_INT128 && !B10_DETAIL_HAVE_EXT_INT
    template <>
    struct selector<128> {
        using type = unsigned __int128;
    };
#endif

template <std::size_t Width>
struct selector {
    #if B10_DETAIL_HAVE_EXT_INT
        using type = unsigned _ExtInt(Width);
    #else
        using type = wide<uint<Width / 2>>;
    #endif
};

template <typename T, std::endian = std::endian::native>
struct wide_storage;

template <typename T>
struct wide_storage<T, std::endian::little> {
    T lo;
    T hi;
};

template <typename T>
struct wide_storage<T, std::endian::big> {
    T hi;
    T lo;
};

template <typename T>
struct wide
: wide_storage<T>
{};

template <typename T>
struct is_wide {
    static constexpr bool value = false;
};

template <typename T>
struct is_wide<wide<T>> {
    static constexpr bool value = true;
};

template <typename T>
constexpr bool is_wide_v = is_wide<T>::value;

template <typename T>
using narrower = uint<width_of<T> / 2>;

template <typename T>
using wider = uint<width_of<T> * 2>;

template <typename T>
HEDLEY_ALWAYS_INLINE
constexpr
auto hi(T x)
-> narrower<T>
{
    if constexpr(is_wide_v<T>) {
        return x.hi;
    } else {
        return x >> (width_of<T> / 2);
    }
}

template <typename T>
HEDLEY_ALWAYS_INLINE
constexpr
auto lo(T x)
-> narrower<T>
{
    if constexpr(is_wide_v<T>) {
        return x.lo;
    } else {
        return x;
    }
}

template <typename T>
HEDLEY_ALWAYS_INLINE
constexpr
auto cat(T hi, T lo)
-> wider<T>
{
    if constexpr(is_wide_v<wider<T>>) {
        wider<T> x;

        x.hi = hi;
        x.lo = lo;

        return x;
    } else {
        return ((wider<T>)hi << width_of<T>) | lo;
    }
}

template <typename T>
HEDLEY_ALWAYS_INLINE
constexpr
auto addc(T x, T y)
-> addc_result<T>
{
    if constexpr(is_wide_v<T>) {
        auto [r_lo, c1] = addc(x.lo, y.lo);
        auto [r_hi, c2] = addc(x.hi, y.hi, c1);
        return {cat(r_hi, r_lo), c2};
    } else {
        #if B10_DETAIL_HAVE_BUILTIN_ADD_OVERFLOW
            if (!std::is_constant_evaluated()) {
                T r;
                bool c = __builtin_add_overflow(x, y, &r);
                return {r, c};
            }
        #elif B10_DETAIL_HAVE_ADDCARRY_U64
            if constexpr(width_of<T> == 64) {
                if (!std::is_constant_evaluated()) {
                    unsigned long long r;
                    c = _addcarry_u64(0, x, y, &r);
                    return r;
                }
            }
        #endif

        T r = x + y;
        return {r, r < x};
    }
}

template <typename T>
HEDLEY_ALWAYS_INLINE
constexpr
auto addc(T x, T y, bool c)
-> addc_result<T>
{
    if constexpr(is_wide_v<T>) {
        auto [r_lo, c1] = addc(x.lo, y.lo, c);
        auto [r_hi, c2] = addc(x.hi, y.hi, c1);
        return {cat(r_hi, r_lo), c2};
    } else {
        #if B10_DETAIL_HAVE_ADDCARRY_U64
            if constexpr(width_of<T> == 64) {
                if (!std::is_constant_evaluated()) {
                    unsigned long long r;
                    c = _addcarry_u64(c, x, y, &r);
                    return r;
                }
            }
        #endif

        auto [t, c1] = addc(x, y);
        auto [r, c2] = addc(t, (T)c);
        return {r, c1 | c2};
    }
}

template <typename T>
HEDLEY_ALWAYS_INLINE
constexpr
auto subb(T x, T y)
-> subb_result<T>
{
    if constexpr(is_wide_v<T>) {
        auto [r_lo, b1] = subb(x.lo, y.lo);
        auto [r_hi, b2] = subb(x.hi, y.hi, b1);
        return {cat(r_hi, r_lo), b2};
    } else {
        #if B10_DETAIL_HAVE_BUILTIN_SUB_OVERFLOW
            if (!std::is_constant_evaluated()) {
                T r;
                bool c = __builtin_sub_overflow(x, y, &r);
                return {r, c};
            }
        #elif B10_DETAIL_HAVE_SUBBORROW_U64
            if constexpr(width_of<T> == 64) {
                if (!std::is_constant_evaluated()) {
                    unsigned long long r;
                    b = _subborrow_u64(0, x, y, &r);
                    return r;
                }
            }
        #endif

        T r = x - y;
        return {r, r > x};
    }
}

template <typename T>
HEDLEY_ALWAYS_INLINE
constexpr
auto subb(T x, T y, bool b)
-> subb_result<T>
{
    if constexpr(is_wide_v<T>) {
        auto [r_lo, b1] = subb(x.lo, y.lo, b);
        auto [r_hi, b2] = subb(x.hi, y.hi, b1);
        return {cat(r_hi, r_lo), b2};
    } else {
        #if B10_DETAIL_HAVE_SUBBORROW_U64
            if constexpr(width_of<T> == 64) {
                if (!std::is_constant_evaluated()) {
                    unsigned long long r;
                    b = _subborrow_u64(b, x, y, &r);
                    return r;
                }
            }
        #endif

        auto [t, b1] = subb(x, y);
        auto [r, b2] = subb(t, (T)b);
        return {r, b1 | b2};
    }
}

} // namespace uint_detail

template <typename T>
HEDLEY_ALWAYS_INLINE
constexpr
auto wmul(T x, T y)
-> uint_detail::wider<T>
{
    using namespace uint_detail;
    if constexpr(is_wide_v<wider<T>>) {
        #if B10_DETAIL_HAVE_UMUL128
            if constexpr(width_of<T> == 64) {
                if (!std::is_constant_evaluated()) {
                    unsigned long long r_hi;
                    T r_lo = _umul128(x, y, &r_hi);
                    return cat(r_hi, r_lo);
                }
            }
        #endif

        T lolo = wmul(lo(x), lo(y));
        T lohi = wmul(lo(x), hi(y));
        T hilo = wmul(hi(x), lo(y));
        T hihi = wmul(hi(x), hi(y));

        auto [t, t_c] = addc(lohi, hilo);
        auto [m, m_c] = addc(cat(lo(hihi), hi(lolo)), t);

        auto r_lolo = lo(lolo);
        auto r_lohi = lo(m);
        auto r_hilo = hi(m);
        auto r_hihi = hi(hihi) + unsigned(t_c + m_c);

        return cat(cat(r_hihi, r_hilo), cat(r_lohi, r_lolo));
    } else {
        return (wider<T>)x * y;
    }
}

template <typename T>
HEDLEY_ALWAYS_INLINE
constexpr
auto wdivmod(T x, uint<width_of<T> / 2> y)
-> divmod_result<uint<width_of<T> / 2>>
{ return {x / y, x % y}; }

template <typename T>
HEDLEY_ALWAYS_INLINE
constexpr
auto divmod(T x, T y)
-> divmod_result<T>
{ return {x / y, x % y}; }

} // namespace b10::detail

#endif // !defined(B10_INCLUDED_DETAIL_UINT_HPP)
