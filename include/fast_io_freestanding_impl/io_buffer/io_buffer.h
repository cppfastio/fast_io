#pragma once

namespace fast_io
{

namespace details
{

template<typename T>
inline constexpr void destroy_basic_io_buffer(T&) noexcept;

}

template<typename handletype,
	typename iobuffertraits>
class basic_io_buffer
{
public:
	using handle_type = handletype;
	using traits_type = iobuffertraits;
	using input_char_type = typename traits_type::input_char_type;
	using output_char_type = typename traits_type::output_char_type;

	using input_buffer_type = std::conditional_t<(traits_type::mode&buffer_mode::in)==buffer_mode::in,
		basic_io_buffer_pointers<input_char_type>,empty_buffer_pointers>;
	using output_buffer_type = std::conditional_t<(traits_type::mode&buffer_mode::out)==buffer_mode::out,
		basic_io_buffer_pointers<output_char_type>,empty_buffer_pointers>;

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
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	handle_type handle;

	inline explicit constexpr basic_io_buffer() = default;
	template<typename ...Args>
	requires (::std::constructible_from<handle_type,Args...>&&
		!::std::constructible_from<basic_io_buffer,Args...>)
	inline explicit constexpr basic_io_buffer(Args&& ...args) : handle(::std::forward<Args>(args)...)
	{
	}
	basic_io_buffer& operator=(basic_io_buffer const&)=delete;
	basic_io_buffer(basic_io_buffer const&)=delete;
	constexpr basic_io_buffer(basic_io_buffer&& __restrict other) noexcept:
		input_buffer(::std::move(other.input_buffer)),
		output_buffer(::std::move(other.output_buffer)),handle(::std::move(other.handle))
	{
		other.input_buffer={};
		other.output_buffer={};
	}

	constexpr basic_io_buffer& operator=(basic_io_buffer&& __restrict other) noexcept
	{
		::fast_io::details::destroy_basic_io_buffer(*this);
		input_buffer=::std::move(other.input_buffer);
		output_buffer=::std::move(other.output_buffer);
		handle=::std::move(other.handle);
		other.input_buffer={};
		other.output_buffer={};
		return *this;
	}

	constexpr ~basic_io_buffer()
	{
		::fast_io::details::destroy_basic_io_buffer(*this);
	}
};

}
