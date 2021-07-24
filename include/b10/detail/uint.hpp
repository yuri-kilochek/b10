#ifndef B10_INCLUDED_DETAIL_UINT_HPP
#define B10_INCLUDED_DETAIL_UINT_HPP

#include <b10/detail/builtin_uint.hpp>

#include <b10/detail/thirdparty/hedley.h>

#include <bit>
#include <cstddef>
#include <concepts>

namespace b10::detail {
namespace uint_detail {

template <typename ValueHalf, std::endian = std::endian::native>
struct storage;

template <typename ValueHalf>
struct storage<ValueHalf, std::endian::little> {
    ValueHalf low_value_half;
    ValueHalf high_value_half;
};

template <typename ValueHalf>
struct storage<ValueHalf, std::endian::big> {
    ValueHalf high_value_half;
    ValueHalf low_value_half;
};

} // namespace uint_detail

template <std::size_t Width>
struct uint
: uint_detail::storage<uint<Width / 2>>
{
    using value_half_type = uint<Width / 2>;

    uint()
    = default;

    template <typename T>
    HEDLEY_ALWAYS_INLINE constexpr
    uint(T x)
    requires { *this = x; }
    { *this = x; }

    template <std::size_t OtherWidth>
    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(uint<OtherWidth> other)
    -> uint<Width>&
    {
        if constexpr(OtherWidth < Width) {
            low_value_half = other;
            high_value_half = 0;
        } else {
            *this = other.low_value_half;
        }
        return *this;
    }

    template <std::convertible_to<value_half_type> T>
    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(T x)
    -> uint<Width>&
    {
        low_value_half = x;
        high_value_half = 0;
        return *this;
    }
};

template <std::size_t Width>
requires { typename builtin_uint<Width>; }
struct uint
{
    using value_type = builtin_uint<Width>;

    value_type value;

    uint()
    = default;

    template <typename T>
    HEDLEY_ALWAYS_INLINE constexpr
    uint(T x)
    requires { *this = x; }
    { *this = x; }

    template <std::size_t OtherWidth>
    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(uint<OtherWidth> other)
    -> uint<Width>&
    {
        if constexpr(requires { other.value; }) {
            value = other.value;
        } else {
            *this = other.low_value_half;
        }
        return *this;
    }

    template <std::convertible_to<value_type> T>
    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(T x)
    -> uint<Width>&
    {
        value = x; 
        return *this;
    }
};

} // namespace b10::detail

#endif
