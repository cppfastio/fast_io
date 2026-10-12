#pragma once


namespace fast_io
{

// std::chrono::weekday / month — localized name scatters, same as
// master/lc_numbers/chrono.h but keyed on lc_ctx
template <::std::integral char_type>
inline constexpr ::fast_io::basic_io_scatter_t<char_type>
print_scatter_define(lc_ctx<char_type> const *ctx,
					 ::std::chrono::weekday wd) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!wd.ok())
	{
		return {};
	}
	return ctx->sc(ctx->all->time.day[wd.c_encoding()]);
}

template <::std::integral char_type>
inline constexpr ::fast_io::basic_io_scatter_t<char_type>
print_scatter_define(lc_ctx<char_type> const *ctx,
					 ::std::chrono::weekday_indexed wdi) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!wdi.ok())
	{
		return {};
	}
	return ctx->sc(ctx->all->time
					   .day[static_cast<::std::size_t>(wdi.weekday().c_encoding())]);
}

template <::std::integral char_type>
inline constexpr ::fast_io::basic_io_scatter_t<char_type>
print_scatter_define(lc_ctx<char_type> const *ctx,
					 ::std::chrono::weekday_last wdl) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!wdl.ok())
	{
		return {};
	}
	return ctx->sc(ctx->all->time
					   .day[static_cast<::std::size_t>(wdl.weekday().c_encoding())]);
}

template <::std::integral char_type>
inline constexpr ::fast_io::basic_io_scatter_t<char_type>
print_scatter_define(lc_ctx<char_type> const *ctx,
					 ::std::chrono::month m) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!m.ok())
	{
		return {};
	}
	return ctx->sc(ctx->all->time
					   .mon[static_cast<::std::size_t>(static_cast<unsigned>(m)) - 1]);
}

// unimbued forms — C-locale day/month names written through
// char_literal so every exec charset is correct. These satisfy
// print_freestanding_okay so the imbued dispatch can see the arg; they
// also give println(out, weekday) the POSIX result
namespace details
{

template <::std::integral char_type>
inline constexpr char_type *lc_put_cname(char_type *iter,
										 ::fast_io::u8string_view s) noexcept
{
	for (char8_t ch : s)
	{
		*iter++ = ::fast_io::char_literal<char_type>(ch);
	}
	return iter;
}

template <::std::integral char_type>
inline constexpr ::fast_io::u8string_view lc_c_wday(::std::size_t i) noexcept
{
	return i < 7 ? ::fast_io::details::lc_c_day[i] : ::fast_io::u8string_view{};
}

template <::std::integral char_type>
inline constexpr ::fast_io::u8string_view lc_c_mon_i(::std::size_t i) noexcept
{
	return i < 12 ? ::fast_io::details::lc_c_mon[i] : ::fast_io::u8string_view{};
}

template <typename T>
concept lc_c_nameable = ::std::same_as<T, ::std::chrono::weekday> ||
	::std::same_as<T, ::std::chrono::weekday_indexed> ||
	::std::same_as<T, ::std::chrono::weekday_last> ||
	::std::same_as<T, ::std::chrono::month>;

template <typename T>
inline constexpr ::fast_io::u8string_view lc_c_name(T v) noexcept
{
	if constexpr (::std::same_as<T, ::std::chrono::month>)
	{
		return lc_c_mon_i<char>(
			static_cast<::std::size_t>(static_cast<unsigned>(v)) - 1);
	}
	else if constexpr (::std::same_as<T, ::std::chrono::weekday>)
	{
		return lc_c_wday<char>(v.c_encoding());
	}
	else
	{
		return lc_c_wday<char>(
			static_cast<::std::size_t>(v.weekday().c_encoding()));
	}
}

} // namespace details

template <::std::integral char_type, ::fast_io::details::lc_c_nameable T>
inline constexpr ::std::size_t
print_reserve_size(::fast_io::io_reserve_type_t<char_type, T>, T v) noexcept
{
	return ::fast_io::details::lc_c_name(v).size();
}

template <::std::integral char_type, ::fast_io::details::lc_c_nameable T>
inline constexpr char_type *
print_reserve_define(::fast_io::io_reserve_type_t<char_type, T>,
					 char_type *iter, T v) noexcept
{
	return ::fast_io::details::lc_put_cname<char_type>(
		iter, ::fast_io::details::lc_c_name(v));
}

} // namespace fast_io

