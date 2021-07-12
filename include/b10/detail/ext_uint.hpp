#ifndef B10_INCLUDED_DETAIL_EXT_UINT_HPP
#define B10_INCLUDED_DETAIL_EXT_UINT_HPP

#include <b10/detail/config.hpp>
#include <b10/detail/always_inline.hpp>
#include <b10/detail/uint64.hpp>
#include <b10/detail/divmod_result.hpp>

#include <cstddef>

namespace b10::detail {
#if B10_DETAIL_HAVE_EXT_INT

#define B10_DETAIL_EXT_UINT_DEFINED 1

template <std::size_t Width>
using ext_uint = unsigned _ExtInt(Width);

B10_DETAIL_ALWAYS_INLINE
constexpr
auto wmul(uint64 x, uint64 y)
-> ext_uint<128>
{ return (ext_uint<128>)x * y; }

template <std::size_t Width>
B10_DETAIL_ALWAYS_INLINE
constexpr
auto wmul(ext_uint<Width> x, ext_uint<Width> y)
-> ext_uint<Width * 2>
{ return (ext_uint<Width * 2>)x * y; }

B10_DETAIL_ALWAYS_INLINE
constexpr
auto wdivmod(ext_uint<128> x, uint64 y)
-> divmod_result<uint64>
{ return {x / y, x % y}; }

template <std::size_t Width>
B10_DETAIL_ALWAYS_INLINE
constexpr
auto wdivmod(ext_uint<Width> x, ext_uint<Width / 2> y)
-> divmod_result<ext_uint<Width>>
{ return {x / y, x % y}; }

template <std::size_t Width>
B10_DETAIL_ALWAYS_INLINE
constexpr
auto divmod(ext_uint<Width> x, ext_uint<Width> y)
-> divmod_result<ext_uint<Width>>
{ return {x / y, x % y}; }

#endif // B10_DETAIL_HAVE_EXT_INT
} // namespace b10::detail

#endif // !defined(B10_INCLUDED_DETAIL_EXT_UINT_HPP)
