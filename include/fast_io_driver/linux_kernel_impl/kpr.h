#pragma once

namespace fast_io
{

extern int linux_kernel_printk(char const* fmt, ...) noexcept __asm__("_printk");

enum class kern
{
emerg,
alert,
crit,
warning,
notice,
info,
debug,
default_value,
cont
};

template<std::integral ch_type>
struct basic_kpr
{
	using char_type = ch_type;
	kern level{kern::default_value};
};

template<bool line>
inline constexpr char8_t const* kern_to_fmt(kern k) noexcept
{
	if constexpr(line)
	{
		switch(k)
		{
		case kern::emerg:
			return u8"\0010%.*s\n";
		case kern::alert:
			return u8"\0011%.*s\n";
		case kern::crit:
			return u8"\0012%.*s\n";
		case kern::warning:
			return u8"\0013%.*s\n";
		case kern::notice:
			return u8"\0014%.*s\n";
		case kern::info:
			return u8"\0015%.*s\n";
		case kern::debug:
			return u8"\0016%.*s\n";
		case kern::cont:
			return u8"\001c%.*s\n";
		break;
		default:
			return u8"%.*s\n";
		}
	}
	else
	{
		switch(k)
		{
		case kern::emerg:
			return u8"\0010%.*s";
		case kern::alert:
			return u8"\0011%.*s";
		case kern::crit:
			return u8"\0012%.*s";
		case kern::warning:
			return u8"\0013%.*s";
		case kern::notice:
			return u8"\0014%.*s";
		case kern::info:
			return u8"\0015%.*s";
		case kern::debug:
			return u8"\0016%.*s";
		case kern::cont:
			return u8"\001c%.*s";
		break;
		default:
			return u8"%.*s";
		}
	}
}

namespace details
{

#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline void linux_kpr_raw_write_fmt(char const* fmt,void const* start,void const* last) noexcept
{
	char const* start_char_ptr{reinterpret_cast<char const*>(start)};
	char const* last_char_ptr{reinterpret_cast<char const*>(last)};

	std::size_t diff{static_cast<std::size_t>(last_char_ptr-start_char_ptr)};
	constexpr int int_max{std::numeric_limits<int>::max()};
	if constexpr(int_max<=std::numeric_limits<std::size_t>::max())
	{
		constexpr std::size_t int_max_size_val{static_cast<std::size_t>(int_max)};
		if(diff>int_max_size_val)//linux kernel ring buffer has restricted size. discard >=INT_MAX bytes
			diff=int_max_size_val;
	}
	::fast_io::linux_kernel_printk(fmt,static_cast<int>(diff),start_char_ptr);
}

template<bool line>
inline void linux_kpr_raw_write(::fast_io::kern k,void const* start,void const* last) noexcept
{
	linux_kpr_raw_write_fmt(reinterpret_cast<char const*>(kern_to_fmt<line>(k)),start,last);
}

template<bool line,std::integral ch_type,typename T>
inline constexpr void print_status_define_single_line_impl(basic_kpr<ch_type> kpr,T t)
{
	basic_io_scatter_t<ch_type> scatter{print_scatter_define(io_reserve_type<ch_type,T>,t)};
	auto base{scatter.base};
	linux_kpr_raw_write<line>(kpr.level,base,base+scatter.len);
}

template<kern k,std::size_t n,bool line>
requires (static_cast<std::size_t>(k)<=static_cast<std::size_t>(kern::cont))
inline constexpr auto linux_kernel_printk_formatter_cal() noexcept
{
	static_assert(n<=(SIZE_MAX-256u)/4u);
	constexpr std::size_t start{(k==kern::default_value?0:2)};
	::fast_io::freestanding::array<char8_t,(k==kern::default_value?0:2)+n*4+1+static_cast<std::size_t>(line)> fmts;
	
	if constexpr(k!=kern::default_value)
	{
		fmts[0]=1;
		if constexpr(k<kern::default_value)
		{
			fmts[1]=static_cast<char8_t>(u8'0'+static_cast<char8_t>(k));
		}
		else
			fmts[1]=u8'c';
	}
	for(std::size_t i{};i!=n;++i)
	{
		std::size_t j{start+i*4};
		fmts[j]=u8'%';
		fmts[j+1]=u8'.';
		fmts[j+2]=u8'*';
		fmts[j+3]=u8's';
	}
	if constexpr(line)
	{
		fmts[fmts.size()-2]=u8'\n';
	}
	fmts.back()=0;
	return fmts;
}

template<kern k,std::size_t n,bool line>
inline constexpr auto kfmtcache{linux_kernel_printk_formatter_cal<k,n,line>()};

template<std::size_t n,bool line>
inline constexpr char8_t const* cal_kpr_fmt(kern k) noexcept
{
	switch(k)
	{
	case kern::emerg:
		return kfmtcache<kern::emerg,n,line>.element;
	case kern::alert:
		return kfmtcache<kern::alert,n,line>.element;
	case kern::crit:
		return kfmtcache<kern::crit,n,line>.element;
	case kern::warning:
		return kfmtcache<kern::warning,n,line>.element;
	case kern::notice:
		return kfmtcache<kern::notice,n,line>.element;
	case kern::info:
		return kfmtcache<kern::info,n,line>.element;
	case kern::debug:
		return kfmtcache<kern::debug,n,line>.element;
	case kern::cont:
		return kfmtcache<kern::cont,n,line>.element;
	break;
	default:
		return kfmtcache<kern::default_value,n,line>.element;
	}
}

struct kpr_scatter_struct
{
	char const* ptr;
	int bytes;
};

template<std::integral char_type,typename T,typename... Args>
inline void kpr_scatter_print_recursive(kpr_scatter_struct* p,T t,Args ...args)
{
	auto scatter{print_scatter_define(io_reserve_type<char_type,T>,t)};
	std::size_t len{scatter.len*sizeof(char_type)};
	constexpr int int_max{std::numeric_limits<int>::max()};
	if(len>int_max)
		len=int_max;
	*--p={reinterpret_cast<char const*>(scatter.base),static_cast<int>(len)};
	if constexpr(sizeof...(Args)!=0)
		kpr_scatter_print_recursive<char_type>(p,args...);
}

template<std::integral char_type,typename T,typename... Args>
inline void kpr_scatter_reserve_print_recursive(kpr_scatter_struct* p,char_type* buffer,T t,Args ...args)
{
	if constexpr(scatter_printable<char_type,T>)
	{
		auto scatter{print_scatter_define(io_reserve_type<char_type,T>,t)};
		std::size_t len{scatter.len*sizeof(char_type)};
		constexpr int int_max{std::numeric_limits<int>::max()};
		if(len>int_max)
			len=int_max;
		*--p={reinterpret_cast<char const*>(scatter.base),static_cast<int>(len)};
	}
	else
	{
		auto newbufferptr{print_reserve_define(io_reserve_type<char_type,T>,buffer,t)};
		*--p={reinterpret_cast<char const*>(buffer),static_cast<int>(newbufferptr-buffer)};
		buffer=newbufferptr;
	}
	if constexpr(sizeof...(Args)!=0)
	{
		if constexpr((scatter_printable<char_type,Args>&&...))
			kpr_scatter_print_recursive<char_type>(p,args...);
		else
			kpr_scatter_reserve_print_recursive(p,buffer,args...);
	}
}


template<std::size_t n,typename... Args>
inline void deal_with_kpr_fmt_real(char const* fmt_str,kpr_scatter_struct* s,Args ...args)
{
	if constexpr(n==0)
		::fast_io::linux_kernel_printk(fmt_str,args...);
	else
		deal_with_kpr_fmt_real<n-1>(fmt_str,s+1,s->bytes,s->ptr,args...);
}

template<std::integral char_type,typename T,typename... Args>
inline constexpr bool kpr_total_print_reserve_size_less_or_equal_than512(std::size_t size) noexcept
{
	if constexpr(reserve_printable<char_type,T>)
	{
		constexpr std::size_t this_size{print_reserve_size(io_reserve_type<char_type,T>)};
		if constexpr(this_size>=std::numeric_limits<int>::max())
			return false;
		if(__builtin_add_overflow(size,this_size,__builtin_addressof(size)))
			return false;
		if(size>512)
			return false;
	}
	else if constexpr(!scatter_printable<char_type,T>)
		return false;
	if constexpr(sizeof...(Args)==0)
	{
		return true;
	}
	else
	{
		return kpr_total_print_reserve_size_less_or_equal_than512<char_type,Args...>(size);
	}
}
template<std::integral char_type>
inline constexpr bool kpr_total_print_reserve_size_less_or_equal_than512(std::size_t size) noexcept
{
	return size<=512;
}

template<std::integral char_type,typename... Args>
inline void deal_with_kpr_ignore_line(char const* fmt_str,Args ...args)
{
	constexpr std::size_t n{sizeof...(Args)};
	if constexpr(n==0)
	{
		::fast_io::linux_kernel_printk(fmt_str);
	}
	else
	{
		kpr_scatter_struct buffer[n];
		if constexpr((scatter_printable<char_type,Args>&&...))
		{
			kpr_scatter_print_recursive<char_type>(buffer+n,args...);
			deal_with_kpr_fmt_real<n>(fmt_str,buffer);
		}
		else
		{
			constexpr std::size_t reserve_size{::fast_io::details::decay::calculate_scatter_reserve_size<char_type,Args...>()};
			char_type reserve_buffer[reserve_size];
			kpr_scatter_reserve_print_recursive(buffer+n,reserve_buffer,args...);
			deal_with_kpr_fmt_real<n>(fmt_str,buffer);
		}
	}
}

template<bool line,std::integral char_type,typename... Args>
inline void deal_with_kpr_common(basic_kpr<char_type> kpr,Args ...args)
{
	constexpr std::size_t n{sizeof...(Args)};
	if constexpr(n==0&&!line)
	{
		if(kpr.level==kern::default_value)
			return;
	}
	deal_with_kpr_ignore_line<char_type>(reinterpret_cast<char const*>(cal_kpr_fmt<n,line>(kpr.level)),args...);
}

}

template<std::integral char_type,::std::contiguous_iterator Iter>
inline void write(basic_kpr<char_type> kpr,Iter first,Iter last) noexcept
{
	details::linux_kpr_raw_write<false>(kpr.level,::std::to_address(first),::std::to_address(last));
}

template<std::integral ch_type>
inline constexpr basic_kpr<ch_type> io_value_handle(basic_kpr<ch_type> linuxkpr) noexcept
{
	return linuxkpr;
}

using kpr = basic_kpr<char>;
using wkpr = basic_kpr<wchar_t>;
using u8kpr = basic_kpr<char8_t>;
using u16kpr = basic_kpr<char16_t>;
using u32kpr = basic_kpr<char32_t>;

template<bool line,std::integral ch_type,typename... Args>
requires (print_freestanding_decay_okay_character_type_no_status<ch_type,Args...>&&sizeof(ch_type)==1&&(!::fast_io::details::is_ebcdic<ch_type>))
inline constexpr void print_status_define(basic_kpr<ch_type> k,Args ...args)
{
	if constexpr((::fast_io::details::kpr_total_print_reserve_size_less_or_equal_than512<ch_type,Args...>(0)))
		::fast_io::details::deal_with_kpr_common<line>(k,args...);
	else
		print_freestanding_decay_no_status<line>(k,args...);
}

}
