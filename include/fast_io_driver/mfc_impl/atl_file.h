#pragma once

namespace fast_io
{

template<std::integral T>
class basic_atl_io_observer
{
public:
	using char_type = T;
	using native_handle_type = CAtlFile*;
	native_handle_type phandle{};
	explicit constexpr operator bool() const noexcept
	{
		return phandle;
	}
	template<win32_family family>
	explicit operator basic_win32_family_io_observer<family,char_type>() const noexcept
	{
		return {phandle->m_hFile};
	}

	template<nt_family family>
	explicit operator basic_nt_family_io_observer<family,char_type>() const noexcept
	{
		return {phandle->m_hFile};
	}
	constexpr native_handle_type native_handle() const noexcept
	{
		return phandle;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{phandle};
		phandle=nullptr;
		return temp;
	}
};

template<std::integral T>
inline constexpr basic_atl_io_observer<T> io_value_handle(basic_atl_io_observer<T> t) noexcept
{
	return t;
}

struct com_error
{
	HRESULT hr{};
};

namespace details
{

inline void throw_com_error(HRESULT hr)
{
#ifdef __cpp_exceptions
#if defined(_MSC_VER) && (!defined(_HAS_EXCEPTIONS) || _HAS_EXCEPTIONS == 0)
	fast_terminate();
#else
	throw com_error{hr};
#endif
#else
	fast_terminate();
#endif
}

#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline void atl_write_n_impl(CAtlFile* cfp,std::byte const* first_ptr,std::size_t n)
{
	if constexpr(sizeof(std::size_t)>sizeof(std::uint_least32_t))
	{
		while(n)
		{
			constexpr std::size_t sz_max{static_cast<std::size_t>(UINT_LEAST32_MAX)};
			std::size_t write_this_round{n};
			if(sz_max<write_this_round)
			{
				write_this_round=sz_max;
			}
			auto hresult{cfp->Write(first_ptr,static_cast<std::uint_least32_t>(write_this_round))};
			if(hresult!=S_OK)
				throw_com_error(hresult); 
			n-=write_this_round;
		}
	}
	else
	{
		cfp->Write(first_ptr,static_cast<std::uint_least32_t>(n));
	}
}

inline void atl_write_impl(CAtlFile* cfp,void const* first,void const* last)
{
	char const* first_ptr{reinterpret_cast<char const*>(first)};
	char const* last_ptr{reinterpret_cast<char const*>(last)};
	atl_write_n_impl(cfp,reinterpret_cast<std::byte const*>(first_ptr),static_cast<std::size_t>(last_ptr-first_ptr));
}

inline std::size_t atl_read_impl(CAtlFile* cfp,void* first,std::size_t to_read)
{
	if constexpr(sizeof(std::size_t)>4)
		if(static_cast<std::size_t>(UINT_LEAST32_MAX)<to_read)
			to_read=static_cast<std::size_t>(UINT_LEAST32_MAX);
	DWORD ret{};
	auto hr{cfp->Read(first,static_cast<std::uint_least32_t>(to_read),ret)};
	if(hr!=S_OK)
		throw_com_error(hr);
	return static_cast<std::size_t>(ret);
}

#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline void atl_scatter_write_impl(CAtlFile* cfp,io_scatter_t const* scats,std::size_t n)
{
	auto i{scats};
	auto e{scats+n};
	for(;i!=e;++i)
	{
		atl_write_n_impl(cfp,reinterpret_cast<std::byte const*>(scats->base),scats->len);
	}
}

}

template<std::integral T,::std::contiguous_iterator Iter>
inline void write(basic_atl_io_observer<T> hd,Iter first,Iter last)
{
	::fast_io::details::atl_write_impl(hd.phandle,::std::to_address(first),::std::to_address(last));
}

template<std::integral T>
inline void scatter_write(basic_atl_io_observer<T> hd,io_scatters_t scatters)
{
	::fast_io::details::atl_scatter_write_impl(hd.phandle,scatters.base,scatters.len);
}

template<std::integral T>
inline void flush(basic_atl_io_observer<T> hd)
{
	hd.phandle->Flush();
}

template<std::integral T,::std::contiguous_iterator Iter>
inline Iter read(basic_atl_io_observer<T> hd,Iter first,Iter last)
{
	return first+atl_read_impl(hd.phandle,::std::to_address(first),(last-first)*sizeof(*first))/sizeof(T);
}

template<std::integral ch_type>
class basic_atl_file:public basic_atl_io_observer<ch_type>
{
public:
	using char_type = ch_type;
	using native_handle_type = CAtlFile*;
	constexpr basic_atl_file() noexcept=default;
	template<typename T>
	requires std::same_as<T,native_handle_type>
	explicit constexpr basic_atl_file(T hd) noexcept:basic_atl_io_observer<ch_type>{hd}{}
	explicit constexpr basic_atl_file(decltype(nullptr)) noexcept = delete;

	constexpr basic_atl_file(basic_atl_io_observer<char_type>) noexcept=delete;
	constexpr basic_atl_file& operator=(basic_atl_io_observer<char_type>) noexcept=delete;
	template<win32_family family>
	explicit basic_atl_file(basic_win32_family_file<family,char_type>&& hd,open_mode):
		basic_atl_io_observer<char_type>{new CAtlFile(hd.handle)}
	{
		hd.release();
	}
	template<nt_family family>
	explicit basic_atl_file(basic_nt_family_file<family,char_type>&& hd,open_mode):
		basic_atl_io_observer<char_type>{new CAtlFile(hd.handle)}
	{
		hd.release();
	}
	basic_atl_file(nt_fs_dirent fsdirent,open_mode om,perms pm=static_cast<perms>(436)):
		basic_atl_file(basic_win32_file<char_type>(fsdirent,om,pm),om)
	{}
	template<::fast_io::constructible_to_os_c_str T>
	basic_atl_file(T const& t,open_mode om,perms pm=static_cast<perms>(436)):
		basic_atl_file(basic_win32_file<char_type>(t,om,pm),om)
	{}
	template<::fast_io::constructible_to_os_c_str T>
	basic_atl_file(nt_at_entry nate,T const& t,open_mode om,perms pm=static_cast<perms>(436)):
		basic_atl_file(basic_win32_file<char_type>(nate,t,om,pm),om)
	{}

	basic_atl_file(basic_atl_file const& mcf):basic_atl_io_observer<ch_type>{mcf.phandle->Duplicate()}{}
	basic_atl_file& operator=(basic_atl_file const& mcf)
	{
		auto temp{mcf.phandle->Duplicate()};
		delete this->phandle;
		this->phandle=temp;
		return *this;
	}
	basic_atl_file(basic_atl_file&& mcf) noexcept:basic_atl_io_observer<ch_type>{mcf.phandle}
	{
		mcf.phandle=nullptr;
	}
	basic_atl_file& operator=(basic_atl_file&& __restrict mcf) noexcept
	{
		delete this->phandle;
		this->phandle=mcf.phandle;
		mcf.phandle=nullptr;
		return *this;
	}
	void close() noexcept
	{
		delete this->phandle;
		this->phandle=nullptr;
	}
	inline constexpr void reset(native_handle_type newhandle=nullptr) noexcept
	{
		delete this->phandle;
		this->phandle=newhandle;
	}
	~basic_atl_file()
	{
		delete this->phandle;
	}
};

using atl_io_observer=basic_atl_io_observer<char>;
using atl_file=basic_atl_file<char>;
using u8atl_io_observer=basic_atl_io_observer<char8_t>;
using u8atl_file=basic_atl_file<char8_t>;
using watl_io_observer=basic_atl_io_observer<wchar_t>;
using watl_file=basic_atl_file<wchar_t>;
using u16atl_io_observer=basic_atl_io_observer<char16_t>;
using u16atl_file=basic_atl_file<char16_t>;
using u32atl_io_observer=basic_atl_io_observer<char32_t>;
using u32atl_file=basic_atl_file<char32_t>;

}
