#pragma once

namespace fast_io
{
namespace manipulators
{
/*
 * mnp::imf_date_get(t) — scan an RFC 9110 IMF-fixdate ("Sun, 06 Nov 1994
 * 08:49:37 GMT") into an iso8601_timestamp, a posix_statx_timestamp64,
 * or a plain integral type holding unix seconds (conversions run
 * through iso8601_to_unix_timestamp_impl).
 * This is the format the HTTP Date / Last-Modified / If-*-Since headers
 * use — obs-date forms (rfc850, asctime) are not accepted.
 */
template <typename T>
struct imf_date_manip_t
{
	using manip_tag = manip_tag_t;
	T *reference;
};

template <typename T>
	requires(::std::same_as<::std::remove_cvref_t<T>, ::fast_io::iso8601_timestamp> ||
			 ::std::same_as<::std::remove_cvref_t<T>, ::fast_io::posix_statx_timestamp64> ||
			 (::fast_io::details::my_integral<::std::remove_cvref_t<T>> &&
			  !::std::same_as<::std::remove_cvref_t<T>, bool>))
inline constexpr imf_date_manip_t<T> imf_date_get(T &t) noexcept
{
	return {__builtin_addressof(t)};
}
} // namespace manipulators

namespace details
{
constexpr char imf_wkday_names[][4]{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
constexpr char imf_month_names[][4]{"Jan", "Feb", "Mar", "Apr", "May", "Jun",
									"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

template <::std::integral char_type>
inline constexpr int imf_dig(char_type c) noexcept
{
	return (c >= ::fast_io::char_literal_v<u8'0', char_type> &&
			c <= ::fast_io::char_literal_v<u8'9', char_type>)
			   ? static_cast<int>(c - ::fast_io::char_literal_v<u8'0', char_type>)
			   : -1;
}

template <::std::integral char_type>
inline constexpr int imf_two(char_type const *p) noexcept
{
	auto const lo{::fast_io::details::imf_dig(p[1])};
	return lo < 0 ? -1 : ::fast_io::details::imf_dig(p[0]) * 10 + lo;
}

template <::std::integral char_type>
inline constexpr bool imf_name_match(char_type const *p, char const *lit) noexcept
{
	/* ascii literals — identity mapping for every fast_io char type */
	return p[0] == static_cast<char_type>(static_cast<unsigned char>(lit[0])) &&
		   p[1] == static_cast<char_type>(static_cast<unsigned char>(lit[1])) &&
		   p[2] == static_cast<char_type>(static_cast<unsigned char>(lit[2]));
}

/* "<wkday>, DD Mon YYYY HH:MM:SS GMT" — fixed 29 bytes. The weekday name
 * must be one of the seven, but the spec date need not agree with it —
 * same looseness every HTTP parser applies */
template <::std::integral char_type>
inline constexpr parse_result<char_type const *>
scn_cnt_define_imfdate_impl(char_type const *begin, char_type const *end,
							::fast_io::iso8601_timestamp &t) noexcept
{
	constexpr ::std::size_t imf_len{29};
	constexpr auto invalid{::fast_io::freestanding::parse_errc::invalid};
	constexpr auto comma{::fast_io::char_literal_v<u8',', char_type>};
	constexpr auto space{::fast_io::char_literal_v<u8' ', char_type>};
	constexpr auto colon{::fast_io::char_literal_v<u8':', char_type>};
	if (static_cast<::std::size_t>(end - begin) < imf_len)
	{
		return {begin, invalid};
	}
	bool wkok{};
	for (auto const &w : ::fast_io::details::imf_wkday_names)
	{
		if (::fast_io::details::imf_name_match(begin, w))
		{
			wkok = true;
			break;
		}
	}
	if (!wkok || begin[3] != comma || begin[4] != space)
	{
		return {begin, invalid};
	}
	auto const day{::fast_io::details::imf_two(begin + 5)};
	if (day <= 0 || 31 < day || begin[7] != space)
	{
		return {begin, invalid};
	}
	unsigned mo{};
	for (; mo != 12; ++mo)
	{
		if (::fast_io::details::imf_name_match(begin + 8, ::fast_io::details::imf_month_names[mo]))
		{
			break;
		}
	}
	if (mo == 12 || begin[11] != space)
	{
		return {begin, invalid};
	}
	int const y0{::fast_io::details::imf_dig(begin[12])};
	int const y1{::fast_io::details::imf_dig(begin[13])};
	int const y2{::fast_io::details::imf_dig(begin[14])};
	int const y3{::fast_io::details::imf_dig(begin[15])};
	if ((y0 | y1 | y2 | y3) < 0 || begin[16] != space)
	{
		return {begin, invalid};
	}
	auto const hh{::fast_io::details::imf_two(begin + 17)};
	auto const mm{::fast_io::details::imf_two(begin + 20)};
	auto const ss{::fast_io::details::imf_two(begin + 23)};
	if (begin[19] != colon || begin[22] != colon || hh < 0 || 24 <= hh || mm < 0 ||
		60 <= mm || ss < 0 || 61 <= ss || begin[25] != space ||
		begin[26] != ::fast_io::char_literal_v<u8'G', char_type> ||
		begin[27] != ::fast_io::char_literal_v<u8'M', char_type> ||
		begin[28] != ::fast_io::char_literal_v<u8'T', char_type>)
	{
		return {begin, invalid};
	}
	t = {.year = y0 * 1000 + y1 * 100 + y2 * 10 + y3,
		 .month = static_cast<::std::uint_least8_t>(mo + 1),
		 .day = static_cast<::std::uint_least8_t>(day),
		 .hours = static_cast<::std::uint_least8_t>(hh),
		 .minutes = static_cast<::std::uint_least8_t>(mm),
		 .seconds = static_cast<::std::uint_least8_t>(ss),
		 .nanoseconds = 0,
		 .timezone = 0};
	return {begin + imf_len, ::fast_io::freestanding::parse_errc::ok};
}
} // namespace details

namespace details
{
template <typename T>
inline constexpr void imf_date_assign_impl(T *ref, ::fast_io::iso8601_timestamp const &iso) noexcept
{
	if constexpr (::std::same_as<::std::remove_cvref_t<T>, ::fast_io::iso8601_timestamp>)
	{
		*ref = iso;
	}
	else if constexpr (::fast_io::details::my_integral<::std::remove_cvref_t<T>>)
	{
		*ref = static_cast<T>(::fast_io::details::iso8601_to_unix_timestamp_impl(iso).tv_sec);
	}
	else
	{
		*ref = ::fast_io::details::iso8601_to_unix_timestamp_impl(iso);
	}
}
} // namespace details

template <::std::integral char_type, typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr parse_result<char_type const *>
scan_contiguous_define(::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::imf_date_manip_t<T>>,
					   char_type const *begin, char_type const *end,
					   ::fast_io::manipulators::imf_date_manip_t<T> t) noexcept
{
	if constexpr (::std::same_as<::std::remove_cvref_t<T>, ::fast_io::iso8601_timestamp>)
	{
		return ::fast_io::details::scn_cnt_define_imfdate_impl(begin, end, *t.reference);
	}
	else
	{
		::fast_io::iso8601_timestamp iso{};
		auto const res{::fast_io::details::scn_cnt_define_imfdate_impl(begin, end, iso)};
		if (res.code == ::fast_io::freestanding::parse_errc::ok)
		{
			::fast_io::details::imf_date_assign_impl(t.reference, iso);
		}
		return res;
	}
}

/*
 * context scanner — the state for scan(stream, mnp::imf_date_get(t)):
 * the fixdate is a fixed 29 bytes, so the machine is just a fill
 * counter; the contiguous impl does the final parse at completion,
 * the same shape scn_ctx_define_in6addr_impl uses */
template <::std::integral char_type>
struct imf_date_scan_state_t
{
	char_type buffer[29];
	::std::uint_least8_t size{};
};

template <::std::integral char_type, typename T>
inline constexpr io_type_t<imf_date_scan_state_t<char_type>>
	scan_context_type(::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::imf_date_manip_t<T>>) noexcept
{
	return {};
}

template <::std::integral char_type, typename T>
inline constexpr parse_result<char_type const *>
scan_context_define(::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::imf_date_manip_t<T>>,
					imf_date_scan_state_t<char_type> &state, char_type const *begin,
					char_type const *end, ::fast_io::manipulators::imf_date_manip_t<T> t) noexcept
{
	auto const avail{static_cast<::std::size_t>(end - begin)};
	auto const need{static_cast<::std::size_t>(29 - state.size)};
	auto const n{avail < need ? avail : need};
	for (::std::size_t i{}; i != n; ++i)
	{
		state.buffer[state.size + i] = begin[i];
	}
	auto const it{begin + n};
	state.size = static_cast<::std::uint_least8_t>(state.size + n);
	if (state.size != 29)
	{
		return {it, ::fast_io::freestanding::parse_errc::partial};
	}
	::fast_io::iso8601_timestamp iso{};
	auto const res{::fast_io::details::scn_cnt_define_imfdate_impl(
		state.buffer, state.buffer + 29, iso)};
	if (res.code != ::fast_io::freestanding::parse_errc::ok)
	{
		return {it, res.code};
	}
	::fast_io::details::imf_date_assign_impl(t.reference, iso);
	return {it, ::fast_io::freestanding::parse_errc::ok};
}

template <::std::integral char_type, typename T>
inline constexpr ::fast_io::freestanding::parse_errc
scan_context_eof_define(::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::imf_date_manip_t<T>>,
						imf_date_scan_state_t<char_type> &state,
						::fast_io::manipulators::imf_date_manip_t<T>) noexcept
{
	return state.size == 29 ? ::fast_io::freestanding::parse_errc::ok
							: ::fast_io::freestanding::parse_errc::end_of_file;
}

} // namespace fast_io
