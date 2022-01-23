#pragma once

namespace fast_io::details
{
enum class posix_api_22
{
renameat,
linkat
};
enum class posix_api_12
{
symlinkat
};
enum class posix_api_1x
{
faccessat,
utimensat,
fchmodat,
fchownat,
fstatat,
mkdirat,
mknodat,
unlinkat
};

template<constructible_to_path path_type>
inline constexpr auto to_its_cstring_view(path_type&& pth) noexcept
{
	if constexpr(std::constructible_from<cstring_view,path_type>)
	{
		return cstring_view(pth);
	}
	else if constexpr(std::constructible_from<u8cstring_view,path_type>)
	{
		return u8cstring_view(pth);
	}
	else if constexpr(std::constructible_from<u16cstring_view,path_type>)
	{
		return u16cstring_view(pth);
	}
	else if constexpr(std::constructible_from<u32cstring_view,path_type>)
	{
		return u32cstring_view(pth);
	}
	else
		return wcstring_view(pth);
}

}
