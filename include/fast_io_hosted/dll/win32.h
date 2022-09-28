#pragma once

namespace fast_io
{

inline constexpr std::uint_least32_t dll_mode_to_win32_ex_flags(dll_mode mode) noexcept
{
	std::uint_least32_t flags{};
	if((mode&dll_mode::win32_dont_resolve_dll_references)==dll_mode::win32_dont_resolve_dll_references)
		flags|=0x00000001;
	if((mode&dll_mode::win32_load_ignore_code_authz_level)==dll_mode::win32_load_ignore_code_authz_level)
		flags|=0x00000010;
	if((mode&dll_mode::win32_load_library_as_datafile)==dll_mode::win32_load_library_as_datafile)
		flags|=0x00000002;
	if((mode&dll_mode::win32_load_library_as_datafile_exclusive)==dll_mode::win32_load_library_as_datafile_exclusive)
		flags|=0x00000040;
	if((mode&dll_mode::win32_load_library_as_image_resource)==dll_mode::win32_load_library_as_image_resource)
		flags|=0x00000020;
	if((mode&dll_mode::win32_load_library_search_application_dir)==dll_mode::win32_load_library_search_application_dir)
		flags|=0x00000200;
	if((mode&dll_mode::win32_load_library_search_default_dirs)==dll_mode::win32_load_library_search_default_dirs)
		flags|=0x00001000;
	if((mode&dll_mode::win32_load_library_search_dll_load_dir)==dll_mode::win32_load_library_search_dll_load_dir)
		flags|=0x00000100;
	if((mode&dll_mode::win32_load_library_search_system32)==dll_mode::win32_load_library_search_system32)
		flags|=0x00000800;
	if((mode&dll_mode::win32_load_library_search_user_dirs)==dll_mode::win32_load_library_search_user_dirs)
		flags|=0x00000400;
	if((mode&dll_mode::win32_load_with_altered_search_path)==dll_mode::win32_load_with_altered_search_path)
		flags|=0x00000008;
	if((mode&dll_mode::win32_load_library_require_signed_target)==dll_mode::win32_load_library_require_signed_target)
		flags|=0x00000080;
	if((mode&dll_mode::win32_load_library_safe_current_dirs)==dll_mode::win32_load_library_safe_current_dirs)
		flags|=0x00002000;
	return flags;
}

template<win32_family family>
class win32_family_dll_io_observer
{
public:
	using native_handle_type = void*;
	native_handle_type hmodule{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return hmodule;
	}
	explicit constexpr operator bool() const noexcept
	{
		return hmodule!=nullptr;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{hmodule};
		hmodule=nullptr;
		return temp;
	}
};

namespace details
{

inline void* create_win32_dll_9xa(char const* filename)	//9x kernel does not support Ex apis
{
	auto hmodule{::fast_io::win32::LoadLibraryA(filename)};
	if(hmodule==nullptr)
		throw_win32_error();
	return hmodule;
}
inline void* create_win32_dll_ntw(char16_t const* filename,[[maybe_unused]] dll_mode mode)
{
	auto hmodule{
#if defined(_WIN32_WINNT) && _WIN32_WINNT <= 0x0500
//Windows 2000 does not support Ex apis
::fast_io::win32::LoadLibraryW(filename)
#else
::fast_io::win32::LoadLibraryExW(filename,nullptr,dll_mode_to_win32_ex_flags(mode))
#endif
};
	if(hmodule==nullptr)
		throw_win32_error();
	return hmodule;
}

template<win32_family family>
struct win32_family_win32_dll_parameter
{
	using family_char_type = std::conditional_t<family==win32_family::wide_nt,char16_t,char>;
	dll_mode mode{};
	inline void* operator()(family_char_type const* filename_c_str)
	{
		if constexpr(family==win32_family::wide_nt)
		{
			return create_win32_dll_ntw(filename_c_str,mode);
		}
		else
		{
			return create_win32_dll_9xa(filename_c_str);
		}
	}
};

template<win32_family family,typename T>
requires (::fast_io::constructible_to_os_c_str<T>)
inline void* create_win32_family_dll_impl(T const& t,dll_mode mode)
{
	return ::fast_io::win32_family_api_common<family>(t,win32_family_win32_dll_parameter<family>{mode});
}

}

template<win32_family family>
class
#if __has_cpp_attribute(clang::trivially_relocatable)
[[clang::trivially_relocatable]]
#endif
win32_family_dll_file:public win32_family_dll_io_observer<family>
{
public:
	using native_handle_type = void*;
	constexpr win32_family_dll_file()=default;
	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	explicit constexpr win32_family_dll_file(native_hd handle) noexcept:win32_family_dll_io_observer<family>{handle}{}
	explicit constexpr win32_family_dll_file(decltype(nullptr)) noexcept = delete;

	constexpr win32_family_dll_file(win32_family_dll_io_observer<family>) noexcept=delete;
	constexpr win32_family_dll_file& operator=(win32_family_dll_io_observer<family>) noexcept=delete;
	template<::fast_io::constructible_to_os_c_str T>
	explicit win32_family_dll_file(T const& t,dll_mode mode):win32_family_dll_io_observer<family>{::fast_io::details::create_win32_family_dll_impl<family>(t,mode)}{}
	win32_family_dll_file(win32_family_dll_file const&)=delete;
	win32_family_dll_file& operator=(win32_family_dll_file const&)=delete;
	constexpr win32_family_dll_file(win32_family_dll_file&& __restrict other) noexcept:win32_family_dll_io_observer<family>{other.hmodule}
	{
		other.hmodule=nullptr;
	}
	win32_family_dll_file& operator=(win32_family_dll_file&& __restrict other) noexcept
	{
		if(this->hmodule)[[likely]]
			win32::FreeLibrary(this->hmodule);
		this->hmodule=other.hmodule;
		other.hmodule=nullptr;
		return *this;
	}
	void close()
	{
		if(this->hmodule)[[likely]]
		{
			auto ret{win32::FreeLibrary(this->hmodule)};
			this->hmodule=nullptr;
			if(!ret)
				throw_win32_error();
		}
	}
	~win32_family_dll_file()
	{
		if(this->hmodule)[[likely]]
			win32::FreeLibrary(this->hmodule);
	}
};

namespace details
{
inline void* win32_dll_load_symbol_impl(void* hmodule,char const* symbol)
{
	auto ptr{::fast_io::win32::GetProcAddress(hmodule,symbol)};
	if(ptr==nullptr)[[unlikely]]
		throw_win32_error();
	return reinterpret_cast<void*>(ptr);
}

struct win32_dll_load_impl_context
{
	void* hmodule{};
	inline void* operator()(char const* symbol) const
	{
		return win32_dll_load_symbol_impl(hmodule,symbol);
	}
};

}

template<win32_family family,::fast_io::constructible_to_os_c_str T>
inline void* dll_load_symbol(win32_family_dll_io_observer<family> pdliob,T const& symbol)
{
	return ::fast_io::posix_api_common(symbol,::fast_io::details::win32_dll_load_impl_context{pdliob.hmodule});
}

template<win32_family family,::fast_io::constructible_to_os_c_str T,::fast_io::constructible_to_os_c_str T1>
inline void* dll_load_versioned_symbol(win32_family_dll_io_observer<family> pdliob,T const& symbol,[[maybe_unused]] T1 const&)
{
	return ::fast_io::posix_api_common(symbol,::fast_io::details::win32_dll_load_impl_context{pdliob.hmodule});
}

using win32_dll_io_observer_9xa = win32_family_dll_io_observer<win32_family::ansi_9x>;
using win32_dll_file_9xa = win32_family_dll_file<win32_family::ansi_9x>;

using win32_dll_io_observer_ntw = win32_family_dll_io_observer<win32_family::wide_nt>;
using win32_dll_file_ntw = win32_family_dll_file<win32_family::wide_nt>;

using win32_dll_io_observer = win32_family_dll_io_observer<win32_family::native>;
using win32_dll_file = win32_family_dll_file<win32_family::native>;

#if !defined(__CYGWIN__) && !defined(__WINE__)
using native_dll_io_observer = win32_dll_io_observer;
using native_dll_file = win32_dll_file;
#endif

}
