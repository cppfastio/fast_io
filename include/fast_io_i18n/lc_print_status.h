#pragma once


// lc_print_status — the locale-aware print concepts, keyed on lc_ctx
// (the all-equivalent: file pointer + section image + hot fields).
// Master's hooks took basic_lc_all<char_type> const*; these take
// lc_ctx<char_type> const* and the wire fields resolve through
// ctx->sc/ctx->pt.

namespace fast_io
{

template <typename char_type, typename T>
concept lc_scatter_printable = ::std::integral<char_type> &&
	requires(lc_ctx<char_type> const *ctx, T t) {
		{
			print_scatter_define(ctx, t)
		} -> ::std::same_as<::fast_io::basic_io_scatter_t<char_type>>;
	};

template <typename char_type, typename T>
concept lc_dynamic_reserve_printable = ::std::integral<char_type> &&
	requires(lc_ctx<char_type> const *ctx, T t, char_type *ptr, ::std::size_t size) {
		{ print_reserve_size(ctx, t) } -> ::std::convertible_to<::std::size_t>;
		{ print_reserve_define(ctx, ptr, t) } -> ::std::convertible_to<char_type *>;
	};

template <typename char_type, typename T>
concept lc_printable = ::std::integral<char_type> &&
	requires(lc_ctx<char_type> const *ctx,
			 ::fast_io::details::dummy_buffer_output_stream<char_type> out, T t) {
		print_define(ctx, out, t);
	};

// any of the three lc-hook shapes exists for T in char_type
template <typename char_type, typename T>
concept lc_any_printable = lc_scatter_printable<char_type, T> ||
	lc_dynamic_reserve_printable<char_type, T> ||
	lc_printable<char_type, T>;

// write one lc arg to the stream — scatter straight out of the image
// when the hook gives one, the obuffer reserve fast path for
// dynamic-reserve hooks, print_define otherwise
namespace details
{

template <typename output, typename T>
	requires lc_any_printable<typename output::output_char_type, T>
inline constexpr void lc_write_lc(lc_ctx<typename output::output_char_type> const &ctx,
								  output out, T &&t) FAST_IO_HERBCEPTIONS_THROWS
{
	using char_type = typename output::output_char_type;
	using value_type = ::std::remove_cvref_t<T>;
	if constexpr (lc_scatter_printable<char_type, value_type>)
	{
		auto const s{print_scatter_define(__builtin_addressof(ctx), t)};
		if (s.base != nullptr && s.len != 0)
		{
			::fast_io::operations::print_freestanding<false>(out, s);
		}
	}
	else if constexpr (lc_dynamic_reserve_printable<char_type, value_type>)
	{
		auto const *cp{__builtin_addressof(ctx)};
		::std::size_t const need{print_reserve_size(cp, t)};
		if constexpr (::fast_io::operations::decay::defines::
						  has_obuffer_basic_operations<output>)
		{
			auto const curr{obuffer_curr(out)};
			auto const ed{obuffer_end(out)};
			auto const diff{ed - curr};
			if (0 <= diff && need <= static_cast<::std::size_t>(diff)) [[likely]]
			{
				obuffer_set_curr(out, print_reserve_define(cp, curr, t));
				return;
			}
			else if constexpr (::fast_io::operations::decay::defines::
								   has_obuffer_flush_reserve_define<output>)
			{
				obuffer_flush_reserve_define(out, need);
				auto const ncurr{obuffer_curr(out)};
				obuffer_set_curr(out, print_reserve_define(cp, ncurr, t));
				return;
			}
		}
		// non-obuffer output — small reserves go through the stack
		// buffer; anything larger needs a scratch allocation sized by
		// the count pass (the reserve hooks write the full count, so a
		// too-small destination is an overflow, not a truncation)
		constexpr ::std::size_t cap{512};
		if (need <= cap)
		{
			char_type buf[cap];
			auto const it{print_reserve_define(cp, buf, t)};
			::fast_io::operations::print_freestanding<false>(
				out,
				::fast_io::basic_io_scatter_t<char_type>{
					buf, static_cast<::std::size_t>(it - buf)});
		}
		else
		{
			using print_allocator_type =
				::fast_io::operations::decay::output_stream_allocator_t<output>;
			using print_typed_allocator_type =
				::fast_io::typed_generic_allocator_adapter<print_allocator_type, char_type>;
			::fast_io::details::buffer_alloc_arr_ptr<char_type, false,
													 print_allocator_type>
				scratch;
			char_type *base;
			if constexpr (print_typed_allocator_type::has_status)
			{
				base = scratch.allocate_new(
					::fast_io::details::print_output_stream_allocator_handle<
						output, print_typed_allocator_type>(out),
					need);
			}
			else
			{
				base = scratch.allocate_new(need);
			}
			auto const it{print_reserve_define(cp, base, t)};
			::fast_io::operations::print_freestanding<false>(
				out,
				::fast_io::basic_io_scatter_t<char_type>{
					base, static_cast<::std::size_t>(it - base)});
		}
	}
	else
	{
		print_define(__builtin_addressof(ctx), out, t);
	}
}

} // namespace details


namespace l10n::details
{

// index of the first arg with an lc hook; sizeof...(Args) when none —
// same template-for consteval pattern as
// details::first_print_define_index_range in the plain path
template <typename char_type, typename... Args>
inline consteval ::std::size_t lc_first_index() noexcept
{
	template for (constexpr auto i : ::fast_io::details::index_array_range<0, sizeof...(Args)>)
	{
		if constexpr (::fast_io::lc_any_printable<char_type, ::std::remove_cvref_t<Args...[i]>>)
		{
			return i;
		}
	}
	return sizeof...(Args);
}

template <typename char_type, typename... Args>
inline constexpr bool lc_any_arg_v{lc_first_index<char_type, Args...>() != sizeof...(Args)};

// args are split at every lc arg: contiguous non-lc segments go through
// print_freestanding_decay (the plain path keeps its reserve / scatter
// batching inside each segment), lc args write through their hooks on
// ctx. The terminal call delegates to the plain path — it also emits
// '\n'.
template <bool line, typename output, typename... Args>
inline constexpr void lc_status_print_impl(lc_ctx<typename output::output_char_type> const &ctx,
										   output optstm, Args... args) FAST_IO_HERBCEPTIONS_THROWS
{
	using char_type = typename output::output_char_type;
	constexpr ::std::size_t fpos{lc_first_index<char_type, Args...>()};
	if constexpr (fpos == sizeof...(Args))
	{
		return ::fast_io::operations::decay::print_freestanding_decay<line>(optstm, args...);
	}
	else
	{
		if constexpr (fpos != 0)
		{
			[&]<::std::size_t... pos>(::std::index_sequence<pos...>) FAST_IO_HERBCEPTIONS_THROWS {
				::fast_io::operations::decay::print_freestanding_decay<false>(optstm, args...[pos]...);
			}(::std::make_index_sequence<fpos>{});
		}
		::fast_io::details::lc_write_lc(ctx, optstm, args...[fpos]);
		return [&]<::std::size_t... pos>(::std::index_sequence<pos...>) FAST_IO_HERBCEPTIONS_THROWS {
			return lc_status_print_impl<line>(ctx, optstm, args...[fpos + 1 + pos]...);
		}(::std::make_index_sequence<sizeof...(Args) - fpos - 1>{});
	}
}

} // namespace l10n::details

} // namespace fast_io

