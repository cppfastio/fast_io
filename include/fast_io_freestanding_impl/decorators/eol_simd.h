#pragma once

namespace fast_io
{

namespace details
{

template<bool cr=false,::std::integral char_type>
inline constexpr deco_result<char_type,char_type>
	simd_lf_crlf_process_chars(char_type const *fromfirst,char_type const *fromlast,
			char_type *tofirst,char_type *tolast) noexcept
{
	constexpr std::size_t initialdiffn{::fast_io::details::optimal_simd_vector_run_with_cpu_instruction_size};
	constexpr unsigned N{initialdiffn/sizeof(char_type)};
	using simd_vector_type = ::fast_io::intrinsics::simd_vector<char_type,N>;
	constexpr char_type lfchct{char_literal_v<u8'\n',std::remove_cvref_t<char_type>>};
	constexpr char_type lfchr{char_literal_v<u8'\r',std::remove_cvref_t<char_type>>};

	constexpr char_type tofdch{cr?lfchr:lfchct};
#if (__cpp_lib_bit_cast >= 201806L) && !defined(__clang__)
	constexpr
	simd_vector_type charsvec{
		std::bit_cast<simd_vector_type>(characters_array_impl<tofdch,char_type,N>)};
#else
	simd_vector_type charsvec;
	charsvec.load(characters_array_impl<tofdch,char_type,N>.data());
#endif
	simd_vector_type vec;
	for(;fromfirst!=fromlast&&tofirst!=tolast;++fromfirst)
	{
		if(*fromfirst!=tofdch)
		{
			*tofirst=*fromfirst;
			++fromfirst;
			++tofirst;
			::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
			::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
			if(todiff<fromdiff)
			{
				fromdiff=todiff;
			}
			fromdiff/=N;
			for(;fromdiff;--fromdiff)
			{
				vec.load(fromfirst);
				auto comres{vec!=charsvec};
				if(!::fast_io::intrinsics::is_all_zeros(comres))
				{
					break;
				}
				vec.store(tofirst);
				fromfirst+=N;
				tofirst+=N;
			}
			if(!fromdiff)
			{
				break;
			}
			::std::size_t fromdiff2{static_cast<::std::size_t>(fromlast-fromfirst)};
			::std::size_t todiff2{static_cast<::std::size_t>(tolast-tofirst)};
			if(todiff2<fromdiff2)
			{
				fromdiff2=todiff2;
			}
			for(;fromdiff2;--fromdiff2)
			{
				auto ch{*fromfirst};
				if(ch==tofdch)
				{
					break;
				}
				*tofirst=ch;
				++fromfirst;
				++tofirst;
			}
			if(!fromdiff2)
			{
				break;
			}
		}
		if constexpr(cr)
		{
			*tofirst=lfchct;
			++tofirst;
			*tofirst=lfchr;
			++tofirst;
		}
		else
		{
			*tofirst=lfchr;
			++tofirst;
			*tofirst=lfchct;
			++tofirst;
		}
	}
	return {fromfirst,tofirst};
}


template<bool cr=false,::std::integral char_type>
inline constexpr deco_result<char_type,char_type>
	simd_crlf_lf_process_chars(char_type const *fromfirst,char_type const *fromlast,
			char_type *tofirst,char_type *tolast) noexcept
{
	constexpr std::size_t initialdiffn{::fast_io::details::optimal_simd_vector_run_with_cpu_instruction_size};
	constexpr unsigned N{initialdiffn/sizeof(char_type)};
	constexpr unsigned Np1{N+1u};
	using simd_vector_type = ::fast_io::intrinsics::simd_vector<char_type,N>;
	constexpr char_type lfchct{char_literal_v<u8'\n',std::remove_cvref_t<char_type>>};
	constexpr char_type lfchr{char_literal_v<u8'\r',std::remove_cvref_t<char_type>>};
#if (__cpp_lib_bit_cast >= 201806L) && !defined(__clang__)
	constexpr
	simd_vector_type charsvec{
		std::bit_cast<simd_vector_type>(characters_array_impl<lfchr,char_type,N>)};
#else
	simd_vector_type charsvec;
	charsvec.load(characters_array_impl<lfchr,char_type,N>.data());
#endif
	for(simd_vector_type vec;;)
	{
		::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
		::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
		if(fromdiff<Np1||todiff<N)
		{
			break;
		}
		vec.load(fromfirst);
		auto comres{vec!=charsvec};
		vec.store(tofirst);
		if(::fast_io::intrinsics::is_all_zeros(comres))
		{
			fromfirst+=N;
			tofirst+=N;
			continue;
		}
		unsigned pos{::fast_io::intrinsics::vector_mask_countr_one(comres)};
		fromfirst+=pos+1;
		tofirst+=pos+1;
		if(*fromfirst!=lfchct)
		{
			continue;
		}
		++fromfirst;
		if constexpr(cr)
		{
			tofirst[-1]=lfchr;
		}
		else
		{
			tofirst[-1]=lfchct;
		}
	}
	return {fromfirst,tofirst};
}

template<bool cr,::std::integral char_type>
inline constexpr deco_result<char_type,char_type>
	simd_lf_cr_process_chars(char_type const *fromfirst,char_type const *fromlast,
			char_type *tofirst,char_type *tolast) noexcept
{
	constexpr std::size_t initialdiffn{::fast_io::details::optimal_simd_vector_run_with_cpu_instruction_size};
	constexpr unsigned N{initialdiffn/sizeof(char_type)};
	using simd_vector_type = ::fast_io::intrinsics::simd_vector<char_type,N>;
	constexpr char_type lfchct{char_literal_v<u8'\n',std::remove_cvref_t<char_type>>};
	constexpr char_type lfchr{char_literal_v<u8'\r',std::remove_cvref_t<char_type>>};
	constexpr char_type tofdch{cr?lfchr:lfchct};
#if (__cpp_lib_bit_cast >= 201806L) && !defined(__clang__)
	constexpr
	simd_vector_type charsvec{
		std::bit_cast<simd_vector_type>(characters_array_impl<tofdch,char_type,N>)};
	constexpr
	simd_vector_type threevec{
		std::bit_cast<simd_vector_type>(characters_array_impl<3,char_type,N>)};
#else
	simd_vector_type charsvec;
	charsvec.load(characters_array_impl<tofdch,char_type,N>.data());
	simd_vector_type threevec;
	threevec.load(characters_array_impl<3,char_type,N>.data());
#endif

	for(simd_vector_type vec;;)
	{
		::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
		::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
		if(fromdiff<N||todiff<N)
		{
			break;
		}
		vec.load(fromfirst);
		auto comres{vec==charsvec};
		auto comresandres{comres&threevec};
		if constexpr(cr)
		{
			vec-=comresandres;
		}
		else
		{
			vec+=comresandres;
		}
		vec.store(tofirst);
		fromfirst+=N;
		tofirst+=N;
	}
	return {fromfirst,tofirst};
}

}

}
