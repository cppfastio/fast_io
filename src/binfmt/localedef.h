#pragma once

// glibc localedata source parser — localedef text -> typed category data.
// Uses fast_io for file I/O, text handling and containers; errors are
// herbceptions (throw throws ::std::errc) with a diagnostic printed first.
//
// Handled: comment_char / escape_char directives, line continuations,
// quoted strings with escapes and <U> codepoint names, `;`-separated
// token lists, `copy "file"` imports, `END` markers.
// LC_CTYPE / LC_COLLATE are not modeled at all — their bodies are
// skipped as unknown categories.

#include "lcblob.h"
#include <fast_io_dsal/impl/misc/push_macros.h>

namespace fast_io_i18n
{
namespace localedef
{

struct lc_field
{
	::fast_io::string name{};
	::fast_io::vector<::fast_io::string> tokens{}; // decoded UTF-8
};

struct lc_file_data
{
	::fast_io::vector<lc_field> fields[::fast_io_i18n::lcblob::cat_count]{};
};

struct parse_ctx
{
	::std::string_view file{};
	::std::size_t line_no{};
};

[[noreturn]] inline void pfails(parse_ctx const &ctx, ::std::string_view msg,
								::std::string_view line) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::perrln(ctx.file, ":", ctx.line_no, ": ", msg, " | ", line);
	throw throws::std::errc::invalid_argument;
}

inline bool is_sp(char c) noexcept
{
	return c == ' ' || c == '\t';
}

inline void rstrip(::std::string_view &s) noexcept
{
	while (!s.empty() && is_sp(s.back()))
	{
		s.remove_suffix(1);
	}
}

inline void lstrip(::std::string_view &s) noexcept
{
	while (!s.empty() && is_sp(s.front()))
	{
		s.remove_prefix(1);
	}
}

inline void strip(::std::string_view &s) noexcept
{
	lstrip(s);
	rstrip(s);
}

// remove comment text: comment_char outside quotes ends the line
inline void strip_comment(::std::string_view &line, char comment, char escape) noexcept
{
	bool inq{};
	bool esc{};
	for (::std::size_t i{}; i < line.size(); ++i)
	{
		char c{line[i]};
		if (esc)
		{
			esc = false;
			continue;
		}
		if (c == escape)
		{
			esc = true;
			continue;
		}
		if (inq)
		{
			if (c == '"')
			{
				inq = false;
			}
			continue;
		}
		if (c == '"')
		{
			inq = true;
			continue;
		}
		if (c == comment)
		{
			line = line.substr(0, i);
			return;
		}
	}
}

// append UTF-8 bytes of a codepoint
inline void put_u8_cp(::fast_io::string &out, char32_t cp) noexcept
{
	char8_t tmp[8];
	auto n{::fast_io::get_utf_code_units(cp, tmp)};
	out.append(reinterpret_cast<char const *>(tmp), n);
}

// decode a quoted string's interior: escape_char, <U..> names -> UTF-8
inline void decode_string_content(::fast_io::string &out, ::std::string_view inner,
								  char escape, parse_ctx const &ctx,
								  ::std::string_view line) FAST_IO_HERBCEPTIONS_THROWS
{
	for (::std::size_t i{}; i < inner.size(); ++i)
	{
		char c{inner[i]};
		if (c == escape)
		{
			if (++i == inner.size())
			{
				pfails(ctx, "dangling escape in string", line);
			}
			out.push_back(inner[i]);
			continue;
		}
		if (c == '<')
		{
			if (i + 2 < inner.size() && inner[i + 1] == 'U')
			{
				auto j{inner.find('>', i + 2)};
				if (j == ::std::string_view::npos)
				{
					pfails(ctx, "unterminated <U> name", line);
				}
				::std::uint_least32_t cp{};
				for (::std::size_t k{i + 2}; k < j; ++k)
				{
					char h{inner[k]};
					unsigned d{static_cast<unsigned>(
						h >= '0' && h <= '9'   ? h - '0'
						: h >= 'A' && h <= 'F' ? h - 'A' + 10
						: h >= 'a' && h <= 'f' ? h - 'a' + 10
											   : 99)};
					if (d > 15)
					{
						pfails(ctx, "bad <U> name", line);
					}
					cp = cp * 16 + d;
					if (cp > 0x10FFFFu)
					{
						pfails(ctx, "<U> out of range", line);
					}
				}
				put_u8_cp(out, cp);
				i = j;
				continue;
			}
			// named repertoire chars (<RLE>, <ARABIC COMMA>…) need a
			// repertoire map we don't carry — hard error
			pfails(ctx, "unsupported repertoire name", line);
		}
		out.push_back(c);
	}
}

// tokenize a field value: `;`-separated quoted strings / bare words
inline void tokenize(lc_field &f, ::std::string_view val, char escape,
					 parse_ctx const &ctx, ::std::string_view line) FAST_IO_HERBCEPTIONS_THROWS
{
	for (;;)
	{
		lstrip(val);
		if (val.empty())
		{
			return;
		}
		if (val.front() == '"')
		{
			val.remove_prefix(1);
			::fast_io::string tok;
			// find end quote (escape-aware)
			::std::size_t i{};
			bool esc{};
			for (; i < val.size(); ++i)
			{
				char c{val[i]};
				if (esc)
				{
					esc = false;
					continue;
				}
				if (c == escape)
				{
					esc = true;
					continue;
				}
				if (c == '"')
				{
					break;
				}
			}
			decode_string_content(tok, val.substr(0, i), escape, ctx, line);
			val.remove_prefix(i);
			if (val.empty() || val.front() != '"')
			{
				pfails(ctx, "unterminated string", line);
			}
			val.remove_prefix(1);
			f.tokens.emplace_back(::std::move(tok));
		}
		else
		{
			auto j{val.find_first_of(";\t ")};
			f.tokens.emplace_back(::fast_io::string_view{
				val.data(), j == ::std::string_view::npos ? val.size() : j});
			if (j == ::std::string_view::npos)
			{
				return;
			}
			val.remove_prefix(j);
		}
		lstrip(val);
		if (val.empty())
		{
			return;
		}
		if (val.front() != ';')
		{
			pfails(ctx, "expected ';' between tokens", line);
		}
		val.remove_prefix(1);
	}
}

// insert or replace a field by name (glibc: later definitions win)
inline void upsert_field(::fast_io::vector<lc_field> &v, lc_field &&f) noexcept
{
	for (auto &e : v)
	{
		if (e.name == f.name)
		{
			e.tokens = ::std::move(f.tokens);
			return;
		}
	}
	v.emplace_back(::std::move(f));
}

// category id from "LC_X" name; cat_count if unknown
inline ::std::size_t cat_id_of(::std::string_view name) noexcept
{
	for (::std::size_t i{}; i < ::fast_io_i18n::lcblob::cat_count; ++i)
	{
		if (name == ::fast_io_i18n::lcblob::cat_schemas[i].glibc_name)
		{
			return i;
		}
	}
	return ::fast_io_i18n::lcblob::cat_count;
}

using file_cache =
	::fast_io::vector<::std::pair<::fast_io::string, lc_file_data>>;

inline void parse_file(::fast_io::dir_file const &df, ::fast_io::string const &name,
					   lc_file_data &out, file_cache &cache, unsigned depth) FAST_IO_HERBCEPTIONS_THROWS;

// parse one logical line's "key value" into the current category
inline void parse_field_line(lc_file_data &out, ::std::size_t cat,
							 ::std::string_view line, char escape,
							 parse_ctx const &ctx) FAST_IO_HERBCEPTIONS_THROWS
{
	auto it{line.find_first_of(" \t")};
	if (it == ::std::string_view::npos)
	{
		return; // bare keyword with no value — ignore
	}
	lc_field f;
	::std::string_view key{line.substr(0, it)};
	::std::string_view val{line.substr(it)};
	strip(val);
	tokenize(f, val, escape, ctx, line);
	if (f.tokens.empty())
	{
		pfails(ctx, "empty field value", line);
	}
	f.name.append(key.data(), key.data() + key.size());
	upsert_field(out.fields[cat], ::std::move(f));
}

inline void parse_file(::fast_io::dir_file const &df, ::fast_io::string const &name,
					   lc_file_data &out, file_cache &cache, unsigned depth) FAST_IO_HERBCEPTIONS_THROWS
{
	if (depth > 8)
	{
		pfails({::std::string_view{name.data(), name.size()}, 0},
			   "copy depth exceeded", {});
	}
	for (auto const &e : cache)
	{
		if (e.first == name)
		{
			out = e.second;
			return;
		}
	}
	::fast_io::native_file_loader fmp(::fast_io::at(df),
									  ::fast_io::mnp::os_c_str(name.c_str()));
	char comment_char{'%'};
	char escape_char{'/'};
	::std::size_t cur_cat{::fast_io_i18n::lcblob::cat_count};
	::fast_io::string pending; // continuation join buffer
	parse_ctx ctx{{name.data(), name.size()}, 0};
	::std::string_view fdata{fmp.data(), fmp.size()};
	for (::std::size_t pos{}; pos <= fdata.size();)
	{
		auto nl{fdata.find('\n', pos)};
		if (nl == ::std::string_view::npos)
		{
			nl = fdata.size();
		}
		::std::string_view line{fdata.substr(pos, nl - pos)};
		pos = nl + 1;
		++ctx.line_no;
		rstrip(line);
		// escape/comment directives must be recognized before the
		// continuation/comment processing below — `escape_char /` ends
		// with the escape char itself, `comment_char %` with the comment
		// char
		if (pending.empty() && line.substr(0, 13) == "comment_char ")
		{
			auto v{line.substr(13)};
			strip(v);
			if (v.empty())
			{
				pfails(ctx, "bad comment_char directive", line);
			}
			comment_char = v.front();
			continue;
		}
		if (pending.empty() && line.substr(0, 12) == "escape_char ")
		{
			auto v{line.substr(12)};
			strip(v);
			if (v.empty())
			{
				pfails(ctx, "bad escape_char directive", line);
			}
			escape_char = v.front();
			continue;
		}
		// continuation: last char == escape_char joins the next line
		if (!line.empty() && line.back() == escape_char)
		{
			pending.append(line.data(), line.size() - 1);
			continue;
		}
		if (!pending.empty())
		{
			pending.append(line.data(), line.size());
			line = {pending.data(), pending.size()};
		}
		strip_comment(line, comment_char, escape_char);
		strip(line);
		if (line.empty())
		{
			pending.clear();
			continue;
		}
		if (line.substr(0, 9) == "category ")
		{
			pending.clear();
			continue; // LC_IDENTIFICATION coverage lines
		}
		if (line.starts_with("LC_"))
		{
			cur_cat = cat_id_of(line);
			pending.clear();
			continue;
		}
		if (line.substr(0, 4) == "END ")
		{
			cur_cat = ::fast_io_i18n::lcblob::cat_count;
			pending.clear();
			continue;
		}
		if (cur_cat == ::fast_io_i18n::lcblob::cat_count)
		{
			pending.clear();
			continue;
		}
		if (line.substr(0, 5) == "copy ")
		{
			auto v{line.substr(5)};
			strip(v);
			if (v.size() < 2 || v.front() != '"' || v.back() != '"')
			{
				pfails(ctx, "bad copy directive", line);
			}
			lc_file_data imp;
			::fast_io::string fname;
			fname.append(v.data() + 1, v.data() + v.size() - 1);
			parse_file(df, fname, imp, cache, depth + 1);
			for (auto &e : imp.fields[cur_cat])
			{
				lc_field cp;
				cp.name = e.name;
				cp.tokens = e.tokens;
				upsert_field(out.fields[cur_cat], ::std::move(cp));
			}
			pending.clear();
			continue;
		}
		parse_field_line(out, cur_cat, line, escape_char, ctx);
		pending.clear();
	}
	cache.emplace_back(name, out);
}

// ---------------------------------------------------------------------------
// lc_file_data -> cat_src bridge (typed slots for the blob builder)
// ---------------------------------------------------------------------------

inline ::std::int_least64_t to_i64(::std::string_view tok, parse_ctx const &ctx,
								   ::std::string_view line) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!tok.empty() && tok.front() == '+')
	{
		tok.remove_prefix(1);
	}
	::std::int_least64_t v{};
	char const *const b{tok.data()};
	auto const r{::fast_io::details::scan_int_contiguous_define_impl<10, true, false, true>(
		b, b + tok.size(), v)};
	if (r.code != ::fast_io::freestanding::parse_errc::ok || r.iter != b + tok.size())
	{
		pfails(ctx, "bad integer", line);
	}
	return v;
}

// era token: "dir:offset:start:end:name:fmt" — already unquoted-decoded
inline void parse_era(::fast_io_i18n::lcblob::era_src &e, ::std::string_view tok,
					  parse_ctx const &ctx, ::std::string_view line) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t segs[5]{};
	{
		// find the first 5 ':' separators; fmt may itself contain ':'
		::std::size_t p{}, c{};
		for (; c < 5 && p != ::std::string_view::npos; ++c)
		{
			p = tok.find(':', c == 0 ? 0 : segs[c - 1] + 1);
			segs[c] = p;
		}
		if (segs[4] == ::std::string_view::npos)
		{
			pfails(ctx, "era record needs 6 fields", line);
		}
	}
	auto seg{[&](unsigned n) noexcept -> ::std::string_view {
		::std::size_t b{n == 0 ? 0 : segs[n - 1] + 1};
		::std::size_t e{n == 5 ? tok.size() : segs[n]};
		return tok.substr(b, e - b);
	}};
	auto date{[&](::std::string_view d, ::std::int_least64_t &yr,
				  ::std::int_least32_t &mo, ::std::int_least32_t &dy) FAST_IO_HERBCEPTIONS_THROWS {
		if (d == "+*")
		{
			yr = 2147483647;
			return;
		}
		if (d == "-*")
		{
			yr = -2147483648;
			return;
		}
		auto p1{d.find('/')};
		auto p2{p1 == ::std::string_view::npos ? ::std::string_view::npos
											   : d.find('/', p1 + 1)};
		if (p1 == ::std::string_view::npos || p2 == ::std::string_view::npos)
		{
			pfails(ctx, "bad era date", line);
		}
		yr = to_i64(d.substr(0, p1), ctx, line);
		mo = static_cast<::std::int_least32_t>(
			to_i64(d.substr(p1 + 1, p2 - p1 - 1), ctx, line));
		dy = static_cast<::std::int_least32_t>(
			to_i64(d.substr(p2 + 1), ctx, line));
	}};
	auto dir{seg(0)};
	if (dir == "+")
	{
		e.direction = 1;
	}
	else if (dir == "-")
	{
		e.direction = -1;
	}
	else
	{
		pfails(ctx, "bad era direction", line);
	}
	e.offset = to_i64(seg(1), ctx, line);
	date(seg(2), e.start_year, e.start_month, e.start_day);
	date(seg(3), e.end_year, e.end_month, e.end_day);
	auto nm{seg(4)};
	auto fm{seg(5)};
	e.name = {reinterpret_cast<char8_t const *>(nm.data()), nm.size()};
	e.fmt = {reinterpret_cast<char8_t const *>(fm.data()), fm.size()};
}

// map parsed fields onto the schema's typed slots; returns filled cats
inline void to_cats(lc_file_data const &d,
					::fast_io_i18n::lcblob::cat_src cats[],
					parse_ctx const &ctx) FAST_IO_HERBCEPTIONS_THROWS
{
	using ::fast_io_i18n::lcblob::cat_count;
	using ::fast_io_i18n::lcblob::cat_schemas;
	using ::fast_io_i18n::lcblob::slot_tag;
	for (::std::size_t c{}; c < cat_count; ++c)
	{
		if (d.fields[c].empty())
		{
			continue;
		}
		cats[c].present = true;
		auto const &sch{cat_schemas[c]};
		cats[c].slots.resize(sch.fields.size());
		for (auto const &f : d.fields[c])
		{
			::std::size_t fi{sch.fields.size()};
			for (::std::size_t k{}; k < sch.fields.size(); ++k)
			{
				if (::std::string_view{f.name.data(), f.name.size()} == sch.fields[k].name)
				{
					fi = k;
					break;
				}
			}
			if (fi == sch.fields.size())
			{
				::fast_io::perrln(ctx.file, ": unknown field `",
								  f.name, "' — skipped");
				continue;
			}
			auto const &def{sch.fields[fi]};
			auto &sl{cats[c].slots[fi]};
			switch (def.tag)
			{
			case slot_tag::string:
			case slot_tag::strlist:
				for (auto const &t : f.tokens)
				{
					sl.strs.emplace_back(
						reinterpret_cast<char8_t const *>(t.data()),
						t.size());
				}
				break;
			case slot_tag::integer:
			case slot_tag::int3:
			case slot_tag::bytes:
				for (auto const &t : f.tokens)
				{
					sl.ints.push_back(to_i64(
						{t.data(), t.size()}, ctx,
						{t.data(), t.size()}));
				}
				break;
			case slot_tag::program:
				sl.fmt_src = {
					reinterpret_cast<char8_t const *>(f.tokens[0].data()),
					f.tokens[0].size()};
				break;
			case slot_tag::eralist:
				for (auto const &t : f.tokens)
				{
					::fast_io_i18n::lcblob::era_src e;
					parse_era(e, {t.data(), t.size()}, ctx,
							  {t.data(), t.size()});
					sl.eras.emplace_back(e);
				}
				break;
			default:
				break;
			}
		}
	}
}

} // namespace localedef
} // namespace fast_io_i18n

#include <fast_io_dsal/impl/misc/pop_macros.h>
