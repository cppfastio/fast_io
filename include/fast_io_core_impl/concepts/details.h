#pragma once

namespace fast_io::details
{

template<std::integral ch_type>
struct dummy_buffer_output_stream
{
	using output_char_type = ch_type;
};

template<std::integral char_type>
inline constexpr dummy_buffer_output_stream<char_type> output_stream_ref_define(dummy_buffer_output_stream<char_type>) noexcept
{
	return {};
}

template<std::integral char_type>
inline constexpr void write_all_overflow_define(dummy_buffer_output_stream<char_type>,char_type const* first,char_type const* last) noexcept;


template<std::integral char_type>
inline constexpr char_type* obuffer_begin(dummy_buffer_output_stream<char_type>) noexcept
{
	return nullptr;
}

template<std::integral char_type>
inline constexpr char_type* obuffer_curr(dummy_buffer_output_stream<char_type>) noexcept
{
	return nullptr;
}

template<std::integral char_type>
inline constexpr char_type* obuffer_end(dummy_buffer_output_stream<char_type>) noexcept
{
	return nullptr;
}

template<std::integral char_type>
inline constexpr void obuffer_set_curr(dummy_buffer_output_stream<char_type>,char_type*) noexcept{}

template<std::integral ch_type>
struct dummy_buffer_input_stream
{
	using input_char_type = ch_type;
};

template<std::integral char_type>
inline constexpr dummy_buffer_input_stream<char_type> input_stream_ref_define(dummy_buffer_input_stream<char_type>) noexcept
{
	return {};
}

template<std::integral char_type>
inline constexpr char_type* ibuffer_begin(dummy_buffer_input_stream<char_type>) noexcept
{
	return nullptr;
}

template<std::integral char_type>
inline constexpr char_type* ibuffer_curr(dummy_buffer_input_stream<char_type>) noexcept
{
	return nullptr;
}

template<std::integral char_type>
inline constexpr char_type* ibuffer_end(dummy_buffer_input_stream<char_type>) noexcept
{
	return nullptr;
}

template<std::integral char_type>
inline constexpr void ibuffer_set_curr(dummy_buffer_input_stream<char_type>,char_type*) noexcept{}

template<std::integral char_type>
inline constexpr bool ibuffer_underflow(dummy_buffer_input_stream<char_type>) noexcept
{
	return true;
}

template<std::integral char_type>
inline constexpr void read_all_overflow_define(dummy_buffer_input_stream<char_type>,char_type*,char_type*) noexcept
{
}

}
