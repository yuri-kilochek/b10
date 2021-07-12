#ifndef B10_INCLUDED_DETAIL_UINT32_HPP
#define B10_INCLUDED_DETAIL_UINT32_HPP

#include <b10/detail/always_inline.hpp>
#include <b10/detail/divmod_result.hpp>

#include <cstdint>

namespace b10::detail {

using uint32 = std::uint32_t;

B10_DETAIL_ALWAYS_INLINE
constexpr
auto divmod(uint32 x, uint32 y)
-> divmod_result<uint32>
{ return {x / y, x % y}; }

} // namespace b10::detail

#endif // !defined(B10_INCLUDED_DETAIL_UINT32_HPP)
