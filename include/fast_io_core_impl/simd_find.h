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

template<bool findnot,std::size_t vec_size,std::integral char_type>
requires (sizeof(char_type)<=vec_size)&&(vec_size%sizeof(char_type)==0)
inline constexpr char_type const* find_c_space_simd_common_impl(char_type const* first,char_type const* last) noexcept
{
	constexpr char_type lfchct{char_literal_v<u8' ',std::remove_cvref_t<char_type>>};

	constexpr char_type five{5};
	constexpr unsigned N{vec_size/sizeof(char_type)};
	using simd_vector_type = ::fast_io::intrinsics::simd_vector<char_type,N>;
#if (__cpp_lib_bit_cast >= 201806L) && !defined(__clang__)
	constexpr
		simd_vector_type spaces{
			std::bit_cast<simd_vector_type>(characters_array_impl<lfch,char_type,N>)};
	constexpr
		simd_vector_type fives{
			std::bit_cast<simd_vector_type>(characters_array_impl<five,char_type,N>)};
#else
	simd_vector_type zeros;
	zeros.load(characters_array_impl<lfch,char_type,N>.data());
	simd_vector_type fives;
	fives.load(characters_array_impl<5,char_type,N>.data());
#endif
	simd_vector_type simdvec;
	std::size_t diff{static_cast<std::size_t>(last-first)};
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
	return first;
}

#endif

template<char8_t lfch,bool findnot,std::integral char_type>
inline constexpr char_type const* find_simd_constant_common_impl(char_type const* first,char_type const* last) noexcept
{
	constexpr char_type lfchct{char_literal_v<lfch,std::remove_cvref_t<char_type>>};

#if (defined(__GNUC__) || defined(__clang__))
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if(!std::is_constant_evaluated())
#endif
	{
	constexpr bool use_builtin_memchr{
#if (__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1))
#if defined(__GLIBC__) && defined(__GLIBC_MINOR__)
#if (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 26))
//For glibc >= 2.26, we use glibc's memchr implementation under hosted environment
	!findnot
#endif
#endif
#endif
	};
	if constexpr(use_builtin_memchr&&(sizeof(char_type)==1||(sizeof(char_type)==4&&sizeof(wchar_t)==4)))
	{
		std::size_t diff{static_cast<std::size_t>(last-first)};
		if(diff==0)
#if __has_cpp_attribute(likely)
		[[likely]]
#endif
		{
			return first;
		}
		void* ret;
		if constexpr(sizeof(char_type)==1)
		{
			ret = __builtin_memchr(first,lfchct,diff);
		}
		else
		{
			ret = __builtin_wmemchr(first,lfchct,diff);
		}
		if(ret==nullptr)
		{
			return last;
		}
		return reinterpret_cast<char_type const*>(ret);
	}
	else if constexpr(::fast_io::details::can_simd_vector_run_with_cpu_instruction<64>)
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
	}
#endif
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


template<char8_t lfch,bool findnot,std::integral char_type>
inline constexpr char_type const* find_space_common_impl(char_type const* first,char_type const* last) noexcept
{
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

template<std::integral char_type>
inline constexpr char_type const* find_none_c_space_simd_impl(char_type const* first,char_type const* last) noexcept
{
	return find_c_space_common_impl<u8'\n',false>(first,last);
}

template<std::integral char_type>
inline constexpr char_type const* find_c_space_simd_impl(char_type const* first,char_type const* last) noexcept
{
	return find_c_space_common_impl<u8'\n',false>(first,last);
}

}
