#pragma once

namespace fast_io
[

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

template<std::integral ch_type,wine_family family>
class basic_wine_family_io_observer
{
public:
	using char_type = ch_type;
	using native_handle_type = int;
	native_handle_type fd{-1};
	constexpr native_handle_type native_handle() const noexcept
	{
		return fd;
	}
	explicit constexpr operator bool() const noexcept
	{
		return fd!=-1;
	}
	constexpr native_handle_type release() noexcept
	{
		auto temp{fd};
		fd=-1;
		return temp;
	}
};

template<std::integral ch_type,wine_family family>
class basic_wine_family_file:public basic_wine_family_io_observer<family>
{
public:
	using typename basic_wine_family_io_observer<ch_type,family>::char_type;
	using typename basic_wine_family_io_observer<ch_type,family>::native_handle_type;
	
};

]
