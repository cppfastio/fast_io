#pragma once

namespace fast_io
{
namespace details
{

template<::std::forward_iterator Iter,typename T,std::size_t size>
inline constexpr Iter print_reserve_df_mm_impl(Iter iter,::fast_io::intrinsics::simd_vector<T,size> const& mm) noexcept
{
	if constexpr(size==0)
		return iter;
	using char_type = ::std::iter_value_t<Iter>;
	*iter=char_literal_v<u8'(',char_type>;
	++iter;
	iter=print_reserve_define(io_reserve_type<char_type,T>,iter,mm[0]);
	for(std::size_t i{1};i!=size;++i)
	{
		*iter=char_literal_v<u8',',char_type>;
		++iter;
		iter=print_reserve_define(io_reserve_type<char_type,T>,iter,mm[i]);
	}
	*iter=char_literal_v<u8')',char_type>;
	++iter;
	return iter;
}

}


template<std::integral char_type,typename T,std::size_t n>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,intrinsics::simd_vector<T,n>>) noexcept
{
	constexpr std::size_t p{print_reserve_size(io_reserve_type<char_type,T>)};
	constexpr std::size_t sz{(1+p)*n+1};
	return sz;
}
template<::std::forward_iterator Iter,typename T,std::size_t n>
inline constexpr Iter print_reserve_define(io_reserve_type_t<::std::iter_value_t<Iter>,intrinsics::simd_vector<T,n>>,Iter iter,intrinsics::simd_vector<T,n> const& t) noexcept
{
	return details::print_reserve_df_mm_impl(iter,t);
}
}
