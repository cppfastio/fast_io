#pragma once

namespace fast_io
{

enum class access_how:std::uint_fast8_t
{
f_ok=0,
x_ok=1,
w_ok=2,
r_ok=4
};

constexpr access_how operator&(access_how x, access_how y) noexcept
{
using utype = typename std::underlying_type<access_how>::type;
return static_cast<access_how>(static_cast<utype>(x) & static_cast<utype>(y));
}

constexpr access_how operator|(access_how x, access_how y) noexcept
{
using utype = typename std::underlying_type<access_how>::type;
return static_cast<access_how>(static_cast<utype>(x) | static_cast<utype>(y));
}

constexpr access_how operator^(access_how x, access_how y) noexcept
{
using utype = typename std::underlying_type<access_how>::type;
return static_cast<access_how>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

constexpr access_how operator~(access_how x) noexcept
{
using utype = typename std::underlying_type<access_how>::type;
return static_cast<access_how>(~static_cast<utype>(x));
}

inline constexpr access_how& operator&=(access_how& x, access_how y) noexcept{return x=x&y;}

inline constexpr access_how& operator|=(access_how& x, access_how y) noexcept{return x=x|y;}

inline constexpr access_how& operator^=(access_how& x, access_how y) noexcept{return x=x^y;}

}