#pragma once

namespace fast_io
{

struct u8utf_mb_state
{
	char8_t bytes[7];
	char8_t size{};
	explicit inline constexpr operator bool() const noexcept
	{
		return static_cast<bool>(size);
	}
};

struct u16utf_mb_state
{
	char16_t value;
	bool state{};
	explicit inline constexpr operator bool() const noexcept
	{
		return state;
	}
};

struct u32utf_mb_state
{
	explicit inline constexpr operator bool() const noexcept
	{
		return false;
	}
};


template<std::integral char_type>
requires (sizeof(char_type)<=4)
using basic_utf_mb_state = std::conditional_t<sizeof(char_type)==4,u32utf_mb_state,
std::conditional_t<sizeof(char_type)==2,u16utf_mb_state,u8utf_mb_state>>;
using utf_mb_state = basic_utf_mb_state<char>;
using wutf_mb_state = basic_utf_mb_state<wchar_t>;

using gb18030_mb_state = utf_mb_state;
using ebcdic_mb_state = utf_mb_state;

using execution_charset_mb_state = utf_mb_state;
using wexecution_charset_mb_state = wutf_mb_state;

enum class encoding_scheme
{
execution_charset,
utf_le,
utf_be,
gb18030,
utf_ebcdic,
utf=std::endian::big==std::endian::native?utf_be:(std::endian::little==std::endian::native?utf_le:5)
};
/*
CppCon 2018: Bob Steagall “Fast Conversion From UTF-8 with C++, DFAs, and SSE Intrinsics”

https://www.youtube.com/watch?v=5FQ87-Ecb-A
https://github.com/BobSteagall/utf_utils/blob/master/src/utf_utils.cpp
*/

namespace details
{

template<std::size_t src_char_type_size,std::size_t dest_char_type_size>
requires ((dest_char_type_size<=4)&&(src_char_type_size<=4))
inline constexpr std::size_t cal_full_reserve_size(std::size_t internal_size) noexcept
{
	constexpr std::size_t external_max{std::numeric_limits<std::size_t>::max()/dest_char_type_size};
	constexpr std::size_t internal_max{std::numeric_limits<std::size_t>::max()/src_char_type_size};
	if constexpr(src_char_type_size==4)
	{
		if constexpr(dest_char_type_size==4)
		{
			constexpr std::size_t imax{external_max};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size;
		}
		else if constexpr(dest_char_type_size==2)
		{
			constexpr std::size_t imax{external_max/2};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size*2;
		}
		else
		{
			constexpr std::size_t imax{external_max/8};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size*8;
		}
	}
	else if constexpr(src_char_type_size==2)
	{
		if constexpr(dest_char_type_size==4)
		{
			constexpr std::size_t imax{external_max};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size;
		}
		else if constexpr(dest_char_type_size==2)
		{
			constexpr std::size_t imax{external_max/2};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size*2;
		}
		else
		{
			constexpr std::size_t imax{external_max/8};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size*8;
		}
	}
	else
	{
		if constexpr(dest_char_type_size==4)
		{
			constexpr std::size_t imax{external_max};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size;
		}
		else if constexpr(dest_char_type_size==2)
		{
			constexpr std::size_t imax{internal_max/2};
			if(internal_size>=imax)
				fast_terminate();
			return 2*internal_size;
		}
		else
		{
			constexpr std::size_t imax{internal_max/8};
			if(internal_size>=imax)
				fast_terminate();
			return 8*internal_size;
		}
	}
}

template<std::size_t src_char_type_size,std::size_t dest_char_type_size>
requires ((dest_char_type_size<=4)&&(src_char_type_size<=4))
inline constexpr std::size_t cal_decorated_reserve_size(std::size_t internal_size) noexcept
{
	constexpr std::size_t external_max{std::numeric_limits<std::size_t>::max()/dest_char_type_size};
	constexpr std::size_t internal_max{std::numeric_limits<std::size_t>::max()/src_char_type_size};
	if constexpr(src_char_type_size==4)
	{
		if constexpr(dest_char_type_size==4)
		{
			constexpr std::size_t imax{external_max};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size;
		}
		else if constexpr(dest_char_type_size==2)
		{
			constexpr std::size_t imax{external_max/2};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size*2;
		}
		else
		{
			constexpr std::size_t imax{external_max/8};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size*8;
		}
	}
	else if constexpr(src_char_type_size==2)
	{
		if constexpr(dest_char_type_size==4)
		{
			constexpr std::size_t imax{external_max-1};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size+1;
		}
		else if constexpr(dest_char_type_size==2)
		{
			constexpr std::size_t imax{external_max/2-1};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size*2+2;
		}
		else
		{
			constexpr std::size_t imax{external_max/8-1};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size*8+8;
		}
	}
	else
	{
		if constexpr(dest_char_type_size==4)
		{
			constexpr std::size_t imax{external_max-7};
			if(internal_size>=imax)
				fast_terminate();
			return internal_size+7;
		}
		else if constexpr(dest_char_type_size==2)
		{
			constexpr std::size_t imax{internal_max/2-7};
			if(internal_size>=imax)
				fast_terminate();
			return 2*internal_size+14;
		}
		else
		{
			constexpr std::size_t imax{internal_max/8-7};
			if(internal_size>=imax)
				fast_terminate();
			return 8*internal_size+56;
		}
	}
}
#if defined(__GNUC_EXECUTION_CHARSET_NAME) || defined(__GNUC_WIDE_EXECUTION_CHARSET_NAME)

template<std::size_t N1,std::size_t N2>
inline constexpr bool execution_charset_is(char const (&str)[N1],char8_t const (&encoding)[N2]) noexcept
{
	if constexpr(N1!=N2)
	{
		return false;
	}
	else
	{
		for(std::size_t i{};i!=N1;++i)
		{
			char8_t ch{::fast_io::char_category::to_c_upper(static_cast<char8_t>(str[i]))};
			char8_t ch1{::fast_io::char_category::to_c_upper(static_cast<char8_t>(encoding[i]))};
			if(ch!=ch1)
			{
				return false;
			}
		}
		return true;
	}
}

#endif
}

template<std::integral char_type>
inline constexpr encoding_scheme execution_charset_encoding_scheme() noexcept
{
	using char_type_no_cvref_t = std::remove_cvref_t<char_type>;
	if constexpr(::fast_io::details::is_ebcdic<char_type_no_cvref_t>)
		return encoding_scheme::utf_ebcdic;
	else
	{
		if constexpr(std::same_as<char_type_no_cvref_t,char>)
		{
#if defined(_MSVC_EXECUTION_CHARACTER_SET)
			if constexpr(_MSVC_EXECUTION_CHARACTER_SET == 936 || _MSVC_EXECUTION_CHARACTER_SET == 54936)
			{
				return encoding_scheme::gb18030;
			}
			else
			{
				return encoding_scheme::utf;
			}
#elif defined(__GNUC_EXECUTION_CHARSET_NAME)
			if constexpr(::fast_io::details::execution_charset_is(__GNUC_EXECUTION_CHARSET_NAME,u8"GB18030")
				|| ::fast_io::details::execution_charset_is(__GNUC_EXECUTION_CHARSET_NAME,u8"GBK"))
			{
				return encoding_scheme::gb18030;
			}
			else
			{
				return encoding_scheme::utf;
			}
#else
			return encoding_scheme::utf;
#endif
		}
		else if constexpr(std::same_as<char_type_no_cvref_t,wchar_t>)
		{
#if defined(__GNUC_WIDE_EXECUTION_CHARSET_NAME)
			if constexpr(sizeof(wchar_t)==1&&
				(::fast_io::details::execution_charset_is(__GNUC_WIDE_EXECUTION_CHARSET_NAME,u8"GB18030")
				|| ::fast_io::details::execution_charset_is(__GNUC_WIDE_EXECUTION_CHARSET_NAME,u8"GBK")))
			{
				return encoding_scheme::gb18030;
			}
			else
			{
				if constexpr(::fast_io::details::wide_is_none_utf_endian)
				{
					if constexpr(encoding_scheme::utf==encoding_scheme::utf_le)
					{
						return encoding_scheme::utf_be;
					}
					else
					{
						return encoding_scheme::utf_le;
					}
				}
				return encoding_scheme::utf;
			}
#else
			return encoding_scheme::utf;
#endif
		}
		else
		{
			return encoding_scheme::utf;
		}
	}
}

template<std::integral char_type,encoding_scheme scheme=execution_charset_encoding_scheme<char_type>()>
using basic_mb_state=basic_utf_mb_state<char_type>;

template<std::integral src_char_type,std::integral dest_char_type>
struct code_cvt_result
{
	src_char_type const* src;
	dest_char_type* dst;
};


namespace details::codecvt
{

inline constexpr bool encoding_is_utf(encoding_scheme scheme) noexcept
{
	return scheme==encoding_scheme::utf||scheme==encoding_scheme::utf_le||scheme==encoding_scheme::utf_be;
}

inline constexpr bool is_native_scheme(encoding_scheme scheme) noexcept
{
	return scheme==encoding_scheme::utf;
}

template<std::integral T>
requires (sizeof(T)==1)
inline constexpr std::size_t get_utf8_invalid_code_units(T* dst) noexcept
{
	*dst = static_cast<T>(0xEF);
	dst[1] = static_cast<T>(0xBF);
	dst[2] = static_cast<T>(0xBD);
	return 3;
}

template<encoding_scheme scheme,std::integral T>
requires (sizeof(T)<=4)
inline constexpr std::size_t get_utf_code_units(char32_t cdpt,T* dst) noexcept
{
	if constexpr(scheme==encoding_scheme::utf_ebcdic)
	{
		return get_utf_ebcdic_code_units(cdpt,dst);
	}
	else if constexpr(scheme==encoding_scheme::gb18030)
	{
		return gb18030::get_gb18030_code_units(cdpt,dst);
	}
	else
	{
		if constexpr(sizeof(T)==sizeof(char32_t))
		{
			if constexpr(is_native_scheme(scheme))
				*dst=static_cast<T>(cdpt);
			else
				*dst=byte_swap(static_cast<T>(cdpt));
			return 1;
		}
		else if constexpr(sizeof(T)==sizeof(char16_t))
		{
			if (cdpt < 0x10000)
			{
				if constexpr(is_native_scheme(scheme))
					*dst = static_cast<T>(cdpt);
				else
					*dst = byte_swap(static_cast<T>(cdpt));
				return 1;
			}
			else
			{
				if constexpr(is_native_scheme(scheme))
				{
					*dst = static_cast<T>(0xD7C0 + (cdpt >> 10));
					dst[1] = static_cast<T>(0xDC00 + (cdpt & 0x3FF));
				}
				else
				{
					*dst = byte_swap(static_cast<T>(0xD7C0 + (cdpt >> 10)));
					dst[1] = byte_swap(static_cast<T>(0xDC00 + (cdpt & 0x3FF)));
				}
				return 2;
			}
		}
		else
		{
			if (cdpt <= 0x7F)[[likely]]
			{
				*dst = static_cast<T>(cdpt);
				return 1;
			}
			else if (cdpt <= 0x7FF)
			{
				*dst = static_cast<T>(0xC0 | ((cdpt >> 6) & 0x1F));
				dst[1] = static_cast<T>(0x80 | (cdpt        & 0x3F));
				return 2;
			}
			else if (cdpt <= 0xFFFF)
			{
				*dst = static_cast<T>(0xE0 | ((cdpt >> 12) & 0x0F));
				dst[1] = static_cast<T>(0x80 | ((cdpt >> 6)  & 0x3F));
				dst[2] = static_cast<T>(0x80 | (cdpt         & 0x3F));
				return 3;
			}
			else if (cdpt <= 0x10FFFF)
			{
				*dst = static_cast<T>(0xF0 | ((cdpt >> 18) & 0x07));
				dst[1] = static_cast<T>(0x80 | ((cdpt >> 12) & 0x3F));
				dst[2] = static_cast<T>(0x80 | ((cdpt >> 6)  & 0x3F));
				dst[3] = static_cast<T>(0x80 | (cdpt         & 0x3F));
				return 4;
			}
			else [[unlikely]]
				return get_utf8_invalid_code_units(dst);
		}
	}
}
inline constexpr bool is_utf16_surrogate(char16_t uc) noexcept { return (uc - 0xd800u) < 2048u; }
inline constexpr bool is_utf16_high_surrogate(char16_t uc) noexcept { return (uc & 0xfffffc00) == 0xd800; }
inline constexpr bool is_utf16_low_surrogate(char16_t uc) noexcept { return (uc & 0xfffffc00) == 0xdc00; }

inline constexpr char32_t utf16_surrogate_to_utf32(char16_t high, char16_t low) noexcept
{ 
	return static_cast<char32_t>((static_cast<std::uint_least32_t>(high) << 10u) + low - 0x35fdc00u); 
}

#if (defined(_MSC_VER)&&defined(_M_AMD64)&&!defined(__clang__)) || (defined(__SSE__) && defined(__x86_64__))
template<std::integral T,std::integral U>
requires ((sizeof(T)==1)&&(sizeof(U)==1||sizeof(U)==2||sizeof(U)==4))
inline code_cvt_result<T,U> convert_ascii_with_sse(T const* __restrict pSrc, U* __restrict pDst) noexcept
{
	std::uint_least32_t mask;
#if (defined(__GNUC__) || defined(__clang__)) && !defined(__INTEL_COMPILER)
	using namespace fast_io::intrinsics;
	constexpr std::size_t m128i_size{16};
	if constexpr(sizeof(U)==1)
	{
		x86_64_v16qi chunk;
		__builtin_memcpy(__builtin_addressof(chunk),pSrc,m128i_size);
		mask = static_cast<std::uint_least32_t>(__builtin_ia32_pmovmskb128(chunk));
		__builtin_memcpy(pDst,__builtin_addressof(chunk),m128i_size);
	}
	else if constexpr(sizeof(U)==2)
	{
		x86_64_v16qi const zero{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		x86_64_v16qi chunk;
		__builtin_memcpy(__builtin_addressof(chunk),pSrc,m128i_size);
		mask = static_cast<std::uint_least32_t>(__builtin_ia32_pmovmskb128(chunk));
#if __has_builtin(__builtin_shufflevector)
		x86_64_v16qi half{__builtin_shufflevector(chunk, zero, 0, 16+0, 1, 16+1, 2, 16+2, 3, 16+3, 4, 16+4, 5, 16+5, 6, 16+6, 7, 16+7)};
		__builtin_memcpy(pDst,__builtin_addressof(half),m128i_size);
		half = __builtin_shufflevector(chunk, zero, 8, 16+8, 9, 16+9, 10, 16+10, 11, 16+11, 12, 16+12, 13, 16+13, 14, 16+14, 15, 16+15);
#else
		x86_64_v16qi half{__builtin_ia32_punpcklbw128(chunk, zero)};
		__builtin_memcpy(pDst,__builtin_addressof(half),m128i_size);
		half = __builtin_ia32_punpckhbw128(chunk, zero);
#endif
		__builtin_memcpy(pDst+8,__builtin_addressof(half),m128i_size);
	}
	else if constexpr(sizeof(U)==4)
	{
		x86_64_v16qi const zero{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		x86_64_v16qi chunk;
		__builtin_memcpy(__builtin_addressof(chunk),pSrc,m128i_size);
		mask = static_cast<std::uint_least32_t>(__builtin_ia32_pmovmskb128(chunk));
#if __has_builtin(__builtin_shufflevector)
		x86_64_v16qi half_result{__builtin_shufflevector(chunk, zero, 0, 16+0, 1, 16+1, 2, 16+2, 3, 16+3, 4, 16+4, 5, 16+5, 6, 16+6, 7, 16+7)};
		x86_64_v8hi half;
		__builtin_memcpy(__builtin_addressof(half),__builtin_addressof(half_result),m128i_size);
		x86_64_v8hi const zero8{0, 0, 0, 0, 0, 0, 0, 0};
		x86_64_v8hi qrtr{__builtin_shufflevector(half, zero8, 0, 8+0, 1, 8+1, 2, 8+2, 3, 8+3)};
		__builtin_memcpy(pDst,__builtin_addressof(qrtr),m128i_size);
		qrtr = __builtin_shufflevector(half, zero8, 4, 8+4, 5, 8+5, 6, 8+6, 7, 8+7);
		__builtin_memcpy(pDst+4,__builtin_addressof(qrtr),m128i_size);
		half_result = __builtin_shufflevector(chunk, zero, 8, 16+8, 9, 16+9, 10, 16+10, 11, 16+11, 12, 16+12, 13, 16+13, 14, 16+14, 15, 16+15);
		__builtin_memcpy(__builtin_addressof(half),__builtin_addressof(half_result),m128i_size);
		qrtr=__builtin_shufflevector(half, zero8, 0, 8+0, 1, 8+1, 2, 8+2, 3, 8+3);
		__builtin_memcpy(pDst+8,__builtin_addressof(qrtr),m128i_size);
		qrtr = __builtin_shufflevector(half, zero8, 4, 8+4, 5, 8+5, 6, 8+6, 7, 8+7);
#else
		x86_64_v16qi half_result{__builtin_ia32_punpcklbw128(chunk, zero)};
		x86_64_v8hi half;
		__builtin_memcpy(__builtin_addressof(half),__builtin_addressof(half_result),m128i_size);
		x86_64_v8hi const zero8{0, 0, 0, 0, 0, 0, 0, 0};
		x86_64_v8hi qrtr{__builtin_ia32_punpcklwd128(half, zero8)};
		__builtin_memcpy(pDst,__builtin_addressof(qrtr),m128i_size);
		qrtr = __builtin_ia32_punpckhwd128(half, zero8);
		__builtin_memcpy(pDst+4,__builtin_addressof(qrtr),m128i_size);
		half_result = __builtin_ia32_punpckhbw128(chunk, zero);
		__builtin_memcpy(__builtin_addressof(half),__builtin_addressof(half_result),m128i_size);
		qrtr=__builtin_ia32_punpcklwd128(half, zero8);
		__builtin_memcpy(pDst+8,__builtin_addressof(qrtr),m128i_size);
		qrtr = __builtin_ia32_punpckhwd128(half, zero8);
#endif
		__builtin_memcpy(pDst+12,__builtin_addressof(qrtr),m128i_size);
	}
#else
	using x86_64_m128i = __m128i;
	if constexpr(sizeof(U)==1)
	{
		x86_64_m128i     chunk;
		chunk = _mm_loadu_si128((x86_64_m128i const*) pSrc);     //- Load the register with 8-bit bytes
		mask  = _mm_movemask_epi8(chunk);                   //- Determine which octets have high bit set
		_mm_storeu_si128((x86_64_m128i*) pDst, chunk);           //- Write to memory
	}
	else if constexpr(sizeof(U)==2)
	{
		x86_64_m128i     chunk, half;
		chunk = _mm_loadu_si128((x86_64_m128i const*) pSrc);     //- Load the register with 8-bit bytes
		mask  = _mm_movemask_epi8(chunk);                   //- Determine which octets have high bit set

		half = _mm_unpacklo_epi8(chunk, _mm_set1_epi8(0));  //- Unpack lower half into 16-bit words
		_mm_storeu_si128((x86_64_m128i*) pDst, half);            //- Write to memory

		half = _mm_unpackhi_epi8(chunk, _mm_set1_epi8(0));  //- Unpack upper half into 16-bit words
		_mm_storeu_si128((x86_64_m128i*) (pDst + 8), half);      //- Write to memory

	}
	else
	{
		x86_64_m128i     chunk, half, qrtr, zero;
		zero  = _mm_set1_epi8(0);                           //- Zero out the interleave register
		chunk = _mm_loadu_si128((x86_64_m128i const*) pSrc);     //- Load a register with 8-bit bytes
		mask  = _mm_movemask_epi8(chunk);                   //- Determine which octets have high bit set

		half = _mm_unpacklo_epi8(chunk, zero);              //- Unpack bytes 0-7 into 16-bit words
		qrtr = _mm_unpacklo_epi16(half, zero);              //- Unpack words 0-3 into 32-bit dwords
		_mm_storeu_si128((x86_64_m128i*) pDst, qrtr);            //- Write to memory
		qrtr = _mm_unpackhi_epi16(half, zero);              //- Unpack words 4-7 into 32-bit dwords
		_mm_storeu_si128((x86_64_m128i*) (pDst + 4), qrtr);      //- Write to memory

		half = _mm_unpackhi_epi8(chunk, zero);              //- Unpack bytes 8-15 into 16-bit words
		qrtr = _mm_unpacklo_epi16(half, zero);              //- Unpack words 8-11 into 32-bit dwords
		_mm_storeu_si128((x86_64_m128i*) (pDst + 8), qrtr);      //- Write to memory
		qrtr = _mm_unpackhi_epi16(half, zero);              //- Unpack words 12-15 into 32-bit dwords
		_mm_storeu_si128((x86_64_m128i*) (pDst + 12), qrtr);     //- Write to memory
	}
#endif
	auto const incr{std::countr_zero(static_cast<std::uint_least16_t>(mask))};
	return {pSrc+incr,pDst+incr};
}

#endif

template<std::integral T>
struct advance_with_big_table_unchecked_result
{
	T const* src;
	char32_t cdpt;
};

template<std::integral T>
requires (sizeof(T)==1)
inline constexpr advance_with_big_table_unchecked_result<T> advance_with_big_table_unchecked(T const* it) noexcept
{
	char8_t const* info{first_unit_info[static_cast<char8_t>(*it)]};
	char32_t cdpt{static_cast<char32_t>(*info)};                                //- From it, get the initial code point value
	std::int_least32_t curr{info[1]};                                 //- From it, get the second state
	for(++it;12<curr;)
	{
		char8_t const unit{static_cast<char8_t>(*it)};
		++it;                                 //- Cache the current code unit
		cdpt = (cdpt << 6) | (unit & 0x3F);             //- Adjust code point with continuation bits
		curr = transitions[curr + octet_category[unit]];
		//- Look up the next state
	}
	if(curr==12)[[unlikely]]
		cdpt=0xFFFD;
	return {it,cdpt};
}

template<std::integral T>
struct advance_with_big_table_result
{
	bool failed;
	T const* src;
	char32_t cdpt;
};

template<std::integral T>
requires (sizeof(T)==1)
inline constexpr advance_with_big_table_result<T> advance_with_big_table(T const* first, T const* last) noexcept
{
	char8_t const* info{first_unit_info[static_cast<char8_t>(*first)]};
	char32_t cdpt{static_cast<char32_t>(*info)};                //- From it, get the initial code point value
	std::int_least32_t curr{info[1]};                                 //- From it, get the second state
	auto it{first};
	for(++it;12<curr;)
	{
		if (it < last)[[likely]]
		{
			char8_t const unit{static_cast<char8_t>(*it)};
			++it;                                 //- Cache the current code unit
			cdpt = (cdpt << 6) | (unit & 0x3F);             //- Adjust code point with continuation bits
			curr = transitions[curr + octet_category[unit]];
			//- Look up the next state
		}
		else
			return {true,nullptr,0};
	}
	if(curr==12)[[unlikely]]
		cdpt=0xFFFD;
	return {false,it,cdpt};
}

}

template<std::integral T>
requires (sizeof(T)<=4)
inline constexpr std::size_t get_utf_code_units(char32_t ch,T* ptr) noexcept
{
	return details::codecvt::get_utf_code_units<encoding_scheme::utf>(ch,ptr);
}

}
