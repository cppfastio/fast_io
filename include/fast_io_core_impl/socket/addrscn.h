#pragma once

namespace fast_io
{

enum class scan_ipv4_context_phase : ::std::uint_least8_t
{
	addr0,
	dot0,
	addr1,
	dot1,
	addr2,
	dot2,
	addr3
};

enum class scan_ipv6_context_phase : ::std::uint_least8_t
{
	nothing,
};

enum class scan_integral_context_phase : ::std::uint_least8_t;
template <typename phase_t, ::std::integral char_type>
struct ip_scan_state_t
{
	// TODO: to find out why +1 is needed here
	static inline constexpr auto max_size{ ::fast_io::details::print_integer_reserved_size_cache<10, false, false, ::std::uint_least16_t> +1 };
	::fast_io::freestanding::array<char_type, max_size> buffer;
	scan_integral_context_phase integer_phase{};
	::std::uint_least8_t size{};
	phase_t ip_phase{};
	static inline constexpr ::std::uint_least8_t port_mark = -1;
	static inline constexpr ::std::uint_least8_t port = -2;
};
template <::std::integral char_type>
using ipv4_scan_state_t = ip_scan_state_t<scan_ipv4_context_phase, char_type>;
template <::std::integral char_type>
using ipv6_scan_state_t = ip_scan_state_t<scan_ipv6_context_phase, char_type>;
template <::std::integral char_type>
using ip_port_scan_state_t = ip_scan_state_t<::std::uint_least8_t, char_type>;

namespace details {

template <::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_cnt_define_inaddr_impl(char_type const* begin, char_type const* end, posix_in_addr& t) noexcept
{
	if (end - begin < 7) [[unlikely]]
		return { begin, parse_code::invalid };
	auto [itr0, ec0] = scan_int_contiguous_define_impl<10, true, false, false>(begin, begin + 3, t.address[0]);
	if (ec0 != parse_code::ok) [[unlikely]]
		return { itr0, ec0 };
	if constexpr (sizeof(decltype(t.address[0])) != 1)
	{
		if (t.address[0] >= 256u) [[unlikely]]
			return { itr0, parse_code::overflow };
	}
	begin = itr0;
	if (begin == end || *begin != char_literal_v<u8'.', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	++begin;
	auto [itr1, ec1] = scan_int_contiguous_define_impl<10, true, false, false>(begin, begin + 3, t.address[1]);
	if (ec1 != parse_code::ok) [[unlikely]]
		return { itr1, ec1 };
	if constexpr (sizeof(decltype(t.address[1])) != 1)
	{
		if (t.address[1] >= 256u) [[unlikely]]
			return { itr1, parse_code::overflow };
	}
	begin = itr1;
	if (begin == end || *begin != char_literal_v<u8'.', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	++begin;
	if (end - begin < 3) [[unlikely]]
		return { begin, parse_code::invalid };
	auto [itr2, ec2] = scan_int_contiguous_define_impl<10, true, false, false>(begin, begin + 3, t.address[2]);
	if (ec2 != parse_code::ok) [[unlikely]]
		return { itr2, ec2 };
	if constexpr (sizeof(decltype(t.address[2])) != 1)
	{
		if (t.address[2] >= 256u) [[unlikely]]
			return { itr2, parse_code::overflow };
	}
	begin = itr2;
	if (begin == end || *begin != char_literal_v<u8'.', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	++begin;
	auto [itr3, ec3] = scan_int_contiguous_define_impl<10, true, false, false>(begin, end, t.address[3]);
	if (ec3 != parse_code::ok) [[unlikely]]
		return { itr3, ec3 };
	if constexpr (sizeof(decltype(t.address[3])) != 1)
	{
		if (t.address[3] >= 256u) [[unlikely]]
			return { itr3, parse_code::overflow };
	}
	begin = itr3;
	return { begin, parse_code::ok };
}

template <::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_ctx_define_inaddr_impl(ipv4_scan_state_t<char_type>& state, char_type const* begin, char_type const* end, posix_in_addr& t) noexcept
{
	switch (state.ip_phase)
	{
	case scan_ipv4_context_phase::addr0:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		auto [itr, ec] = scan_context_define_parse_impl<10, false, false, false>(state, begin, end, t.address[0]);
		if (ec != parse_code::ok) [[unlikely]]
			return { itr, ec };
		if constexpr (sizeof(decltype(t.address[0])) != 1)
		{
			if (t.address[0] >= 256) [[unlikely]]
				return { itr, parse_code::overflow };
		}
		state.size = 0;
		state.integer_phase = scan_integral_context_phase::zero;
		begin = itr;
		state.ip_phase = scan_ipv4_context_phase::dot0;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::dot0:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		if (*begin != char_literal_v<u8'.', char_type>) [[unlikely]]
			return { begin, parse_code::invalid };
		++begin;
		state.ip_phase = scan_ipv4_context_phase::addr1;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::addr1:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		auto [itr, ec] = scan_context_define_parse_impl<10, true, false, false>(state, begin, end, t.address[1]);
		if (ec != parse_code::ok) [[unlikely]]
			return { itr, ec };
		if constexpr (sizeof(decltype(t.address[1])) != 1)
		{
			if (t.address[1] >= 256) [[unlikely]]
				return { itr, parse_code::overflow };
		}
		state.size = 0;
		state.integer_phase = scan_integral_context_phase::zero;
		begin = itr;
		state.ip_phase = scan_ipv4_context_phase::dot1;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::dot1:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		if (*begin != char_literal_v<u8'.', char_type>) [[unlikely]]
			return { begin, parse_code::invalid };
		++begin;
		state.ip_phase = scan_ipv4_context_phase::addr2;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::addr2:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		auto [itr, ec] = scan_context_define_parse_impl<10, true, false, false>(state, begin, end, t.address[2]);
		if (ec != parse_code::ok) [[unlikely]]
			return { itr, ec };
		if constexpr (sizeof(decltype(t.address[2])) != 1)
		{
			if (t.address[2] >= 256) [[unlikely]]
				return { itr, parse_code::overflow };
		}
		state.size = 0;
		state.integer_phase = scan_integral_context_phase::zero;
		begin = itr;
		state.ip_phase = scan_ipv4_context_phase::dot2;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::dot2:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		if (*begin != char_literal_v<u8'.', char_type>) [[unlikely]]
			return { begin, parse_code::invalid };
		++begin;
		state.ip_phase = scan_ipv4_context_phase::addr3;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::addr3:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		auto [itr, ec] = scan_context_define_parse_impl<10, true, false, false>(state, begin, end, t.address[3]);
		if (ec != parse_code::ok) [[unlikely]]
			return { itr, ec };
		if constexpr (sizeof(decltype(t.address[3])) != 1)
		{
			if (t.address[3] >= 256) [[unlikely]]
				return { itr, parse_code::overflow };
		}
		begin = itr;
		return { begin, parse_code::ok };
	}
	}
#ifdef __has_builtin
#if __has_builtin(__builtin_unreachable)
	__builtin_unreachable();
#endif
#endif
}

template <::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_cnt_define_port_impl(char_type const* begin, char_type const* end, ::std::uint_least16_t& t) noexcept
{
	if (begin == end) [[unlikely]]
		return { begin, parse_code::invalid };
	if (*begin != char_literal_v<u8':', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	++begin;
	return scan_int_contiguous_define_impl<10, true, false, false>(begin, end, t);
}

template <::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_ctx_define_port_impl(ip_port_scan_state_t<char_type>& state, char_type const* begin, char_type const* end, ::std::uint_least16_t& t) noexcept
{
	switch (state.ip_phase)
	{
	case state.port_mark:
		if (begin == end)
			return { begin, parse_code::partial };
		if (*begin != char_literal_v<u8':', char_type>) [[unlikely]]
			return { begin, parse_code::invalid };
		++begin;
		state.ip_phase = state.port;
		[[fallthrough]];
	case state.port:
		if (begin == end)
			return { begin, parse_code::partial };
		return scan_context_define_parse_impl<10, true, false, false>(state, begin, end, t);
	default:;
#ifdef __has_builtin
#if __has_builtin(__builtin_unreachable)
		__builtin_unreachable();
#endif
#endif
	}
}

template <typename state_t, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr parse_result<char_type const*> scn_ctx_define_port_type_general_impl(state_t& state, char_type const* begin, char_type const* end, ::std::uint_least16_t& t) noexcept
{
	auto new_state{ ::std::bit_cast<ip_port_scan_state_t<char_type>>(state) };
	auto result{ scn_ctx_define_port_impl<char_type>(new_state, begin, end, t) };
	state = ::std::bit_cast<state_t>(new_state);
	return result;
}

template <bool allowv6uppercase, ::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_cnt_define_in6addr_4_digits_impl(char_type const* begin, char_type const* end, ::std::uint_least16_t& t) noexcept
{
	constexpr bool big_endian{ ::std::endian::native == ::std::endian::big };
	using unsigned_char_type = ::std::make_unsigned_t<char_type>;
	::std::uint_least8_t retval[2];
	if (begin == end) [[unlikely]]
		return { begin, parse_code::invalid };
	if (*begin == char_literal_v<u8':', char_type>) [[unlikely]]
		// use eof here to represent shorten case. should be handled from the caller.
		return { begin, parse_code::end_of_file };
	bool zero_started{};
	// loop unrolled, because each time the return changes a bit
	auto result{ static_cast<unsigned_char_type>(*begin) };
	if (char_digit_to_literal<16, char_type>(result)) [[unlikely]]
		return { begin, parse_code::invalid };
	if (result == 0)
		zero_started = true;
	retval[0] = result;
	++begin;
	if (begin == end)
	{
		if constexpr (big_endian)
			t = retval[0];
		else
			t = retval[0] << 8;
		return { begin, parse_code::ok };
	}
	result = *begin;
	if (char_digit_to_literal<16, char_type>(result))
	{
		if constexpr (big_endian)
			t = retval[0];
		else
			t = retval[0] << 8;
		return { begin, parse_code::ok };
	}
	retval[0] *= 16;
	retval[0] += result;
	++begin;
	if (begin == end)
	{
		if (zero_started) [[unlikely]]
			return { begin, parse_code::invalid };
		else
		{
			if constexpr (big_endian)
				t = retval[0];
			else
				t = retval[0] << 8;
			return { begin, parse_code::ok };
		}
	}
	result = *begin;
	if (char_digit_to_literal<16, char_type>(result))
	{
		if (zero_started) [[unlikely]]
			return { begin, parse_code::invalid };
		else
		{
			if constexpr (big_endian)
				t = retval[0];
			else
				t = retval[0] << 8;
			return { begin, parse_code::ok };
		}
	}
	retval[1] = result;
	++begin;
	if (begin == end)
	{
		if (zero_started) [[unlikely]]
			return { begin, parse_code::invalid };
		else
		{
			if constexpr (big_endian)
				t = retval[0] | retval[1] << 8;
			else
				t = retval[0] << 8 | retval[1];
			return { begin, parse_code::ok };
		}
	}
	result = *begin;
	if (char_digit_to_literal<16, char_type>(result))
	{
		if (zero_started) [[unlikely]]
			return { begin, parse_code::invalid };
		else
		{
			if constexpr (big_endian)
				t = retval[0] | retval[1] << 8;
			else
				t = retval[0] << 8 | retval[1];
			return { begin, parse_code::ok };
		}
	}
	retval[1] *= 16;
	retval[1] += result;
	++begin;
	if (begin != end && char_is_digit<16, char_type>(*begin)) [[unlikely]]
		return { begin, parse_code::overflow };
	if constexpr (big_endian)
		t = retval[0] | retval[1] << 8;
	else
		t = retval[0] << 8 | retval[1];
	return { begin, parse_code::ok };
}

template <bool allowv6uppercase, ::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_cnt_define_in6addr_shorten_impl(char_type const* begin, char_type const* end, posix_in6_addr& t) noexcept
{
	struct in6addr_scan_basic_state_t
	{
		::std::uint_least8_t seg_cnt{};
		::std::uint_least8_t shorten_begin_index{};
		::std::uint_least8_t cur_zero_len{};
		::std::uint_least8_t max_zero_len{};
	};
	in6addr_scan_basic_state_t state;
	while (state.seg_cnt < 8)
	{
		::std::uint_least16_t addrvalue;
		auto [itr, ec] = scn_cnt_define_in6addr_4_digits_impl<allowv6uppercase>(begin, end, addrvalue);
		begin = itr;
		if (ec == parse_code::ok)
		{
			t.address[state.seg_cnt] = addrvalue;
			if (addrvalue != 0)
			{
				if (state.cur_zero_len > state.max_zero_len) [[unlikely]]
				{
					state.max_zero_len = state.cur_zero_len;
					state.cur_zero_len = 0;
				}
				++state.seg_cnt;
				continue;
			}
			else
			{
				++state.cur_zero_len;
				++state.seg_cnt;
				continue;
			}
		}
		else if (ec == parse_code::end_of_file)
		{
			if (state.shorten_begin_index != 0) [[unlikely]]
				return { begin, parse_code::invalid };
			// TODO
		}
		else [[unlikely]]
			return { itr, ec };
	}
}

template <bool allowv6uppercase, ::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_cnt_define_in6addr_nonshorten_impl(char_type const* begin, char_type const* end, posix_in6_addr& t) noexcept
{
	for (::std::size_t i{}; i < 7; ++i)
	{
		auto [itr, ec] = scn_cnt_define_in6addr_4_digits_impl<allowv6uppercase>(begin, end, t.address[i]);
		if (ec != parse_code::ok) [[unlikely]]
		{
			if (ec == parse_code::end_of_file)
				return { itr, parse_code::invalid };
			else
				return { itr, ec };
		}
			if (*itr != char_literal_v<u8':', char_type>) [[unlikely]]
				return { itr, parse_code::invalid };
		begin = itr + 1;
	}
	auto [itr, ec] = scn_cnt_define_in6addr_4_digits_impl<allowv6uppercase>(begin, end, t.address[7]);
	if (ec != parse_code::ok) [[unlikely]]
	{
		if (ec == parse_code::end_of_file)
			return { itr, parse_code::invalid };
		else
			return { itr, ec };
	}
	return { itr, parse_code::ok };
}

template <bool allowv6uppercase, ::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_cnt_define_in6addr_full_impl(char_type const* begin, char_type const* end, posix_in6_addr& t) noexcept
{
	if constexpr (true)
		return scn_cnt_define_in6addr_nonshorten_impl<allowv6uppercase>(begin, end, t);
}

template <bool allowv6shorten, bool allowv6uppercase, bool allowv6bracket, bool requirev6full, ::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_cnt_define_in6addr_impl(char_type const* begin, char_type const* end, posix_in6_addr& t) noexcept
{
	begin = ::fast_io::details::find_space_common_impl<false, true>(begin, end);
	if constexpr (allowv6bracket)
	{
		if (begin == end) [[unlikely]]
			return { begin, parse_code::invalid };
		if (*begin != char_literal_v<u8'[', char_type>) [[unlikely]]
			return scn_cnt_define_in6addr_impl<allowv6shorten, allowv6uppercase, false, requirev6full>(begin, end, t);
		++begin;
		auto result = scn_cnt_define_in6addr_impl<allowv6shorten, allowv6uppercase, false, requirev6full>(begin, end, t);
		if (result.code != parse_code::ok) [[unlikely]]
			return result;
		begin = result.iter;
		if (begin == end) [[unlikely]]
			return { begin, parse_code::invalid };
		if (*begin != char_literal_v<u8']', char_type>) [[unlikely]]
			return { begin, parse_code::invalid };
		return { begin + 1, parse_code::ok };
	}
	if constexpr (allowv6shorten)
		return scn_cnt_define_in6addr_shorten_impl<allowv6uppercase>(begin, end, t);
	else if constexpr (requirev6full)
		return scn_cnt_define_in6addr_full_impl<allowv6uppercase>(begin, end, t);
	else
		return scn_cnt_define_in6addr_nonshorten_impl<allowv6uppercase>(begin, end, t);
}

}

namespace manipulators
{

template<ip_scan_flags flags, ::fast_io::details::iptypesimpl iptype>
inline constexpr ip_scan_manip_t<flags, ::std::remove_cvref_t<iptype>*> ip_scan_generic(iptype& ipaddr) noexcept
{
	return { __builtin_addressof(ipaddr) };
}

}

#if 0
inline constexpr mnp::ip_scan_manip_t<mnp::ip_scan_flags{0,1,1,0,0}, posix_in6_addr*> scan_alias_define(io_alias_t, posix_in6_addr& t) noexcept
{
	return { __builtin_addressof(t) };
}
#else
template<::fast_io::details::iptypesimpl iptype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::manipulators::ip_scan_manip_t<
	::fast_io::details::inaddrimpl<iptype> ?
	::fast_io::manipulators::ip_scan_default_inaddr_flags
	:
	::fast_io::manipulators::ip_scan_default_flags, ::std::remove_cvref_t<iptype>*> scan_alias_define(io_alias_t, iptype& t) noexcept
{
	return { __builtin_addressof(t) };
}
#endif

template<::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags, ::fast_io::details::iptypesimpl iptype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr parse_result<char_type const*> scan_contiguous_define(io_reserve_type_t<char_type,
	manipulators::ip_scan_manip_t<flags, iptype*>>, char_type const* begin, char_type const* end, manipulators::ip_scan_manip_t<flags, iptype*> val) noexcept
{
	if constexpr (::std::same_as<iptype, posix_in_addr>)
	{
		auto result{ details::scn_cnt_define_inaddr_impl(begin, end, *val.reference) };
		if constexpr (flags.requireport == true)
		{
			if (result.code != parse_code::ok) [[unlikely]]
				return result;
			::std::uint_least16_t port;
			return details::scn_cnt_define_port_impl(result.iter, end, port);
		}
		return result;
	}
	else if constexpr (::std::same_as<iptype, ::fast_io::ipv4>)
	{
		auto result{ ::fast_io::details::scn_cnt_define_inaddr_impl(begin, end, val.reference->address) };
		if (result.code != parse_code::ok) [[unlikely]]
			return result;
		begin = result.iter;
		if constexpr (flags.requireport == false)
		{
			val.reference->port = 0;
			return result;
		}
		else
			return ::fast_io::details::scn_cnt_define_port_impl(begin, end, val.reference->port);
	}
	else if constexpr (::std::same_as<iptype, ::fast_io::posix_in6_addr>)
	{
		auto result{ details::scn_cnt_define_in6addr_impl<flags.allowv6shorten, flags.allowv6uppercase, flags.allowv6bracket, flags.requirev6full>(begin, end, *val.reference) };
		if constexpr (flags.requireport == true)
		{
			if (result.code != parse_code::ok) [[unlikely]]
				return result;
			::std::uint_least16_t port;
			return details::scn_cnt_define_port_impl(result.iter, end, port);
		}
		return result;
	}
	else if constexpr (::std::same_as<iptype, ::fast_io::ipv6>)
	{
		//return ::fast_io::details::prtrsv_ipv6_define_impl<flags.v6shorten, flags.v6uppercase,
		//	flags.showport ? true : flags.v6bracket, flags.v6full,
		//	flags.showport>(iter, val.reference);
		return {};
	}
	else if constexpr (::std::same_as<iptype, ::fast_io::ip_address>)
	{
		//return ::fast_io::details::prtrsv_ip_address_define_impl<flags.v6shorten, flags.v6uppercase,
		//	flags.showport ? true : flags.v6bracket, flags.v6full,
		//	flags.showport>(iter, val.reference);
		return {};
	}
	else
	{
		//return ::fast_io::details::prtrsv_ip_define_impl<flags.v6shorten, flags.v6uppercase,
		//	flags.showport ? true : flags.v6bracket, flags.v6full,
		//	flags.showport>(iter, val.reference);
		return {};
	}
}

template<::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr io_type_t<ipv4_scan_state_t<char_type>> scan_context_type(io_reserve_type_t<char_type, ::fast_io::manipulators::ip_scan_manip_t<flags, posix_in_addr*>>) noexcept
{
	return {};
}

template<::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr parse_result<char_type const*> scan_context_define(::fast_io::io_reserve_type_t<char_type,
	::fast_io::manipulators::ip_scan_manip_t<flags, posix_in_addr*>>, ipv4_scan_state_t<char_type>& state, char_type const* begin, char_type const* end, ::fast_io::manipulators::ip_scan_manip_t<flags, posix_in_addr*> t) noexcept
{
	return details::scn_ctx_define_inaddr_impl(state, begin, end, *t.reference);
}

template<::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr parse_code scan_context_eof_define(::fast_io::io_reserve_type_t<char_type,
	::fast_io::manipulators::ip_scan_manip_t<flags, posix_in_addr*>>, ipv4_scan_state_t<char_type>& state, ::fast_io::manipulators::ip_scan_manip_t<flags, posix_in_addr*> t) noexcept
{
	if (state.ip_phase != scan_ipv4_context_phase::addr3)
		return parse_code::end_of_file;
	else
		return details::scan_int_contiguous_none_space_part_define_impl<10>(state.buffer.data(), state.buffer.data() + state.size, t.reference->address[3]).code;
}

template<::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr io_type_t<ipv4_scan_state_t<char_type>> scan_context_type(io_reserve_type_t<char_type, ::fast_io::manipulators::ip_scan_manip_t<flags, ipv4*>>) noexcept
{
	return {};
}

template<::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr parse_result<char_type const*> scan_context_define(::fast_io::io_reserve_type_t<char_type,
	::fast_io::manipulators::ip_scan_manip_t<flags, ipv4*>>, ipv4_scan_state_t<char_type>& state, char_type const* begin, char_type const* end, ::fast_io::manipulators::ip_scan_manip_t<flags, ipv4*> t) noexcept
{
	switch (state.ip_phase)
	{
	case scan_ipv4_context_phase::addr0:
	case scan_ipv4_context_phase::dot0:
	case scan_ipv4_context_phase::addr1:
	case scan_ipv4_context_phase::dot1:
	case scan_ipv4_context_phase::addr2:
	case scan_ipv4_context_phase::dot2:
	case scan_ipv4_context_phase::addr3:
	{
		auto result{ details::scn_ctx_define_inaddr_impl(state, begin, end, t.reference->address) };
		if (result.code != parse_code::ok)
			return result;
		begin = result.iter;
		state.integer_phase = scan_integral_context_phase::zero;
		state.size = 0;
		state.ip_phase = ::std::bit_cast<scan_ipv4_context_phase>(state.port_mark);
		[[fallthrough]];
	}
	// clang bug for std::bit_cast is not a constexpr expression
#if !defined(__clang__)
	case ::std::bit_cast<scan_ipv4_context_phase>(state.port_mark) :
		case ::std::bit_cast<scan_ipv4_context_phase>(state.port) :
		return { details::scn_ctx_define_port_type_general_impl(state, begin, end, t.reference->port) };
		default:;
#else
	default:
		if (state.ip_phase == ::std::bit_cast<scan_ipv4_context_phase>(state.port_mark) ||
			state.ip_phase == ::std::bit_cast<scan_ipv4_context_phase>(state.port)) [[likely]]
			return { details::scn_ctx_define_port_type_general_impl(state, begin, end, t.reference->port) };
#endif
#ifdef __has_builtin
#if __has_builtin(__builtin_unreachable)
		__builtin_unreachable();
#endif
#endif
	}
}

template<::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr parse_code scan_context_eof_define(::fast_io::io_reserve_type_t<char_type,
	::fast_io::manipulators::ip_scan_manip_t<flags, ipv4*>>, ipv4_scan_state_t<char_type>& state, ::fast_io::manipulators::ip_scan_manip_t<flags, ipv4*> t) noexcept
{
	if (state.ip_phase != ::std::bit_cast<scan_ipv4_context_phase>(state.port))
		return parse_code::end_of_file;
	else
		return details::scan_int_contiguous_none_space_part_define_impl<10>(state.buffer.data(), state.buffer.data() + state.size, t.reference->port).code;
}

}
