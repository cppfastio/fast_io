#pragma once

namespace fast_io::details
{

template<char8_t lfch,bool findnot,std::integral char_type>
inline constexpr char_type const* find_simd_constant_common_impl(char_type const* first,char_type const* last) noexcept
{
	constexpr char_type lfchct{char_literal_v<lfch,std::remove_cvref_t<char_type>>};

#if (defined(__GNUC__) || defined(__clang__)) && defined(__SSE4_1__) && defined(__x86_64__)
	if constexpr(sizeof(char_type)==1)
	{
		constexpr
			::fast_io::intrinsics::simd_vector<char,16> zeros{lfch,lfch,lfch,lfch,
						lfch,lfch,lfch,lfch,
						lfch,lfch,lfch,lfch,
						lfch,lfch,lfch,lfch};
		::fast_io::intrinsics::simd_vector<char,16> simdvec;
		std::size_t diff{static_cast<std::size_t>(last-first)};
		for(;diff>=16;diff-=16)
		{
			simdvec.load(first);
			::fast_io::intrinsics::simd_vector<char,16> chunk{zeros==simdvec};
			std::uint_least32_t mask{static_cast<std::uint_least32_t>(__builtin_ia32_pmovmskb128(chunk))};
			int incr;
			if constexpr(findnot)
			{
				incr=std::countr_one(static_cast<std::uint_least16_t>(mask));
			}
			else
			{
				incr=std::countr_zero(static_cast<std::uint_least16_t>(mask));
			}
			if(incr!=16)
			{
				return first+incr;
			}
			first+=16;
		}

	}
#endif
	if constexpr(findnot)
	{
		return ::fast_io::freestanding::find_not(first,last,lfchct);
	}
	else
	{
		return ::fast_io::freestanding::find(first,last,lfchct);
	}
}


template<std::integral char_type>
inline constexpr char_type const* find_lf_simd_impl(char_type const* first,char_type const* last) noexcept
{
	return find_simd_constant_common_impl<u8'\n',false>(first,last);
}

template<std::integral char_type>
inline constexpr char_type const* find_none_zero_simd_impl(char_type const* first,char_type const* last) noexcept
{
	return find_simd_constant_common_impl<u8'0',true>(first,last);
}

}
