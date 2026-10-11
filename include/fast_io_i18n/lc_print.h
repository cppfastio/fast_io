#pragma once


// lc_print — dump a whole locale in the LC_* category text form
// master's lc_print.h wrote through print_define, here over the rva
// wire image. The raw lc_locale pointer stays non-printable — the
// dump is an explicit manipulator, mnp::lc_dump(loc), printable on
// any stream and an lc arg inside an imbued one. The section dumped
// is the stream char_type's (char follows the file's declared
// codeset, char8_t/16/32 their own sections).
//
// Compiled programs (d_t_fmt, name_fmt, postal_fmt, tel_*_fmt and the
// era formats) have no text form in the file; they dump as \xNN
// escapes inside quotes. Everything else mirrors master's layout:
//   key\t"value"   /   strlist  key\t"a";"b"   /   key\t<int>
// grouping bytes print as decimals with 0xFF -> -1; sections are
// separated by a blank line.
//
// The hooks are lc_dynamic_reserve_printable: the count and write
// passes run the same emitters through the lc sinks (lc.h), so the
// reserved size is the exact char count and the write is bounded by
// it — they can never disagree.

namespace fast_io
{

namespace details
{

// ---------------------------------------------------------------------------
// line emitters — every line is key\t<value>\n; literals go through
// char_literal so one source serves every char_type
// ---------------------------------------------------------------------------

// key\t"text"\n — a text member resolved to char_type units
template <::std::integral char_type, typename sink>
inline constexpr void lc_dump_str(sink &sk, ::fast_io::u8string_view key,
								  ::fast_io::l10n::lc_scatter<char_type> m,
								  ::fast_io::l10n::lc_locale const *loc)
	FAST_IO_HERBCEPTIONS_THROWS
{
	lc_put_cstr<char_type>(sk, key);
	sk.put(::fast_io::char_literal_v<u8'\t', char_type>);
	sk.put(::fast_io::char_literal_v<u8'\"', char_type>);
	lc_put_scatter<char_type>(sk, ::fast_io::l10n::lc_get_scatter(loc, m));
	sk.put(::fast_io::char_literal_v<u8'\"', char_type>);
	sk.put(::fast_io::char_literal_v<u8'\n', char_type>);
}

// key\t<decimal>\n — an s32 member decoded from the LE file scalar
template <::std::integral char_type, typename sink>
inline constexpr void lc_dump_int(sink &sk, ::fast_io::u8string_view key,
								  ::std::int_least32_t v) noexcept
{
	lc_put_cstr<char_type>(sk, key);
	sk.put(::fast_io::char_literal_v<u8'\t', char_type>);
	lc_put_num<char_type>(sk, v, 0, 0);
	sk.put(::fast_io::char_literal_v<u8'\n', char_type>);
}

// key\t<g;g;...>\n — a grouping byte list; empty prints -1 and a 0xFF
// byte prints -1 like master's SIZE_MAX sentinel
template <::std::integral char_type, typename sink>
inline constexpr void lc_dump_bytes(sink &sk, ::fast_io::u8string_view key,
									::fast_io::l10n::lc_scatter<char8_t> m,
									::fast_io::l10n::lc_locale const *loc)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto const b{::fast_io::l10n::lc_get_scatter(loc, m)};
	lc_put_cstr<char_type>(sk, key);
	sk.put(::fast_io::char_literal_v<u8'\t', char_type>);
	if (b.base == nullptr || b.len == 0)
	{
		lc_put_cstr<char_type>(sk, u8"-1");
	}
	else
	{
		for (::std::size_t i{}; i < b.len; ++i)
		{
			if (i != 0)
			{
				sk.put(::fast_io::char_literal_v<u8';', char_type>);
			}
			auto const v{static_cast<::std::uint_least8_t>(b.base[i])};
			if (v == 0xFFu)
			{
				lc_put_cstr<char_type>(sk, u8"-1");
			}
			else
			{
				lc_put_num<char_type>(sk, v, 0, 0);
			}
		}
	}
	sk.put(::fast_io::char_literal_v<u8'\n', char_type>);
}

// the compiled fmt bytes as \xNN escapes — programs have no text form
// in the file, so the bytes are the field's only representation
template <::std::integral char_type, typename sink>
inline constexpr void lc_dump_prog_bytes(
	sink &sk, ::fast_io::basic_io_scatter_t<char8_t> b) noexcept
{
	for (::std::size_t i{}; i < b.len; ++i)
	{
		auto const v{static_cast<::std::uint_least8_t>(b.base[i])};
		sk.put(::fast_io::char_literal_v<u8'\\', char_type>);
		sk.put(::fast_io::char_literal_v<u8'x', char_type>);
		sk.put(::fast_io::char_literal<char_type>(
			static_cast<char8_t>(u8"0123456789abcdef"[v >> 4])));
		sk.put(::fast_io::char_literal<char_type>(
			static_cast<char8_t>(u8"0123456789abcdef"[v & 0xFu])));
	}
}

// key\t"\xNN..."\n — a compiled binfmt program member
template <::std::integral char_type, typename sink>
inline constexpr void lc_dump_prog(sink &sk, ::fast_io::u8string_view key,
								   ::fast_io::l10n::lc_scatter<char8_t> m,
								   ::fast_io::l10n::lc_locale const *loc)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto const b{::fast_io::l10n::lc_get_scatter(loc, m)};
	lc_put_cstr<char_type>(sk, key);
	sk.put(::fast_io::char_literal_v<u8'\t', char_type>);
	sk.put(::fast_io::char_literal_v<u8'\"', char_type>);
	lc_dump_prog_bytes<char_type>(sk, b);
	sk.put(::fast_io::char_literal_v<u8'\"', char_type>);
	sk.put(::fast_io::char_literal_v<u8'\n', char_type>);
}

// key\t"a";"b";...\n — a fixed inline scatter array (abday, am_pm
// & friends); always emitted, absent elements print ""
template <::std::integral char_type, typename sink, ::std::size_t n>
inline constexpr void lc_dump_strs(
	sink &sk, ::fast_io::u8string_view key,
	::fast_io::l10n::lc_scatter<char_type> const (&arr)[n],
	::fast_io::l10n::lc_locale const *loc) FAST_IO_HERBCEPTIONS_THROWS
{
	lc_put_cstr<char_type>(sk, key);
	sk.put(::fast_io::char_literal_v<u8'\t', char_type>);
	for (::std::size_t i{}; i < n; ++i)
	{
		if (i != 0)
		{
			sk.put(::fast_io::char_literal_v<u8';', char_type>);
		}
		sk.put(::fast_io::char_literal_v<u8'\"', char_type>);
		lc_put_scatter<char_type>(
			sk, ::fast_io::l10n::lc_get_scatter(loc, arr[i]));
		sk.put(::fast_io::char_literal_v<u8'\"', char_type>);
	}
	sk.put(::fast_io::char_literal_v<u8'\n', char_type>);
}

// a strref-table member's elements, without the key — the line is
// key\t"a";"b";...\n when the list is present, absent lists emit
// nothing at all (master's empty-list early return)
template <::std::integral char_type, typename sink>
inline constexpr void lc_dump_list_body(
	sink &sk,
	::fast_io::l10n::lc_scatter<::fast_io::l10n::lc_scatter<char_type>> m,
	::fast_io::l10n::lc_locale const *loc) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const tbl{::fast_io::l10n::lc_get_scatter(loc, m)};
	if (tbl.base == nullptr || tbl.len == 0)
	{
		return;
	}
	for (::std::size_t i{}; i < tbl.len; ++i)
	{
		if (i != 0)
		{
			sk.put(::fast_io::char_literal_v<u8';', char_type>);
		}
		sk.put(::fast_io::char_literal_v<u8'\"', char_type>);
		lc_put_scatter<char_type>(
			sk, ::fast_io::l10n::lc_get_scatter(loc, tbl.base[i]));
		sk.put(::fast_io::char_literal_v<u8'\"', char_type>);
	}
}

template <::std::integral char_type, typename sink>
inline constexpr void lc_dump_list(
	sink &sk, ::fast_io::u8string_view key,
	::fast_io::l10n::lc_scatter<::fast_io::l10n::lc_scatter<char_type>> m,
	::fast_io::l10n::lc_locale const *loc) FAST_IO_HERBCEPTIONS_THROWS
{
	if (::fast_io::l10n::lc_u32(m.ref.off) == 0)
	{
		return;
	}
	lc_put_cstr<char_type>(sk, key);
	sk.put(::fast_io::char_literal_v<u8'\t', char_type>);
	lc_dump_list_body<char_type>(sk, m, loc);
	sk.put(::fast_io::char_literal_v<u8'\n', char_type>);
}

// one era date — "+*" / "-*" are the open-ended sentinels (INT32
// extremes like the parser writes), else yyyy/mm/dd
template <::std::integral char_type, typename sink>
inline constexpr void lc_dump_era_date(sink &sk, ::std::int_least32_t y,
									   ::std::int_least32_t mo,
									   ::std::int_least32_t d)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(sk.put(char_type{})) ||
								   !noexcept(lc_put_num<char_type>(sk, y, 0, 0)))
{
	constexpr ::std::int_least32_t mx{::std::numeric_limits<::std::int_least32_t>::max()};
	constexpr ::std::int_least32_t mn{::std::numeric_limits<::std::int_least32_t>::min()};
	if (y == mx || y == mn)
	{
		sk.put(y == mx ? ::fast_io::char_literal_v<u8'+', char_type>
					   : ::fast_io::char_literal_v<u8'-', char_type>);
		sk.put(::fast_io::char_literal_v<u8'*', char_type>);
		return;
	}
	lc_put_num<char_type>(sk, y, 0, 0);
	sk.put(::fast_io::char_literal_v<u8'/', char_type>);
	lc_put_num<char_type>(sk, mo, 0, 0);
	sk.put(::fast_io::char_literal_v<u8'/', char_type>);
	lc_put_num<char_type>(sk, d, 0, 0);
}

// era\t"dir:off:start:end:name:fmt";...\n — the record text master's
// dump printed, rebuilt from the record's fields; the compiled fmt
// shows as \xNN like every program field
template <::std::integral char_type, typename sink>
inline constexpr void lc_dump_era(
	sink &sk,
	::fast_io::l10n::lc_scatter<::fast_io::l10n::basic_lc_time_era<char_type>> m,
	::fast_io::l10n::lc_locale const *loc) FAST_IO_HERBCEPTIONS_THROWS
{
	namespace lc = ::fast_io::l10n;
	auto const tbl{lc::lc_get_scatter(loc, m)};
	if (tbl.base == nullptr || tbl.len == 0)
	{
		return;
	}
	lc_put_cstr<char_type>(sk, u8"era");
	sk.put(::fast_io::char_literal_v<u8'\t', char_type>);
	for (::std::size_t i{}; i < tbl.len; ++i)
	{
		auto const &e{tbl.base[i]};
		if (i != 0)
		{
			sk.put(::fast_io::char_literal_v<u8';', char_type>);
		}
		sk.put(::fast_io::char_literal_v<u8'\"', char_type>);
		sk.put(lc::lc_s32(static_cast<::std::uint_least32_t>(e.direction)) > 0
				   ? ::fast_io::char_literal_v<u8'+', char_type>
				   : ::fast_io::char_literal_v<u8'-', char_type>);
		sk.put(::fast_io::char_literal_v<u8':', char_type>);
		lc_put_num<char_type>(
			sk, lc::lc_s32(static_cast<::std::uint_least32_t>(e.offset)), 0, 0);
		sk.put(::fast_io::char_literal_v<u8':', char_type>);
		lc_dump_era_date<char_type>(
			sk, lc::lc_s32(static_cast<::std::uint_least32_t>(e.start_year)),
			static_cast<::std::int_least32_t>(lc::lc_u32(e.start_month)),
			static_cast<::std::int_least32_t>(lc::lc_u32(e.start_day)));
		sk.put(::fast_io::char_literal_v<u8':', char_type>);
		lc_dump_era_date<char_type>(
			sk, lc::lc_s32(static_cast<::std::uint_least32_t>(e.end_year)),
			static_cast<::std::int_least32_t>(lc::lc_u32(e.end_month)),
			static_cast<::std::int_least32_t>(lc::lc_u32(e.end_day)));
		sk.put(::fast_io::char_literal_v<u8':', char_type>);
		lc_put_scatter<char_type>(sk, lc::lc_get_scatter(loc, e.name));
		sk.put(::fast_io::char_literal_v<u8':', char_type>);
		lc_dump_prog_bytes<char_type>(
			sk, lc::lc_get_scatter(loc, e.era_format));
		sk.put(::fast_io::char_literal_v<u8'\"', char_type>);
	}
	sk.put(::fast_io::char_literal_v<u8'\n', char_type>);
}

// ---------------------------------------------------------------------------
// category emitters — file member order, master's section text
// ---------------------------------------------------------------------------

template <::std::integral char_type, typename sink>
inline constexpr void
lc_dump_identification(sink &sk, ::fast_io::l10n::lc_locale const *loc,
					   ::fast_io::l10n::basic_lc_all<char_type> const *all)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto const &s{all->identification};
	lc_put_cstr<char_type>(sk, u8"LC_IDENTIFICATION\n");
	lc_dump_str<char_type>(sk, u8"name", s.name, loc);
	lc_dump_str<char_type>(sk, u8"encoding", s.encoding, loc);
	lc_dump_str<char_type>(sk, u8"title", s.title, loc);
	lc_dump_str<char_type>(sk, u8"source", s.source, loc);
	lc_dump_str<char_type>(sk, u8"address", s.address, loc);
	lc_dump_str<char_type>(sk, u8"contact", s.contact, loc);
	lc_dump_str<char_type>(sk, u8"email", s.email, loc);
	lc_dump_str<char_type>(sk, u8"tel", s.tel, loc);
	lc_dump_str<char_type>(sk, u8"fax", s.fax, loc);
	lc_dump_str<char_type>(sk, u8"language", s.language, loc);
	lc_dump_str<char_type>(sk, u8"territory", s.territory, loc);
	lc_dump_str<char_type>(sk, u8"audience", s.audience, loc);
	lc_dump_str<char_type>(sk, u8"application", s.application, loc);
	lc_dump_str<char_type>(sk, u8"abbreviation", s.abbreviation, loc);
	lc_dump_str<char_type>(sk, u8"revision", s.revision, loc);
	lc_dump_str<char_type>(sk, u8"date", s.date, loc);
	lc_put_cstr<char_type>(sk, u8"END LC_IDENTIFICATION");
}

template <::std::integral char_type, typename sink>
inline constexpr void
lc_dump_monetary(sink &sk, ::fast_io::l10n::lc_locale const *loc,
				 ::fast_io::l10n::basic_lc_all<char_type> const *all)
	FAST_IO_HERBCEPTIONS_THROWS
{
	namespace lc = ::fast_io::l10n;
	auto const &s{all->monetary};
	lc_put_cstr<char_type>(sk, u8"LC_MONETARY\n");
	lc_dump_str<char_type>(sk, u8"int_curr_symbol", s.int_curr_symbol, loc);
	lc_dump_str<char_type>(sk, u8"currency_symbol", s.currency_symbol, loc);
	lc_dump_str<char_type>(sk, u8"mon_decimal_point", s.mon_decimal_point, loc);
	lc_dump_str<char_type>(sk, u8"mon_thousands_sep", s.mon_thousands_sep, loc);
	lc_dump_bytes<char_type>(sk, u8"mon_grouping", s.mon_grouping, loc);
	lc_dump_str<char_type>(sk, u8"positive_sign", s.positive_sign, loc);
	lc_dump_str<char_type>(sk, u8"negative_sign", s.negative_sign, loc);
	lc_dump_int<char_type>(sk, u8"int_frac_digits", lc::lc_s32(static_cast<::std::uint_least32_t>(s.int_frac_digits)));
	lc_dump_int<char_type>(sk, u8"frac_digits", lc::lc_s32(static_cast<::std::uint_least32_t>(s.frac_digits)));
	lc_dump_int<char_type>(sk, u8"p_cs_precedes", lc::lc_s32(static_cast<::std::uint_least32_t>(s.p_cs_precedes)));
	lc_dump_int<char_type>(sk, u8"p_sep_by_space", lc::lc_s32(static_cast<::std::uint_least32_t>(s.p_sep_by_space)));
	lc_dump_int<char_type>(sk, u8"n_cs_precedes", lc::lc_s32(static_cast<::std::uint_least32_t>(s.n_cs_precedes)));
	lc_dump_int<char_type>(sk, u8"n_sep_by_space", lc::lc_s32(static_cast<::std::uint_least32_t>(s.n_sep_by_space)));
	lc_dump_int<char_type>(sk, u8"int_p_cs_precedes", lc::lc_s32(static_cast<::std::uint_least32_t>(s.int_p_cs_precedes)));
	lc_dump_int<char_type>(sk, u8"int_p_sep_by_space", lc::lc_s32(static_cast<::std::uint_least32_t>(s.int_p_sep_by_space)));
	lc_dump_int<char_type>(sk, u8"int_n_cs_precedes", lc::lc_s32(static_cast<::std::uint_least32_t>(s.int_n_cs_precedes)));
	lc_dump_int<char_type>(sk, u8"int_n_sep_by_space", lc::lc_s32(static_cast<::std::uint_least32_t>(s.int_n_sep_by_space)));
	lc_dump_int<char_type>(sk, u8"p_sign_posn", lc::lc_s32(static_cast<::std::uint_least32_t>(s.p_sign_posn)));
	lc_dump_int<char_type>(sk, u8"n_sign_posn", lc::lc_s32(static_cast<::std::uint_least32_t>(s.n_sign_posn)));
	lc_dump_int<char_type>(sk, u8"int_p_sign_posn", lc::lc_s32(static_cast<::std::uint_least32_t>(s.int_p_sign_posn)));
	lc_dump_int<char_type>(sk, u8"int_n_sign_posn", lc::lc_s32(static_cast<::std::uint_least32_t>(s.int_n_sign_posn)));
	lc_put_cstr<char_type>(sk, u8"END LC_MONETARY");
}

template <::std::integral char_type, typename sink>
inline constexpr void
lc_dump_numeric(sink &sk, ::fast_io::l10n::lc_locale const *loc,
				::fast_io::l10n::basic_lc_all<char_type> const *all)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto const &s{all->numeric};
	lc_put_cstr<char_type>(sk, u8"LC_NUMERIC\n");
	lc_dump_str<char_type>(sk, u8"decimal_point", s.decimal_point, loc);
	lc_dump_str<char_type>(sk, u8"thousands_sep", s.thousands_sep, loc);
	lc_dump_bytes<char_type>(sk, u8"grouping", s.grouping, loc);
	lc_put_cstr<char_type>(sk, u8"END LC_NUMERIC");
}

template <::std::integral char_type, typename sink>
inline constexpr void
lc_dump_time(sink &sk, ::fast_io::l10n::lc_locale const *loc,
			 ::fast_io::l10n::basic_lc_all<char_type> const *all)
	FAST_IO_HERBCEPTIONS_THROWS
{
	namespace lc = ::fast_io::l10n;
	auto const &s{all->time};
	lc_put_cstr<char_type>(sk, u8"LC_TIME\n");
	lc_dump_strs<char_type>(sk, u8"abday", s.abday, loc);
	lc_dump_strs<char_type>(sk, u8"day", s.day, loc);
	lc_dump_strs<char_type>(sk, u8"abmon", s.abmon, loc);
	lc_dump_strs<char_type>(sk, u8"ab_alt_mon", s.ab_alt_mon, loc);
	lc_dump_strs<char_type>(sk, u8"mon", s.mon, loc);
	lc_dump_strs<char_type>(sk, u8"alt_mon", s.alt_mon, loc);
	lc_dump_prog<char_type>(sk, u8"d_t_fmt", s.d_t_fmt, loc);
	lc_dump_prog<char_type>(sk, u8"d_fmt", s.d_fmt, loc);
	lc_dump_prog<char_type>(sk, u8"t_fmt", s.t_fmt, loc);
	lc_dump_prog<char_type>(sk, u8"t_fmt_ampm", s.t_fmt_ampm, loc);
	lc_dump_prog<char_type>(sk, u8"date_fmt", s.date_fmt, loc);
	lc_dump_strs<char_type>(sk, u8"am_pm", s.am_pm, loc);
	lc_dump_era<char_type>(sk, s.era, loc);
	lc_dump_prog<char_type>(sk, u8"era_d_fmt", s.era_d_fmt, loc);
	lc_dump_prog<char_type>(sk, u8"era_d_t_fmt", s.era_d_t_fmt, loc);
	lc_dump_prog<char_type>(sk, u8"era_t_fmt", s.era_t_fmt, loc);
	lc_dump_list<char_type>(sk, u8"alt_digits", s.alt_digits, loc);
	lc_put_cstr<char_type>(sk, u8"week");
	sk.put(::fast_io::char_literal_v<u8'\t', char_type>);
	lc_put_num<char_type>(sk, lc::lc_s32(static_cast<::std::uint_least32_t>(s.week.ndays)), 0, 0);
	sk.put(::fast_io::char_literal_v<u8';', char_type>);
	lc_put_num<char_type>(sk, lc::lc_s32(static_cast<::std::uint_least32_t>(s.week.first_day)), 0, 0);
	sk.put(::fast_io::char_literal_v<u8';', char_type>);
	lc_put_num<char_type>(sk, lc::lc_s32(static_cast<::std::uint_least32_t>(s.week.first_week)), 0, 0);
	sk.put(::fast_io::char_literal_v<u8'\n', char_type>);
	lc_dump_int<char_type>(sk, u8"first_weekday", lc::lc_s32(static_cast<::std::uint_least32_t>(s.first_weekday)));
	lc_dump_int<char_type>(sk, u8"first_workday", lc::lc_s32(static_cast<::std::uint_least32_t>(s.first_workday)));
	lc_dump_int<char_type>(sk, u8"cal_direction", lc::lc_s32(static_cast<::std::uint_least32_t>(s.cal_direction)));
	lc_dump_list<char_type>(sk, u8"timezone", s.timezone, loc);
	lc_put_cstr<char_type>(sk, u8"END LC_TIME");
}

template <::std::integral char_type, typename sink>
inline constexpr void
lc_dump_messages(sink &sk, ::fast_io::l10n::lc_locale const *loc,
				 ::fast_io::l10n::basic_lc_all<char_type> const *all)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto const &s{all->messages};
	lc_put_cstr<char_type>(sk, u8"LC_MESSAGES\n");
	lc_dump_str<char_type>(sk, u8"yesexpr", s.yesexpr, loc);
	lc_dump_str<char_type>(sk, u8"noexpr", s.noexpr, loc);
	lc_dump_str<char_type>(sk, u8"yesstr", s.yesstr, loc);
	lc_dump_str<char_type>(sk, u8"nostr", s.nostr, loc);
	lc_put_cstr<char_type>(sk, u8"END LC_MESSAGES");
}

template <::std::integral char_type, typename sink>
inline constexpr void
lc_dump_paper(sink &sk, ::fast_io::l10n::lc_locale const *,
			  ::fast_io::l10n::basic_lc_all<char_type> const *all)
	FAST_IO_HERBCEPTIONS_THROWS
{
	namespace lc = ::fast_io::l10n;
	lc_put_cstr<char_type>(sk, u8"LC_PAPER\n");
	lc_dump_int<char_type>(sk, u8"width", lc::lc_s32(static_cast<::std::uint_least32_t>(all->paper.width)));
	lc_dump_int<char_type>(sk, u8"height", lc::lc_s32(static_cast<::std::uint_least32_t>(all->paper.height)));
	lc_put_cstr<char_type>(sk, u8"END LC_PAPER");
}

template <::std::integral char_type, typename sink>
inline constexpr void
lc_dump_telephone(sink &sk, ::fast_io::l10n::lc_locale const *loc,
				  ::fast_io::l10n::basic_lc_all<char_type> const *all)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto const &s{all->telephone};
	lc_put_cstr<char_type>(sk, u8"LC_TELEPHONE\n");
	lc_dump_prog<char_type>(sk, u8"tel_int_fmt", s.tel_int_fmt, loc);
	lc_dump_prog<char_type>(sk, u8"tel_dom_fmt", s.tel_dom_fmt, loc);
	lc_dump_str<char_type>(sk, u8"int_select", s.int_select, loc);
	lc_dump_str<char_type>(sk, u8"int_prefix", s.int_prefix, loc);
	lc_put_cstr<char_type>(sk, u8"END LC_TELEPHONE");
}

template <::std::integral char_type, typename sink>
inline constexpr void
lc_dump_name(sink &sk, ::fast_io::l10n::lc_locale const *loc,
			 ::fast_io::l10n::basic_lc_all<char_type> const *all)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto const &s{all->name};
	lc_put_cstr<char_type>(sk, u8"LC_NAME\n");
	lc_dump_prog<char_type>(sk, u8"name_fmt", s.name_fmt, loc);
	lc_dump_str<char_type>(sk, u8"name_gen", s.name_gen, loc);
	lc_dump_str<char_type>(sk, u8"name_miss", s.name_miss, loc);
	lc_dump_str<char_type>(sk, u8"name_mr", s.name_mr, loc);
	lc_dump_str<char_type>(sk, u8"name_mrs", s.name_mrs, loc);
	lc_dump_str<char_type>(sk, u8"name_ms", s.name_ms, loc);
	lc_put_cstr<char_type>(sk, u8"END LC_NAME");
}

template <::std::integral char_type, typename sink>
inline constexpr void
lc_dump_address(sink &sk, ::fast_io::l10n::lc_locale const *loc,
				::fast_io::l10n::basic_lc_all<char_type> const *all)
	FAST_IO_HERBCEPTIONS_THROWS
{
	namespace lc = ::fast_io::l10n;
	auto const &s{all->address};
	lc_put_cstr<char_type>(sk, u8"LC_ADDRESS\n");
	lc_dump_prog<char_type>(sk, u8"postal_fmt", s.postal_fmt, loc);
	lc_dump_str<char_type>(sk, u8"country_name", s.country_name, loc);
	lc_dump_str<char_type>(sk, u8"country_post", s.country_post, loc);
	lc_dump_str<char_type>(sk, u8"country_ab2", s.country_ab2, loc);
	lc_dump_str<char_type>(sk, u8"country_ab3", s.country_ab3, loc);
	lc_dump_int<char_type>(sk, u8"country_num", lc::lc_s32(static_cast<::std::uint_least32_t>(s.country_num)));
	lc_dump_str<char_type>(sk, u8"country_car", s.country_car, loc);
	lc_dump_str<char_type>(sk, u8"country_isbn", s.country_isbn, loc);
	lc_dump_str<char_type>(sk, u8"lang_name", s.lang_name, loc);
	lc_dump_str<char_type>(sk, u8"lang_ab", s.lang_ab, loc);
	lc_dump_str<char_type>(sk, u8"lang_term", s.lang_term, loc);
	lc_dump_str<char_type>(sk, u8"lang_lib", s.lang_lib, loc);
	lc_put_cstr<char_type>(sk, u8"END LC_ADDRESS");
}

template <::std::integral char_type, typename sink>
inline constexpr void
lc_dump_measurement(sink &sk, ::fast_io::l10n::lc_locale const *,
					::fast_io::l10n::basic_lc_all<char_type> const *all)
	FAST_IO_HERBCEPTIONS_THROWS
{
	lc_put_cstr<char_type>(sk, u8"LC_MEASUREMENT\n");
	lc_dump_int<char_type>(sk, u8"measurement",
						   ::fast_io::l10n::lc_s32(static_cast<::std::uint_least32_t>(
							   all->measurement.measurement)));
	lc_put_cstr<char_type>(sk, u8"END LC_MEASUREMENT");
}

template <::std::integral char_type, typename sink>
inline constexpr void
lc_dump_keyboard(sink &sk, ::fast_io::l10n::lc_locale const *loc,
				 ::fast_io::l10n::basic_lc_all<char_type> const *all)
	FAST_IO_HERBCEPTIONS_THROWS
{
	lc_put_cstr<char_type>(sk, u8"LC_KEYBOARD\nkeyboards\t");
	lc_dump_list_body<char_type>(sk, all->keyboard.keyboards, loc);
	sk.put(::fast_io::char_literal_v<u8'\n', char_type>);
	lc_put_cstr<char_type>(sk, u8"END LC_KEYBOARD");
}

// the whole section, blank line between categories — struct/file
// order: identification, monetary, numeric, time, messages, paper,
// telephone, name, address, measurement, keyboard
template <::std::integral char_type, typename sink>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void
lc_dump_lc_all(sink &sk, ::fast_io::l10n::lc_locale const *loc,
			   ::fast_io::l10n::basic_lc_all<char_type> const *all)
	FAST_IO_HERBCEPTIONS_THROWS
{
	lc_dump_identification<char_type>(sk, loc, all);
	lc_put_cstr<char_type>(sk, u8"\n\n");
	lc_dump_monetary<char_type>(sk, loc, all);
	lc_put_cstr<char_type>(sk, u8"\n\n");
	lc_dump_numeric<char_type>(sk, loc, all);
	lc_put_cstr<char_type>(sk, u8"\n\n");
	lc_dump_time<char_type>(sk, loc, all);
	lc_put_cstr<char_type>(sk, u8"\n\n");
	lc_dump_messages<char_type>(sk, loc, all);
	lc_put_cstr<char_type>(sk, u8"\n\n");
	lc_dump_paper<char_type>(sk, loc, all);
	lc_put_cstr<char_type>(sk, u8"\n\n");
	lc_dump_telephone<char_type>(sk, loc, all);
	lc_put_cstr<char_type>(sk, u8"\n\n");
	lc_dump_name<char_type>(sk, loc, all);
	lc_put_cstr<char_type>(sk, u8"\n\n");
	lc_dump_address<char_type>(sk, loc, all);
	lc_put_cstr<char_type>(sk, u8"\n\n");
	lc_dump_measurement<char_type>(sk, loc, all);
	lc_put_cstr<char_type>(sk, u8"\n\n");
	lc_dump_keyboard<char_type>(sk, loc, all);
}

// the shared count/write bodies every hook shape forwards to
template <::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr ::std::size_t
lc_dump_size(::fast_io::l10n::lc_locale const *loc)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (loc == nullptr)
	{
		return 0;
	}
	auto const *all{::fast_io::l10n::lc_get_all<char_type>(loc)};
	if (all == nullptr)
	{
		return 0;
	}
	lc_count_sink<char_type> sk{};
	lc_dump_lc_all<char_type>(sk, loc, all);
	return sk.n;
}

template <::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr char_type *
lc_dump_define(::fast_io::l10n::lc_locale const *loc, char_type *iter)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (loc == nullptr)
	{
		return iter;
	}
	auto const *all{::fast_io::l10n::lc_get_all<char_type>(loc)};
	if (all == nullptr)
	{
		return iter;
	}
	lc_write_sink<char_type> sk{iter, iter + lc_dump_size<char_type>(loc)};
	lc_dump_lc_all<char_type>(sk, loc, all);
	return sk.it;
}

} // namespace details

namespace manipulators
{

// the whole-locale dump, as a value — the raw lc_locale pointer stays
// non-printable; wrapping it asks for the dump explicitly, like
// master's print_define on basic_lc_all did for `loc`
struct lc_dump_t
{
	using manip_tag = manip_tag_t;
	::fast_io::l10n::lc_locale const *reference;
};

inline constexpr lc_dump_t
lc_dump(::fast_io::l10n::lc_locale const *loc) noexcept
{
	return {loc};
}

} // namespace manipulators

// ---------------------------------------------------------------------------
// hooks — the dump is dynamic_reserve_printable on the manipulator:
// the count pass is the exact size, the write pass fills the reserved
// span. The section comes from the wrapped locale's own file, so a
// dumped locale need not be the imbued one.
//
// The plain pair makes the manipulator printable on any stream (and
// lets println(imbue(loc, out), mnp::lc_dump(loc)) pass the
// print_freestanding_okay stream check); the lc_ctx pair makes it an
// lc arg inside an imbued stream so it goes through the obuffer
// reserve path like the other lc hooks.
// ---------------------------------------------------------------------------

template <::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr ::std::size_t
print_reserve_size(
	::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::lc_dump_t>,
	::fast_io::manipulators::lc_dump_t t) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::lc_dump_size<char_type>(t.reference);
}

template <::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr char_type *
print_reserve_define(
	::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::lc_dump_t>,
	char_type *iter, ::fast_io::manipulators::lc_dump_t t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::lc_dump_define<char_type>(t.reference, iter);
}

template <::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr ::std::size_t
print_reserve_size(lc_ctx<char_type> const *,
				   ::fast_io::manipulators::lc_dump_t t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::lc_dump_size<char_type>(t.reference);
}

template <::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr char_type *
print_reserve_define(lc_ctx<char_type> const *, char_type *iter,
					 ::fast_io::manipulators::lc_dump_t t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::lc_dump_define<char_type>(t.reference, iter);
}

} // namespace fast_io
