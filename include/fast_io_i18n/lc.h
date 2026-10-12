#pragma once


// lc — the runtime locale context every lc_* print hook is keyed on.
// Master's hooks take `basic_lc_all<char_type> const*`; here the key is
// lc_ctx: the lc_locale file pointer (needed to resolve the wire
// format's RVAs) plus the selected section image plus the hot
// LC_NUMERIC fields decoded once per imbue.
//
// Field access in ported code is mechanical:
//   master:  all->numeric.thousands_sep          (basic_io_scatter_t)
//   here:    ctx->sc(ctx->all->numeric.thousands_sep)
//            ^ sc resolves the wire lc_scatter to a real scatter
//   master:  all->time.era.base                  (pointer)
//   here:    ctx->pt(ctx->all->time.era.ref)     (lc_rva -> pointer)

namespace fast_io
{

inline constexpr ::std::size_t lc_sep_capacity{16};

template <::std::integral char_type>
struct lc_ctx
{
	::fast_io::l10n::lc_locale const *loc{};
	// the section image — wire-layout identical for every char_type,
	// so it is read through the char instantiation
	::fast_io::l10n::basic_lc_all<char> const *all{};
	// hot LC_NUMERIC fields decoded once per imbue: thousands_sep as
	// char_type units and grouping as u8 group sizes (both capped — a
	// longer field disables grouping rather than allocating)
	char_type sep[lc_sep_capacity]{};
	::std::size_t sep_len{};
	char8_t grouping[lc_sep_capacity]{};
	::std::size_t grouping_len{};

	// resolve a wire scatter member to a real scatter of char_type
	// units — the member's declared T is the charset tag (char for
	// every string field); the actual units are this section's
	// char_type. rva==0 gives {nullptr,0}
	template <typename T>
	inline constexpr ::fast_io::basic_io_scatter_t<char_type> sc(
		::fast_io::l10n::lc_scatter<T> s) const FAST_IO_HERBCEPTIONS_THROWS
	{
		auto const r{::fast_io::l10n::lc_get_scatter(loc, s)};
		return {reinterpret_cast<char_type const *>(r.base), r.len};
	}
	// resolve a wire rva member to a pointer; rva==0 gives nullptr
	template <typename T>
	inline constexpr T const *pt(::fast_io::l10n::lc_rva<T> r) const
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return ::fast_io::l10n::lc_get_rva(loc, r);
	}
	// a strref-table element of a dynamic string list (alt_digits and
	// friends): member is {tbl_rva,count}, tbl = lc_scatter[count]
	template <typename T>
	inline constexpr ::fast_io::basic_io_scatter_t<char_type> sc_elem(
		::fast_io::l10n::lc_scatter<::fast_io::l10n::lc_scatter<T>> s,
		::std::size_t i) const FAST_IO_HERBCEPTIONS_THROWS
	{
		// the table itself is bounded first — off + len*sizeof covers
		// the whole element array, so tbl[i] is always inside the image
		auto const tbl{::fast_io::l10n::lc_get_scatter(loc, s)};
		if (tbl.base == nullptr || i >= tbl.len)
		{
			return {};
		}
		auto const e{::fast_io::l10n::lc_get_scatter(loc, tbl.base[i])};
		return {reinterpret_cast<char_type const *>(e.base), e.len};
	}
};

template <::std::integral char_type>
inline lc_ctx<char_type>
lc_load_ctx(::fast_io::l10n::lc_locale const *v)
	FAST_IO_HERBCEPTIONS_THROWS
{
	namespace lc = ::fast_io::l10n;
	lc_ctx<char_type> ctx{};
	auto const *all{lc::lc_get_all<char_type>(v)};
	if (all == nullptr)
	{
		return ctx;
	}
	ctx.loc = v;
	ctx.all = reinterpret_cast<lc::basic_lc_all<char> const *>(all);
	// payload units already are char_type — no codecvt anywhere
	if (auto const sep{lc::lc_get_scatter(v, all->numeric.thousands_sep)};
		sep.base != nullptr && sep.len != 0)
	{
		if (sep.len > lc_sep_capacity)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::invalid_argument);
		}
		::fast_io::details::my_memcpy(ctx.sep, sep.base, sep.len * sizeof(char_type));
		ctx.sep_len = sep.len;
	}
	if (auto const g{lc::lc_get_scatter(v, all->numeric.grouping)};
		g.base != nullptr && g.len != 0)
	{
		if (g.len > lc_sep_capacity)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::invalid_argument);
		}
		::fast_io::details::my_memcpy(ctx.grouping, g.base, g.len);
		ctx.grouping_len = g.len;
	}
	return ctx;
}


namespace details
{

// glibc grouping semantics: entry gi is the group size for group gi
// counting from the right; past the end the last value repeats; a 0
// repeats the previous value; 0xFF (CHAR_MAX) or 0-with-no-previous
// stops grouping entirely
template <::std::integral char_type>
inline ::std::uint_least8_t lc_group_size(lc_ctx<char_type> const *ctx,
										  ::std::size_t gi,
										  ::std::uint_least8_t prev) noexcept
{
	if (gi < ctx->grouping_len) [[likely]]
	{
		auto g{static_cast<::std::uint_least8_t>(ctx->grouping[gi])};
		if (g == 0xFFu) [[unlikely]]
		{
			return 0;
		}
		if (g != 0)
		{
			return g;
		}
		return prev;
	}
	return prev;
}


// the count and write sinks every lc_* emitter shares — count and
// write passes run the same code through these, so they can never
// disagree
template <::std::integral char_type>
struct lc_count_sink
{
	::std::size_t n{};
	inline constexpr void put(char_type) noexcept
	{
		++n;
	}
	inline constexpr void put_units(char_type const *, ::std::size_t len) noexcept
	{
		n += len;
	}
};

template <::std::integral char_type>
struct lc_write_sink
{
	char_type *it{}, *dend{};
	inline constexpr void put(char_type ch) noexcept
	{
		if (it != dend) [[likely]]
		{
			*it = ch;
			++it;
		}
	}
	inline constexpr void put_units(char_type const *p, ::std::size_t len) noexcept
	{
		auto const n{::std::min(len, static_cast<::std::size_t>(dend - it))};
		::fast_io::details::my_memcpy(it, p, n * sizeof(char_type));
		it += n;
	}
};

} // namespace details

} // namespace fast_io

