#pragma once

namespace fast_io
{

struct win32_wsa_service
{
	using native_handle_type = ::fast_io::win32::wsadata;
	native_handle_type wsa_data;
	/* first parameter mirrors the posix side's net_service_flags —
	 * win32 has no SIGPIPE, so the flag is a documented no-op;
	 * second parameter selects the requested winsock version (514 =
	 * MAKEWORD(2,2)) */
	inline explicit win32_wsa_service(
		::fast_io::net_service_flags flags = ::fast_io::net_service_flags::none,
		::std::uint_least16_t version = 514) FAST_IO_HERBCEPTIONS_THROWS
	{
		(void)flags;
		// The WSAStartup function directly returns the extended error code in its return value. Calling the WSAGetLastError function is unnecessary and should not be used.

		auto const res{::fast_io::win32::WSAStartup(version, __builtin_addressof(wsa_data))};

		if (res) [[unlikely]]
		{
			throw_win32_error(static_cast<::std::uint_least32_t>(res));
		}
	}
	inline win32_wsa_service(win32_wsa_service const &) = delete;
	inline win32_wsa_service &operator=(win32_wsa_service const &) = delete;
	inline ~win32_wsa_service()
	{
		::fast_io::win32::WSACleanup();
	}
};

#if !defined(__CYGWIN__) && !defined(__WINE__)
using net_service = win32_wsa_service;
#elif defined(__CYGWIN__)
/* cygwin takes the win32 network layer without winsock — the posix
 * netop chain isn't included under _WIN32 either, so net_service
 * stays the core dummy */
using net_service = ::fast_io::posix_empty_network_service;
#endif
} // namespace fast_io
