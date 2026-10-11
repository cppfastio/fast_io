#pragma once

// lcblob — flat position-independent locale container (spec Part 7).
// The file is the locale data itself: the basic_lc_* C-struct layout
// with every pointer an lc_rva<T> and every scatter an lc_scatter<T>
// (see fast_io_i18n/lcblob.h for the wire types). All fields are
// little-endian u32/s32 — deterministic on every ABI.
//
// Layout:
//   [lc_locale root 40B][name pool][section image]*
//   section image = basic_lc_all<char_type> struct bytes + payload pool;
//   every rva in it is a u32 file offset.
//
// Emission: each section's payload references are written pool-relative
// with fixup slots recorded; once the section's file base is known the
// fixups relocate every rva in one pass. Single pass, no fixpoint.

#include "compile.h"
#include <fast_io_i18n.h>
#include <fast_io_dsal/impl/misc/push_macros.h>

namespace fast_io_i18n
{
namespace lcblob
{

inline constexpr ::std::uint_least32_t magic{::fast_io::l10n::magic}; // 'FCL1'
inline constexpr ::std::uint_least32_t blob_version{::fast_io::l10n::blob_version};

// categories — master parity (ctype/collate/xliterate are not modeled)
enum lc_cat : ::std::uint_least32_t
{
	cat_identification = 0,
	cat_monetary = 1,
	cat_numeric = 2,
	cat_time = 3,
	cat_messages = 4,
	cat_paper = 5,
	cat_telephone = 6,
	cat_name = 7,
	cat_address = 8,
	cat_measurement = 9,
	cat_keyboard = 10,
	cat_count = 11,
};

enum class slot_tag : ::std::uint_least8_t
{
	absent = 0,
	string = 1,
	strlist = 2,
	integer = 3,
	bytes = 4,
	program = 5,
	int3 = 6,
	eralist = 7,
};

// front-end used for program slots
enum class src_lang : ::std::uint_least8_t
{
	none = 0,
	pct_time = 1,    // strict strftime directives
	pct_generic = 2, // generic %-letters (name_fmt, postal_fmt, tel_*)
};

struct field_def
{
	char const *name;
	slot_tag tag;
	src_lang lang{src_lang::none};
};

// ---------------------------------------------------------------------------
// slot schemas — field order matches fast_io basic_lc_* member order
// ---------------------------------------------------------------------------

inline constexpr ::std::size_t szf{0};

#define LC_S(name_)                                      \
	{                                                    \
		name_, ::fast_io_i18n::lcblob::slot_tag::string, \
		{}                                               \
	}
#define LC_SL(name_)                                      \
	{                                                     \
		name_, ::fast_io_i18n::lcblob::slot_tag::strlist, \
		{}                                                \
	}
#define LC_I(name_)                                       \
	{                                                     \
		name_, ::fast_io_i18n::lcblob::slot_tag::integer, \
		{}                                                \
	}
#define LC_B(name_)                                     \
	{                                                   \
		name_, ::fast_io_i18n::lcblob::slot_tag::bytes, \
		{}                                              \
	}
#define LC_PT(name_)                                      \
	{                                                     \
		name_, ::fast_io_i18n::lcblob::slot_tag::program, \
		::fast_io_i18n::lcblob::src_lang::pct_time}
#define LC_PG(name_)                                      \
	{                                                     \
		name_, ::fast_io_i18n::lcblob::slot_tag::program, \
		::fast_io_i18n::lcblob::src_lang::pct_generic}
#define LC_I3(name_)                                   \
	{                                                  \
		name_, ::fast_io_i18n::lcblob::slot_tag::int3, \
		{}                                             \
	}
#define LC_EL(name_)                                      \
	{                                                     \
		name_, ::fast_io_i18n::lcblob::slot_tag::eralist, \
		{}                                                \
	}

// schema field order == file-struct member order in every category
inline constexpr ::fast_io_i18n::lcblob::field_def identification_fields[]{
	LC_S("title"),
	LC_S("source"),
	LC_S("address"),
	LC_S("contact"),
	LC_S("email"),
	LC_S("tel"),
	LC_S("fax"),
	LC_S("language"),
	LC_S("territory"),
	LC_S("audience"),
	LC_S("application"),
	LC_S("abbreviation"),
	LC_S("revision"),
	LC_S("date"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def monetary_fields[]{
	LC_S("int_curr_symbol"),
	LC_S("currency_symbol"),
	LC_S("mon_decimal_point"),
	LC_S("mon_thousands_sep"),
	LC_B("mon_grouping"),
	LC_S("positive_sign"),
	LC_S("negative_sign"),
	LC_I("int_frac_digits"),
	LC_I("frac_digits"),
	LC_I("p_cs_precedes"),
	LC_I("p_sep_by_space"),
	LC_I("n_cs_precedes"),
	LC_I("n_sep_by_space"),
	LC_I("int_p_cs_precedes"),
	LC_I("int_p_sep_by_space"),
	LC_I("int_n_cs_precedes"),
	LC_I("int_n_sep_by_space"),
	LC_I("p_sign_posn"),
	LC_I("n_sign_posn"),
	LC_I("int_p_sign_posn"),
	LC_I("int_n_sign_posn"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def numeric_fields[]{
	LC_S("decimal_point"),
	LC_S("thousands_sep"),
	LC_B("grouping"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def time_fields[]{
	LC_SL("abday"),
	LC_SL("day"),
	LC_SL("abmon"),
	LC_SL("ab_alt_mon"),
	LC_SL("mon"),
	LC_SL("alt_mon"),
	LC_PT("d_t_fmt"),
	LC_PT("d_fmt"),
	LC_PT("t_fmt"),
	LC_PT("t_fmt_ampm"),
	LC_PT("date_fmt"),
	LC_SL("am_pm"),
	LC_EL("era"),
	LC_PT("era_d_fmt"),
	LC_PT("era_d_t_fmt"),
	LC_PT("era_t_fmt"),
	LC_SL("alt_digits"),
	LC_I3("week"),
	LC_I("first_weekday"),
	LC_I("first_workday"),
	LC_I("cal_direction"),
	LC_SL("timezone"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def messages_fields[]{
	LC_S("yesexpr"),
	LC_S("noexpr"),
	LC_S("yesstr"),
	LC_S("nostr"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def paper_fields[]{
	LC_I("height"),
	LC_I("width"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def telephone_fields[]{
	LC_PG("tel_int_fmt"),
	LC_PG("tel_dom_fmt"),
	LC_S("int_select"),
	LC_S("int_prefix"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def name_fields[]{
	LC_PG("name_fmt"),
	LC_S("name_gen"),
	LC_S("name_miss"),
	LC_S("name_mr"),
	LC_S("name_mrs"),
	LC_S("name_ms"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def address_fields[]{
	LC_PG("postal_fmt"),
	LC_S("country_name"),
	LC_S("country_post"),
	LC_S("country_ab2"),
	LC_S("country_ab3"),
	LC_I("country_num"),
	LC_S("country_car"),
	LC_S("country_isbn"),
	LC_S("lang_name"),
	LC_S("lang_ab"),
	LC_S("lang_term"),
	LC_S("lang_lib"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def measurement_fields[]{
	LC_I("measurement"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def keyboard_fields[]{
	LC_SL("keyboards"),
};

struct cat_schema
{
	::fast_io::containers::span<::fast_io_i18n::lcblob::field_def const> fields;
	char const *glibc_name;
};

// glibc_name maps localedef "LC_FOO" keywords; the array order is the
// cat id. LC_CTYPE/LC_COLLATE are intentionally absent — their bodies
// are ignored by the parser.
inline constexpr ::fast_io_i18n::lcblob::cat_schema cat_schemas[cat_count]{
	{::fast_io::containers::span{identification_fields, ::std::size(identification_fields)}, "LC_IDENTIFICATION"},
	{::fast_io::containers::span{monetary_fields, ::std::size(monetary_fields)}, "LC_MONETARY"},
	{::fast_io::containers::span{numeric_fields, ::std::size(numeric_fields)}, "LC_NUMERIC"},
	{::fast_io::containers::span{time_fields, ::std::size(time_fields)}, "LC_TIME"},
	{::fast_io::containers::span{messages_fields, ::std::size(messages_fields)}, "LC_MESSAGES"},
	{::fast_io::containers::span{paper_fields, ::std::size(paper_fields)}, "LC_PAPER"},
	{::fast_io::containers::span{telephone_fields, ::std::size(telephone_fields)}, "LC_TELEPHONE"},
	{::fast_io::containers::span{name_fields, ::std::size(name_fields)}, "LC_NAME"},
	{::fast_io::containers::span{address_fields, ::std::size(address_fields)}, "LC_ADDRESS"},
	{::fast_io::containers::span{measurement_fields, ::std::size(measurement_fields)}, "LC_MEASUREMENT"},
	{::fast_io::containers::span{keyboard_fields, ::std::size(keyboard_fields)}, "LC_KEYBOARD"},
};

#undef LC_S
#undef LC_SL
#undef LC_I
#undef LC_B
#undef LC_PT
#undef LC_PG
#undef LC_I3
#undef LC_EL

// ---------------------------------------------------------------------------
// charset of the container's payload bytes
// ---------------------------------------------------------------------------

// payload encoding of one blob section — not to be confused with the
// section selector in the consumer header
enum class blob_charset : ::std::uint_least8_t
{
	utf8 = 0,
	utf16 = 1, // LE
	utf32 = 2, // LE
	gb18030 = 3,
	utf_ebcdic = 4,
};

inline constexpr ::fast_io::u8string_view blob_charset_name(blob_charset cs) noexcept
{
	switch (cs)
	{
	case blob_charset::utf8:
		return u8"UTF-8";
	case blob_charset::utf16:
		return u8"UTF-16";
	case blob_charset::utf32:
		return u8"UTF-32";
	case blob_charset::gb18030:
		return u8"GB18030";
	default:
		return u8"UTF-EBCDIC";
	}
}

// u8 source -> section payload bytes
inline ::fast_io::string transcode(::fast_io::u8string_view sv, blob_charset cs) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::string out;
	switch (cs)
	{
	case blob_charset::utf8:
		out.append(reinterpret_cast<char const *>(sv.data()), sv.size());
		return out;
	case blob_charset::utf16:
	{
		::fast_io::u16string u16;
		::fast_io::u16ostring_ref_fast_io ref{__builtin_addressof(u16)};
		::fast_io::print(
			ref, ::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
										  ::fast_io::encoding_scheme::utf_le>(
					 ::fast_io::basic_io_scatter_t<char8_t>{sv.data(), sv.size()}));
		out.append(reinterpret_cast<char const *>(u16.data()), u16.size() * 2);
		return out;
	}
	case blob_charset::utf32:
	{
		::fast_io::u32string u32;
		::fast_io::u32ostring_ref_fast_io ref{__builtin_addressof(u32)};
		::fast_io::print(
			ref, ::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
										  ::fast_io::encoding_scheme::utf_le>(
					 ::fast_io::basic_io_scatter_t<char8_t>{sv.data(), sv.size()}));
		out.append(reinterpret_cast<char const *>(u32.data()), u32.size() * 4);
		return out;
	}
	case blob_charset::gb18030:
	{
		::fast_io::ostring_ref_fast_io oref{__builtin_addressof(out)};
		::fast_io::print(
			oref, ::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
										   ::fast_io::encoding_scheme::gb18030>(
					  ::fast_io::basic_io_scatter_t<char8_t>{sv.data(), sv.size()}));
		return out;
	}
	default: // utf_ebcdic — a byte transform of the utf8 form
	{
		::fast_io::ostring_ref_fast_io oref{__builtin_addressof(out)};
		::fast_io::print(
			oref, ::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
										   ::fast_io::encoding_scheme::utf_ebcdic>(
					  ::fast_io::basic_io_scatter_t<char8_t>{sv.data(), sv.size()}));
		return out;
	}
	}
}

// ---------------------------------------------------------------------------
// program transcode — walk a compiled UTF-8 program and re-emit with all
// text payloads (literal, fill, chrono/element bodies) in charset cs
// ---------------------------------------------------------------------------

namespace details
{

inline void transcode_nodes(::fast_io_i18n::binfmt::reader &r,
							::fast_io_i18n::binfmt::encoder &out, blob_charset cs,
							unsigned depth) FAST_IO_HERBCEPTIONS_THROWS;

// copy a node whose tag was just read, byte-identical [start, cur)
inline void copy_node_tail(::fast_io_i18n::binfmt::reader &r, char8_t const *start,
						   ::fast_io_i18n::binfmt::node_head h,
						   ::fast_io_i18n::binfmt::encoder &out) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io_i18n::binfmt::skip_payload(r, h);
	out.buf.append(reinterpret_cast<char const *>(start),
				   static_cast<::std::size_t>(r.cur() - start));
}

inline void transcode_field_children(::std::uint_least64_t nchildren,
									 ::fast_io_i18n::binfmt::reader &r,
									 ::fast_io_i18n::binfmt::encoder &out,
									 blob_charset cs, unsigned depth) FAST_IO_HERBCEPTIONS_THROWS
{
	using namespace ::fast_io_i18n::binfmt;
	for (::std::uint_least64_t k{}; k < nchildren; ++k)
	{
		char8_t const *start{r.cur()};
		node_head h;
		next_tag(r, h);
		if (h.kind == kind_bytes &&
			h.code == static_cast<::std::uint_least32_t>(field_param::fill))
		{
			::std::uint_least64_t n{};
			r.get_leb(n);
			::fast_io::u8string_view bytes;
			r.get_bytes(n, bytes);
			auto tb{transcode(bytes, cs)};
			out.node_bytes(h.code, ::fast_io::string_view{tb.data(), tb.size()});
		}
		else if (h.kind == kind_bytes &&
				 (h.code == static_cast<::std::uint_least32_t>(field_param::chrono) ||
				  h.code == static_cast<::std::uint_least32_t>(field_param::element)))
		{
			::std::uint_least64_t n{};
			r.get_leb(n);
			::fast_io::u8string_view bytes;
			r.get_bytes(n, bytes);
			reader sub{bytes};
			encoder subout;
			transcode_nodes(sub, subout, cs, depth + 1);
			out.node_bytes(h.code,
						   ::fast_io::string_view{subout.buf.data(), subout.buf.size()});
		}
		else
		{
			copy_node_tail(r, start, h, out);
		}
	}
}

inline void transcode_nodes(::fast_io_i18n::binfmt::reader &r,
							::fast_io_i18n::binfmt::encoder &out, blob_charset cs,
							unsigned depth) FAST_IO_HERBCEPTIONS_THROWS
{
	using namespace ::fast_io_i18n::binfmt;
	if (depth > 8)
	{
		throw throws::std::errc::invalid_argument;
	}
	while (!r.is_empty())
	{
		char8_t const *start{r.cur()};
		node_head h;
		next_tag(r, h);
		if (h.code == op_literal && h.kind == kind_bytes)
		{
			::std::uint_least64_t n{};
			r.get_leb(n);
			::fast_io::u8string_view bytes;
			r.get_bytes(n, bytes);
			auto tb{transcode(bytes, cs)};
			out.literal(::fast_io::string_view{tb.data(), tb.size()});
		}
		else if (h.code == op_field && h.kind == kind_list)
		{
			::std::uint_least64_t n{};
			r.get_leb(n);
			// emit children into a side buffer: the out list node
			// needs the child count first
			encoder sub;
			transcode_field_children(n, r, sub, cs, depth);
			out.node_list_begin(op_field, n);
			out.buf.append(sub.buf.data(), sub.buf.size());
		}
		else
		{
			copy_node_tail(r, start, h, out);
		}
	}
}

} // namespace details

// transcode every text payload of a compiled program into charset cs
inline ::fast_io::string transcode_program(::fast_io::string_view prog,
										   blob_charset cs) FAST_IO_HERBCEPTIONS_THROWS
{
	if (cs == blob_charset::utf8)
	{
		::fast_io::string r;
		r.append(prog.data(), prog.size());
		return r;
	}
	::fast_io_i18n::binfmt::reader r{prog};
	::fast_io_i18n::binfmt::encoder out;
	details::transcode_nodes(r, out, cs, 0);
	return ::std::move(out.buf);
}

// ---------------------------------------------------------------------------
// builder
// ---------------------------------------------------------------------------

struct era_src
{
	::std::int_least64_t direction{};
	::std::int_least64_t offset{};
	::std::int_least64_t start_year{};
	::std::int_least32_t start_month{};
	::std::int_least32_t start_day{};
	::std::int_least64_t end_year{};
	::std::int_least32_t end_month{};
	::std::int_least32_t end_day{};
	::fast_io::u8string_view name{};
	::fast_io::u8string_view fmt{}; // pct program source text
};

struct slot_src
{
	// STRING: strs[0]; STRLIST: all entries
	::fast_io::vector<::fast_io::u8string_view> strs{};
	// INT ints[0]; INT3 ints[0..2]; BYTES ints as i8 values
	::fast_io::vector<::std::int_least64_t> ints{};
	// PROGRAM source text
	::fast_io::u8string_view fmt_src{};
	// ERALIST
	::fast_io::vector<era_src> eras{};
};

struct cat_src
{
	bool present{false};
	::fast_io::vector<slot_src> slots{}; // sized to schema fields
};

namespace details
{

// little-endian scalar <-> host (file fields are always LE)
inline constexpr ::std::uint_least32_t le32(::std::uint_least32_t v) noexcept
{
	if constexpr (::std::endian::native == ::std::endian::big)
	{
		v = ::std::byteswap(v);
	}
	return v;
}

// append one LE u32 to a byte buffer — byte-wise, host independent
inline void put_u32(::fast_io::string &buf, ::std::uint_least32_t v) noexcept
{
	char b[4]{static_cast<char>(v), static_cast<char>(v >> 8),
			  static_cast<char>(v >> 16), static_cast<char>(v >> 24)};
	buf.append(b, 4);
}

inline ::std::uint_least32_t rd_le32(char const *p) noexcept
{
	return static_cast<::std::uint_least8_t>(p[0]) |
		   (static_cast<::std::uint_least32_t>(static_cast<::std::uint_least8_t>(p[1])) << 8) |
		   (static_cast<::std::uint_least32_t>(static_cast<::std::uint_least8_t>(p[2])) << 16) |
		   (static_cast<::std::uint_least32_t>(static_cast<::std::uint_least8_t>(p[3])) << 24);
}

inline void wr_le32(char *p, ::std::uint_least32_t v) noexcept
{
	p[0] = static_cast<char>(v);
	p[1] = static_cast<char>(v >> 8);
	p[2] = static_cast<char>(v >> 16);
	p[3] = static_cast<char>(v >> 24);
}

inline bool slot_has(slot_tag tag, slot_src const &s) noexcept
{
	switch (tag)
	{
	case slot_tag::string:
	case slot_tag::strlist:
		return !s.strs.is_empty();
	case slot_tag::bytes:
	case slot_tag::integer:
	case slot_tag::int3:
		return !s.ints.is_empty();
	case slot_tag::program:
		return !s.fmt_src.is_empty();
	case slot_tag::eralist:
		return !s.eras.is_empty();
	default:
		return false;
	}
}

} // namespace details

// ---------------------------------------------------------------------------
// one section under construction: a real basic_lc_all<char> image (all
// basic_lc_all<T> share the same all-u32 layout — the char_type tag only
// documents semantics) plus the payload pool after it.
//
// Member rva fields are written POOL-RELATIVE; fixups relocate them to
// file offsets (pool_base = section file offset + sizeof(lc_all)) once
// the container layout is known.
// ---------------------------------------------------------------------------

struct sec_build
{
	::fast_io::l10n::basic_lc_all<char> img{};
	::fast_io::string pool{};
	::fast_io::vector<::std::uint_least32_t> fix_img{};
	::fast_io::vector<::std::uint_least32_t> fix_pool{};

	inline ::std::uint_least32_t here() const noexcept
	{
		return static_cast<::std::uint_least32_t>(pool.size());
	}
	inline ::std::uint_least32_t intern(::fast_io::string const &bytes) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto const rva{here()};
		pool.append(bytes.data(), bytes.size());
		return rva;
	}
	// an rva member carrying a pool-relative offset — relocate() adds
	// the pool's file base. fix_img stores BYTE OFFSETS into img, not
	// pointers: the object must survive being moved into secs[].
	template <typename T>
	inline void rva(::fast_io::l10n::lc_rva<T> &m, ::std::uint_least32_t off) FAST_IO_HERBCEPTIONS_THROWS
	{
		m.off = details::le32(off);
		fix_img.push_back(static_cast<::std::uint_least32_t>(
			reinterpret_cast<char *>(__builtin_addressof(m.off)) -
			reinterpret_cast<char *>(&img)));
	}
	// scatter member: rva + element count (no fixup on len)
	template <typename T>
	inline void scatter(::fast_io::l10n::lc_scatter<T> &m, ::std::uint_least32_t off,
						::std::uint_least32_t len) FAST_IO_HERBCEPTIONS_THROWS
	{
		rva(m.ref, off);
		m.len = details::le32(len);
	}
	// plain s32 member
	inline void sint(::std::int_least32_t &m, ::std::int_least64_t v) noexcept
	{
		m = static_cast<::std::int_least32_t>(
			details::le32(static_cast<::std::uint_least32_t>(
				static_cast<::std::int_least32_t>(v))));
	}
	// a pool-internal rva slot (strref tables, era recs) — appended LE
	inline void pool_rva(::std::uint_least32_t off) FAST_IO_HERBCEPTIONS_THROWS
	{
		fix_pool.push_back(here());
		details::put_u32(pool, off);
	}
	// every recorded rva slot += pool's file base
	inline void relocate(::std::uint_least32_t pool_base) noexcept
	{
		for (auto off : fix_img)
		{
			auto *m{reinterpret_cast<::std::uint_least32_t *>(
				reinterpret_cast<char *>(&img) + off)};
			*m = details::le32(details::le32(*m) + pool_base);
		}
		for (auto off : fix_pool)
		{
			details::wr_le32(pool.data() + off,
							 details::rd_le32(pool.data() + off) + pool_base);
		}
	}
};

namespace details
{

// byte size of one char unit in a section's payload charset
inline constexpr ::std::size_t cs_units(blob_charset cs) noexcept
{
	return cs == blob_charset::utf16 ? 2 : cs == blob_charset::utf32 ? 4
																	 : 1;
}

// member of the section image by byte offset
template <typename M>
inline M *mem_at(void *img, ::std::uint_least32_t off) noexcept
{
	return reinterpret_cast<M *>(static_cast<char *>(img) + off);
}

// text member <- transcoded string; len is in section char units
inline void sec_string(::fast_io::l10n::lc_scatter<char> &m,
					   ::fast_io::u8string_view sv, sec_build &b, blob_charset cs,
					   ::std::size_t units) FAST_IO_HERBCEPTIONS_THROWS
{
	if (sv.is_empty())
	{
		return;
	}
	auto t{transcode(sv, cs)};
	b.scatter(m, b.intern(t),
			  static_cast<::std::uint_least32_t>(t.size() / units));
}

// fixed inline scatter array (abday[7], am_pm[2] ...): at most cap elems
inline void sec_strs(::fast_io::l10n::lc_scatter<char> *arr,
					 ::std::size_t cap,
					 ::fast_io::vector<::fast_io::u8string_view> const &strs,
					 sec_build &b, blob_charset cs, ::std::size_t units) FAST_IO_HERBCEPTIONS_THROWS
{
	for (::std::size_t i{}; i < cap && i < strs.size(); ++i)
	{
		sec_string(arr[i], strs[i], b, cs, units);
	}
}

// dynamic list member {tbl_rva,count} — tbl = strref{u32,u32}[] in pool
inline void sec_strlist(
	::fast_io::l10n::lc_scatter<::fast_io::l10n::lc_scatter<char>> &m,
	::fast_io::vector<::fast_io::u8string_view> const &strs, sec_build &b,
	blob_charset cs, ::std::size_t units) FAST_IO_HERBCEPTIONS_THROWS
{
	if (strs.is_empty())
	{
		return;
	}
	::fast_io::vector<::std::uint_least32_t> rvas;
	::fast_io::vector<::std::size_t> lens;
	for (auto e : strs)
	{
		auto t{transcode(e, cs)};
		lens.push_back(t.size());
		rvas.push_back(b.intern(t));
	}
	auto const tbl{b.here()};
	auto li{lens.cbegin()};
	for (auto r : rvas)
	{
		b.pool_rva(r);
		put_u32(b.pool, static_cast<::std::uint_least32_t>(*li++ / units));
	}
	b.scatter(m, tbl, static_cast<::std::uint_least32_t>(rvas.size()));
}

// compiled-program member — a rejected directive drops the slot, not
// the locale
inline void sec_program(::fast_io::l10n::lc_scatter<char8_t> &m,
						::fast_io::u8string_view fmt_src, src_lang lang, sec_build &b,
						blob_charset cs,
						::fast_io_i18n::binfmt::src_ctx ctx,
						char const *fname) FAST_IO_HERBCEPTIONS_THROWS
{
	try
	{
		auto prog{lang == src_lang::pct_generic
					  ? ::fast_io_i18n::binfmt::compile_generic_pct(fmt_src, ctx)
					  : ::fast_io_i18n::binfmt::compile_time(fmt_src, ctx)};
		auto tc{transcode_program(::fast_io::string_view{prog.data(), prog.size()}, cs)};
		b.scatter(m, b.intern(tc), static_cast<::std::uint_least32_t>(tc.size()));
	}
	catch throws(::std::error)
	{
		::fast_io::perrln("  skip program `", ::fast_io::mnp::os_c_str(fname), "' in ",
						  ctx.where);
	}
}

// raw byte list member (grouping)
inline void sec_bytes(::fast_io::l10n::lc_scatter<char8_t> &m,
					  ::fast_io::vector<::std::int_least64_t> const &ints,
					  sec_build &b) FAST_IO_HERBCEPTIONS_THROWS
{
	if (ints.is_empty())
	{
		return;
	}
	auto const rva{b.here()};
	for (auto v : ints)
	{
		b.pool.push_back(static_cast<char>(v & 0xFF));
	}
	b.scatter(m, rva, static_cast<::std::uint_least32_t>(ints.size()));
}

// era list member {tbl_rva,count} — tbl = basic_lc_time_era[count] in
// pool: s32 direction | s32 offset | s32 start_year | u32 start_month |
// u32 start_day | s32 end_year | u32 end_month | u32 end_day |
// strref name | strref fmt(program)
inline void sec_era(
	::fast_io::l10n::lc_scatter<::fast_io::l10n::basic_lc_time_era<char>> &m,
	::fast_io::vector<era_src> const &eras, sec_build &b, blob_charset cs,
	::std::size_t units, ::fast_io_i18n::binfmt::src_ctx ctx) FAST_IO_HERBCEPTIONS_THROWS
{
	if (eras.is_empty())
	{
		return;
	}
	::fast_io::vector<::std::uint_least32_t> name_rvas;
	::fast_io::vector<::std::size_t> name_lens;
	::fast_io::vector<::std::uint_least32_t> fmt_rvas;
	::fast_io::vector<::fast_io::string> fmts;
	try
	{
		for (auto const &e : eras)
		{
			auto t{transcode(e.name, cs)};
			name_lens.push_back(t.size());
			name_rvas.push_back(b.intern(t));
			auto prog{::fast_io_i18n::binfmt::compile_time(e.fmt, ctx)};
			fmts.emplace_back(transcode_program(::fast_io::string_view{prog.data(), prog.size()}, cs));
			fmt_rvas.push_back(b.intern(fmts.back()));
		}
	}
	catch throws(::std::error)
	{
		::fast_io::perrln("  skip `era' in ", ctx.where);
		return;
	}
	auto const tbl{b.here()};
	auto ni{name_rvas.cbegin()};
	auto nl{name_lens.cbegin()};
	auto fi{fmt_rvas.cbegin()};
	auto ti{fmts.cbegin()};
	for (auto const &e : eras)
	{
		put_u32(b.pool, static_cast<::std::uint_least32_t>(
							static_cast<::std::int_least32_t>(e.direction)));
		put_u32(b.pool, static_cast<::std::uint_least32_t>(
							static_cast<::std::int_least32_t>(e.offset)));
		put_u32(b.pool, static_cast<::std::uint_least32_t>(
							static_cast<::std::int_least32_t>(e.start_year)));
		put_u32(b.pool, static_cast<::std::uint_least32_t>(e.start_month));
		put_u32(b.pool, static_cast<::std::uint_least32_t>(e.start_day));
		put_u32(b.pool, static_cast<::std::uint_least32_t>(
							static_cast<::std::int_least32_t>(e.end_year)));
		put_u32(b.pool, static_cast<::std::uint_least32_t>(e.end_month));
		put_u32(b.pool, static_cast<::std::uint_least32_t>(e.end_day));
		b.pool_rva(*ni++);
		put_u32(b.pool, static_cast<::std::uint_least32_t>(*nl++ / units));
		b.pool_rva(*fi++);
		put_u32(b.pool, static_cast<::std::uint_least32_t>(ti->size()));
		++ti;
	}
	b.scatter(m, tbl, static_cast<::std::uint_least32_t>(eras.size()));
}

} // namespace details

// ---------------------------------------------------------------------------
// emit — fill one section image (basic_lc_all members at compile-time
// offsets), then assemble the container
// ---------------------------------------------------------------------------

namespace details
{

using all_t = ::fast_io::l10n::basic_lc_all<char>;

// member byte offsets per schema field index — schema order == member
// order in every category
constexpr ::std::uint_least32_t identification_off[]{
	offsetof(all_t, identification.title),
	offsetof(all_t, identification.source),
	offsetof(all_t, identification.address),
	offsetof(all_t, identification.contact),
	offsetof(all_t, identification.email),
	offsetof(all_t, identification.tel),
	offsetof(all_t, identification.fax),
	offsetof(all_t, identification.language),
	offsetof(all_t, identification.territory),
	offsetof(all_t, identification.audience),
	offsetof(all_t, identification.application),
	offsetof(all_t, identification.abbreviation),
	offsetof(all_t, identification.revision),
	offsetof(all_t, identification.date)};

constexpr ::std::uint_least32_t monetary_off[]{
	offsetof(all_t, monetary.int_curr_symbol),
	offsetof(all_t, monetary.currency_symbol),
	offsetof(all_t, monetary.mon_decimal_point),
	offsetof(all_t, monetary.mon_thousands_sep),
	offsetof(all_t, monetary.mon_grouping),
	offsetof(all_t, monetary.positive_sign),
	offsetof(all_t, monetary.negative_sign),
	offsetof(all_t, monetary.int_frac_digits),
	offsetof(all_t, monetary.frac_digits),
	offsetof(all_t, monetary.p_cs_precedes),
	offsetof(all_t, monetary.p_sep_by_space),
	offsetof(all_t, monetary.n_cs_precedes),
	offsetof(all_t, monetary.n_sep_by_space),
	offsetof(all_t, monetary.int_p_cs_precedes),
	offsetof(all_t, monetary.int_p_sep_by_space),
	offsetof(all_t, monetary.int_n_cs_precedes),
	offsetof(all_t, monetary.int_n_sep_by_space),
	offsetof(all_t, monetary.p_sign_posn),
	offsetof(all_t, monetary.n_sign_posn),
	offsetof(all_t, monetary.int_p_sign_posn),
	offsetof(all_t, monetary.int_n_sign_posn)};

constexpr ::std::uint_least32_t numeric_off[]{
	offsetof(all_t, numeric.decimal_point),
	offsetof(all_t, numeric.thousands_sep),
	offsetof(all_t, numeric.grouping)};

constexpr ::std::uint_least32_t time_off[]{
	offsetof(all_t, time.abday),
	offsetof(all_t, time.day),
	offsetof(all_t, time.abmon),
	offsetof(all_t, time.ab_alt_mon),
	offsetof(all_t, time.mon),
	offsetof(all_t, time.alt_mon),
	offsetof(all_t, time.d_t_fmt),
	offsetof(all_t, time.d_fmt),
	offsetof(all_t, time.t_fmt),
	offsetof(all_t, time.t_fmt_ampm),
	offsetof(all_t, time.date_fmt),
	offsetof(all_t, time.am_pm),
	offsetof(all_t, time.era),
	offsetof(all_t, time.era_d_fmt),
	offsetof(all_t, time.era_d_t_fmt),
	offsetof(all_t, time.era_t_fmt),
	offsetof(all_t, time.alt_digits),
	offsetof(all_t, time.week),
	offsetof(all_t, time.first_weekday),
	offsetof(all_t, time.first_workday),
	offsetof(all_t, time.cal_direction),
	offsetof(all_t, time.timezone)};

// aux = fixed inline-array capacity (0 = regular slot)
constexpr ::std::uint_least8_t time_aux[]{
	7, 7, 12, 12, 12, 12, 0, 0, 0, 0, 0, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

constexpr ::std::uint_least32_t messages_off[]{
	offsetof(all_t, messages.yesexpr), offsetof(all_t, messages.noexpr),
	offsetof(all_t, messages.yesstr), offsetof(all_t, messages.nostr)};

constexpr ::std::uint_least32_t paper_off[]{
	offsetof(all_t, paper.height), offsetof(all_t, paper.width)};

constexpr ::std::uint_least32_t telephone_off[]{
	offsetof(all_t, telephone.tel_int_fmt),
	offsetof(all_t, telephone.tel_dom_fmt),
	offsetof(all_t, telephone.int_select),
	offsetof(all_t, telephone.int_prefix)};

constexpr ::std::uint_least32_t name_off[]{
	offsetof(all_t, name.name_fmt), offsetof(all_t, name.name_gen),
	offsetof(all_t, name.name_miss), offsetof(all_t, name.name_mr),
	offsetof(all_t, name.name_mrs), offsetof(all_t, name.name_ms)};

constexpr ::std::uint_least32_t address_off[]{
	offsetof(all_t, address.postal_fmt),
	offsetof(all_t, address.country_name),
	offsetof(all_t, address.country_post),
	offsetof(all_t, address.country_ab2),
	offsetof(all_t, address.country_ab3),
	offsetof(all_t, address.country_num),
	offsetof(all_t, address.country_car),
	offsetof(all_t, address.country_isbn),
	offsetof(all_t, address.lang_name),
	offsetof(all_t, address.lang_ab),
	offsetof(all_t, address.lang_term),
	offsetof(all_t, address.lang_lib)};

constexpr ::std::uint_least32_t measurement_off[]{
	offsetof(all_t, measurement.measurement)};

constexpr ::std::uint_least32_t keyboard_off[]{
	offsetof(all_t, keyboard.keyboards)};

constexpr ::std::uint_least8_t zero_aux[22]{};

struct cat_map
{
	::std::uint_least32_t const *off;
	::std::uint_least8_t const *aux;
};

constexpr cat_map cat_maps[cat_count]{
	{identification_off, zero_aux}, {monetary_off, zero_aux}, {numeric_off, zero_aux}, {time_off, time_aux}, {messages_off, zero_aux}, {paper_off, zero_aux}, {telephone_off, zero_aux}, {name_off, zero_aux}, {address_off, zero_aux}, {measurement_off, zero_aux}, {keyboard_off, zero_aux}};

// fill one category's members in the section image
inline void fill_cat(void *img, cat_map const &m, cat_schema const &sch,
					 cat_src const &cat, sec_build &b, blob_charset cs,
					 ::std::size_t units,
					 ::fast_io_i18n::binfmt::src_ctx ctx) FAST_IO_HERBCEPTIONS_THROWS
{
	for (::std::size_t f{}; f < sch.fields.size(); ++f)
	{
		auto const &def{sch.fields[f]};
		auto const &s{cat.slots[f]};
		if (!slot_has(def.tag, s))
		{
			continue;
		}
		auto const off{m.off[f]};
		switch (def.tag)
		{
		case slot_tag::string:
			sec_string(*mem_at<::fast_io::l10n::lc_scatter<char>>(img, off),
					   s.strs[0], b, cs, units);
			break;
		case slot_tag::program:
			sec_program(*mem_at<::fast_io::l10n::lc_scatter<char8_t>>(img, off),
						s.fmt_src, def.lang, b, cs, ctx, def.name);
			break;
		case slot_tag::bytes:
			sec_bytes(*mem_at<::fast_io::l10n::lc_scatter<char8_t>>(img, off),
					  s.ints, b);
			break;
		case slot_tag::strlist:
			if (m.aux[f])
			{
				sec_strs(mem_at<::fast_io::l10n::lc_scatter<char>>(img, off),
						 m.aux[f], s.strs, b, cs, units);
			}
			else
			{
				sec_strlist(
					*mem_at<::fast_io::l10n::lc_scatter<
						::fast_io::l10n::lc_scatter<char>>>(img, off),
					s.strs, b, cs, units);
			}
			break;
		case slot_tag::eralist:
			sec_era(*mem_at<::fast_io::l10n::lc_scatter<
						::fast_io::l10n::basic_lc_time_era<char>>>(img, off),
					s.eras, b, cs, units, ctx);
			break;
		case slot_tag::integer:
			b.sint(*mem_at<::std::int_least32_t>(img, off), s.ints[0]);
			break;
		case slot_tag::int3:
			for (::std::size_t k{}; k < 3 && k < s.ints.size(); ++k)
			{
				b.sint(*mem_at<::std::int_least32_t>(img, off + k * 4), s.ints[k]);
			}
			break;
		default:
			break;
		}
	}
}

} // namespace details

// fill one section image for payload charset cs. name is the canonical
// locale name and encname this section's charset name — both land in
// identification in the section's own encoding.
inline sec_build build_section(cat_src const cats[cat_count],
							   ::fast_io::u8string_view name,
							   ::fast_io::u8string_view encname, blob_charset cs,
							   ::fast_io::string_view file_ctx) FAST_IO_HERBCEPTIONS_THROWS
{
	sec_build b;
	auto const units{details::cs_units(cs)};
	::fast_io_i18n::binfmt::src_ctx ctx{file_ctx};
	auto &a{b.img};
	details::sec_string(a.identification.name, name, b, cs, units);
	details::sec_string(a.identification.encoding, encname, b, cs, units);
	for (::std::size_t c{}; c < cat_count; ++c)
	{
		if (cats[c].present)
		{
			details::fill_cat(&a, details::cat_maps[c], cat_schemas[c],
							  cats[c], b, cs, units, ctx);
		}
	}
	return b;
}

// ---------------------------------------------------------------------------
// container — the file IS an lc_locale:
//   [lc_locale root 40B][name bytes utf8][section image]*4
//   section image = basic_lc_all<char_type> bytes + payload pool; every
//   rva inside it is a u32 file offset.
// Sections in slot order: [charset, utf8, utf16, utf32]; the charset
// section aliases the utf8 one when codeset == utf8 (secs[0] unused).
// ---------------------------------------------------------------------------

inline ::fast_io::string build_container(sec_build (&secs)[4], bool charset_is_utf8,
										 ::fast_io::u8string_view name,
										 ::std::uint_least32_t codeset) FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr ::std::uint_least32_t img_size{
		sizeof(::fast_io::l10n::basic_lc_all<char>)};
	::std::uint_least32_t const name_rva{
		static_cast<::std::uint_least32_t>(
			sizeof(::fast_io::l10n::lc_locale))};
	::std::uint_least32_t off{
		name_rva + static_cast<::std::uint_least32_t>(name.size())};
	::std::uint_least32_t base[4]{};
	::std::size_t const first{charset_is_utf8 ? 1zu : 0zu};
	for (::std::size_t c{first}; c < 4; ++c)
	{
		base[c] = off;
		off += img_size + static_cast<::std::uint_least32_t>(secs[c].pool.size());
	}
	if (charset_is_utf8)
	{
		base[0] = base[1];
	}
	for (::std::size_t c{first}; c < 4; ++c)
	{
		secs[c].relocate(base[c] + img_size);
	}
	::std::uint_least32_t const total{off};

	::fast_io::string blob;
	details::put_u32(blob, magic);
	details::put_u32(blob, blob_version);
	details::put_u32(blob, total);
	details::put_u32(blob, 0); // flags
	details::put_u32(blob, codeset);
	details::put_u32(blob, name_rva);
	details::put_u32(blob, static_cast<::std::uint_least32_t>(name.size()));
	for (auto sbase : base)
	{
		details::put_u32(blob, sbase);
	}
	blob.append(reinterpret_cast<char const *>(name.data()), name.size());
	for (::std::size_t c{first}; c < 4; ++c)
	{
		blob.append(reinterpret_cast<char const *>(&secs[c].img), img_size);
		blob.append(secs[c].pool.data(), secs[c].pool.size());
	}
	return blob;
}


} // namespace lcblob
} // namespace fast_io_i18n

#include <fast_io_dsal/impl/misc/pop_macros.h>
