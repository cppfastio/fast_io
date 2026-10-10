#pragma once

namespace fast_io
{

namespace details
{

/*
 * string_filters validators: each returns whether the range satisfies the
 * grammar. Validation runs eagerly inside the filter functions so an
 * invalid value throws before the string reaches the output or a syscall.
 */

/* RFC 7230 tchar: DIGIT / ALPHA / "!#$%&'*+-.^_`|~" */
template <::std::integral ch_type>
inline constexpr bool string_filters_is_tchar(ch_type ch) noexcept
{
	if (::fast_io::char_category::is_c_alnum(ch))
	{
		return true;
	}
	switch (ch)
	{
	case ::fast_io::char_literal_v<u8'!', ch_type>:
	case ::fast_io::char_literal_v<u8'#', ch_type>:
	case ::fast_io::char_literal_v<u8'$', ch_type>:
	case ::fast_io::char_literal_v<u8'%', ch_type>:
	case ::fast_io::char_literal_v<u8'&', ch_type>:
	case ::fast_io::char_literal_v<u8'\'', ch_type>:
	case ::fast_io::char_literal_v<u8'*', ch_type>:
	case ::fast_io::char_literal_v<u8'+', ch_type>:
	case ::fast_io::char_literal_v<u8'-', ch_type>:
	case ::fast_io::char_literal_v<u8'.', ch_type>:
	case ::fast_io::char_literal_v<u8'^', ch_type>:
	case ::fast_io::char_literal_v<u8'_', ch_type>:
	case ::fast_io::char_literal_v<u8'`', ch_type>:
	case ::fast_io::char_literal_v<u8'|', ch_type>:
	case ::fast_io::char_literal_v<u8'~', ch_type>:
		return true;
	default:
		return false;
	}
}

/* RFC 3986 characters allowed literally in a URI reference:
 * unreserved (alnum + -._~) / gen-delims (:/?#[]@) / sub-delims (!$&'()*+,;=).
 * '%' is handled separately — it must introduce two hex digits. */
template <::std::integral ch_type>
inline constexpr bool string_filters_is_url_char(ch_type ch) noexcept
{
	if (::fast_io::char_category::is_c_alnum(ch))
	{
		return true;
	}
	switch (ch)
	{
	case ::fast_io::char_literal_v<u8'-', ch_type>:
	case ::fast_io::char_literal_v<u8'.', ch_type>:
	case ::fast_io::char_literal_v<u8'_', ch_type>:
	case ::fast_io::char_literal_v<u8'~', ch_type>:
	case ::fast_io::char_literal_v<u8':', ch_type>:
	case ::fast_io::char_literal_v<u8'/', ch_type>:
	case ::fast_io::char_literal_v<u8'?', ch_type>:
	case ::fast_io::char_literal_v<u8'#', ch_type>:
	case ::fast_io::char_literal_v<u8'[', ch_type>:
	case ::fast_io::char_literal_v<u8']', ch_type>:
	case ::fast_io::char_literal_v<u8'@', ch_type>:
	case ::fast_io::char_literal_v<u8'!', ch_type>:
	case ::fast_io::char_literal_v<u8'$', ch_type>:
	case ::fast_io::char_literal_v<u8'&', ch_type>:
	case ::fast_io::char_literal_v<u8'\'', ch_type>:
	case ::fast_io::char_literal_v<u8'(', ch_type>:
	case ::fast_io::char_literal_v<u8')', ch_type>:
	case ::fast_io::char_literal_v<u8'*', ch_type>:
	case ::fast_io::char_literal_v<u8'+', ch_type>:
	case ::fast_io::char_literal_v<u8',', ch_type>:
	case ::fast_io::char_literal_v<u8';', ch_type>:
	case ::fast_io::char_literal_v<u8'=', ch_type>:
		return true;
	default:
		return false;
	}
}

/* Host header / DNS name grammar: a bracketed IP literal ("[...]" must
 * contain at least one ':', inner chars restricted to hex digits, ':' and
 * '.') or dot-separated labels of [0-9A-Za-z-] where no label is empty,
 * starts or ends with '-', or exceeds 63 characters; the whole name is at
 * most 253 characters and may carry one trailing dot (FQDN root). */
template <::std::integral ch_type>
inline constexpr bool string_filters_validate_host(ch_type const *first,
												   ch_type const *last) noexcept
{
	if (first == last)
	{
		return false;
	}
	if (*first == ::fast_io::char_literal_v<u8'[', ch_type>)
	{
		auto const closing{last - 1};
		if (closing == first ||
			*closing != ::fast_io::char_literal_v<u8']', ch_type>)
		{
			return false;
		}
		bool has_colon{};
		for (auto p{first + 1}; p != closing; ++p)
		{
			auto const ch{*p};
			if (ch == ::fast_io::char_literal_v<u8':', ch_type>)
			{
				has_colon = true;
			}
			else if (!::fast_io::char_category::is_c_xdigit(ch) &&
					 ch != ::fast_io::char_literal_v<u8'.', ch_type>)
			{
				return false;
			}
		}
		return has_colon;
	}
	::std::size_t label_len{};
	for (auto p{first}; p != last; ++p)
	{
		auto const ch{*p};
		if (ch == ::fast_io::char_literal_v<u8'.', ch_type>)
		{
			if (label_len == 0 || *(p - 1) == ::fast_io::char_literal_v<u8'-', ch_type>)
			{
				return false;
			}
			label_len = 0;
			continue;
		}
		if (ch == ::fast_io::char_literal_v<u8'-', ch_type>)
		{
			if (label_len == 0)
			{
				return false;
			}
		}
		else if (!::fast_io::char_category::is_c_alnum(ch))
		{
			return false;
		}
		++label_len;
		if (label_len > 63)
		{
			return false;
		}
	}
	if (label_len == 0)
	{
		/* the range ended in a dot — only a single trailing dot on a
		 * non-empty name is acceptable */
		return *(last - 1) == ::fast_io::char_literal_v<u8'.', ch_type> &&
			   first + 1 != last;
	}
	if (*(last - 1) == ::fast_io::char_literal_v<u8'-', ch_type>)
	{
		return false;
	}
	return static_cast<::std::size_t>(last - first) <= 253;
}

/* URI reference per RFC 3986: every character must be in the allowed set
 * above and '%' must introduce a two-digit percent-encoding. Rejects
 * controls, spaces, non-ASCII code units and RFC-forbidden characters
 * ("<>\^`{|}). */
template <::std::integral ch_type>
inline constexpr bool string_filters_validate_url(ch_type const *first,
												  ch_type const *last) noexcept
{
	for (auto p{first}; p != last; ++p)
	{
		auto const ch{*p};
		if (ch == ::fast_io::char_literal_v<u8'%', ch_type>)
		{
			if (last - p < 3 ||
				!::fast_io::char_category::is_c_xdigit(p[1]) ||
				!::fast_io::char_category::is_c_xdigit(p[2]))
			{
				return false;
			}
			p += 2;
			continue;
		}
		if (!::fast_io::details::string_filters_is_url_char(ch))
		{
			return false;
		}
	}
	return true;
}

/* HTTP field-value content per RFC 9110: HTAB, SP..~ and obs-text (0x80+)
 * only — every control character, DEL included, is rejected. This is the
 * guard against response/request header injection. */
template <::std::integral ch_type>
inline constexpr bool string_filters_validate_header(ch_type const *first,
													 ch_type const *last) noexcept
{
	for (; first != last; ++first)
	{
		::std::uint_least32_t u;
		if constexpr (sizeof(ch_type) == 1)
		{
			u = static_cast<::std::uint_least8_t>(*first);
		}
		else
		{
			if constexpr (::std::signed_integral<ch_type>)
			{
				if (*first < 0)
				{
					return false;
				}
			}
			u = static_cast<::std::uint_least32_t>(*first);
		}
		if (u == 0x09)
		{
			continue;
		}
		if (u < 0x20 || u == 0x7F)
		{
			return false;
		}
	}
	return true;
}

/* plain SQL string content — rejects the quoting and comment machinery
 * usable for injection: ' " ` ; \ NUL Ctrl-Z(0x1A) '#' and the sequences
 * --, / *, * /. This is a whitelist-style gate for simple values; it does
 * not escape and must not be used where arbitrary text is legitimate. */
template <::std::integral ch_type>
inline constexpr bool string_filters_validate_sql_string(ch_type const *first,
														 ch_type const *last) noexcept
{
	for (auto p{first}; p != last; ++p)
	{
		auto const ch{*p};
		switch (ch)
		{
		case ::fast_io::char_literal_v<u8'\'', ch_type>:
		case ::fast_io::char_literal_v<u8'"', ch_type>:
		case ::fast_io::char_literal_v<u8'`', ch_type>:
		case ::fast_io::char_literal_v<u8';', ch_type>:
		case ::fast_io::char_literal_v<u8'\\', ch_type>:
		case ::fast_io::char_literal_v<u8'#', ch_type>:
		case ::fast_io::char_literal_v<u8'\0', ch_type>:
		case ::fast_io::char_literal_v<u8'\x1A', ch_type>:
			return false;
		default:
			break;
		}
		if (p + 1 != last)
		{
			auto const next{p[1]};
			if ((ch == ::fast_io::char_literal_v<u8'-', ch_type> &&
				 next == ::fast_io::char_literal_v<u8'-', ch_type>) ||
				(ch == ::fast_io::char_literal_v<u8'/', ch_type> &&
				 next == ::fast_io::char_literal_v<u8'*', ch_type>) ||
				(ch == ::fast_io::char_literal_v<u8'*', ch_type> &&
				 next == ::fast_io::char_literal_v<u8'/', ch_type>))
			{
				return false;
			}
		}
	}
	return true;
}

/* HTTP token: non-empty and every character a tchar — header names,
 * method names, cookie names, auth schemes. */
template <::std::integral ch_type>
inline constexpr bool string_filters_validate_token(ch_type const *first,
													ch_type const *last) noexcept
{
	if (first == last)
	{
		return false;
	}
	for (; first != last; ++first)
	{
		if (!::fast_io::details::string_filters_is_tchar(*first))
		{
			return false;
		}
	}
	return true;
}

/* percent-encoded request-target path: every '%' must introduce two
 * hex digits; decoded characters reject C0 controls, DEL, NUL (a %00
 * would truncate a C-string path) and '\' (a windows separator
 * hazard); any '/'-separated segment that decodes to exactly ".."
 * fails — the path-traversal guard for request paths that get mapped
 * onto a filesystem. Decoded non-ASCII (UTF-8 filenames) and '.'
 * segments stay legal */
template <::std::integral ch_type>
inline constexpr bool string_filters_validate_url_path(ch_type const *first,
													   ch_type const *last) noexcept
{
	constexpr auto slashesc{::fast_io::char_literal_v<u8'/', ch_type>};
	constexpr auto percent{::fast_io::char_literal_v<u8'%', ch_type>};
	constexpr auto nine{::fast_io::char_literal_v<u8'9', ch_type>};
	constexpr auto zero{::fast_io::char_literal_v<u8'0', ch_type>};
	constexpr auto hexval{[&](ch_type c) noexcept -> unsigned {
		return c <= nine ? static_cast<unsigned>(c - zero)
						 : static_cast<unsigned>((c | 0x20) - 'a' + 10);
	}};
	for (auto seg{first};;)
	{
		auto se{seg};
		while (se != last && *se != slashesc)
		{
			++se;
		}
		bool dotdot{true};
		::std::size_t nch{};
		for (auto p{seg}; p != se;)
		{
			::std::uint_least32_t ch;
			if (*p == percent)
			{
				if (se - p < 3 || !::fast_io::char_category::is_c_xdigit(p[1]) ||
					!::fast_io::char_category::is_c_xdigit(p[2]))
				{
					return false;
				}
				ch = hexval(p[1]) * 16 + hexval(p[2]);
				p += 3;
			}
			else
			{
				ch = static_cast<::std::uint_least32_t>(*p);
				++p;
			}
			if (ch < 0x20 || ch == 0x7f || ch == 0x5c)
			{
				return false;
			}
			if (dotdot)
			{
				if (nch == 2 || ch != 0x2e)
				{
					dotdot = false;
				}
				++nch;
			}
		}
		if (dotdot && nch == 2)
		{
			return false;
		}
		if (se == last)
		{
			return true;
		}
		seg = se + 1;
	}
}

enum class string_filter_kind : ::std::uint_least32_t
{
	host,
	url,
	sql_string,
	header_value,
	token,
	url_path
};

template <string_filter_kind kind, ::std::integral ch_type>
inline constexpr void string_filters_validate_or_throw(ch_type const *first,
													   ch_type const *last)
	FAST_IO_HERBCEPTIONS_THROWS
{
	bool valid;
	if constexpr (kind == string_filter_kind::host)
	{
		valid = string_filters_validate_host(first, last);
	}
	else if constexpr (kind == string_filter_kind::url)
	{
		valid = string_filters_validate_url(first, last);
	}
	else if constexpr (kind == string_filter_kind::sql_string)
	{
		valid = string_filters_validate_sql_string(first, last);
	}
	else if constexpr (kind == string_filter_kind::header_value)
	{
		valid = string_filters_validate_header(first, last);
	}
	else if constexpr (kind == string_filter_kind::token)
	{
		valid = string_filters_validate_token(first, last);
	}
	else if constexpr (kind == string_filter_kind::url_path)
	{
		valid = string_filters_validate_url_path(first, last);
	}
	else
	{
		static_assert(!sizeof(ch_type), "unknown string_filter_kind");
	}
	if (!valid)
	{
		::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::invalid);
	}
}

/*
 * The filters are not manipulators — they sanitize a string and return a
 * view of it:
 * - a source with .c_str() is null-terminated by contract, so the result
 *   keeps that information as os_c_str_with_known_size;
 * - a plain contiguous range (string_view, span, array) has no such
 *   guarantee, so the result is a basic_io_scatter_t view — for a char
 *   array an implicit trailing NUL is the literal terminator and is not
 *   part of the validated content.
 * Raw char const* is deliberately not accepted: wrap it in mnp::os_c_str,
 * matching the print rule that a pointer is never silently a C string.
 */
template <string_filter_kind kind, typename T>
inline constexpr auto string_filters_apply(T &&t) FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (requires(T &tt) {
					  { tt.c_str() };
					  requires ::std::integral<::std::remove_cv_t<
						  ::std::remove_pointer_t<decltype(tt.c_str())>>>;
				  })
	{
		/* null-terminated source: os_c_str, os_c_str_with_known_size,
		 * cstring_view, basic_string, std::string, ... */
		auto const ptr{t.c_str()};
		::std::size_t n;
		if constexpr (requires { t.size(); })
		{
			n = t.size();
		}
		else
		{
			n = ::fast_io::cstr_len(ptr);
		}
		string_filters_validate_or_throw<kind>(ptr, ptr + n);
		return ::fast_io::manipulators::os_c_str_with_known_size(ptr, n);
	}
	else if constexpr (::std::ranges::contiguous_range<T &&> &&
					   ::std::integral<::std::ranges::range_value_t<T &&>>)
	{
		auto const first{::std::ranges::data(t)};
		::std::size_t n{::std::ranges::size(t)};
		if constexpr (::std::is_bounded_array_v<::std::remove_reference_t<T>>)
		{
			if (n != 0 && first[n - 1] == ::std::ranges::range_value_t<T &&>{})
			{
				--n;
			}
		}
		string_filters_validate_or_throw<kind>(first, first + n);
		using ch_type = ::std::ranges::range_value_t<T &&>;
		return ::fast_io::basic_io_scatter_t<ch_type>{first, n};
	}
	else
	{
		static_assert(!sizeof(T),
					  "string_filters: argument must be a contiguous character range or "
					  "provide .c_str()");
	}
}

} // namespace details

namespace manipulators
{

namespace string_filters
{

/* RFC 1034 hostname / IP-literal validation — the guard for Host headers
 * and any hostname-shaped input. */
template <typename T>
inline constexpr auto host(T &&t) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::string_filters_apply<
		::fast_io::details::string_filter_kind::host>(::fast_io::freestanding::forward<T>(t));
}

inline constexpr void host(::std::nullptr_t) = delete;

/* RFC 3986 URI character validation including %-encoding well-formedness. */
template <typename T>
inline constexpr auto url(T &&t) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::string_filters_apply<
		::fast_io::details::string_filter_kind::url>(::fast_io::freestanding::forward<T>(t));
}

inline constexpr void url(::std::nullptr_t) = delete;

/* plain SQL string content for values spliced into a statement — rejects
 * the quoting/comment machinery usable for injection; not an escaper */
template <typename T>
inline constexpr auto sql_string(T &&t) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::string_filters_apply<
		::fast_io::details::string_filter_kind::sql_string>(::fast_io::freestanding::forward<T>(t));
}

inline constexpr void sql_string(::std::nullptr_t) = delete;

/* RFC 9110 field-value content — no control characters or DEL; the header
 * injection guard for arbitrary field values. */
template <typename T>
inline constexpr auto header_value(T &&t) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::string_filters_apply<
		::fast_io::details::string_filter_kind::header_value>(
		::fast_io::freestanding::forward<T>(t));
}

inline constexpr void header_value(::std::nullptr_t) = delete;

/* RFC 7230 token — header names, method names, auth schemes. */
template <typename T>
inline constexpr auto token(T &&t) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::string_filters_apply<
		::fast_io::details::string_filter_kind::token>(::fast_io::freestanding::forward<T>(t));
}

inline constexpr void token(::std::nullptr_t) = delete;

/* percent-encoded request-target path — rejects malformed escapes,
 * decoded control/del/NUL/backslash characters, and any segment that
 * decodes to ".."; the traversal guard for paths bound for a
 * filesystem */
template <typename T>
inline constexpr auto url_path(T &&t) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::string_filters_apply<
		::fast_io::details::string_filter_kind::url_path>(
		::fast_io::freestanding::forward<T>(t));
}

inline constexpr void url_path(::std::nullptr_t) = delete;

} // namespace string_filters

} // namespace manipulators

} // namespace fast_io
