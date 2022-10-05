#pragma once

namespace fast_io
{

namespace details
{

inline constexpr bool denom_is_natural_pow10(std::int_least64_t denom) noexcept
{
	if(denom<0)
		return false;
	std::uint_least64_t den{static_cast<std::uint_least64_t>(denom)};
	for(;!(den%10u);den/=10u);
	return den==1;
}

}
#if !defined(__MSDOS__) && !defined(__AVR__) && (!defined(__MINGW32__) || __has_include(<_mingw_stat64.h>))

template<std::integral char_type>
constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,struct timespec>) noexcept
{
	return print_reserve_size(io_reserve_type<char_type,std::time_t>)+1+print_reserve_size(io_reserve_type<char_type,long>);
}

namespace details
{

inline constexpr std::size_t denom_natural_log10(std::uint_least64_t den) noexcept
{
	std::size_t logarithm{};
	for(;!(den%10u);den/=10u)
		++logarithm;
	return logarithm;
}

template<std::uint_least64_t lim,std::integral char_type,::std::random_access_iterator Iter,details::my_unsigned_integral uint_type>
inline constexpr Iter subseconds_part_print_sz_impl(Iter it,uint_type nsec) noexcept
{
	if constexpr(std::numeric_limits<uint_type>::max()>=lim)
	{
		constexpr uint_type lmt{static_cast<uint_type>(lim)};
		if((!nsec)||lmt<=nsec)
			return it;
	}
	else
	{
		if(!nsec)
			return it;
	}
	*it=char_literal_v<u8'.',char_type>;
	++it;
	constexpr std::size_t lgsz{denom_natural_log10(lim)};
	std::size_t sz{lgsz};
	for(;nsec%10==0;--sz)
		nsec/=10;
	if constexpr(sizeof(uint_type)<=sizeof(unsigned))
		with_length_output_unsigned(it,static_cast<unsigned>(nsec),sz);
	else
		with_length_output_unsigned(it,nsec,sz);
	return it+sz;
}

template<std::integral char_type,::std::random_access_iterator Iter>
inline constexpr Iter timespec_print_impl(Iter it,struct timespec spc) noexcept
{
	return subseconds_part_print_sz_impl<1000000000UL,char_type>(
		print_reserve_define(io_reserve_type<char_type,std::time_t>,it,spc.tv_sec),
		static_cast<unsigned long>(spc.tv_nsec));
}

}

template<std::integral char_type>
constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,struct timespec>, char_type* it,struct timespec spc) noexcept
{
	return details::timespec_print_impl<char_type>(it,spc);	
}

inline constexpr unix_timestamp timespec_to_unix_timestamp(struct timespec tsc) noexcept
{
	constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/1000000000u};
	return {static_cast<std::int_least64_t>(tsc.tv_sec),static_cast<std::uint_least64_t>(tsc.tv_nsec)*mul_factor};
}

#endif

}
