#ifndef B10_INCLUDED_DETAIL_WIDTH_OF_HPP
#define B10_INCLUDED_DETAIL_WIDTH_OF_HPP

#include <cstddef>
#include <climits>

namespace b10::detail {

template <typename T>
constexpr std::size_t width_of = sizeof(T) * CHAR_BIT;

} // namespace b10::detail

#endif
