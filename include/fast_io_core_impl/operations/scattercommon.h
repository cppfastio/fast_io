#pragma once

namespace fast_io
{

inline constexpr io_scatter_status_t scatter_size_to_status(std::size_t sz,io_scatter_t const *base,::std::size_t len) noexcept
{
	std::size_t total{sz};
	for(std::size_t i{};i!=len;++i)
	{
		::std::size_t blen{base[i].len};
		if(total<blen)[[unlikely]]
			return {sz,i,total};
		total-=blen;
	}
	return {sz,len,0};
}

template<typename T>
inline constexpr ::std::size_t scatter_total_size(basic_io_scatter_t<T> const *base,::std::size_t len) noexcept
{
	std::size_t total{};
	for(auto i{base},e{base+len};i!=e;++i)
	{
		total=::fast_io::details::intrinsics::add_or_overflow_die(total,i->len);
	}
	return total;
}

}
