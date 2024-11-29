#pragma once

namespace fast_io
{

struct win32_wsa_service
{
	using native_handle_type = ::fast_io::win32::wsadata;
	native_handle_type wsa_data;
	inline explicit win32_wsa_service(::std::uint_least16_t version)
	{
		if (::fast_io::win32::WSAStartup(version, __builtin_addressof(wsa_data)))
		{
			throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
		}
	}
	inline win32_wsa_service()
		: win32_wsa_service(514)
	{}
	inline win32_wsa_service(win32_wsa_service const &) = delete;
	inline win32_wsa_service &operator=(win32_wsa_service const &) = delete;
	inline ~win32_wsa_service()
	{
		::fast_io::win32::WSACleanup();
	}
};

#if !defined(__CYGWIN__) && !defined(__WINE__)
using net_service = win32_wsa_service;
#endif
} // namespace fast_io
