#pragma once

namespace fast_io::char_category
{

enum class char_category_family : ::std::uint_least32_t
{
	c_alnum,                    // Alphanumeric characters (letters and digits)
	c_alpha,                    // Alphabetic Character
	c_blank,                    // Space or tab
	c_cntrl,                    // Control characters (ASCII 0x00-0x1F, 0x7F or EBCDIC equivalents)
	c_digit,                    // Numeric digits (0-9)
	c_fullwidth,                // Full-width character
	c_graph,                    // Graphical characters (alphanumeric + punctuation)
	c_halfwidth,                // Half-width character
	c_lower,                    // Lowercase alphabetic characters
	c_print,                    // Printable characters (includes space)
	c_punct,                    // Punctuation characters
	c_space,                    // Whitespace characters (space, tab, newline, etc.)
	c_upper,                    // Uppercase alphabetic characters
	c_xdigit,                   // Hexadecimal digits (0-9, A-F, a-f)
	dos_path_invalid_character, // DOS Path invalid character
	html_whitespace             // HTML whitespace
};

template <::fast_io::char_category::char_category_family fam, bool negate>
	requires(static_cast<std::underlying_type_t<::fast_io::char_category::char_category_family>>(fam) <=
			 static_cast<std::underlying_type_t<::fast_io::char_category::char_category_family>>(
				 ::fast_io::char_category::char_category_family::html_whitespace))
class char_category_traits
{
public:
	static inline constexpr ::fast_io::char_category::char_category_family family{fam};
	static inline constexpr bool is_negate{negate};
	template <::std::integral char_type>
	static inline constexpr bool char_is(char_type ch) noexcept
	{
		bool ret;
		if constexpr (fam == ::fast_io::char_category::char_category_family::c_alpha)
		{
			ret = ::fast_io::char_category::is_c_alpha(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_blank)
		{
			ret = ::fast_io::char_category::is_c_blank(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_punct)
		{
			ret = ::fast_io::char_category::is_c_punct(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_graph)
		{
			ret = ::fast_io::char_category::is_c_graph(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_print)
		{
			ret = ::fast_io::char_category::is_c_print(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_xdigit)
		{
			ret = ::fast_io::char_category::is_c_xdigit(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_cntrl)
		{
			ret = ::fast_io::char_category::is_c_cntrl(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_alnum)
		{
			ret = ::fast_io::char_category::is_c_alnum(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_lower)
		{
			ret = ::fast_io::char_category::is_c_lower(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_upper)
		{
			ret = ::fast_io::char_category::is_c_upper(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_digit)
		{
			ret = ::fast_io::char_category::is_c_digit(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_space)
		{
			ret = ::fast_io::char_category::is_c_space(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::html_whitespace)
		{
			ret = ::fast_io::char_category::is_html_whitespace(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_halfwidth)
		{
			ret = ::fast_io::char_category::is_c_halfwidth(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_fullwidth)
		{
			ret = ::fast_io::char_category::is_c_fullwidth(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::dos_path_invalid_character)
		{
			ret = ::fast_io::char_category::is_dos_path_invalid_character(ch);
		}
		if constexpr (negate)
		{
			return !ret;
		}
		else
		{
			return ret;
		}
	}
	template <::std::integral char_type>
#ifdef __cpp_static_call_operator
	static
#endif
		inline constexpr bool operator()(char_type ch) noexcept
	{
		return char_is(ch);
	}

	template <::std::forward_iterator Iter>
	static inline constexpr Iter find(Iter first, Iter last) noexcept
	{
		for (; first != last && !char_is(*first); ++first)
		{
		}
		return first;
	}

	template <::std::forward_iterator Iter>
	static inline constexpr Iter find_not(Iter first, Iter last) noexcept
	{
		for (; first != last && char_is(*first); ++first)
		{
		}
		return first;
	}

	template <::std::bidirectional_iterator Iter>
	static inline constexpr Iter find_last(Iter first, Iter last) noexcept
	{
		while (first != last)
		{
			auto lastprev{last};
			--lastprev;
			if (char_is(*lastprev))
			{
				return last;
			}
			last = lastprev;
		}
		return last;
	}

	template <::std::bidirectional_iterator Iter>
	static inline constexpr Iter find_last_not(Iter first, Iter last) noexcept
	{
		while (first != last)
		{
			auto lastprev{last};
			--lastprev;
			if (!char_is(*lastprev))
			{
				return last;
			}
			last = lastprev;
		}
		return last;
	}
	template <::std::forward_iterator Iter>
	static inline constexpr bool is(Iter first, Iter last) noexcept
	{
		return find_first_not(first, last) == last;
	}
};

using c_alnum = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_alnum, false>;
using c_alpha = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_alpha, false>;
using c_blank = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_blank, false>;
using c_cntrl = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_cntrl, false>;
using c_digit = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_digit, false>;
using c_fullwidth = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_fullwidth, false>;
using c_graph = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_graph, false>;
using c_halfwidth = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_halfwidth, false>;
using c_lower = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_lower, false>;
using c_print = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_print, false>;
using c_punct = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_punct, false>;
using c_space = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_space, false>;
using c_upper = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_upper, false>;
using c_xdigit = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_xdigit, false>;
using dos_path_invalid_character = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::dos_path_invalid_character, false>;
using html_whitespace = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::html_whitespace, false>;


using not_c_alnum = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_alnum, true>;
using not_c_alpha = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_alpha, true>;
using not_c_blank = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_blank, true>;
using not_c_cntrl = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_cntrl, true>;
using not_c_digit = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_digit, true>;
using not_c_fullwidth = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_fullwidth, true>;
using not_c_graph = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_graph, true>;
using not_c_halfwidth = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_halfwidth, true>;
using not_c_lower = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_lower, true>;
using not_c_print = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_print, true>;
using not_c_punct = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_punct, true>;
using not_c_space = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_space, true>;
using not_c_upper = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_upper, true>;
using not_c_xdigit = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_xdigit, true>;
using not_dos_path_invalid_character = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::dos_path_invalid_character, true>;
using not_html_whitespace = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::html_whitespace, true>;

namespace details
{
template <::fast_io::char_category::char_category_family fam>
struct to_c_common_fn_impl
{
	template <::std::integral char_type>
#ifdef __cpp_static_call_operator
	static
#endif
		inline constexpr char_type operator()(char_type ch)
#ifndef __cpp_static_call_operator
			const
#endif
		noexcept
	{
		using unsigned_char_type = ::std::make_unsigned_t<char_type>;
		if constexpr (fam == ::fast_io::char_category::char_category_family::c_lower)
		{
			if constexpr (!::fast_io::details::is_ebcdic<char_type>)
			{
				return static_cast<char_type>(
					static_cast<unsigned_char_type>(::fast_io::char_category::details::to_c_lower_ascii_impl(static_cast<unsigned_char_type>(ch))));
			}
			else if constexpr (::std::same_as<char, char_type>)
			{
				switch (ch)
				{
				case 'A':
					return 'a';
				case 'B':
					return 'b';
				case 'C':
					return 'c';
				case 'D':
					return 'd';
				case 'E':
					return 'e';
				case 'F':
					return 'f';
				case 'G':
					return 'g';
				case 'H':
					return 'h';
				case 'I':
					return 'i';
				case 'J':
					return 'j';
				case 'K':
					return 'k';
				case 'L':
					return 'l';
				case 'M':
					return 'm';
				case 'N':
					return 'n';
				case 'O':
					return 'o';
				case 'P':
					return 'p';
				case 'Q':
					return 'q';
				case 'R':
					return 'r';
				case 'S':
					return 's';
				case 'T':
					return 't';
				case 'U':
					return 'u';
				case 'V':
					return 'v';
				case 'W':
					return 'w';
				case 'X':
					return 'x';
				case 'Y':
					return 'y';
				case 'Z':
					return 'z';
				default:
					return ch;
				}
			}
			else if constexpr (::std::same_as<wchar_t, char_type>)
			{
				switch (ch)
				{
				case L'A':
					return L'a';
				case L'B':
					return L'b';
				case L'C':
					return L'c';
				case L'D':
					return L'd';
				case L'E':
					return L'e';
				case L'F':
					return L'f';
				case L'G':
					return L'g';
				case L'H':
					return L'h';
				case L'I':
					return L'i';
				case L'J':
					return L'j';
				case L'K':
					return L'k';
				case L'L':
					return L'l';
				case L'M':
					return L'm';
				case L'N':
					return L'n';
				case L'O':
					return L'o';
				case L'P':
					return L'p';
				case L'Q':
					return L'q';
				case L'R':
					return L'r';
				case L'S':
					return L's';
				case L'T':
					return L't';
				case L'U':
					return L'u';
				case L'V':
					return L'v';
				case L'W':
					return L'w';
				case L'X':
					return L'x';
				case L'Y':
					return L'y';
				case L'Z':
					return L'z';
				default:
					return ch;
				}
			}
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_upper)
		{
			if constexpr (!::fast_io::details::is_ebcdic<char_type>)
			{
				return static_cast<char_type>(
					static_cast<unsigned_char_type>(::fast_io::char_category::details::to_c_upper_ascii_impl(static_cast<unsigned_char_type>(ch))));
			}
			else if constexpr (::std::same_as<char, char_type>)
			{
				switch (ch)
				{
				case 'a':
					return 'A';
				case 'b':
					return 'B';
				case 'c':
					return 'C';
				case 'd':
					return 'D';
				case 'e':
					return 'E';
				case 'f':
					return 'F';
				case 'g':
					return 'G';
				case 'h':
					return 'H';
				case 'i':
					return 'I';
				case 'j':
					return 'J';
				case 'k':
					return 'K';
				case 'l':
					return 'L';
				case 'm':
					return 'M';
				case 'n':
					return 'N';
				case 'o':
					return 'O';
				case 'p':
					return 'P';
				case 'q':
					return 'Q';
				case 'r':
					return 'R';
				case 's':
					return 'S';
				case 't':
					return 'T';
				case 'u':
					return 'U';
				case 'v':
					return 'V';
				case 'w':
					return 'W';
				case 'x':
					return 'X';
				case 'y':
					return 'Y';
				case 'z':
					return 'Z';
				default:
					return ch;
				}
			}
			else if constexpr (::std::same_as<wchar_t, char_type>)
			{
				switch (ch)
				{
				case L'a':
					return L'A';
				case L'b':
					return L'B';
				case L'c':
					return L'C';
				case L'd':
					return L'D';
				case L'e':
					return L'E';
				case L'f':
					return L'F';
				case L'g':
					return L'G';
				case L'h':
					return L'H';
				case L'i':
					return L'I';
				case L'j':
					return L'J';
				case L'k':
					return L'K';
				case L'l':
					return L'L';
				case L'm':
					return L'M';
				case L'n':
					return L'N';
				case L'o':
					return L'O';
				case L'p':
					return L'P';
				case L'q':
					return L'Q';
				case L'r':
					return L'R';
				case L's':
					return L'S';
				case L't':
					return L'T';
				case L'u':
					return L'U';
				case L'v':
					return L'V';
				case L'w':
					return L'W';
				case L'x':
					return L'X';
				case L'y':
					return L'Y';
				case L'z':
					return L'Z';
				default:
					return ch;
				}
			}
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_halfwidth)
		{
			if constexpr (sizeof(char_type) < sizeof(char32_t))
			{
				return ch;
			}
			else if constexpr (!::std::same_as<char_type, char32_t> && sizeof(char_type) == sizeof(char32_t))
			{
				return static_cast<char_type>(operator()(static_cast<char32_t>(ch)));
			}
			else if constexpr (::std::signed_integral<char_type>)
			{
				return static_cast<char_type>(operator()(static_cast<unsigned_char_type>(ch)));
			}
			else if constexpr (::std::same_as<char_type, wchar_t> && ::fast_io::details::wide_is_none_utf_endian)
			{
				constexpr unsigned_char_type fullwidth_exclaimation_mark_val{0xFF01};
				constexpr unsigned_char_type num{94};
				constexpr unsigned_char_type halfwidth_exclaimation_mark_val{u8'!'};
				unsigned_char_type cht{ch};
				cht = ::fast_io::byte_swap(cht);
				unsigned_char_type const umav{static_cast<unsigned_char_type>(cht - fullwidth_exclaimation_mark_val)};
				if (umav < num)
				{
					return static_cast<unsigned_char_type>(umav + halfwidth_exclaimation_mark_val);
				}
				return cht;
			}
			else
			{
				constexpr unsigned_char_type fullwidth_exclaimation_mark_val{0xFF01};
				constexpr unsigned_char_type num{94};
				constexpr unsigned_char_type halfwidth_exclaimation_mark_val{u8'!'};
				unsigned_char_type const umav{static_cast<unsigned_char_type>(ch - fullwidth_exclaimation_mark_val)};
				if (umav < num)
				{
					return static_cast<unsigned_char_type>(umav + halfwidth_exclaimation_mark_val);
				}
				return ch;
			}
		}
	}
};

} // namespace details

inline constexpr ::fast_io::char_category::details::to_c_common_fn_impl<::fast_io::char_category::char_category_family::c_lower> to_c_lower{};
inline constexpr ::fast_io::char_category::details::to_c_common_fn_impl<::fast_io::char_category::char_category_family::c_upper> to_c_upper{};
inline constexpr ::fast_io::char_category::details::to_c_common_fn_impl<::fast_io::char_category::char_category_family::c_halfwidth> to_c_halfwidth{};

namespace ranges
{
namespace details
{
template <::fast_io::char_category::char_category_family fam>
struct to_c_common_rg_fn
{

	template <::std::input_iterator Iter, ::std::sentinel_for<Iter> S, typename Proj = ::std::identity>
		requires ::std::integral<::std::iter_value_t<Iter>>
#ifdef __cpp_static_call_operator
	static
#endif
		inline constexpr Iter operator()(Iter first, S last, Proj proj = {})
#ifndef __cpp_static_call_operator
			const
#endif
		noexcept
	{
		for (; first != last; ++first)
		{
			if constexpr (fam == ::fast_io::char_category::char_category_family::c_lower)
			{
				*first = ::fast_io::char_category::to_c_lower(proj(*first));
			}
			else if constexpr (fam == ::fast_io::char_category::char_category_family::c_upper)
			{
				*first = ::fast_io::char_category::to_c_upper(proj(*first));
			}
			else if constexpr (fam == ::fast_io::char_category::char_category_family::c_halfwidth)
			{
				*first = ::fast_io::char_category::to_c_halfwidth(proj(*first));
			}
		}
		return first;
	}
	template <::std::ranges::input_range R, typename Proj = ::std::identity>
		requires ::std::integral<::std::ranges::range_value_t<R>>
#ifdef __cpp_static_call_operator
	static
#endif
		inline constexpr ::std::ranges::borrowed_iterator_t<R> operator()(R &&r, Proj proj = {})
#ifndef __cpp_static_call_operator
			const
#endif
		noexcept
	{
		return operator()(::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(proj));
	}
};
} // namespace details

inline constexpr ::fast_io::char_category::ranges::details::to_c_common_rg_fn<::fast_io::char_category::char_category_family::c_lower> to_c_lower{};
inline constexpr ::fast_io::char_category::ranges::details::to_c_common_rg_fn<::fast_io::char_category::char_category_family::c_upper> to_c_upper{};
inline constexpr ::fast_io::char_category::ranges::details::to_c_common_rg_fn<::fast_io::char_category::char_category_family::c_halfwidth> to_c_halfwidth{};
} // namespace ranges
} // namespace fast_io::char_category
