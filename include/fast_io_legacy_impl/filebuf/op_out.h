#pragma once

/*
This provides a compatibility layer for printing custom types with existing ostream apis.
*/

namespace fast_io::details
{
template<typename char_type,typename T>
concept printable_to_cxx_ostringstream=std::integral<char_type>&&requires(std::basic_ostringstream<char_type>& oss,T const& t)
{
	oss<<t;
};

}

namespace fast_io::manipulators
{

template<typename... Args>
requires (sizeof...(Args)!=0)
inline std::string operator_output(Args const& ...args)
{
	constexpr bool type_error{(::fast_io::details::printable_to_cxx_ostringstream<char,Args>&&...)};
	if constexpr(type_error)
	{
		std::ostringstream oss;
		(oss<<...<<args);
		return oss.str();
	}
	else
	{
static_assert(type_error,"this type is not printable with C++ ostream facilities.");
		return std::string();
	}
	return {};
}

template<typename... Args>
requires (sizeof...(Args)!=0)
inline std::basic_string<wchar_t> woperator_output(Args const& ...args)
{
	constexpr bool type_error{(::fast_io::details::printable_to_cxx_ostringstream<wchar_t,Args>&&...)};
	if constexpr(type_error)
	{
		std::basic_ostringstream<wchar_t> oss;
		(oss<<...<<args);
		return oss.str();
	}
	else
	{
static_assert(type_error,"this type is not printable with C++ ostream facilities.");
		return std::basic_string<wchar_t>();
	}
	return {};
}

}
