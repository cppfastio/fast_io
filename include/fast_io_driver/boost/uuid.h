#pragma once
#ifndef BOOST_UUID_HPP
#include<boost/uuid/uuid.hpp>
#endif

namespace fast_io
{

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,boost::uuids::uuid>) noexcept
{
	constexpr std::size_t sz{36};
	static_assert(boost::uuids::uuid::static_size()==16);
	return sz;
}

namespace details
{

template<bool uppercase,::fast_io::freestanding::random_access_iterator Iter>
inline constexpr Iter pr_rsv_boost_uuid(Iter iter,boost::uuids::uuid const& u) noexcept
{
	static_assert(::fast_io::freestanding::contiguous_iterator<boost::uuids::uuid::const_iterator>);
	auto first{::fast_io::freestanding::to_address(u.begin())};
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(std::is_constant_evaluated())
#endif
	{
		std::byte buffer[16];
		for(std::size_t i{};i!=16;++i)
		{
			buffer[i]=static_cast<std::byte>(first[i]);
		}
		return pr_rsv_uuid<uppercase>(iter,buffer);
	}
	else
	{
		return pr_rsv_uuid<uppercase>(iter,reinterpret_cast<std::byte const*>(first));
	}
}

}

template<::fast_io::freestanding::random_access_iterator Iter>
inline constexpr Iter print_reserve_define(io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,boost::uuids::uuid>,Iter iter,boost::uuids::uuid const& u) noexcept
{
	return ::fast_io::details::pr_rsv_boost_uuid<false>(iter,u);
}

}
