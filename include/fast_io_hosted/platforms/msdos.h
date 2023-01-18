#pragma once

namespace fast_io
{
/*
MSDOS should not contain any issues with multiple write calls
*/

namespace details
{

inline io_scatter_status_t posix_scatter_read_impl_with_normal_read(int fd,io_scatters_t scatters)
{
	std::size_t total_size{};
	for(std::size_t i{};i!=scatters.len;++i)
	{
		auto e{scatters.base[i]};
		auto ret{noexcept_call(::read,fd,const_cast<void*>(e.base),e.len)};
		if(ret==-1)
			throw_posix_error();
		std::size_t tsize{static_cast<std::size_t>(ret)};
		total_size+=tsize;
		if(tsize!=e.len)
			return {total_size,i,tsize};
	}
	return {total_size,scatters.len,0};
}

inline io_scatter_status_t posix_scatter_write_impl_with_normal_write(int fd,io_scatters_t scatters)
{
	std::size_t total_size{};
	for(std::size_t i{};i!=scatters.len;++i)
	{
		auto e{scatters.base[i]};
		auto ret{noexcept_call(::write,fd,e.base,e.len)};
		if(ret==-1)
			throw_posix_error();
		std::size_t tsize{static_cast<std::size_t>(ret)};
		total_size+=tsize;
		if(tsize!=e.len)
			return {total_size,i,tsize};
	}
	return {total_size,scatters.len,0};
}

}

template<std::integral char_type>
inline io_scatter_status_t scatter_read(basic_posix_io_observer<char_type> piob,io_scatters_t scatters)
{
	return details::posix_scatter_read_impl_with_normal_read(piob.fd,scatters);
}

template<std::integral char_type>
inline io_scatter_status_t scatter_write(basic_posix_io_observer<char_type> piob,io_scatters_t scatters)
{
	return details::posix_scatter_write_impl_with_normal_write(piob.fd,scatters);
}

}
