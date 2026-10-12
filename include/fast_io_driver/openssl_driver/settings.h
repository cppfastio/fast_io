#pragma once

namespace fast_io
{

using settings_observer = details::observer<OPENSSL_INIT_SETTINGS *>;

class settings : public settings_observer
{
public:
	using native_handle_type = OPENSSL_INIT_SETTINGS *;
	constexpr settings(native_handle_type handle) noexcept
		: settings_observer{handle}
	{}
	settings() noexcept
		: settings_observer{OPENSSL_INIT_new()}
	{}
	settings(settings const &) = delete;
	settings &operator=(settings const &) = delete;

	constexpr settings(settings &&__restrict bmv) noexcept
		: settings_observer(bmv.handle)
	{
		bmv.handle = nullptr;
	}
	settings &operator=(settings &&__restrict bmv) noexcept
	{
		if (this->native_handle()) [[likely]]
		{
			OPENSSL_INIT_free(this->native_handle());
		}
		this->native_handle() = bmv.native_handle();
		bmv->native_handle() = nullptr;
		return *this;
	}
	constexpr auto release() noexcept
	{
		auto temp{this->native_handle()};
		this->native_handle() = nullptr;
		return temp;
	}
	~settings()
	{
		if (this->native_handle()) [[likely]]
		{
			OPENSSL_INIT_free(this->native_handle());
		}
	}
};

}; // namespace fast_io