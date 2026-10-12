#pragma once

namespace fast_io::details
{

template <unsigned streambuftype, ::std::integral char_type>
	requires(::std::same_as<char_type, char> || ::std::same_as<char_type, wchar_t> ||
			 ::std::same_as<char_type, char8_t> || ::std::same_as<char_type, char16_t> ||
			 ::std::same_as<char_type, char32_t>)
inline constexpr auto compute_libstdcxx_stdinoutbufname() noexcept
{
	static_assert(streambuftype < 3u);
	constexpr bool ismsvcabi{
#if defined(_LIBCPP_ABI_MICROSOFT) || defined(_MSC_VER)
		true
#endif
	};
	/*
	These implementations are mostly based on imaginary future since libstdc++ does not support MSVC ABI and MSVC does not support Itanium.

	However, I see no technical reasons why they cannot happen. Maybe Linux vs Microsoft political BS?
	*/

	if constexpr (ismsvcabi)
	{
		if constexpr (streambuftype == 2) //__gnu_cxx::stdio_sync_filebuf
		{
			if constexpr (::std::same_as<char_type, char>)
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$stdio_sync_filebuf@DU?$char_traits@D@std@@@__gnu_cxx@@");
			}
			else if constexpr (::std::same_as<char_type, wchar_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$stdio_sync_filebuf@_WU?$char_traits@_W@std@@@__gnu_cxx@@");
			}
			else if constexpr (::std::same_as<char_type, char8_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$stdio_sync_filebuf@_QU?$char_traits@_Q@std@@@__gnu_cxx@@");
			}
			else if constexpr (::std::same_as<char_type, char16_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$stdio_sync_filebuf@_SU?$char_traits@_S@std@@@__gnu_cxx@@");
			}
			else
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$stdio_sync_filebuf@_UU?$char_traits@_U@std@@@__gnu_cxx@@");
			}
		}
		else if constexpr (streambuftype == 1) //__gnu_cxx::stdio_filebuf
		{
			if constexpr (::std::same_as<char_type, char>)
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$stdio_filebuf@DU?$char_traits@D@std@@@__gnu_cxx@@");
			}
			else if constexpr (::std::same_as<char_type, wchar_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$stdio_filebuf@_WU?$char_traits@_W@std@@@__gnu_cxx@@");
			}
			else if constexpr (::std::same_as<char_type, char8_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$stdio_filebuf@_QU?$char_traits@_Q@std@@@__gnu_cxx@@");
			}
			else if constexpr (::std::same_as<char_type, char16_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$stdio_filebuf@_SU?$char_traits@_S@std@@@__gnu_cxx@@");
			}
			else
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$stdio_filebuf@_UU?$char_traits@_U@std@@@__gnu_cxx@@");
			}
		}
		else // MSVC STL and GNU libstdc++ use the same symbol for std::filebuf i think
		{
			if constexpr (::std::same_as<char_type, char>)
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$basic_filebuf@DU?$char_traits@D@std@@@std@@");
			}
			else if constexpr (::std::same_as<char_type, wchar_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$basic_filebuf@_WU?$char_traits@_W@std@@@std@@");
			}
			else if constexpr (::std::same_as<char_type, char8_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$basic_filebuf@_QU?$char_traits@_Q@std@@@std@@");
			}
			else if constexpr (::std::same_as<char_type, char16_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$basic_filebuf@_SU?$char_traits@_S@std@@@std@@");
			}
			else
			{
				return ::fast_io::details::compute_symbol_name(u8".?AV?$basic_filebuf@_UU?$char_traits@_U@std@@@std@@");
			}
		}
	}
	else
	{
		if constexpr (streambuftype == 2) //__gnu_cxx::stdio_sync_filebuf
		{
			if constexpr (::std::same_as<char_type, char>)
			{
				return ::fast_io::details::compute_symbol_name(u8"N9__gnu_cxx18stdio_sync_filebufIcSt11char_traitsIcEEE");
			}
			else if constexpr (::std::same_as<char_type, wchar_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8"N9__gnu_cxx18stdio_sync_filebufIwSt11char_traitsIwEEE");
			}
			else if constexpr (::std::same_as<char_type, char8_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8"N9__gnu_cxx18stdio_sync_filebufIDuSt11char_traitsIDuEEE");
			}
			else if constexpr (::std::same_as<char_type, char16_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8"N9__gnu_cxx18stdio_sync_filebufIDsSt11char_traitsIDsEEE");
			}
			else
			{
				return ::fast_io::details::compute_symbol_name(u8"N9__gnu_cxx18stdio_sync_filebufIDiSt11char_traitsIDiEEE");
			}
		}
		else if constexpr (streambuftype == 1) //__gnu_cxx::stdio_filebuf
		{
			if constexpr (::std::same_as<char_type, char>)
			{
				return ::fast_io::details::compute_symbol_name(u8"N9__gnu_cxx13stdio_filebufIcSt11char_traitsIcEEE");
			}
			else if constexpr (::std::same_as<char_type, wchar_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8"N9__gnu_cxx13stdio_filebufIwSt11char_traitsIwEEE");
			}
			else if constexpr (::std::same_as<char_type, char8_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8"N9__gnu_cxx13stdio_filebufIDuSt11char_traitsIDuEEE");
			}
			else if constexpr (::std::same_as<char_type, char16_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8"N9__gnu_cxx13stdio_filebufIDuSt11char_traitsIDuEEE");
			}
			else
			{
				return ::fast_io::details::compute_symbol_name(u8"N9__gnu_cxx13stdio_filebufIDiSt11char_traitsIDiEEE");
			}
		}
		else // MSVC STL and GNU libstdc++ use the same symbol for std::filebuf i think
		{
			if constexpr (::std::same_as<char_type, char>)
			{
				return ::fast_io::details::compute_symbol_name(u8"St13basic_filebufIcSt11char_traitsIcEE");
			}
			else if constexpr (::std::same_as<char_type, wchar_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8"St13basic_filebufIwSt11char_traitsIwEE");
			}
			else if constexpr (::std::same_as<char_type, char8_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8"St13basic_filebufIDuSt11char_traitsIDuEE");
			}
			else if constexpr (::std::same_as<char_type, char16_t>)
			{
				return ::fast_io::details::compute_symbol_name(u8"St13basic_filebufIDsSt11char_traitsIDsEE");
			}
			else
			{
				return ::fast_io::details::compute_symbol_name(u8"St13basic_filebufIDiSt11char_traitsIDiEE");
			}
		}
	}
}

template <unsigned streambuftype, ::std::integral char_type>
inline constexpr auto libstdcxx_streambufname{::fast_io::details::compute_libstdcxx_stdinoutbufname<streambuftype, char_type>()};
} // namespace fast_io::details