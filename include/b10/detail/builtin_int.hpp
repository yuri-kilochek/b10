#ifndef B10_INCLUDED_DETAIL_BUILTIN_INT_HPP
#define B10_INCLUDED_DETAIL_BUILTIN_INT_HPP

#include <b10/detail/have_ext_int.hpp>
#include <b10/detail/have_int128.hpp>

#include <cstddef>
#include <cstdint>

namespace b10::detail {
namespace builtin_int_detail {

template <std::size_t Width>
struct select;

template <>
struct select<32> {
    using unsigned_type = std::uint32_t;
    using signed_type = std::int32_t;
};

template <>
struct select<64> {
    using unsigned_type = std::uint64_t;
    using signed_type = std::int64_t;
};

#if B10_DETAIL_HAVE_INT128 && !B10_DETAIL_HAVE_EXT_INT
    template <>
    struct select<128> {
        using unsigned_type = unsigned __int128;
        using signed_type = __int128;
    };
#endif

#if B10_DETAIL_HAVE_EXT_INT
    template <std::size_t Width>
    struct select {
        using unsigned_type = unsigned _ExtInt(Width);
        using signed_type = _ExtInt(Width);
    };
#endif

} // namespace builtin_int_detail

template <std::size_t Width>
using unsigned_builtin_int = builtin_int_detail::select<Width>::unsigned_type;

template <std::size_t Width>
using signed_builtin_int = builtin_int_detail::select<Width>::signed_type;

} // namespace b10::detail

#endif