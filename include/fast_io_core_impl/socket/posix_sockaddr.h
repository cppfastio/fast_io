#pragma once

namespace fast_io
{
#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : 4324)
#endif
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
alignas(16)
posix_sockaddr
{
	std::uint_least16_t sa_family{};
};


struct posix_in_addr
{
	unsigned char address[4]{};
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
alignas(16)
posix_sockaddr_in
{
	std::uint_least16_t sin_family{};
	std::uint_least16_t sin_port{};
	posix_in_addr sin_addr{};
};

struct posix_in6_addr
{
	::std::uint_least16_t address[8]{};
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
posix_sockaddr_in6
{
	std::uint_least16_t sin6_family{};
	std::uint_least16_t sin6_port{};
	std::uint_least32_t sin6_flowinfo{};
	posix_in6_addr sin6_addr{};
	std::uint_least32_t sin6_scoped_id{};
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
alignas(128)
posix_sockaddr_storage
{
	std::uint_least16_t ss_family{};
};

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
}
