#pragma once

// binfmt compilers — text format string -> bare node sequence, run
// offline (localedef/codegen). Anything on the format-string attack
// surface is a hard error here (throw throws), never a blob encoding.
//
// Output programs are UTF-8 source-charset; the container emitter
// transcodes literal/fill payloads to the target charset afterwards
// (compile.h deliberately knows nothing about container charsets).

#include "binfmt.h"
#include <fast_io_dsal/impl/misc/push_macros.h>

namespace fast_io_i18n
{
namespace binfmt
{

// ---------------------------------------------------------------------------
// diagnostics — the {domain, code} channel carries no message, so we print
// context first, then throw. std::errc domain.
// ---------------------------------------------------------------------------

struct src_ctx
{
	::fast_io::string_view where{}; // file / category.field context
};

[[noreturn]] inline void fail(src_ctx ctx, ::fast_io::u8string_view src,
							  ::std::size_t pos, ::fast_io::string_view msg) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::perrln(ctx.where, ": ", msg, " [pos ", pos, "] ",
					  ::fast_io::string_view{reinterpret_cast<char const *>(src.data()), src.size()});
	throw throws::std::errc::invalid_argument;
}

// ---------------------------------------------------------------------------
// shared lexical helpers
// ---------------------------------------------------------------------------

namespace details
{


// bytes of the UTF-8 code point starting at s[i]; 0 = invalid
inline constexpr ::std::size_t utf8_cp_size(::fast_io::u8string_view s,
											::std::size_t i) noexcept
{
	char8_t c{s[i]};
	if (c < 0x80)
	{
		return 1;
	}
	::std::size_t n{static_cast<::std::size_t>(
		(c >> 5) == 0x6    ? 2
		: (c >> 4) == 0xE  ? 3
		: (c >> 3) == 0x1E ? 4
						   : 0)};
	if (n == 0 || i + n > s.size())
	{
		return 0;
	}
	for (::std::size_t k{1}; k < n; ++k)
	{
		if ((s[i + k] >> 6) != 0x2)
		{
			return 0;
		}
	}
	return n;
}

inline ::std::uint_least32_t parse_u32(::fast_io::u8string_view s,
									   ::std::size_t &i, ::std::size_t j,
									   src_ctx ctx, ::fast_io::string_view what) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::uint_least32_t v{};
	char8_t const *const b{s.data()};
	auto const r{::fast_io::details::scan_int_contiguous_define_impl<10, true, false, true>(
		b + i, b + j, v)};
	if (r.code != ::fast_io::freestanding::parse_errc::ok || r.iter == b + i)
	{
		fail(ctx, s, i, what);
	}
	i = static_cast<::std::size_t>(r.iter - b);
	return v;
}

class litbuf
{
public:
	void push(char8_t c) noexcept
	{
		lit_.push_back(static_cast<char>(c));
	}
	void push(::fast_io::u8string_view v) noexcept
	{
		lit_.append(reinterpret_cast<char const *>(v.data()), v.size());
	}
	void flush(encoder &b) noexcept
	{
		if (!lit_.is_empty())
		{
			b.literal(::fast_io::string_view{lit_.data(), lit_.size()});
			lit_.clear();
		}
	}

private:
	::fast_io::string lit_;
};

// ---------------------------------------------------------------------------
// pct (strftime) compiler — LC_TIME fmts + generic %-letter slots
// ---------------------------------------------------------------------------

inline ::std::optional<pct_conv> pct_conv_lookup(char8_t c) noexcept
{
	switch (c)
	{
	case u8'a':
		return pct_conv::a;
	case u8'A':
		return pct_conv::A;
	case u8'b':
		return pct_conv::b;
	case u8'h':
		return pct_conv::b; // %h canonicalizes to %b
	case u8'B':
		return pct_conv::B;
	case u8'c':
		return pct_conv::c;
	case u8'C':
		return pct_conv::C;
	case u8'd':
		return pct_conv::d;
	case u8'e':
		return pct_conv::e;
	case u8'F':
		return pct_conv::F;
	case u8'G':
		return pct_conv::G;
	case u8'H':
		return pct_conv::H;
	case u8'I':
		return pct_conv::I;
	case u8'j':
		return pct_conv::j;
	case u8'k':
		return pct_conv::k;
	case u8'l':
		return pct_conv::l;
	case u8'm':
		return pct_conv::m;
	case u8'M':
		return pct_conv::M;
	case u8'p':
		return pct_conv::p;
	case u8'P':
		return pct_conv::P;
	case u8'r':
		return pct_conv::r;
	case u8'R':
		return pct_conv::R;
	case u's':
		return pct_conv::s;
	case u8'S':
		return pct_conv::S;
	case u8'T':
		return pct_conv::T;
	case u8'u':
		return pct_conv::u;
	case u8'U':
		return pct_conv::U;
	case u8'V':
		return pct_conv::V;
	case u8'w':
		return pct_conv::w;
	case u8'W':
		return pct_conv::W;
	case u8'x':
		return pct_conv::x;
	case u8'X':
		return pct_conv::X;
	case u8'Y':
		return pct_conv::Y;
	case u8'z':
		return pct_conv::z;
	case u8'Z':
		return pct_conv::Z;
	case u8'+':
		return pct_conv::plus;
	default:
		return {};
	}
}

struct pct_options
{
	bool allow_nested_fmt{false}; // chrono specs: '{...}' fields allowed
};

::fast_io::string compile_pct_impl(::fast_io::u8string_view s, pct_options opts,
								   unsigned depth, src_ctx ctx) FAST_IO_HERBCEPTIONS_THROWS;

::std::size_t parse_fmt_field(::fast_io::u8string_view s, ::std::size_t pos,
							  encoder &b, unsigned depth, src_ctx ctx) FAST_IO_HERBCEPTIONS_THROWS;

inline ::fast_io::string compile_pct_impl(::fast_io::u8string_view s, pct_options opts,
										  unsigned depth, src_ctx ctx) FAST_IO_HERBCEPTIONS_THROWS
{
	if (depth > 8)
	{
		fail(ctx, s, 0, "nested format depth exceeded");
	}
	encoder b;
	litbuf lit;
	for (::std::size_t i{}; i < s.size();)
	{
		char8_t c{s[i]};
		if (c == u8'{' && opts.allow_nested_fmt)
		{
			lit.flush(b);
			i = parse_fmt_field(s, i, b, depth, ctx);
			continue;
		}
		if (c != u8'%')
		{
			lit.push(c);
			++i;
			continue;
		}
		lit.flush(b);
		++i;
		if (i == s.size())
		{
			lit.push(u8'%');
			break;
		}
		switch (s[i])
		{
		case u8'%':
			lit.push(u8'%');
			++i;
			continue;
		case u8'n':
			lit.push(u8'\n');
			++i;
			continue;
		case u8't':
			lit.push(u8'\t');
			++i;
			continue;
		default:
			break;
		}
		::std::uint_least32_t pad{};
		::std::uint_least32_t casef{};
		for (; i < s.size(); ++i)
		{
			switch (s[i])
			{
			case u8'-':
				pad |= 1;
				continue;
			case u8'_':
				pad |= 2;
				continue;
			case u8'0':
				pad |= 4;
				continue;
			case u8'^':
				casef |= 1;
				continue;
			case u8'#':
				casef |= 2;
				continue;
			default:
				break;
			}
			break;
		}
		::std::uint_least32_t width{};
		bool has_width{};
		if (i < s.size() && ::fast_io::char_category::is_c_digit(s[i]))
		{
			width = parse_u32(s, i, s.size(), ctx, "width too large");
			has_width = true;
		}
		::std::uint_least32_t colons{};
		while (i < s.size() && s[i] == u8':' && colons < 3)
		{
			++colons;
			++i;
		}
		::std::uint_least32_t modifier{};
		if (i < s.size() && s[i] == u8'E')
		{
			modifier = pct_modifier_era;
			++i;
		}
		else if (i < s.size() && s[i] == u8'O')
		{
			modifier = pct_modifier_alt;
			++i;
		}
		if (i == s.size())
		{
			fail(ctx, s, i, "dangling '%'");
		}
		char8_t conv{s[i++]};
		if (colons != 0 && conv != u8'z')
		{
			fail(ctx, s, i - 1, "%: is only valid with z");
		}
		// 2-digit year forms substitute the 4-digit equivalent
		// (spec Part 6): %y->%Y, %Oy->%OY, %Ey->%EY (same encoding),
		// %g->%G, %D->%m/%d/%Y
		if (conv == u'D')
		{
			b.node_uleb(op_pct,
						static_cast<::std::uint_least32_t>(pct_conv::m));
			b.literal("/");
			b.node_uleb(op_pct,
						static_cast<::std::uint_least32_t>(pct_conv::d));
			b.literal("/");
			b.node_uleb(op_pct,
						static_cast<::std::uint_least32_t>(pct_conv::Y));
			continue;
		}
		if (conv == u8'y')
		{
			conv = u8'Y';
		}
		else if (conv == u8'g')
		{
			conv = u8'G';
		}
		auto cv{pct_conv_lookup(conv)};
		if (!cv)
		{
			fail(ctx, s, i - 1, "unsupported % directive");
		}
		if (!pad && !casef && !modifier && !has_width && !colons)
		{
			// scalar form: tag + uleb conv — 2 bytes
			b.node_uleb(op_pct, static_cast<::std::uint_least32_t>(*cv));
			continue;
		}
		::std::uint_least64_t nk{1u + (modifier ? 1u : 0u) + (pad ? 1u : 0u) +
								 (casef ? 1u : 0u) + (colons ? 1u : 0u) +
								 (has_width ? 1u : 0u)};
		b.node_list_begin(op_pct, nk);
		b.node_uleb(static_cast<::std::uint_least32_t>(pct_param::conv),
					static_cast<::std::uint_least32_t>(*cv));
		if (modifier)
		{
			b.node_uleb(static_cast<::std::uint_least32_t>(pct_param::modifier), modifier);
		}
		if (pad)
		{
			b.node_uleb(static_cast<::std::uint_least32_t>(pct_param::pad), pad);
		}
		if (casef)
		{
			b.node_uleb(static_cast<::std::uint_least32_t>(pct_param::casef), casef);
		}
		if (colons)
		{
			b.node_uleb(static_cast<::std::uint_least32_t>(pct_param::colons), colons);
		}
		if (has_width)
		{
			b.node_uleb(static_cast<::std::uint_least32_t>(field_param::width), width);
		}
	}
	lit.flush(b);
	return ::std::move(b.buf);
}

} // namespace details

// ---------------------------------------------------------------------------
// fmt front-end ({} fields)
// ---------------------------------------------------------------------------

namespace details
{

struct fmt_spec
{
	::fast_io::vector<::fast_io::string> children{};
	bool ok{true};
};

// standard spec parse -> field children; on any non-standard content
// returns ok=false (caller retries as chrono pct program)
inline fmt_spec parse_fmt_standard(::fast_io::u8string_view spec, src_ctx ctx,
								   unsigned depth) FAST_IO_HERBCEPTIONS_THROWS
{
	fmt_spec f;
	encoder k;
	::std::size_t i{};
	::std::size_t const n{spec.size()};
	auto push = [&f](encoder &e) FAST_IO_HERBCEPTIONS_THROWS { f.children.emplace_back(::std::move(e.buf)); };
	// [fill]align
	::std::uint_least32_t al{};
	auto align_of = [](char8_t c) noexcept -> ::std::uint_least32_t {
		switch (c)
		{
		case u8'<':
			return align_left;
		case u8'>':
			return align_right;
		case u8'^':
			return align_center;
		default:
			return 0;
		}
	};
	if (i < n && align_of(spec[i]))
	{
		al = align_of(spec[i]);
		++i;
	}
	else if (::std::size_t cp{utf8_cp_size(spec, i)};
			 cp != 0 && i + cp < n && align_of(spec[i + cp]))
	{
		// fill bytes = raw code point bytes in source charset
		::fast_io::string_view fv{
			reinterpret_cast<char const *>(spec.data() + i), cp};
		k.node_bytes(static_cast<::std::uint_least32_t>(field_param::fill), fv);
		push(k);
		al = align_of(spec[i + cp]);
		i += cp + 1;
	}
	if (al)
	{
		k.node_uleb(static_cast<::std::uint_least32_t>(field_param::align), al);
		push(k);
	}
	// sign
	if (i < n)
	{
		::std::uint_least32_t sg{};
		switch (spec[i])
		{
		case u8'+':
			sg = sign_plus;
			break;
		case u8'-':
			sg = sign_minus;
			break;
		case u8' ':
			sg = sign_space;
			break;
		default:
			break;
		}
		if (sg)
		{
			k.node_uleb(static_cast<::std::uint_least32_t>(field_param::sign), sg);
			push(k);
			++i;
		}
	}
	// '#'
	if (i < n && spec[i] == u8'#')
	{
		k.node_none(static_cast<::std::uint_least32_t>(field_param::flag_alt));
		push(k);
		++i;
	}
	// '0'
	if (i < n && spec[i] == u8'0')
	{
		k.node_none(static_cast<::std::uint_least32_t>(field_param::flag_zero));
		push(k);
		++i;
	}
	// width — digits only; '{' dynamic width rejected
	if (i < n && spec[i] == u8'{')
	{
		fail(ctx, spec, i, "dynamic width not supported");
	}
	if (i < n && ::fast_io::char_category::is_c_digit(spec[i]))
	{
		k.node_uleb(static_cast<::std::uint_least32_t>(field_param::width),
					parse_u32(spec, i, n, ctx, "width too large"));
		push(k);
	}
	// precision
	bool has_prec{};
	if (i < n && spec[i] == u8'.')
	{
		++i;
		has_prec = true;
		if (i < n && spec[i] == u8'{')
		{
			fail(ctx, spec, i, "dynamic precision not supported");
		}
		if (i == n || !::fast_io::char_category::is_c_digit(spec[i]))
		{
			fail(ctx, spec, i, "bare '.' precision");
		}
		k.node_uleb(static_cast<::std::uint_least32_t>(field_param::prec),
					parse_u32(spec, i, n, ctx, "precision too large"));
		push(k);
	}
	// 'L'
	if (i < n && spec[i] == u8'L')
	{
		k.node_none(static_cast<::std::uint_least32_t>(field_param::flag_locale));
		push(k);
		++i;
	}
	if (i == n)
	{
		return f;
	}
	// type / range
	bool upper{};
	auto type_of = [](char8_t c) noexcept -> ::std::optional<conv_type> {
		switch (c)
		{
		case u8'b':
		case u8'B':
			return conv_type::b;
		case u8'c':
			return conv_type::c;
		case u8'd':
			return conv_type::d;
		case u8'o':
			return conv_type::o;
		case u8'x':
		case u8'X':
			return conv_type::x;
		case u's':
			return conv_type::s;
		case u8'?':
			return conv_type::debug;
		case u8'p':
		case u8'P':
			return conv_type::p;
		case u8'a':
		case u8'A':
			return conv_type::hexf;
		case u8'e':
		case u8'E':
			return conv_type::scip;
		case u8'f':
		case u8'F':
			return conv_type::fixp;
		case u8'g':
		case u8'G':
			return conv_type::genp;
		default:
			return {};
		}
	};
	char8_t tc{spec[i]};
	upper = (tc >= u8'A' && tc <= u8'Z');
	auto ty{type_of(tc)};
	if (ty)
	{
		// float letters with absent precision bake the fmt default
		switch (*ty)
		{
		case conv_type::fixp:
		case conv_type::scip:
		case conv_type::genp:
			if (!has_prec)
			{
				k.node_uleb(static_cast<::std::uint_least32_t>(field_param::prec), 6);
				push(k);
			}
			break;
		default:
			break;
		}
		if (*ty == conv_type::hexf && has_prec)
		{
			ty = conv_type::hexfp;
		}
		k.node_uleb(static_cast<::std::uint_least32_t>(field_param::type),
					static_cast<::std::uint_least32_t>(*ty));
		push(k);
		if (upper)
		{
			k.node_none(static_cast<::std::uint_least32_t>(field_param::flag_upper));
			push(k);
		}
		++i;
	}
	else if (tc == u8'n' || tc == u8'm' || tc == u8':')
	{
		// range presentation: n naked, m map, ':' default with element spec
		conv_type rt{tc == u8'n'   ? conv_type::rngn
					 : tc == u8'm' ? conv_type::rngm
								   : conv_type::rng};
		++i;
		if (i < n && spec[i] == u8':')
		{
			++i;
			// element spec follows — its own field-children grammar
			::fast_io::u8string_view elspec{spec.subview(i)};
			fmt_spec inner{parse_fmt_standard(elspec, ctx, depth + 1)};
			if (!inner.ok)
			{
				f.ok = false;
				return f;
			}
			encoder eb;
			for (auto const &e : inner.children)
			{
				eb.buf.append(e.data(), e.size());
			}
			k.node_bytes(static_cast<::std::uint_least32_t>(field_param::element),
						 ::fast_io::string_view{eb.buf.data(), eb.buf.size()});
			push(k);
			i = n;
		}
		else if (i != n)
		{
			f.ok = false;
			return f;
		}
		k.node_uleb(static_cast<::std::uint_least32_t>(field_param::type),
					static_cast<::std::uint_least32_t>(rt));
		push(k);
	}
	else
	{
		f.ok = false;
		return f;
	}
	if (i != n)
	{
		f.ok = false;
		return f;
	}
	return f;
}

inline ::std::size_t parse_fmt_field(::fast_io::u8string_view s, ::std::size_t pos,
									 encoder &b, unsigned depth, src_ctx ctx) FAST_IO_HERBCEPTIONS_THROWS
{
	if (depth > 8)
	{
		fail(ctx, s, pos, "nested format depth exceeded");
	}
	::std::size_t i{pos + 1};
	::std::size_t const n{s.size()};
	::fast_io::vector<::fast_io::string> children;
	encoder k;
	// arg id
	if (i < n && ::fast_io::char_category::is_c_digit(s[i]))
	{
		k.node_uleb(static_cast<::std::uint_least32_t>(field_param::arg),
					parse_u32(s, i, n, ctx, "arg index too large"));
		children.emplace_back(::std::move(k.buf));
	}
	else if (i < n && s[i] != u8':' && s[i] != u8'}' && s[i] != u8'!')
	{
		fail(ctx, s, i, "named args are not supported");
	}
	if (i < n && s[i] == u8'!')
	{
		fail(ctx, s, i, "!r/!a/!s conversions are not supported");
	}
	if (i < n && s[i] == u8':')
	{
		++i;
		::std::size_t const spec_begin{i};
		unsigned lvl{};
		for (; i < n; ++i)
		{
			if (s[i] == u8'{')
			{
				++lvl;
			}
			else if (s[i] == u8'}')
			{
				if (lvl == 0)
				{
					break;
				}
				--lvl;
			}
		}
		if (i == n)
		{
			fail(ctx, s, pos, "unterminated '{'");
		}
		::fast_io::u8string_view spec{s.subview(spec_begin, i - spec_begin)};
		fmt_spec f{parse_fmt_standard(spec, ctx, depth)};
		if (f.ok)
		{
			for (auto &e : f.children)
			{
				children.emplace_back(::std::move(e));
			}
		}
		else
		{
			// the whole spec is a chrono (pct) program
			auto blob{compile_pct_impl(
				spec, pct_options{.allow_nested_fmt = true},
				depth + 1, ctx)};
			encoder ck;
			ck.node_bytes(
				static_cast<::std::uint_least32_t>(field_param::chrono),
				::fast_io::string_view{blob.data(), blob.size()});
			children.emplace_back(::std::move(ck.buf));
		}
	}
	if (i >= n || s[i] != u8'}')
	{
		fail(ctx, s, i, "expected '}'");
	}
	::fast_io::vector<::fast_io::string_view> kidv;
	for (auto const &e : children)
	{
		kidv.emplace_back(e.data(), e.size());
	}
	b.node_list(op_field, ::fast_io::containers::span<::fast_io::string_view const>{kidv.data(), kidv.size()});
	return i + 1;
}

} // namespace details

// fmt.dev / std::format grammar
inline ::fast_io::string compile_fmt(::fast_io::u8string_view s, src_ctx ctx = {}) FAST_IO_HERBCEPTIONS_THROWS
{
	encoder b;
	details::litbuf lit;
	for (::std::size_t i{}; i < s.size();)
	{
		char8_t c{s[i]};
		if (c == u8'{')
		{
			if (i + 1 < s.size() && s[i + 1] == u8'{')
			{
				lit.push(u8'{');
				i += 2;
				continue;
			}
			lit.flush(b);
			i = details::parse_fmt_field(s, i, b, 0, ctx);
			continue;
		}
		if (c == u8'}')
		{
			if (i + 1 < s.size() && s[i + 1] == u8'}')
			{
				lit.push(u8'}');
				i += 2;
				continue;
			}
			fail(ctx, s, i, "unmatched '}'");
		}
		lit.push(c);
		++i;
	}
	lit.flush(b);
	return ::std::move(b.buf);
}

// strftime / chrono %-directive grammar (LC_TIME d_t_fmt, d_fmt, t_fmt,
// t_fmt_ampm, date_fmt, era_*_fmt, era fmt inside era entries)
inline ::fast_io::string compile_time(::fast_io::u8string_view s, src_ctx ctx = {}) FAST_IO_HERBCEPTIONS_THROWS
{
	return details::compile_pct_impl(s, {}, 0, ctx);
}

// generic %-letter slots (LC_NAME.name_fmt, LC_ADDRESS.postal_fmt,
// LC_TELEPHONE.tel_*_fmt): each %X letter is an argument selector, so it
// compiles to field{arg=letter} — the slot's consumer resolves letters
// to components. No %n/%t escapes, no strftime conv table: the letter IS
// the argument identity.
inline ::fast_io::string compile_generic_pct(::fast_io::u8string_view s,
											 src_ctx ctx = {}) FAST_IO_HERBCEPTIONS_THROWS
{
	encoder b;
	details::litbuf lit;
	for (::std::size_t i{}; i < s.size();)
	{
		char8_t c{s[i]};
		if (c != u8'%')
		{
			lit.push(c);
			++i;
			continue;
		}
		lit.flush(b);
		++i;
		if (i == s.size())
		{
			fail(ctx, s, i - 1, "dangling % at end of format");
		}
		char8_t c2{s[i]};
		++i;
		if (c2 == u8'%')
		{
			lit.push(u8'%');
			continue;
		}
		if (!::fast_io::char_category::is_c_alpha(c2))
		{
			fail(ctx, s, i - 1, "bad % directive in generic format");
		}
		// field{arg=letter} — letter codepoint is the arg identity
		encoder k;
		k.node_uleb(static_cast<::std::uint_least32_t>(field_param::arg), c2);
		::fast_io::string_view kv{k.buf.data(), k.buf.size()};
		b.node_list(op_field, ::fast_io::containers::span<::fast_io::string_view const>{__builtin_addressof(kv), 1});
	}
	lit.flush(b);
	return ::std::move(b.buf);
}

// ---------------------------------------------------------------------------
// printf front-end — compiles into the same field nodes as fmt
// ---------------------------------------------------------------------------

// target ABI for printf length-modifier resolution (compile-time target)
struct c_abi
{
	bool lp64{true};    // long/ptrdiff/size are 64-bit (LP64 vs LLP64)
	bool wchar32{true}; // wchar_t is 32-bit (Linux) vs 16 (Windows)
	bool ld80{true};    // long double is x87 80-bit (vs f128 on arm/ppc)
};

namespace details
{

inline ctype length_int_ctype(unsigned len_kind, bool u, c_abi abi) noexcept
{
	// len_kind: 0 none/h/hh (promote to i32), 1 l, 2 ll/j/z/t/q/w64, 3 L
	switch (len_kind)
	{
	case 0:
		return u ? ctype::u32 : ctype::i32;
	case 1:
		return abi.lp64 ? (u ? ctype::u64 : ctype::i64)
						: (u ? ctype::u32 : ctype::i32);
	case 2:
		return u ? ctype::u64 : ctype::i64;
	default:
		return u ? ctype::u128 : ctype::i128;
	}
}

} // namespace details

inline ::fast_io::string compile_stdio(::fast_io::u8string_view s, src_ctx ctx = {},
									   c_abi abi = {}) FAST_IO_HERBCEPTIONS_THROWS
{
	encoder b;
	details::litbuf lit;
	for (::std::size_t i{}; i < s.size();)
	{
		char8_t c{s[i]};
		if (c != u8'%')
		{
			lit.push(c);
			++i;
			continue;
		}
		lit.flush(b);
		++i;
		if (i == s.size())
		{
			fail(ctx, s, i, "dangling '%'");
		}
		if (s[i] == u8'%')
		{
			lit.push(u8'%');
			++i;
			continue;
		}
		::fast_io::vector<::fast_io::string> children;
		encoder k;
		auto push = [&children](encoder &e) FAST_IO_HERBCEPTIONS_THROWS {
			children.emplace_back(::std::move(e.buf));
		};
		// [argnum$]
		if (::fast_io::char_category::is_c_digit(s[i]))
		{
			::std::size_t j{i};
			while (j < s.size() && ::fast_io::char_category::is_c_digit(s[j]))
			{
				++j;
			}
			if (j < s.size() && s[j] == u8'$')
			{
				::std::uint_least32_t v{details::parse_u32(s, i, j, ctx, "arg index too large")};
				if (v == 0)
				{
					fail(ctx, s, i, "argnum$ is 1-based");
				}
				k.node_uleb(static_cast<::std::uint_least32_t>(field_param::arg), v - 1u);
				push(k);
				i = j + 1;
			}
		}
		// flags
		for (; i < s.size(); ++i)
		{
			switch (s[i])
			{
			case u8'-':
				k.node_uleb(static_cast<::std::uint_least32_t>(field_param::align), align_left);
				push(k);
				continue;
			case u8'+':
				k.node_uleb(static_cast<::std::uint_least32_t>(field_param::sign), sign_plus);
				push(k);
				continue;
			case u8' ':
				k.node_uleb(static_cast<::std::uint_least32_t>(field_param::sign), sign_space);
				push(k);
				continue;
			case u8'#':
				k.node_none(static_cast<::std::uint_least32_t>(field_param::flag_alt));
				push(k);
				continue;
			case u8'0':
				k.node_none(static_cast<::std::uint_least32_t>(field_param::flag_zero));
				push(k);
				continue;
			case u8'\'':
				k.node_none(static_cast<::std::uint_least32_t>(field_param::flag_group));
				push(k);
				continue;
			case u8'I':
				k.node_none(static_cast<::std::uint_least32_t>(field_param::flag_outdigits));
				push(k);
				continue;
			default:
				break;
			}
			break;
		}
		// width
		if (i < s.size() && s[i] == u8'*')
		{
			fail(ctx, s, i, "dynamic '*' width not supported");
		}
		if (i < s.size() && ::fast_io::char_category::is_c_digit(s[i]))
		{
			k.node_uleb(static_cast<::std::uint_least32_t>(field_param::width),
						details::parse_u32(s, i, s.size(), ctx, "width too large"));
			push(k);
		}
		// precision
		bool has_prec{};
		::std::uint_least32_t prec{};
		if (i < s.size() && s[i] == u8'.')
		{
			++i;
			has_prec = true;
			if (i < s.size() && s[i] == u8'*')
			{
				fail(ctx, s, i, "dynamic '*' precision not supported");
			}
			if (i < s.size() && ::fast_io::char_category::is_c_digit(s[i]))
			{
				prec = details::parse_u32(s, i, s.size(), ctx, "precision too large");
			}
			// bare '.' -> 0
		}
		// length
		unsigned len_kind{}; // 0 none/h/hh, 1 l, 2 ll/j/z/t/q/w, 3 L
		bool len_wide{};     // l before c/s
		if (i < s.size())
		{
			switch (s[i])
			{
			case u8'h':
				++i;
				if (i < s.size() && s[i] == u8'h')
				{
					++i;
				}
				break;
			case u8'l':
				++i;
				len_kind = 1;
				len_wide = true;
				if (i < s.size() && s[i] == u8'l')
				{
					++i;
					len_kind = 2;
				}
				break;
			case u8'j':
			case u8'z':
			case u8't':
			case u8'q':
				++i;
				len_kind = 2;
				break;
			case u8'L':
				++i;
				len_kind = 3;
				break;
			case u8'w':
			{
				++i;
				if (i < s.size() && s[i] == u8'f')
				{
					++i;
				}
				auto bits{details::parse_u32(s, i, s.size(), ctx, "w length bits too large")};
				len_kind = bits == 64 ? 2u : 0u; // w8/w16/w32 promote to int
				break;
			}
			default:
				break;
			}
		}
		if (i == s.size())
		{
			fail(ctx, s, i, "dangling conversion");
		}
		char8_t conv{s[i++]};
		switch (conv)
		{
		case u8'n':
		case u8'm':
		case u8'[':
			fail(ctx, s, i - 1, "unsafe conversion rejected");
		default:
			break;
		}
		bool upper{conv >= u8'A' && conv <= u8'Z'};
		ctype ct{ctype::other};
		conv_type ty{};
		switch (conv)
		{
		case u8'd':
		case u8'i':
			ty = conv_type::d;
			ct = details::length_int_ctype(len_kind, false, abi);
			break;
		case u8'u':
			ty = conv_type::u;
			ct = details::length_int_ctype(len_kind, true, abi);
			break;
		case u8'o':
			ty = conv_type::o;
			ct = details::length_int_ctype(len_kind, true, abi);
			break;
		case u8'x':
		case u8'X':
			ty = conv_type::x;
			ct = details::length_int_ctype(len_kind, true, abi);
			break;
		case u8'b':
		case u8'B':
			ty = conv_type::b;
			ct = details::length_int_ctype(len_kind, true, abi);
			break;
		case u8'f':
		case u8'F':
			ty = conv_type::fixp;
			if (!has_prec)
			{
				has_prec = true;
				prec = 6;
			}
			ct = len_kind == 3 ? (abi.ld80 ? ctype::f80 : ctype::f128)
							   : ctype::f64;
			break;
		case u8'e':
		case u8'E':
			ty = conv_type::scip;
			if (!has_prec)
			{
				has_prec = true;
				prec = 6;
			}
			ct = len_kind == 3 ? (abi.ld80 ? ctype::f80 : ctype::f128)
							   : ctype::f64;
			break;
		case u8'g':
		case u8'G':
			ty = conv_type::genp;
			if (!has_prec)
			{
				has_prec = true;
				prec = 6;
			}
			ct = len_kind == 3 ? (abi.ld80 ? ctype::f80 : ctype::f128)
							   : ctype::f64;
			break;
		case u8'a':
		case u8'A':
			ty = has_prec ? conv_type::hexfp : conv_type::hexf;
			ct = len_kind == 3 ? (abi.ld80 ? ctype::f80 : ctype::f128)
							   : ctype::f64;
			break;
		case u8'c':
			ty = conv_type::c;
			ct = len_wide ? (abi.wchar32 ? ctype::c32 : ctype::c16)
						  : ctype::c8;
			break;
		case u8'C': // glibc alias for %lc
			ty = conv_type::c;
			ct = abi.wchar32 ? ctype::c32 : ctype::c16;
			break;
		case u's':
			ty = conv_type::s;
			ct = len_wide ? (abi.wchar32 ? ctype::c32ptr : ctype::c16ptr)
						  : ctype::c8ptr;
			break;
		case u8'S': // glibc alias for %ls
			ty = conv_type::s;
			ct = abi.wchar32 ? ctype::c32ptr : ctype::c16ptr;
			break;
		case u8'p':
			ty = conv_type::p;
			ct = ctype::ptr;
			break;
		default:
			fail(ctx, s, i - 1, "unknown conversion");
		}
		if (has_prec)
		{
			k.node_uleb(static_cast<::std::uint_least32_t>(field_param::prec), prec);
			push(k);
		}
		if (ct != ctype::other)
		{
			k.node_uleb(static_cast<::std::uint_least32_t>(field_param::ctype),
						static_cast<::std::uint_least32_t>(ct));
			push(k);
		}
		k.node_uleb(static_cast<::std::uint_least32_t>(field_param::type),
					static_cast<::std::uint_least32_t>(ty));
		push(k);
		if (upper)
		{
			k.node_none(static_cast<::std::uint_least32_t>(field_param::flag_upper));
			push(k);
		}
		::fast_io::vector<::fast_io::string_view> kidv;
		for (auto const &e : children)
		{
			kidv.emplace_back(e.data(), e.size());
		}
		b.node_list(op_field, ::fast_io::containers::span<::fast_io::string_view const>{kidv.data(), kidv.size()});
	}
	lit.flush(b);
	return ::std::move(b.buf);
}

} // namespace binfmt
} // namespace fast_io_i18n

#include <fast_io_dsal/impl/misc/pop_macros.h>
