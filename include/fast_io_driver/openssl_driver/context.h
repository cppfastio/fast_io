#pragma once

namespace fast_io
{

using ssl_context_observer = details::observer<SSL_CTX*>;

enum class tls_method
{
tls,
tls_server,
tls_client,
datagram_tls,
datagram_server,
datagram_client,
};

namespace details
{
inline SSL_METHOD const* get_method(tls_method m)
{
	switch(m)
	{
	case tls_method::tls:
		return TLS_method();
	case tls_method::tls_server:
		return TLS_server_method();
	case tls_method::tls_client:
		return TLS_client_method();
	case tls_method::datagram_tls:
		return DTLS_method();
	case tls_method::datagram_server:
		return DTLS_server_method();
	case tls_method::datagram_client:
		return DTLS_client_method();
	default:
		throw_openssl_error();
	};
}
}

class ssl_context:public ssl_context_observer
{
public:
	ssl_context(ossl_lib_context_observer ocob,cstring_view view,tls_method m):ssl_context_observer(SSL_CTX_new_ex(ocob.native_handle(),view.data(),details::get_method(m)))
	{
		if(this->native_handle()==nullptr)
			throw_openssl_error();
	}
	ssl_context(tls_method m):ssl_context_observer(SSL_CTX_new(details::get_method(m)))
	{
		if(this->native_handle()==nullptr)
			throw_openssl_error();
	}
	ssl_context(ssl_context const&) = delete;
	ssl_context& operator=(ssl_context const&) = delete;
	constexpr ssl_context(ssl_context&& __restrict bmv) noexcept:ssl_context_observer(bmv.native_handle())
	{
		bmv.native_handle()=nullptr;
	}
	ssl_context& operator=(ssl_context&& __restrict bmv) noexcept
	{
		if(this->native_handle())[[likely]]
			SSL_CTX_free(this->native_handle());
		this->native_handle()=bmv.native_handle();
		bmv.native_handle()=nullptr;
		return *this;
	}
	~ssl_context()
	{
		if(this->native_handle())[[likely]]
			SSL_CTX_free(this->native_handle());
	}
};

}