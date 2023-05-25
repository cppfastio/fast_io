#pragma once

namespace fast_io
{

template<typename handletype,
	::std::integral input_char_type,
	::std::integral output_char_type,
	buffer_mode mde,
	typename allocatortp,
	std::size_t bfs>
class basic_io_buffer
{
public:
	using handle_type = handletype;
	inline static constexpr buffer_mode mode = mde;
	inline static constexpr std::size_t buffer_size = bfs;
	inline static constexpr bool need_secure_clear = (mode&buffer_mode::secure_clear)==buffer_mode::secure_clear;
	inline static constexpr bool has_input_buffer = (mode&buffer_mode::in)==buffer_mode::in;
	inline static constexpr bool has_output_buffer = (mode&buffer_mode::out)==buffer_mode::out;
#if 0
	inline static constexpr bool has_input_decorator = details::has_internal_decorator_impl<decorators_type>;
	inline static constexpr bool has_output_decorator = details::has_external_decorator_impl<decorators_type>;

	inline static constexpr bool has_input_decorator = false;
	inline static constexpr bool has_output_decorator = false;
#endif

	using input_char_type = typename decorators_type::input_char_type;
	using output_char_type = typename decorators_type::output_char_type;
	using allocator_type = allocatortp;

	using input_buffer_type = std::conditional_t<has_ibuffer,
		basic_io_buffer_pointers<input_char_type>>;
	using output_buffer_type = std::conditional_t<has_obuffer,
		basic_io_buffer_pointers<output_char_type>>;

#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	input_buffer_type input_buffer;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	output_buffer_type output_buffer;
};

}
