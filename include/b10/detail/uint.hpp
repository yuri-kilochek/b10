#ifndef B10_INCLUDED_DETAIL_UINT_HPP
#define B10_INCLUDED_DETAIL_UINT_HPP

#include <b10/detail/config.hpp>
#include <b10/detail/uint32.hpp>
#include <b10/detail/uint64.hpp>
#include <b10/detail/uint128.hpp>
#include <b10/detail/ext_uint.hpp>
#include <b10/detail/wide_uint.hpp>

#include <cstddef>

namespace b10::detail {
namespace uint_detail {

template <std::size_t Width>
struct selector;

} // namespace uint_detail

template <std::size_t Width>
using uint = uint_detail::selector<Width>::type;

namespace uint_detail {

template <>
struct selector<32> {
    using type = uint32;
};

template <>
struct selector<64> {
    using type = uint64;
};

#if B10_DETAIL_UINT128_DEFINED
    template <>
    struct selector<128> {
        using type = uint128;
    };
#endif

template <std::size_t Width>
struct selector {
    #if B10_DETAIL_EXT_UINT_DEFINED
        using type = ext_uint<Width>;
    #else
        using type = wide_uint<uint<Width / 2>>;
    #else
};

} // namespace uint_detail
} // namespace b10::detail

#endif // !defined(B10_INCLUDED_DETAIL_UINT_HPP)
