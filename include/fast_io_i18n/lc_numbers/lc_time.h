#pragma once


// lc_time — run a compiled binfmt time program (LC_TIME *_fmt fields)
// against a timestamp. The program is the binary representation of a
// strftime-format source, compiled at emit time; it is executed here —
// never parsed as text. A malformed program throws herbceptions-style,
// like everything else.
//
// Node framing (fast_io_tools/binfmt/binfmt.h):
//   [uleb128 tag][payload]   tag = (code << 3) | payload_kind
//   kind 0 none | 1 uleb | 2 bytes | 3 list | 4 sleb
//
// Emit goes through a sink so the same interpreter serves
// print_reserve_size (count) and print_reserve_define (write).

namespace fast_io
{

namespace details
{

// ---------------------------------------------------------------------------
// calendar fields the interpreter needs — extracted once per value.
// ---------------------------------------------------------------------------

template <::std::integral year_type>
inline constexpr bool is_leap_year(year_type year) noexcept
{
	if constexpr (::fast_io::details::my_signed_integral<year_type>)
	{
		return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
	}
	else
	{
		return year % 4u == 0 && (year % 100u != 0 || year % 400u == 0);
	}
}

inline constexpr ::std::uint_least16_t month_accum[]{0, 31, 59, 90, 120, 151,
													 181, 212, 243, 273, 304, 334};

struct lc_tm
{
	::std::int_least64_t year{}; // full year (2025)
	::std::int_least64_t unix_seconds{};
	::std::int_least64_t iso_year{}; // ISO week-numbering year (%G)
	::std::uint_least32_t month{1};  // 1-12
	::std::uint_least32_t day{1};    // 1-31
	::std::uint_least32_t hour{};
	::std::uint_least32_t hour12{}; // 1-12
	::std::uint_least32_t minute{};
	::std::uint_least32_t second{};
	::std::uint_least32_t wday{};     // 0=Sunday
	::std::uint_least32_t yday{};     // 0-365 (day of year - 1)
	::std::uint_least32_t uday{7};    // %u — Mon=1..Sun=7
	::std::uint_least32_t week_sun{}; // %U
	::std::uint_least32_t week_mon{}; // %W
	::std::uint_least32_t iso_week{}; // %V
	::std::uint_least32_t nanosecond{};
	::std::int_least32_t tzoff_sec{}; // seconds east of UTC
	::std::uint_least8_t ampm{};
};

// ISO week-numbering (C99 %V/%G): weeks start Monday; week 1 holds the
// year's first Thursday. week = (yday - days_since_monday + 10) / 7
inline constexpr ::std::int_least64_t lc_iso_week_of(::std::int_least64_t yday,
													 ::std::int_least64_t wday) noexcept
{
	return (yday - ((wday + 6) % 7) + 10) / 7;
}

// a year has 53 ISO weeks iff Jan 1 is Thursday (wday 4), or a leap
// year starting Wednesday (wday 3)
inline constexpr ::std::int_least64_t lc_iso_max_week(::std::int_least64_t year,
													  ::std::int_least64_t jan1_wday) noexcept
{
	return (jan1_wday == 4 || (is_leap_year(year) && jan1_wday == 3)) ? 53 : 52;
}

inline constexpr void lc_iso_week(lc_tm &m) noexcept
{
	auto const y{static_cast<::std::int_least64_t>(m.yday)};
	auto const w{static_cast<::std::int_least64_t>(m.wday)};
	// Jan 1's weekday, 0=Sun
	auto const jan1w{(w - y % 7 + 7) % 7};
	::std::int_least64_t week{lc_iso_week_of(y, w)};
	::std::int_least64_t yr{m.year};
	if (week < 1)
	{
		// belongs to the previous ISO year — its last week
		--yr;
		week = lc_iso_max_week(yr, (jan1w - (is_leap_year(yr) ? 2 : 1) + 7) % 7);
	}
	else if (week > lc_iso_max_week(yr, jan1w))
	{
		++yr;
		week = 1;
	}
	m.iso_week = static_cast<::std::uint_least32_t>(week);
	m.iso_year = yr;
}

inline constexpr lc_tm lc_tm_from(::fast_io::iso8601_timestamp const &t) noexcept
{
	lc_tm m{};
	m.year = t.year;
	m.month = t.month;
	m.day = t.day;
	m.hour = t.hours;
	m.minute = t.minutes;
	m.second = t.seconds;
	m.nanosecond = t.nanoseconds;
	m.tzoff_sec = t.timezone;
	m.hour12 = (t.hours + 11) % 12 + 1;
	m.ampm = t.hours >= 12;
	m.wday = ::fast_io::weekday(t.year, t.month, t.day);
	::std::uint_least32_t yd{
		static_cast<::std::uint_least32_t>(month_accum[t.month - 1] + t.day - 1)};
	if (t.month > 2 && is_leap_year(t.year))
	{
		++yd;
	}
	m.yday = yd;
	m.uday = m.wday == 0 ? 7 : m.wday;
	m.week_sun = (yd + 7 - m.wday) / 7;
	m.week_mon = (yd + 7 - ((m.wday + 6) % 7)) / 7;
	m.unix_seconds = ::fast_io::details::iso8601_to_unix_timestamp_impl(t).tv_sec;
	lc_iso_week(m);
	return m;
}

inline constexpr lc_tm lc_tm_from(::fast_io::posix_timespec64 t) noexcept
{
	auto const u{::fast_io::utc(t)};
	lc_tm m{lc_tm_from(u)};
	m.unix_seconds = t.tv_sec;
	m.tzoff_sec = 0;
	return m;
}

// std::chrono — anything exposing time_since_epoch() (sys_time,
// sys_seconds, sys_days, time_point...) becomes unix seconds
template <typename T>
inline constexpr lc_tm lc_tm_from(T const &v) noexcept
{
	lc_tm m{};
	if constexpr (requires { v.time_since_epoch().count(); })
	{
		using dur = decltype(v.time_since_epoch());
		::std::int_least64_t const secs{static_cast<::std::int_least64_t>(
			v.time_since_epoch().count() * dur::period::num / dur::period::den)};
		m = lc_tm_from(::fast_io::posix_timespec64{secs, 0});
	}
	else if constexpr (requires { v.count(); })
	{
		m = lc_tm_from(::fast_io::posix_timespec64{
			static_cast<::std::int_least64_t>(v.count()), 0});
	}
	else
	{
		static_assert(!sizeof(T *), "unsupported time type for locale formatting");
	}
	return m;
}

// ---------------------------------------------------------------------------
// leb reader over program bytes — malformed input throws
// ---------------------------------------------------------------------------

struct lc_prog_reader
{
	char8_t const *cur{}, *end{};
	inline constexpr bool is_empty() const noexcept
	{
		return cur >= end;
	}
	inline constexpr ::std::uint_least64_t get_uleb() FAST_IO_HERBCEPTIONS_THROWS
	{
		::std::uint_least64_t r{};
		unsigned sh{};
		for (auto p{cur}; p != end; ++p)
		{
			auto const b{static_cast<::std::uint_least8_t>(*p)};
			r |= static_cast<::std::uint_least64_t>(b & 0x7F) << sh;
			if (!(b & 0x80))
			{
				cur = p + 1;
				return r;
			}
			sh += 7;
			if (sh > 63)
			{
				break;
			}
		}
		::fast_io::herbceptions::throws_errc(::std::errc::invalid_argument);
	}
	inline constexpr ::fast_io::u8string_view get_bytes(::std::uint_least64_t n)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		if (n > static_cast<::std::uint_least64_t>(end - cur))
		{
			::fast_io::herbceptions::throws_errc(::std::errc::invalid_argument);
		}
		::fast_io::u8string_view r{cur, static_cast<::std::size_t>(n)};
		cur += n;
		return r;
	}
};

// ---------------------------------------------------------------------------
// emit sinks — the interpreter writes through these so count and write
// passes share the logic
// ---------------------------------------------------------------------------

// padded decimal — pad: 1 zero, 2 space, 3 none
template <::std::integral char_type, typename sink>
inline constexpr void lc_put_num(sink &sk, ::std::int_least64_t v,
								 ::std::uint_least32_t width,
								 ::std::uint_least32_t pad) noexcept
{
	char_type tmp[32];
	char_type *te{tmp};
	::std::uint_least64_t u;
	bool const neg{v < 0};
	if (neg)
	{
		constexpr ::std::uint_least64_t zero{};
		u = zero - static_cast<::std::uint_least64_t>(v);
	}
	else
	{
		u = static_cast<::std::uint_least64_t>(v);
	}
	do
	{
		*te++ = ::fast_io::char_literal_add<char_type>(u % 10u);
		u /= 10u;
	} while (u != 0);
	if (neg)
	{
		*te++ = ::fast_io::char_literal_v<u8'-', char_type>;
	}
	auto const nd{static_cast<::std::uint_least32_t>(te - tmp)};
	if (pad != 3 && pad != 0 && width > nd)
	{
		char_type const fill{pad == 2
								 ? ::fast_io::char_literal_v<u8' ', char_type>
								 : ::fast_io::char_literal_v<u8'0', char_type>};
		for (::std::uint_least32_t k{nd}; k < width; ++k)
		{
			sk.put(fill);
		}
	}
	while (te != tmp)
	{
		sk.put(*--te);
	}
}

template <::std::integral char_type, typename sink>
inline constexpr void lc_put_scatter(sink &sk,
									 ::fast_io::basic_io_scatter_t<char_type> s) noexcept
{
	if (s.base != nullptr)
	{
		sk.put_units(s.base, s.len);
	}
}

// case-fold for emitted names (casef: 1 '^' upper, 2 '#' swap) —
// through fast_io's C-category helpers so the classification follows
// the unit's charset
template <::std::integral char_type>
inline constexpr char_type lc_case(char_type ch, ::std::uint_least32_t mode) noexcept
{
	if (mode == 1)
	{
		return ::fast_io::char_category::to_c_upper(ch);
	}
	if (mode == 2)
	{
		return ::fast_io::char_category::is_c_lower(ch)
				   ? ::fast_io::char_category::to_c_upper(ch)
				   : ::fast_io::char_category::to_c_lower(ch);
	}
	return ch;
}

// ---------------------------------------------------------------------------
// era lookup — find the era covering the civil date
// ---------------------------------------------------------------------------

template <::std::integral char_type>
inline constexpr ::fast_io::l10n::basic_lc_time_era<char> const *
lc_era_lookup(lc_ctx<char_type> const *ctx, ::std::int_least64_t y,
			  ::std::uint_least32_t mo, ::std::uint_least32_t dy)
	FAST_IO_HERBCEPTIONS_THROWS
{
	namespace lc = ::fast_io::l10n;
	auto const &eras{ctx->all->time.era};
	auto const et{lc::lc_get_scatter(ctx->loc, eras)};
	auto const *tbl{et.base};
	auto const n{static_cast<::std::size_t>(et.len)};
	if (tbl == nullptr)
	{
		return nullptr;
	}
	auto cmp{[](::std::int_least64_t ey, ::std::uint_least32_t em,
				::std::uint_least32_t ed, ::std::int_least64_t y,
				::std::uint_least32_t mo, ::std::uint_least32_t dy) noexcept {
		if (ey != y)
		{
			return ey < y ? -1 : 1;
		}
		if (em != mo)
		{
			return static_cast<int>(em) < static_cast<int>(mo) ? -1 : 1;
		}
		if (ed != dy)
		{
			return static_cast<int>(ed) < static_cast<int>(dy) ? -1 : 1;
		}
		return 0;
	}};
	for (::std::uint_least32_t i{}; i < n; ++i)
	{
		auto const &e{tbl[i]};
		auto const dir{lc::lc_s32(e.direction)};
		auto const ey{lc::lc_s32(e.end_year)};
		bool in{};
		if (dir > 0)
		{
			if (cmp(lc::lc_s32(e.start_year), lc::lc_u32(e.start_month),
					lc::lc_u32(e.start_day), y, mo, dy) <= 0 &&
				(ey == INT32_MAX ||
				 cmp(y, mo, dy, ey, lc::lc_u32(e.end_month),
					 lc::lc_u32(e.end_day)) <= 0))
			{
				in = true;
			}
		}
		else
		{
			if (cmp(y, mo, dy, lc::lc_s32(e.start_year),
					lc::lc_u32(e.start_month), lc::lc_u32(e.start_day)) <= 0 &&
				(ey == INT32_MIN ||
				 cmp(ey, lc::lc_u32(e.end_month), lc::lc_u32(e.end_day), y, mo,
					 dy) <= 0))
			{
				in = true;
			}
		}
		if (in)
		{
			return __builtin_addressof(e);
		}
	}
	return nullptr;
}

// ---------------------------------------------------------------------------
// the interpreter — flat node stream through the sink, recursion
// depth-capped like the compiler
// ---------------------------------------------------------------------------

namespace lc_conv
{
inline constexpr ::std::uint_least32_t a{1}, A{2}, b{3}, B{4}, c{5}, C{6}, d{7},
	e{8}, F{9}, G{10}, H{11}, I{12}, j{13}, k{14}, l{15}, m{16}, M{17}, p{18},
	P{19}, r{20}, R{21}, s{22}, S{23}, T{24}, u{25}, U{26}, V{27}, w{28}, W{29},
	x{30}, X{31}, Y{32}, z{33}, Z{34}, plus{35}, iso8601{36}, iso8601_utc{37},
	fracsec{38};
} // namespace lc_conv

template <::std::integral char_type, typename sink>
inline constexpr void lc_prog_run(char8_t const *prog, ::std::size_t plen,
								  lc_ctx<char_type> const *ctx, lc_tm const &tm,
								  sink &sk, ::std::uint_least32_t depth,
								  bool in_era)
	FAST_IO_HERBCEPTIONS_THROWS;

// run a program member (empty member = nothing) — the container is
// verified at load (lc_verify), so the member range is trusted; the
// program reader still bounds-checks its own LEB stream. in_era marks
// execution inside an era fmt program: %Ey is the era's year number
// there, not another fmt recursion
template <::std::integral char_type, typename sink>
inline constexpr void lc_run_member(sink &sk, lc_ctx<char_type> const *ctx,
									::fast_io::l10n::lc_scatter<char8_t> prog,
									lc_tm const &tm, ::std::uint_least32_t depth,
									bool in_era = false)
	FAST_IO_HERBCEPTIONS_THROWS
{
	namespace lc = ::fast_io::l10n;
	auto const b{lc::lc_get_scatter(ctx->loc, prog)};
	if (b.base == nullptr)
	{
		return;
	}
	lc_prog_run<char_type>(b.base, b.len, ctx, tm, sk, depth, in_era);
}

// a name member with optional case fold
template <::std::integral char_type, typename sink>
inline constexpr void lc_put_name(sink &sk, lc_ctx<char_type> const *ctx,
								  ::fast_io::l10n::lc_scatter<char> member,
								  ::std::uint_least32_t casef)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto const s{ctx->sc(member)};
	if (s.base == nullptr)
	{
		return;
	}
	if (casef == 0)
	{
		sk.put_units(s.base, s.len);
		return;
	}
	for (::std::size_t i{}; i < s.len; ++i)
	{
		sk.put(lc_case(s.base[i], casef));
	}
}

// pct conversion
template <::std::integral char_type, typename sink>
inline constexpr void lc_pct_conv(sink &sk, lc_ctx<char_type> const *ctx,
								  lc_tm const &tm, ::std::uint_least32_t conv,
								  ::std::uint_least32_t pad,
								  ::std::uint_least32_t casef,
								  ::std::uint_least32_t modifier,
								  ::std::uint_least32_t colons,
								  ::std::uint_least32_t width,
								  ::std::uint_least32_t depth,
								  bool in_era)
	FAST_IO_HERBCEPTIONS_THROWS
{
	namespace lc = ::fast_io::l10n;
	auto const &t{ctx->all->time};
	// pad param bits: bit0 '-'(no pad) bit1 '_'(space) bit2 '0'(zero)
	::std::uint_least32_t const pad_kind{
		pad == 0    ? 1u // default zero
		: (pad & 1) ? 3u
		: (pad & 2) ? 2u
					: 1u};
	// numeric emit honoring the O modifier's alt_digits
	auto num{[&](::std::int_least64_t v, ::std::uint_least32_t w,
				 ::std::uint_least32_t pk) FAST_IO_HERBCEPTIONS_THROWS {
		if (modifier == 2 && lc::lc_u32(t.alt_digits.ref.off) != 0 &&
			v >= 0)
		{
			lc_put_scatter<char_type>(
				sk, ctx->sc_elem(t.alt_digits,
								 static_cast<::std::uint_least32_t>(v)));
			return;
		}
		lc_put_num<char_type>(sk, v, w, pk);
	}};
	bool const era{modifier == 1};
	switch (conv)
	{
	case lc_conv::a:
		lc_put_name<char_type>(sk, ctx, t.abday[tm.wday], casef);
		break;
	case lc_conv::A:
		lc_put_name<char_type>(sk, ctx, t.day[tm.wday], casef);
		break;
	case lc_conv::b:
		if (modifier == 2 && lc::lc_u32(t.ab_alt_mon[0].ref.off) != 0)
		{
			lc_put_name<char_type>(sk, ctx, t.ab_alt_mon[tm.month - 1], casef);
		}
		else
		{
			lc_put_name<char_type>(sk, ctx, t.abmon[tm.month - 1], casef);
		}
		break;
	case lc_conv::B:
		if (modifier == 2 && lc::lc_u32(t.alt_mon[0].ref.off) != 0)
		{
			lc_put_name<char_type>(sk, ctx, t.alt_mon[tm.month - 1], casef);
		}
		else
		{
			lc_put_name<char_type>(sk, ctx, t.mon[tm.month - 1], casef);
		}
		break;
	case lc_conv::c:
		lc_run_member<char_type>(sk, ctx,
								 era ? t.era_d_t_fmt : t.d_t_fmt, tm, depth + 1);
		break;
	case lc_conv::C:
		if (era)
		{
			if (auto const *e{lc_era_lookup(ctx, tm.year, tm.month, tm.day)};
				e != nullptr)
			{
				lc_put_name<char_type>(sk, ctx, e->name, casef);
			}
		}
		else
		{
			num(tm.year / 100, 2, pad_kind);
		}
		break;
	case lc_conv::d:
		num(tm.day, width ? width : 2, pad_kind);
		break;
	case lc_conv::e:
		num(tm.day, width ? width : 2, pad == 0 ? 2 : pad_kind);
		break;
	case lc_conv::F:
		lc_put_num<char_type>(sk, tm.year, 4, 1);
		sk.put(::fast_io::char_literal_v<u8'-', char_type>);
		lc_put_num<char_type>(sk, tm.month, 2, 1);
		sk.put(::fast_io::char_literal_v<u8'-', char_type>);
		lc_put_num<char_type>(sk, tm.day, 2, 1);
		break;
	case lc_conv::G:
		num(tm.iso_year, 4, pad_kind);
		break;
	case lc_conv::H:
		num(tm.hour, width ? width : 2, pad_kind);
		break;
	case lc_conv::I:
		num(tm.hour12, width ? width : 2, pad_kind);
		break;
	case lc_conv::j:
		num(tm.yday + 1, 3, pad_kind);
		break;
	case lc_conv::k:
		lc_put_num<char_type>(sk, tm.hour, 2, 2);
		break;
	case lc_conv::l:
		lc_put_num<char_type>(sk, tm.hour12, 2, 2);
		break;
	case lc_conv::m:
		num(tm.month, width ? width : 2, pad_kind);
		break;
	case lc_conv::M:
		num(tm.minute, width ? width : 2, pad_kind);
		break;
	case lc_conv::p:
		lc_put_name<char_type>(sk, ctx, t.am_pm[tm.ampm], casef);
		break;
	case lc_conv::P:
	{
		auto const s{ctx->sc(t.am_pm[tm.ampm])};
		if (s.base != nullptr)
		{
			for (::std::size_t i{}; i < s.len; ++i)
			{
				sk.put(::fast_io::char_category::to_c_lower(s.base[i]));
			}
		}
		break;
	}
	case lc_conv::r:
		lc_run_member<char_type>(sk, ctx,
								 lc::lc_u32(t.t_fmt_ampm.ref.off) != 0
									 ? t.t_fmt_ampm
									 : t.t_fmt,
								 tm, depth + 1);
		break;
	case lc_conv::R:
		lc_put_num<char_type>(sk, tm.hour, 2, 1);
		sk.put(::fast_io::char_literal_v<u8':', char_type>);
		lc_put_num<char_type>(sk, tm.minute, 2, 1);
		break;
	case lc_conv::s:
		num(tm.unix_seconds, width, pad_kind);
		break;
	case lc_conv::S:
		num(tm.second, width ? width : 2, pad_kind);
		break;
	case lc_conv::T:
		lc_put_num<char_type>(sk, tm.hour, 2, 1);
		sk.put(::fast_io::char_literal_v<u8':', char_type>);
		lc_put_num<char_type>(sk, tm.minute, 2, 1);
		sk.put(::fast_io::char_literal_v<u8':', char_type>);
		lc_put_num<char_type>(sk, tm.second, 2, 1);
		break;
	case lc_conv::u:
		num(tm.uday, 1, pad_kind);
		break;
	case lc_conv::U:
		num(tm.week_sun, 2, pad_kind);
		break;
	case lc_conv::V:
		num(tm.iso_week, 2, pad_kind);
		break;
	case lc_conv::w:
		num(tm.wday, 1, pad_kind);
		break;
	case lc_conv::W:
		num(tm.week_mon, 2, pad_kind);
		break;
	case lc_conv::x:
		lc_run_member<char_type>(sk, ctx,
								 era ? t.era_d_fmt : t.d_fmt, tm, depth + 1);
		break;
	case lc_conv::X:
		lc_run_member<char_type>(sk, ctx,
								 era ? t.era_t_fmt : t.t_fmt, tm, depth + 1);
		break;
	case lc_conv::Y:
		if (era)
		{
			if (auto const *e{lc_era_lookup(ctx, tm.year, tm.month, tm.day)};
				e != nullptr)
			{
				auto const sy{lc::lc_s32(e->start_year)};
				auto const off{lc::lc_s32(e->offset)};
				auto const dir{lc::lc_s32(e->direction)};
				if (in_era)
				{
					// inside the era's fmt program %Ey is just the
					// era year number — no fmt recursion
					num(dir > 0 ? tm.year - sy + off : sy - tm.year + off, 0, 0);
				}
				else if (lc::lc_u32(e->era_format.ref.off) != 0)
				{
					// %EY runs the era's own era_format program
					// ("%EC%Ey\u5e74" & co)
					lc_run_member<char_type>(sk, ctx, e->era_format, tm,
											 depth + 1, true);
				}
				else
				{
					lc_put_name<char_type>(sk, ctx, e->name, casef);
					num(dir > 0 ? tm.year - sy + off : sy - tm.year + off, 0, 0);
				}
			}
			else
			{
				num(tm.year, 4, pad_kind);
			}
		}
		else
		{
			num(tm.year, 4, pad_kind);
		}
		break;
	case lc_conv::z:
	{
		::std::int_least32_t const o{tm.tzoff_sec};
		if (o == 0 && colons == 0)
		{
			sk.put(::fast_io::char_literal_v<u8'U', char_type>);
			sk.put(::fast_io::char_literal_v<u8'T', char_type>);
			sk.put(::fast_io::char_literal_v<u8'C', char_type>);
			break;
		}
		auto a{o < 0 ? -o : o};
		sk.put(o < 0 ? ::fast_io::char_literal_v<u8'-', char_type>
					 : ::fast_io::char_literal_v<u8'+', char_type>);
		lc_put_num<char_type>(sk, a / 3600, 2, 1);
		if (colons != 0)
		{
			sk.put(::fast_io::char_literal_v<u8':', char_type>);
		}
		lc_put_num<char_type>(sk, a / 60 % 60, 2, 1);
		if (colons >= 2)
		{
			sk.put(::fast_io::char_literal_v<u8':', char_type>);
			lc_put_num<char_type>(sk, a % 60, 2, 1);
		}
		break;
	}
	case lc_conv::Z:
		lc_put_scatter<char_type>(sk, ctx->sc_elem(t.timezone, 0));
		break;
	case lc_conv::plus:
		lc_run_member<char_type>(sk, ctx, t.date_fmt, tm, depth + 1);
		break;
	case lc_conv::iso8601:
	case lc_conv::iso8601_utc:
	{
		lc_put_num<char_type>(sk, tm.year, 4, 1);
		sk.put(::fast_io::char_literal_v<u8'-', char_type>);
		lc_put_num<char_type>(sk, tm.month, 2, 1);
		sk.put(::fast_io::char_literal_v<u8'-', char_type>);
		lc_put_num<char_type>(sk, tm.day, 2, 1);
		sk.put(::fast_io::char_literal_v<u8'T', char_type>);
		lc_put_num<char_type>(sk, tm.hour, 2, 1);
		sk.put(::fast_io::char_literal_v<u8':', char_type>);
		lc_put_num<char_type>(sk, tm.minute, 2, 1);
		sk.put(::fast_io::char_literal_v<u8':', char_type>);
		lc_put_num<char_type>(sk, tm.second, 2, 1);
		if (tm.nanosecond != 0)
		{
			sk.put(::fast_io::char_literal_v<u8'.', char_type>);
			lc_put_num<char_type>(sk, tm.nanosecond, 9, 1);
		}
		::std::int_least32_t const o{
			conv == lc_conv::iso8601_utc ? 0 : tm.tzoff_sec};
		if (o == 0)
		{
			sk.put(::fast_io::char_literal_v<u8'Z', char_type>);
			break;
		}
		auto a{o < 0 ? -o : o};
		sk.put(o < 0 ? ::fast_io::char_literal_v<u8'-', char_type>
					 : ::fast_io::char_literal_v<u8'+', char_type>);
		lc_put_num<char_type>(sk, a / 3600, 2, 1);
		sk.put(::fast_io::char_literal_v<u8':', char_type>);
		lc_put_num<char_type>(sk, a / 60 % 60, 2, 1);
		break;
	}
	case lc_conv::fracsec:
		lc_put_num<char_type>(sk, tm.nanosecond, 0, 0);
		break;
	default:
		::fast_io::herbceptions::throws_errc(::std::errc::invalid_argument);
	}
}

template <::std::integral char_type, typename sink>
inline constexpr void lc_prog_run(char8_t const *prog, ::std::size_t plen,
								  lc_ctx<char_type> const *ctx, lc_tm const &tm,
								  sink &sk, ::std::uint_least32_t depth,
								  bool in_era)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (depth > 8)
	{
		::fast_io::herbceptions::throws_errc(::std::errc::invalid_argument);
	}
	if (prog == nullptr)
	{
		return;
	}
	lc_prog_reader r{prog, prog + plen};
	while (!r.is_empty())
	{
		auto const tag{r.get_uleb()};
		auto const code{static_cast<::std::uint_least32_t>(tag >> 3)};
		auto const kind{static_cast<::std::uint_least32_t>(tag & 7)};
		if (code == 1) // op_literal — bytes
		{
			auto const n{r.get_uleb()};
			auto const b{r.get_bytes(n)};
			sk.put_units(reinterpret_cast<char_type const *>(b.data()),
						 b.size() / sizeof(char_type));
		}
		else if (code == 3) // op_pct
		{
			::std::uint_least32_t conv{}, pad{}, casef{}, modifier{}, colons{},
				width{};
			[[maybe_unused]] ::std::uint_least32_t prec{};
			if (kind == 1)
			{
				conv = static_cast<::std::uint_least32_t>(r.get_uleb());
			}
			else
			{
				auto const nc{r.get_uleb()};
				for (::std::uint_least64_t k{}; k < nc; ++k)
				{
					auto const ctag{r.get_uleb()};
					auto const cv{r.get_uleb()};
					auto const pc{static_cast<::std::uint_least32_t>(ctag >> 3)};
					auto const u{static_cast<::std::uint_least32_t>(cv)};
					switch (pc)
					{
					case 20:
						conv = u;
						break;
					case 22:
						pad = u;
						break;
					case 23:
						casef = u;
						break;
					case 24:
						modifier = u;
						break;
					case 25:
						colons = u;
						break;
					case 10:
						width = u;
						break;
					case 11:
						prec = u;
						break;
					default:
						::fast_io::herbceptions::throws_errc(
							::std::errc::invalid_argument);
					}
				}
			}
			lc_pct_conv<char_type>(sk, ctx, tm, conv, pad, casef, modifier,
								   colons, width, depth, in_era);
		}
		else
		{
			// op_field / op_plural / unknown — not valid in LC_TIME
			// programs
			::fast_io::herbceptions::throws_errc(::std::errc::invalid_argument);
		}
	}
}

// pick the program member for a time_flag, era formats falling back to
// the plain one when empty — master's semantics
template <::fast_io::manipulators::lc_time_flag tf, ::std::integral char_type>
inline constexpr ::fast_io::l10n::lc_scatter<char8_t>
lc_time_program(lc_ctx<char_type> const *ctx) noexcept
{
	namespace lc = ::fast_io::l10n;
	namespace mp = ::fast_io::manipulators;
	auto const &t{ctx->all->time};
	lc::lc_scatter<char8_t> prog{};
	if constexpr (tf == mp::lc_time_flag::d_t_fmt)
	{
		prog = t.d_t_fmt;
	}
	else if constexpr (tf == mp::lc_time_flag::d_fmt)
	{
		prog = t.d_fmt;
	}
	else if constexpr (tf == mp::lc_time_flag::t_fmt)
	{
		prog = t.t_fmt;
	}
	else if constexpr (tf == mp::lc_time_flag::t_fmt_ampm)
	{
		prog = t.t_fmt_ampm;
	}
	else if constexpr (tf == mp::lc_time_flag::date_fmt)
	{
		prog = t.date_fmt;
	}
	else if constexpr (tf == mp::lc_time_flag::era_d_t_fmt)
	{
		prog = lc::lc_u32(t.era_d_t_fmt.ref.off) != 0 ? t.era_d_t_fmt : t.d_t_fmt;
	}
	else if constexpr (tf == mp::lc_time_flag::era_d_fmt)
	{
		prog = lc::lc_u32(t.era_d_fmt.ref.off) != 0 ? t.era_d_fmt : t.d_fmt;
	}
	else if constexpr (tf == mp::lc_time_flag::era_t_fmt)
	{
		prog = lc::lc_u32(t.era_t_fmt.ref.off) != 0 ? t.era_t_fmt : t.t_fmt;
	}
	return prog;
}

// value types the interpreter can read
template <typename T>
concept lc_time_value = ::std::same_as<::std::remove_cvref_t<T>, ::fast_io::iso8601_timestamp> ||
						::std::same_as<::std::remove_cvref_t<T>, ::fast_io::posix_timespec64>;

template <typename T>
concept lc_time_value_or_chrono = lc_time_value<T> || requires(T const &v) {
	{ lc_tm_from(v) } -> ::std::same_as<lc_tm>;
};

// flags with a live time_flag
template <typename T>
inline constexpr bool lc_time_scalar_v{false};

template <::fast_io::manipulators::scalar_flags flags, typename T>
inline constexpr bool lc_time_scalar_v<::fast_io::manipulators::scalar_manip_t<flags, T>>{
	static_cast<::std::uint_least8_t>(
		static_cast<::std::uint_least8_t>(flags.time_flag) -
		static_cast<::std::uint_least8_t>(1u)) < 8u};

template <::fast_io::manipulators::lc_time_flag flag>
inline constexpr ::fast_io::manipulators::scalar_flags base_lc_time_flags_cache{
	.time_flag = flag};

// POSIX/C-locale defaults — the unimbued path. These are the formats
// strftime produces in the C locale; used only when a time scalar is
// printed without a locale context
inline constexpr ::fast_io::u8string_view lc_c_abday[]{u8"Sun", u8"Mon", u8"Tue", u8"Wed",
													   u8"Thu", u8"Fri", u8"Sat"};
inline constexpr ::fast_io::u8string_view lc_c_day[]{u8"Sunday", u8"Monday", u8"Tuesday",
													 u8"Wednesday", u8"Thursday", u8"Friday",
													 u8"Saturday"};
inline constexpr ::fast_io::u8string_view lc_c_abmon[]{u8"Jan", u8"Feb", u8"Mar", u8"Apr",
													   u8"May", u8"Jun", u8"Jul", u8"Aug",
													   u8"Sep", u8"Oct", u8"Nov", u8"Dec"};
inline constexpr ::fast_io::u8string_view lc_c_mon[]{u8"January", u8"February", u8"March",
													 u8"April", u8"May", u8"June", u8"July",
													 u8"August", u8"September", u8"October",
													 u8"November", u8"December"};

template <::std::integral char_type, typename sink>
inline constexpr void lc_put_cstr(sink &sk, ::fast_io::u8string_view s) noexcept
{
	for (char8_t ch : s)
	{
		sk.put(::fast_io::char_literal<char_type>(ch));
	}
}

// emit the C-locale form of a time_flag — mirrors the binary
// interpreter's output for the POSIX locale
template <::std::integral char_type, typename sink>
inline constexpr void lc_default_emit(::fast_io::manipulators::lc_time_flag flag,
									  lc_tm const &tm, sink &sk) noexcept
{
	namespace mp = ::fast_io::manipulators;
	auto put{[&](char8_t ch) noexcept { sk.put(::fast_io::char_literal<char_type>(ch)); }};
	auto num{[&](::std::int_least64_t v, ::std::uint_least32_t w) noexcept {
		lc_put_num<char_type>(sk, v, w, 1);
	}};
	auto hms{[&]() noexcept {
		num(tm.hour, 2);
		put(u8':');
		num(tm.minute, 2);
		put(u8':');
		num(tm.second, 2);
	}};
	switch (flag)
	{
	case mp::lc_time_flag::d_t_fmt:
	case mp::lc_time_flag::era_d_t_fmt:
		// "%a %b %e %H:%M:%S %Y"
		lc_put_cstr<char_type>(sk, lc_c_abday[tm.wday]);
		put(u8' ');
		lc_put_cstr<char_type>(sk, lc_c_abmon[tm.month - 1]);
		put(u8' ');
		lc_put_num<char_type>(sk, tm.day, 2, 2);
		put(u8' ');
		hms();
		put(u8' ');
		num(tm.year, 4);
		break;
	case mp::lc_time_flag::d_fmt:
	case mp::lc_time_flag::era_d_fmt:
		// "%m/%d/%y"
		num(tm.month, 2);
		put(u8'/');
		num(tm.day, 2);
		put(u8'/');
		num(tm.year % 100, 2);
		break;
	case mp::lc_time_flag::t_fmt:
	case mp::lc_time_flag::era_t_fmt:
		// "%H:%M:%S"
		hms();
		break;
	case mp::lc_time_flag::t_fmt_ampm:
		// "%I:%M:%S %p"
		num(tm.hour12, 2);
		put(u8':');
		num(tm.minute, 2);
		put(u8':');
		num(tm.second, 2);
		put(u8' ');
		lc_put_cstr<char_type>(sk, tm.ampm ? u8string_view{u8"PM"} : u8string_view{u8"AM"});
		break;
	case mp::lc_time_flag::date_fmt:
		// "%a %b %e %H:%M:%S %Z %Y"
		lc_put_cstr<char_type>(sk, lc_c_abday[tm.wday]);
		put(u8' ');
		lc_put_cstr<char_type>(sk, lc_c_abmon[tm.month - 1]);
		put(u8' ');
		lc_put_num<char_type>(sk, tm.day, 2, 2);
		put(u8' ');
		hms();
		put(u8' ');
		lc_put_cstr<char_type>(sk, u8string_view{u8"UTC"});
		put(u8' ');
		num(tm.year, 4);
		break;
	default:
		break;
	}
}

} // namespace details

// ---------------------------------------------------------------------------
// locale print hooks keyed on lc_ctx — count pass and write pass share
// lc_prog_run through the sink
// ---------------------------------------------------------------------------

template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags,
		  typename T>
	requires(::fast_io::details::lc_time_scalar_v<
				 ::fast_io::manipulators::scalar_manip_t<flags, T>> &&
			 ::fast_io::details::lc_time_value_or_chrono<T>)
inline constexpr ::std::size_t
print_reserve_size(lc_ctx<char_type> const *ctx,
				   ::fast_io::manipulators::scalar_manip_t<flags, T> t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	namespace lc = ::fast_io::l10n;
	if (ctx->all == nullptr)
	{
		return 0;
	}
	auto const prog{::fast_io::details::lc_time_program<flags.time_flag>(ctx)};
	auto const b{lc::lc_get_scatter(ctx->loc, prog)};
	if (b.base == nullptr)
	{
		return 0;
	}
	::fast_io::details::lc_tm tm{::fast_io::details::lc_tm_from(t.reference)};
	::fast_io::details::lc_count_sink<char_type> sk{};
	::fast_io::details::lc_prog_run<char_type>(b.base, b.len, ctx, tm,
											   sk, 0, false);
	return sk.n;
}

template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags,
		  typename T>
	requires(::fast_io::details::lc_time_scalar_v<
				 ::fast_io::manipulators::scalar_manip_t<flags, T>> &&
			 ::fast_io::details::lc_time_value_or_chrono<T>)
inline constexpr char_type *
print_reserve_define(lc_ctx<char_type> const *ctx, char_type *iter,
					 ::fast_io::manipulators::scalar_manip_t<flags, T> t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	namespace lc = ::fast_io::l10n;
	if (ctx->all == nullptr)
	{
		return iter;
	}
	auto const prog{::fast_io::details::lc_time_program<flags.time_flag>(ctx)};
	auto const b{lc::lc_get_scatter(ctx->loc, prog)};
	if (b.base == nullptr)
	{
		return iter;
	}
	::fast_io::details::lc_tm tm{::fast_io::details::lc_tm_from(t.reference)};
	// the write pass is bounded by the count pass's size — a file
	// whose program emits more than it counted is truncated, never an
	// overflow
	::fast_io::details::lc_write_sink<char_type> sk{
		iter, iter + print_reserve_size(ctx, t)};
	::fast_io::details::lc_prog_run<char_type>(b.base, b.len, ctx, tm,
											   sk, 0, false);
	return sk.it;
}

// direct write — the program output is bounded by the buffer; overflow
// truncates (a hostile program cannot write unbounded memory)
template <typename output, ::std::integral char_type,
		  ::fast_io::manipulators::scalar_flags flags, typename T>
	requires(::fast_io::details::lc_time_scalar_v<
				 ::fast_io::manipulators::scalar_manip_t<flags, T>> &&
			 ::fast_io::details::lc_time_value_or_chrono<T> &&
			 ::std::same_as<typename output::output_char_type, char_type>)
inline constexpr void
print_define(lc_ctx<char_type> const *ctx, output out,
			 ::fast_io::manipulators::scalar_manip_t<flags, T> t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	namespace lc = ::fast_io::l10n;
	if (ctx->all == nullptr)
	{
		return;
	}
	auto const prog{::fast_io::details::lc_time_program<flags.time_flag>(ctx)};
	auto const b{lc::lc_get_scatter(ctx->loc, prog)};
	if (b.base == nullptr)
	{
		return;
	}
	::fast_io::details::lc_tm tm{::fast_io::details::lc_tm_from(t.reference)};
	char_type buf[512];
	::fast_io::details::lc_write_sink<char_type> sk{buf, buf + 512};
	::fast_io::details::lc_prog_run<char_type>(b.base, b.len, ctx, tm,
											   sk, 0, false);
	::fast_io::operations::print_freestanding<false>(
		out, ::fast_io::basic_io_scatter_t<char_type>{
				 buf, static_cast<::std::size_t>(sk.it - buf)});
}

// unimbued hooks — a time scalar printed without a locale produces
// the POSIX/C-locale form. These also satisfy print_freestanding_okay
// so the imbued dispatch can see the scalar.
template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags,
		  typename T>
	requires(::fast_io::details::lc_time_scalar_v<
				 ::fast_io::manipulators::scalar_manip_t<flags, T>> &&
			 ::fast_io::details::lc_time_value_or_chrono<T>)
inline constexpr ::std::size_t
print_reserve_size(::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::scalar_manip_t<flags, T>>,
				   ::fast_io::manipulators::scalar_manip_t<flags, T> t) noexcept
{
	::fast_io::details::lc_tm tm{::fast_io::details::lc_tm_from(t.reference)};
	::fast_io::details::lc_count_sink<char_type> sk{};
	::fast_io::details::lc_default_emit<char_type>(flags.time_flag, tm, sk);
	return sk.n;
}

template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags,
		  typename T>
	requires(::fast_io::details::lc_time_scalar_v<
				 ::fast_io::manipulators::scalar_manip_t<flags, T>> &&
			 ::fast_io::details::lc_time_value_or_chrono<T>)
inline constexpr char_type *
print_reserve_define(::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::scalar_manip_t<flags, T>>,
					 char_type *iter,
					 ::fast_io::manipulators::scalar_manip_t<flags, T> t) noexcept
{
	::fast_io::details::lc_tm tm{::fast_io::details::lc_tm_from(t.reference)};
	::fast_io::details::lc_write_sink<char_type> sk{
		iter, iter + print_reserve_size(
						 ::fast_io::io_reserve_type<char_type,
													::fast_io::manipulators::scalar_manip_t<flags, T>>,
						 t)};
	::fast_io::details::lc_default_emit<char_type>(flags.time_flag, tm, sk);
	return sk.it;
}

namespace manipulators
{
// locale time formats — scalar with time_flag set; the section's
// compiled binfmt program runs inside an imbued stream. v is any time
// value the i18n layer can read (iso8601_timestamp,
// posix_timespec64, chrono types, ...)
template <typename T>
inline constexpr scalar_manip_t<
	::fast_io::details::base_lc_time_flags_cache<lc_time_flag::d_t_fmt>, T const &>
d_t_fmt(T const &tsp) noexcept
{
	return {tsp};
}
template <typename T>
inline constexpr scalar_manip_t<
	::fast_io::details::base_lc_time_flags_cache<lc_time_flag::d_fmt>, T const &>
d_fmt(T const &tsp) noexcept
{
	return {tsp};
}
template <typename T>
inline constexpr scalar_manip_t<
	::fast_io::details::base_lc_time_flags_cache<lc_time_flag::t_fmt>, T const &>
t_fmt(T const &tsp) noexcept
{
	return {tsp};
}
template <typename T>
inline constexpr scalar_manip_t<
	::fast_io::details::base_lc_time_flags_cache<lc_time_flag::t_fmt_ampm>, T const &>
t_fmt_ampm(T const &tsp) noexcept
{
	return {tsp};
}
template <typename T>
inline constexpr scalar_manip_t<
	::fast_io::details::base_lc_time_flags_cache<lc_time_flag::date_fmt>, T const &>
date_fmt(T const &tsp) noexcept
{
	return {tsp};
}
template <typename T>
inline constexpr scalar_manip_t<
	::fast_io::details::base_lc_time_flags_cache<lc_time_flag::era_d_t_fmt>, T const &>
era_d_t_fmt(T const &tsp) noexcept
{
	return {tsp};
}
template <typename T>
inline constexpr scalar_manip_t<
	::fast_io::details::base_lc_time_flags_cache<lc_time_flag::era_d_fmt>, T const &>
era_d_fmt(T const &tsp) noexcept
{
	return {tsp};
}
template <typename T>
inline constexpr scalar_manip_t<
	::fast_io::details::base_lc_time_flags_cache<lc_time_flag::era_t_fmt>, T const &>
era_t_fmt(T const &tsp) noexcept
{
	return {tsp};
}

} // namespace manipulators

} // namespace fast_io
