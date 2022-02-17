#pragma once

namespace fast_io::details
{

template<std::integral char_type>
inline constexpr char_type const* find_lf_simd_impl(char_type const* first,char_type const* last) noexcept
{
	constexpr char_type lfchct{char_literal_v<u8'\n',std::remove_cvref_t<char_type>>};
#if (defined(__GNUC__) || defined(__clang__)) && defined(__SSE4_1__) && defined(__x86_64__)
	if constexpr(sizeof(char_type)==1)
	{
		constexpr char lfch{static_cast<char>(lfchct)};
		constexpr ::fast_io::intrinsics::simd_vector<char,16> zeros{lfch,lfch,lfch,lfch,
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
			auto const incr{std::countr_zero(static_cast<std::uint_least16_t>(mask))};
			if(incr!=16)
			{
				return first+incr;
			}
			first+=16;
		}
		for(;first!=last&&*first!=lfch;++first);
		return first;
	}
	else
#endif
	{
		return ::fast_io::freestanding::find(first,last,lfchct);
	}
}

}

