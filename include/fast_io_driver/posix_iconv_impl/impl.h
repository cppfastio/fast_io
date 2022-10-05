#pragma once

#include<iconv.h>

namespace fast_io
{

class posix_iconv_io_observer
{
public:
	using native_handle_type = iconv_t;
	iconv_t cd{std::bit_cast<iconv_t>(static_cast<uintptr_t>(-1))};

	constexpr auto& native_handle() noexcept
	{
		return cd;
	}
	constexpr auto& native_handle() const noexcept
	{
		return cd;
	}
	constexpr iconv_t release() noexcept
	{
		auto temp{cd};
		cd = std::bit_cast<iconv_t>(static_cast<uintptr_t>(-1));
		return temp;
	}
};

struct iconv_deco_t
{
	iconv_t cd{std::bit_cast<iconv_t>(static_cast<uintptr_t>(-1))}; 
	char buffer[32]{};
	std::size_t len{};
	iconv_deco_t(posix_iconv_io_observer piiob) noexcept:cd(piiob.cd){}
};

inline constexpr posix_iconv_io_observer deco_value_handle(posix_iconv_io_observer piiob) noexcept
{
	return piiob;
}

namespace details
{

struct iconv_deco_from_chars_result
{
	char const* from;
	char* to;
};

inline iconv_deco_from_chars_result iconv_do_impl(iconv_t cd,
	char const* from_first2,
	char const* from_last2,
	char* to_first,
	char* to_last) noexcept
{
	char* from_first{const_cast<char*>(from_first2)};
	char* from_last{const_cast<char*>(from_last2)};
	while(from_first!=from_last)
	{
		std::size_t inbytes_left{static_cast<std::size_t>(from_last-from_first)};
		std::size_t outbytes_left{static_cast<std::size_t>(to_last-to_first)};
		std::size_t ret{iconv(cd,
		__builtin_addressof(from_first),
		__builtin_addressof(inbytes_left),
		__builtin_addressof(to_first),
		__builtin_addressof(outbytes_left))};
		from_first=from_last-inbytes_left;
		to_first=to_last-outbytes_left;
		if(ret!=static_cast<std::size_t>(-1))
			break;
		switch(errno)
		{
		case EINVAL:
			goto lable_end;
		case EILSEQ:
			if(from_first==from_last)
				break;
			*to_first=0;
			++to_first;
			++from_first;
		break;
		default:
			fast_terminate();
		};
	}
lable_end:
	if(16<from_last-from_first)[[unlikely]]
		fast_terminate();
	return {from_first,to_first};
}

inline std::size_t iconv_print_reserve_define_impl(iconv_t cd,
	char const* inbyteptr,
	std::size_t inbytesize,
	char* outbyteptr) noexcept
{
	auto out_bytes{inbytesize*8};
	auto end_ptr{inbyteptr+inbytesize};
	auto [src_it,dst_it]=iconv_do_impl(cd,inbyteptr,end_ptr,outbyteptr,outbyteptr+out_bytes);
	if(src_it!=end_ptr)
	{
		*dst_it=0;
		++dst_it;
	}
	return dst_it-outbyteptr;
}

inline char* iconv_dst_impl(
	iconv_deco_t& icdt,char const* first,char const* last,
	char* dst_first,char* dst_last) noexcept
{
	std::size_t len{icdt.len};
	std::size_t diff{static_cast<std::size_t>(last-first)};

	if(len)
	{
		std::size_t to_copy{32-len};
		if(diff<to_copy)
			to_copy=diff;
		std::size_t total_len{len+to_copy};
		non_overlapped_copy_n(first,to_copy,icdt.buffer+len);
		auto buffer_total_ed{icdt.buffer+total_len};
		auto [src_it,dst_it]=iconv_do_impl(icdt.cd,
			icdt.buffer,
			buffer_total_ed,
			dst_first,
			dst_last);
		std::size_t converted{static_cast<std::size_t>(src_it-icdt.buffer)};
		if(converted<len)
		{
			std::size_t remained{static_cast<std::size_t>(buffer_total_ed-src_it)};
			my_copy_n(src_it,remained,icdt.buffer);
			return dst_it;
		}
		else
		{
			icdt.len=0;
			first+=converted-len;
			dst_first=dst_it;
		}
	}
	auto [src2_it,dst2_it]=iconv_do_impl(icdt.cd,first,last,dst_first,dst_last);
	diff=static_cast<std::size_t>(last-src2_it);
	non_overlapped_copy_n(src2_it,diff,icdt.buffer);
	icdt.len=diff;
	return dst2_it;
}

inline std::size_t iconv_deco_reserve_define_impl(iconv_deco_t& icdt,
	char const* first,std::size_t insize,char* outbyteptr,std::size_t size)
{
	return iconv_dst_impl(icdt,first,first+insize,outbyteptr,outbyteptr+size*8)-outbyteptr;
}


}

template<std::integral to_char_type>
inline constexpr std::size_t deco_reserve_size(io_reserve_type_t<to_char_type,iconv_deco_t>,
	iconv_deco_t& icdt,std::size_t size) noexcept
{
	constexpr std::size_t factor{8/sizeof(to_char_type)};
	return details::intrinsics::mul_or_overflow_die(
		details::intrinsics::add_or_overflow_die(size,32),factor);
}

template<::std::contiguous_iterator fromIter,::std::contiguous_iterator toIter>
inline toIter deco_reserve_define(io_reserve_type_t<::std::iter_value_t<toIter>,iconv_deco_t>,
	iconv_deco_t& icdt,fromIter first,fromIter last,toIter iter)
{
	using to_char_type = ::std::iter_value_t<toIter>;
	using from_char_type = ::std::iter_value_t<fromIter>;
	constexpr std::size_t factor{8/sizeof(to_char_type)};
	std::size_t dis{static_cast<std::size_t>(last-first)};
	std::size_t inbytes{sizeof(from_char_type)*dis};
	std::size_t const allocated_bytes{(inbytes+32)*8};
	return iter+details::iconv_deco_reserve_define_impl(icdt,
		reinterpret_cast<char const*>(::std::to_address(first)),
		inbytes,
		reinterpret_cast<char*>(::std::to_address(iter)),
		allocated_bytes)/sizeof(to_char_type);
}

namespace details
{

inline iconv_t my_iconv_open(char const* tocode,char const* fromcode)
{
	auto cd{iconv_open(tocode,fromcode)};
	if(cd==std::bit_cast<iconv_t>(static_cast<uintptr_t>(-1)))
		throw_posix_error();
	return cd;
}

}

class posix_iconv_file:public posix_iconv_io_observer
{
public:
	using native_handle_type = iconv_t;
	constexpr posix_iconv_file() noexcept = default;
	constexpr posix_iconv_file(iconv_t icv) noexcept : posix_iconv_io_observer{icv}{}

	posix_iconv_file(cstring_view tocode,cstring_view fromcode):
		posix_iconv_io_observer{details::my_iconv_open(tocode.c_str(),fromcode.c_str())}
	{}
	posix_iconv_file(u8cstring_view tocode,u8cstring_view fromcode):
		posix_iconv_io_observer{
			details::my_iconv_open(reinterpret_cast<char const*>(tocode.c_str()),
			reinterpret_cast<char const*>(fromcode.c_str()))}
	{}
	posix_iconv_file(posix_iconv_file const&)=delete;
	posix_iconv_file& operator=(posix_iconv_file const&)=delete;
	constexpr posix_iconv_file(posix_iconv_file&& __restrict other) noexcept:posix_iconv_io_observer{other.release()}{}	
	posix_iconv_file& operator=(posix_iconv_file&& __restrict other) noexcept
	{
		iconv_close(this->cd);
		this->cd=other.release();
		return *this;
	}
	void close()
	{
		if(iconv_close(this->cd)==-1)
			throw_posix_error();
	}
	~posix_iconv_file()
	{
		iconv_close(this->cd);
	}
};

namespace details
{
inline std::size_t do_iconv_impl(iconv_t cd,
	char** __restrict__ inbuf,std::size_t* __restrict__  inbytesleft,
	char** __restrict__ outbuf,std::size_t* __restrict__ outbytesleft)
{
	std::size_t res{iconv(cd,inbuf,inbytesleft,
		outbuf,outbytesleft)};
	if(res==static_cast<std::size_t>(-1))
		throw_posix_error();
	return res;
}
}

inline std::size_t do_iconv(posix_iconv_io_observer piciob,
	char** __restrict__ inbuf,std::size_t* __restrict__  inbytesleft,
	char** __restrict__ outbuf,std::size_t* __restrict__ outbytesleft)
{
	return details::do_iconv_impl(piciob.cd,inbuf,inbytesleft,outbuf,outbytesleft);
}

inline void reset_state(posix_iconv_io_observer piciob)
{
	if(iconv(piciob.cd,nullptr,0,nullptr,nullptr)==static_cast<std::size_t>(-1))
		throw_posix_error();
}

namespace manipulators
{

template<typename T>
struct iconv_code_cvt_t
{
	using manip_tag = manip_tag_t;
	iconv_t cd;
	T reference;
};

template<std::integral char_type>
constexpr iconv_code_cvt_t<io_scatter_t> iconv_code_cvt(posix_iconv_io_observer piiob,basic_io_scatter_t<char_type> scatter) noexcept
{
	return {piiob.cd,scatter};
}

template<typename rg>
requires (std::ranges::contiguous_range<std::remove_cvref_t<rg>>&&std::integral<std::ranges::range_value_t<std::remove_cvref_t<rg>>>)
constexpr iconv_code_cvt_t<io_scatter_t> iconv_code_cvt(posix_iconv_io_observer piiob,rg&& t)
{
	if constexpr(std::is_array_v<std::remove_cvref_t<rg>>)
		return {piiob.cd,{std::ranges::data(t),sizeof(std::ranges::range_value_t<std::remove_cvref_t<rg>>)*(std::ranges::size(t)-1)}};
	else
		return {piiob.cd,{std::ranges::data(t),sizeof(std::ranges::range_value_t<std::remove_cvref_t<rg>>)*std::ranges::size(t)}};
}

template<std::integral char_type>
constexpr iconv_code_cvt_t<io_scatter_t> iconv_code_cvt(posix_iconv_io_observer piiob,chvw_t<char_type const*> t) noexcept
{
	::fast_io::freestanding::basic_string_view<char_type> view(t.reference);
	return {piiob.cd,{view.data(),sizeof(char_type)*view.size()}};
}
template<std::integral char_type>
inline std::size_t print_reserve_size(
	io_reserve_type_t<char_type,
	iconv_code_cvt_t<io_scatter_t>>,iconv_code_cvt_t<io_scatter_t> v) noexcept
{
	static_assert(sizeof(char_type)<=8);
	constexpr std::size_t maximum_size_sentinel{std::numeric_limits<std::size_t>::max()/8*sizeof(char_type)};
	if(v.reference.len>maximum_size_sentinel)[[unlikely]]
		fast_terminate();
	return v.reference.len*8/sizeof(char_type);
}

template<::std::integral char_type>
inline Iter print_reserve_define(io_reserve_type_t<char_type,iconv_code_cvt_t<io_scatter_t>>,
	Iter iter,iconv_code_cvt_t<io_scatter_t> v) noexcept
{
	std::size_t const sz{::fast_io::details::iconv_print_reserve_define_impl(v.cd,
	reinterpret_cast<char const*>(::std::to_address(v.reference.base)),
	v.reference.len,reinterpret_cast<char*>(::std::to_address(iter)))};
	return iter+sz/sizeof(char_type);
}
}


}