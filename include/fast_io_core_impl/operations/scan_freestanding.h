#pragma once

namespace fast_io
{

namespace details
{

template <typename input, typename P>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr bool scan_context_status_impl(input in, P arg) FAST_IO_HERBCEPTIONS_THROWS
{
	using char_type = typename input::input_char_type;
	for (typename ::std::remove_cvref_t<decltype(scan_context_type(io_reserve_type<char_type, P>))>::type state;;)
	{
		auto curr{ibuffer_curr(in)};
		auto end{ibuffer_end(in)};
		auto [it, ec] = scan_context_define(io_reserve_type<char_type, P>, state, curr, end, arg);
		if constexpr (::std::same_as<decltype(curr), decltype(it)>)
		{
			ibuffer_set_curr(in, it);
		}
		else
		{
			ibuffer_set_curr(in, it - curr + curr);
		}
		if (ec == ::fast_io::freestanding::parse_errc::ok)
		{
			return true;
		}
		else if (ec != ::fast_io::freestanding::parse_errc::partial)
		{
			::fast_io::herbceptions::throws_parse_errc(ec);
		}
		if (!ibuffer_underflow(in)) [[unlikely]]
		{
			ec = scan_context_eof_define(io_reserve_type<char_type, P>, state, arg);
			if (ec == ::fast_io::freestanding::parse_errc::ok)
			{
				return true;
			}
			else if (ec == ::fast_io::freestanding::parse_errc::end_of_file)
			{
				break;
			}
			::fast_io::herbceptions::throws_parse_errc(ec);
		}
	}
	return false;
}

template <bool>
inline constexpr bool type_not_scannable = false;

} // namespace details

namespace operations::decay
{

template <typename input, typename... Args>
[[nodiscard]] inline constexpr decltype(auto) scan_some_freestanding_decay(input instm, Args... args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (::fast_io::operations::decay::defines::has_status_scan_some_define<input>)
	{
		return status_scan_some_define(instm, args...);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<input>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::input_stream_mutex_ref_decay(instm)};
		return ::fast_io::operations::decay::scan_some_freestanding_decay(
			::fast_io::operations::decay::input_stream_unlocked_ref_decay(instm), args...);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_ibuffer_basic_operations<input>)
	{
		using char_type = typename input::input_char_type;
		template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
		{
			using argtype = ::std::remove_cvref_t<Args...[i]>;
			if constexpr (::fast_io::precise_reserve_scannable<char_type, argtype>)
			{
				constexpr ::std::size_t n{scan_precise_reserve_size(::fast_io::io_reserve_type<char_type, argtype>)};
				char_type buffer[n];
				if constexpr (::fast_io::details::asan_state::current == ::fast_io::details::asan_state::activate)
				{
					char_type const *p{buffer};
					::fast_io::operations::decay::read_all_decay(instm, buffer, n);
					if constexpr (::fast_io::precise_reserve_scannable_no_error<char_type, argtype>)
					{
						scan_precise_reserve_define(::fast_io::io_reserve_type<char_type, argtype>, p, args...[i]);
					}
					else
					{
						auto ret{scan_precise_reserve_define(::fast_io::io_reserve_type<char_type, argtype>, p, args...[i])};
						if (ret != ::fast_io::freestanding::parse_errc::ok)
						{
							if (ret == ::fast_io::freestanding::parse_errc::end_of_file)
							{
								return ::fast_io::scan_some_result_t{sizeof...(Args) - i};
							}
							::fast_io::herbceptions::throws_parse_errc(ret);
						}
					}
				}
				else
				{
					auto curr_ptr{ibuffer_curr(instm)};
					char_type const *curr{curr_ptr};
					char_type const *end{ibuffer_end(instm)};
					char_type const *p{curr};
					::std::size_t const diff{static_cast<::std::size_t>(end - curr)};
					bool const inbuffer{diff < n};
					if (inbuffer) [[unlikely]]
					{
						::fast_io::operations::decay::read_all_decay(instm, buffer, n);
						p = buffer;
					}
					if constexpr (::fast_io::precise_reserve_scannable_no_error<char_type, argtype>)
					{
						scan_precise_reserve_define(::fast_io::io_reserve_type<char_type, argtype>, p, args...[i]);
					}
					else
					{
						auto ret{scan_precise_reserve_define(::fast_io::io_reserve_type<char_type, argtype>, p, args...[i])};
						if (ret != ::fast_io::freestanding::parse_errc::ok)
						{
							if (ret == ::fast_io::freestanding::parse_errc::end_of_file)
							{
								return ::fast_io::scan_some_result_t{sizeof...(Args) - i};
							}
							::fast_io::herbceptions::throws_parse_errc(ret);
						}
					}
					if (!inbuffer) [[likely]]
					{
						ibuffer_set_curr(instm, curr_ptr + n);
					}
				}
			}
			else if constexpr (::fast_io::contiguous_scannable<char_type, argtype> &&
							   ::fast_io::context_scannable<char_type, argtype>)
			{
				auto curr{ibuffer_curr(instm)};
				auto end{ibuffer_end(instm)};
				auto [it, ec] = scan_contiguous_define(::fast_io::io_reserve_type<char_type, argtype>, curr, end, args...[i]);
				if (it == end)
				{
					if (!::fast_io::details::scan_context_status_impl(instm, args...[i]))
					{
						return ::fast_io::scan_some_result_t{sizeof...(Args) - i};
					}
				}
				else
				{
					if constexpr (::std::same_as<decltype(curr), decltype(it)>)
					{
						ibuffer_set_curr(instm, it);
					}
					else
					{
						ibuffer_set_curr(instm, it - curr + curr);
					}
					if (ec != ::fast_io::freestanding::parse_errc::ok)
					{
						::fast_io::herbceptions::throws_parse_errc(ec);
					}
				}
			}
			else if constexpr (::fast_io::context_scannable<char_type, argtype>)
			{
				bool scanned{false};
				for (typename ::std::remove_cvref_t<decltype(scan_context_type(io_reserve_type<char_type, argtype>))>::type state;;)
				{
					auto curr{ibuffer_curr(instm)};
					auto end{ibuffer_end(instm)};
					auto [it, ec] = scan_context_define(io_reserve_type<char_type, argtype>, state, curr, end, args...[i]);
					if constexpr (::std::same_as<decltype(curr), decltype(it)>)
					{
						ibuffer_set_curr(instm, it);
					}
					else
					{
						ibuffer_set_curr(instm, it - curr + curr);
					}
					if (ec == ::fast_io::freestanding::parse_errc::ok)
					{
						scanned = true;
						break;
					}
					else if (ec != ::fast_io::freestanding::parse_errc::partial)
					{
						::fast_io::herbceptions::throws_parse_errc(ec);
					}
					if (!ibuffer_underflow(instm)) [[unlikely]]
					{
						ec = scan_context_eof_define(io_reserve_type<char_type, argtype>, state, args...[i]);
						if (ec == ::fast_io::freestanding::parse_errc::ok)
						{
							scanned = true;
							break;
						}
						else if (ec == ::fast_io::freestanding::parse_errc::end_of_file)
						{
							break;
						}
						::fast_io::herbceptions::throws_parse_errc(ec);
					}
				}
				if (!scanned)
				{
					return ::fast_io::scan_some_result_t{sizeof...(Args) - i};
				}
			}
			else
			{
				constexpr bool not_scannable{::fast_io::context_scannable<char_type, argtype>};
				static_assert(not_scannable, "type not scannable. need context_scannable");
			}
		}
		return ::fast_io::scan_some_result_t{};
	}
	else if constexpr (::fast_io::operations::defines::available_add_ibuf<input>)
	{
		static_assert(::fast_io::operations::decay::defines::has_status_scan_some_define<input>,
					  "If you want to scan this type of file, please add ::fast_io::basic_ibuf.");
		return ::fast_io::scan_some_result_t{};
	}
	else
	{
		static_assert(::fast_io::operations::decay::defines::has_status_scan_some_define<input>, "type not scannable.");
		return ::fast_io::scan_some_result_t{};
	}
}

} // namespace operations::decay

} // namespace fast_io
