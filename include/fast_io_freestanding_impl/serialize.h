#pragma once

namespace fast_io
{

/*
Haven't figured out how to deal with locale specific serializing which is using other rules.
*/

template<output_stream stm>
requires (value_based_stream<stm>&&requires(stm sm)
{
	{io_value_handle(sm)}->std::same_as<stm>;
})
struct serializer
{
	using handle_type = stm;
	using char_type = typename handle_type::char_type;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	handle_type handle{};
};

template<output_stream stm>
inline constexpr serializer<stm> io_value_handle(serializer<stm> sm) noexcept
{
	return sm;
}

template<output_stream stm>
requires (std::is_lvalue_reference_v<stm>||std::is_trivially_copyable_v<stm>)
inline constexpr auto serialize(stm&& out) noexcept
{
	if constexpr(value_based_stream<stm>)
		return serializer{io_value_handle(out)};
	else
		return serializer{io_ref(out)};
}

template<output_stream output,typename... Args>
inline constexpr void print_status_define(serializer<output> sout,Args... args)
{
	details::decay::print_freestanding_decay_normal<false,details::decay::print_control_impl::serialize>(sout.handle,args...);
}

template<output_stream output,typename... Args>
inline constexpr void println_status_define(serializer<output> sout,Args... args)
{
	details::decay::print_freestanding_decay_normal<true,details::decay::print_control_impl::serialize>(sout.handle,args...);
}

template<input_stream stm>
requires (value_based_stream<stm>&&requires(stm sm)
{
	{io_value_handle(sm)}->std::same_as<stm>;
})
struct deserializer
{
	using handle_type = stm;
	using char_type = typename handle_type::char_type;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	handle_type handle{};
};

template<stream stm>
inline constexpr deserializer<stm> io_value_handle(deserializer<stm> sm) noexcept
{
	return sm;
}


template<stream stm>
requires (std::is_lvalue_reference_v<stm>||std::is_trivially_copyable_v<stm>)
inline constexpr auto deserialize(stm&& out) noexcept
{
	if constexpr(value_based_stream<stm>)
		return deserializer{io_value_handle(out)};
	else
		return deserializer{io_ref(out)};
}

}