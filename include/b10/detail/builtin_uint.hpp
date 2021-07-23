#ifndef B10_INCLUDED_DETAIL_BUILTIN_UINT_HPP
#define B10_INCLUDED_DETAIL_BUILTIN_UINT_HPP

#include <b10/detail/have_ext_int.hpp>
#include <b10/detail/have_int128.hpp>

#include <cstddef>
#include <cstdint>

namespace b10::detail {
namespace builtin_uint_detail {

template <std::size_t Width>
struct select {
    #if B10_DETAIL_HAVE_EXT_INT
        using type = unsigned _ExtInt(Width);
    #endif
};

#if B10_DETAIL_HAVE_INT128 && !B10_DETAIL_HAVE_EXT_INT
    template <>
    struct select<128> {
        using type = unsigned __int128;
    };
#endif

template <>
struct select<64> {
    using type = std::uint64_t;
};

template <>
struct select<32> {
    using type = std::uint32_t;
};

} // namespace builtin_uint_detail

template <std::size_t Width>
using builtin_uint = builtin_uint_detail::select<Width>::type;

} // namespace b10::detail

#endif
