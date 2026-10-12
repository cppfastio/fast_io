#pragma once

namespace fast_io
{

template <input_stream input>
	requires(!buffer_input_stream<input>)
struct single_character_input_buffer
{
public:
	using char_type = typename input::char_type;
	using input_char_type = char_type;
	input &reference{};
	char_type single_character{};
	bool pos{};
	bool pos_end{};
};

template <input_stream input>
	requires(!buffer_input_stream<input>)
inline constexpr typename input::char_type *
read_some_underflow_define(single_character_input_buffer<input> &in, typename input::char_type *first,
						   ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::operations::decay::read_some_decay(in.reference, first,
																							  count))
{
	if (in.pos != in.pos_end)
	{
		if (!count)
		{
			return first;
		}
		*first = in.single_character;
		in.pos = in.pos_end;
		++first;
		--count;
	}
	return ::fast_io::operations::decay::read_some_decay(in.reference, first, count);
}

template <input_stream input>
	requires(!buffer_input_stream<input>)
inline constexpr auto ibuffer_begin(single_character_input_buffer<input> &in)
{
	return __builtin_addressof(in.single_character);
}
template <input_stream input>
	requires(!buffer_input_stream<input>)
inline constexpr auto ibuffer_curr(single_character_input_buffer<input> &in)
{
	return __builtin_addressof(in.single_character) + static_cast<::std::size_t>(in.pos);
}
template <input_stream input>
	requires(!buffer_input_stream<input>)
inline constexpr auto ibuffer_end(single_character_input_buffer<input> &in)
{
	return __builtin_addressof(in.single_character) + static_cast<::std::size_t>(in.pos_end);
}

template <input_stream input>
	requires(!buffer_input_stream<input>)
inline constexpr void ibuffer_set_curr(single_character_input_buffer<input> &in, typename input::char_type *ptr)
{
	in.pos = (ptr != __builtin_addressof(in.single_character));
}

template <input_stream input>
	requires(!buffer_input_stream<input>)
inline constexpr bool ibuffer_underflow(single_character_input_buffer<input> &in)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::operations::decay::read_some_decay(
		in.reference, __builtin_addressof(in.single_character), 1))
{
	in.pos_end = (::fast_io::operations::decay::read_some_decay(
					  in.reference, __builtin_addressof(in.single_character), 1) !=
				  __builtin_addressof(in.single_character));
	in.pos = {};
	return in.pos_end;
}

template <input_stream input>
	requires(!buffer_input_stream<input>)
inline constexpr void avoid_scan_reserve(single_character_input_buffer<input> &)
{}

} // namespace fast_io
