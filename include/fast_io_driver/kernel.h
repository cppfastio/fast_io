#pragma once

namespace fast_io::kernel
{
template<std::size_t wd=80,std::size_t ht=25>
struct terminal
{
	using char_type = char;
	static inline constexpr std::size_t vga_width=wd;
	static inline constexpr std::size_t vga_height=ht;
	std::size_t row{};
	std::size_t column{};
	void clear() noexcept
	{
		auto ptr=(char volatile*)0xb8000;
		constexpr std::size_t resolution{vga_width*vga_height};
		for(std::size_t i{};i!=resolution;++i)
		{
			*ptr=u8' ';
			ptr+=2;
		}
	}
	terminal() noexcept
	{
		clear();
	}
};
namespace details
{
template<std::size_t vga_width=80,std::size_t vga_height=25>
inline void clear_line(terminal<vga_width,vga_height>& tem) noexcept
{
	auto ptr=(char volatile*)0xb8000+((vga_width*tem.row)<<1);
	for(std::size_t i{};i!=vga_width;++i)
	{
		*ptr=u8' ';
		ptr+=2;
	}
}

template<std::size_t wd=80,std::size_t ht=25>
inline void next_line(terminal<wd,ht>& tem) noexcept
{
	if(++tem.row==ht)[[unlikely]]
		tem.row={};
	clear_line(tem);
}
template<std::size_t wd=80,std::size_t ht=25>
inline void next_character(terminal<wd,ht>& tem) noexcept
{
	if (++tem.column == terminal<wd,ht>::vga_width)[[unlikely]]
	{
		tem.column={};
		next_line(tem);
	}
}
}

template<std::size_t wd,std::size_t ht>
inline void put(terminal<wd,ht>& tem,char ch) noexcept
{
	auto ptr=(char volatile*)0xb8000;
	switch(ch)
	{
	case u8'\n':
		tem.column={};
		details::next_line(tem);
		return;
	break;
	case u8'\t':
	{
		if(tem.column==wd)
		{
			tem.column={};
			++tem.row;
			details::clear_line(tem);
			break;
		}
		std::size_t col{tem.column};
		auto pos=ptr+(((tem.row*wd)+col)<<1);
		for(std::size_t i(col%8);i!=8;++i)
		{
			*pos=u8' ';
			pos+=2;
			++tem.column;
		}
		if(tem.column==wd)
		{
			tem.column={};
			details::next_line(tem);
		}
	}
	break;
	default:
		ptr[((tem.row*wd)+tem.column)<<1]=ch;
		details::next_character(tem);
	}
}

namespace details
{
template<std::size_t wd,std::size_t ht>
constexpr inline void write_impl(terminal<wd,ht>& tem,char const* b,char const* e) noexcept
{
	for(;b!=e;++b)
		put(tem,*b);
}

}

template<std::size_t wd,std::size_t ht,::std::contiguous_iterator Iter>
constexpr inline void write(terminal<wd,ht>& tem,Iter begin, Iter end) noexcept
{
	if constexpr(std::same_as<::std::iter_value_t<Iter>,char>)
		details::write_impl(tem,::std::to_address(begin),::std::to_address(end));
	else
		details::write_impl(tem,reinterpret_cast<char const*>(::std::to_address(begin)),reinterpret_cast<char const*>(::std::to_address(end)));
}

template<std::size_t wd,std::size_t ht>
constexpr inline void scatter_write(terminal<wd,ht>& tem,std::span<fast_io::io_scatter_t const> sp) noexcept
{
	for(auto const& e : sp)
		details::write_impl(tem,reinterpret_cast<char const*>(e.base),reinterpret_cast<char const*>(e.base)+e.len);
}

}
