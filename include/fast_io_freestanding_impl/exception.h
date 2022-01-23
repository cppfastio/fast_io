#pragma once

#include<exception>

namespace fast_io
{
template<typename T>
requires (std::same_as<T,std::exception>)
inline basic_io_scatter_t<char> print_alias_define(T const &e) noexcept
{
	auto const c_str{e.what()};
	return {c_str,cstr_len(c_str)};
}
}