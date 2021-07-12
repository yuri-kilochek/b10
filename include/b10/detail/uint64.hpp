#ifndef B10_INCLUDED_DETAIL_UINT64_HPP
#define B10_INCLUDED_DETAIL_UINT64_HPP

#include <b10/detail/always_inline.hpp>
#include <b10/detail/uint32.hpp>
#include <b10/detail/divmod_result.hpp>

#include <cstdint>

namespace b10::detail {

using uint64 = std::uint64_t;

B10_DETAIL_ALWAYS_INLINE
constexpr
auto wmul(uint32 x, uint32 y)
-> uint64
{ return (uint64)x * y; }

B10_DETAIL_ALWAYS_INLINE
constexpr
auto wdivmod(uint64 x, uint32 y)
-> divmod_result<uint32>
{ return {x / y, x % y}; }

B10_DETAIL_ALWAYS_INLINE
constexpr
auto divmod(uint64 x, uint64 y)
-> divmod_result<uint64>
{ return {x / y, x % y}; }

} // namespace b10::detail

#endif // !defined(B10_INCLUDED_DETAIL_UINT64_HPP)
