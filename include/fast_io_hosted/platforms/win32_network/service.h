#pragma once

namespace fast_io
{

struct win32_wsa_service
{
	using native_handle_type = win32::wsadata;
	win32::wsadata wsa_data;
	explicit win32_wsa_service(std::uint_least16_t version)
	{
		if(win32::WSAStartup(version,__builtin_addressof(wsa_data)))
			throw_win32_error(static_cast<std::uint_least32_t>(win32::WSAGetLastError()));
	}
	win32_wsa_service():win32_wsa_service(514)
	{}
	win32_wsa_service(win32_wsa_service const&)=delete;
	win32_wsa_service& operator=(win32_wsa_service const&)=delete;
	~win32_wsa_service()
	{
		win32::WSACleanup();
	}
};

#if !defined(__CYGWIN__) && !defined(__WINE__)
using net_service = win32_wsa_service;
#endif
}
