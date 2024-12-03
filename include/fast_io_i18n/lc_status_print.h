#pragma once

namespace fast_io
{

template <typename char_type, typename T>
concept lc_dynamic_reserve_printable =
	::std::integral<char_type> &&
	requires(T t, ::fast_io::basic_lc_object<char_type> const *lc, char_type *ptr, ::std::size_t size) {
		{ print_reserve_size(lc, t) } -> ::std::convertible_to<::std::size_t>;
		{ print_reserve_define(lc, ptr, t) } -> ::std::convertible_to<char_type *>;
	};

template <::std::integral char_type, typename value_type>
	requires lc_dynamic_reserve_printable<char_type, ::std::remove_cvref_t<value_type>>
inline constexpr auto print_reserve_size(::fast_io::basic_lc_object<char_type> const *lc, parameter<value_type> para)
{
	return print_reserve_size(lc, para.reference);
}

template <::std::integral char_type, typename value_type>
	requires lc_dynamic_reserve_printable<char_type, ::std::remove_cvref_t<value_type>>
inline constexpr auto print_reserve_define(::fast_io::basic_lc_object<char_type> const *lc, char_type *begin, ::fast_io::parameter<value_type> para)
{
	return print_reserve_define(lc, begin, para.reference);
}

template <typename char_type, typename T>
concept lc_scatter_printable = requires(::fast_io::basic_lc_object<char_type> const *lc, T t) {
	{ print_scatter_define(lc, t) } -> ::std::same_as<basic_io_scatter_t<char_type>>;
};

template <typename char_type, typename T>
concept lc_printable = requires(::fast_io::basic_lc_object<char_type> const *lc, ::fast_io::details::dummy_buffer_output_stream<char_type> out, T t) {
	print_define(lc, out, t);
};

template <typename char_type, typename T>
concept lc_printable_internal_shift = requires(::fast_io::basic_lc_object<char_type> const *lc, T t) {
	{ print_define_internal_shift(lc, t) } -> ::std::same_as<::std::size_t>;
};

template <::std::integral char_type, typename value_type>
	requires lc_printable_internal_shift<char_type, ::std::remove_cvref_t<value_type>>
inline constexpr auto print_define_internal_shift(::fast_io::basic_lc_object<char_type> const *lc, parameter<value_type> para)
{
	return print_define_internal_shift(lc, para.reference);
}

namespace details::decay
{

template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind>
inline constexpr ::fast_io::details::decay::static_reserve_attribute_t count_lc_static_reserve_attribute(::fast_io::details::decay::static_reserve_attribute_t previous = {}) noexcept
{
	return previous;
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename T, typename... Args>
inline constexpr ::fast_io::details::decay::static_reserve_attribute_t count_lc_static_reserve_attribute(::fast_io::details::decay::static_reserve_attribute_t previous = {}) noexcept
{
	if constexpr (end_ind == 0)
	{
		return previous;
	}
	else if constexpr (beg_ind != 0)
	{
		return count_lc_static_reserve_attribute<char_type, beg_ind - 1, end_ind - 1, Args...>(previous);
	}
	else
	{
		if constexpr (lc_dynamic_reserve_printable<char_type, T>)
		{
			previous.has_dynamic_reserve = true;
			previous.last_is_reserve = true;
		}
		else if constexpr (lc_scatter_printable<char_type, T>)
		{
			previous.scatters_count = ::fast_io::details::intrinsics::add_or_overflow_die(previous.scatters_count, static_cast<::std::size_t>(1));
		}
		else if constexpr (lc_printable<char_type, T>)
		{
			previous.last_is_reserve = false;
		}
		else
		{
			previous = ::fast_io::details::decay::count_static_reserve_attribute<char_type, 0, 1, T>(previous);
		}
		return count_lc_static_reserve_attribute<char_type, 0, end_ind - 1, Args...>(previous);
	}
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename... Args>
	requires(end_ind <= sizeof...(Args))
inline constexpr ::fast_io::details::decay::static_reserve_attribute_t count_lc_static_reserve_attribute_v{count_lc_static_reserve_attribute<char_type, beg_ind, end_ind, Args...>()};

template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind>
	requires(end_ind == 0)
inline constexpr ::std::size_t calculate_all_lc_and_nonlc_dynamic_reserve_printable_size(
#if __has_cpp_attribute(maybe_unused)
	[[maybe_unused]]
#endif
	::fast_io::basic_lc_object<char_type> const *lc) noexcept
{
	return 0;
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename T, typename... Args>
	requires(end_ind <= sizeof...(Args) + 1)
inline constexpr ::std::size_t calculate_all_lc_and_nonlc_dynamic_reserve_printable_size(::fast_io::basic_lc_object<char_type> const *lc, T t, Args... args)
{
	if constexpr (end_ind == 0)
	{
		return 0;
	}
	else if constexpr (beg_ind != 0)
	{
		return calculate_all_lc_and_nonlc_dynamic_reserve_printable_size<char_type, beg_ind - 1, end_ind - 1>(lc, args...);
	}
	else
	{
		if constexpr (::fast_io::lc_dynamic_reserve_printable<char_type, T>)
		{
			return ::fast_io::details::intrinsics::add_or_overflow_die(
				print_reserve_size(lc, t),
				calculate_all_lc_and_nonlc_dynamic_reserve_printable_size<char_type, 0, end_ind - 1>(lc, args...));
		}
		else if constexpr (::fast_io::lc_scatter_printable<char_type, T> || ::fast_io::lc_printable<char_type, T>)
		{
			return calculate_all_lc_and_nonlc_dynamic_reserve_printable_size<char_type, 0, end_ind - 1>(lc, args...);
		}
		else
		{
			return ::fast_io::details::intrinsics::add_or_overflow_die(
				::fast_io::details::decay::calculate_all_dynamic_reserve_size<char_type, 0, 1>(t),
				calculate_all_lc_and_nonlc_dynamic_reserve_printable_size<char_type, 0, end_ind - 1>(lc, args...));
		}
	}
}

template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind>
inline constexpr ::std::size_t find_first_lc_any_printable() noexcept
{
	return 0;
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename T, typename... Args>
inline constexpr ::std::size_t find_first_lc_any_printable() noexcept
{
	if constexpr (end_ind == 0)
	{
		return 0;
	}
	else if constexpr (beg_ind != 0)
	{
		return find_first_lc_any_printable<char_type, beg_ind - 1, end_ind - 1, Args...>();
	}
	else
	{
		if constexpr (::fast_io::lc_dynamic_reserve_printable<char_type, T> || ::fast_io::lc_scatter_printable<char_type, T> || ::fast_io::lc_printable<char_type, T>)
		{
			return 0;
		}
		else
		{
			return ::fast_io::details::intrinsics::add_or_overflow_die(
				static_cast<::std::size_t>(1),
				find_first_lc_any_printable<char_type, 0, end_ind - 1, Args...>());
		}
	}
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename... Args>
inline constexpr ::std::size_t find_first_lc_any_printable_v{find_first_lc_any_printable<char_type, beg_ind, end_ind, Args...>()};

template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind>
inline constexpr ::std::size_t find_continuous_lc_and_nonlc_any_reserve_printable() noexcept
{
	return 0;
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename T, typename... Args>
inline constexpr ::std::size_t find_continuous_lc_and_nonlc_any_reserve_printable() noexcept
{
	if constexpr (end_ind == 0)
	{
		return 0;
	}
	else if constexpr (beg_ind != 0)
	{
		return find_continuous_lc_and_nonlc_any_reserve_printable<char_type, beg_ind - 1, end_ind - 1, Args...>();
	}
	else
	{
		if constexpr (::fast_io::lc_dynamic_reserve_printable<char_type, T>)
		{
			return ::fast_io::details::intrinsics::add_or_overflow_die(
				static_cast<::std::size_t>(1),
				find_continuous_lc_and_nonlc_any_reserve_printable<char_type, 0, end_ind - 1, Args...>());
		}
		else
		{
			if constexpr (::fast_io::details::decay::find_continuous_any_reserve_printable_v<char_type, 0, 1, T> == 1)
			{
				return ::fast_io::details::intrinsics::add_or_overflow_die(
					static_cast<::std::size_t>(1),
					find_continuous_lc_and_nonlc_any_reserve_printable<char_type, 0, end_ind - 1, Args...>());
			}
			else
			{
				return 0;
			}
		}
	}
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename... Args>
	requires(end_ind <= sizeof...(Args))
inline constexpr ::std::size_t find_continuous_lc_and_nonlc_any_reserve_printable_v{find_continuous_lc_and_nonlc_any_reserve_printable<char_type, beg_ind, end_ind, Args...>()};

template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind>
inline constexpr ::std::size_t find_continuous_lc_and_nonlc_any_scatter_printable() noexcept
{
	return 0;
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename T, typename... Args>
inline constexpr ::std::size_t find_continuous_lc_and_nonlc_any_scatter_printable() noexcept
{
	if constexpr (end_ind == 0)
	{
		return 0;
	}
	else if constexpr (beg_ind != 0)
	{
		return find_continuous_lc_and_nonlc_any_scatter_printable<char_type, beg_ind - 1, end_ind - 1, Args...>();
	}
	else
	{
		if constexpr (::fast_io::lc_scatter_printable<char_type, T>)
		{
			return ::fast_io::details::intrinsics::add_or_overflow_die(
				static_cast<::std::size_t>(1),
				find_continuous_lc_and_nonlc_any_scatter_printable<char_type, 0, end_ind - 1, Args...>());
		}
		else
		{
			if constexpr (::fast_io::details::decay::find_continuous_any_scatter_printable_v<char_type, 0, 1, T> == 1)
			{
				return ::fast_io::details::intrinsics::add_or_overflow_die(
					static_cast<::std::size_t>(1),
					find_continuous_lc_and_nonlc_any_scatter_printable<char_type, 0, end_ind - 1, Args...>());
			}
			else
			{
				return 0;
			}
		}
	}
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename... Args>
	requires(end_ind <= sizeof...(Args))
inline constexpr ::std::size_t find_continuous_lc_and_nonlc_any_scatter_printable_v{find_continuous_lc_and_nonlc_any_scatter_printable<char_type, beg_ind, end_ind, Args...>()};

template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind>
	requires(end_ind == 0)
inline constexpr ::std::size_t calculate_next_n_lc_and_nonlc_any_reserve_printable_size(
#if __has_cpp_attribute(maybe_unused)
	[[maybe_unused]]
#endif
	::fast_io::basic_lc_object<char_type> const *lc) noexcept
{
	return 0;
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename T, typename... Args>
	requires(end_ind <= sizeof...(Args) + 1)
inline constexpr ::std::size_t calculate_next_n_lc_and_nonlc_any_reserve_printable_size(::fast_io::basic_lc_object<char_type> const *lc, T t, Args... args)
{
	if constexpr (end_ind == 0)
	{
		return 0;
	}
	else if constexpr (beg_ind != 0)
	{
		return calculate_next_n_lc_and_nonlc_any_reserve_printable_size<char_type, beg_ind - 1, end_ind - 1>(lc, args...);
	}
	else
	{
		if constexpr (::fast_io::lc_dynamic_reserve_printable<char_type, T>)
		{
			return ::fast_io::details::intrinsics::add_or_overflow_die(
				print_reserve_size(lc, t),
				calculate_next_n_lc_and_nonlc_any_reserve_printable_size<char_type, 0, end_ind - 1>(lc, args...));
		}
		else
		{
			constexpr auto N{::fast_io::details::decay::find_first_lc_any_printable_v<char_type, beg_ind, end_ind, T, Args...>};
			return ::fast_io::details::intrinsics::add_or_overflow_die(
				::fast_io::details::decay::calculate_next_n_any_reserve_printable_size<char_type, 0, N>(t, args...),
				calculate_next_n_lc_and_nonlc_any_reserve_printable_size<char_type, N - 1, end_ind - 1>(lc, args...));
		}
	}
}

template <::std::integral char_type, ::std::size_t N>
	requires(N == 0)
inline constexpr char_type *print_next_n_continuous_any_reserve_printable(
#if __has_cpp_attribute(maybe_unused)
	[[maybe_unused]]
#endif
	::fast_io::basic_lc_object<char_type> const *lc,
	char_type *buffer)
{
	return buffer;
}
template <::std::integral char_type, ::std::size_t N, typename T, typename... Args>
	requires(N <= sizeof...(Args) + 1)
inline constexpr char_type *lc_print_next_n_continuous_any_reserve_printable(::fast_io::basic_lc_object<char_type> const *lc, char_type *buffer, T t, Args... args)
{
	if constexpr (N == 0)
	{
		return buffer;
	}
	else
	{
		if constexpr (::fast_io::lc_dynamic_reserve_printable<char_type, T>)
		{
			auto ptr{print_reserve_define(lc, buffer, t)};
			return lc_print_next_n_continuous_any_reserve_printable<char_type, N - 1>(lc, ptr, args...);
		}
		else
		{
			auto ptr{::fast_io::details::decay::print_next_n_continuous_any_reserve_printable<char_type, 1>(buffer, t)};
			return lc_print_next_n_continuous_any_reserve_printable<char_type, N - 1>(lc, ptr, args...);
		}
	}
}

template <bool line = false, typename output, typename T>
inline constexpr void lc_print_control_fallback_single(::fast_io::basic_lc_object<typename output::output_char_type> const *lc, output outstm, T t)
{
	using char_type = typename output::output_char_type;
	constexpr bool write_bytes{::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<output>};
	if constexpr (::fast_io::lc_dynamic_reserve_printable<char_type, T>)
	{
		auto const size{print_reserve_size(lc, t)};
		if constexpr (requires { typename output::output_char_type; })
		{
			::fast_io::details::local_operator_new_array_ptr<char_type, typename output::allocator_type> array;
			auto buffer{array.allocate_new(size)};
			auto ptr{print_reserve_define(lc, buffer, t)};
			::fast_io::operations::decay::write_all_decay(outstm, buffer, ptr);
		}
		else
		{
			::fast_io::details::local_operator_new_array_ptr<char_type> array;
			auto buffer{array.allocate_new(size)};
			auto ptr{print_reserve_define(lc, buffer, t)};
			::fast_io::operations::decay::write_all_decay(outstm, buffer, ptr);
		}
	}
	else if constexpr (::fast_io::lc_scatter_printable<char_type, T>)
	{
		auto scatter{print_scatter_define(lc, t)};
		if constexpr (write_bytes)
		{
			cast_to_scatter_void(scatter);
			using io_scatter_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= ::fast_io::io_scatter_t *;
			::fast_io::operations::decay::scatter_write_all_bytes_decay(outstm, reinterpret_cast<io_scatter_alias_ptr>(__builtin_addressof(scatter)), 1);
		}
		else
		{
			::fast_io::operations::decay::scatter_write_all_decay(outstm, __builtin_addressof(scatter), 1);
		}
	}
	else if constexpr (::fast_io::lc_printable<char_type, T>)
	{
		print_define(lc, outstm, t);
	}
	else
	{
		::fast_io::details::decay::print_control_fallback_single(outstm, t);
	}
}

template <::std::size_t beg_ind, ::std::size_t end_ind, typename output>
inline constexpr ::fast_io::basic_reserve_scatters_define_result<typename output::output_char_type> lc_print_control_main_loop_impl(
#if __has_cpp_attribute(maybe_unused)
	[[maybe_unused]]
#endif
	::fast_io::basic_lc_object<typename output::output_char_type> const *lc,
#if __has_cpp_attribute(maybe_unused)
	[[maybe_unused]]
#endif
	::fast_io::basic_io_scatter_t<typename output::output_char_type> *scatters_base,
	::fast_io::basic_io_scatter_t<typename output::output_char_type> *scatters,
	typename output::output_char_type *buffer,
#if __has_cpp_attribute(maybe_unused)
	[[maybe_unused]]
#endif
	output outstm)
{
	return {scatters, buffer};
}
template <::std::size_t beg_ind, ::std::size_t end_ind, typename output, typename T, typename... Args>
inline constexpr ::fast_io::basic_reserve_scatters_define_result<typename output::output_char_type> lc_print_control_main_loop_impl(::fast_io::basic_lc_object<typename output::output_char_type> const *lc, ::fast_io::basic_io_scatter_t<typename output::output_char_type> *scatters_base, ::fast_io::basic_io_scatter_t<typename output::output_char_type> *scatters, typename output::output_char_type *buffer, output outstm, T t, Args... args)
{
	if constexpr (end_ind == 0)
	{
		return {scatters, buffer};
	}
	else if constexpr (beg_ind != 0)
	{
		return lc_print_control_main_loop_impl<beg_ind - 1, end_ind - 1>(lc, scatters_base, scatters, buffer, outstm);
	}
	else
	{
		// beg_ind == 0, end_ind != 0
		using char_type = typename output::output_char_type;
		if constexpr (::fast_io::lc_dynamic_reserve_printable<char_type, T>)
		{
			constexpr auto N{find_continuous_lc_and_nonlc_any_reserve_printable_v<char_type, 0, end_ind, T, Args...>};
			auto ptr{::fast_io::details::decay::lc_print_next_n_continuous_any_reserve_printable<char_type, N>(lc, buffer, t, args...)};
			*scatters = {buffer, static_cast<::std::size_t>(ptr - buffer)};
			return lc_print_control_main_loop_impl<N - 1, end_ind - 1>(lc, scatters_base, ++scatters, ptr, outstm, args...);
		}
		else if constexpr (::fast_io::lc_scatter_printable<char_type, T>)
		{
			*scatters = print_scatter_define(lc, t);
			return lc_print_control_main_loop_impl<0, end_ind - 1>(lc, scatters_base, ++scatters, buffer, outstm, args...);
		}
		else if constexpr (::fast_io::lc_printable<char_type, T>)
		{
			::fast_io::details::decay::lc_print_control_fallback_single(lc, outstm, t);
			return lc_print_control_main_loop_impl<0, end_ind - 1>(lc, scatters_base, scatters, buffer, outstm, args...);
		}
		else
		{
			constexpr auto N{::fast_io::details::decay::find_first_lc_any_printable_v<char_type, 0, end_ind, T, Args...>};
			auto [sct_ptr, buf_ptr]{::fast_io::details::decay::print_control_main_loop_impl<0, N>(scatters_base, scatters, buffer, outstm, t, args...)};
			return lc_print_control_main_loop_impl<N - 1, end_ind - 1>(lc, scatters_base, sct_ptr, buf_ptr, outstm, args...);
		}
	}
}

template <bool line, ::std::size_t beg_ind, ::std::size_t end_ind, typename output>
inline constexpr void lc_print_control_impl(
#if __has_cpp_attribute(maybe_unused)
	[[maybe_unused]]
#endif
	::fast_io::basic_lc_object<typename output::output_char_type> const *lc,
	output outstm)
{
	if constexpr (line)
	{
		::fast_io::operations::decay::char_put_decay(outstm, ::fast_io::char_literal_v<u8'\n', typename output::output_char_type>);
	}
	return;
}
template <bool line, ::std::size_t beg_ind, ::std::size_t end_ind, typename output, typename T, typename... Args>
inline constexpr void lc_print_control_impl(::fast_io::basic_lc_object<typename output::output_char_type> const *lc, output outstm, T t, Args... args)
{
	if constexpr (end_ind == 0)
	{
		if constexpr (line)
		{
			::fast_io::operations::decay::char_put_decay(outstm, ::fast_io::char_literal_v<u8'\n', typename output::output_char_type>);
		}
		return;
	}
	else if constexpr (beg_ind != 0)
	{
		return lc_print_control_impl<line, beg_ind - 1, end_ind - 1>(outstm, args...);
	}
	else
	{
		// beg_ind == 0, end_ind != 0
		using char_type = typename output::output_char_type;
		constexpr bool write_bytes{::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<output>};
		constexpr auto raw_res_attr{::fast_io::details::decay::count_lc_static_reserve_attribute_v<char_type, beg_ind, end_ind, T, Args...>};
		constexpr auto raw_res_attr_line{::fast_io::details::decay::add_line_scatter<line, char_type>(raw_res_attr)};
		constexpr auto res_attr{::fast_io::details::decay::refine_static_reserve_attribute(raw_res_attr_line)};
		if constexpr (res_attr.has_dynamic_reserve)
		{
			auto const size{::fast_io::details::intrinsics::add_or_overflow_die(res_attr.reserved_space, ::fast_io::details::decay::calculate_all_lc_and_nonlc_dynamic_reserve_printable_size<char_type, 0, end_ind>(lc, t, args...))};
			if constexpr (requires { typename output::allocator_type; })
			{
				::fast_io::basic_io_scatter_t<char_type> scatters[res_attr.scatters_count];
				::fast_io::details::local_operator_new_array_ptr<char_type, typename output::allocator_type> buffer;
				auto ptr{buffer.allocate_new(size)};
				auto [sct_ptr, unusedbufptr]{::fast_io::details::decay::lc_print_control_main_loop_impl<beg_ind, end_ind>(lc, scatters, scatters, ptr, outstm, t, args...)};
				return ::fast_io::details::decay::print_control_write_back_impl<line, write_bytes, res_attr.last_is_reserve>(scatters, sct_ptr, outstm);
			}
			else
			{
				::fast_io::basic_io_scatter_t<char_type> scatters[res_attr.scatters_count];
				::fast_io::details::local_operator_new_array_ptr<char_type> buffer;
				auto ptr{buffer.allocate_new(size)};
				auto [sct_ptr, unusedbufptr]{::fast_io::details::decay::lc_print_control_main_loop_impl<beg_ind, end_ind>(lc, scatters, scatters, ptr, outstm, t, args...)};
				return ::fast_io::details::decay::print_control_write_back_impl<line, write_bytes, res_attr.last_is_reserve>(scatters, sct_ptr, outstm);
			}
		}
		else
		{
			::fast_io::basic_io_scatter_t<char_type> scatters[res_attr.scatters_count];
			char_type buffer[res_attr.reserved_space];
			auto [sct_ptr, unusedbufptr]{::fast_io::details::decay::lc_print_control_main_loop_impl<beg_ind, end_ind>(lc, scatters, scatters, buffer, outstm, t, args...)};
			return ::fast_io::details::decay::print_control_write_back_impl<line, write_bytes, res_attr.last_is_reserve>(scatters, sct_ptr, outstm);
		}
	}
}
template <bool line, typename output, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void lc_print_control_all_args_impl(basic_lc_object<typename output::output_char_type> const *lc, output outstm, Args... args)
{
	return ::fast_io::details::decay::lc_print_control_impl<line, 0, sizeof...(Args)>(lc, outstm, args...);
}

template <::std::size_t N, typename output>
inline constexpr void lc_pcb_continuious_n_lc_or_nonlc_any_reserve_printable_impl(
#if __has_cpp_attribute(maybe_unused)
	[[maybe_unused]]
#endif
	::fast_io::basic_lc_object<typename output::output_char_type> const *lc,
#if __has_cpp_attribute(maybe_unused)
	[[maybe_unused]]
#endif
	output outstm)
{
	return;
}
template <::std::size_t N, typename output, typename T, typename... Args>
inline constexpr void lc_pcb_continuious_n_lc_or_nonlc_any_reserve_printable_impl(::fast_io::basic_lc_object<typename output::output_char_type>* lc, output outstm, T t, Args... args)
{
	using char_type = typename output::output_char_type;
	auto const curr{obuffer_curr(outstm)};
	auto const end{obuffer_end(outstm)};
	auto const diff{end - curr};
	if (diff < 0) [[unlikely]]
	{
		// is this check necessary?
		::fast_io::fast_terminate();
	}
	auto const diff_nonneg{static_cast<::std::size_t>(diff)};
	if constexpr (::fast_io::operations::decay::defines::has_obuffer_minimum_size_operations<output>)
	{
		if constexpr (::fast_io::lc_dynamic_reserve_printable<char_type, T>)
		{
			auto const size{print_reserve_size(lc, t)};
			if (size <= diff_nonneg) [[likely]]
			{
				auto const ptr{print_reserve_define(lc, curr, t)};
				obuffer_set_curr(outstm, ptr);
				return lc_pcb_continuious_n_lc_or_nonlc_any_reserve_printable_impl<N - 1>(lc, outstm, args...);
			}
			else
			{
				constexpr auto obuffer_minimum_size_define_v{obuffer_minimum_size_define(::fast_io::io_reserve_type<char_type, output>)};
				if (size <= obuffer_minimum_size_define_v) [[likely]]
				{
					obuffer_minimum_size_flush_prepare_define(outstm);
					auto new_curr{obuffer_curr(outstm)};
					auto new_end{obuffer_end(outstm)};
					auto new_diff{new_end - new_curr};
					if (new_diff < 0) [[unlikely]]
					{
						// is this check necessary?
						::fast_io::fast_terminate();
					}
					auto new_diff_nonneg{static_cast<::std::size_t>(new_diff)};
					if (size <= new_diff_nonneg) [[likely]]
					{
						auto const ptr{print_reserve_define(::fast_io::io_reserve_type<char_type, T>, new_curr, t)};
						obuffer_set_curr(outstm, ptr);
						return lc_pcb_continuious_n_lc_or_nonlc_any_reserve_printable_impl<N - 1>(lc, outstm, args...);
					}
					else
					{
						::fast_io::details::decay::lc_print_control_fallback_single(lc, outstm, t);
						return lc_pcb_continuious_n_lc_or_nonlc_any_reserve_printable_impl<N - 1>(lc, outstm, args...);
					}
				}
				else
				{
					::fast_io::details::decay::lc_print_control_fallback_single(lc, outstm, t);
					return lc_pcb_continuious_n_lc_or_nonlc_any_reserve_printable_impl<N - 1>(lc, outstm, args...);
				}
			}
		}
		else
		{
			::fast_io::details::decay::pcb_continuous_n_any_reserve_printable_impl<1>(outstm, t);
			return lc_pcb_continuious_n_lc_or_nonlc_any_reserve_printable_impl<N - 1>(lc, outstm, args...);
		}
	}
	else if constexpr (::fast_io::operations::decay::defines::has_obuffer_flush_reserve_define<output>)
	{
		// if we can reserve the buffer, just reserve the space we need
		::std::size_t const size{::fast_io::details::decay::calculate_next_n_lc_and_nonlc_any_reserve_printable_size<char_type, 0, N>(lc, t, args...)};
		if (size <= diff_nonneg) [[likely]]
		{
			auto const ptr{::fast_io::details::decay::lc_print_next_n_continuous_any_reserve_printable<char_type, N>(lc, curr, t, args...)};
			obuffer_set_curr(outstm, ptr);
			return;
		}
		else
		{
			obuffer_flush_reserve_define(outstm, size);
			auto const new_curr{obuffer_curr(outstm)};
			auto const new_end{obuffer_end(outstm)};
			auto const new_diff{new_end - new_curr};
			if (new_diff < 0) [[unlikely]]
			{
				// is this check necessary?
				::fast_io::fast_terminate();
			}
			auto const new_diff_nonneg{static_cast<::std::size_t>(new_diff)};
			if (size <= new_diff_nonneg) [[likely]]
			{
				auto const ptr{::fast_io::details::decay::lc_print_next_n_continuous_any_reserve_printable<char_type, N>(lc, new_curr, t, args...)};
				obuffer_set_curr(outstm, ptr);
				return;
			}
			else
			{
				::fast_io::details::decay::lc_print_control_fallback_single(lc, outstm, t);
				return lc_pcb_continuious_n_lc_or_nonlc_any_reserve_printable_impl<N - 1>(lc, outstm, args...);
			}
		}
	}
	else
	{
		// the buffer do not provide any method. hope there will be enough space
		::std::size_t const size{::fast_io::details::decay::calculate_next_n_lc_and_nonlc_any_reserve_printable_size<char_type, 0, N>(lc, t, args...)};
		if (size <= diff_nonneg) [[likely]]
		{
			auto const ptr{::fast_io::details::decay::print_next_n_continuous_any_reserve_printable<char_type, N>(lc, curr, t, args...)};
			obuffer_set_curr(outstm, ptr);
			return;
		}
		else
		{
			// then there is no difference whether the buffer exist
			::fast_io::details::decay::lc_print_control_impl<false, 0, N>(outstm, t, args...);
			return;
		}
	}
}

template <bool line, ::std::size_t beg_ind, ::std::size_t end_ind, typename output>
	requires(end_ind == 0)
inline constexpr void lc_print_control_buffer_impl(output outstm)
{
	if constexpr (line)
	{
		::fast_io::operations::decay::char_put_decay(outstm, ::fast_io::char_literal_v<u8'\n', typename output::output_char_type>);
	}
	return;
}
template <bool line, ::std::size_t beg_ind, ::std::size_t end_ind, typename output, typename T, typename... Args>
	requires(end_ind <= sizeof...(Args) + 1)
inline constexpr void lc_print_control_buffer_impl(::fast_io::basic_lc_object<typename output::output_char_type> const *lc, output outstm, T t, Args... args)
{
	if constexpr (end_ind == 0)
	{
		if constexpr (line)
		{
			::fast_io::operations::decay::char_put_decay(outstm, ::fast_io::char_literal_v<u8'\n', typename output::output_char_type>);
		}
		return;
	}
	else if constexpr (beg_ind != 0)
	{
		return lc_print_control_buffer_impl<line, beg_ind - 1, end_ind - 1>(outstm, args...);
	}
	else
	{
		// beg_ind == 0, end_ind != 0
		using char_type = typename output::output_char_type;
		if constexpr (::fast_io::lc_dynamic_reserve_printable<char_type, T>)
		{
			constexpr auto N{::fast_io::details::decay::find_continuous_lc_and_nonlc_any_reserve_printable_v<char_type, 0, end_ind, T, Args...>};
			// TODO

			return lc_print_control_buffer_impl<line, N - 1, end_ind - 1>(lc, outstm, args...);
		}
		else if constexpr (::fast_io::lc_scatter_printable<char_type, T>)
		{
			constexpr auto N{::fast_io::details::decay::find_continuous_lc_and_nonlc_any_scatter_printable_v<char_type, 0, end_ind, T, Args...>};
			::fast_io::details::decay::lc_print_control_impl<false, 0, N>(lc, outstm, t);
			return lc_print_control_buffer_impl<line, N - 1, end_ind - 1>(lc, outstm, args...);
		}
		else if constexpr (::fast_io::lc_printable<char_type, T>)
		{
			::fast_io::details::decay::lc_print_control_fallback_single(lc, outstm, t);
			return lc_print_control_buffer_impl<line, 0, end_ind - 1>(lc, outstm, args...);
		}
		else
		{
			constexpr auto N{::fast_io::details::decay::find_first_lc_any_printable_v<char_type, 0, end_ind, T, Args...>};
			::fast_io::details::decay::print_control_buffer_impl<false, 0, N>(lc, outstm, t);
			return lc_print_control_buffer_impl<line, N - 1, end_ind - 1>(lc, outstm, args...);
		}
	}
}

} // namespace details::decay

namespace operations::decay
{

template <bool line, typename output, typename... Args>
inline constexpr void lc_status_print_define_decay(::fast_io::basic_lc_object<typename output::output_char_type> const *lc, output outstm, Args... args)
{
	if constexpr (sizeof...(Args) == 0)
	{
		if constexpr (line)
		{
			using char_type = typename output::output_char_type;
			return ::fast_io::operations::decay::char_put_decay(outstm, ::fast_io::char_literal_v<u8'\n', char_type>);
		}
		else
		{
			return;
		}
	}
	else if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<output>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::output_stream_mutex_ref_decay(outstm)};
		return lc_status_print_define_decay<line>(lc, ::fast_io::operations::decay::output_stream_unlocked_ref_decay(outstm), args...);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_obuffer_basic_operations<output>)
	{
		// return ::fast_io::details::decay::lc_print_control_buffer_all_args_impl<line>(lc, outstm, args...);
	}
	else
	{
		return ::fast_io::details::decay::lc_print_control_all_args_impl<line>(lc, outstm, args...);
	}
}

} // namespace operations::decay

template <bool line, typename output, typename... Args>
inline constexpr void status_print_define(::fast_io::lc_imbuer<output> imb, Args... args)
{
	::fast_io::operations::decay::lc_status_print_define_decay<line>(imb.locale, imb.handle, args...);
}

} // namespace fast_io
