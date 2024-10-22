#pragma once

namespace fast_io
{

namespace details::decay
{

struct static_reserve_attribute_t
{
	::std::size_t scatters_count;
	::std::size_t reserved_space;
	bool last_is_reserve;
	bool has_dynamic_reserve;
};

template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind>
inline constexpr static_reserve_attribute_t count_static_reserve_attribute(static_reserve_attribute_t previous = {}) noexcept
{
	return previous;
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename T, typename... Args>
inline constexpr static_reserve_attribute_t count_static_reserve_attribute(static_reserve_attribute_t previous = {}) noexcept
{
	if constexpr (end_ind == 0)
	{
		return previous;
	}
	else if constexpr (beg_ind != 0)
	{
		return count_static_reserve_attribute<char_type, beg_ind - 1, end_ind - 1, Args...>(previous);
	}
	else
	{
		// beg_ind == 0, end_ind != 0
		if constexpr (::fast_io::reserve_printable<char_type, T> || ::fast_io::dynamic_reserve_printable<char_type, T>)
		{
			if (!previous.last_is_reserve)
			{
				previous.scatters_count = ::fast_io::details::intrinsics::add_or_overflow_die(previous.scatters_count, static_cast<::std::size_t>(1));
			}
			if constexpr (::fast_io::reserve_printable<char_type, T>)
			{
				previous.reserved_space = ::fast_io::details::intrinsics::add_or_overflow_die(previous.reserved_space, print_reserve_size(::fast_io::io_reserve_type<char_type, T>));
			}
			else if constexpr (::fast_io::dynamic_reserve_printable<char_type, T>)
			{
				previous.has_dynamic_reserve = true;
			}
			previous.last_is_reserve = true;
		}
		else if constexpr (::fast_io::scatter_printable<char_type, T>)
		{
			previous.scatters_count = ::fast_io::details::intrinsics::add_or_overflow_die(previous.scatters_count, static_cast<::std::size_t>(1));
			previous.last_is_reserve = false;
		}
		else if constexpr (::fast_io::reserve_scatters_printable<char_type, T>)
		{
			constexpr auto sct_rsv{print_reserve_scatters_size(::fast_io::io_reserve_type<char_type, T>)};
			previous.scatters_count = ::fast_io::details::intrinsics::add_or_overflow_die(previous.scatters_count, sct_rsv.scatters_size);
			previous.reserved_space = ::fast_io::details::intrinsics::add_or_overflow_die(previous.reserved_space, sct_rsv.reserve_size);
			previous.last_is_reserve = false;
		}
		else
		{
			previous.last_is_reserve = false;
		}
		return count_static_reserve_attribute<char_type, 0, end_ind - 1, Args...>(previous);
	}
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename... Args>
	requires(end_ind <= sizeof...(Args))
inline constexpr static_reserve_attribute_t count_static_reserve_attribute_v = count_static_reserve_attribute<char_type, beg_ind, end_ind, Args...>();

template <bool line, ::std::integral char_type>
inline constexpr static_reserve_attribute_t add_line_scatter(static_reserve_attribute_t attr) noexcept
{
	if constexpr (line)
	{
		if (attr.last_is_reserve)
		{
			attr.reserved_space = ::fast_io::details::intrinsics::add_or_overflow_die(attr.reserved_space, sizeof(char_type));
		}
		else
		{
			attr.scatters_count = ::fast_io::details::intrinsics::add_or_overflow_die(attr.scatters_count, static_cast<::std::size_t>(1));
		}
	}
	return attr;
}

inline constexpr static_reserve_attribute_t refine_static_reserve_attribute(static_reserve_attribute_t attr) noexcept
{
	// fight against the compiler, cannot allocate an array of constant size 0
	if (attr.scatters_count == 0)
	{
		attr.scatters_count = 1;
	}
	if (attr.reserved_space == 0)
	{
		attr.reserved_space = 1;
	}
	return attr;
}

template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind>
	requires(end_ind == 0)
inline constexpr ::std::size_t calculate_all_dynamic_reserve_size() noexcept
{
	return 0;
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename T, typename... Args>
	requires(end_ind <= sizeof...(Args) + 1)
inline constexpr ::std::size_t calculate_all_dynamic_reserve_size(T t, Args... args)
{
	if constexpr (end_ind == 0)
	{
		return 0;
	}
	else if constexpr (beg_ind != 0)
	{
		return calculate_all_dynamic_reserve_size<char_type, beg_ind - 1, end_ind - 1, Args...>();
	}
	else
	{
		if constexpr (::fast_io::dynamic_reserve_printable<char_type, T>)
		{
			return ::fast_io::details::intrinsics::add_or_overflow_die(
				print_reserve_size(::fast_io::io_reserve_type<char_type, T>, t),
				calculate_all_dynamic_reserve_size<char_type, 0, end_ind - 1>(args...));
		}
		else
		{
			return calculate_all_dynamic_reserve_size<char_type, 0, end_ind - 1>(args...);
		}
	}
}

template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind>
inline constexpr ::std::size_t find_continuous_reserve_printable() noexcept
{
	return 0;
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename T, typename... Args>
inline constexpr ::std::size_t find_continuous_reserve_printable() noexcept
{
	if constexpr (end_ind == 0)
	{
		return 0;
	}
	else if constexpr (beg_ind != 0)
	{
		return find_continuous_reserve_printable<char_type, beg_ind - 1, end_ind - 1, Args...>();
	}
	else
	{
		if constexpr (::fast_io::reserve_printable<char_type, T>)
		{
			return ::fast_io::details::intrinsics::add_or_overflow_die(
				static_cast<::std::size_t>(1),
				find_continuous_reserve_printable<char_type, 0, end_ind - 1, Args...>());
		}
		else
		{
			return 0;
		}
	}
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename... Args>
	requires(end_ind <= sizeof...(Args))
inline constexpr ::std::size_t find_continuous_reserve_printable_v = find_continuous_reserve_printable<char_type, beg_ind, end_ind, Args...>();

template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind>
inline constexpr ::std::size_t find_continuous_any_reserve_printable() noexcept
{
	return 0;
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename T, typename... Args>
inline constexpr ::std::size_t find_continuous_any_reserve_printable() noexcept
{
	if constexpr (end_ind == 0)
	{
		return 0;
	}
	else if constexpr (beg_ind != 0)
	{
		return find_continuous_any_reserve_printable<char_type, beg_ind - 1, end_ind - 1, Args...>();
	}
	else
	{
		if constexpr (::fast_io::reserve_printable<char_type, T> || ::fast_io::dynamic_reserve_printable<char_type, T>)
		{
			return ::fast_io::details::intrinsics::add_or_overflow_die(
				static_cast<::std::size_t>(1),
				find_continuous_any_reserve_printable<char_type, 0, end_ind - 1, Args...>());
		}
		else
		{
			return 0;
		}
	}
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename... Args>
	requires(end_ind <= sizeof...(Args))
inline constexpr ::std::size_t find_continuous_any_reserve_printable_v = find_continuous_any_reserve_printable<char_type, beg_ind, end_ind, Args...>();

template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind>
inline constexpr ::std::size_t find_continuous_any_scatter_printable() noexcept
{
	return 0;
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename T, typename... Args>
inline constexpr ::std::size_t find_continuous_any_scatter_printable() noexcept
{
	if constexpr (end_ind == 0)
	{
		return 0;
	}
	else if constexpr (beg_ind != 0)
	{
		return find_continuous_any_scatter_printable<char_type, beg_ind - 1, end_ind - 1, Args...>();
	}
	else
	{
		if constexpr (::fast_io::scatter_printable<char_type, T> || ::fast_io::reserve_scatters_printable<char_type, T>)
		{
			return ::fast_io::details::intrinsics::add_or_overflow_die(
				static_cast<::std::size_t>(1),
				find_continuous_any_scatter_printable<char_type, 0, end_ind - 1, Args...>());
		}
		else
		{
			return 0;
		}
	}
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename... Args>
	requires(end_ind <= sizeof...(Args))
inline constexpr ::std::size_t find_continuous_any_scatter_printable_v = find_continuous_any_scatter_printable<char_type, beg_ind, end_ind, Args...>();

template <::std::integral char_type, ::std::size_t N>
inline constexpr ::std::size_t calculate_next_n_reserve_printable_size() noexcept
{
	return 0;
}
template <::std::integral char_type, ::std::size_t N, typename T, typename... Args>
inline constexpr ::std::size_t calculate_next_n_reserve_printable_size() noexcept
{
	if constexpr (N == 0)
	{
		return 0;
	}
	else
	{
		return ::fast_io::details::intrinsics::add_or_overflow_die(
			print_reserve_size(::fast_io::io_reserve_type<char_type, T>),
			calculate_next_n_reserve_printable_size<char_type, N - 1, Args...>());
	}
}
template <::std::integral char_type, ::std::size_t N, typename... Args>
	requires(N <= sizeof...(Args))
inline constexpr ::std::size_t calculate_next_n_reserve_printable_size_v = calculate_next_n_reserve_printable_size<char_type, N, Args...>();

template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind>
	requires(end_ind == 0)
inline constexpr ::std::size_t calculate_next_n_any_reserve_printable_size() noexcept
{
	return 0;
}
template <::std::integral char_type, ::std::size_t beg_ind, ::std::size_t end_ind, typename T, typename... Args>
	requires(end_ind <= sizeof...(Args) + 1)
inline constexpr ::std::size_t calculate_next_n_any_reserve_printable_size(T t, Args... args) noexcept
{
	if constexpr (end_ind == 0)
	{
		return 0;
	}
	else if constexpr (beg_ind != 0)
	{
		return calculate_next_n_any_reserve_printable_size<char_type, beg_ind - 1, end_ind - 1>(args...);
	}
	else
	{
		constexpr auto N{::fast_io::details::decay::find_continuous_reserve_printable_v<char_type, 0, end_ind, T, Args...>};
		if constexpr (N == 0)
		{
			return ::fast_io::details::intrinsics::add_or_overflow_die(
				print_reserve_size(::fast_io::io_reserve_type<char_type, T>, t),
				calculate_next_n_any_reserve_printable_size<char_type, 0, end_ind - 1>(args...));
		}
		else
		{
			constexpr auto size{::fast_io::details::decay::calculate_next_n_reserve_printable_size_v<char_type, N, T, Args...>};
			return ::fast_io::details::intrinsics::add_or_overflow_die(size,
																	   calculate_next_n_any_reserve_printable_size<char_type, N - 1, end_ind - 1>(args...));
		}
	}
}

template <::std::integral char_type, ::std::size_t N>
	requires(N == 0)
inline constexpr char_type *print_next_n_continuous_any_reserve_printable(char_type *buffer)
{
	return buffer;
}
template <::std::integral char_type, ::std::size_t N, typename T, typename... Args>
	requires(N <= sizeof...(Args) + 1)
inline constexpr char_type *print_next_n_continuous_any_reserve_printable(char_type *buffer, T t, Args... args)
{
	if constexpr (N == 0)
	{
		return buffer;
	}
	else
	{
		return print_next_n_continuous_any_reserve_printable<char_type, N - 1>(print_reserve_define(::fast_io::io_reserve_type<char_type, T>, buffer, t), args...);
	}
}

template <::std::integral char_type>
inline constexpr void cast_to_scatter_void(::fast_io::basic_io_scatter_t<char_type> &sct) noexcept
{
	sct.len *= sizeof(char_type);
}

template <typename output, typename T>
inline constexpr void print_control_fallback_single(output outstm, T t)
{
	using char_type = typename output::output_char_type;
	constexpr bool write_bytes{::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<output>};
	if constexpr (::fast_io::reserve_printable<char_type, T>)
	{
		char_type buffer[print_reserve_size(::fast_io::io_reserve_type<char_type, T>)];
		auto ptr{print_reserve_define(::fast_io::io_reserve_type<char_type, T>, buffer, t)};
		::fast_io::operations::decay::write_all_decay(outstm, buffer, ptr);
	}
	else if constexpr (::fast_io::dynamic_reserve_printable<char_type, T>)
	{
		auto const size{print_reserve_size(::fast_io::io_reserve_type<char_type, T>, t)};
		if constexpr (requires { typename output::allocator_type; })
		{
			::fast_io::details::local_operator_new_array_ptr<char_type, typename output::allocator_type> array;
			auto buffer{array.allocate_new(size)};
			auto ptr{print_reserve_define(::fast_io::io_reserve_type<char_type, T>, buffer, t)};
			::fast_io::operations::decay::write_all_decay(outstm, buffer, ptr);
		}
		else
		{
			::fast_io::details::local_operator_new_array_ptr<char_type> array;
			auto buffer{array.allocate_new(size)};
			auto ptr{print_reserve_define(::fast_io::io_reserve_type<char_type, T>, buffer, t)};
			::fast_io::operations::decay::write_all_decay(outstm, buffer, ptr);
		}
	}
	else if constexpr (::fast_io::scatter_printable<char_type, T>)
	{
		auto scatter{print_scatter_define(::fast_io::io_reserve_type<char_type, T>, t)};
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
	else if constexpr (::fast_io::reserve_scatters_printable<char_type, T>)
	{
		constexpr auto sct_rsv{print_reserve_scatters_size(::fast_io::io_reserve_type<char_type, T>)};
		::fast_io::basic_io_scatter_t<char_type> scatters[sct_rsv.scatters_size];
		char_type buffer[sct_rsv.reserve_size];
		auto const [sct_ptr, buf_ptr]{print_reserve_scatters_define(::fast_io::io_reserve_type<char_type, T>, scatters, buffer, t)};
		if constexpr (write_bytes)
		{
			using scatter_char_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= ::fast_io::basic_io_scatter_t<char_type> *;
			using io_scatter_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= ::fast_io::io_scatter_t *;
			for (scatter_char_alias_ptr ptr{scatters}; ptr != sct_ptr; ++ptr)
			{
				cast_to_scatter_void(*ptr);
			}
			::fast_io::operations::decay::scatter_write_all_bytes_decay(outstm, reinterpret_cast<io_scatter_alias_ptr>(scatters), static_cast<::std::size_t>(sct_ptr - scatters));
		}
		else
		{
			::fast_io::operations::decay::scatter_write_all_decay(outstm, scatters, static_cast<::std::size_t>(sct_ptr - scatters));
		}
	}
	else if constexpr (::fast_io::context_printable<char_type, T>)
	{
		constexpr ::std::size_t buffer_size{256 / sizeof(char_type)};
		char_type buffer[buffer_size];
		typename ::std::remove_cvref_t<decltype(print_context_type(io_reserve_type<char_type, T>))>::type state;
		for (::fast_io::context_print_result<char_type *> print_result{}; !print_result.done;)
		{
			print_result = state.print_context_define(t, buffer, buffer + buffer_size);
			::fast_io::operations::decay::write_all_decay(outstm, buffer, print_result.ptr);
		}
	}
	else if constexpr (::fast_io::transcode_imaginary_printable<char_type, T>)
	{
		// TODO
	}
	else if constexpr (::fast_io::printable<char_type, T>)
	{
		print_define(::fast_io::io_reserve_type<char_type, T>, outstm, t);
	}
	else
	{
		static_assert(::fast_io::printable<char_type, T>, "not printable");
	}
	return;
}

template <bool line, bool write_bytes, bool last_is_reserve, typename output>
inline constexpr void print_control_write_back_impl(::fast_io::basic_io_scatter_t<typename output::output_char_type> *scatters_base, ::fast_io::basic_io_scatter_t<typename output::output_char_type> *scatters, output outstm)
{
	using char_type = typename output::output_char_type;
	if constexpr (line)
	{
		if constexpr (last_is_reserve)
		{
			// should guarentee that the last scatter, which is generated by *any*_reserve_printable,
			// has allocated the space for line feed, and also is on the writable area(which is not a string literal)
			--scatters;
			const_cast<char_type *>(scatters->base)[scatters->len] = ::fast_io::char_literal_v<u8'\n', char_type>;
			++scatters->len;
			++scatters;
		}
		else
		{
			*scatters = {__builtin_addressof(::fast_io::char_literal_v<u8'\n', char_type>), static_cast<::std::size_t>(1)};
			++scatters;
		}
	}
	if constexpr (write_bytes)
	{
		using scatter_char_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif // __has_cpp_attribute([[__gnu__::__may_alias__]])
			= ::fast_io::basic_io_scatter_t<char_type> *;
		using io_scatter_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif // __has_cpp_attribute([[__gnu__::__may_alias__]])
			= ::fast_io::io_scatter_t *;
		for (scatter_char_alias_ptr ptr{scatters_base}; ptr != scatters; ++ptr)
		{
			cast_to_scatter_void(*ptr);
		}
		::fast_io::operations::decay::scatter_write_all_bytes_decay(outstm, reinterpret_cast<io_scatter_alias_ptr>(scatters_base), static_cast<::std::size_t>(scatters - scatters_base));
	}
	else
	{
		::fast_io::operations::decay::scatter_write_all_decay(outstm, scatters_base, static_cast<::std::size_t>(scatters - scatters_base));
	}
}

template <bool line, ::std::size_t beg_ind, ::std::size_t end_ind, bool write_bytes, bool last_is_reserve, typename output>
	requires(end_ind == 0)
inline constexpr void print_control_main_loop_impl(::fast_io::basic_io_scatter_t<typename output::output_char_type> *scatters_base, ::fast_io::basic_io_scatter_t<typename output::output_char_type> *scatters,
#if __has_cpp_attribute(maybe_unused)
												   [[maybe_unused]]
#endif
												   typename output::output_char_type *buffer,
												   output outstm)
{
	return print_control_write_back_impl<line, write_bytes, last_is_reserve>(scatters_base, scatters, outstm);
}
template <bool line, ::std::size_t beg_ind, ::std::size_t end_ind, bool write_bytes, bool last_is_reserve, typename output, typename T, typename... Args>
	requires(end_ind <= sizeof...(Args) + 1)
inline constexpr void print_control_main_loop_impl(::fast_io::basic_io_scatter_t<typename output::output_char_type> *scatters_base, ::fast_io::basic_io_scatter_t<typename output::output_char_type> *scatters, typename output::output_char_type *buffer, output outstm, T t, Args... args)
{
	if constexpr (end_ind == 0)
	{
		return print_control_write_back_impl<line, write_bytes, last_is_reserve>(scatters_base, scatters, outstm);
	}
	else if constexpr (beg_ind != 0)
	{
		return print_control_main_loop_impl<line, beg_ind - 1, end_ind - 1, write_bytes, last_is_reserve>(scatters_base, scatters, buffer, outstm, args...);
	}
	else
	{
		// beg_ind == 0, end_ind != 0
		using char_type = typename output::output_char_type;
		if constexpr (::fast_io::reserve_printable<char_type, T> || ::fast_io::dynamic_reserve_printable<char_type, T>)
		{
			constexpr auto N{::fast_io::details::decay::find_continuous_any_reserve_printable_v<char_type, beg_ind, end_ind, T, Args...>};
			auto ptr{::fast_io::details::decay::print_next_n_continuous_any_reserve_printable<char_type, N>(buffer, t, args...)};
			*scatters = {buffer, static_cast<::std::size_t>(ptr - buffer)};
			return print_control_main_loop_impl<line, N - 1, end_ind - 1, write_bytes, last_is_reserve>(scatters_base, ++scatters, ptr, outstm, args...);
		}
		else if constexpr (::fast_io::scatter_printable<char_type, T>)
		{
			*scatters = print_scatter_define(::fast_io::io_reserve_type<char_type, T>, t);
			return print_control_main_loop_impl<line, 0, end_ind - 1, write_bytes, last_is_reserve>(scatters_base, ++scatters, buffer, outstm, args...);
		}
		else if constexpr (::fast_io::reserve_scatters_printable<char_type, T>)
		{
			auto [sct, rsv]{print_reserve_scatters_define(::fast_io::io_reserve_type<char_type, T>, scatters, buffer, t)};
			return print_control_main_loop_impl<line, 0, end_ind - 1, write_bytes, last_is_reserve>(scatters_base, sct, rsv, outstm, args...);
		}
		else if constexpr (::fast_io::transcode_imaginary_printable<char_type, T>)
		{
			::fast_io::details::decay::print_control_fallback_single(outstm, t);
			return print_control_main_loop_impl<line, 0, end_ind - 1, write_bytes, last_is_reserve>(scatters_base, scatters, buffer, outstm, args...);
		}
		else if constexpr (::fast_io::context_printable<char_type, T>)
		{
			::fast_io::details::decay::print_control_fallback_single(outstm, t);
			return print_control_main_loop_impl<line, 0, end_ind - 1, write_bytes, last_is_reserve>(scatters_base, scatters, buffer, outstm, args...);
		}
		else if constexpr (::fast_io::printable<char_type, T>)
		{
			::fast_io::details::decay::print_control_fallback_single(outstm, t);
			return print_control_main_loop_impl<line, 0, end_ind - 1, write_bytes, last_is_reserve>(scatters_base, scatters, buffer, outstm, args...);
		}
		else
		{
			static_assert(::fast_io::printable<char_type, T>, "not printable");
		}
	}
}

template <bool line, ::std::size_t beg_ind, ::std::size_t end_ind, typename output, typename T, typename... Args>
	requires(end_ind == 0)
inline constexpr void print_control_impl(output outstm, T t, Args... args)
{
	if constexpr (line)
	{
		::fast_io::operations::decay::char_put_decay(outstm, ::fast_io::char_literal_v<u8'\n', typename output::output_char_type>);
	}
	return;
}
template <bool line, ::std::size_t beg_ind, ::std::size_t end_ind, typename output, typename T, typename... Args>
	requires(end_ind <= sizeof...(Args) + 1)
inline constexpr void print_control_impl(output outstm, T t, Args... args)
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
		return print_control_impl<line, beg_ind - 1, end_ind - 1>(outstm, args...);
	}
	else
	{
		// beg_ind == 0, end_ind != 0
		using char_type = typename output::output_char_type;
		constexpr bool write_bytes{::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<output>};
		constexpr auto raw_res_attr{::fast_io::details::decay::count_static_reserve_attribute_v<char_type, beg_ind, end_ind, T, Args...>};
		constexpr auto raw_res_attr_line{::fast_io::details::decay::add_line_scatter<line, char_type>(raw_res_attr)};
		constexpr auto res_attr{::fast_io::details::decay::refine_static_reserve_attribute(raw_res_attr_line)};
		if constexpr (res_attr.has_dynamic_reserve)
		{
			auto const size{::fast_io::details::intrinsics::add_or_overflow_die(res_attr.reserved_space, ::fast_io::details::decay::calculate_all_dynamic_reserve_size<char_type, 0, end_ind>(t, args...))};
			if constexpr (requires { typename output::allocator_type; })
			{
				::fast_io::basic_io_scatter_t<char_type> scatters[res_attr.scatters_count];
				::fast_io::details::local_operator_new_array_ptr<char_type, typename output::allocator_type> buffer;
				auto ptr{buffer.allocate_new(size)};
				return ::fast_io::details::decay::print_control_main_loop_impl<line, beg_ind, end_ind, write_bytes, res_attr.last_is_reserve>(scatters, scatters, ptr, outstm, t, args...);
			}
			else
			{
				::fast_io::basic_io_scatter_t<char_type> scatters[res_attr.scatters_count];
				::fast_io::details::local_operator_new_array_ptr<char_type> buffer;
				auto ptr{buffer.allocate_new(size)};
				return ::fast_io::details::decay::print_control_main_loop_impl<line, beg_ind, end_ind, write_bytes, res_attr.last_is_reserve>(scatters, scatters, ptr, outstm, t, args...);
			}
		}
		else
		{
			::fast_io::basic_io_scatter_t<char_type> scatters[res_attr.scatters_count];
			char_type buffer[res_attr.reserved_space];
			return ::fast_io::details::decay::print_control_main_loop_impl<line, beg_ind, end_ind, write_bytes, res_attr.last_is_reserve>(scatters, scatters, buffer, outstm, t, args...);
		}
	}
}
template <bool line, typename output, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void print_control_all_args_impl(output outstm, Args... args)
{
	return ::fast_io::details::decay::print_control_impl<line, 0, sizeof...(Args)>(outstm, args...);
}

template <::std::size_t N, typename output, typename T, typename... Args>
inline constexpr void pcb_continuous_N_any_reserve_printable_impl(output outstm, T t, Args... args)
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
	if constexpr (N == 1)
	{
		::std::size_t size{};
		if constexpr (::fast_io::reserve_printable<char_type, T>)
		{
			size = print_reserve_size(::fast_io::io_reserve_type<char_type, T>);
		}
		else if constexpr (::fast_io::dynamic_reserve_printable<char_type, T>)
		{
			size = print_reserve_size(::fast_io::io_reserve_type<char_type, T>, t);
		}
		if (size <= diff_nonneg) [[likely]]
		{
			auto const ptr{print_reserve_define(::fast_io::io_reserve_type<char_type, T>, curr, t)};
			obuffer_set_curr(outstm, ptr);
			return;
		}
		else
		{
			::fast_io::details::decay::print_control_fallback_single(outstm, t);
			return;
		}
	}
	else
	{
		if constexpr (::fast_io::operations::decay::defines::has_obuffer_minimum_size_operations<output>)
		{
			// if we can get the minimum size of the buffer, we can just print args one by one
			if constexpr (::fast_io::reserve_printable<char_type, T>)
			{
				constexpr auto size{print_reserve_size(::fast_io::io_reserve_type<char_type, T>)};
				constexpr auto obuffer_minimum_size_define_v{obuffer_minimum_size_define(::fast_io::io_reserve_type<char_type, outstm>)};
				if constexpr (size > obuffer_minimum_size_define_v)
				{
					if (size <= diff_nonneg) [[unlikely]]
					{
						auto const ptr{print_reserve_define(::fast_io::io_reserve_type<char_type, T>, curr, t)};
						obuffer_set_curr(outstm, ptr);
						return ::fast_io::details::decay::pcb_continuous_N_any_reserve_printable_impl<N - 1>(outstm, args...);
					}
					else
					{
						::fast_io::details::decay::print_control_fallback_single(outstm, t);
						return ::fast_io::details::decay::pcb_continuous_N_any_reserve_printable_impl<N - 1>(outstm, args...);
					}
				}
				else
				{
					if (size <= diff_nonneg) [[likely]]
					{
						auto const ptr{print_reserve_define(::fast_io::io_reserve_type<char_type, T>, curr, t)};
						obuffer_set_curr(outstm, ptr);
						return ::fast_io::details::decay::pcb_continuous_N_any_reserve_printable_impl<N - 1>(outstm, args...);
					}
					else
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
							return ::fast_io::details::decay::pcb_continuous_N_any_reserve_printable_impl<N - 1>(outstm, args...);
						}
						else
						{
							::fast_io::details::decay::print_control_fallback_single(outstm, t);
							return ::fast_io::details::decay::pcb_continuous_N_any_reserve_printable_impl<N - 1>(outstm, args...);
						}
					}
				}
			}
			else if constexpr (::fast_io::dynamic_reserve_printable<char_type, T>)
			{
				auto const size{print_reserve_size(::fast_io::io_reserve_type<char_type, T>, t)};
				if (size <= diff_nonneg) [[likely]]
				{
					auto const ptr{print_reserve_define(::fast_io::io_reserve_type<char_type, T>, curr, t)};
					obuffer_set_curr(outstm, ptr);
					return ::fast_io::details::decay::pcb_continuous_N_any_reserve_printable_impl<N - 1>(outstm, args...);
				}
				else
				{
					constexpr auto obuffer_minimum_size_define_v{obuffer_minimum_size_define(::fast_io::io_reserve_type<char_type, outstm>)};
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
							return ::fast_io::details::decay::pcb_continuous_N_any_reserve_printable_impl<N - 1>(outstm, args...);
						}
						else
						{
							::fast_io::details::decay::print_control_fallback_single(outstm, t);
							return ::fast_io::details::decay::pcb_continuous_N_any_reserve_printable_impl<N - 1>(outstm, args...);
						}
					}
					else
					{
						::fast_io::details::decay::print_control_fallback_single(outstm, t);
						return ::fast_io::details::decay::pcb_continuous_N_any_reserve_printable_impl<N - 1>(outstm, args...);
					}
				}
			}
		}
		else if constexpr (::fast_io::operations::decay::defines::has_obuffer_flush_reserve_define<output>)
		{
			// if we can reserve the buffer, just reserve the space we need
			::std::size_t const size{calculate_next_n_any_reserve_printable_size<char_type, 0, N>(t, args...)};
			if (size <= diff_nonneg) [[likely]]
			{
				auto const ptr{print_next_n_continuous_any_reserve_printable<char_type, N>(curr, t, args...)};
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
					auto const ptr{print_next_n_continuous_any_reserve_printable<char_type, N>(new_curr, t, args...)};
					obuffer_set_curr(outstm, ptr);
					return;
				}
				else
				{
					::fast_io::details::decay::print_control_fallback_single(outstm, t);
					return ::fast_io::details::decay::pcb_continuous_N_any_reserve_printable_impl<N - 1>(outstm, args...);
				}
			}
		}
		else
		{
			// the buffer do not provide any method. hope there will be enough space
			::std::size_t const size{calculate_next_n_any_reserve_printable_size<char_type, 0, N>(t, args...)};
			if (size <= diff_nonneg) [[likely]]
			{
				auto const ptr{print_next_n_continuous_any_reserve_printable<char_type, N>(curr, t, args...)};
				obuffer_set_curr(outstm, ptr);
				return;
			}
			else
			{
				// then there is no difference whether the buffer exist
				::fast_io::details::decay::print_control_impl<false, 0, N>(outstm, t, args...);
				return;
			}
		}
	}
}

template <typename output, typename T>
inline constexpr void pcb_context_printable_impl(output outstm, T t)
{
	using char_type = typename output::output_char_type;
	auto curr{obuffer_curr(outstm)};
	auto end{obuffer_end(outstm)};
	typename ::std::remove_cvref_t<decltype(print_context_type(io_reserve_type<char_type, T>))>::type state;
	if constexpr (::fast_io::operations::decay::defines::has_obuffer_minimum_size_operations<output>)
	{
		for (auto print_result{state.print_context_define(t, curr, end)}; !print_result.done;)
		{
			obuffer_minimum_size_flush_prepare_define(outstm);
			curr = obuffer_curr(outstm);
			end = obuffer_end(outstm);
			print_result = state.print_context_define(t, curr, end);
		}
		return;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_obuffer_flush_reserve_define<output>)
	{
		for (auto print_result{state.print_context_define(t, curr, end)}; !print_result.done;)
		{
			obuffer_flush_reserve_define(outstm, 256 / sizeof(char_type));
			curr = obuffer_curr(outstm);
			end = obuffer_end(outstm);
			print_result = state.print_context_define(t, curr, end);
		}
		return;
	}
	else
	{
		print_control_fallback_single(outstm, t);
		return;
	}
}

template <bool line, ::std::size_t beg_ind, ::std::size_t end_ind, typename output>
	requires(end_ind == 0)
inline constexpr void print_control_buffer_impl(output outstm)
{
	if constexpr (line)
	{
		::fast_io::operations::decay::char_put_decay(outstm, ::fast_io::char_literal_v<u8'\n', typename output::output_char_type>);
	}
	return;
}
template <bool line, ::std::size_t beg_ind, ::std::size_t end_ind, typename output, typename T, typename... Args>
	requires(end_ind <= sizeof...(Args) + 1)
inline constexpr void print_control_buffer_impl(output outstm, T t, Args... args)
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
		return print_control_buffer_impl<line, beg_ind - 1, end_ind - 1>(outstm, args...);
	}
	else
	{
		// beg_ind == 0, end_ind != 0
		using char_type = typename output::output_char_type;
		if constexpr (::fast_io::reserve_printable<char_type, T> || ::fast_io::dynamic_reserve_printable<char_type, T>)
		{
			constexpr auto N{::fast_io::details::decay::find_continuous_any_reserve_printable_v<char_type, 0, end_ind, T, Args...>};
			::fast_io::details::decay::pcb_continuous_N_any_reserve_printable_impl<N>(outstm, t, args...);
			return ::fast_io::details::decay::print_control_buffer_impl<line, N - 1, end_ind - 1>(outstm, args...);
		}
		else if constexpr (::fast_io::scatter_printable<char_type, T> || ::fast_io::reserve_scatters_printable<char_type, T>)
		{
			// we can't make use of buffer here
			constexpr auto N{::fast_io::details::decay::find_continuous_any_scatter_printable_v<char_type, 0, end_ind, T, Args...>};
			::fast_io::details::decay::print_control_impl<false, 0, N>(outstm, t, args...);
			return ::fast_io::details::decay::print_control_buffer_impl<line, N - 1, end_ind - 1>(outstm, args...);
		}
		else if constexpr (::fast_io::transcode_imaginary_printable<char_type, T>)
		{
			::fast_io::details::decay::print_control_fallback_single(outstm, t);
			return ::fast_io::details::decay::print_control_buffer_impl<line, 0, end_ind - 1>(outstm, args...);
		}
		else if constexpr (::fast_io::context_printable<char_type, T>)
		{
			::fast_io::details::decay::pcb_context_printable_impl(outstm, t);
			return ::fast_io::details::decay::print_control_buffer_impl<line, 0, end_ind - 1>(outstm, args...);
		}
		else if constexpr (::fast_io::printable<char_type, T>)
		{
			::fast_io::details::decay::print_control_fallback_single(outstm, t);
			return ::fast_io::details::decay::print_control_buffer_impl<line, 0, end_ind - 1>(outstm, args...);
		}
		else
		{
			static_assert(::fast_io::printable<char_type, T>, "not printable");
		}
	}
}
template <bool line, typename output, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void print_control_buffer_all_args_impl(output outstm, Args... args)
{
	return ::fast_io::details::decay::print_control_buffer_impl<line, 0, sizeof...(Args)>(outstm, args...);
}

} // namespace details::decay

namespace operations
{

namespace decay
{

template <bool line, typename outputstmtype, typename... Args>
inline constexpr decltype(auto) print_freestanding_decay(outputstmtype optstm, Args... args)
{
	if constexpr (::fast_io::operations::decay::defines::has_status_print_define<outputstmtype>)
	{
		return status_print_define<line>(optstm, args...);
	}
	else if constexpr (sizeof...(Args) == 0)
	{
		if constexpr (line)
		{
			using char_type = typename outputstmtype::output_char_type;
			return ::fast_io::operations::decay::char_put_decay(optstm, char_literal_v<u8'\n', char_type>);
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
		return ::fast_io::operations::decay::print_freestanding_decay<line>(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(optstm), args...);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_obuffer_basic_operations<outputstmtype>)
	{
		return ::fast_io::details::decay::print_control_buffer_all_args_impl<line>(optstm, args...);
	}
	else
	{
		return ::fast_io::details::decay::print_control_all_args_impl<line>(optstm, args...);
	}
}

template <bool line, typename outputstmtype, typename... Args>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr decltype(auto) print_freestanding_decay_cold(outputstmtype optstm, Args... args)
{
#if !__has_cpp_attribute(__gnu__::__cold__) && __has_cpp_attribute(unlikely)
	if (true) [[unlikely]]
#endif
		return ::fast_io::operations::decay::print_freestanding_decay<line>(optstm, args...);
}

} // namespace decay

namespace decay::defines
{

template <typename char_type, typename... Args>
concept print_freestanding_params_okay =
	::std::integral<char_type> &&
	((::fast_io::printable<char_type, Args> || ::fast_io::reserve_printable<char_type, Args> ||
	  ::fast_io::dynamic_reserve_printable<char_type, Args> || ::fast_io::scatter_printable<char_type, Args> ||
	  ::fast_io::reserve_scatters_printable<char_type, Args> || ::fast_io::context_printable<char_type, Args> ||
	  ::fast_io::transcode_imaginary_printable<char_type, Args>) &&
	 ...);

template <typename output, typename... Args>
concept print_freestanding_okay =
	::fast_io::operations::decay::defines::print_freestanding_params_okay<typename output::output_char_type, Args...>;

} // namespace decay::defines

namespace defines
{

template <typename char_type, typename... Args>
concept print_freestanding_params_okay = ::fast_io::operations::decay::defines::print_freestanding_params_okay<char_type,
																											   decltype(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(::std::declval<Args>())))...>;

template <typename output, typename... Args>
concept print_freestanding_okay = ::fast_io::operations::decay::defines::print_freestanding_okay<
	decltype(::fast_io::operations::output_stream_ref(::std::declval<output>())),
	decltype(::fast_io::io_print_forward<typename decltype(::fast_io::operations::output_stream_ref(
				 ::std::declval<output>()))::output_char_type>(::fast_io::io_print_alias(::std::declval<Args>())))...>;

} // namespace defines

template <bool line, typename output, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void print_freestanding(output &&outstm, Args &&...args)
{
	::fast_io::operations::decay::print_freestanding_decay<line>(
		::fast_io::operations::output_stream_ref(outstm),
		io_print_forward<typename decltype(::fast_io::operations::output_stream_ref(outstm))::output_char_type>(
			io_print_alias(args))...);
}

} // namespace operations

} // namespace fast_io
