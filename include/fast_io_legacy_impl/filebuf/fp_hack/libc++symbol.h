#pragma once

namespace fast_io::details
{

inline constexpr ::std::size_t compute_libcxx_symbol_name_chars_len(::std::size_t value) noexcept
{
	::std::size_t sz{1};
	for (; 10u <= value; value /= 10u)
	{
		++sz;
	}
	return sz;
}

#define FAST_IO_LIBCPP_ABI_NAMESPACE_STR_SSTRIFY(s) u8## #s
#define FAST_IO_LIBCPP_ABI_NAMESPACE_STR_STRIFY(s) FAST_IO_LIBCPP_ABI_NAMESPACE_STR_SSTRIFY(s)
#define FAST_IO_LIBCPP_ABI_NAMESPACE_STR FAST_IO_LIBCPP_ABI_NAMESPACE_STR_STRIFY(_LIBCPP_ABI_NAMESPACE)

inline constexpr ::std::size_t libcxx_symbol_len{sizeof(FAST_IO_LIBCPP_ABI_NAMESPACE_STR) - 1u};

inline constexpr ::std::size_t libcxx_symbol_name_chars_len{compute_libcxx_symbol_name_chars_len(libcxx_symbol_len)};

inline constexpr char8_t *compute_libcxx_itanium_prefix(char8_t *iter) noexcept
{
	auto ed{iter + libcxx_symbol_name_chars_len};
	auto rit{ed};
	for (::std::size_t v{libcxx_symbol_len}; iter != rit; --rit)
	{
		::std::size_t dv{static_cast<::std::size_t>(v / 10u)};
		char8_t md{static_cast<char8_t>(v % 10u)};
		rit[-1] = static_cast<char8_t>(md + u8'0');
		v = dv;
	}
	return ed;
}

template <unsigned streambuftype, ::std::integral char_type>
	requires(::std::same_as<char_type, char> || ::std::same_as<char_type, wchar_t> ||
			 ::std::same_as<char_type, char8_t> || ::std::same_as<char_type, char16_t> ||
			 ::std::same_as<char_type, char32_t>)
inline constexpr auto compute_libcxx_stdinoutbufname() noexcept
{
	static_assert(streambuftype < 3u);
	constexpr bool ismsvcabi{
#ifdef _LIBCPP_ABI_MICROSOFT
		true
#endif
	};
	if constexpr (ismsvcabi)
	{
		if constexpr (streambuftype == 2)
		{
			if constexpr (::std::same_as<char_type, char>)
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::basic_filebuf<char,struct std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::char_traits<char> >");
			}
			else if constexpr (::std::same_as<char_type, wchar_t>)
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::basic_filebuf<wchar_t,struct std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::char_traits<wchar_t> >");
			}
			else if constexpr (::std::same_as<char_type, char8_t>)
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::basic_filebuf<char8_t,struct std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::char_traits<char8_t> >");
			}
			else if constexpr (::std::same_as<char_type, char16_t>)
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::basic_filebuf<char16_t,struct std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::char_traits<char16_t> >");
			}
			else
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::basic_filebuf<char32_t,struct std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::char_traits<char32_t> >");
			}
		}
		else if constexpr (streambuftype == 1)
		{
			if constexpr (::std::same_as<char_type, char>)
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdoutbuf<char>");
			}
			else if constexpr (::std::same_as<char_type, wchar_t>)
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdoutbuf<wchar_t>");
			}
			else if constexpr (::std::same_as<char_type, char8_t>)
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdoutbuf<char8_t>");
			}
			else if constexpr (::std::same_as<char_type, char16_t>)
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdoutbuf<char16_t>");
			}
			else
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdoutbuf<char32_t>");
			}
		}
		else
		{
			if constexpr (::std::same_as<char_type, char>)
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdinbuf<char>");
			}
			else if constexpr (::std::same_as<char_type, wchar_t>)
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdinbuf<wchar_t>");
			}
			else if constexpr (::std::same_as<char_type, char8_t>)
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdinbuf<char8_t>");
			}
			else if constexpr (::std::same_as<char_type, char16_t>)
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdinbuf<char16_t>");
			}
			else
			{
				return ::fast_io::details::compute_symbol_name(
					u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdinbuf<char32_t>");
			}
		}
	}
	else
	{
		/*
		Itanium ABI
		*/
		if constexpr (streambuftype == 2)
		{
			/*
			char: NSt3__113basic_filebufIcNS_11char_traitsIcEEEE
			wchar_t: NSt3__113basic_filebufIwNS_11char_traitsIwEEEE
			char8_t: NSt3__113basic_filebufIDuNS_11char_traitsIDuEEEE
			char16_t: NSt3__113basic_filebufIDsNS_11char_traitsIDsEEEE
			char32_t: NSt3__113basic_filebufIDiNS_11char_traitsIDiEEEE
			*/
			constexpr ::std::size_t buffersize{sizeof(u8"NSt"
													  u8"13basic_filebufI"
													  u8"NS_11char_traitsI"
													  u8"EEEE") +
											   ::fast_io::details::itanium_char_type_symbol_size<char_type> * 2 +
											   ::fast_io::details::libcxx_symbol_len +
											   ::fast_io::details::libcxx_symbol_name_chars_len - 1};
			::fast_io::freestanding::array<char8_t, buffersize> buffer;
			auto it{buffer.data()};
			it = ::fast_io::details::copy_string_literal(u8"NSt", it);
			it = ::fast_io::details::compute_libcxx_itanium_prefix(it);
			it = ::fast_io::details::copy_string_literal(FAST_IO_LIBCPP_ABI_NAMESPACE_STR, it);
			it = ::fast_io::details::copy_string_literal(u8"13basic_filebufI", it);
			it = ::fast_io::details::compute_itanium_char_type_identification<char_type>(it);
			it = ::fast_io::details::copy_string_literal(u8"NS_11char_traitsI", it);
			it = ::fast_io::details::compute_itanium_char_type_identification<char_type>(it);
			::fast_io::details::copy_string_literal(u8"EEEE", it);
			return buffer;
		}
		else if constexpr (streambuftype != 2)
		{
			/*
			char: NSt3__111__stdoutbufIcEE
			wchar_t: NSt3__111__stdoutbufIwEE
			char: NSt3__110__stdinbufIcEE
			wchar_t: NSt3__110__stdinbufIwEE
			*/
			constexpr ::std::size_t stdinoutsz{(streambuftype == 1 ? sizeof(u8"11__stdoutbufI") : sizeof(u8"10__stdinbufI")) - 1};
			constexpr ::std::size_t buffersize{sizeof(u8"NSt"
													  u8"EE") +
											   stdinoutsz + ::fast_io::details::itanium_char_type_symbol_size<char_type> +
											   ::fast_io::details::libcxx_symbol_len +
											   ::fast_io::details::libcxx_symbol_name_chars_len - 1};
			::fast_io::freestanding::array<char8_t, buffersize> buffer;
			auto it{buffer.data()};
			it = ::fast_io::details::copy_string_literal(u8"NSt", it);
			it = ::fast_io::details::compute_libcxx_itanium_prefix(it);
			it = ::fast_io::details::copy_string_literal(FAST_IO_LIBCPP_ABI_NAMESPACE_STR, it);
			if constexpr (streambuftype == 1)
			{
				it = ::fast_io::details::copy_string_literal(u8"11__stdoutbufI", it);
			}
			else
			{
				it = ::fast_io::details::copy_string_literal(u8"10__stdinbufI", it);
			}
			it = ::fast_io::details::compute_itanium_char_type_identification<char_type>(it);
			::fast_io::details::copy_string_literal(u8"EE", it);
			return buffer;
		}
	}
}

template <unsigned streambuftype, ::std::integral char_type>
inline constexpr auto libcxx_streambufname{::fast_io::details::compute_libcxx_stdinoutbufname<streambuftype, char_type>()};

#undef FAST_IO_LIBCPP_ABI_NAMESPACE_STR_SSTRIFY
#undef FAST_IO_LIBCPP_ABI_NAMESPACE_STR_STRIFY
#undef FAST_IO_LIBCPP_ABI_NAMESPACE_STR

} // namespace fast_io::details
