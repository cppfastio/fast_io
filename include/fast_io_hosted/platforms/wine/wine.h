#pragma once

namespace fast_io
{

enum class wine_family:std::uint_least32_t
{
unspecified_host=0,
linux_host=1,
cygwin_host=2,
native=
#if defined(_WIN32_WINE_LINUX)
linux_host
#else
unspecified_host
#endif
};

template<wine_family family,std::integral ch_type>
class basic_wine_family_io_observer
{
public:
	using char_type = ch_type;
	using native_handle_type = int;
	native_handle_type host_fd{-1};
	constexpr native_handle_type native_handle() const noexcept
	{
		return host_fd;
	}
	explicit constexpr operator bool() const noexcept
	{
		return host_fd!=-1;
	}
	constexpr native_handle_type release() noexcept
	{
		auto temp{host_fd};
		host_fd=-1;
		return temp;
	}
};

template<wine_family family>
struct
#if __has_cpp_attribute(clang::trivially_relocatable)
[[clang::trivially_relocatable]]
#endif
wine_family_file_factory
{
	using native_handle_type = int;
	int host_fd{-1};
	explicit constexpr wine_family_file_factory(int v) noexcept:host_fd(v){};
	wine_family_file_factory(wine_family_file_factory const&)=delete;
	wine_family_file_factory& operator=(wine_family_file_factory const&)=delete;
	~wine_family_file_factory()
	{
		if(host_fd!=-1)[[likely]]
			::fast_io::details::wine::wine_close<family>(host_fd);
	}
};

using wine_file_factory = wine_family_file_factory<wine_family::native>;

template<wine_family family,std::integral ch_type>
class basic_wine_family_file:public basic_wine_family_io_observer<family,ch_type>
{
public:
	using typename basic_wine_family_io_observer<family,ch_type>::char_type;
	using typename basic_wine_family_io_observer<family,ch_type>::native_handle_type;
	using file_factory_type = wine_family_file_factory<family>;
	constexpr basic_wine_family_file() noexcept = default;
	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	explicit constexpr basic_wine_family_file(native_hd host_fd1) noexcept: basic_wine_family_io_observer<family,ch_type>{host_fd1}{}
	constexpr basic_wine_family_file(decltype(nullptr)) noexcept = delete;
	constexpr basic_wine_family_file(basic_wine_family_file<family,ch_type>) noexcept=delete;
	constexpr basic_wine_family_file& operator=(basic_wine_family_file<family,ch_type>) noexcept=delete;

};

}
