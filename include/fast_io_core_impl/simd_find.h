#pragma once

namespace fast_io::details
{
#if (defined(__GNUC__) || defined(__clang__))

template<typename T>
inline constexpr T create_simd_vector_with_single_value(typename T::value_type value) noexcept
{
	::fast_io::freestanding::array<typename T::value_type,T::size()> arr;
	for(auto& e : arr)
	{
		e=value;
	}
#if __cpp_lib_bit_cast >= 201806L
	return std::bit_cast<T>(arr);
#else
	return arr;
#endif
}

template<char8_t lfch,std::integral char_type,std::size_t N>
inline constexpr
	::fast_io::freestanding::array<char_type,N> characters_array_impl{
		create_simd_vector_with_single_value<::fast_io::freestanding::array<char_type,N>>(lfch)};

template<char8_t lfch,bool findnot,std::size_t vec_size,bool single_round=false,std::integral char_type>
requires (sizeof(char_type)<=vec_size)&&(vec_size%sizeof(char_type)==0)
inline constexpr char_type const* find_simd_constant_simd_common_impl(char_type const* first,char_type const* last) noexcept
{
	constexpr char_type lfchct{char_literal_v<lfch,std::remove_cvref_t<char_type>>};
	constexpr unsigned N{vec_size/sizeof(char_type)};
	using simd_vector_type = ::fast_io::intrinsics::simd_vector<char_type,N>;
#if (__cpp_lib_bit_cast >= 201806L) && !defined(__clang__)
	constexpr
		simd_vector_type zeros{
			std::bit_cast<simd_vector_type>(characters_array_impl<lfch,char_type,N>)};
#else
	simd_vector_type zeros;
	zeros.load(characters_array_impl<lfch,char_type,N>.data());
#endif
	simd_vector_type simdvec;
	std::size_t diff{static_cast<std::size_t>(last-first)};
	if constexpr(single_round)
	{
		if(N<=diff)
		{
			simdvec.load(first);
			simd_vector_type chunk{zeros==simdvec};
			unsigned incr;
			if constexpr(findnot)
			{
				incr=vector_mask_countr_one(chunk);
			}
			else
			{
				incr=vector_mask_countr_zero(chunk);
			}
			if(incr!=N)
			{
				return first+incr;
			}
			first+=N;
		}
		return first;
	}
	else
	{
		for(;N<=diff;diff-=N)
		{
			simdvec.load(first);
			simd_vector_type chunk{zeros==simdvec};
			unsigned incr;
			if constexpr(findnot)
			{
				incr=vector_mask_countr_one(chunk);
			}
			else
			{
				incr=vector_mask_countr_zero(chunk);
			}
			if(incr!=N)
			{
				return first+incr;
			}
			first+=N;
		}
	}
	return first;
}
#endif

template<char8_t lfch,bool findnot,std::integral char_type>
inline constexpr char_type const* find_simd_constant_common_impl(char_type const* first,char_type const* last) noexcept
{
	constexpr char_type lfchct{char_literal_v<lfch,std::remove_cvref_t<char_type>>};

#if (defined(__GNUC__) || defined(__clang__))
	if constexpr(::fast_io::details::can_simd_vector_run_with_cpu_instruction<64>)
	{
		first=find_simd_constant_simd_common_impl<lfch,findnot,64>(first,last);
	}
	else if constexpr(::fast_io::details::can_simd_vector_run_with_cpu_instruction<32>)
	{
		first=find_simd_constant_simd_common_impl<lfch,findnot,32>(first,last);
	}
	else if constexpr(::fast_io::details::can_simd_vector_run_with_cpu_instruction<16>)
	{
		first=find_simd_constant_simd_common_impl<lfch,findnot,16>(first,last);
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
