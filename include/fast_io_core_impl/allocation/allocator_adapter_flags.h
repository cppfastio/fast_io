#pragma once

enum class allocator_adapter_flags
{
	none = 0,
	secure_clear = 1,
	throws_on_allocation_failure = 2,    // by default we fail fast
	throws_on_violations = 4,            // by default we fail fast
	throws_on_overflow = 8,              // by default we fail fast
	prefer_big_allocation_on_stack = 16, // pick stack scratch (e.g. print) over heap
};

constexpr allocator_adapter_flags operator&(allocator_adapter_flags x, allocator_adapter_flags y) noexcept
{
	using utype = typename ::std::underlying_type<allocator_adapter_flags>::type;
	return static_cast<allocator_adapter_flags>(static_cast<utype>(x) & static_cast<utype>(y));
}

constexpr allocator_adapter_flags operator|(allocator_adapter_flags x, allocator_adapter_flags y) noexcept
{
	using utype = typename ::std::underlying_type<allocator_adapter_flags>::type;
	return static_cast<allocator_adapter_flags>(static_cast<utype>(x) | static_cast<utype>(y));
}

constexpr allocator_adapter_flags operator^(allocator_adapter_flags x, allocator_adapter_flags y) noexcept
{
	using utype = typename ::std::underlying_type<allocator_adapter_flags>::type;
	return static_cast<allocator_adapter_flags>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

constexpr allocator_adapter_flags operator~(allocator_adapter_flags x) noexcept
{
	using utype = typename ::std::underlying_type<allocator_adapter_flags>::type;
	return static_cast<allocator_adapter_flags>(~static_cast<utype>(x));
}

inline constexpr allocator_adapter_flags &operator&=(allocator_adapter_flags &x, allocator_adapter_flags y) noexcept
{
	return x = x & y;
}

inline constexpr allocator_adapter_flags &operator|=(allocator_adapter_flags &x, allocator_adapter_flags y) noexcept
{
	return x = x | y;
}

inline constexpr allocator_adapter_flags &operator^=(allocator_adapter_flags &x, allocator_adapter_flags y) noexcept
{
	return x = x ^ y;
}
