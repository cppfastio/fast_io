#pragma once

enum class allocator_adapter_flags
{
none=0,
record_size=1,
secure_clear=2
};

constexpr allocator_adapter_flags operator&(allocator_adapter_flags x, allocator_adapter_flags y) noexcept
{
using utype = typename std::underlying_type<allocator_adapter_flags>::type;
return static_cast<allocator_adapter_flags>(static_cast<utype>(x) & static_cast<utype>(y));
}

constexpr allocator_adapter_flags operator|(allocator_adapter_flags x, allocator_adapter_flags y) noexcept
{
using utype = typename std::underlying_type<allocator_adapter_flags>::type;
return static_cast<allocator_adapter_flags>(static_cast<utype>(x) | static_cast<utype>(y));
}

constexpr allocator_adapter_flags operator^(allocator_adapter_flags x, allocator_adapter_flags y) noexcept
{
using utype = typename std::underlying_type<allocator_adapter_flags>::type;
return static_cast<allocator_adapter_flags>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

constexpr allocator_adapter_flags operator~(allocator_adapter_flags x) noexcept
{
using utype = typename std::underlying_type<allocator_adapter_flags>::type;
return static_cast<allocator_adapter_flags>(~static_cast<utype>(x));
}

inline constexpr allocator_adapter_flags& operator&=(allocator_adapter_flags& x, allocator_adapter_flags y) noexcept{return x=x&y;}

inline constexpr allocator_adapter_flags& operator|=(allocator_adapter_flags& x, allocator_adapter_flags y) noexcept{return x=x|y;}

inline constexpr allocator_adapter_flags& operator^=(allocator_adapter_flags& x, allocator_adapter_flags y) noexcept{return x=x^y;}
