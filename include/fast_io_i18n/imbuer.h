#pragma once


// locale-aware printing. imbue(loc, out) wraps an output stream so that
// locale-printable arguments format through the mapped locale:
//   ::fast_io::println(::fast_io::imbue(loc, obf), 1242141242124);
//   ::fast_io::println(::fast_io::imbue(loc, obf), ::fast_io::mnp::d_t_fmt(ts));
//   ::fast_io::println(::fast_io::imbue(loc, obf), ::fast_io::mnp::boolalpha(b));
//
// The locale handle is a ::fast_io::l10n::lc_locale const* — an
// lc_ctx built from it at imbue time selects the section for the
// stream's char_type: char follows the file's declared codeset
// (UTF-8/GB18030/UTF-EBCDIC), char8_t/char16_t/char32_t select their
// own always-present sections.

namespace fast_io
{

template <typename stm>
struct lc_imbuer
{
	using handle_type = stm;
	using output_char_type = typename handle_type::output_char_type;
	using char_type = output_char_type;
	lc_ctx<char_type> ctx{};
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	handle_type handle{};
};

template <typename stm>
inline constexpr lc_imbuer<stm> output_stream_ref_define(lc_imbuer<stm> t) noexcept
{
	return t;
}

template <typename stm>
	requires(::std::is_lvalue_reference_v<stm> || ::std::is_trivially_copyable_v<stm>)
inline constexpr auto imbue(::fast_io::l10n::lc_locale const *loc,
							stm &&out)
	FAST_IO_HERBCEPTIONS_THROWS
{
	using reftype = decltype(::fast_io::operations::output_stream_ref(out));
	using char_type = typename reftype::output_char_type;
	return lc_imbuer<reftype>{::fast_io::lc_load_ctx<char_type>(loc),
							  ::fast_io::operations::output_stream_ref(out)};
}



template <bool line, typename output, typename... Args>
inline constexpr void status_print_define(::fast_io::lc_imbuer<output> imb, Args... args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (::fast_io::l10n::details::lc_any_arg_v<
					  typename lc_imbuer<output>::char_type, Args...>)
	{
		return ::fast_io::l10n::details::lc_status_print_impl<line>(imb.ctx, imb.handle,
																  args...);
	}
	else
	{
		return ::fast_io::operations::decay::print_freestanding_decay<line>(imb.handle, args...);
	}
}

#if defined(__HERBCEPTIONS__)
/* the async counterpart of status_print_define: locale formatting is
   synchronous by design (the locale hooks are pure computation), so it
   runs eagerly into owned storage and the ordinary async print path
   submits the bytes. Args carrying no locale state forward straight to
   the plain async path — no copy, no string */
template <bool line, typename schedulertype, typename output, typename... Args>
inline auto async_status_print_define(schedulertype sched,
									  ::fast_io::posix_statx_timestamp_opt timeout,
									  ::fast_io::lc_imbuer<output> imb, Args... args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	using char_type = typename lc_imbuer<output>::char_type;
	if constexpr (::fast_io::l10n::details::lc_any_arg_v<char_type, Args...>)
	{
		::fast_io::containers::basic_string<char_type, ::fast_io::native_global_allocator>
			payload;
		::fast_io::l10n::details::lc_status_print_impl<line>(
			imb.ctx, io_strlike_ref(::fast_io::io_alias, payload), args...);
		return ::fast_io::operations::decay::async_print_decay<false>(
			sched, timeout, imb.handle,
			::fast_io::mnp::strvw(payload));
	}
	else
	{
		return ::fast_io::operations::decay::async_print_decay<line>(
			sched, timeout, imb.handle, args...);
	}
}
#endif

} // namespace fast_io

