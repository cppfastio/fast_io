#pragma once

namespace fast_io::details
{

template<typename output>
requires (output_stream<output>)
struct temporary_buffer
{
	using char_type = typename output::char_type;
	static inline constexpr std::size_t buffer_size{512};
	std::size_t pos{};
	char_type buffer[buffer_size];
	output out{};
};

template<typename output>
inline constexpr typename output::char_type* obuffer_begin(temporary_buffer<output>& out) noexcept
{
	return out.buffer;
}

template<typename output>
inline constexpr typename output::char_type* obuffer_curr(temporary_buffer<output>& out) noexcept
{
	return out.buffer+out.pos;
}

template<typename output>
inline constexpr typename output::char_type* obuffer_end(temporary_buffer<output>& out) noexcept
{
	return out.buffer+temporary_buffer<output>::buffer_size;
}

template<typename output>
inline constexpr void obuffer_set_curr(temporary_buffer<output>& out,typename output::char_type* ptr) noexcept
{
	out.pos=static_cast<std::size_t>(ptr-out.buffer);
}

template<typename output>
inline constexpr void flush(temporary_buffer<output>& out)
{
	auto start{out.buffer};
	if(out.pos==0)
		return;
	write(out.out,start,start+out.pos);
	out.pos=0;
}

template<typename output>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void obuffer_overflow(temporary_buffer<output>& out,typename output::char_type ch)
{
	auto start{out.buffer};
	write(out.out,start,start+temporary_buffer<output>::buffer_size);
	*start=ch;
	out.pos=1;
}

template<bool line,typename output>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void temporary_buffer_write_cold_path(temporary_buffer<output>& out,
	typename output::char_type const* first,
	typename output::char_type const* last)
{
	using char_type = typename output::char_type;
	auto start{out.buffer};
	std::size_t const pos{out.pos};
	constexpr std::size_t buffsz{temporary_buffer<output>::buffer_size};
	constexpr std::size_t buffszm1{buffsz-1};
	std::size_t const remain_space{static_cast<std::size_t>(buffsz-pos)};
	if constexpr(line)
	{
		std::size_t diff{static_cast<std::size_t>(last-first)};
		std::size_t diffp1{diff};
		++diffp1;
		if(diffp1==remain_space)
		{
			non_overlapped_copy_n(first,diff,start+pos);
			out.buffer[buffszm1]=char_literal_v<u8'\n',char_type>;
			write(out,start,start+buffsz);
			out.pos=0;
			return;
		}

	}
	non_overlapped_copy_n(first,remain_space,start+pos);
	first+=remain_space;
	write(out.out,start,start+buffsz);
	out.pos=0;
	std::size_t const new_diff{static_cast<std::size_t>(last-first)};
	if constexpr(line)
	{
		if(new_diff<buffszm1)
		{
			non_overlapped_copy_n(first,new_diff,start);
			*(out.pos=new_diff)=char_literal_v<u8'\n',char_type>;
			++out.pos;
		}
		else
		{
			if constexpr(output_stream_with_writeln<output>)
			{
				writeln(out.out,first,last);
			}
			else
			{
				write(out.out,first,last);
				*out.buffer=char_literal_v<u8'\n',char_type>;
				out.pos=0;
			}
		}
	}
	else
	{
		if(new_diff<buffsz)
		{
			non_overlapped_copy_n(first,new_diff,start);
			out.pos=new_diff;
		}
		else
		{
			write(out.out,first,last);	//write remaining characters
		}
	}
}

template<typename output>
inline constexpr void write(temporary_buffer<output>& out,
	typename output::char_type const* first,
	typename output::char_type const* last)
{
	std::size_t const pos{out.pos};
	std::size_t const remain_space{temporary_buffer<output>::buffer_size-pos};
	std::size_t const ptr_diff{static_cast<std::size_t>(last-first)};
	if(remain_space<=ptr_diff)
#if __has_cpp_attribute(unlikely)
[[unlikely]]
#endif
		return temporary_buffer_write_cold_path<false>(out,first,last);
	auto start{out.buffer};
	non_overlapped_copy_n(first,ptr_diff,start+pos);
	out.pos+=ptr_diff;
}

template<typename output>
inline constexpr void writeln(temporary_buffer<output>& out,
	typename output::char_type const* first,
	typename output::char_type const* last)
{
	using char_type = typename output::char_type;
	std::size_t const pos{out.pos};
	std::size_t const remain_space{temporary_buffer<output>::buffer_size-pos};
	std::size_t const ptr_diff{static_cast<std::size_t>(last-first)};
	if(remain_space<ptr_diff)
#if __has_cpp_attribute(unlikely)
[[unlikely]]
#endif
		return temporary_buffer_write_cold_path<true>(out,first,last);
	auto start{out.buffer};
	non_overlapped_copy_n(first,ptr_diff,start+pos);
	*(out.pos+=ptr_diff)=char_literal_v<u8'\n',char_type>;
	++out.pos;
}

}
