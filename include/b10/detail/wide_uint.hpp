#ifndef B10_INCLUDED_DETAIL_WIDE_UINT_HPP
#define B10_INCLUDED_DETAIL_WIDE_UINT_HPP

#include <b10/detail/config.hpp>
#include <b10/detail/strong_inline.hpp>
#include <b10/detail/uint128.hpp>
#include <b10/detail/uint64.hpp>
#include <b10/detail/uint32.hpp>

#include <cstdint>
#include <climits>
#include <bit>

namespace b10::detail {
namespace wide_uint_detail {

template <typename H, std::endian = std::endian::native>
struct storage;

template <typename H>
struct storage<H, std::endian::little> {
    H lo;
    H hi;
};

template <typename H>
struct storage<H, std::endian::big> {
    H hi;
    H lo;
};

#if B10_DETAIL_UINT128_DEFINED
    using root_t = uint128;
    using halfroot_t = uint64;
#else
    using root_t = uint64;
    using halfroot_t = uint32;
#endif

B10_DETAIL_STRONG_INLINE
constexpr
auto hi(root_t x)
-> halfroot_t
{ return x >> (sizeof(halfroot_t) * CHAR_BIT); }

B10_DETAIL_STRONG_INLINE
constexpr
auto lo(root_t x)
-> halfroot_t
{ return x; }

B10_DETAIL_STRONG_INLINE
constexpr
auto cat(halfroot_t hi, halfroot_t lo)
-> root_t
{ return ((root_t)x << (sizeof(halfroot_t) * CHAR_BIT)) | lo; }

B10_DETAIL_STRONG_INLINE
constexpr
auto addc(root_t x, root_t y, bool& c)
-> root_t
{
    root_t r_lo = (root_t)lo(x) + lo(y) + c;
    c = hi(r_lo);
    root_t r_hi = (root_t)hi(x) + hi(y) + c;
    c = hi(r_hi);
    return cat(lo(r_hi), lo(r_lo));
}

B10_DETAIL_STRONG_INLINE
constexpr
auto subb(root_t x, root_t y, bool& b)
-> root_t
{
    root_t r_lo = (root_t)lo(x) - lo(y) - b;
    b = hi(r_lo);
    root_t r_hi = (root_t)hi(x) - hi(y) - b;
    b = hi(r_hi);
    return cat(lo(r_hi), lo(r_lo));
}

} // namespace wide_uint_detail

template <typename H>
struct wide_uint
: wide_uint_detail::storage<H>
{};

namespace wide_uint_detail {

template <typename H>
B10_DETAIL_STRONG_INLINE
constexpr
auto hi(wide_uint<H> x)
-> H
{ return x.hi; }

template <typename H>
B10_DETAIL_STRONG_INLINE
constexpr
auto lo(wide_uint<H> x)
-> H
{ return x.lo; }

template <typename H>
B10_DETAIL_STRONG_INLINE
constexpr
auto cat(H hi, H lo)
-> wide_uint<H>
{
    wide_uint<H> x;

    x.hi = hi;
    x.lo = lo;

    return x;
}

template <typename H>
B10_DETAIL_STRONG_INLINE
constexpr
auto addc(wide_uint<H> x, wide_uint<H> y, bool& c)
-> wide_uint<H>
{
    wide_uint<H> r;
    r.lo = addc(x.lo, y.lo, c);
    r.hi = addc(x.hi, y.hi, c);
    return r;
}

template <typename H>
B10_DETAIL_STRONG_INLINE
constexpr
auto subb(wide_uint<H> x, wide_uint<H> y, bool& b)
-> wide_uint<H>
{
    wide_uint<H> r;
    r.lo = subb(x.lo, y.lo, b);
    r.hi = subb(x.hi, y.hi, b);
    return r;
}

} // namespace wide_uint_detail

template <typename H>
B10_DETAIL_STRONG_INLINE
constexpr
auto wmul(H x, H y)
-> wide_uint<H>
{
    using namespace wide_uint_detail;

    H lolo = wmul(lo(x), lo(y));
    H lohi = wmul(lo(x), hi(y));
    H hilo = wmul(hi(x), lo(y));
    H hihi = wmul(hi(x), hi(y));

    bool m_c = 0;
    H m = addc(lohi, hilo, m_c);

    bool r_c = 0;
    auto r_lolo = lo(lolo);
    auto r_lohi = addc(hi(lolo), lo(m), r_c);
    auto r_hilo = addc(lo(hihi), hi(m), r_c);
    auto r_hihi = hi(hihi) + unsigned(m_c + r_c);

    return cat(cat(r_hihi, r_hilo), cat(r_lohi, r_lolo));
}

//template <typename H>
//B10_DETAIL_STRONG_INLINE
//constexpr
//auto wdivmod(wide_uint<H> x, H y)
//-> wdivmod_result<H>
//{
//
//}

} // namespace b10::detail

#endif // !defined(B10_INCLUDED_DETAIL_WIDE_UINT_HPP)
