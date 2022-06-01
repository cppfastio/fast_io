#pragma once

namespace fast_io
{

inline constexpr int dll_mode_to_posix_rtld_mode(dll_mode mode) noexcept
{
	int m{};
#if defined(RTLD_LAZY)
	if((mode&dll_mode::posix_rtld_lazy)==dll_mode::posix_rtld_lazy)
		m|=RTLD_LAZY;
#endif
#if defined(RTLD_NOW)
	if((mode&dll_mode::posix_rtld_now)==dll_mode::posix_rtld_now)
		m|=RTLD_NOW;
#endif
#if defined(RTLD_BINDING_MASK)
	if((mode&dll_mode::posix_rtld_binding_mask)==dll_mode::posix_rtld_binding_mask)
		m|=RTLD_BINDING_MASK;
#endif
#if defined(RTLD_GLOBAL)
	if((mode&dll_mode::posix_rtld_global)==dll_mode::posix_rtld_global)
		m|=RTLD_GLOBAL;
#endif
#if defined(RTLD_LOCAL)
	if((mode&dll_mode::posix_rtld_local)==dll_mode::posix_rtld_local)
		m|=RTLD_LOCAL;
#endif
#if defined(RTLD_NODELETE)
	if((mode&dll_mode::posix_rtld_nodelete)==dll_mode::posix_rtld_nodelete)
		m|=RTLD_NODELETE;
#endif
#if defined(RTLD_NOLOAD)
	if((mode&dll_mode::posix_rtld_noload)==dll_mode::posix_rtld_noload)
		m|=RTLD_NOLOAD;
#endif
#if defined(RTLD_DEEPBIND)
	if((mode&dll_mode::posix_rtld_deepbind)==dll_mode::posix_rtld_deepbind)
		m|=RTLD_DEEPBIND;
#endif
	return m;
}

class posix_dll_io_observer
{
public:
	using native_handle_type = void*;
	native_handle_type rtld_handle{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return rtld_handle;
	}
	explicit constexpr operator bool() const noexcept
	{
		return rtld_handle;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{rtld_handle};
		rtld_handle=nullptr;
		return temp;
	}
};

namespace details
{

[[noreturn]] inline void throw_posix_rtld_error(int err=EINVAL)
{
	int en{errno};
	if(!en)
	{
		en=err;
	}
	throw_posix_error(en);
}

inline void* create_posix_rtld(char const* filename,dll_mode mode)
{
	errno={};
	auto hd{noexcept_call(dlopen,filename,::fast_io::dll_mode_to_posix_rtld_mode(mode))};
	if(hd==nullptr)
	{
		::fast_io::details::throw_posix_rtld_error(
#if defined(ENOENT)
		ENOENT
#endif
		);
	}
	return hd;
}

struct posix_rtld_impl_context
{
	dll_mode mode{};
	inline void* operator()(char const* filename) const
	{
		return create_posix_rtld(filename,mode);
	}
};

template<typename T>
inline void* create_posix_rtld_impl(T const& t,dll_mode mode)
{
	return ::fast_io::posix_api_common(t,posix_rtld_impl_context{mode});
}

}

class
#if __has_cpp_attribute(clang::trivially_relocatable)
[[clang::trivially_relocatable]]
#endif
posix_dll_file:public posix_dll_io_observer
{
public:
	using native_handle_type = void*;
	constexpr posix_dll_file()=default;
	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	explicit constexpr posix_dll_file(native_hd handle) noexcept:posix_dll_io_observer{handle}{}
	explicit constexpr posix_dll_file(decltype(nullptr)) noexcept = delete;

	constexpr posix_dll_file(posix_dll_io_observer) noexcept=delete;
	constexpr posix_dll_file& operator=(posix_dll_io_observer) noexcept=delete;
	template<::fast_io::constructible_to_os_c_str T>
	explicit posix_dll_file(T const& t,dll_mode mode):posix_dll_io_observer{::fast_io::details::create_posix_rtld_impl(t,mode)}{}
	posix_dll_file(posix_dll_file const&)=delete;
	posix_dll_file& operator=(posix_dll_file const&)=delete;
	constexpr posix_dll_file(posix_dll_file&& __restrict other) noexcept:posix_dll_io_observer{other.rtld_handle}
	{
		other.rtld_handle=nullptr;
	}
	posix_dll_file& operator=(posix_dll_file&& __restrict other) noexcept
	{
		if(this->rtld_handle)[[likely]]
			noexcept_call(dlclose,this->rtld_handle);
		this->rtld_handle=other.rtld_handle;
		other.rtld_handle=nullptr;
		return *this;
	}
	void close()
	{
		if(this->rtld_handle)[[likely]]
		{
			errno={};
			auto ret{noexcept_call(dlclose,this->rtld_handle)};
			this->rtld_handle=nullptr;
			if(ret)
			{
				::fast_io::details::throw_posix_rtld_error();
			}
		}
	}
	~posix_dll_file()
	{
		if(this->rtld_handle)[[likely]]
			noexcept_call(dlclose,this->rtld_handle);
	}
};

namespace details
{

inline void* posix_dll_load_symbol_impl(void* rtld_handle,char const* symbol)
{
	auto ptr{noexcept_call(dlsym,rtld_handle,symbol)};
	if(ptr==nullptr)[[unlikely]]
		throw_posix_error(EINVAL);
	return ptr;
}

struct posix_dll_load_impl_context
{
	void* rtld_handle{};
	inline void* operator()(char const* symbol) const
	{
		return posix_dll_load_symbol_impl(rtld_handle,symbol);
	}
};

#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2,0)
inline void* posix_dll_load_vers_symbol_impl(void* rtld_handle,char const* symbol,char const* vers)
{
	auto ptr{noexcept_call(dlvsym,rtld_handle,symbol,vers)};
	if(ptr==nullptr)[[unlikely]]
		throw_posix_error(EINVAL);
	return ptr;
}

struct posix_dll_load_versioned_symbol_impl_context
{
	void* rtld_handle{};
	char const* vers{};
	inline void* operator()(char const* symbol) const
	{
		return posix_dll_load_vers_symbol_impl(rtld_handle,symbol,vers);
	}
};

#endif
#endif

}

template<::fast_io::constructible_to_os_c_str T>
inline void* dll_load_symbol(posix_dll_io_observer pdliob,T const& symbol)
{
	return ::fast_io::posix_api_common(symbol,::fast_io::details::posix_dll_load_impl_context{pdliob.rtld_handle});
}

template<::fast_io::constructible_to_os_c_str T,::fast_io::constructible_to_os_c_str T1>
inline void* dll_load_versioned_symbol(posix_dll_io_observer pdliob,T const& symbol,[[maybe_unused]] T1 const& vers)
{
#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2,0)
	return ::fast_io::posix_api_common(vers,[&](char const* ver)
	{
		return ::fast_io::posix_api_common(symbol,::fast_io::details::posix_dll_load_versioned_symbol_impl_context{pdliob.rtld_handle,ver});
	});
#else
	return ::fast_io::posix_api_common(symbol,::fast_io::details::posix_dll_load_impl_context{pdliob.rtld_handle});
#endif
#else
	return ::fast_io::posix_api_common(symbol,::fast_io::details::posix_dll_load_impl_context{pdliob.rtld_handle});
#endif
}

using native_dll_io_observer = posix_dll_io_observer;
using native_dll_file = posix_dll_file;

}
