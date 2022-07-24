#pragma once

namespace fast_io
{

namespace details
{

template<::std::integral char_type>
inline constexpr char_type* output_iso8601_subseconds_main(char_type* iter,std::uint_least64_t subseconds) noexcept
{
	constexpr std::size_t digitsm1(std::numeric_limits<std::uint_least64_t>::digits10);
	std::size_t sz(digitsm1);
	for(;subseconds%10==0;--sz)
		subseconds/=10;
	print_reserve_integral_main_impl<10,false>(iter+=sz,subseconds,sz);
	return iter;
}

template<bool comma=false, ::std::integral char_type>
inline constexpr char_type* output_iso8601_subseconds(char_type* iter,std::uint_least64_t subseconds) noexcept
{
	*iter=char_literal_v<(comma?u8',':u8'.'),char_type>;
	++iter;
	return output_iso8601_subseconds_main(iter,subseconds);
}

inline constexpr std::uint_least64_t cal_uint_least64_d10_max() noexcept
{
	std::size_t v(std::numeric_limits<std::uint_least64_t>::digits10);
	std::uint_least64_t value{1};
	for(std::size_t i{};i!=v;++i)
		value*=10u;
	return value;
}

template<std::unsigned_integral T>
inline constexpr auto cal_uint_least64_d10_all_table() noexcept
{
	::fast_io::freestanding::array<T,std::numeric_limits<T>::digits10> array;
	array.back()=1;
	for(std::size_t i{array.size()-1};i--;)
		array[i]=array[i+1]*10u;
	return array;
}

template<std::unsigned_integral T>
inline constexpr auto d10_reverse_table{cal_uint_least64_d10_all_table<T>()};
}

inline constexpr std::uint_least64_t uint_least64_subseconds_per_second{details::cal_uint_least64_d10_max()};

}

#include"iso8601.h"
