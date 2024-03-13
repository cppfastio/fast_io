#pragma once

namespace fast_io::containers::details
{
template <typename handle>
concept is_trivally_stored_allocator_handle = ::fast_io::freestanding::is_zero_default_constructible_v<handle> &&
											  ::fast_io::freestanding::is_trivially_relocatable_v<handle> &&
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
	constexpr handle_holder() noexcept
		: value{}
	{}
	constexpr handle_holder(decltype(nullptr)) noexcept = delete;
	constexpr handle_holder(handle small) noexcept
		requires(is_trivally_stored_allocator_handle<handle>)
		: value(small)
	{}
	constexpr handle_holder(handle *small) noexcept
		requires is_trivally_stored_allocator_handle<handle>
		: value(*small)
	{}
	template <typename A>
	constexpr handle_holder(A &&large) noexcept
		requires(::std::same_as<::std::remove_cvref_t<A>, handle> && !is_trivally_stored_allocator_handle<handle>)
	{
		value = ::fast_io::typed_generic_allocator_adapter<handle, handle>::handle_allocate(large, 1);
		::std::construct_at(value, ::std::forward<handle>(large));
	}
	constexpr handle_holder(handle *large) noexcept
		requires(!is_trivally_stored_allocator_handle<handle>)
		: value(large)
	{}
	constexpr handle_holder(handle_holder const &) noexcept = default;
	constexpr handle_holder &operator=(handle_holder const &) noexcept = default;
	constexpr handle_holder(handle_holder &&) noexcept = default;
	constexpr handle_holder &operator=(handle_holder &&) noexcept = default;
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

template <::std::size_t size, bool trivial>
inline constexpr ::std::size_t cal_grow_twice_size(::std::size_t cap) noexcept
{
	constexpr ::std::size_t mx_value2{::std::numeric_limits<::std::size_t>::max() / size};
	constexpr ::std::size_t mx_value{trivial ? mx_value2 * size : mx_value2};
	constexpr ::std::size_t mx_half_value{mx_value >> 1u};
	if (cap == mx_value)
	{
		::fast_io::fast_terminate();
	}
	else if (cap > mx_half_value)
	{
		if constexpr (trivial)
		{
			return mx_value;
		}
		else
		{
			return 1;
		}
	}
	else if (cap == 0)
	{
		return size;
	}
	return static_cast<::std::size_t>(cap << 1);
}

template<::std::bidirectional_iterator Iter1, ::std::bidirectional_iterator Iter2>
constexpr void move_backward_construct(Iter1 first, Iter1 last, Iter2 d_last)
{
	if constexpr(::std::contiguous_iterator<Iter1>&&!::std::is_pointer_v<Iter1>&&::std::contiguous_iterator<Iter2>&&!::std::is_pointer_v<Iter2>)
	{
		move_backward_construct(::std::to_address(first),::std::to_address(last),
			::std::to_address(d_last));
	}
	else if constexpr(::std::contiguous_iterator<Iter1>&&!::std::is_pointer_v<Iter1>)
	{
		move_backward_construct(::std::to_address(first),::std::to_address(last),
			d_last);
	}
	else if constexpr(::std::contiguous_iterator<Iter2>&&!::std::is_pointer_v<Iter2>)
	{
		move_backward_construct(first,last,
			::std::to_address(d_last));
	}
	else
	{
		using iter1valuetype = ::std::iter_value_t<Iter1>;
		using iter2valuetype = ::std::iter_value_t<Iter2>;
		if constexpr(::std::is_pointer_v<Iter1>&&::std::is_pointer_v<Iter2>&&
			::std::same_as<iter1valuetype,iter2valuetype>&&::std::is_trivially_copyable_v<iter1valuetype>)
		{
#ifdef __cpp_if_consteval
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
			{
				::std::size_t n{static_cast<::std::size_t>(last-first)};
				if(n) [[likely]]
				{
#if defined(__GNUC__) || defined(__clang__)
					__builtin_memmove
#else
					::std::memmove
#endif
					(::std::to_address(first), ::std::to_address(d_last)-n, n*sizeof(iter1valuetype));
				}
				return;
			}
		}
		auto last2{last};
		for(;first!=last&&last2!=d_last;)
		{
			::std::construct_at(--d_last, std::move(*(--last)));
		}
		::std::move_backward(first,last,d_last);
	}
}

} // namespace fast_io::containers::details