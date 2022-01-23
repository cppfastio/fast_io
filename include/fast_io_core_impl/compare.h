#pragma once

namespace fast_io
{

template<std::integral char_type,typename T>
requires (std::same_as<T,std::weak_ordering>||std::same_as<T,std::strong_ordering>||
	std::same_as<T,std::partial_ordering>)
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,T>) noexcept
{
	return 1;
}

template<::fast_io::freestanding::random_access_iterator Iter,typename T>
requires (std::same_as<T,std::weak_ordering>||std::same_as<T,std::strong_ordering>||
	std::same_as<T,std::partial_ordering>)
inline constexpr Iter print_reserve_define(io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,T>,Iter iter,T t) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	if(t<0)
	{
		if constexpr(std::same_as<char_type,char>)
			*iter='<';
		else if constexpr(std::same_as<char_type,wchar_t>)
			*iter=L'<';
		else
			*iter=u8'<';
	}
	else if(t==0)
	{
		if constexpr(std::same_as<char_type,char>)
			*iter='=';
		else if constexpr(std::same_as<char_type,wchar_t>)
			*iter=L'=';
		else
			*iter=u8'=';
	}
	else
	{
		if constexpr(std::same_as<char_type,char>)
			*iter='>';
		else if constexpr(std::same_as<char_type,wchar_t>)
			*iter=L'>';
		else
			*iter=u8'>';
	}
	++iter;
	return iter;
}


}