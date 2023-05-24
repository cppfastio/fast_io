#pragma once

namespace fast_io
{


struct empty_decorator
{
};

template<std::integral ch_type,
typename internaltype=empty_decorator,
typename externaltype=empty_decorator>
struct basic_decorators
{
	using internal_type = ch_type;
	using internal_decorator_type=internaltype;
	using external_decorator_type=externaltype;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	internal_decorator_type internal_decorator;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	external_decorator_type external_decorator;
};

template<std::integral char_type,typename internaltype,typename externaltype>
requires (!std::same_as<internaltype,empty_decorator>)
inline constexpr auto internal_decorator(basic_decorators<char_type,internaltype,externaltype>& decos) noexcept
{
	return io_deco_ref(decos.internal_decorator);
}

template<std::integral char_type,typename internaltype,typename externaltype>
requires (!std::same_as<externaltype,empty_decorator>)
inline constexpr auto external_decorator(basic_decorators<char_type,internaltype,externaltype>& decos) noexcept
{
	return io_deco_ref(decos.external_decorator);
}


}