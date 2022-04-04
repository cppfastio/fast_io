#pragma once

namespace fast_io
{

namespace details
{

template<::fast_io::constructible_to_os_c_str path_type>
inline void* posix_load_l10n_impl(path_type const& p,lc_locale& loc)
{
}

}

class posix_l10n
{
public:
	lc_locale loc{};
	void* dll_handle{};
	constexpr posix_l10n() noexcept=default;

	template<::fast_io::constructible_to_os_c_str path_type>
	explicit posix_l10n(path_type const& p)
	{
		this->dll_handle=::fast_io::details::posix_load_l10n_impl(loc);
	}

	explicit constexpr operator bool() const noexcept
	{
		return dll_handle!=nullptr;
	}
	void close() noexcept
	{
		if(dll_handle)[[likely]]
		{
			fast_io::win32::FreeLibrary(dll_handle);
			dll_handle=nullptr;
		}
	}
	posix_l10n& operator=(posix_l10n const&)=delete;
	posix_l10n(posix_l10n const&)=delete;
	posix_l10n& operator=(posix_l10n&& __restrict other) noexcept
	{
		close();
		loc=other.loc;
		dll_handle=other.dll_handle;
		other.dll_handle=nullptr;
		other.loc={};
		return *this;
	}
	~posix_l10n()
	{
		close();
	}
};

}
