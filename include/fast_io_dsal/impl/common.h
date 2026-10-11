#pragma once

namespace fast_io
{

namespace containers
{

inline constexpr ::std::size_t npos{::std::numeric_limits<::std::size_t>::max()};

}

using ::fast_io::containers::npos;

} // namespace fast_io

namespace fast_io::containers::details
{

/*
Reports a container contract violation (out of bounds access, size overflow,
invalid index range ...). When the allocator adapter carries the
throws_on_violations flag the violation is reported through herbceptions,
otherwise the program terminates.
*/
template <::fast_io::allocator_adapter_flags flags>
concept valid_view_flags =
	flags == ::fast_io::allocator_adapter_flags::none ||
	flags == ::fast_io::allocator_adapter_flags::throws_on_violations;

template <bool throwing>
[[noreturn]] inline constexpr void contract_violation_report(::std::errc ec) FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
{
	if constexpr (throwing)
	{
		::fast_io::herbceptions::throws_errc(ec);
	}
	else
	{
		::fast_io::fast_terminate();
	}
}

/*
Completeness-safe wrappers around the standard nothrow traits. Querying a
nothrow trait on an incomplete type is a hard error; guarding it with
sizeof(T) inside a constraint makes the wrapper simply report false so
containers can still be instantiated against recursive (incomplete) element
types. Specifications use the may_throw_* forms: an incomplete element type
is assumed not to throw so that declarations checked while the element type
is still being defined stay callable; the real check happens when the member
function body is instantiated.
*/
template <typename T>
concept complete_type = requires { sizeof(T); };

template <typename T, typename... Args>
concept may_throw_constructible = complete_type<T> && !::std::is_nothrow_constructible_v<T, Args...>;

template <typename T>
concept may_throw_default_constructible = complete_type<T> && !::std::is_nothrow_default_constructible_v<T>;

template <typename T>
concept may_throw_copy_constructible = complete_type<T> && !::std::is_nothrow_copy_constructible_v<T>;

template <typename T>
concept may_throw_move_constructible = complete_type<T> && !::std::is_nothrow_move_constructible_v<T>;

template <typename T>
concept may_throw_move_assignable = complete_type<T> && !::std::is_nothrow_assignable_v<T &, T>;

// destructors are never allowed to report failures; incomplete types pass the
// concept so recursive containers still instantiate
template <typename T>
concept never_throw_destructible = !complete_type<T> || ::std::is_nothrow_destructible_v<T>;

// constraint level short circuit keeps alignof(T) from being instantiated
// while T is still incomplete; incomplete types are treated as defaulted
template <typename T, ::std::size_t default_alignment>
concept defaulted_alignment = !complete_type<T> || alignof(T) <= default_alignment;

template <typename T>
concept copy_constructible = complete_type<T> && ::std::is_copy_constructible_v<T>;

template <typename handle>
concept is_trivally_stored_allocator_handle = ::fast_io::freestanding::is_zero_default_constructible_v<handle> &&
											  ::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<handle> &&
											  ::std::is_trivially_copy_constructible_v<handle> &&
											  sizeof(handle) <= sizeof(handle *) && alignof(handle) <= alignof(handle *);

template <typename handle>
struct handle_holder
{
	using handle_type = handle;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	::std::conditional_t<is_trivally_stored_allocator_handle<handle>, handle, handle *> value;
	inline constexpr handle_holder() noexcept
		: value{}
	{}
	inline constexpr handle_holder(decltype(nullptr)) noexcept = delete;
	inline constexpr handle_holder(handle small_object) noexcept
		requires(is_trivally_stored_allocator_handle<handle>)
		: value(small_object)
	{}
	inline constexpr handle_holder(handle *small_object) noexcept
		requires is_trivally_stored_allocator_handle<handle>
		: value(*small_object)
	{}
	template <typename A>
	inline constexpr handle_holder(A &&large_object) noexcept
		requires(::std::same_as<::std::remove_cvref_t<A>, handle> && !is_trivally_stored_allocator_handle<handle>)
	{
		value = ::fast_io::typed_generic_allocator_adapter<handle, handle>::handle_allocate(large_object, 1);
		::std::construct_at(value, ::std::forward<handle>(large_object));
	}
	inline constexpr handle_holder(handle *large_object) noexcept
		requires(!is_trivally_stored_allocator_handle<handle>)
		: value(large_object)
	{}
	inline constexpr handle_holder(handle_holder const &) noexcept = default;
	inline constexpr handle_holder &operator=(handle_holder const &) noexcept = default;
	inline constexpr handle_holder(handle_holder &&) noexcept = default;
	inline constexpr handle_holder &operator=(handle_holder &&) noexcept = default;
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr handle const &get() const noexcept
	{
		if constexpr (is_trivally_stored_allocator_handle<handle>)
		{
			return value;
		}
		else
		{
			return *value;
		}
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr handle &get() noexcept
	{
		if constexpr (is_trivally_stored_allocator_handle<handle>)
		{
			return value;
		}
		else
		{
			return *value;
		}
	}
};
template <::std::equality_comparable handle>
inline constexpr auto operator==(handle_holder<handle> left, handle_holder<handle> right) noexcept
{
	return left.get() == right.get();
}

/*
Re-wrapping an allocator through generic_allocator_adapter drops any
caller-provided adapter_flags (the wrap defaults to none); this alias
forwards the wrapped allocator's own adapter_flags (or none for a raw
allocator) so the requested die/try behaviour survives rewrapping.
*/
template <typename allocator>
using generic_allocator_adapter_preserving_flags =
	::fast_io::generic_allocator_adapter<allocator, ::fast_io::details::adapter_flags_or_default<allocator>()>;

template <typename allocator>
inline constexpr bool allocator_throws_on_allocation_failure{
	(::fast_io::details::adapter_flags_or_default<allocator>() & ::fast_io::allocator_adapter_flags::throws_on_allocation_failure) != ::fast_io::allocator_adapter_flags::none};
template <typename allocator>
inline constexpr bool allocator_throws_on_violations{
	(::fast_io::details::adapter_flags_or_default<allocator>() & ::fast_io::allocator_adapter_flags::throws_on_violations) != ::fast_io::allocator_adapter_flags::none};
template <typename allocator>
inline constexpr bool allocator_throws_on_overflow{
	(::fast_io::details::adapter_flags_or_default<allocator>() & ::fast_io::allocator_adapter_flags::throws_on_overflow) != ::fast_io::allocator_adapter_flags::none};
template <typename allocator>
inline constexpr bool allocator_throws_any{
	::fast_io::containers::details::allocator_throws_on_allocation_failure<allocator> ||
	::fast_io::containers::details::allocator_throws_on_violations<allocator> ||
	::fast_io::containers::details::allocator_throws_on_overflow<allocator>};

template <typename sztype>
inline constexpr sztype cal_grow_twice_size_size_based(sztype cap) noexcept
{
	constexpr sztype mx_value2{::std::numeric_limits<sztype>::max()};
	constexpr sztype mx_value{mx_value2};
	constexpr sztype mx_half_value{mx_value >> 1u};
	if (cap == mx_value)
	{
		::fast_io::fast_terminate();
	}
	else if (mx_half_value < cap)
	{
		return mx_value;
	}
	else if (!cap)
	{
		return 1u;
	}
	return static_cast<sztype>(cap << 1u);
}

template <::std::size_t size, bool trivial, bool throwing = false>
inline constexpr ::std::size_t cal_grow_twice_size(::std::size_t cap) FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
{
	constexpr ::std::size_t mx_value2{::std::numeric_limits<::std::size_t>::max() / size};
	constexpr ::std::size_t mx_value{trivial ? mx_value2 * size : mx_value2};
	constexpr ::std::size_t mx_half_value{mx_value >> 1u};
	if (cap == mx_value)
	{
		::fast_io::containers::details::contract_violation_report<throwing>(::std::errc::value_too_large);
	}
	else if (mx_half_value < cap)
	{
		return mx_value;
	}
	else if (!cap)
	{
		return size;
	}
	return static_cast<::std::size_t>(cap << 1);
}

template <::std::unsigned_integral T>
inline constexpr T reduce_sizet_to_small_sizetype(::std::size_t count) noexcept
{
	if constexpr (sizeof(::std::size_t) <= sizeof(T))
	{
		return static_cast<T>(count);
	}
	else
	{
		constexpr T mx{::std::numeric_limits<T>::max()};
		if (mx < count) [[unlikely]]
		{
			return mx;
		}
		return static_cast<T>(count);
	}
}

} // namespace fast_io::containers::details
