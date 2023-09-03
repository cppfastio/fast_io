#pragma once

namespace fast_io
{

template<typename handletype,
	typename iobuffertraits,
	typename decoratorstp>
class basic_io_deco_filter
{
public:
	using handle_type = handletype;
	using traits_type = iobuffertraits;
	using input_char_type = typename traits_type::input_char_type;
	using output_char_type = typename traits_type::output_char_type;
	using input_buffer_type = std::conditional_t<(traits_type::mode&buffer_mode::in)==buffer_mode::in,
		basic_io_buffer_pointers<input_char_type>,empty_buffer_pointers>;

	using decorators_type = decoratorstp;

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
	handle_type handle;

#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	decorators_type decorators;


	inline explicit constexpr basic_io_deco_filter() = default;
	template<typename ...Args>
	requires (::std::constructible_from<handle_type,Args...>)
	inline explicit constexpr basic_io_deco_filter(decorators_type d,Args&& ...args) : handle(::std::forward<Args>(args)...),
		decorators(::fast_io::freestanding::forward<decorators_type>(d))
	{
	}
	basic_io_deco_filter& operator=(basic_io_deco_filter const&)=delete;
	basic_io_deco_filter(basic_io_deco_filter const&)=delete;
	constexpr basic_io_deco_filter(basic_io_deco_filter&& __restrict other) noexcept:
		input_buffer(::std::move(other.input_buffer)),
		handle(::std::move(other.handle)),
		decorators(::std::move(other.decorators))
	{
		other.input_buffer={};
	}
	template<typename ...Args>
	requires (::std::constructible_from<handle_type,Args...>)
	inline constexpr void reopen(Args&& ...args)
	{
		::fast_io::details::clear_basic_io_filter_pointers(*this);
		if constexpr(::fast_io::details::has_reopen_impl<handle_type,Args...>)
		{
			handle.reopen(std::forward<Args>(args)...);
		}
		else
		{
			handle=handle_type(std::forward<Args>(args)...);
		}
	}

	inline constexpr void close()
	{
		::fast_io::details::clear_basic_io_filter_pointers(*this);
		if constexpr(::fast_io::details::has_close_impl<handle_type>)
		{
			handle.close();
		}
		else
		{
			handle=handle_type();
		}
	}
	constexpr basic_io_deco_filter& operator=(basic_io_deco_filter&& __restrict other) noexcept
	{
		::fast_io::details::destroy_basic_io_filter(*this);
		input_buffer=::std::move(other.input_buffer);
		handle=::std::move(other.handle);
		decorators=::std::move(other.decorators);
		other.input_buffer={};
		return *this;
	}

	constexpr ~basic_io_deco_filter()
	{
		::fast_io::details::destroy_basic_io_filter(*this);
	}
};

};
