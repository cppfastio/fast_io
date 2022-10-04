#pragma once

namespace fast_io
{

template<input_stream input>
requires (!buffer_input_stream<input>)
struct single_character_input_buffer
{
public:
	using char_type = typename input::char_type;
	input& reference{};
	char_type single_character{};
	bool pos{};
	bool pos_end{};
};

template<input_stream input,::std::contiguous_iterator Iter>
requires (std::same_as<typename input::char_type,::std::iter_value_t<Iter>>||std::same_as<char,typename input::char_type>)
constexpr Iter read(single_character_input_buffer<input>& in,Iter begin,Iter end)
{
	if constexpr(std::same_as<typename input::char_type,::std::iter_value_t<Iter>>)
	{
		if(in.pos!=in.pos_end)
		{
			if(begin==end)
				return begin;
			details::non_overlapped_copy_n(__builtin_addressof(in.single_character),1,::std::to_address(begin));
			in.pos=in.pos_end;
			++begin;
		}
		return read(in.reference,begin,end);
	}
	else
	{
		return read(in,reinterpret_cast<char*>(::std::to_address(begin)),reinterpret_cast<char*>(::std::to_address(end)));
	}
}

template<input_stream input>
constexpr auto ibuffer_begin(single_character_input_buffer<input>& in)
{
	return __builtin_addressof(in.single_character);
}
template<input_stream input>
constexpr auto ibuffer_curr(single_character_input_buffer<input>& in)
{
	return __builtin_addressof(in.single_character)+static_cast<std::size_t>(in.pos);
}
template<input_stream input>
constexpr auto ibuffer_end(single_character_input_buffer<input>& in)
{
	return __builtin_addressof(in.single_character)+static_cast<std::size_t>(in.pos_end);
}

template<input_stream input>
constexpr void ibuffer_set_curr(single_character_input_buffer<input>& in,typename input::char_type* ptr)
{
	in.pos=(ptr!=__builtin_addressof(in.single_character));
}

template<input_stream input>
constexpr bool ibuffer_underflow(single_character_input_buffer<input>& in)
{
	in.pos_end=(read(in.reference,__builtin_addressof(in.single_character),__builtin_addressof(in.single_character)+1)!=__builtin_addressof(in.single_character));
	in.pos={};
	return in.pos_end;
}

template<input_stream input>
constexpr void avoid_scan_reserve(single_character_input_buffer<input>&){}

}