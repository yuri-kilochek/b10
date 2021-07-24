#ifndef B10_INCLUDED_DETAIL_INT_HPP
#define B10_INCLUDED_DETAIL_INT_HPP

#include <b10/detail/builtin_int.hpp>

#include <b10/detail/thirdparty/hedley.h>

#include <bit>
#include <cstddef>
#include <concepts>

namespace b10::detail {
namespace int_detail {

template <typename Half, std::endian = std::endian::native>
struct storage;

template <typename Half>
struct storage<Half, std::endian::little> {
    Half low_half;
    Half high_half;
};

template <typename Half>
struct storage<Half, std::endian::big> {
    Half high_half;
    Half low_half;
};

} // namespace int_detail

template <std::size_t Width>
struct unsigned_int
: int_detail::storage<unsigned_int<Width / 2>>
{
    using half_type = unsigned_int<Width / 2>;

    unsigned_int()
    = default;

    template <typename T>
    HEDLEY_ALWAYS_INLINE constexpr
    unsigned_int(T x)
    requires { *this = x; }
    { *this = x; }

    template <std::size_t OtherWidth>
    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(unsigned_int<OtherWidth> other)
    -> unsigned_int<Width>&
    {
        if constexpr(OtherWidth < Width) {
            low_half = other;
            high_half = 0;
        } else {
            *this = other.low_half;
        }
        return *this;
    }

    // TODO: assignment from signed_int

    template <std::convertible_to<half_type> T>
    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(T x)
    -> unsigned_int<Width>&
    {
        low_half = x;
        high_half = -(x < 0);
        return *this;
    }

    template <typename T>
    requires std::convertible_to<half_type, T>
    HEDLEY_ALWAYS_INLINE constexpr
    operator T() const {
        return low_half;
    }
};

template <std::size_t Width>
requires { typename unsigned_builtin_int<Width>; }
struct unsigned_int {
    using builtin_type = unsigned_builtin_int<Width>;

    builtin_type builtin;

    unsigned_int()
    = default;

    template <typename T>
    HEDLEY_ALWAYS_INLINE constexpr
    unsigned_int(T x)
    requires { *this = x; }
    { *this = x; }

    template <std::size_t OtherWidth>
    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(unsigned_int<OtherWidth> other)
    -> unsigned_int<Width>&
    {
        if constexpr(requires { other.builtin; }) {
            builtin = other.builtin;
        } else {
            *this = other.low_half;
        }
        return *this;
    }
    
    // TODO: assignment from signed_int

    template <std::convertible_to<builtin_type> T>
    HEDLEY_ALWAYS_INLINE constexpr
    auto operator=(T x)
    -> unsigned_int<Width>&
    {
        builtin = x; 
        return *this;
    }

    template <typename T>
    requires std::convertible_to<builtin_type, T>
    HEDLEY_ALWAYS_INLINE constexpr
    operator T() const {
        return builtin;
    }
};

} // namespace b10::detail

#endif
