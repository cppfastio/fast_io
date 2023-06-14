#pragma once

namespace fast_io
{

namespace details
{

template<::std::integral char_type>
inline constexpr deco_result<char_type,char_type>
	simd_lf_crlf_process_chars(char_type const *fromfirst,char_type const *fromlast,
			char_type *tofirst,char_type *tolast) noexcept
{
	constexpr std::size_t initialdiffn{::fast_io::details::optimal_simd_vector_run_with_cpu_instruction_size};
	constexpr std::size_t initialdiffndf{initialdiffn+1u};
	constexpr unsigned N{initialdiffn/sizeof(char_type)};
	using simd_vector_type = ::fast_io::intrinsics::simd_vector<char_type,N>;
	constexpr char_type lfchct{char_literal_v<u8'\n',std::remove_cvref_t<char_type>>};
	constexpr char_type lfchr{char_literal_v<u8'\r',std::remove_cvref_t<char_type>>};
#if (__cpp_lib_bit_cast >= 201806L) && !defined(__clang__)
	constexpr
	simd_vector_type charsvec{
		std::bit_cast<simd_vector_type>(characters_array_impl<lfchct,char_type,N>)};
#else
	simd_vector_type charsvec;
	charsvec.load(characters_array_impl<lfchct,char_type,N>.data());
#endif
	for(simd_vector_type vec;;)
	{
		::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
		::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
		if(fromdiff<initialdiffn||todiff<initialdiffndf)
		{
			break;
		}
		vec.load(fromfirst);
		auto comres{vec!=charsvec};
		vec.store(tofirst);
		if(::fast_io::intrinsics::is_all_zeros(comres))
		{
			fromfirst+=initialdiffn;
			tofirst+=initialdiffn;
			continue;
		}
		unsigned pos{::fast_io::intrinsics::vector_mask_countr_one(comres)};
		fromfirst+=pos+1;
		tofirst+=pos;
		*tofirst=lfchr;
		++tofirst;
		*tofirst=lfchct;
		++tofirst;
	}
	return {fromfirst,tofirst};
}


template<::std::integral char_type>
inline constexpr deco_result<char_type,char_type>
	simd_crlf_lf_process_chars(char_type const *fromfirst,char_type const *fromlast,
			char_type *tofirst,char_type *tolast) noexcept
{
	constexpr std::size_t initialdiffn{::fast_io::details::optimal_simd_vector_run_with_cpu_instruction_size};
	constexpr std::size_t initialdiffndf{initialdiffn+1u};
	constexpr unsigned N{initialdiffn/sizeof(char_type)};
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
		if(fromdiff<initialdiffndf||todiff<initialdiffn)
		{
			break;
		}
		vec.load(fromfirst);
		auto comres{vec!=charsvec};
		vec.store(tofirst);
		if(::fast_io::intrinsics::is_all_zeros(comres))
		{
			fromfirst+=initialdiffn;
			tofirst+=initialdiffn;
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
		tofirst[-1]=lfchct;
	}
	return {fromfirst,tofirst};
}

}

}
