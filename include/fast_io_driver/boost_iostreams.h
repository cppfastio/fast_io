#pragma once
#include<ios>
#include"../fast_io_core_impl/seek.h"

namespace fast_io
{

template<typename src_type>
concept boost_iostreams_input_device=requires(src_type& src,char* s,std::streamsize n)
{
	{src.read(s,n)}->std::same_as<std::streamsize>;
};

template<typename src_type>
concept boost_iostreams_output_device=requires(src_type& src,char const* s,std::streamsize n)
{
	{src.write(s,n)}->std::same_as<std::streamsize>;
};

template<typename src_type>
concept boost_iostreams_any_device=boost_iostreams_input_device<src_type>||boost_iostreams_output_device<src_type>;

template<typename src_type>
concept boost_iostreams_io_device=boost_iostreams_input_device<src_type>&&boost_iostreams_output_device<src_type>;

template<typename src_type>
concept boost_iostreams_flushable_device=boost_iostreams_any_device<src_type>&&requires(src_type& src)
{
	{src.flush()};
};

template<typename src_type>
concept boost_iostream_seekable_device=boost_iostreams_any_device<src_type>&&requires(src_type& src,std::intmax_t offset,std::ios::seekdir dir)
{
	{src.seek(offset,dir)}->std::convertible_to<std::intmax_t>;
};

template<std::integral ch_type, boost_iostreams_any_device src_type>
class basic_boost_iostreams
{
public:
	using char_type = ch_type;
	using source_type = src_type;
	source_type source;
};

template<std::integral ch_type, boost_iostreams_input_device src_type,::std::contiguous_iterator Iter>
inline Iter read(basic_boost_iostreams<ch_type,src_type>& in_device,Iter begin,Iter end)
{
	auto ret{in_device.source.read(reinterpret_cast<char*>(::std::to_address(begin)),(end-begin)*sizeof(*begin))};
	if(ret==-1)
		return begin;
	return begin+ret/sizeof(*begin);
}

template<std::integral ch_type, boost_iostreams_output_device src_type,::std::contiguous_iterator Iter>
inline Iter write(basic_boost_iostreams<ch_type,src_type>& out_device,Iter begin,Iter end)
{
	auto ret{out_device.source.write(reinterpret_cast<char const*>(::std::to_address(begin)),(end-begin)*sizeof(*begin))};
	if(ret==-1)
		return begin;
	return begin+ret/sizeof(*begin);
}
template<std::integral ch_type, boost_iostreams_flushable_device src_type>
inline void flush(basic_boost_iostreams<ch_type,src_type>& out_device)
{
	out_device.source.flush();
}

template<std::integral ch_type, boost_iostream_seekable_device src_type,::std::contiguous_iterator Iter,typename T,std::integral U>
inline auto seek(basic_boost_iostreams<ch_type,src_type>& dev,seek_type_t<T>,U i,seekdir s=seekdir::beg)
{
	return dev.seek(seek_precondition<std::intmax_t,T,ch_type>(i),static_cast<std::ios::seekdir>(static_cast<int>(s)));
}

template<std::integral ch_type, boost_iostream_seekable_device src_type,std::integral U>
inline auto seek(basic_boost_iostreams<ch_type,src_type> cfhd,U i,seekdir s=seekdir::beg)
{
	return seek(cfhd,seek_type<ch_type>,i,s);
}

template<boost_iostreams_any_device src_type>
using boost_iostreams = basic_boost_iostreams<char,src_type>;
template<boost_iostreams_any_device src_type>
using u8boost_iostreams = basic_boost_iostreams<char8_t,src_type>;
template<boost_iostreams_any_device src_type>
using wboost_iostreams = basic_boost_iostreams<wchar_t,src_type>;
}
