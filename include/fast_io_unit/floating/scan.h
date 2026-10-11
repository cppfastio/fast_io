#pragma once
/*
Floating-point scanning.

Grammar: optional spaces (unless noskipws), optional sign, then decimal
digits with an optional fractional point and an optional e/E exponent, a
hexadecimal 0x/0X sequence with optional point and optional p/P binary
exponent, or the spellings inf / infinity / nan when no digits were seen.
Significant digits beyond the per-format midpoint capacity (nibbles for
hex) are dropped with a sticky flag; the heavy conversion runs in the
fast_io.floating module (fp_scan_decimal / fp_scan_hex).  Like the
dangling e/E case, a 0x prefix with no hexadecimal digit behind it is
invalid mid-stream and scans as the value 0 at end of stream.
*/
namespace fast_io::details
{

// significant digits a midpoint expansion can reach for this format:
// digits of 5^(p - e_min) ~= (p + bias) * log10(5), plus slack
template <typename flt>
inline constexpr ::std::size_t scan_flt_digits_cap{
	((iec559_traits<flt>::mbits + 1u + ((1u << (iec559_traits<flt>::ebits - 1u)) - 1u)) * 45875u >>
	 16u) +
	8u};

// significant nibbles the hex scanner retains: enough that the kept
// sequence always carries more than p significand bits whenever a nibble
// had to be dropped, so the dropped part is a pure sticky flag
template <typename flt>
inline constexpr ::std::size_t scan_flt_hex_cap{(iec559_traits<flt>::mbits + 8u) / 4u};

enum class scan_floating_phase : ::std::uint_least8_t
{
	space,
	sign,
	digits,
	hex_digits,
	exp_sign,
	exp_digits,
	special,
	done
};

template <::std::integral char_type, typename flt>
struct scan_floating_context
{
	char buffer[scan_flt_digits_cap<flt>];
	::std::int_least64_t exponent{}; // parsed exponent magnitude (clamped)
	::std::size_t int_digits{};      // digits before the point (all of them)
	::std::size_t total_digits{};    // index across the combined int+frac stream
	::std::size_t first_sig{};       // combined index of the first nonzero digit
	::std::size_t ndigits{};         // retained significant digits
	scan_floating_phase phase{scan_floating_phase::space};
	::std::uint_least8_t special_idx{};
	bool negative{};
	bool exp_negative{};
	bool has_digit{};
	bool seen_point{};
	bool seen_sig{};
	bool sticky{};
	bool exp_has_digit{};
	bool exp_overflow{};
	bool hex{};

	inline constexpr void reset() noexcept
	{
		*this = {};
	}
};

// case-insensitive letter compare; the case delta is charset dependent
// (0x20 ASCII, 0x40 EBCDIC) so both spellings go through char_literal_v
template <char8_t lo, char8_t up, ::std::integral char_type>
inline constexpr bool scan_flt_ci(char_type ch) noexcept
{
	return ch == char_literal_v<lo, char_type> || ch == char_literal_v<up, char_type>;
}

// nibble value of a hexadecimal digit, -1 when the char is not one.
// '0'-'9', 'a'-'f' and 'A'-'F' are each contiguous in both ASCII and
// EBCDIC, so each range decodes against its own literal
template <::std::integral char_type>
inline constexpr ::std::int_least32_t scan_flt_hex_val(char_type ch) noexcept
{
	using utype = ::std::make_unsigned_t<char_type>;
	auto const c{static_cast<utype>(ch)};
	auto v{static_cast<utype>(c - static_cast<utype>(char_literal_v<u8'0', char_type>))};
	if (v <= 9u)
	{
		return static_cast<::std::int_least32_t>(v);
	}
	v = static_cast<utype>(c - static_cast<utype>(char_literal_v<u8'a', char_type>));
	if (v <= 5u)
	{
		return static_cast<::std::int_least32_t>(v + 10u);
	}
	v = static_cast<utype>(c - static_cast<utype>(char_literal_v<u8'A', char_type>));
	if (v <= 5u)
	{
		return static_cast<::std::int_least32_t>(v + 10u);
	}
	return -1;
}

// append one mantissa digit; the index runs over the combined int+frac
// stream and the buffer stores the digit value, not the character
template <::std::integral char_type, typename flt>
inline constexpr void scan_flt_digit(scan_floating_context<char_type, flt> &st, char_type ch) noexcept
{
	auto const idx{st.total_digits++};
	if (!st.seen_point)
	{
		++st.int_digits;
	}
	st.has_digit = true;
	auto const v{static_cast<char>(ch - char_literal_v<u8'0', char_type>)};
	if (!st.seen_sig)
	{
		if (!v)
		{
			return; // leading zero: position only
		}
		st.seen_sig = true;
		st.first_sig = idx;
	}
	if (st.ndigits < scan_flt_digits_cap<flt>)
	{
		st.buffer[st.ndigits++] = v;
	}
	else if (v)
	{
		st.sticky = true;
	}
}

// append one hexadecimal nibble (stored as its value, not the character)
template <::std::integral char_type, typename flt>
inline constexpr void scan_flt_hex_digit(scan_floating_context<char_type, flt> &st,
										 ::std::uint_least32_t v) noexcept
{
	auto const idx{st.total_digits++};
	if (!st.seen_point)
	{
		++st.int_digits;
	}
	st.has_digit = true;
	if (!st.seen_sig)
	{
		if (!v)
		{
			return; // leading zero nibble: position only
		}
		st.seen_sig = true;
		st.first_sig = idx;
	}
	if (st.ndigits < scan_flt_hex_cap<flt>)
	{
		st.buffer[st.ndigits++] = static_cast<char>(v);
	}
	else if (v)
	{
		st.sticky = true;
	}
}

// a "0x" prefix was consumed: discard the bookkeeping of the prefix zero
// and continue in hexadecimal mode
template <::std::integral char_type, typename flt>
inline constexpr void scan_flt_enter_hex(scan_floating_context<char_type, flt> &st) noexcept
{
	st.hex = true;
	st.phase = scan_floating_phase::hex_digits;
	st.int_digits = st.total_digits = st.first_sig = st.ndigits = 0;
	st.has_digit = st.seen_point = st.seen_sig = st.sticky = false;
}

// write a kernel result into the floating value; the kernel's lo/hi hold
// the full significand (implicit bit set for normals)
template <typename flt>
inline constexpr void scan_flt_assign(flt &v, fp_scan_result const &r, bool negative) noexcept
{
	using trait = iec559_traits<flt>;
	using mantissa_type = typename trait::mantissa_type;
	if constexpr (trait::mbits == 63u && trait::ebits == 15u && sizeof(flt) >= 10u)
	{
		// binary80: lo is the stored significand including the integer bit
		float80_storage<sizeof(flt) - sizeof(::std::uint_least64_t) - sizeof(::std::uint_least16_t)>
			st{r.lo,
			   static_cast<::std::uint_least16_t>((negative ? 0x8000u : 0u) |
												  static_cast<::std::uint_least16_t>(r.efield)),
			   {}};
		v = ::std::bit_cast<flt>(st);
	}
	else if constexpr (sizeof(mantissa_type) > sizeof(::std::uint_least64_t))
	{
		// binary128
		constexpr ::std::size_t mbits{trait::mbits};
		auto const word{
			(static_cast<mantissa_type>(negative) << (mbits + trait::ebits)) |
			(static_cast<mantissa_type>(static_cast<::std::uint_least32_t>(r.efield)) << mbits) |
			((static_cast<mantissa_type>(r.hi) << 64u | r.lo) &
			 ((static_cast<mantissa_type>(1) << mbits) - 1u))};
		v = ::std::bit_cast<flt>(word);
	}
	else
	{
		constexpr ::std::size_t mbits{trait::mbits};
		auto const word{static_cast<mantissa_type>(
			(static_cast<mantissa_type>(negative) << (mbits + trait::ebits)) |
			(static_cast<mantissa_type>(static_cast<::std::uint_least32_t>(r.efield)) << mbits) |
			(static_cast<mantissa_type>(r.lo) & ((static_cast<mantissa_type>(1) << mbits) - 1u)))};
		v = ::std::bit_cast<flt>(word);
	}
}

template <typename flt>
inline constexpr void scan_flt_assign_inf(flt &v, bool negative) noexcept
{
	using trait = iec559_traits<flt>;
	fp_scan_result r{};
	r.efield = static_cast<::std::int_least32_t>((1u << trait::ebits) - 1u);
	if constexpr (trait::mbits == 63u && trait::ebits == 15u && sizeof(flt) >= 10u)
	{
		r.lo = ::std::uint_least64_t{1} << 63u;
	}
	scan_flt_assign(v, r, negative);
}

template <typename flt>
inline constexpr void scan_flt_assign_nan(flt &v, bool negative) noexcept
{
	using trait = iec559_traits<flt>;
	using mantissa_type = typename trait::mantissa_type;
	fp_scan_result r{};
	r.efield = static_cast<::std::int_least32_t>((1u << trait::ebits) - 1u);
	if constexpr (trait::mbits == 63u && trait::ebits == 15u && sizeof(flt) >= 10u)
	{
		r.lo = ::std::uint_least64_t{3} << 62u; // int bit + quiet bit
	}
	else if constexpr (sizeof(mantissa_type) > sizeof(::std::uint_least64_t))
	{
		r.hi = ::std::uint_least64_t{1} << (trait::mbits - 65u); // quiet nan
	}
	else
	{
		r.lo = ::std::uint_least64_t{1} << (trait::mbits - 1u); // quiet nan
	}
	scan_flt_assign(v, r, negative);
}

// finalize: special values bypass the kernel, digits go through
// fp_scan_decimal in the floating module
template <::std::integral char_type, typename flt>
inline constexpr ::fast_io::freestanding::parse_errc
scan_flt_assign_result(scan_floating_context<char_type, flt> &st, flt &t) noexcept
{
	if (st.special_idx)
	{
		if (st.buffer[0] == 'n')
		{
			scan_flt_assign_nan(t, st.negative);
		}
		else
		{
			scan_flt_assign_inf(t, st.negative);
		}
		return ::fast_io::freestanding::parse_errc::ok;
	}
	if (st.hex)
	{
		if (!st.has_digit)
		{
			// bare "0x" at end of stream: the prefix zero is the value
			fp_scan_result const z{};
			scan_flt_assign(t, z, st.negative);
			return ::fast_io::freestanding::parse_errc::ok;
		}
		// V = (S + tail) * 2^e2, S the retained nibble sequence
		auto const e2{(st.exp_negative ? -st.exponent : st.exponent) +
					  4 * (static_cast<::std::int_least64_t>(st.int_digits) -
						   static_cast<::std::int_least64_t>(st.first_sig) -
						   static_cast<::std::int_least64_t>(st.ndigits))};
		using trait = iec559_traits<flt>;
		auto const r{fp_scan_hex(st.buffer, st.ndigits, e2, st.sticky,
								 static_cast<::std::uint_least32_t>(trait::mbits + 1u),
								 static_cast<::std::uint_least32_t>(trait::ebits))};
		scan_flt_assign(t, r, st.negative);
		return r.code ? ::fast_io::freestanding::parse_errc::overflow
					  : ::fast_io::freestanding::parse_errc::ok;
	}
	auto const e10{(st.exp_negative ? -st.exponent : st.exponent) +
				   static_cast<::std::int_least64_t>(st.int_digits) -
				   static_cast<::std::int_least64_t>(st.first_sig) -
				   static_cast<::std::int_least64_t>(st.ndigits)};
	using trait = iec559_traits<flt>;
	auto const r{fp_scan_decimal(st.buffer, st.ndigits, e10, st.sticky,
								 static_cast<::std::uint_least32_t>(trait::mbits + 1u),
								 static_cast<::std::uint_least32_t>(trait::ebits))};
	scan_flt_assign(t, r, st.negative);
	return r.code ? ::fast_io::freestanding::parse_errc::overflow
				  : ::fast_io::freestanding::parse_errc::ok;
}

// incremental parser over one buffer slice; returns where it stopped
template <bool noskipws, ::std::integral char_type, typename flt>
inline constexpr parse_result<char_type const *>
scan_flt_define_impl(scan_floating_context<char_type, flt> &st, char_type const *first,
					 char_type const *last, flt &t) noexcept
{
	constexpr auto partial{::fast_io::freestanding::parse_errc::partial};
	constexpr auto invalid{::fast_io::freestanding::parse_errc::invalid};
	for (;;)
	{
		switch (st.phase)
		{
		case scan_floating_phase::space:
		{
			if constexpr (!noskipws)
			{
				first = ::fast_io::details::find_space_common_impl<false, true>(first, last);
				if (first == last)
				{
					return {first, partial};
				}
			}
			st.phase = scan_floating_phase::sign;
			break;
		}
		case scan_floating_phase::sign:
		{
			if (first == last)
			{
				return {first, partial};
			}
			if (*first == char_literal_v<u8'-', char_type>)
			{
				st.negative = true;
				++first;
			}
			else if (*first == char_literal_v<u8'+', char_type>)
			{
				++first;
			}
			st.phase = scan_floating_phase::digits;
			break;
		}
		case scan_floating_phase::digits:
		{
			while (first != last)
			{
				auto const ch{*first};
				if (ch >= char_literal_v<u8'0', char_type> && ch <= char_literal_v<u8'9', char_type>)
				{
					scan_flt_digit(st, ch);
					++first;
					continue;
				}
				if (ch == char_literal_v<u8'.', char_type> && !st.seen_point)
				{
					st.seen_point = true;
					++first;
					continue;
				}
				if (!st.has_digit)
				{
					if (scan_flt_ci<u8'i', u8'I'>(ch) || scan_flt_ci<u8'n', u8'N'>(ch))
					{
						st.buffer[0] = scan_flt_ci<u8'i', u8'I'>(ch) ? 'i' : 'n';
						st.special_idx = 1;
						st.phase = scan_floating_phase::special;
						++first;
						break;
					}
					return {first, invalid};
				}
				if (ch == char_literal_v<u8'e', char_type> || ch == char_literal_v<u8'E', char_type>)
				{
					st.phase = scan_floating_phase::exp_sign;
					++first;
					break;
				}
				if (scan_flt_ci<u8'x', u8'X'>(ch) && st.total_digits == 1u && st.int_digits == 1u &&
					!st.seen_sig && !st.seen_point)
				{
					// "0x" prefix: hexadecimal scanning
					scan_flt_enter_hex(st);
					++first;
					break;
				}
				st.phase = scan_floating_phase::done;
				break;
			}
			if (first == last && st.phase == scan_floating_phase::digits)
			{
				return {first, partial};
			}
			break;
		}
		case scan_floating_phase::hex_digits:
		{
			while (first != last)
			{
				auto const ch{*first};
				auto const hv{scan_flt_hex_val(ch)};
				if (hv >= 0)
				{
					scan_flt_hex_digit(st, static_cast<::std::uint_least32_t>(hv));
					++first;
					continue;
				}
				if (ch == char_literal_v<u8'.', char_type> && !st.seen_point)
				{
					st.seen_point = true;
					++first;
					continue;
				}
				if (!st.has_digit)
				{
					return {first, invalid};
				}
				if (scan_flt_ci<u8'p', u8'P'>(ch))
				{
					st.phase = scan_floating_phase::exp_sign;
					++first;
					break;
				}
				st.phase = scan_floating_phase::done;
				break;
			}
			if (first == last && st.phase == scan_floating_phase::hex_digits)
			{
				return {first, partial};
			}
			break;
		}
		case scan_floating_phase::exp_sign:
		{
			if (first == last)
			{
				return {first, partial};
			}
			if (*first == char_literal_v<u8'-', char_type>)
			{
				st.exp_negative = true;
				++first;
			}
			else if (*first == char_literal_v<u8'+', char_type>)
			{
				++first;
			}
			st.phase = scan_floating_phase::exp_digits;
			break;
		}
		case scan_floating_phase::exp_digits:
		{
			while (first != last)
			{
				auto const ch{*first};
				if (ch >= char_literal_v<u8'0', char_type> && ch <= char_literal_v<u8'9', char_type>)
				{
					st.exp_has_digit = true;
					constexpr ::std::int_least64_t limit{1000000000};
					if (!st.exp_overflow)
					{
						auto const v{st.exponent * 10 +
									 static_cast<::std::int_least64_t>(ch - char_literal_v<u8'0', char_type>)};
						if (v > limit)
						{
							st.exponent = limit;
							st.exp_overflow = true;
						}
						else
						{
							st.exponent = v;
						}
					}
					++first;
					continue;
				}
				if (!st.exp_has_digit)
				{
					return {first, invalid};
				}
				st.phase = scan_floating_phase::done;
				break;
			}
			if (first == last && st.phase == scan_floating_phase::exp_digits)
			{
				return {first, partial};
			}
			break;
		}
		case scan_floating_phase::special:
		{
			// buffer[0] is 'i' ("nf" then optional "inity") or 'n' ("an")
			for (; first != last; ++first)
			{
				auto const ch{*first};
				if (st.buffer[0] == 'n')
				{
					constexpr char_type tail_lo[]{char_literal_v<u8'a', char_type>,
												  char_literal_v<u8'n', char_type>};
					constexpr char_type tail_up[]{char_literal_v<u8'A', char_type>,
												  char_literal_v<u8'N', char_type>};
					if (st.special_idx < 3u &&
						(ch == tail_lo[st.special_idx - 1u] || ch == tail_up[st.special_idx - 1u]))
					{
						++st.special_idx;
						continue;
					}
					if (st.special_idx < 3u)
					{
						return {first, invalid};
					}
					st.phase = scan_floating_phase::done;
					break;
				}
				constexpr char_type tail_lo[]{char_literal_v<u8'n', char_type>,
											  char_literal_v<u8'f', char_type>,
											  char_literal_v<u8'i', char_type>,
											  char_literal_v<u8'n', char_type>,
											  char_literal_v<u8'i', char_type>,
											  char_literal_v<u8't', char_type>,
											  char_literal_v<u8'y', char_type>};
				constexpr char_type tail_up[]{char_literal_v<u8'N', char_type>,
											  char_literal_v<u8'F', char_type>,
											  char_literal_v<u8'I', char_type>,
											  char_literal_v<u8'N', char_type>,
											  char_literal_v<u8'I', char_type>,
											  char_literal_v<u8'T', char_type>,
											  char_literal_v<u8'Y', char_type>};
				if (st.special_idx < 8u &&
					(ch == tail_lo[st.special_idx - 1u] || ch == tail_up[st.special_idx - 1u]))
				{
					++st.special_idx;
					continue;
				}
				if (st.special_idx == 3u || st.special_idx == 8u)
				{
					// "inf" or "infinity" complete; the next char is a terminator
					st.phase = scan_floating_phase::done;
					break;
				}
				// partial "inity" is malformed
				return {first, invalid};
			}
			if (st.phase != scan_floating_phase::done)
			{
				if (first == last)
				{
					return {first, partial};
				}
				break;
			}
			break;
		}
		default:
		{
			return {first, scan_flt_assign_result(st, t)};
		}
		}
	}
}

// finalize at end-of-stream
template <bool noskipws, ::std::integral char_type, typename flt>
inline constexpr ::fast_io::freestanding::parse_errc
scan_flt_eof_impl(scan_floating_context<char_type, flt> &st, flt &t) noexcept
{
	constexpr auto invalid{::fast_io::freestanding::parse_errc::invalid};
	switch (st.phase)
	{
	case scan_floating_phase::space:
	case scan_floating_phase::sign:
		return ::fast_io::freestanding::parse_errc::end_of_file;
	case scan_floating_phase::digits:
		if (!st.has_digit)
		{
			return invalid;
		}
		return scan_flt_assign_result(st, t);
	case scan_floating_phase::hex_digits:
		// a bare "0x" scans as the value 0 at end of stream
		return scan_flt_assign_result(st, t);
	case scan_floating_phase::exp_sign:
	case scan_floating_phase::exp_digits:
		// dangling e / e± at end of stream: parse the mantissa only
		return st.has_digit ? scan_flt_assign_result(st, t) : invalid;
	case scan_floating_phase::special:
		if (st.buffer[0] == 'n' ? st.special_idx == 3u
								: st.special_idx == 3u || st.special_idx == 8u)
		{
			return scan_flt_assign_result(st, t);
		}
		return invalid;
	default:
		return scan_flt_assign_result(st, t);
	}
}

} // namespace fast_io::details

namespace fast_io
{

template <details::my_floating_point T>
inline constexpr ::fast_io::manipulators::scalar_manip_t<
	::fast_io::details::base_scan_mani_flags_cache<10, false, false, false>, T &>
scan_alias_define(io_alias_t, T &t) noexcept
{
	return {t};
}

template <::std::integral char_type, manipulators::scalar_flags flags, details::my_floating_point T>
	requires(flags.base == 10)
inline constexpr auto scan_context_type(
	io_reserve_type_t<char_type, ::fast_io::manipulators::scalar_manip_t<flags, T &>>) noexcept
{
	return io_type_t<details::scan_floating_context<char_type, ::std::remove_cvref_t<T>>>{};
}

template <::std::integral char_type, manipulators::scalar_flags flags, details::my_floating_point T>
	requires(flags.base == 10)
inline constexpr parse_result<char_type const *>
scan_contiguous_define(io_reserve_type_t<char_type, ::fast_io::manipulators::scalar_manip_t<flags, T &>>,
					   char_type const *begin, char_type const *end,
					   ::fast_io::manipulators::scalar_manip_t<flags, T &> t) noexcept
{
	details::scan_floating_context<char_type, ::std::remove_cvref_t<T>> st;
	auto ret{details::scan_flt_define_impl<flags.noskipws>(st, begin, end, t.reference)};
	if (ret.code == ::fast_io::freestanding::parse_errc::partial)
	{
		ret.code = details::scan_flt_eof_impl<flags.noskipws>(st, t.reference);
		ret.iter = end;
	}
	return ret;
}

template <::std::integral char_type, manipulators::scalar_flags flags, typename State,
		  details::my_floating_point T>
	requires(flags.base == 10)
inline constexpr parse_result<char_type const *>
scan_context_define(io_reserve_type_t<char_type, ::fast_io::manipulators::scalar_manip_t<flags, T &>>,
					State &state, char_type const *begin, char_type const *end,
					::fast_io::manipulators::scalar_manip_t<flags, T &> t) noexcept
{
	return details::scan_flt_define_impl<flags.noskipws>(state, begin, end, t.reference);
}

template <::std::integral char_type, manipulators::scalar_flags flags, typename State,
		  details::my_floating_point T>
	requires(flags.base == 10)
inline constexpr ::fast_io::freestanding::parse_errc
scan_context_eof_define(io_reserve_type_t<char_type, ::fast_io::manipulators::scalar_manip_t<flags, T &>>,
						State &state, ::fast_io::manipulators::scalar_manip_t<flags, T &> t) noexcept
{
	return details::scan_flt_eof_impl<flags.noskipws>(state, t.reference);
}

} // namespace fast_io
