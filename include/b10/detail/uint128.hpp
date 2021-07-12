#ifndef B10_INCLUDED_DETAIL_UINT128_HPP
#define B10_INCLUDED_DETAIL_UINT128_HPP

#include <b10/detail/config.hpp>
#include <b10/detail/always_inline.hpp>
#include <b10/detail/uint64.hpp>
#include <b10/detail/divmod_result.hpp>

namespace b10::detail {
#if B10_DETAIL_HAVE_INT128

using uint128 = unsigned __int128;

B10_DETAIL_ALWAYS_INLINE
constexpr
auto wmul(uint64 x, uint64 y)
-> uint128
{ return (uint128)x * y; }

B10_DETAIL_ALWAYS_INLINE
constexpr
auto wdivmod(uint128 x, uint64 y)
-> divmod_result<uint64>
{ return {x / y, x % y}; }

B10_DETAIL_ALWAYS_INLINE
constexpr
auto divmod(uint128 x, uint128 y)
-> divmod_result<uint128>
{ return {x / y, x % y}; }

#define B10_DETAIL_UINT128_DEFINED 1

#endif // B10_DETAIL_HAVE_INT128
} // namespace b10::detail

#endif // !defined(B10_INCLUDED_DETAIL_UINT128_HPP)
