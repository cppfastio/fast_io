﻿#pragma once

namespace fast_io
{

namespace details
{

template <::std::integral char_type>
inline constexpr char_type *prtrsv_inaddr_define_impl(char_type *it, posix_in_addr inaddr) noexcept
{
	auto j{inaddr.address};
	it = print_reserve_integral_define<10>(it, *j);
	++j;
	for (auto e{inaddr.address + 4}; j != e; ++j)
	{
		*it = ::fast_io::char_literal_v<u8'.', char_type>;
		++it;
		it = print_reserve_integral_define<10>(it, *j);
	}
	return it;
}

template <::std::integral char_type>
inline constexpr char_type *prtrsv_ipv4_define_impl(char_type *it, ipv4 v) noexcept
{
	it = prtrsv_inaddr_define_impl(it, v.address);
	*it = ::fast_io::char_literal_v<u8':', char_type>;
	++it;
	return ::fast_io::details::print_reserve_integral_define<10>(it, v.port);
}

} // namespace details

template <::std::integral char_type>
inline constexpr ::std::size_t print_reserve_size(io_reserve_type_t<char_type, posix_in_addr>) noexcept
{
	constexpr ::std::size_t char_unsigned_sz{
		::fast_io::details::print_integer_reserved_size_cache<10, false, false, char unsigned>};
	constexpr ::std::size_t res{char_unsigned_sz * 4 + 3};
	return res;
}

template <::std::integral char_type>
inline constexpr char_type *print_reserve_define(io_reserve_type_t<char_type, posix_in_addr>, char_type *it,
												 posix_in_addr inaddr) noexcept
{
	return ::fast_io::details::prtrsv_inaddr_define_impl(it, inaddr);
}

template <::std::integral char_type>
inline constexpr ::std::size_t print_reserve_size(io_reserve_type_t<char_type, ipv4>) noexcept
{
	constexpr ::std::size_t char_unsigned_sz{print_reserve_size(io_reserve_type<char_type, posix_in_addr>)};
	constexpr ::std::size_t res{
		char_unsigned_sz + 1 +
		::fast_io::details::print_integer_reserved_size_cache<10, false, false, ::std::uint_least16_t>};
	return res;
}

template <::std::integral char_type>
inline constexpr char_type *print_reserve_define(io_reserve_type_t<char_type, ipv4>, char_type *it,
												 ipv4 inaddr) noexcept
{
	return ::fast_io::details::prtrsv_ipv4_define_impl(it, inaddr);
}

} // namespace fast_io
