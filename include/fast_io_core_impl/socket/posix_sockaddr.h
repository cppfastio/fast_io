#pragma once

#include <cstdint>

namespace fast_io
{

/* ABI twins of the C structs: same size, same member offsets, NATURAL
 * alignment. Do NOT alignas these — real struct sockaddr aligns to 2,
 * sockaddr_in to 4, and pointers of these types are overlaid on
 * kernel/libc-allocated results (getifaddrs, getaddrinfo, accept,
 * getpeername) that only meet the C alignment; a stricter requirement
 * is UB on every read */
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	posix_sockaddr
{
	::std::uint_least16_t sa_family{};
	unsigned char sa_data[14]{};
};

struct posix_in_addr
{
	unsigned char address[4]{};
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	posix_sockaddr_in
{
	::std::uint_least16_t sin_family{};
	::std::uint_least16_t sin_port{};
	posix_in_addr sin_addr{};
	unsigned char sin_zero[8]{};
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
	::std::uint_least16_t sin6_family{};
	::std::uint_least16_t sin6_port{};
	::std::uint_least32_t sin6_flowinfo{};
	posix_in6_addr sin6_addr{};
	::std::uint_least32_t sin6_scoped_id{};
};

/* sockaddr_storage: 128 bytes, aligned like the C struct (a u64 member,
 * matching glibc __ss_align/unsigned long and winsock __ss_align/__int64) */
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	posix_sockaddr_storage
{
	::std::uint_least16_t ss_family{};
	unsigned char ss_padding[118]{};
	::std::uint_least64_t ss_align{};
};

static_assert(sizeof(posix_sockaddr) == 16 && alignof(posix_sockaddr) == alignof(::std::uint_least16_t));
static_assert(sizeof(posix_sockaddr_in) == 16);
static_assert(sizeof(posix_sockaddr_in6) == 28);
static_assert(sizeof(posix_sockaddr_storage) == 128);

} // namespace fast_io
