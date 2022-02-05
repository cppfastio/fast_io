#pragma once

namespace fast_io
{

using ossl_lib_context_observer = details::observer<OSSL_LIB_CTX*>;

class openssl_context:public ossl_lib_context_observer
{
public:
	openssl_context():ossl_lib_context_observer(OSSL_LIB_CTX_new())
	{
		if(this->native_handle()==nullptr)
			throw_openssl_error();
	}
	openssl_context(openssl_context const&) = delete;
	openssl_context& operator=(openssl_context const&) = delete;
	constexpr openssl_context(openssl_context&& __restrict bmv) noexcept:ossl_lib_context_observer(bmv.native_handle())
	{
		bmv.native_handle()=nullptr;
	}
	openssl_context& operator=(openssl_context&& __restrict bmv) noexcept
	{
		if(this->native_handle())[[likely]]
			OSSL_LIB_CTX_free(this->native_handle());
		this->native_handle()=bmv.native_handle();
		bmv.native_handle()=nullptr;
		return *this;
	}
	~openssl_context()
	{
		if(this->native_handle())[[likely]]
			OSSL_LIB_CTX_free(this->native_handle());
	}
};

};