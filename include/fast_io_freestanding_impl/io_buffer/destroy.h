#pragma once

namespace fast_io::details
{

template<typename T>
inline constexpr void destroy_basic_io_buffer(T& t) noexcept
{
	using traits_type = typename T::traits_type;
	using allocator_type = traits_type::allocator_type;
	constexpr auto mode{traits_type::mode};
	if constexpr((mode&buffer_mode::out)==buffer_mode::out)
	{
#if (defined(_MSC_VER)&&_HAS_EXCEPTIONS!=0) || (!defined(_MSC_VER)&&defined(__cpp_exceptions))
		try
		{
#endif
			output_stream_buffer_flush_define(::fast_io::manipulators::output_stream_ref(t));
#if (defined(_MSC_VER)&&_HAS_EXCEPTIONS!=0) || (!defined(_MSC_VER)&&defined(__cpp_exceptions))
		}
		catch(...)
		{
		}
#endif
		auto buffer_begin{t.output_buffer.buffer_begin};
		if(buffer_begin)
		{
			::fast_io::typed_generic_allocator_adapter<allocator_type,
			typename traits_type::output_char_type>::deallocate_n(buffer_begin,
			buffer_begin+traits_type::output_buffer_size);
		}
	}

	if constexpr((mode&buffer_mode::in)==buffer_mode::in)
	{
		auto buffer_begin{t.input_buffer.buffer_begin};
		if(buffer_begin)
		{
			::fast_io::typed_generic_allocator_adapter<allocator_type,
			typename traits_type::output_char_type>::deallocate_n(buffer_begin,
			buffer_begin+traits_type::input_buffer_size);
		}
	}
}

}
