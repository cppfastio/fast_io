#pragma once

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning( disable : 4820 )
#endif

namespace fast_io::details
{

template<typename flt>
struct iec559_traits;


#if defined(__SIZEOF_FLOAT16__) || defined(__FLOAT16__)
template<>
struct iec559_traits<__float16>
{
	using mantissa_type = std::uint_least16_t;
	inline static constexpr std::size_t mbits{10};
	inline static constexpr std::size_t ebits{5};
	inline static constexpr std::uint_least32_t m10digits{4};
	inline static constexpr std::uint_least32_t m2hexdigits{3};
	inline static constexpr std::uint_least32_t e10digits{2};
	inline static constexpr std::uint_least32_t e2hexdigits{2};
	inline static constexpr std::uint_least32_t e10max{7};
};
#endif

template<>
struct iec559_traits<float>
{
	using mantissa_type = std::uint_least32_t;
	inline static constexpr std::size_t mbits{23};
	inline static constexpr std::size_t ebits{8};
	inline static constexpr std::uint_least32_t m10digits{9};
	inline static constexpr std::uint_least32_t m2hexdigits{6};
	inline static constexpr std::uint_least32_t e10digits{2};
	inline static constexpr std::uint_least32_t e2hexdigits{3};
	inline static constexpr std::uint_least32_t e10max{38};
};

template<>
struct iec559_traits<double>
{
	using mantissa_type = std::uint_least64_t;
	inline static constexpr std::size_t mbits{52};
	inline static constexpr std::size_t ebits{11};
	inline static constexpr std::uint_least32_t m10digits{17};
	inline static constexpr std::uint_least32_t m2hexdigits{13};
	inline static constexpr std::uint_least32_t e10digits{3};
	inline static constexpr std::uint_least32_t e2hexdigits{4};
	inline static constexpr std::uint_least32_t e10max{308};
};

#if defined(__SIZEOF_INT128__)

#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
template<>
struct iec559_traits<__float128>
{
	using mantissa_type = __uint128_t;
	inline static constexpr std::size_t mbits{112};
	inline static constexpr std::size_t ebits{15};
	inline static constexpr std::uint_least32_t m10digits{37};
	inline static constexpr std::uint_least32_t m2hexdigits{28};
	inline static constexpr std::uint_least32_t e10digits{4};
	inline static constexpr std::uint_least32_t e2hexdigits{5};
	inline static constexpr std::uint_least32_t e10max{4966};
};
#endif

#endif

#ifdef __STDCPP_FLOAT16_T__
template<>
struct iec559_traits<_Float16>
{
	using mantissa_type = std::uint_least16_t;
	inline static constexpr std::size_t mbits{10};
	inline static constexpr std::size_t ebits{5};
	inline static constexpr std::uint_least32_t m10digits{4};
	inline static constexpr std::uint_least32_t m2hexdigits{3};
	inline static constexpr std::uint_least32_t e10digits{2};
	inline static constexpr std::uint_least32_t e2hexdigits{2};
	inline static constexpr std::uint_least32_t e10max{7};
};
#endif

#ifdef __STDCPP_FLOAT32_T__
template<>
struct iec559_traits<_Float32>
{
	using mantissa_type = std::uint_least32_t;
	inline static constexpr std::size_t mbits{23};
	inline static constexpr std::size_t ebits{8};
	inline static constexpr std::uint_least32_t m10digits{9};
	inline static constexpr std::uint_least32_t m2hexdigits{6};
	inline static constexpr std::uint_least32_t e10digits{2};
	inline static constexpr std::uint_least32_t e2hexdigits{3};
	inline static constexpr std::uint_least32_t e10max{38};
};
#endif

#ifdef __STDCPP_FLOAT64_T__
template<>
struct iec559_traits<_Float64>
{
	using mantissa_type = std::uint_least64_t;
	inline static constexpr std::size_t mbits{52};
	inline static constexpr std::size_t ebits{11};
	inline static constexpr std::uint_least32_t m10digits{17};
	inline static constexpr std::uint_least32_t m2hexdigits{13};
	inline static constexpr std::uint_least32_t e10digits{3};
	inline static constexpr std::uint_least32_t e2hexdigits{4};
	inline static constexpr std::uint_least32_t e10max{308};
};
#endif

#if defined(__STDCPP_FLOAT128_T__) && defined(__SIZEOF_INT128__)
template<>
struct iec559_traits<_Float128>
{
	using mantissa_type = __uint128_t;
	inline static constexpr std::size_t mbits{112};
	inline static constexpr std::size_t ebits{15};
	inline static constexpr std::uint_least32_t m10digits{37};
	inline static constexpr std::uint_least32_t m2hexdigits{28};
	inline static constexpr std::uint_least32_t e10digits{4};
	inline static constexpr std::uint_least32_t e2hexdigits{5};
	inline static constexpr std::uint_least32_t e10max{4966};
};
#endif

#ifdef __STDCPP_BFLOAT16_T__

/*
To do: change the value
*/
template<>
struct iec559_traits<decltype(0.0bf16)>
{
	using mantissa_type = std::uint_least16_t;
	inline static constexpr std::size_t mbits{7};
	inline static constexpr std::size_t ebits{8};
	inline static constexpr std::uint_least32_t m10digits{4};
	inline static constexpr std::uint_least32_t m2hexdigits{3};
	inline static constexpr std::uint_least32_t e10digits{2};
	inline static constexpr std::uint_least32_t e2hexdigits{2};
	inline static constexpr std::uint_least32_t e10max{7};
};
#endif

template<my_unsigned_integral T>
#if defined(_MSC_VER) && !defined(__clang__)
#if __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
#endif
inline constexpr int my_countr_zero_unchecked(T x) noexcept	//contract: t cannot be zero
{
//referenced from libstdc++
#if defined(__GNUC__) || defined(__clang__)
	constexpr auto Nd = std::numeric_limits<char>::digits*sizeof(T);
	constexpr auto Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto Nd_u = std::numeric_limits<unsigned>::digits;
	if constexpr (Nd <= Nd_u)
		return __builtin_ctz(x);
	else if constexpr (Nd <= Nd_ul)
		return __builtin_ctzl(x);
	else if constexpr (Nd <= Nd_ull)
		return __builtin_ctzll(x);
	else
	{
		static_assert(Nd <= (2 * Nd_ull),
				"Maximum supported integer size is 128-bit");
		constexpr auto max_ull = std::numeric_limits<unsigned long long>::max();
		unsigned long long low = x & max_ull;
		if (low != 0)
			return __builtin_ctzll(low);
		unsigned long long high = x >> Nd_ull;
		return __builtin_ctzll(high) + Nd_ull;
	}
#else
	return std::countr_zero(x);
#endif
}

template<std::floating_point flt>
struct iec559_rep
{
	using mantissa_type = typename iec559_traits<flt>::mantissa_type;
	mantissa_type m;
	std::int_least32_t e;
};

template<bool uppercase,::std::integral char_type>
inline constexpr char_type* prsv_fp_nan_impl(char_type* iter,bool isnan) noexcept
{
	if(isnan)
	{
		if constexpr(uppercase)
		{
			if constexpr(std::same_as<char_type,char>)
				return copy_string_literal("NAN",iter);
			else if constexpr(std::same_as<char_type,wchar_t>)
				return copy_string_literal(L"NAN",iter);
			else if constexpr(std::same_as<char_type,char16_t>)
				return copy_string_literal(u"NAN",iter);
			else if constexpr(std::same_as<char_type,char32_t>)
				return copy_string_literal(U"NAN",iter);
			else
				return copy_string_literal(u8"NAN",iter);
		}
		else
		{
			if constexpr(std::same_as<char_type,char>)
				return copy_string_literal("nan",iter);
			else if constexpr(std::same_as<char_type,wchar_t>)
				return copy_string_literal(L"nan",iter);
			else if constexpr(std::same_as<char_type,char16_t>)
				return copy_string_literal(u"nan",iter);
			else if constexpr(std::same_as<char_type,char32_t>)
				return copy_string_literal(U"nan",iter);
			else
				return copy_string_literal(u8"nan",iter);
		}
	}
	else
	{
		if constexpr(uppercase)
		{
			if constexpr(std::same_as<char_type,char>)
				return copy_string_literal("INF",iter);
			else if constexpr(std::same_as<char_type,wchar_t>)
				return copy_string_literal(L"INF",iter);
			else if constexpr(std::same_as<char_type,char16_t>)
				return copy_string_literal(u"INF",iter);
			else if constexpr(std::same_as<char_type,char32_t>)
				return copy_string_literal(U"INF",iter);
			else
				return copy_string_literal(u8"INF",iter);
		}
		else
		{
			if constexpr(std::same_as<char_type,char>)
				return copy_string_literal("inf",iter);
			else if constexpr(std::same_as<char_type,wchar_t>)
				return copy_string_literal(L"inf",iter);
			else if constexpr(std::same_as<char_type,char16_t>)
				return copy_string_literal(u"inf",iter);
			else if constexpr(std::same_as<char_type,char32_t>)
				return copy_string_literal(U"inf",iter);
			else
				return copy_string_literal(u8"inf",iter);
		}
	}
}

template<bool uppercase,::std::integral char_type>
inline constexpr char_type* prsv_fp_hex_0(char_type* iter) noexcept
{
	if constexpr(uppercase)
	{
		if constexpr(std::same_as<char_type,char>)
			return copy_string_literal("0P+0",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			return copy_string_literal(L"0P+0",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			return copy_string_literal(u"0P+0",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			return copy_string_literal(U"0P+0",iter);
		else
			return copy_string_literal(u8"0P+0",iter);
	}
	else
	{
		if constexpr(std::same_as<char_type,char>)
			return copy_string_literal("0p+0",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			return copy_string_literal(L"0p+0",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			return copy_string_literal(u"0p+0",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			return copy_string_literal(U"0p+0",iter);
		else
			return copy_string_literal(u8"0p+0",iter);
	}
}

template<bool comma=false,::std::integral char_type>
inline constexpr char_type* prsv_fp_hex1d(char_type* iter) noexcept
{
	if constexpr(comma)
	{
		if constexpr(std::same_as<char_type,char>)
			return copy_string_literal("1,",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			return copy_string_literal(L"1,",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			return copy_string_literal(u"1,",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			return copy_string_literal(U"1,",iter);
		else
			return copy_string_literal(u8"1,",iter);
	}
	else
	{
		if constexpr(std::same_as<char_type,char>)
			return copy_string_literal("1.",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			return copy_string_literal(L"1.",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			return copy_string_literal(u"1.",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			return copy_string_literal(U"1.",iter);
		else
			return copy_string_literal(u8"1.",iter);
	}
}

template<bool comma=false,::std::integral char_type>
inline constexpr char_type* prsv_fp_hex0d(char_type* iter) noexcept
{
	if constexpr(comma)
	{
		if constexpr(std::same_as<char_type,char>)
			return copy_string_literal("0,",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			return copy_string_literal(L"0,",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			return copy_string_literal(u"0,",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			return copy_string_literal(U"0,",iter);
		else
			return copy_string_literal(u8"0,",iter);
	}
	else
	{
		if constexpr(std::same_as<char_type,char>)
			return copy_string_literal("0.",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			return copy_string_literal(L"0.",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			return copy_string_literal(u"0.",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			return copy_string_literal(U"0.",iter);
		else
			return copy_string_literal(u8"0.",iter);
	}
}

template<bool uppercase,::std::integral char_type>
inline constexpr char_type* prsv_fp_hex0p0(char_type* iter) noexcept
{
	if constexpr(uppercase)
	{
		if constexpr(std::same_as<char_type,char>)
			return copy_string_literal("0P+0",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			return copy_string_literal(L"0P+0",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			return copy_string_literal(u"0P+0",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			return copy_string_literal(U"0P+0",iter);
		else
			return copy_string_literal(u8"0P+0",iter);
	}
	else
	{
		if constexpr(std::same_as<char_type,char>)
			return copy_string_literal("0p+0",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			return copy_string_literal(L"0p+0",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			return copy_string_literal(u"0p+0",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			return copy_string_literal(U"0p+0",iter);
		else
			return copy_string_literal(u8"0p+0",iter);
	}
}

template<bool uppercase,::std::integral char_type>
inline constexpr char_type* prsv_fp_dece0(char_type* iter) noexcept
{
	if constexpr(uppercase)
	{
		if constexpr(std::same_as<char_type,char>)
			return copy_string_literal("0E+0",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			return copy_string_literal(L"0E+0",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			return copy_string_literal(u"0E+0",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			return copy_string_literal(U"0E+0",iter);
		else
			return copy_string_literal(u8"0E+0",iter);
	}
	else
	{
		if constexpr(std::same_as<char_type,char>)
			return copy_string_literal("0e+0",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			return copy_string_literal(L"0e+0",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			return copy_string_literal(u"0e+0",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			return copy_string_literal(U"0e+0",iter);
		else
			return copy_string_literal(u8"0e+0",iter);
	}
}

template<typename flt>
struct punning_result
{
	typename iec559_traits<flt>::mantissa_type mantissa;
	std::uint_least32_t exponent;
	bool sign;
};

struct
#if __has_cpp_attribute(__gnu__::__packed__)
[[__gnu__::__packed__]]
#endif
float80_result
{
	std::uint_least64_t mantissa;
	std::uint_least16_t exponent;
};

template<typename flt>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr punning_result<flt> get_punned_result(flt f) noexcept
{
	using trait = iec559_traits<flt>;
	using mantissa_type = typename trait::mantissa_type;
	constexpr std::size_t mbits{trait::mbits};
	constexpr std::size_t ebits{trait::ebits};
	constexpr std::size_t total_bits{mbits+ebits};
	constexpr mantissa_type mantissa_mask{(static_cast<mantissa_type>(1)<<mbits)-1};
	constexpr mantissa_type exponent_mask{(static_cast<mantissa_type>(1)<<ebits)-1};

	auto unwrap =
#if defined(__has_builtin)
#if __has_builtin(__builtin_bit_cast)
	__builtin_bit_cast(mantissa_type,f)
#else
	bit_cast<mantissa_type>(f)
#endif
#elif defined(_MSC_VER) && __cpp_lib_bit_cast >= 201806L
	__builtin_bit_cast(mantissa_type,f)
#else
	bit_cast<mantissa_type>(f)
#endif
	;
	return {unwrap&mantissa_mask,static_cast<std::uint_least32_t>((unwrap>>mbits)&exponent_mask),static_cast<bool>((unwrap>>total_bits)&1u)};
}

template<bool showpos,::std::integral char_type>
inline constexpr char_type* print_rsv_fp_sign_impl(char_type* iter,bool sign) noexcept
{
	if constexpr(showpos)
	{
		*iter=sign?char_literal_v<u8'-',char_type>:char_literal_v<u8'+',char_type>;
		++iter;
	}
	else
	{
		if(sign)
		{
			*iter=char_literal_v<u8'-',char_type>;
			++iter;
		}
	}
	return iter;
}

template<::std::integral char_type,my_unsigned_integral U>
#if __has_cpp_attribute(__gnu__::__hot__)
[[__gnu__::__hot__]]
#endif
inline constexpr char_type* prt_rsv_hundred_flt_impl(char_type* iter,U u) noexcept
{
	return non_overlapped_copy_n(::fast_io::details::digits_table<char_type,10,false>+(u<<1),2,iter);
}

template<std::size_t mxdigits,bool indent,::std::integral char_type,my_unsigned_integral U>
inline constexpr char_type* prt_rsv_exponent_impl(char_type* iter,U u) noexcept
{
	if constexpr(mxdigits==0)
		return iter;
	else if constexpr(mxdigits==1)
	{
		*iter=::fast_io::char_literal_add<char_type>(u);
		++iter;
		return iter;
	}
	else
	{
		constexpr U ten{10u};
		constexpr U hundred{100u};
		constexpr U thousand{1000u};
		if constexpr(mxdigits==2)
		{
			if constexpr(indent)
			{
				return prt_rsv_hundred_flt_impl(iter,u);
			}
			else
			{
				if(u>=ten)
					return prt_rsv_hundred_flt_impl(iter,u);
				else
				{
					*iter=::fast_io::char_literal_add<char_type>(u);
					++iter;
					return iter;
				}
			}
		}
		else if constexpr(mxdigits==3)
		{
			if constexpr(indent)
			{
				if(u>=hundred)
				{
					U div100{u/hundred};
					U mod100{u%hundred};
					*iter=::fast_io::char_literal_add<char_type>(div100);
					++iter;
					u=mod100;
				}
				return prt_rsv_hundred_flt_impl(iter,u);
			}
			else
			{
				if(u>=hundred)
				{
					U div100{u/hundred};
					U mod100{u%hundred};
					*iter=::fast_io::char_literal_add<char_type>(div100);
					++iter;
					return prt_rsv_hundred_flt_impl(iter,mod100);
				}
				else if(u<ten)
				{
					*iter=::fast_io::char_literal_add<char_type>(u);
					++iter;
					return iter;
				}
				return prt_rsv_hundred_flt_impl(iter,u);
			}
		}
		else if constexpr(mxdigits==4)
		{
			if constexpr(indent)
			{
				if(u<hundred)
					return prt_rsv_hundred_flt_impl(iter,u);
				std::size_t sz(3);
				if(u>=thousand)
					sz=4;
				print_reserve_integral_main_impl<10,false>(iter+=sz,u,sz);
			}
			else
			{
				std::size_t sz(1);
				if(u>=thousand)
					sz=4;
				else if(u>=hundred)
					sz=3;
				else if(u>=ten)
					sz=2;
				print_reserve_integral_main_impl<10,false>(iter+=sz,u,sz);
			}
			return iter;
		}
		else if constexpr(mxdigits==5)
		{		
			constexpr U tenthousand{10000u};
			if constexpr(indent)
			{
				if(u<hundred)
					return prt_rsv_hundred_flt_impl(iter,u);
				std::size_t sz(3);
				if(u>=tenthousand)
					sz=5;
				else if(u>=thousand)
					sz=4;
				print_reserve_integral_main_impl<10,false>(iter+=sz,u,sz);
			}
			else
			{
				std::size_t sz(1);
				if(u>=tenthousand)
					sz=5;
				else if(u>=thousand)
					sz=4;
				else if(u>=hundred)
					sz=3;
				else if(u>=ten)
					sz=2;
				print_reserve_integral_main_impl<10,false>(iter+=sz,u,sz);
			}
			return iter;
		}
		else
		{
			if constexpr(indent)
			{
				if(u<hundred)
					return prt_rsv_hundred_flt_impl(iter,u);
			}
			std::size_t sz{chars_len<10,false>(u)};
			auto temp{iter+sz};
			print_reserve_integral_main_impl<10,false>(temp,u,sz);
			return temp;
		}
	}
}

}

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
