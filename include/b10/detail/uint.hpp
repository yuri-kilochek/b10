#ifndef B10_INCLUDED_DETAIL_UINT_HPP
#define B10_INCLUDED_DETAIL_UINT_HPP

#include <b10/detail/builtin_uint.hpp>
#include <b10/detail/stacked_uint.hpp>

#include <cstddef>

namespace b10::detail {
namespace uint_detail {

template <std::size_t Width>
struct select;

} // namespace uint_detail

template <std::size_t Width>
using uint = uint_detail::select<Width>::type;

namespace uint_detail {

template <std::size_t Width>
requires requires { typename builtin_uint<Width>; }
struct select<Width> {
    using type = builtin_uint<Width>;
};

template <std::size_t Width>
requires (Width % 2u == 0u)
struct select {
    using type = stacked_uint<uint<Width / 2>>;
};

} // namespace uint_detail
} // namespace b10::detail

#endif
