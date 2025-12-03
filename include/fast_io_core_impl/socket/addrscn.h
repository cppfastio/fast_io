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
	/// @todo forgot?
	// TODO: to find out why +1 is needed here
	static inline constexpr auto max_size{
		::fast_io::details::print_integer_reserved_size_cache<10, false, false, ::std::uint_least16_t> + 1};
	::fast_io::freestanding::array<char_type, max_size> buffer;
	scan_integral_context_phase integer_phase{};
	::std::uint_least8_t size{};
	phase_t ip_phase{};
	static inline constexpr ::std::uint_least8_t port_mark = static_cast<::std::uint_least8_t>(-1);
	static inline constexpr ::std::uint_least8_t port = static_cast<::std::uint_least8_t>(-2);
};
template <::std::integral char_type>
using ipv4_scan_state_t = ip_scan_state_t<scan_ipv4_context_phase, char_type>;
template <::std::integral char_type>
using ipv6_scan_state_t = ip_scan_state_t<scan_ipv6_context_phase, char_type>;
template <::std::integral char_type>
using ip_port_scan_state_t = ip_scan_state_t<::std::uint_least8_t, char_type>;

namespace details
{

template <::std::integral char_type>
inline constexpr parse_result<char_type const *>
scn_cnt_define_inaddr_impl(char_type const *begin, char_type const *end, posix_in_addr &t) noexcept
{
	if (end - begin < 7) [[unlikely]]
	{
		return {begin, parse_code::invalid};
	}
	auto [itr0, ec0] = scan_int_contiguous_define_impl<10, true, false, false>(begin, begin + 3, t.address[0]);
	if (ec0 != parse_code::ok) [[unlikely]]
	{
		return {itr0, ec0};
	}
	if constexpr (sizeof(decltype(t.address[0])) != 1)
	{
		if (t.address[0] >= 256u) [[unlikely]]
		{
			return {itr0, parse_code::overflow};
		}
	}
	begin = itr0;
	if (begin == end || *begin != char_literal_v<u8'.', char_type>) [[unlikely]]
	{
		return {begin, parse_code::invalid};
	}
	++begin;
	auto [itr1, ec1] = scan_int_contiguous_define_impl<10, true, false, false>(begin, begin + 3, t.address[1]);
	if (ec1 != parse_code::ok) [[unlikely]]
	{
		return {itr1, ec1};
	}
	if constexpr (sizeof(decltype(t.address[1])) != 1)
	{
		if (t.address[1] >= 256u) [[unlikely]]
		{
			return {itr1, parse_code::overflow};
		}
	}
	begin = itr1;
	if (begin == end || *begin != char_literal_v<u8'.', char_type>) [[unlikely]]
	{
		return {begin, parse_code::invalid};
	}
	++begin;
	if (end - begin < 3) [[unlikely]]
	{
		return {begin, parse_code::invalid};
	}
	auto [itr2, ec2] = scan_int_contiguous_define_impl<10, true, false, false>(begin, begin + 3, t.address[2]);
	if (ec2 != parse_code::ok) [[unlikely]]
	{
		return {itr2, ec2};
	}
	if constexpr (sizeof(decltype(t.address[2])) != 1)
	{
		if (t.address[2] >= 256u) [[unlikely]]
		{
			return {itr2, parse_code::overflow};
		}
	}
	begin = itr2;
	if (begin == end || *begin != char_literal_v<u8'.', char_type>) [[unlikely]]
	{
		return {begin, parse_code::invalid};
	}
	++begin;
	auto [itr3, ec3] = scan_int_contiguous_define_impl<10, true, false, false>(begin, end, t.address[3]);
	if (ec3 != parse_code::ok) [[unlikely]]
	{
		return {itr3, ec3};
	}
	if constexpr (sizeof(decltype(t.address[3])) != 1)
	{
		if (t.address[3] >= 256u) [[unlikely]]
		{
			return {itr3, parse_code::overflow};
		}
	}
	begin = itr3;
	return {begin, parse_code::ok};
}

template <::std::integral char_type>
inline constexpr parse_result<char_type const *>
scn_ctx_define_inaddr_impl(ipv4_scan_state_t<char_type> &state, char_type const *begin, char_type const *end,
						   posix_in_addr &t) noexcept
{
	switch (state.ip_phase)
	{
	case scan_ipv4_context_phase::addr0:
	{
		if (begin == end)
		{
			return {begin, parse_code::partial};
		}
		auto [itr, ec] = scan_context_define_parse_impl<10, false, false, false>(state, begin, end, t.address[0]);
		if (ec != parse_code::ok) [[unlikely]]
		{
			return {itr, ec};
		}
		if constexpr (sizeof(decltype(t.address[0])) != 1)
		{
			if (t.address[0] >= 256) [[unlikely]]
			{
				return {itr, parse_code::overflow};
			}
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
		{
			return {begin, parse_code::partial};
		}
		if (*begin != char_literal_v<u8'.', char_type>) [[unlikely]]
		{
			return {begin, parse_code::invalid};
		}
		++begin;
		state.ip_phase = scan_ipv4_context_phase::addr1;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::addr1:
	{
		if (begin == end)
		{
			return {begin, parse_code::partial};
		}
		auto [itr, ec] = scan_context_define_parse_impl<10, true, false, false>(state, begin, end, t.address[1]);
		if (ec != parse_code::ok) [[unlikely]]
		{
			return {itr, ec};
		}
		if constexpr (sizeof(decltype(t.address[1])) != 1)
		{
			if (t.address[1] >= 256) [[unlikely]]
			{
				return {itr, parse_code::overflow};
			}
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
		{
			return {begin, parse_code::partial};
		}
		if (*begin != char_literal_v<u8'.', char_type>) [[unlikely]]
		{
			return {begin, parse_code::invalid};
		}
		++begin;
		state.ip_phase = scan_ipv4_context_phase::addr2;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::addr2:
	{
		if (begin == end)
		{
			return {begin, parse_code::partial};
		}
		auto [itr, ec] = scan_context_define_parse_impl<10, true, false, false>(state, begin, end, t.address[2]);
		if (ec != parse_code::ok) [[unlikely]]
		{
			return {itr, ec};
		}
		if constexpr (sizeof(decltype(t.address[2])) != 1)
		{
			if (t.address[2] >= 256) [[unlikely]]
			{
				return {itr, parse_code::overflow};
			}
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
		{
			return {begin, parse_code::partial};
		}
		if (*begin != char_literal_v<u8'.', char_type>) [[unlikely]]
		{
			return {begin, parse_code::invalid};
		}
		++begin;
		state.ip_phase = scan_ipv4_context_phase::addr3;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::addr3:
	{
		if (begin == end)
		{
			return {begin, parse_code::partial};
		}
		auto [itr, ec] = scan_context_define_parse_impl<10, true, false, false>(state, begin, end, t.address[3]);
		if (ec != parse_code::ok) [[unlikely]]
		{
			return {itr, ec};
		}
		if constexpr (sizeof(decltype(t.address[3])) != 1)
		{
			if (t.address[3] >= 256) [[unlikely]]
			{
				return {itr, parse_code::overflow};
			}
		}
		begin = itr;
		return {begin, parse_code::ok};
	}
	}
	::fast_io::unreachable();
}

template <::std::integral char_type>
inline constexpr parse_result<char_type const *> scn_cnt_define_port_impl(char_type const *begin, char_type const *end,
																		  ::std::uint_least16_t &t) noexcept
{
	if (begin == end) [[unlikely]]
	{
		return {begin, parse_code::invalid};
	}
	if (*begin != char_literal_v<u8':', char_type>) [[unlikely]]
	{
		return {begin, parse_code::invalid};
	}
	++begin;
	return scan_int_contiguous_define_impl<10, true, false, false>(begin, end, t);
}

template <::std::integral char_type>
inline constexpr parse_result<char_type const *> scn_ctx_define_port_impl(ip_port_scan_state_t<char_type> &state,
																		  char_type const *begin, char_type const *end,
																		  ::std::uint_least16_t &t) noexcept
{
	switch (state.ip_phase)
	{
	case state.port_mark:
		if (begin == end)
		{
			return {begin, parse_code::partial};
		}
		if (*begin != char_literal_v<u8':', char_type>) [[unlikely]]
		{
			return {begin, parse_code::invalid};
		}
		++begin;
		state.ip_phase = state.port;
		[[fallthrough]];
	case state.port:
		if (begin == end)
		{
			return {begin, parse_code::partial};
		}
		return scan_context_define_parse_impl<10, true, false, false>(state, begin, end, t);
	default:;
		::fast_io::unreachable();
	}
}

template <typename state_t, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr parse_result<char_type const *>
scn_ctx_define_port_type_general_impl(state_t &state, char_type const *begin, char_type const *end,
									  ::std::uint_least16_t &t) noexcept
{
	auto new_state{::std::bit_cast<ip_port_scan_state_t<char_type>>(state)};
	auto result{scn_ctx_define_port_impl<char_type>(new_state, begin, end, t)};
	state = ::std::bit_cast<state_t>(new_state);
	return result;
}

template <bool allowv6uppercase, ::std::integral char_type>
inline constexpr parse_result<char_type const *>
scn_cnt_define_in6addr_4_digits_impl(char_type const *begin, char_type const *end, ::std::uint_least16_t &t) noexcept
{
	using unsigned_char_type = ::std::make_unsigned_t<char_type>;
	if (begin == end) [[unlikely]]
	{
		return {begin, parse_code::invalid};
	}
	if (*begin == char_literal_v<u8':', char_type>) [[unlikely]]
	{
		// use eof here to represent shorten case. should be handled from the caller.
		return {begin, parse_code::end_of_file};
	}
	// loop unrolled, because each time the return changes a bit
	::std::uint_least16_t value{};
	::std::size_t digits{};
	auto it{begin};
	for (; it != end; ++it)
	{
		auto const ch_raw{*it};

		if constexpr (!allowv6uppercase)
		{
			if (::fast_io::char_category::is_c_upper(ch_raw))
			{
				return {it, parse_code::invalid};
			}
		}

		auto ch{static_cast<unsigned_char_type>(ch_raw)};

		if (char_digit_to_literal<16, char_type>(ch))
		{
			break;
		}
		if (digits == 4u) [[unlikely]]
		{
			// More than 4 hex digits in a single group is overflow
			return {it, parse_code::overflow};
		}
		value = static_cast<::std::uint_least16_t>((value << 4u) | ch);
		++digits;
	}
	if (digits == 0u) [[unlikely]]
	{
		return {begin, parse_code::invalid};
	}

	t = ::fast_io::big_endian(value);

	return {it, parse_code::ok};
}

template <bool allowv6uppercase, ::std::integral char_type>
inline constexpr parse_result<char_type const *>
scn_cnt_define_in6addr_shorten_impl(char_type const *begin, char_type const *end, posix_in6_addr &t) noexcept
{
	constexpr auto colon{char_literal_v<u8':', char_type>};

	::std::uint_least16_t words[8]{};
	::std::uint_least16_t *cur{words};
	::std::uint_least16_t *const words_end{words + 8u};
	::std::uint_least16_t *colonp{}; // Record the expansion position of “::” (pointer within words)

	auto it{begin};

	if (it == end) [[unlikely]]
	{
		return {it, parse_code::invalid};
	}

	// Handling cases starting with “::”
	if (*it == colon)
	{
		++it;
		if (it == end || *it != colon) [[unlikely]]
		{
			return {it, parse_code::invalid};
		}
		++it;
		colonp = cur; // “::” at the very beginning
	}

	// Main loop: parse each hextet, record position when "::" is encountered
	while (it != end)
	{
		if (cur == words_end) [[unlikely]]
		{
			// Exceeds 8 groups
			return {it, parse_code::overflow};
		}

		// Find this segment [token_begin, token_end)
		auto token_begin{it};
		auto token_end{it};
		while (token_end != end && *token_end != colon)
		{
			++token_end;
		}

		if (token_begin == token_end) [[unlikely]]
		{
			// Empty token, should not occur ("::" has already been handled above)
			return {it, parse_code::invalid};
		}
		bool has_dot{};
		for (auto p{token_begin}; p != token_end; ++p)
		{
			if (*p == char_literal_v<u8'.', char_type>)
			{
				has_dot = true;
				break;
			}
		}

		if (has_dot)
		{
			if (static_cast<::std::size_t>(words_end - cur) < 2u) [[unlikely]]
			{
				return {it, parse_code::overflow};
			}
			posix_in_addr v4{};
			auto [next4, ec4] = scn_cnt_define_inaddr_impl(token_begin, token_end, v4);
			if (ec4 != parse_code::ok || next4 != token_end) [[unlikely]]
			{
				return {next4, ec4 == parse_code::ok ? parse_code::invalid : ec4};
			}
			it = token_end;
			if (it != end) [[unlikely]]
			{
				return {it, parse_code::invalid};
			}
			::std::uint_least16_t hi{
				static_cast<::std::uint_least16_t>(
					(static_cast<::std::uint_least16_t>(v4.address[0]) << 8u) |
					static_cast<::std::uint_least16_t>(v4.address[1]))};
			::std::uint_least16_t lo{
				static_cast<::std::uint_least16_t>(
					(static_cast<::std::uint_least16_t>(v4.address[2]) << 8u) |
					static_cast<::std::uint_least16_t>(v4.address[3]))};
			*cur = ::fast_io::big_endian(hi);
			++cur;
			*cur = ::fast_io::big_endian(lo);
			++cur;
			break;
		}

		// Use the existing 4-digit parsing function to parse this group
		::std::uint_least16_t value{};
		auto [next, ec] =
			scn_cnt_define_in6addr_4_digits_impl<allowv6uppercase>(token_begin, token_end, value);
		if (ec != parse_code::ok || next != token_end) [[unlikely]]
		{
			return {next, ec == parse_code::ok ? parse_code::invalid : ec};
		}

		*cur = value;
		++cur;
		it = token_end;

		if (it == end)
		{
			break;
		}

		// Here *it == ':'
		++it;
		if (it != end && *it == colon)
		{
			// Encountered "::"
			if (colonp != nullptr) [[unlikely]]
			{
				// Can only have one "::"
				return {it, parse_code::invalid};
			}
			colonp = cur;
			++it;
			if (it == end)
			{
				// "::" at the end, followed by all zeros
				break;
			}
		}
	}

	auto n_words{static_cast<::std::size_t>(cur - words)};

	if (colonp == nullptr)
	{
		// When there is no "::", it must be exactly 8 groups
		if (n_words != 8u) [[unlikely]]
		{
			return {it, parse_code::invalid};
		}
	}
	else
	{
		// When there is "::", perform 0 padding
		auto head_count{static_cast<::std::size_t>(colonp - words)};
		auto tail_count{n_words - head_count};

		if (n_words > 8u || (colonp != nullptr && n_words == 8u)) [[unlikely]]
		{
			return {it, parse_code::invalid};
		}

		// Move the tail to the end of the array
		{
			auto src_begin{words + head_count};
			auto src_end{words + head_count + tail_count};
			auto dst_end{words + 8u};
			for (auto s{src_end}, d{dst_end}; s != src_begin;)
			{
				--s;
				--d;
				*d = *s;
			}
		}

		// Fill 0 between head and tail
		auto zero_end{8u - tail_count};
		{
			auto zero_begin{words + head_count};
			auto zero_last{words + zero_end};
			::std::size_t zero_bytes{static_cast<::std::size_t>(zero_last - zero_begin) * sizeof(::std::uint_least16_t)};
			if (zero_bytes)
			{
				::fast_io::details::my_memset(zero_begin, 0, zero_bytes);
			}
		}

		n_words = 8u;
	}

	if (n_words != 8u) [[unlikely]]
	{
		return {it, parse_code::invalid};
	}

	// Copy to target in6_addr
	{
		auto src{words};
		auto dst{t.address};
		for (auto s{src}, d{dst}; s != words + 8u; ++s, ++d)
		{
			*d = *s;
		}
	}

	return {it, parse_code::ok};
}

template <bool allowv6shorten, bool allowv6uppercase, bool allowv6bracket, bool requirev6full,
		  ::std::integral char_type>
inline constexpr parse_result<char_type const *>
scn_cnt_define_in6addr_impl(char_type const *begin, char_type const *end, posix_in6_addr &t) noexcept
{
	begin = ::fast_io::details::find_space_common_impl<false, true>(begin, end);
	if constexpr (allowv6bracket)
	{
		if (begin == end) [[unlikely]]
		{
			return {begin, parse_code::invalid};
		}
		if (*begin != char_literal_v<u8'[', char_type>) [[unlikely]]
		{
			return scn_cnt_define_in6addr_impl<allowv6shorten, allowv6uppercase, false, requirev6full>(begin, end, t);
		}
		auto inner_begin{begin + 1u};
		auto inner_end{inner_begin};
		for (; inner_end != end && *inner_end != char_literal_v<u8']', char_type>; ++inner_end)
		{
		}
		if (inner_end == end) [[unlikely]]
		{
			return {inner_end, parse_code::invalid};
		}
		auto result =
			scn_cnt_define_in6addr_impl<allowv6shorten, allowv6uppercase, false, requirev6full>(inner_begin, inner_end, t);
		if (result.code != parse_code::ok || result.iter != inner_end) [[unlikely]]
		{
			return {result.iter, result.code == parse_code::ok ? parse_code::invalid : result.code};
		}
		return {inner_end + 1, parse_code::ok};
	}
	if constexpr (allowv6shorten)
	{
		return scn_cnt_define_in6addr_shorten_impl<allowv6uppercase>(begin, end, t);
	}
	else if constexpr (requirev6full)
	{
		return scn_cnt_define_in6addr_full_impl<allowv6uppercase>(begin, end, t);
	}
	else
	{
		return scn_cnt_define_in6addr_nonshorten_impl<allowv6uppercase>(begin, end, t);
	}
}

template <::std::integral char_type>
inline constexpr parse_result<char_type const *>
scn_ctx_define_in6addr_impl(ipv6_scan_state_t<char_type> &state, char_type const *begin, char_type const *end,
							posix_in6_addr &addr) noexcept
{
	auto result{
		scn_cnt_define_in6addr_impl<true, true, true, false>(begin, end, addr)};
	// If parsing fails exactly at the buffer end, treat it as a "need more input" case
	// for the streaming context scanner, similar to IPv4 behavior.
	if (result.code == parse_code::invalid && result.iter == end)
	{
		state.size = 1; // mark that we have an unfinished IPv6 parse for EOF handling
		result.code = parse_code::partial;
	}
	else if (result.code != parse_code::partial)
	{
		state.size = 0;
		state.integer_phase = scan_integral_context_phase::zero;
	}
	return result;
}

} // namespace details

namespace manipulators
{

template <ip_scan_flags flags, ::fast_io::details::iptypesimpl iptype>
inline constexpr ip_scan_manip_t<flags, ::std::remove_cvref_t<iptype> *> ip_scan_generic(iptype &ipaddr) noexcept
{
	return {__builtin_addressof(ipaddr)};
}

} // namespace manipulators

#if 0
inline constexpr mnp::ip_scan_manip_t<mnp::ip_scan_flags{0,1,1,0,0}, posix_in6_addr*> scan_alias_define(io_alias_t, posix_in6_addr& t) noexcept
{
	return { __builtin_addressof(t) };
}
#else
template <::fast_io::details::iptypesimpl iptype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::manipulators::ip_scan_manip_t<::fast_io::details::inaddrimpl<iptype>
															  ? ::fast_io::manipulators::ip_scan_default_inaddr_flags
															  : ::fast_io::manipulators::ip_scan_default_flags,
														  ::std::remove_cvref_t<iptype> *>
scan_alias_define(io_alias_t, iptype &t) noexcept
{
	return {__builtin_addressof(t)};
}
#endif

template <::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags,
		  ::fast_io::details::iptypesimpl iptype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr parse_result<char_type const *>
scan_contiguous_define(io_reserve_type_t<char_type, manipulators::ip_scan_manip_t<flags, iptype *>>,
					   char_type const *begin, char_type const *end,
					   manipulators::ip_scan_manip_t<flags, iptype *> val) noexcept
{
	if constexpr (::std::same_as<iptype, posix_in_addr>)
	{
		auto result{details::scn_cnt_define_inaddr_impl(begin, end, *val.reference)};
		if constexpr (flags.requireport == true)
		{
			if (result.code != parse_code::ok) [[unlikely]]
			{
				return result;
			}
			::std::uint_least16_t port;
			return details::scn_cnt_define_port_impl(result.iter, end, port);
		}
		return result;
	}
	else if constexpr (::std::same_as<iptype, ::fast_io::ipv4>)
	{
		auto result{::fast_io::details::scn_cnt_define_inaddr_impl(begin, end, val.reference->address)};
		if (result.code != parse_code::ok) [[unlikely]]
		{
			return result;
		}
		begin = result.iter;
		if constexpr (flags.requireport == false)
		{
			val.reference->port = 0;
			return result;
		}
		else
		{
			return ::fast_io::details::scn_cnt_define_port_impl(begin, end, val.reference->port);
		}
	}
	else if constexpr (::std::same_as<iptype, ::fast_io::posix_in6_addr>)
	{
		auto result{
			details::scn_cnt_define_in6addr_impl<flags.allowv6shorten, flags.allowv6uppercase, flags.allowv6bracket,
												 flags.requirev6full>(begin, end, *val.reference)};
		if constexpr (flags.requireport == true)
		{
			if (result.code != parse_code::ok) [[unlikely]]
			{
				return result;
			}
			::std::uint_least16_t port;
			return details::scn_cnt_define_port_impl(result.iter, end, port);
		}
		return result;
	}
	else if constexpr (::std::same_as<iptype, ::fast_io::ipv6>)
	{
		auto result{
			::fast_io::details::scn_cnt_define_in6addr_impl<
				flags.allowv6shorten,
				flags.allowv6uppercase,
				flags.allowv6bracket,
				flags.requirev6full>(begin, end, val.reference->address)};
		if (result.code != parse_code::ok) [[unlikely]]
		{
			return result;
		}
		begin = result.iter;
		if constexpr (flags.requireport == false)
		{
			val.reference->port = 0;
			return result;
		}
		else
		{
			return ::fast_io::details::scn_cnt_define_port_impl(begin, end, val.reference->port);
		}

		return result;
	}
	else if constexpr (::std::same_as<iptype, ::fast_io::ip_address>)
	{
		/// @todo forgot?
		// return ::fast_io::details::prtrsv_ip_address_define_impl<flags.v6shorten, flags.v6uppercase,
		//	flags.showport ? true : flags.v6bracket, flags.v6full,
		//	flags.showport>(iter, val.reference);
		return {};
	}
	else
	{
		/// @todo forgot?
		// return ::fast_io::details::prtrsv_ip_define_impl<flags.v6shorten, flags.v6uppercase,
		//	flags.showport ? true : flags.v6bracket, flags.v6full,
		//	flags.showport>(iter, val.reference);
		return {};
	}
}

template <::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr io_type_t<ipv4_scan_state_t<char_type>> scan_context_type(
	io_reserve_type_t<char_type, ::fast_io::manipulators::ip_scan_manip_t<flags, posix_in_addr *>>) noexcept
{
	return {};
}

template <::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr parse_result<char_type const *> scan_context_define(
	::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::ip_scan_manip_t<flags, posix_in_addr *>>,
	ipv4_scan_state_t<char_type> &state, char_type const *begin, char_type const *end,
	::fast_io::manipulators::ip_scan_manip_t<flags, posix_in_addr *> t) noexcept
{
	return details::scn_ctx_define_inaddr_impl(state, begin, end, *t.reference);
}

template <::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr parse_code scan_context_eof_define(
	::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::ip_scan_manip_t<flags, posix_in_addr *>>,
	ipv4_scan_state_t<char_type> &state, ::fast_io::manipulators::ip_scan_manip_t<flags, posix_in_addr *> t) noexcept
{
	if (state.ip_phase != scan_ipv4_context_phase::addr3)
	{
		return parse_code::end_of_file;
	}
	else
	{
		return details::scan_int_contiguous_none_space_part_define_impl<10>(
				   state.buffer.data(), state.buffer.data() + state.size, t.reference->address[3])
			.code;
	}
}

template <::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr io_type_t<ipv4_scan_state_t<char_type>>
scan_context_type(io_reserve_type_t<char_type, ::fast_io::manipulators::ip_scan_manip_t<flags, ipv4 *>>) noexcept
{
	return {};
}

template <::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr parse_result<char_type const *>
scan_context_define(::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::ip_scan_manip_t<flags, ipv4 *>>,
					ipv4_scan_state_t<char_type> &state, char_type const *begin, char_type const *end,
					::fast_io::manipulators::ip_scan_manip_t<flags, ipv4 *> t) noexcept
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
		auto result{details::scn_ctx_define_inaddr_impl(state, begin, end, t.reference->address)};
		if (result.code != parse_code::ok)
		{
			return result;
		}
		begin = result.iter;
		state.integer_phase = scan_integral_context_phase::zero;
		state.size = 0;
		state.ip_phase = ::std::bit_cast<scan_ipv4_context_phase>(state.port_mark);
		[[fallthrough]];
	}
	// clang bug for ::std::bit_cast is not a constexpr expression
#if !defined(__clang__)
	case ::std::bit_cast<scan_ipv4_context_phase>(state.port_mark):
	case ::std::bit_cast<scan_ipv4_context_phase>(state.port):
		return {details::scn_ctx_define_port_type_general_impl(state, begin, end, t.reference->port)};
	default:;
#else
	default:
		if (state.ip_phase == ::std::bit_cast<scan_ipv4_context_phase>(state.port_mark) ||
			state.ip_phase == ::std::bit_cast<scan_ipv4_context_phase>(state.port)) [[likely]]
		{
			return {details::scn_ctx_define_port_type_general_impl(state, begin, end, t.reference->port)};
		}
#endif
		::fast_io::unreachable();
	}
}

template <::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr parse_code scan_context_eof_define(
	::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::ip_scan_manip_t<flags, ipv4 *>>,
	ipv4_scan_state_t<char_type> &state, ::fast_io::manipulators::ip_scan_manip_t<flags, ipv4 *> t) noexcept
{
	if (state.ip_phase != ::std::bit_cast<scan_ipv4_context_phase>(state.port))
	{
		return parse_code::end_of_file;
	}
	else
	{
		return details::scan_int_contiguous_none_space_part_define_impl<10>(
				   state.buffer.data(), state.buffer.data() + state.size, t.reference->port)
			.code;
	}
}

template <::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr io_type_t<ipv6_scan_state_t<char_type>>
scan_context_type(io_reserve_type_t<char_type, ::fast_io::manipulators::ip_scan_manip_t<flags, posix_in6_addr *>>) noexcept
{
	return {};
}

template <::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr io_type_t<ipv6_scan_state_t<char_type>>
scan_context_type(io_reserve_type_t<char_type, ::fast_io::manipulators::ip_scan_manip_t<flags, ipv6 *>>) noexcept
{
	return {};
}

template <::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr parse_result<char_type const *>
scan_context_define(
	io_reserve_type_t<char_type,
					  ::fast_io::manipulators::ip_scan_manip_t<flags, posix_in6_addr *>>,
	ipv6_scan_state_t<char_type> &state, char_type const *begin, char_type const *end,
	::fast_io::manipulators::ip_scan_manip_t<flags, posix_in6_addr *> t) noexcept
{
	// IPv6 only has one parsing stage, entering it means full parsing
	auto result{::fast_io::details::scn_ctx_define_in6addr_impl(state, begin, end, *t.reference)};
	return result;
}

template <::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr parse_result<char_type const *>
scan_context_define(
	io_reserve_type_t<char_type,
					  ::fast_io::manipulators::ip_scan_manip_t<flags, ipv6 *>>,
	ipv6_scan_state_t<char_type> &state, char_type const *begin, char_type const *end,
	::fast_io::manipulators::ip_scan_manip_t<flags, ipv6 *> t) noexcept
{
	auto result{
		::fast_io::details::scn_ctx_define_in6addr_impl(state, begin, end, t.reference->address)};

	if (result.code != parse_code::ok)
	{
		return result;
	}

	if constexpr (flags.requireport == false)
	{
		t.reference->port = 0;
		return result;
	}

	// port parsing
	auto newstate = ::std::bit_cast<ip_port_scan_state_t<char_type>>(state);
	newstate.ip_phase = newstate.port_mark;

	auto result2{
		::fast_io::details::scn_ctx_define_port_impl(newstate, result.iter, end,
													 t.reference->port)};

	state = ::std::bit_cast<ipv6_scan_state_t<char_type>>(newstate);
	return result2;
}

template <::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr parse_code
scan_context_eof_define(
	io_reserve_type_t<char_type,
					  ::fast_io::manipulators::ip_scan_manip_t<flags, posix_in6_addr *>>,
	ipv6_scan_state_t<char_type> &state,
	::fast_io::manipulators::ip_scan_manip_t<flags, posix_in6_addr *> t) noexcept
{
	// IPv6 itself does not have the concept of "the last group of partial digits"
	// So if it's not parsed, it's end_of_file
	return state.size == 0
			   ? parse_code::ok
			   : parse_code::invalid;
}

template <::std::integral char_type, ::fast_io::manipulators::ip_scan_flags flags>
inline constexpr parse_code
scan_context_eof_define(
	io_reserve_type_t<char_type,
					  ::fast_io::manipulators::ip_scan_manip_t<flags, ipv6 *>>,
	ipv6_scan_state_t<char_type> &state,
	::fast_io::manipulators::ip_scan_manip_t<flags, ipv6 *> t) noexcept
{
	if constexpr (flags.requireport == false)
	{
		return parse_code::ok;
	}

	// Try to complete the integer tail
	return ::fast_io::details::scan_int_contiguous_none_space_part_define_impl<10>(
			   state.buffer.data(),
			   state.buffer.data() + state.size,
			   t.reference->port)
		.code;
}

} // namespace fast_io
