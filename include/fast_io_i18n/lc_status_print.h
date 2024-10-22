#pragma once

namespace fast_io
{

template <typename char_type, typename T>
concept lc_dynamic_reserve_printable =
	::std::integral<char_type> &&
	requires(T t, basic_lc<char_type> const *all, char_type *ptr, ::std::size_t size) {
		{ print_reserve_size(all, t) } -> ::std::convertible_to<::std::size_t>;
		{ print_reserve_define(all, ptr, t) } -> ::std::convertible_to<char_type *>;
	};

template <::std::integral char_type, typename value_type>
	requires lc_dynamic_reserve_printable<char_type, ::std::remove_cvref_t<value_type>>
inline constexpr auto print_reserve_size(basic_lc<char_type> const *__restrict all, parameter<value_type> para)
{
	return print_reserve_size(all, para.reference);
}

template <::std::integral char_type, typename value_type>
	requires lc_dynamic_reserve_printable<char_type, ::std::remove_cvref_t<value_type>>
inline constexpr auto print_reserve_define(basic_lc<char_type> const *__restrict all, char_type *begin, parameter<value_type> para)
{
	return print_reserve_define(all, begin, para.reference);
}

template <typename char_type, typename T>
concept lc_scatter_printable = requires(basic_lc<char_type> const *all, T t) {
	{ print_scatter_define(all, t) } -> ::std::same_as<basic_io_scatter_t<char_type>>;
};

template <typename char_type, typename T>
concept lc_printable = requires(basic_lc<char_type> const *all, ::fast_io::details::dummy_buffer_output_stream<char_type> out, T t) {
	print_define(all, out, t);
};

template <typename char_type, typename T>
concept lc_printable_internal_shift = requires(basic_lc<char_type> const *all, T t) {
	{ print_define_internal_shift(all, t) } -> ::std::same_as<::std::size_t>;
};

template <::std::integral char_type, typename value_type>
	requires lc_printable_internal_shift<char_type, ::std::remove_cvref_t<value_type>>
inline constexpr auto print_define_internal_shift(basic_lc<char_type> const *__restrict all, parameter<value_type> para)
{
	return print_define_internal_shift(all, para.reference);
}

namespace details::decay
{

template <::std::integral char_type, typename T, typename... Args>
inline constexpr ::fast_io::details::decay::contiguous_scatter_result find_lc_ontinuous_scatters_n()
{
	::fast_io::details::decay::contiguous_scatter_result ret{};
	if constexpr (::fast_io::lc_scatter_printable<char_type, T>)
	{
		if constexpr (sizeof...(Args) != 0)
		{
			ret = find_continuous_scatters_n<char_type, Args...>();
		}
		++ret.position;
		ret.hasscatters = true;
		ret.neededscatters = ::fast_io::details::intrinsics::add_or_overflow_die_chain(ret.neededscatters, static_cast<::std::size_t>(1));
	}
	else if constexpr (::fast_io::lc_dynamic_reserve_printable<char_type, T>)
	{
		if constexpr (sizeof...(Args) != 0)
		{
			ret = find_continuous_scatters_n<char_type, Args...>();
		}
		else
		{
			ret.lastisreserve = true;
		}
		++ret.position;
		ret.neededscatters = ::fast_io::details::intrinsics::add_or_overflow_die_chain(ret.neededscatters, static_cast<::std::size_t>(1));
		ret.hasdynamicreserve = true;
	}
	return ret;
}

template <bool line = false, typename output, typename T>
	requires(::std::is_trivially_copyable_v<output> && ::std::is_trivially_copyable_v<T>)
inline constexpr void lc_print_control_single(basic_lc<typename output::output_char_type> const *__restrict lc, output outstm, T t)
{
	using char_type = typename output::output_char_type;
	using value_type = ::std::remove_cvref_t<T>;
	constexpr bool asan_activated{::fast_io::details::asan_state::current == ::fast_io::details::asan_state::activate};
	constexpr auto lfch{char_literal_v<u8'\n', char_type>};
}


// template <::std::integral char_type, typename T, typename... Args>
// inline constexpr ::std::size_t calculate_lc_scatter_dynamic_reserve_size(
//	basic_lc<char_type> const *__restrict all, T t, Args... args)
//{
//	if constexpr (lc_dynamic_reserve_printable<char_type, T>)
//	{
//		::std::size_t res{print_reserve_size(all, t)};
//		if constexpr (sizeof...(Args) == 0)
//		{
//			return res;
//		}
//		else
//		{
//			return ::fast_io::details::intrinsics::add_or_overflow_die(
//				res, calculate_lc_scatter_dynamic_reserve_size(all, args...));
//		}
//	}
//	else if constexpr (!reserve_printable<char_type, T> && dynamic_reserve_printable<char_type, T>)
//	{
//		::std::size_t res{print_reserve_size(io_reserve_type<char_type, T>, t)};
//		if constexpr (sizeof...(Args) == 0)
//		{
//			return res;
//		}
//		else
//		{
//			return ::fast_io::details::intrinsics::add_or_overflow_die(
//				res, calculate_lc_scatter_dynamic_reserve_size(all, args...));
//		}
//	}
//	else
//	{
//		if constexpr (sizeof...(Args) == 0)
//		{
//			return 0;
//		}
//		else
//		{
//			return calculate_lc_scatter_dynamic_reserve_size(all, args...);
//		}
//	}
// }
//
// template <::std::integral char_type, typename T, typename... Args>
// inline constexpr void lc_scatter_print_with_dynamic_reserve_recursive(
//	basic_lc<char_type> const *__restrict all,
//	io_scatter_t *__restrict arr,
//	char_type *__restrict ptr,
//	char_type *__restrict dynamic_buffer_ptr, T t, Args... args)
//{
//	if constexpr (lc_scatter_printable<char_type, T>)
//	{
//		auto sc{print_scatter_define(all, t)};
//		*arr = {sc.base, sc.len * sizeof(char_type)};
//	}
//	else if constexpr (lc_dynamic_reserve_printable<char_type, T>)
//	{
//		auto end_ptr = print_reserve_define(all, dynamic_buffer_ptr, t);
//		*arr = {dynamic_buffer_ptr, (end_ptr - dynamic_buffer_ptr) * sizeof(*dynamic_buffer_ptr)};
//		if constexpr (sizeof...(Args) != 0)
//		{
//			dynamic_buffer_ptr = end_ptr;
//		}
//	}
//	else if constexpr (scatter_printable<char_type, T>)
//	{
//		auto sc{print_scatter_define(io_reserve_type<char_type, T>, t)};
//		*arr = {sc.base, sc.len * sizeof(char_type)};
//	}
//	else if constexpr (reserve_printable<char_type, T>)
//	{
//		auto end_ptr = print_reserve_define(io_reserve_type<char_type, T>, ptr, t);
//		*arr = {ptr, (end_ptr - ptr) * sizeof(*ptr)};
//		if constexpr (sizeof...(Args) != 0)
//		{
//			ptr = end_ptr;
//		}
//	}
//	else
//	{
//		auto end_ptr = print_reserve_define(io_reserve_type<char_type, T>, dynamic_buffer_ptr, t);
//		*arr = {dynamic_buffer_ptr,
//				static_cast<::std::size_t>(end_ptr - dynamic_buffer_ptr) * sizeof(*dynamic_buffer_ptr)};
//		if constexpr (sizeof...(Args) != 0)
//		{
//			dynamic_buffer_ptr = end_ptr;
//		}
//	}
//	if constexpr (sizeof...(Args) != 0)
//	{
//		if constexpr (((!lc_dynamic_reserve_printable<char_type, Args> && !lc_scatter_printable<char_type, Args>) && ...))
//		{
//			scatter_print_with_dynamic_reserve_recursive(arr + 1, ptr, dynamic_buffer_ptr, args...);
//		}
//		else
//		{
//			lc_scatter_print_with_dynamic_reserve_recursive(all, arr + 1, ptr, dynamic_buffer_ptr, args...);
//		}
//	}
// }
//
// template <::std::integral char_type, typename T, typename... Args>
// inline constexpr void lc_scatter_print_with_dynamic_only_reserve_recursive(
//	basic_lc<char_type> const *__restrict all,
//	io_scatter_t *__restrict arr,
//	char_type *__restrict dynamic_buffer_ptr, T t, Args... args)
//{
//	static_assert(!reserve_printable<char_type, T>);
//	if constexpr (lc_scatter_printable<char_type, T>)
//	{
//		auto sc{print_scatter_define(all, t)};
//		*arr = {sc.base, sc.len * sizeof(char_type)};
//	}
//	else if constexpr (lc_dynamic_reserve_printable<char_type, T>)
//	{
//		auto end_ptr = print_reserve_define(all, dynamic_buffer_ptr, t);
//		*arr = {dynamic_buffer_ptr, (end_ptr - dynamic_buffer_ptr) * sizeof(*dynamic_buffer_ptr)};
//		if constexpr (sizeof...(Args) != 0)
//		{
//			dynamic_buffer_ptr = end_ptr;
//		}
//	}
//	else if constexpr (scatter_printable<char_type, T>)
//	{
//		auto sc{print_scatter_define(io_reserve_type<char_type, T>, t)};
//		*arr = {sc.base, sc.len * sizeof(char_type)};
//	}
//	else
//	{
//		auto end_ptr = print_reserve_define(io_reserve_type<char_type, T>, dynamic_buffer_ptr, t);
//		*arr = {dynamic_buffer_ptr,
//				static_cast<::std::size_t>(end_ptr - dynamic_buffer_ptr) * sizeof(*dynamic_buffer_ptr)};
//		if constexpr (sizeof...(Args) != 0)
//		{
//			dynamic_buffer_ptr = end_ptr;
//		}
//	}
//	if constexpr (sizeof...(Args) != 0)
//	{
//		if constexpr (((!lc_dynamic_reserve_printable<char_type, Args> && !lc_scatter_printable<char_type, Args>) && ...))
//		{
//			scatter_print_with_dynamic_only_reserve_recursive(arr + 1, dynamic_buffer_ptr, args...);
//		}
//		else
//		{
//			lc_scatter_print_with_dynamic_only_reserve_recursive(all, arr + 1, dynamic_buffer_ptr, args...);
//		}
//	}
// }
//
// template <bool line, typename output, typename T>
// inline constexpr void lc_print_control_reserve_bad_path(basic_lc_all<typename output::char_type> const *__restrict lc,
//														output out, T t, ::std::size_t size)
//{
//	using char_type = typename output::char_type;
//	if constexpr (line)
//	{
//		if (SIZE_MAX == size)
//		{
//			fast_terminate();
//		}
//		++size;
//	}
//	local_operator_new_array_ptr<char_type> ptr(size);
//	auto it{print_reserve_define(lc, ptr.ptr, t)};
//	if constexpr (line)
//	{
//		*it = char_literal_v<u8'\n', char_type>;
//		++it;
//	}
//	write(out, ptr.ptr, it);
// }
//
// template <bool line, typename output, typename T>
//	requires(::std::is_trivially_copyable_v<output> && ::std::is_trivially_copyable_v<T>)
// inline constexpr void lc_print_control(basic_lc_all<typename output::char_type> const *__restrict lc, output out, T t)
//{
//	using char_type = typename output::char_type;
//	using value_type = ::std::remove_cvref_t<T>;
//	if constexpr (lc_scatter_printable<char_type, value_type>)
//	{
//		print_control<line>(out, print_scatter_define(lc, t));
//	}
//	else if constexpr (lc_dynamic_reserve_printable<char_type, value_type>)
//	{
//		::std::size_t sz{print_reserve_size(lc, t)};
//		// if constexpr (buffer_output_stream<output>)
//		//{
//		//	auto bcurr{obuffer_curr(out)};
//		//	auto bend{obuffer_end(out)};
//		//	::std::ptrdiff_t diff(bend - bcurr);
//		//	if constexpr (line)
//		//	{
//		//		--diff;
//		//	}
//		//	if (static_cast<::std::ptrdiff_t>(sz) < diff) [[likely]]
//		//	{
//		//		// To check whether this affects performance.
//		//		auto it{print_reserve_define(lc, bcurr, t)};
//		//		if constexpr (line)
//		//		{
//		//			*it = char_literal_v<u8'\n', char_type>;
//		//			++it;
//		//		}
//		//		obuffer_set_curr(out, it);
//		//	}
//		//	else
//		//	{
//		//		lc_print_control_reserve_bad_path<line>(lc, out, t, sz);
//		//	}
//		// }
//		// else
//		//{
//		lc_print_control_reserve_bad_path<line>(lc, out, t, sz);
//		//}
//	}
//	else if constexpr (lc_printable<char_type, value_type>)
//	{
//		print_define(lc, out, t);
//		if constexpr (line)
//		{
//			put(out, char_literal_v<u8'\n', char_type>);
//		}
//	}
//	else
//	{
//		print_control<line>(out, t);
//	}
// }
//
// template <bool line, typename output, typename T, typename... Args>
// #if __has_cpp_attribute(__gnu__::__always_inline__)
//[[__gnu__::__always_inline__]]
// #elif __has_cpp_attribute(msvc::forceinline)
//[[msvc::forceinline]]
// #endif
// inline constexpr void lc_print_controls_line(basic_lc<typename output::char_type> const *__restrict lc, output out, T t, Args... args)
//{
//	if constexpr (sizeof...(Args) == 0)
//	{
//		lc_print_control<line>(lc, out, t);
//	}
//	else
//	{
//		using char_type = typename output::char_type;
//		lc_print_control<false>(lc, out, t);
//		if constexpr (((lc_scatter_printable<char_type, Args> || lc_dynamic_reserve_printable<char_type, Args> ||
//						lc_printable<char_type, Args>) ||
//					   ...))
//		{
//			if constexpr (line)
//			{
//				lc_print_controls_line<line>(lc, out, args...);
//			}
//			else
//			{
//				(lc_print_control<false>(lc, out, args), ...);
//			}
//		}
//		else
//		{
//			print_controls_line<line>(out, args...);
//		}
//	}
// }
//
// template <::std::integral char_type, typename T, typename... Args>
// inline constexpr void lc_scatter_print_recursive(basic_lc<char_type> const *__restrict lc, io_scatter_t *arr, T t, Args... args)
//{
//	if constexpr (lc_scatter_printable<char_type, T>)
//	{
//		auto sc{print_scatter_define(lc, t)};
//		*arr = {sc.base, sc.len * sizeof(char_type)};
//	}
//	else
//	{
//		auto sc{print_scatter_define(io_reserve_type<char_type, T>, t)};
//		*arr = {sc.base, sc.len * sizeof(char_type)};
//	}
//	if constexpr (sizeof...(Args) != 0)
//	{
//		if constexpr (((!lc_scatter_printable<char_type, Args>) && ...))
//		{
//			scatter_print_recursive<char_type>(arr + 1, args...);
//		}
//		else
//		{
//			lc_scatter_print_recursive(lc, arr + 1, args...);
//		}
//	}
// }
//
// template <bool ln, typename output, typename... Args>
// inline constexpr void lc_print_fallback(basic_lc_all<typename output::char_type> const *__restrict lc, output out, Args... args)
//{
//	using char_type = typename output::char_type;
//	if constexpr ((((!lc_dynamic_reserve_printable<char_type, Args> && !lc_printable<char_type, Args> &&
//					 !lc_scatter_printable<char_type, Args>)) &&
//				   ...))
//	{
//		print_freestanding_decay_no_status<ln>(out, args...);
//	}
//	// else if constexpr ((scatter_output_stream<output> || scatter_constant_output_stream<output>)&&(
//	//					   (reserve_printable<char_type, Args> || dynamic_reserve_printable<char_type, Args> ||
//	//						lc_dynamic_reserve_printable<char_type, Args> || scatter_printable<char_type, Args> ||
//	//						lc_scatter_printable<char_type, Args>)&&...))
//	//{
//	//	constexpr ::std::size_t args_num{sizeof...(Args)};
//	//	constexpr ::std::size_t scatters_num{args_num + static_cast<::std::size_t>(ln)};
//	//	io_scatter_t scatters[scatters_num];
//	//	if constexpr (((scatter_printable<char_type, Args> || lc_scatter_printable<char_type, Args>)&&...))
//	//	{
//	//		lc_scatter_print_recursive(lc, scatters, args...);
//	//		if constexpr (ln)
//	//		{
//	//			scatters[args_num] = {__builtin_addressof(char_literal_v<u8'\n', char_type>), sizeof(char_type)};
//	//		}
//	//		if constexpr (scatter_constant_output_stream<output>)
//	//		{
//	//			scatter_constant_write<scatters_num>(out, scatters);
//	//		}
//	//		else
//	//		{
//	//			scatter_write(out, {scatters, scatters_num});
//	//		}
//	//	}
//	//	else
//	//	{
//	//		constexpr ::std::size_t arrayn{calculate_scatter_reserve_size<char_type, Args...>()};
//	//		if constexpr (arrayn != 0)
//	//		{
//	//			char_type reserve_array[arrayn];
//	//			local_operator_new_array_ptr<char_type> new_ptr(
//	//				calculate_lc_scatter_dynamic_reserve_size<char_type>(lc, args...));
//	//			lc_scatter_print_with_dynamic_reserve_recursive(lc, scatters, reserve_array, new_ptr.ptr, args...);
//	//			if constexpr (ln)
//	//			{
//	//				scatters[args_num] = {__builtin_addressof(char_literal_v<u8'\n', char_type>), sizeof(char_type)};
//	//			}
//	//			if constexpr (scatter_constant_output_stream<output>)
//	//			{
//	//				scatter_constant_write<scatters_num>(out, scatters);
//	//			}
//	//			else
//	//			{
//	//				scatter_write(out, {scatters, scatters_num});
//	//			}
//	//		}
//	//		else
//	//		{
//	//			local_operator_new_array_ptr<char_type> new_ptr(
//	//				calculate_lc_scatter_dynamic_reserve_size<char_type>(lc, args...));
//	//			lc_scatter_print_with_dynamic_only_reserve_recursive(lc, scatters, new_ptr.ptr, args...);
//	//			if constexpr (ln)
//	//			{
//	//				scatters[args_num] = {__builtin_addressof(char_literal_v<u8'\n', char_type>), sizeof(char_type)};
//	//			}
//	//			if constexpr (scatter_constant_output_stream<output>)
//	//			{
//	//				scatter_constant_write<scatters_num>(out, scatters);
//	//			}
//	//			else
//	//			{
//	//				scatter_write(out, {scatters, scatters_num});
//	//			}
//	//		}
//	//	}
//	// }
//	// else
//	//{
//	//	temporary_buffer<output> buffer;
//	//	buffer.out = out;
//	//	auto ref{io_ref(buffer)};
//	//	lc_print_controls_line<ln>(lc, ref, args...);
//	//	flush(buffer);
//	// }
// }

template <bool line, typename outputstmtype, ::std::size_t skippings = 0, typename T, typename... Args>
inline constexpr void lc_print_controls_impl(basic_lc<typename outputstmtype::output_char_type> const *__restrict lc, outputstmtype optstm, T t, Args... args)
{
	using char_type = typename outputstmtype::output_char_type;
	using scatter_type = ::std::conditional_t<
		::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<outputstmtype>,
		io_scatter_t, basic_io_scatter_t<char_type>>;
	// TODO: contiguous_scatter_result
	constexpr auto res{::fast_io::details::decay::find_lc_ontinuous_scatters_n<char_type, T, Args...>()};
	if constexpr (skippings != 0)
	{
		lc_print_controls_impl<line, outputstmtype, skippings - 1>(optstm, args...);
	}
	else if constexpr (sizeof...(Args) == 0)
	{
		lc_print_control_single<line>(lc, optstm, t);
	}
	// TODO
}

} // namespace details::decay

namespace decay
{

template <bool line, typename outputstmtype, typename... Args>
inline constexpr void
lc_status_print_define_decay(basic_lc<typename outputstmtype::output_char_type> const *__restrict lc, outputstmtype optstm, Args... args)
{
	using char_type = typename outputstmtype::output_char_type;
	if constexpr (sizeof...(Args) == 0)
	{
		if constexpr (line)
		{
			using char_type = typename outputstmtype::output_char_type;
			return ::fast_io::operations::decay::char_put_decay(optstm, ::fast_io::char_literal_v<u8'\n', char_type>);
		}
		else
		{
			return;
		}
	}
	else if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outputstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::output_stream_mutex_ref_decay(optstm)};
		return ::fast_io::decay::lc_status_print_define_decay<line>(lc,
																	::fast_io::operations::decay::output_stream_unlocked_ref_decay(optstm), args...);
	}
	// else if constexpr (::fast_io::operations::decay::defines::has_obuffer_basic_operations<outputstmtype>)
	//{
	//	return ::fast_io::details::decay::lc_print_controls_buffer_impl<line>(lc, optstm, args...);
	// }
	else
	{
		return ::fast_io::details::decay::lc_print_controls_impl<line>(lc, optstm, args...);
	}

	// else if constexpr (buffer_output_stream<output>)
	//{
	//	if constexpr (sizeof...(Args) == 0 && ln)
	//	{
	//		put(out, char_literal_v<u8'\n', char_type>);
	//	}
	//	else
	//	{
	//		lc_print_controls_line<ln>(lc, out, args...);
	//	}
	// }
	// else if constexpr (sizeof...(Args) == 1 &&
	//				   (!ln || output_stream_with_writeln<output>)&&(
	//					   (printable<output, Args> || scatter_printable<char_type, Args> ||
	//						lc_scatter_printable<char_type, Args> || lc_dynamic_reserve_printable<char_type, Args> ||
	//						reserve_printable<char_type, Args> || dynamic_reserve_printable<char_type, Args>)&&...))
	//{
	//	lc_print_controls_line<ln>(lc, out, args...);
	// }
	// else if constexpr (sizeof...(Args) == 1 && line && ((lc_dynamic_reserve_printable<char_type, Args>) && ...))
	//{
	//	(lc_print_control<true>(lc, out, args), ...);
	//}
	// else
	//{
	//	lc_print_fallback<line>(lc, out, args...);
	//}
}

} // namespace decay

template <typename char_type, typename... Args>
concept lc_status_print_define_okay_character_type =
	::std::integral<char_type> &&
	(((printable<char_type, Args> || reserve_printable<char_type, Args> || dynamic_reserve_printable<char_type, Args> ||
	   scatter_printable<char_type, Args> || lc_printable<char_type, Args> ||
	   lc_dynamic_reserve_printable<char_type, Args> || lc_scatter_printable<char_type, Args>) &&
	  ...));

template <bool line, typename output, typename... Args>
	requires lc_status_print_define_okay_character_type<typename output::char_type, Args...>
inline constexpr void status_print_define(lc_imbuer<output> imb, Args... args)
{
	::fast_io::decay::lc_status_print_define_decay<line>(imb.locale, imb.handle, args...);
}

} // namespace fast_io
