#pragma once

namespace fast_io
{
namespace details
{
inline void avr_libc_write_common_impl(FILE* fp, char const* first, char const* last)
{
	for(;first!=last;++first)
		if(noexcept_call(fp->put,*first,fp))[[unlikely]]
			throw_posix_error(EINVAL);
}

inline void avr_libc_scatter_write_impl_with_normal_write(FILE* fp,io_scatter_t const* scatters,std::size_t n)
{
	auto put_func{fp->put};
	if(put_func==nullptr)
		throw_posix_error(EINVAL);
	for(std::size_t i{};i!=n;++i)
	{
		char const* bs{reinterpret_cast<char const*>(scatters[i].base)};
		avr_libc_write_common_impl(fp,bs,bs+scatters[i].len);
	}
}
inline void avr_libc_write_internal_impl(FILE* fp, char const* first, char const* last)
{
	auto put_func{fp->put};
	if(put_func==nullptr)
		throw_posix_error(EINVAL);
	avr_libc_write_common_impl(fp,first,last);
}

inline char* avr_libc_read_internal_impl(FILE* fp,char* first,char* last)
{
	auto get_func{fp->get};
	if(get_func==nullptr)
		throw_posix_error(EINVAL);
	for (;first!=last;++first)
	{
		int ret{noexcept_call(getc,fp)};
		if(ret==EOF)
			return first;
		*first=static_cast<char>(static_cast<char unsigned>(ret));
	}
	return last;
}

}

template<std::integral char_type,::std::contiguous_iterator Iter>
inline constexpr void write(basic_c_family_io_observer<c_family::emulated_unlocked,char_type> ciob,Iter first,Iter last)
{
	::fast_io::details::avr_libc_write_internal_impl(ciob.fp,reinterpret_cast<char const*>(::std::to_address(first)),
					reinterpret_cast<char const*>(::std::to_address(last)));
}

template<std::integral char_type>
inline constexpr void scatter_write(basic_c_family_io_observer<c_family::emulated_unlocked,char_type> ciob,io_scatters_t scatters)
{
	::fast_io::details::avr_libc_scatter_write_impl_with_normal_write(ciob.fp,scatters.base,scatters.len);
}

template<std::integral char_type,::std::contiguous_iterator Iter>
inline constexpr Iter read(basic_c_family_io_observer<c_family::emulated_unlocked,char_type> ciob,Iter first,Iter last)
{
	auto first_addr{reinterpret_cast<char*>(::std::to_address(first))};
	return (::fast_io::details::avr_libc_read_internal_impl(ciob.fp,
	first_addr,reinterpret_cast<char*>(::std::to_address(last)))-first_addr)+first;
}

template<std::integral char_type>
requires (sizeof(char_type)==sizeof(char))
inline void try_unget(basic_c_family_io_observer<c_family::emulated_unlocked,char_type> ciob,char_type ch) noexcept
{
	noexcept_call(::ungetc,static_cast<int>(ch),ciob.fp);
}

}
