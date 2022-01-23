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

template<::fast_io::freestanding::contiguous_iterator Iter,typename T>
inline constexpr Iter print_reserve_define(::fast_io::io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,::fast_io::space_reserve<T>>,Iter iter,::fast_io::space_reserve<T> const& t)
{
	if constexpr(std::is_pointer_v<Iter>)
	{
		return t.func(iter,iter+t.size);
	}
	else
	{
		auto toaddressiter{::fast_io::freestanding::to_address(iter)};
		return t.func(toaddressiter,toaddressiter+t.size)-toaddressiter+iter;
	}
}

}
