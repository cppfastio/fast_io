#pragma once

namespace fast_io
{

namespace details
{

template <::std::integral char_type, typename state, typename T, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void
inplace_to_decay_context_impl(::fast_io::details::basic_dynamic_output_buffer_ref<::fast_io::details::basic_dynamic_output_buffer<char_type>> buffer, state &s,
							  T t, Args... args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		::fast_io::operations::decay::print_freestanding_decay<false>(buffer, args...[i]);
		char_type *buffer_beg{buffer.dob_ptr->begin_ptr};
		char_type const *buffer_begin{buffer_beg};
		char_type const *buffer_curr{buffer.dob_ptr->curr_ptr};
		auto [it, ec] = scan_context_define(io_reserve_type<char_type, T>, s, buffer_begin, buffer_curr, t);
		if (it != buffer_curr)
		{
			if (ec != ::fast_io::freestanding::parse_errc::ok)
			{
				::fast_io::herbceptions::throws_parse_errc(ec);
			}
			return;
		}
		if constexpr (i + 1zu != sizeof...(Args))
		{
			buffer.dob_ptr->curr_ptr = buffer_beg;
		}
	}
	::fast_io::freestanding::parse_errc p{scan_context_eof_define(io_reserve_type<char_type, T>, s, t)};
	if (p != ::fast_io::freestanding::parse_errc::ok)
	{
		::fast_io::herbceptions::throws_parse_errc(p);
	}
}

template <::std::integral char_type, typename state, typename T, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void inplace_to_decay_buffer_scatter_context_impl(state &s, T t, Args... args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		using argtype = ::std::remove_cvref_t<Args...[i]>;
		basic_io_scatter_t<char_type> scatter{print_scatter_define(io_reserve_type<char_type, argtype>, args...[i])};
		char_type const *buffer_begin{scatter.base};
		char_type const *buffer_curr{buffer_begin + scatter.len};
		auto [it, ec] = scan_context_define(io_reserve_type<char_type, T>, s, buffer_begin, buffer_curr, t);
		if (it != buffer_curr)
		{
			if (ec != ::fast_io::freestanding::parse_errc::ok)
			{
				::fast_io::herbceptions::throws_parse_errc(ec);
			}
			return;
		}
	}
	::fast_io::freestanding::parse_errc p{scan_context_eof_define(io_reserve_type<char_type, T>, s, t)};
	if (p != ::fast_io::freestanding::parse_errc::ok)
	{
		::fast_io::herbceptions::throws_parse_errc(p);
	}
}

template <::std::integral char_type, typename state, typename T, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void inplace_to_decay_buffer_context_impl(char_type *buffer, state &s, T t, Args... args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr ((scatter_printable<char_type, Args> && ...))
	{
		inplace_to_decay_buffer_scatter_context_impl<char_type>(s, t, args...);
	}
	else
	{
		template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
		{
			using argtype = ::std::remove_cvref_t<Args...[i]>;
			char_type const *buffer_begin;
			char_type const *buffer_curr;
			if constexpr (scatter_printable<char_type, argtype>)
			{
				auto scatter{print_scatter_define(io_reserve_type<char_type, argtype>, args...[i])};
				buffer_begin = scatter.base;
				buffer_curr = buffer_begin + scatter.len;
			}
			else
			{
				buffer_curr = print_reserve_define(io_reserve_type<char_type, argtype>, buffer, args...[i]);
				buffer_begin = buffer;
			}
			auto [it, ec] = scan_context_define(io_reserve_type<char_type, T>, s, buffer_begin, buffer_curr, t);
			if (it != buffer_curr)
			{
				if (ec != ::fast_io::freestanding::parse_errc::ok)
				{
					::fast_io::herbceptions::throws_parse_errc(ec);
				}
				return;
			}
		}
		::fast_io::freestanding::parse_errc p{scan_context_eof_define(io_reserve_type<char_type, T>, s, t)};
		if (p != ::fast_io::freestanding::parse_errc::ok)
		{
			::fast_io::herbceptions::throws_parse_errc(p);
		}
	}
}

template <::std::integral char_type, bool ln, typename... Args>
inline constexpr ::std::size_t calculate_print_normal_maxium_size_main(::std::size_t mx_value) noexcept
{
	template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		::std::size_t val{};
		if constexpr (ln && (i + 1zu == sizeof...(Args)))
		{
			++val;
		}
		if constexpr (reserve_printable<char_type, ::std::remove_cvref_t<Args...[i]>>)
		{
			constexpr ::std::size_t size{print_reserve_size(io_reserve_type<char_type, ::std::remove_cvref_t<Args...[i]>>)};
			static_assert(size != SIZE_MAX, "overflow");
			val += size;
		}
		if (mx_value < val)
		{
			mx_value = val;
		}
	}
	return mx_value;
}

template <::std::integral char_type, bool ln, typename... Args>
inline constexpr ::std::size_t calculate_print_normal_maxium_size() noexcept
{
	return calculate_print_normal_maxium_size_main<char_type, ln, Args...>(0);
}

template <::std::integral char_type, bool ln, typename... Args>
inline constexpr ::std::size_t calculate_print_normal_dynamic_maxium_main(::std::size_t mx_value, Args... args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		::fast_io::details::has_any_print_define_operations_may_throw<char_type, Args...>)
{
	template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		using argtype = ::std::remove_cvref_t<Args...[i]>;
		::std::size_t size{};
		if constexpr (reserve_printable<char_type, argtype>)
		{
			constexpr ::std::size_t sz{print_reserve_size(io_reserve_type<char_type, argtype>)};
			static_assert(sz != SIZE_MAX, "overflow");
			size = sz;
		}
		else if constexpr (dynamic_reserve_printable<char_type, argtype>)
		{
			size = print_reserve_size(io_reserve_type<char_type, argtype>, args...[i]);
		}
		if constexpr (ln && (i + 1zu == sizeof...(Args)))
		{
			if (size == SIZE_MAX)
			{
				fast_terminate();
			}
			++size;
		}
		if (mx_value < size)
		{
			mx_value = size;
		}
	}
	return mx_value;
}

template <::std::integral char_type, typename T>
inline constexpr void deal_with_single_to(char_type const *buffer_begin, char_type const *buffer_end, T t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto code{scan_contiguous_define(io_reserve_type<char_type, T>, buffer_begin, buffer_end, t).code};
	if (code != ::fast_io::freestanding::parse_errc::ok)
	{
		::fast_io::herbceptions::throws_parse_errc(code);
	}
}

template <::std::integral char_type, typename T, typename Arg>
inline constexpr void to_deal_with_contiguous_single_scatter(T t, Arg arg)
	FAST_IO_HERBCEPTIONS_THROWS
{
	basic_io_scatter_t<char_type> scatter{print_scatter_define(io_reserve_type<char_type, Arg>, arg)};
	auto base{scatter.base};
	deal_with_single_to<char_type>(base, base + scatter.len, t);
}

template <::std::integral char_type, typename... Args>
inline constexpr char_type *to_impl_with_reserve(char_type *p, Args... args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		::fast_io::details::has_any_print_define_operations_may_throw<char_type, Args...>)
{
	template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		using argtype = ::std::remove_cvref_t<Args...[i]>;
		if constexpr (scatter_printable<char_type, argtype>)
		{
			p = copy_scatter(print_scatter_define(io_reserve_type<char_type, argtype>, args...[i]), p);
		}
		else
		{
			p = print_reserve_define(io_reserve_type<char_type, argtype>, p, args...[i]);
		}
	}
	return p;
}

template <::std::integral char_type, typename... Args>
inline constexpr ::std::size_t calculate_scatter_dynamic_reserve_size_with_scatter([[maybe_unused]] Args... args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		::fast_io::details::has_any_print_define_operations_may_throw<char_type, Args...>)
{
	::std::size_t total{};
	template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		using argtype = ::std::remove_cvref_t<Args...[i]>;
		if constexpr (dynamic_reserve_printable<char_type, argtype>)
		{
			total = ::fast_io::details::intrinsics::add_or_overflow_die(
				total, print_reserve_size(io_reserve_type<char_type, argtype>, args...[i]));
		}
		else if constexpr (scatter_printable<char_type, argtype>)
		{
			total = ::fast_io::details::intrinsics::add_or_overflow_die(
				total, print_scatter_define(io_reserve_type<char_type, argtype>, args...[i]).len);
		}
	}
	return total;
}

template <typename char_type, typename T, typename... Args>
concept inplace_to_decay_detect =
	::std::integral<char_type> &&
	(sizeof...(Args) != 0 &&
	 ::fast_io::operations::decay::defines::print_freestanding_params_okay<char_type, Args...> &&
	 (contiguous_scannable<char_type, T> || context_scannable<char_type, T>));

} // namespace details

template <::std::integral char_type, typename T, typename... Args>
inline constexpr void basic_inplace_to_decay(T t, Args... args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::inplace_to_decay_detect<char_type, T, Args...>};
	if constexpr (failed)
	{
		if constexpr (((reserve_printable<char_type, Args> || dynamic_reserve_printable<char_type, Args> ||
						scatter_printable<char_type, Args>) &&
					   ...))
		{
			constexpr bool all_scatters{((scatter_printable<char_type, Args>) && ...)};
			constexpr bool no_need_dynamic_reserve{
				((reserve_printable<char_type, Args> || scatter_printable<char_type, Args>) && ...)};
			if constexpr (context_scannable<char_type, T> &&
						  (!(contiguous_scannable<char_type, T> && sizeof...(args) == 1)))
			{
				typename ::std::remove_cvref_t<decltype(scan_context_type(io_reserve_type<char_type, T>))>::type state;
				if constexpr (all_scatters)
				{
					::fast_io::details::inplace_to_decay_buffer_scatter_context_impl<char_type>(state, t, args...);
				}
				else if constexpr (no_need_dynamic_reserve)
				{
					constexpr ::std::size_t maximum_reserve_size{
						::fast_io::details::calculate_print_normal_maxium_size<char_type, false, Args...>()};
					char_type buffer[maximum_reserve_size];
					::fast_io::details::inplace_to_decay_buffer_context_impl<char_type>(buffer, state, t, args...);
				}
				else
				{
					::std::size_t const maximum_reserve_size{
						::fast_io::details::calculate_print_normal_dynamic_maxium_main<char_type, false>(0, args...)};
					::fast_io::details::local_operator_new_array_ptr<char_type> heap_buffer(maximum_reserve_size);
					::fast_io::details::inplace_to_decay_buffer_context_impl<char_type>(heap_buffer.ptr, state, t,
																						args...);
				}
			}
			else if constexpr (contiguous_scannable<char_type, T>)
			{
				if constexpr (all_scatters && sizeof...(Args) == 1) // crucial for performance
				{
					::fast_io::details::to_deal_with_contiguous_single_scatter<char_type>(t, args...);
				}
				else if constexpr (((reserve_printable<char_type, Args>) && ...))
				{
					constexpr ::std::size_t total_size{
						::fast_io::details::decay::calculate_scatter_reserve_size<char_type, Args...>()};
					char_type buffer[total_size];
					auto ret{::fast_io::details::to_impl_with_reserve(buffer, args...)};
					::fast_io::details::deal_with_single_to<char_type>(buffer, ret, t);
				}
				else
				{
					::std::size_t const maximum_reserve_size{
						::fast_io::details::calculate_scatter_dynamic_reserve_size_with_scatter<char_type>(args...)};
					::fast_io::details::local_operator_new_array_ptr<char_type> heap_buffer(maximum_reserve_size);
					auto ret{::fast_io::details::to_impl_with_reserve(heap_buffer.ptr, args...)};
					::fast_io::details::deal_with_single_to<char_type>(heap_buffer.ptr, ret, t);
				}
			}
		}
		else
		{
			::fast_io::details::basic_dynamic_output_buffer<char_type> buffer;
			auto ref{::fast_io::operations::output_stream_ref(buffer)};
			if constexpr (context_scannable<char_type, T> &&
						  (!(contiguous_scannable<char_type, T> && sizeof...(args) == 1)))
			{
				typename ::std::remove_cvref_t<decltype(scan_context_type(io_reserve_type<char_type, T>))>::type state;
				::fast_io::details::inplace_to_decay_context_impl(ref, state, t, args...);
			}
			else if constexpr (contiguous_scannable<char_type, T>)
			{
				::fast_io::operations::decay::print_freestanding_decay<false>(ref, args...);
				::fast_io::details::deal_with_single_to<char_type>(buffer.begin_ptr, buffer.curr_ptr, t);
			}
			else
			{
				constexpr bool type_error{context_scannable<char_type, T>};
				static_assert(type_error, "scan type error");
			}
		}
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
	}
}

namespace details
{

template <::std::integral char_type, typename T, typename... Args>
	requires ::fast_io::details::inplace_to_decay_detect<char_type, T, Args...>
inline constexpr void basic_inplace_to_decay_model(T, Args...) FAST_IO_HERBCEPTIONS_THROWS
{
}

template <typename char_type, typename T, typename... Args>
concept can_do_inplace_to = requires(T &t, Args &&...args) {
	::fast_io::details::basic_inplace_to_decay_model<char_type>(
		::fast_io::io_scan_forward<char_type>(::fast_io::io_scan_alias(t)),
		::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
};

} // namespace details

template <::std::integral char_type, typename T, typename... Args>
inline constexpr void basic_inplace_to(T &&t, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::can_do_inplace_to<char_type, T, Args...>};
	if constexpr (failed)
	{
		::fast_io::basic_inplace_to_decay<char_type>(::fast_io::io_scan_forward<char_type>(::fast_io::io_scan_alias(t)),
													 ::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
	}
}

template <typename T, typename... Args>
inline constexpr void inplace_to(T &&t, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::can_do_inplace_to<char, T, Args...>};
	if constexpr (failed)
	{
		::fast_io::basic_inplace_to_decay<char>(::fast_io::io_scan_forward<char>(::fast_io::io_scan_alias(t)),
												::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
	}
}

template <typename T, typename... Args>
inline constexpr void winplace_to(T &&t, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::can_do_inplace_to<wchar_t, T, Args...>};
	if constexpr (failed)
	{
		::fast_io::basic_inplace_to_decay<wchar_t>(::fast_io::io_scan_forward<wchar_t>(::fast_io::io_scan_alias(t)),
												   ::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
	}
}

template <typename T, typename... Args>
inline constexpr void u8inplace_to(T &&t, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::can_do_inplace_to<char8_t, T, Args...>};
	if constexpr (failed)
	{
		::fast_io::basic_inplace_to_decay<char8_t>(::fast_io::io_scan_forward<char8_t>(::fast_io::io_scan_alias(t)),
												   ::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
	}
}

template <typename T, typename... Args>
inline constexpr void u16inplace_to(T &&t, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::can_do_inplace_to<char16_t, T, Args...>};
	if constexpr (failed)
	{
		::fast_io::basic_inplace_to_decay<char16_t>(::fast_io::io_scan_forward<char16_t>(::fast_io::io_scan_alias(t)),
													::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
	}
}

template <typename T, typename... Args>
inline constexpr void u32inplace_to(T &&t, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::can_do_inplace_to<char32_t, T, Args...>};
	if constexpr (failed)
	{
		::fast_io::basic_inplace_to_decay<char32_t>(::fast_io::io_scan_forward<char32_t>(::fast_io::io_scan_alias(t)),
													::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
	}
}

namespace decay
{

template <::std::integral char_type, typename T, typename... Args>
inline constexpr T basic_to_decay(Args... args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::can_do_inplace_to<char_type, T, Args...>};
	if constexpr (sizeof...(Args) == 0)
	{
		return T();
	}
	else if constexpr (failed)
	{
		if constexpr (::std::is_scalar_v<T>)
		{
			T v{};
			basic_inplace_to_decay<char_type>(::fast_io::io_scan_forward<char_type>(::fast_io::io_scan_alias(v)),
											  args...);
			return v;
		}
		else
		{
			T v;
			basic_inplace_to_decay<char_type>(::fast_io::io_scan_forward<char_type>(::fast_io::io_scan_alias(v)),
											  args...);
			return v;
		}
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
		return T();
	}
}

} // namespace decay

template <::std::integral char_type, typename T, typename... Args>
inline constexpr T basic_to(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::can_do_inplace_to<char_type, T, Args...>};
	if constexpr (failed)
	{
		return ::fast_io::decay::basic_to_decay<char_type, T>(io_print_forward<char_type>(io_print_alias(args))...);
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
		return T();
	}
}

template <typename T, typename... Args>
[[nodiscard]] inline constexpr T to(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::can_do_inplace_to<char, T, Args...>};
	if constexpr (failed)
	{
		return ::fast_io::decay::basic_to_decay<char, T>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
		return T();
	}
}

template <typename T, typename... Args>
[[nodiscard]] inline constexpr T wto(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::can_do_inplace_to<wchar_t, T, Args...>};
	if constexpr (failed)
	{
		return ::fast_io::decay::basic_to_decay<wchar_t, T>(::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
		return T();
	}
}

template <typename T, typename... Args>
[[nodiscard]] inline constexpr T u8to(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::can_do_inplace_to<char8_t, T, Args...>};
	if constexpr (failed)
	{
		return ::fast_io::decay::basic_to_decay<char8_t, T>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
		return T();
	}
}

template <typename T, typename... Args>
[[nodiscard]] inline constexpr T u16to(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::can_do_inplace_to<char16_t, T, Args...>};
	if constexpr (failed)
	{
		return ::fast_io::decay::basic_to_decay<char16_t, T>(::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
		return T();
	}
}

template <typename T, typename... Args>
[[nodiscard]] inline constexpr T u32to(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool failed{::fast_io::details::can_do_inplace_to<char32_t, T, Args...>};
	if constexpr (failed)
	{
		return ::fast_io::decay::basic_to_decay<char32_t, T>(::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(failed, "either somes args not printable or some type not detectable");
		return T();
	}
}

} // namespace fast_io
