#pragma once
#include<windows.h>
#include<wininet.h>

#undef min
#undef max
#undef interface

namespace fast_io
{


class win32_internet_handle
{
public:
	void* handle{};
	constexpr win32_internet_handle(){}
	constexpr win32_internet_handle(void* value):handle(value){}
	
	constexpr win32_internet_handle(win32_internet_handle&& __restrict bmv) noexcept:handle(bmv.handle)
	{
		bmv.handle={};
	}
	void close()
	{
		if(handle)
		{
			if(!InternetCloseHandle(handle))
				throw fast_io::win32_error();
			handle=nullptr;
		}
	}
	win32_internet_handle& operator=(win32_internet_handle&& __restrict bmv) noexcept
	{
		if(handle)
			InternetCloseHandle(handle);
		handle=bmv.handle;
		bmv.handle={};
		return *this;
	}
	win32_internet_handle(win32_internet_handle const&) = delete;
	win32_internet_handle& operator=(win32_internet_handle const&) = delete;
	~win32_internet_handle()
	{
		if(handle)
			InternetCloseHandle(handle);
	}
};

template<typename... Args>
requires requires(Args&& ...args)
{
	InternetOpenA(::std::forward<Args>(args)...);
}
inline win32_internet_handle win32_internet_open(Args&& ...args)
{
	auto ptr(InternetOpenA(::std::forward<Args>(args)...));
	if(ptr==nullptr)
		throw fast_io::win32_error();
	return win32_internet_handle(ptr);
}


template<typename... Args>
requires requires(win32_internet_handle& handle,Args&& ...args)
{
	InternetConnectA(handle.handle,::std::forward<Args>(args)...);
}
inline win32_internet_handle win32_internet_connect(win32_internet_handle& handle,Args&& ...args)
{
	auto ptr(InternetConnectA(handle.handle,::std::forward<Args>(args)...));
	if(ptr==nullptr)
		throw fast_io::win32_error();
	return win32_internet_handle(ptr);
}

template<typename... Args>
requires requires(win32_internet_handle& handle,Args&& ...args)
{
	HttpOpenRequestA(handle.handle,::std::forward<Args>(args)...);
}
inline win32_internet_handle win32_http_open_request(win32_internet_handle& handle,Args&& ...args)
{
	auto ptr(HttpOpenRequestA(handle.handle,::std::forward<Args>(args)...));
	if(ptr==nullptr)
		throw fast_io::win32_error();
	return win32_internet_handle(ptr);
}

inline void win32_http_send_request(win32_internet_handle& handle)
{
	if(!HttpSendRequestA(handle.handle,nullptr,0,nullptr,0))
		throw fast_io::win32_error();
}

inline void win32_http_send_request(win32_internet_handle& handle,cstring_view sview)
{
	if(!HttpSendRequestA(handle.handle,sview.data(),sview.size(),nullptr,0))
		throw fast_io::win32_error();
}
inline void win32_http_send_request(win32_internet_handle& handle,cstring_view sview,std::span<std::byte> extra)
{
	if(!HttpSendRequestA(handle.handle,sview.data(),sview.size(),extra.data(),extra.size()))
		throw fast_io::win32_error();
}

template<std::integral ch_type>
class basic_win32_internet_io_observer
{
public:
	using char_type = ch_type;
	using native_handle_type = void*;
	native_handle_type handle{};
	constexpr operator bool() noexcept
	{
		return handle;
	}
	constexpr auto& native_handle() noexcept
	{
		return handle;
	}
	constexpr auto& native_handle() const noexcept
	{
		return handle;
	}
};

template<std::integral ch_type>
class basic_win32_internet_https_client:public basic_win32_internet_io_observer<ch_type>
{
public:
	using char_type = ch_type;
	using native_handle_type = basic_win32_internet_io_observer<ch_type>::native_handle_type;
	win32_internet_handle session;
	win32_internet_handle connection;
	win32_internet_handle request;
	constexpr basic_win32_internet_https_client()=default;
	basic_win32_internet_https_client(cstring_view host,cstring_view method,cstring_view object_name,std::uint_least32_t port=443):
		session(win32_internet_open("Microsoft Internet Explorer",INTERNET_OPEN_TYPE_PRECONFIG,nullptr,nullptr,0)),
		connection(win32_internet_connect(session,host.data(),port,nullptr,nullptr,INTERNET_SERVICE_HTTP,0,0)),
		request(win32_http_open_request(connection,method.data(),object_name.data(),HTTP_VERSION,nullptr,nullptr,
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE | INTERNET_FLAG_KEEP_CONNECTION |
		INTERNET_FLAG_NO_AUTO_REDIRECT | INTERNET_FLAG_READ_PREFETCH | INTERNET_FLAG_NO_COOKIES |
		INTERNET_FLAG_NO_AUTH |	INTERNET_FLAG_RESTRICTED_ZONE |	INTERNET_FLAG_CACHE_IF_NET_FAIL |
		INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |	INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_NO_UI,
		0))
	{
		win32_http_send_request(request);
		this->native_handle()=request.handle;
	}
};

template<std::integral ch_type,::std::contiguous_iterator Iter>
inline Iter read(basic_win32_internet_io_observer<ch_type> iob,Iter begin,Iter end)
{
	unsigned long readed{};
	if(!InternetReadFile(iob.handle,::std::to_address(begin),sizeof(*begin)*(end-begin),__builtin_addressof(readed)))
		throw fast_io::win32_error();
	return begin+readed/sizeof(begin);
}

template<std::integral ch_type,::std::contiguous_iterator Iter>
inline Iter write(basic_win32_internet_io_observer<ch_type> iob,Iter begin,Iter end)
{
	unsigned long written{};
	if(!InternetWriteFile(iob.handle,::std::to_address(begin),sizeof(*begin)*(end-begin),__builtin_addressof(written)))
		throw fast_io::win32_error();
	return begin+written/sizeof(begin);
}

using win32_internet_io_observer = basic_win32_internet_io_observer<char>;

using ibuf_win32_internet_io_observer = fast_io::basic_ibuf<win32_internet_io_observer>;

using win32_internet_https_client = basic_win32_internet_https_client<char>;

using ibuf_win32_internet_https_client = fast_io::basic_ibuf<win32_internet_https_client>;
}
