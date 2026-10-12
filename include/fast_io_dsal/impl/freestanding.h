#pragma once

namespace fast_io::operations::defines
{

template <typename Iter1, typename Snt, typename Iter2>
struct memory_algorithm_define_type
{
	explicit constexpr memory_algorithm_define_type() noexcept = default;
};

template <typename Iter1, typename Snt, typename Iter2>
inline constexpr memory_algorithm_define_type<Iter1, Snt, Iter2> memory_algorithm_define{};

template <typename Iter1, typename Snt, typename Iter2>
concept has_uninitialized_relocate_define = ::std::sentinel_for<Snt, Iter1> && requires(Iter1 first, Snt last, Iter2 dest) {
	{ uninitialized_relocate_define(::fast_io::operations::defines::memory_algorithm_define<Iter1, Snt, Iter2>, first, last, dest) } -> ::std::same_as<Iter2>;
};

template <typename Iter1, typename Snt, typename Iter2>
concept has_uninitialized_relocate_backward_define = ::std::sentinel_for<Snt, Iter1> && requires(Iter1 first, Snt last, Iter2 dest) {
	{ uninitialized_relocate_backward_define(::fast_io::operations::defines::memory_algorithm_define<Iter1, Snt, Iter2>, first, last, dest) } -> ::std::same_as<Iter2>;
};

template <typename Iter1, typename Snt, typename Iter2>
concept has_uninitialized_move_define = ::std::sentinel_for<Snt, Iter1> && requires(Iter1 first, Snt last, Iter2 dest) {
	{ uninitialized_move_define(::fast_io::operations::defines::memory_algorithm_define<Iter1, Snt, Iter2>, first, last, dest) } -> ::std::same_as<Iter2>;
};

template <typename Iter1, typename Snt, typename Iter2>
concept has_uninitialized_move_backward_define = ::std::sentinel_for<Snt, Iter1> && requires(Iter1 first, Snt last, Iter2 dest) {
	{ uninitialized_move_backward_define(::fast_io::operations::defines::memory_algorithm_define<Iter1, Snt, Iter2>, first, last, dest) } -> ::std::same_as<Iter2>;
};

} // namespace fast_io::operations::defines

namespace fast_io::freestanding
{

/*
uninitialized_relocate requires two range are not overlapped.
*/

template <::std::input_or_output_iterator Iter1, ::std::sentinel_for<Iter1> Sent, ::std::input_or_output_iterator Iter2>
inline constexpr Iter2 uninitialized_relocate_ignore_define(Iter1 first, Sent last, Iter2 dest)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_constructible_v<::std::iter_value_t<Iter2>, decltype(::std::move(*first))> ||
								   !::std::is_nothrow_destructible_v<::std::iter_value_t<Iter1>>)
{
	if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1> && ::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate_ignore_define(::std::to_address(first), ::std::to_address(last),
													::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1>)
	{
		return uninitialized_relocate_ignore_define(::std::to_address(first), ::std::to_address(last),
													dest);
	}
	else if constexpr (::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate_ignore_define(first, last, ::std::to_address(dest)) -
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
			FAST_IO_IF_NOT_CONSTEVAL
			{
				return reinterpret_cast<Iter2>(::fast_io::freestanding::bytes_copy(reinterpret_cast<::std::byte const *>(first), reinterpret_cast<::std::byte const *>(last), reinterpret_cast<::std::byte *>(dest)));
			}
		}
		// we do not allow move constructor to throw EH.
		while (first != last)
		{
			::new (static_cast<void *>(::std::addressof(*dest))) iter2valuetype(::std::move(*first));

			if constexpr (!::std::is_trivially_destructible_v<iter1valuetype>)
			{
				if constexpr (::std::is_pointer_v<Iter1>)
				{
					first->~iter1valuetype();
				}
				else
				{
					__builtin_addressof(*first)->~iter1valuetype();
				}
			}
			++first;
			++dest;
		}
		return dest;
	}
}

template <::std::input_or_output_iterator Iter1, ::std::sentinel_for<Iter1> Snt, ::std::input_or_output_iterator Iter2>
inline constexpr Iter2 uninitialized_relocate(Iter1 first, Snt last, Iter2 dest)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::freestanding::uninitialized_relocate_ignore_define(first, last, dest))
{
	if constexpr (
		::std::same_as<Iter1, Snt> && ::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1> && ::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate(::std::to_address(first), ::std::to_address(last),
									  ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else if constexpr (::std::same_as<Iter1, Snt> && ::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1>)
	{
		return uninitialized_relocate(::std::to_address(first), ::std::to_address(last),
									  dest);
	}
	else if constexpr (::std::same_as<Iter1, Snt> && ::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate(first, last, ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else
	{
		if constexpr (::fast_io::operations::defines::has_uninitialized_relocate_define<Iter1, Snt, Iter2>)
		{
			return uninitialized_relocate_define(::fast_io::operations::defines::memory_algorithm_define<Iter1, Snt, Iter2>, first, last, dest);
		}
		else
		{
			return ::fast_io::freestanding::uninitialized_relocate_ignore_define(first, last, dest);
		}
	}
}

template <::std::bidirectional_iterator Iter1, ::std::sentinel_for<Iter1> Snt, ::std::bidirectional_iterator Iter2>
inline constexpr Iter2 uninitialized_relocate_backward_ignore_define(Iter1 first, Snt last, Iter2 dest)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_constructible_v<::std::iter_value_t<Iter2>, decltype(::std::move(*first))> ||
								   !::std::is_nothrow_destructible_v<::std::iter_value_t<Iter1>>)
{
	// Semantics:
	//   Relocate the range [first, last) into the uninitialized memory ending at `dest`.
	//   `dest` is treated as the end iterator (one past the last element) of the destination range.
	//   The function returns the begin iterator of the destination range:
	//       dest - (last - first)

	if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1> &&
				  ::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate_backward_ignore_define(::std::to_address(first),
															 ::std::to_address(last),
															 ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1>)
	{
		return uninitialized_relocate_backward_ignore_define(::std::to_address(first),
															 ::std::to_address(last),
															 dest);
	}
	else if constexpr (::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate_backward_ignore_define(first,
															 last,
															 ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else
	{
		using iter1valuetype = ::std::iter_value_t<Iter1>;
		using iter2valuetype = ::std::iter_value_t<Iter2>;

		// Fast path: trivial relocation via byte copy
		if constexpr (::std::is_pointer_v<Iter1> && ::std::is_pointer_v<Iter2> &&
					  (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<iter1valuetype> &&
					   ::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<iter2valuetype> &&
					   (::std::same_as<iter1valuetype, iter2valuetype> ||
						((::std::integral<iter1valuetype> || ::std::same_as<iter1valuetype, ::std::byte>) &&
						 (::std::integral<iter2valuetype> || ::std::same_as<iter2valuetype, ::std::byte>) &&
						 sizeof(iter1valuetype) == sizeof(iter2valuetype)))))
		{
			FAST_IO_IF_NOT_CONSTEVAL
			{
				auto const firstbyteptr{reinterpret_cast<::std::byte const *>(first)};
				auto const lastbyteptr{reinterpret_cast<::std::byte const *>(last)};
				auto const n{lastbyteptr - firstbyteptr};

				// Compute the beginning of the destination range
				auto const destfirst{reinterpret_cast<::std::byte *>(dest) - n};

				// Perform the raw byte copy
				::fast_io::freestanding::bytes_copy(firstbyteptr,
													lastbyteptr,
													destfirst);

				return reinterpret_cast<Iter2>(destfirst);
			}
		}
		// Generic slow path:
		//   Move-construct elements in reverse order into uninitialized memory,
		//   then destroy the original elements.
		while (first != last)
		{
			--last;
			--dest;
			::new (static_cast<void *>(::std::addressof(*dest))) iter2valuetype(::std::move(*last));

			if constexpr (!::std::is_trivially_destructible_v<iter1valuetype>)
			{
				if constexpr (::std::is_pointer_v<Iter1>)
				{
					last->~iter1valuetype();
				}
				else
				{
					__builtin_addressof(*last)->~iter1valuetype();
				}
			}
		}

		// Return the begin iterator of the relocated range
		return dest;
	}
}

template <::std::bidirectional_iterator Iter1, ::std::sentinel_for<Iter1> Snt, ::std::bidirectional_iterator Iter2>
inline constexpr Iter2 uninitialized_relocate_backward(Iter1 first, Snt last, Iter2 dest)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::freestanding::uninitialized_relocate_backward_ignore_define(first, last, dest))
{
	// Semantics:
	//   Relocate the range [first, last) into the uninitialized memory ending at `dest`.
	//   `dest` is treated as the end iterator (one past the last element) of the destination range.
	//   The function returns the begin iterator of the destination range:
	//       dest - (last - first)

	if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1> &&
				  ::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate_backward(::std::to_address(first),
											   ::std::to_address(last),
											   ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1>)
	{
		return uninitialized_relocate_backward(::std::to_address(first),
											   ::std::to_address(last),
											   dest);
	}
	else if constexpr (::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate_backward(first,
											   last,
											   ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else
	{
		if constexpr (::fast_io::operations::defines::has_uninitialized_relocate_backward_define<Iter1, Iter1, Iter2>)
		{
			return uninitialized_relocate_backward_define(::fast_io::operations::defines::memory_algorithm_define<Iter1, Snt, Iter2>, first, last, dest);
		}
		else
		{
			return ::fast_io::freestanding::uninitialized_relocate_backward_ignore_define(first, last, dest);
		}
	}
}

template <::std::input_or_output_iterator Iter1, ::std::input_or_output_iterator Iter2>
inline constexpr Iter2 uninitialized_move(Iter1 first, Iter1 last, Iter2 dest)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_constructible_v<::std::iter_value_t<Iter2>, decltype(::std::move(*first))>)
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
			FAST_IO_IF_NOT_CONSTEVAL
			{
				return reinterpret_cast<Iter2>(::fast_io::freestanding::bytes_copy(reinterpret_cast<::std::byte const *>(first), reinterpret_cast<::std::byte const *>(last), reinterpret_cast<::std::byte *>(dest)));
			}
		}
#if 0
		else if constexpr (::fast_io::operations::defines::has_uninitialized_move_define<Iter1, Iter1, Iter2>)
		{
			return uninitialized_move_define(::fast_io::operations::defines::memory_algorithm_define<Iter1, Snt, Iter2>, first, last, dest);
		}
#endif
		// we do not allow move constructor to throw EH.
		while (first != last)
		{
			::new (static_cast<void *>(::std::addressof(*dest))) iter2valuetype(::std::move(*first));
			++first;
			++dest;
		}
		return dest;
	}
}

template <::std::bidirectional_iterator Iter1, ::std::bidirectional_iterator Iter2>
inline constexpr Iter2 uninitialized_move_backward(Iter1 first, Iter1 last, Iter2 d_last)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_constructible_v<::std::iter_value_t<Iter2>, decltype(::std::move(*last))>)
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
			FAST_IO_IF_NOT_CONSTEVAL
#else
			if (!__builtin_is_constant_evaluated())
#endif
			{
				auto d_start{d_last - (last - first)};
				::fast_io::freestanding::bytes_copy(reinterpret_cast<::std::byte const *>(first), reinterpret_cast<::std::byte const *>(last), reinterpret_cast<::std::byte *>(d_start));
				return d_start;
			}
		}
#if 0
		else if constexpr (::fast_io::operations::defines::has_uninitialized_move_backward_define<Iter1, Iter1, Iter2>)
		{
			return uninitialized_move_backward_define(::fast_io::operations::defines::memory_algorithm_define<Iter1, Snt, Iter2>, first, last, d_last);
		}
#endif
		while (first != last)
		{
			::new (static_cast<void *>(--d_last)) iter2valuetype(::std::move(*(--last)));
		}
		return d_last;
	}
}

template <::std::input_or_output_iterator Iter, typename T>
inline constexpr Iter uninitialized_fill(Iter first, Iter last, T const &ele)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_constructible_v<::std::iter_value_t<Iter>, T const &>)
{
	using itervaluetype = ::std::iter_value_t<Iter>;
	if constexpr (::std::contiguous_iterator<itervaluetype>)
	{
		if constexpr (::std::is_trivially_copyable_v<itervaluetype> &&
					  ::std::is_scalar_v<itervaluetype> && sizeof(itervaluetype) == 1)
		{
#ifdef __cpp_if_consteval
			FAST_IO_IF_NOT_CONSTEVAL
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
		::new (static_cast<void *>(__builtin_addressof(*first))) itervaluetype(ele);
	}
	return last;
}

template <::std::input_or_output_iterator Iter, typename T>
inline constexpr Iter uninitialized_fill_n(Iter first, ::std::size_t n, T const &ele)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::freestanding::uninitialized_fill(first, first + n, ele))
{
	return ::fast_io::freestanding::uninitialized_fill(first, first + n, ele);
}

} // namespace fast_io::freestanding
