#pragma once

namespace fast_io
{

template<buffer_input_stream input>
struct basic_input_buffer_iterator
{
	input *ptr{};
};

namespace details
{
template<buffer_input_stream input>
constexpr basic_input_buffer_iterator<input>& deal_with_underflow(basic_input_buffer_iterator<input>& gen)
{
	auto& ref(*gen.ptr);
	if(!ibuffer_underflow(ref))[[unlikely]]
	{
		gen.ptr={};
		return gen;
	}
	return gen;
}
}
template<buffer_input_stream input>
inline constexpr auto operator*(basic_input_buffer_iterator<input>& gen)
{
	return *ibuffer_curr(*gen.ptr);
}
template<buffer_input_stream input>
inline constexpr basic_input_buffer_iterator<input>& operator++(basic_input_buffer_iterator<input>& gen)
{
	ibuffer_set_curr(*gen.ptr,ibuffer_curr(*gen.ptr)+1);
	if constexpr(!contiguous_input_stream<input>)
	{
		if(ibuffer_curr(*gen.ptr)==ibuffer_end(*gen.ptr))[[unlikely]]
			return details::deal_with_underflow(gen);
	}
	return gen;
}

template<buffer_input_stream input>
inline constexpr bool operator!=(basic_input_buffer_iterator<input> a, ::std::default_sentinel_t)
{
	if constexpr(contiguous_input_stream<input>)
		return ibuffer_curr(*a.ptr)!=ibuffer_end(*a.ptr);
	else
		return a.ptr;
}

template<buffer_input_stream input>
inline constexpr bool operator==(basic_input_buffer_iterator<input> a, ::std::default_sentinel_t)
{
	if constexpr(contiguous_input_stream<input>)
		return ibuffer_curr(*a.ptr)==ibuffer_end(*a.ptr);
	else
		return a.ptr==nullptr;
}

template<buffer_input_stream input>
inline constexpr bool operator!=(::std::default_sentinel_t,basic_input_buffer_iterator<input> a)
{
	if constexpr(contiguous_input_stream<input>)
		return ibuffer_curr(*a.ptr)!=ibuffer_end(*a.ptr);
	else
		return a.ptr;
}

template<buffer_input_stream input>
inline constexpr bool operator==(::std::default_sentinel_t,basic_input_buffer_iterator<input> a)
{
	if constexpr(contiguous_input_stream<input>)
		return ibuffer_curr(*a.ptr)==ibuffer_end(*a.ptr);
	else
		return a.ptr==nullptr;
}

template<buffer_input_stream input>
struct basic_input_buffer_generator
{
	input *ptr{};
};

template<buffer_input_stream input>
inline constexpr basic_input_buffer_iterator<input> begin(basic_input_buffer_generator<input> a)
{
	return {a.ptr};
}

template<buffer_input_stream input>
inline constexpr ::std::default_sentinel_t end(basic_input_buffer_generator<input>)
{
	return {};
}

template<buffer_input_stream input>
inline constexpr basic_input_buffer_generator<input> igenerator(input& in)
{
	if constexpr(!contiguous_input_stream<input>)
	{
		if(ibuffer_curr(in)==ibuffer_end(in))[[unlikely]]
		{
			if(!ibuffer_underflow(in))[[unlikely]]
				return {};
		}
	}
	return {__builtin_addressof(in)};
}

}
