#pragma once

namespace fast_io::operations::defines
{
template <typename T>
concept has_uninitialized_relocate_define = requires(T *ptr) {
	{ uninitialized_relocate_define(ptr, ptr, ptr) } -> ::std::same_as<T *>;
};

template <typename T>
concept has_uninitialized_move_define = requires(T *ptr) {
	{ uninitialized_move_define(ptr, ptr, ptr) } -> ::std::same_as<T *>;
};

template <typename T>
concept has_uninitialized_move_backward_define = requires(T *ptr) {
	{ uninitialized_move_backward_define(ptr, ptr, ptr) } -> ::std::same_as<T *>;
};

} // namespace fast_io::operations::defines

namespace fast_io::freestanding
{

template <::std::random_access_iterator Iter1, ::std::random_access_iterator Iter2>
	requires ::std::is_trivially_constructible_v<Iter2>
inline constexpr Iter2 overlapped_copy(Iter1 first, Iter1 last, Iter2 dest) noexcept
{
	if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1> && ::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return overlapped_copy(::std::to_address(first), ::std::to_address(last),
							   ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1>)
	{
		return overlapped_copy(::std::to_address(first), ::std::to_address(last),
							   dest);
	}
	else if constexpr (::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return overlapped_copy(first, last, ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else
	{
		using iter1valuetype = ::std::iter_value_t<Iter1>;
		using iter2valuetype = ::std::iter_value_t<Iter2>;
		if constexpr (::std::is_pointer_v<Iter1> && ::std::is_pointer_v<Iter2> &&
					  (::std::is_trivially_copyable_v<iter1valuetype> &&
					   ::std::is_trivially_copyable_v<iter2valuetype> &&
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

	if (__builtin_is_constant_evaluated())
		{
			::fast_io::details::overlapped_copy_buffer_ptr<iter2valuetype> tempbuffer(static_cast<::std::size_t>(::std::distance(first, last)));
			auto buffered{::std::copy(first, last, tempbuffer.ptr)};
			return ::std::move(tempbuffer.ptr, buffered, dest);
		}
		else
		{
			// we do not allow move constructor to throw EH.
			if (first <= dest && dest < last)
			{
				auto res{dest + (last - first)};
				::std::copy_backward(first, last, res);
				return res;
			}
			else
			{
				return ::std::copy(first, last, dest);
			}
		}
	}
}

template <::std::random_access_iterator Iter1, ::std::random_access_iterator Iter2>
inline constexpr Iter2 overlapped_copy_n(Iter1 first, ::std::size_t n, Iter2 dest) noexcept
{
	return overlapped_copy(first, first + n, dest);
}

/*
uninitialized_relocate requires two range are not overlapped.
*/
template <::std::input_or_output_iterator Iter1, ::std::input_or_output_iterator Iter2>
inline constexpr Iter2 uninitialized_relocate(Iter1 first, Iter1 last, Iter2 dest) noexcept
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
					  (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<iter1valuetype> &&
					   ::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<iter2valuetype> &&
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
		else if constexpr (::std::same_as<iter1valuetype, iter2valuetype> && ::fast_io::operations::defines::has_uninitialized_relocate_define<iter1valuetype>)
		{
			return uninitialized_relocate_define(first, last, dest);
		}
		// we do not allow move constructor to throw EH.
		while (first != last)
		{
			::std::construct_at(dest, ::std::move(*first));
			if constexpr (!::std::is_trivially_destructible_v<iter1valuetype>)
			{
				if constexpr (::std::is_pointer_v<Iter1>)
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
inline constexpr Iter2 uninitialized_move(Iter1 first, Iter1 last, Iter2 dest) noexcept
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
					  (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<iter1valuetype> &&
					   ::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<iter2valuetype> &&
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
		else if constexpr (::std::same_as<iter1valuetype, iter2valuetype> && ::fast_io::operations::defines::has_uninitialized_move_backward_define<iter1valuetype>)
		{
			return uninitialized_move_define(first, last, dest);
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
inline constexpr Iter2 uninitialized_move_backward(Iter1 first, Iter1 last, Iter2 d_last) noexcept
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
					  (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<iter1valuetype> &&
					   ::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<iter2valuetype> &&
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
				auto d_start{d_last - (last - first)};
				::fast_io::freestanding::bytes_copy(reinterpret_cast<::std::byte const *>(first), reinterpret_cast<::std::byte const *>(last), reinterpret_cast<::std::byte *>(d_start));
				return d_start;
			}
		}
		else if constexpr (::std::same_as<iter1valuetype, iter2valuetype> && ::fast_io::operations::defines::has_uninitialized_move_backward_define<iter1valuetype>)
		{
			return uninitialized_move_backward_define(first, last, d_last);
		}
		while (first != last)
		{
			::std::construct_at(--d_last, std::move(*(--last)));
		}
		return d_last;
	}
}

template <::std::input_or_output_iterator Iter, typename T>
inline constexpr Iter uninitialized_fill(Iter first, Iter last, T const &ele)
{
	using itervaluetype = ::std::iter_value_t<Iter>;
	if constexpr (::std::contiguous_iterator<itervaluetype>)
	{
		if constexpr (::std::is_trivially_copyable_v<itervaluetype> &&
					  ::std::is_scalar_v<itervaluetype> && sizeof(itervaluetype) == 1)
		{
#ifdef __cpp_if_consteval
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
			{
#if FAST_IO_HAS_BUILTIN(__builtin_memset)
				__builtin_memset
#else
				::std::memset
#endif
					(::std::to_address(first),
					 static_cast<itervaluetype>(ele),
					 static_cast<::std::size_t>(last - first));
				return last;
			}
		}
	}
	for (; first != last; ++first)
	{
		::std::construct_at(__builtin_addressof(*first), ele);
	}
	return last;
}

template <::std::input_or_output_iterator Iter, typename T>
inline constexpr Iter uninitialized_fill_n(Iter first, ::std::size_t n, T const &ele)
{
	return ::fast_io::freestanding::uninitialized_fill(first, first + n, ele);
}

} // namespace fast_io::freestanding
