#include<__wine_unix.h>
#include<cstring>

namespace __wineunix
{

inline constexpr bool sanitize_file_path(void const* str, size_t len) noexcept
{
	char const* cstr{reinterpret_cast<char const*>(str)};
	if(::std::strnlen(cstr,len) == len)
	{
		return cstr[len]==0;
	}
	return false;
}
#if 0
inline constexpr ::__wineunix::errvalue
#endif
}

extern "C"
{

__WINE_UNIX_DLLEXPORT __wine_unix_result1_t __WINE_UNIX_DEFAULTCALL __wine_unix_ret_openat(__wine_host_fd_t dirfd, void const* filename, size_t filenamelen, __wine_host_flags_t flags, __wine_host_mode_t hostfd) __WINE_UNIX_NOEXCEPT;
{
	
}

}