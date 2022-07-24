#pragma once

namespace fast_io
{

template<typename Function>
struct space_reserve
{
	using function_type = Function;
	std::size_t size{};
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	function_type func{};
};

template<std::integral char_type,typename T>
inline constexpr std::size_t print_reserve_size(::fast_io::io_reserve_type_t<char_type,::fast_io::space_reserve<T>>,::fast_io::space_reserve<T> const& t) noexcept
{
	return t.size;
}

template<std::integral char_type,typename T>
inline constexpr char_type* print_reserve_define(::fast_io::io_reserve_type_t<char_type,::fast_io::space_reserve<T>>, char_type* iter,::fast_io::space_reserve<T> const& t)
{
	return t.func(iter,iter+t.size);
}

}
