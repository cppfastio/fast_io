#pragma once

namespace fast_io
{

struct source_location_scatter
{
	basic_io_scatter_t<char> file_name;
	basic_io_scatter_t<char> function_name;
	std::uint_least32_t line;
	std::uint_least32_t column;
};

namespace details
{

inline constexpr std::size_t print_reserve_size_source_location_impl(source_location_scatter location) noexcept
{
	constexpr std::size_t uint32_rsv_size{print_reserve_size(io_reserve_type<char,std::uint_least32_t>)};
	constexpr std::size_t total_uint_least32_t_rsv_size{uint32_rsv_size*2+3};
	return intrinsics::add_or_overflow_die_chain(location.file_name.len,location.function_name.len,total_uint_least32_t_rsv_size);
}

inline constexpr char* print_reserve_define_source_location_impl(char* iter,source_location_scatter location) noexcept
{
	*(iter=non_overlapped_copy_n(location.file_name.base,location.file_name.len,iter))=':';
	*(iter=print_reserve_define(io_reserve_type<char,std::uint_least32_t>,++iter,location.line))=':';
	*(iter=print_reserve_define(io_reserve_type<char,std::uint_least32_t>,++iter,location.column))=':';
	return non_overlapped_copy_n(location.function_name.base,location.function_name.len,++iter);
}

inline constexpr source_location_scatter print_alias_define_source_location_impl(std::source_location location) noexcept
{
	return {{location.file_name(),cstr_len(location.file_name())},
	{location.function_name(),cstr_len(location.function_name())},
	location.line(),location.column()};
}

}

inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char,source_location_scatter>,source_location_scatter location) noexcept
{
	return details::print_reserve_size_source_location_impl(location);
}

inline constexpr char* print_reserve_define(io_reserve_type_t<char,source_location_scatter>, char* iter,source_location_scatter location) noexcept
{
	return details::print_reserve_define_source_location_impl(iter,location);
}

inline constexpr source_location_scatter print_alias_define(io_alias_t,std::source_location location) noexcept
{
	return details::print_alias_define_source_location_impl(location);
}

namespace manipulators
{

inline constexpr ::std::source_location cur_src_loc(::std::source_location loc=::std::source_location::current()) noexcept
{
	return loc;
}

}

}
