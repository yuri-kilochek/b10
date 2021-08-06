#ifndef B10_INCLUDED_DETAIL_BUILTIN_UINT_HPP
#define B10_INCLUDED_DETAIL_BUILTIN_UINT_HPP

#include <b10/detail/have_ext_int.hpp>
#include <b10/detail/have_int128.hpp>
#include <b10/detail/width_of.hpp>
#include <b10/detail/divmod_result.hpp>

#include <b10/detail/thirdparty/hedley.h>

#include <cstddef>
#include <cstdint>
#include <concepts>

namespace b10::detail {
namespace builtin_uint_detail {

template <std::size_t Width>
struct select;

#if B10_DETAIL_HAVE_EXT_INT
    template <std::size_t Width>
    struct select {
        using type = unsigned _ExtInt(Width);
    };
#elif B10_DETAIL_HAVE_INT128
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

template <typename T>
concept builtin_unsigned_integral = std::same_as<T, builtin_uint<width_of<T>>>;

template <builtin_unsigned_integral T>
requires requires { typename builtin_uint<width_of<T> * 2>; }
HEDLEY_ALWAYS_INLINE constexpr
auto wmul(T x, T y)
-> builtin_uint<width_of<T> * 2>
{ return (builtin_uint<width_of<T> * 2>)x * y; }

template <builtin_unsigned_integral X, builtin_unsigned_integral Y>
requires (width_of<X> == 2 * width_of<Y>)
HEDLEY_ALWAYS_INLINE constexpr
auto wdivmod(X x, Y y)
-> divmod_result<Y>
{ return {x / y, x % y}; }

template <builtin_unsigned_integral T>
HEDLEY_ALWAYS_INLINE constexpr
auto divmod(T x, T y)
-> divmod_result<T>
{ return {x / y, x % y}; }

} // namespace b10::detail

#endif
