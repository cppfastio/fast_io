#pragma once

namespace fast_io::freestanding
{

/*
uninitialized_relocate requires two range are not overlapped.
*/
template <::std::input_or_output_iterator Iter1, ::std::input_or_output_iterator Iter2>
constexpr Iter2 uninitialized_relocate(Iter1 first, Iter1 last, Iter2 dest) noexcept
{
	if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1> && ::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate(::std::to_address(first), ::std::to_address(last),
									  ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1>)
	{
		return uninitialized_relocate(::std::to_address(first), ::std::to_address(last),
									  dest);
	}
	else if constexpr (::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate(first, last, ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else
	{
		using iter1valuetype = ::std::iter_value_t<Iter1>;
		using iter2valuetype = ::std::iter_value_t<Iter2>;
		if constexpr (::std::is_pointer_v<Iter1> && ::std::is_pointer_v<Iter2> &&
					  (::fast_io::freestanding::is_trivially_relocatable_v<iter1valuetype> &&
					   ::fast_io::freestanding::is_trivially_relocatable_v<iter2valuetype> &&
					   (::std::same_as<iter1valuetype, iter2valuetype> ||
						((::std::integral<iter1valuetype> || ::std::same_as<iter1valuetype, ::std::byte>) &&
						 (::std::integral<iter2valuetype> || ::std::same_as<iter2valuetype, ::std::byte>) &&
						 sizeof(iter1valuetype) == sizeof(iter2valuetype)))))
		{
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
			{
				return reinterpret_cast<Iter2>(::fast_io::freestanding::bytes_copy(reinterpret_cast<::std::byte const *>(first), reinterpret_cast<::std::byte const *>(last), reinterpret_cast<::std::byte *>(dest)));
			}
		}
		// we do not allow move constructor to throw EH.
		while (first != last)
		{
			::std::construct_at(dest, ::std::move(*first));
			if constexpr(!::std::is_trivially_destructible_v<iter1valuetype>)
			{
				if constexpr(::std::is_pointer_v<Iter1>)
				{
					::std::destroy_at(first);
				}
				else
				{
					::std::destroy_at(__builtin_addressof(*first));
				}
			}
			++first;
			++dest;
		}
		return dest;
	}
}

template <::std::input_or_output_iterator Iter1, ::std::input_or_output_iterator Iter2>
constexpr Iter2 uninitialized_move(Iter1 first, Iter1 last, Iter2 dest) noexcept
{
	if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1> && ::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_move(::std::to_address(first), ::std::to_address(last),
									  ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1>)
	{
		return uninitialized_move(::std::to_address(first), ::std::to_address(last),
									  dest);
	}
	else if constexpr (::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_move(first, last, ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else
	{
		using iter1valuetype = ::std::iter_value_t<Iter1>;
		using iter2valuetype = ::std::iter_value_t<Iter2>;
		if constexpr (::std::is_pointer_v<Iter1> && ::std::is_pointer_v<Iter2> &&
					  (::fast_io::freestanding::is_trivially_relocatable_v<iter1valuetype> &&
					   ::fast_io::freestanding::is_trivially_relocatable_v<iter2valuetype> &&
					   (::std::same_as<iter1valuetype, iter2valuetype> ||
						((::std::integral<iter1valuetype> || ::std::same_as<iter1valuetype, ::std::byte>) &&
						 (::std::integral<iter2valuetype> || ::std::same_as<iter2valuetype, ::std::byte>) &&
						 sizeof(iter1valuetype) == sizeof(iter2valuetype)))))
		{
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
			{
				return reinterpret_cast<Iter2>(::fast_io::freestanding::bytes_copy(reinterpret_cast<::std::byte const *>(first), reinterpret_cast<::std::byte const *>(last), reinterpret_cast<::std::byte *>(dest)));
			}
		}
		// we do not allow move constructor to throw EH.
		while (first != last)
		{
			::std::construct_at(dest, ::std::move(*first));
			++first;
			++dest;
		}
		return dest;
	}
}

template <::std::bidirectional_iterator Iter1, ::std::bidirectional_iterator Iter2>
constexpr Iter2 uninitialized_move_backward(Iter1 first, Iter1 last, Iter2 d_last) noexcept
{
	// we do not allow move constructor to throw EH.
	if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1> && ::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_move_backward(::std::to_address(first), ::std::to_address(last),
											   ::std::to_address(d_last)) -
			   ::std::to_address(d_last) + d_last;
	}
	else if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1>)
	{
		return uninitialized_move_backward(::std::to_address(first), ::std::to_address(last),
											   d_last);
	}
	else if constexpr (::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_move_backward(first, last, ::std::to_address(d_last)) -
			   ::std::to_address(d_last) + d_last;
	}
	else
	{
		using iter1valuetype = ::std::iter_value_t<Iter1>;
		using iter2valuetype = ::std::iter_value_t<Iter2>;
		if constexpr (::std::is_pointer_v<Iter1> && ::std::is_pointer_v<Iter2> &&
					  (::fast_io::freestanding::is_trivially_relocatable_v<iter1valuetype> &&
					   ::fast_io::freestanding::is_trivially_relocatable_v<iter2valuetype> &&
					   (::std::same_as<iter1valuetype, iter2valuetype> ||
						((::std::integral<iter1valuetype> || ::std::same_as<iter1valuetype, ::std::byte>) &&
						 (::std::integral<iter2valuetype> || ::std::same_as<iter2valuetype, ::std::byte>) &&
						 sizeof(iter1valuetype) == sizeof(iter2valuetype)))))
		{
#ifdef __cpp_if_consteval
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
			{
				auto d_start{d_last - (last-first)};
				::fast_io::freestanding::bytes_copy(reinterpret_cast<::std::byte const *>(first), reinterpret_cast<::std::byte const *>(last), reinterpret_cast<::std::byte *>(d_start));
				return d_start;
			}
		}
		while (first != last)
		{
			::std::construct_at(--d_last, std::move(*(--last)));
		}
		return d_last;
	}
}

} // namespace fast_io::freestanding
