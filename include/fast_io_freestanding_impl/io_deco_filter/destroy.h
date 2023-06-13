#pragma once

namespace fast_io::details
{

template<typename T>
inline constexpr void clear_basic_io_filter_pointers(T& t) noexcept
{
	using traits_type = typename T::traits_type;
	constexpr auto mode{traits_type::mode};
	if constexpr((mode&buffer_mode::out)==buffer_mode::out)
	{
		t.output_buffer.buffer_curr=t.output_buffer.buffer_begin;
	}
	if constexpr((mode&buffer_mode::in)==buffer_mode::in)
	{
		t.input_buffer.buffer_curr=t.input_buffer.buffer_begin;
	}
}

template<typename T>
inline constexpr void destroy_basic_io_filter(T& t) noexcept
{
	using traits_type = typename T::traits_type;
	using allocator_type = traits_type::allocator_type;
	constexpr auto mode{traits_type::mode};
	if constexpr((mode&buffer_mode::out)==buffer_mode::out)
	{
#if 0
#if (defined(_MSC_VER)&&_HAS_EXCEPTIONS!=0) || (!defined(_MSC_VER)&&defined(__cpp_exceptions))
		try
		{
#endif
			::fast_io::details::close_basic_io_buffer(t);
#if (defined(_MSC_VER)&&_HAS_EXCEPTIONS!=0) || (!defined(_MSC_VER)&&defined(__cpp_exceptions))
		}
		catch(...)
		{
		}
#endif
#endif
		auto buffer_begin{t.output_buffer.buffer_begin};
		if(buffer_begin)
		{
			if constexpr((mode&buffer_mode::secure_clear)==buffer_mode::secure_clear)
			{
				secure_clear(buffer_begin,
				traits_type::output_buffer_size*sizeof(typename traits_type::output_char_type));
			}
			::fast_io::typed_generic_allocator_adapter<allocator_type,
			typename traits_type::output_char_type>::deallocate_n(buffer_begin,
			traits_type::output_buffer_size);
		}
	}

	if constexpr((mode&buffer_mode::in)==buffer_mode::in)
	{
		auto buffer_begin{t.input_buffer.buffer_begin};
		if(buffer_begin)
		{
			if constexpr((mode&buffer_mode::secure_clear)==buffer_mode::secure_clear)
			{
				secure_clear(buffer_begin,
				traits_type::input_buffer_size*sizeof(typename traits_type::input_char_type));
			}
			::fast_io::typed_generic_allocator_adapter<allocator_type,
			typename traits_type::output_char_type>::deallocate_n(buffer_begin,
			traits_type::input_buffer_size);
		}
	}
}


}