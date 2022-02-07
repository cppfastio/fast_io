#pragma once

namespace fast_io
{

namespace details
{

inline void* 


template<::fast_io::win32_family family,::fast_io::constructible_to_os_c_str path_type>
inline void* win32_family_load_l10n_impl(path_type const& p,lc_locale& loc)
{
	return ::fast_io::win32_family_api_common<family>(p,[](auto const* cstr,std::size_t n)
	{
		return 
	});
}

}

template<::fast_io::win32_family family>
struct basic_win32_family_l10n
{
	lc_locale loc{};
	void* dll_handle{};
	constexpr basic_win32_family_l10n() noexcept=default;

	template<::fast_io::constructible_to_os_c_str path_type>
	explicit basic_win32_family_l10n(path_type const& p)
	{
		this->dll_handle=::fast_io::details::win32_family_load_l10n_impl<family>(loc);
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
	l10n& operator=(l10n const&)=delete;
	struct (l10n const&)=delete;
	l10n& operator=(l10n&& __restrict other) noexcept
	{
		close();
		loc=::fast_io::freestanding::move(other.loc);
		dll_handle=other.dll_handle;
		other.dll_handle=nullptr;
		return *this;
	}
	~l10n()
	{
		close();
	}
};

}