#pragma once

// binfmt — compiled binary format-string program format.
// See spec.md in this directory. Wire format: bare node sequences of
// LEB128-tagged nodes; no program header, the embedding bound ends it.
//
// Tooling-side header: uses fast_io containers. Everything that can
// report an error reports it through herbceptions
// (throw throws ::std::errc) — this header is built with -fherbceptions.

#include <cstdint>
#include <cstddef>
#include <span>
#include <string_view>
#include <optional>
#include <fast_io_dsal/impl/misc/push_macros.h>

namespace fast_io_i18n
{
namespace binfmt
{

// ---------------------------------------------------------------------------
// node framing: [uleb128 tag][payload], tag = (code << 3) | payload_kind
// (kind is 3 bits — values 0-4 are used; 5-7 reserved)
// ---------------------------------------------------------------------------

inline constexpr ::std::uint_least8_t kind_none{0};
inline constexpr ::std::uint_least8_t kind_uleb{1};
inline constexpr ::std::uint_least8_t kind_bytes{2};
inline constexpr ::std::uint_least8_t kind_list{3};
inline constexpr ::std::uint_least8_t kind_sleb{4};
// kinds 5-7 reserved

// ---------------------------------------------------------------------------
// top-level op codes
// ---------------------------------------------------------------------------

inline constexpr ::std::uint_least32_t op_literal{1};
inline constexpr ::std::uint_least32_t op_field{2};
inline constexpr ::std::uint_least32_t op_pct{3};
// op 4 free
inline constexpr ::std::uint_least32_t op_plural{0x40};
inline constexpr ::std::uint_least32_t op_plural_count{0x41};
inline constexpr ::std::uint_least32_t op_plural_form{0x42};

// ---------------------------------------------------------------------------
// field child param codes
// ---------------------------------------------------------------------------

enum class field_param : ::std::uint_least32_t
{
	arg = 1,
	fill = 2,
	align = 3,
	sign = 4,
	flag_alt = 5,
	flag_zero = 6,
	flag_group = 7,
	flag_locale = 8,
	flag_outdigits = 9,
	width = 10,
	prec = 11,
	ctype = 12,
	type = 13,
	chrono = 14,
	element = 15,
	flag_upper = 16,
};

// align values
inline constexpr ::std::uint_least32_t align_left{1};
inline constexpr ::std::uint_least32_t align_right{2};
inline constexpr ::std::uint_least32_t align_center{3};

// sign values
inline constexpr ::std::uint_least32_t sign_plus{1};
inline constexpr ::std::uint_least32_t sign_minus{2};
inline constexpr ::std::uint_least32_t sign_space{3};

// ---------------------------------------------------------------------------
// pct child param codes
// ---------------------------------------------------------------------------

enum class pct_param : ::std::uint_least32_t
{
	conv = 20,
	pad = 22,      // bit0 '-' bit1 '_' bit2 '0'
	casef = 23,    // bit0 '^' bit1 '#'
	modifier = 24, // 1 E (era), 2 O (alternative)
	colons = 25,   // 1-3, conv z only
				   // 10 width, 11 prec are shared with field_param codes
};

inline constexpr ::std::uint_least32_t pct_modifier_era{1};
inline constexpr ::std::uint_least32_t pct_modifier_alt{2};

// ---------------------------------------------------------------------------
// pct conv enum (spec Part 4)
// ---------------------------------------------------------------------------

enum class pct_conv : ::std::uint_least32_t
{
	a = 1,     // weekday abbr
	A = 2,     // weekday full
	b = 3,     // month abbr (absorbs %h)
	B = 4,     // month full
	c = 5,     // composite -> d_t_fmt
	C = 6,     // century
	d = 7,     // day of month
	e = 8,     // day of month, space-padded
	F = 9,     // ISO date composite
	G = 10,    // ISO week-numbering year
	H = 11,    // hour 00-23
	I = 12,    // hour 01-12
	j = 13,    // day of year
	k = 14,    // hour 0-23 space-pad
	l = 15,    // hour 1-12 space-pad
	m = 16,    // month
	M = 17,    // minute
	p = 18,    // AM/PM
	P = 19,    // am/pm lowercase
	r = 20,    // composite -> t_fmt_ampm
	R = 21,    // %H:%M
	s = 22,    // seconds since epoch
	S = 23,    // second
	T = 24,    // %H:%M:%S
	u = 25,    // weekday Mon=1..Sun=7
	U = 26,    // week number (Sunday)
	V = 27,    // ISO week number
	w = 28,    // weekday Sun=0..Sat=6
	W = 29,    // week number (Monday)
	x = 30,    // composite -> d_fmt
	X = 31,    // composite -> t_fmt
	Y = 32,    // year (full)
	z = 33,    // tz offset
	Z = 34,    // tz name
	plus = 35, // composite -> date_fmt
	iso8601 = 36,
	iso8601_utc = 37,
	fracsec = 38,
};

// ---------------------------------------------------------------------------
// ctype enum (spec Part 3) — declared C arg type, blob metadata
// ---------------------------------------------------------------------------

enum class ctype : ::std::uint_least32_t
{
	other = 0, // user-defined / absent
	i32 = 1,
	i64 = 2,
	i128 = 3,
	u32 = 4,
	u64 = 5,
	u128 = 6,
	f16 = 7,
	bf16 = 8,
	f32 = 9,
	f64 = 10,
	f80 = 11,
	f128 = 12,
	cf16 = 13,
	cbf16 = 14,
	cf32 = 15,
	cf64 = 16,
	cf80 = 17,
	cf128 = 18,
	c8 = 19,
	c16 = 20,
	c32 = 21,
	cebc = 22,
	c8ptr = 23,
	c16ptr = 24,
	c32ptr = 25,
	ebcptr = 26,
	gbptr = 27,
	c8view = 28,
	c16view = 29,
	c32view = 30,
	ebcview = 31,
	gbview = 32,
	ptr = 33,
	fptr = 34,
	fldptr_i = 35, // Itanium member object ptr — 1 word
	mthptr_i = 36, // Itanium member fn ptr — 2 words
	fldptr_m1 = 37,
	fldptr_m2 = 38,
	mthptr_m1 = 39,
	mthptr_m2 = 40,
	mthptr_m3 = 41,
	mthptr_m4 = 42,
	error = 43, // herbceptions std::error — {domain const*, size_t}
};

// ---------------------------------------------------------------------------
// type enum (spec Part 3)
// ---------------------------------------------------------------------------

enum class conv_type : ::std::uint_least32_t
{
	d = 1,
	u = 2,
	o = 3,
	x = 4,
	b = 5,
	c = 6,
	s = 7,
	debug = 8, // '?'
	p = 9,
	addr = 10, // addrvw/pointervw/itervw/funcvw/fieldptrvw
	mth = 11,  // methodvw
	dec = 12,  // fast_io decimal shortest (float default)
	decp = 13,
	fix = 14,
	fixp = 15,
	sci = 16,
	scip = 17,
	gen = 18,
	genp = 19,
	hexf = 20,
	hexfp = 21,
	rng = 22,
	rngn = 23,
	rngm = 24,
};

// ---------------------------------------------------------------------------
// encoder — appends to a byte buffer. LEB128 encode/decode reuse
// fast_io's serializations/leb128.h (pr_rsv_leb128_impl / scn_cnt_*).
// ---------------------------------------------------------------------------

template <::fast_io::details::my_integral T>
inline void put_leb128_to(::fast_io::string &buf, T v) noexcept
{
	char tmp[::fast_io::details::leb128_length_val<T>];
	char *e{::fast_io::details::pr_rsv_leb128_impl(tmp, v)};
	buf.append(tmp, e);
}

struct encoder
{
	::fast_io::string buf{};

	void tag(::std::uint_least32_t code, ::std::uint_least8_t kind) noexcept
	{
		put_leb128_to(buf, (static_cast<::std::uint_least64_t>(code) << 3) | kind);
	}
	void node_none(::std::uint_least32_t code) noexcept
	{
		tag(code, kind_none);
	}
	void node_uleb(::std::uint_least32_t code, ::std::uint_least64_t v) noexcept
	{
		tag(code, kind_uleb);
		put_leb128_to(buf, v);
	}
	void node_sleb(::std::uint_least32_t code, ::std::int_least64_t v) noexcept
	{
		tag(code, kind_sleb);
		put_leb128_to(buf, v);
	}
	void node_bytes(::std::uint_least32_t code, ::std::string_view bytes) noexcept
	{
		tag(code, kind_bytes);
		put_leb128_to(buf, bytes.size());
		buf.append(bytes.data(), bytes.size());
	}
	// list: count written first, children appended after (their own bytes)
	void node_list_begin(::std::uint_least32_t code, ::std::uint_least64_t nchildren) noexcept
	{
		tag(code, kind_list);
		put_leb128_to(buf, nchildren);
	}
	void node_list(::std::uint_least32_t code, ::std::span<::std::string_view const> children) noexcept
	{
		node_list_begin(code, children.size());
		for (auto c : children)
		{
			buf.append(c.data(), c.size());
		}
	}
	void literal(::std::string_view bytes) noexcept
	{
		node_bytes(op_literal, bytes);
	}
};

// ---------------------------------------------------------------------------
// decoder — bounds-checked cursor over a program's byte span.
// LEB128 reads go through scan() on an ibuffer_view: end-of-input throws
// std::error (parse domain) by itself; structural violations throw
// std::errc::invalid_argument here.
// ---------------------------------------------------------------------------

struct reader
{
	::fast_io::u8ibuffer_view iv{};

	constexpr reader() noexcept = default;
	constexpr reader(::std::u8string_view sv) noexcept
		: iv{sv.data(), sv.data() + sv.size()}
	{
	}
	constexpr reader(::fast_io::u8string_view sv) noexcept
		: iv{sv.data(), sv.data() + sv.size()}
	{
	}
	// byte buffer view: chars reinterpreted as u8 (blob payloads are
	// charset bytes; container charset is a blob-level property)
	constexpr reader(::std::string_view sv) noexcept
		: iv{reinterpret_cast<char8_t const *>(sv.data()),
			 reinterpret_cast<char8_t const *>(sv.data() + sv.size())}
	{
	}

	template <::fast_io::details::my_integral T>
	void get_leb(T &v) FAST_IO_HERBCEPTIONS_THROWS
	{
		::fast_io::scan(iv, ::fast_io::mnp::leb128_get(v));
	}

	void get_bytes(::std::size_t n, ::fast_io::u8string_view &v) FAST_IO_HERBCEPTIONS_THROWS
	{
		if (static_cast<::std::size_t>(iv.end_ptr - iv.curr_ptr) < n)
		{
			throw throws::std::errc::invalid_argument;
		}
		v = ::fast_io::u8string_view{iv.curr_ptr, n};
		iv.curr_ptr += n;
	}

	[[nodiscard]] bool empty() const noexcept
	{
		return iv.curr_ptr == iv.end_ptr;
	}
	[[nodiscard]] char8_t const *cur() const noexcept
	{
		return iv.curr_ptr;
	}
};

struct node_head
{
	::std::uint_least32_t code{};
	::std::uint_least8_t kind{};
};

// Read one node tag. On success the reader is positioned at the payload.
// Out-param: this toolchain zeroes the upper bytes of register-sized
// aggregate returns from functions that make calls — out-params avoid it.
inline void next_tag(reader &r, node_head &h) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::uint_least64_t tag{};
	r.get_leb(tag);
	h.code = static_cast<::std::uint_least32_t>(tag >> 3);
	h.kind = static_cast<::std::uint_least8_t>(tag & 7u);
	if (h.kind > kind_sleb)
	{
		throw throws::std::errc::invalid_argument;
	}
}

// Skip the payload of a node whose tag was already read.
inline void skip_payload(reader &r, node_head h) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::uint_least64_t v{};
	switch (h.kind)
	{
	case kind_none:
		return;
	case kind_uleb:
		r.get_leb(v);
		return;
	case kind_sleb:
	{
		::std::int_least64_t sv{};
		r.get_leb(sv);
		return;
	}
	case kind_bytes:
	{
		r.get_leb(v);
		::fast_io::u8string_view bytes;
		r.get_bytes(v, bytes);
		return;
	}
	case kind_list:
	{
		r.get_leb(v);
		// children are scalars/bytes/markers — a list never contains a list
		for (; v; --v)
		{
			node_head c;
			next_tag(r, c);
			if (c.kind == kind_list)
			{
				throw throws::std::errc::invalid_argument;
			}
			skip_payload(r, c);
		}
		return;
	}
	default:
		throw throws::std::errc::invalid_argument;
	}
}

} // namespace binfmt
} // namespace fast_io_i18n

#include <fast_io_dsal/impl/misc/pop_macros.h>
