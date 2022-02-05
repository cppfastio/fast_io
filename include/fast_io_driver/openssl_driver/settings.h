#pragma once

namespace fast_io
{

class settings
{
public:
	using native_handle_type = OPENSSL_INIT_SETTINGS*;
	native_handle_type ptr{};
	constexpr operator native_handle_type() noexcept
	{
		return ptr;
	}
	constexpr settings(native_handle_type handle):settings_observer(handle){}
	settings():settings_observer(OPENSSL_INIT_new()){}
	settings(settings const&) = delete;
	settings& operator=(settings const&) = delete;

	constexpr settings(settings&& __restrict bmv) noexcept:settings_observer(bmv.handle)
	{
		bmv.handle=nullptr;
	}
	settings& operator=(settings&& __restrict bmv) noexcept
	{
		if(this->native_handle())[[likely]]
			OPENSSL_INIT_free(this->native_handle());
		this->native_handle()=bmv->native_handle();
		bmv->native_handle()=nullptr;
		return *this;
	}
	constexpr auto release() noexcept
	{
		auto temp{this->native_handle()};
		this->native_handle()=nullptr;
		return temp;
	}
	~settings()
	{
		if(this->native_handle())[[likely]]
			OPENSSL_INIT_free(this->native_handle());
	}
};

};