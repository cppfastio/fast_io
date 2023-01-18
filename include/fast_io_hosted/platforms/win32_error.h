#pragma once

namespace fast_io
{

namespace details
{

inline constexpr int eother_value{
#if defined(EOTHER)
EOTHER
#else
131
#endif
};

inline constexpr int win32_code_to_errno(std::uint_least32_t code) noexcept
{
	switch(code)
	{
#include"win32_code_to_errno.h"
default:
return eother_value;
	};
}

inline constexpr std::uint_least32_t errno_to_win32_code(int code) noexcept
{
	switch(code)
	{
#include"errno_to_win32_code.h"
default:
return 0x00000057;
	};
}

inline constexpr bool win32_code_equals_errno(std::uint_least32_t code,int ec) noexcept
{
	auto e{win32_code_to_errno(code)};
	if(e==eother_value)
	{
		return false;
	}
	return e==ec;
}

inline bool nt_code_equivalent_impl(std::uintptr_t domain,std::uintptr_t code,std::uint_least32_t e) noexcept
{
	if(nt_domain_value==domain)
	{
		return code==e;
	}
	else if(win32_domain_value==domain||posix_domain_value==domain)
	{
		auto lasterr{::fast_io::win32::nt::rtl_nt_status_to_dos_error(e)};
		if(win32_domain_value==domain)
		{
			return lasterr==code;
		}
		else
		{
			return win32_code_equals_errno(lasterr,static_cast<int>(static_cast<unsigned>(code)));
		}
	}
	else
	{
		return false;
	}
}

inline bool win32_code_equivalent_impl(std::uintptr_t domain,std::uintptr_t code,std::uint_least32_t e) noexcept
{
	if(win32_domain_value==domain)
	{
		return code==e;
	}

	else if(nt_domain_value==domain)
	{
#ifdef _WIN32_WINDOWS
		return false;
#else
		return ::fast_io::win32::nt::rtl_nt_status_to_dos_error(static_cast<std::uint_least32_t>(code))==e;
#endif
	}
	else if(posix_domain_value==domain)
	{
		return win32_code_equals_errno(e,static_cast<int>(static_cast<unsigned>(code)));
	}
	else
	{
		return false;
	}
}

inline bool posix_code_equivalent_impl(std::uintptr_t domain,std::uintptr_t code,int e) noexcept
{
	if(posix_domain_value==domain)
	{
		using common_type = std::common_type_t<std::uintptr_t,unsigned>;
		return static_cast<common_type>(static_cast<unsigned>(e))==code;
	}
	else if(win32_domain_value==domain||nt_domain_value==domain)
	{
		std::uint_least32_t win32cd{};
		if(nt_domain_value==domain)
		{
#ifdef _WIN32_WINDOWS
			win32cd=0x00000057;			
#else
			win32cd=::fast_io::win32::nt::rtl_nt_status_to_dos_error(static_cast<std::uint_least32_t>(code));
#endif
		}
		else
		{
			win32cd=static_cast<std::uint_least32_t>(code);
		}
		return win32_code_equals_errno(win32cd,static_cast<int>(static_cast<unsigned>(e)));
	}
	else
	{
		return false;
	}
}

inline constexpr std::uint_least32_t to_win32_code_impl(std::uintptr_t domain,std::uintptr_t code) noexcept
{
	if(win32_domain_value==domain)
	{
		return static_cast<std::uint_least32_t>(code);
	}
	else if(nt_domain_value==domain)
	{
#ifdef _WIN32_WINDOWS
		return 0x00000057;
#else
		return ::fast_io::win32::nt::rtl_nt_status_to_dos_error(static_cast<std::uint_least32_t>(code));
#endif
	}
	else if(posix_domain_value==domain)
	{
		return errno_to_win32_code(static_cast<int>(static_cast<unsigned>(code)));
	}
	else
	{
		return 0x00000057;
	}
}

inline constexpr int to_posix_code_impl(std::uintptr_t domain,std::uintptr_t code) noexcept
{
	if(posix_domain_value==domain)
	{
		return win32_code_to_errno(static_cast<std::uint_least32_t>(code));
	}
	else if(win32_domain_value==domain||nt_domain_value==domain)
	{
		std::uint_least32_t win32cd{};
		if(nt_domain_value==domain)
		{
#ifdef _WIN32_WINDOWS
			return 0x00000057;
#else
			win32cd=::fast_io::win32::nt::rtl_nt_status_to_dos_error(static_cast<std::uint_least32_t>(code));
#endif
		}
		else
		{
			win32cd=static_cast<std::uint_least32_t>(code);
		}
		return win32_code_to_errno(win32cd);
	}
	else
	{
		return eother_value;
	}
}

}

inline bool equivalent_define(error_type_t<win32_code>,error e,win32_code we) noexcept
{
	return ::fast_io::details::win32_code_equivalent_impl(e.domain,e.code,we.ec);
}

inline bool equivalent_define(error_type_t<nt_code>,error e,nt_code we) noexcept
{
	return ::fast_io::details::nt_code_equivalent_impl(e.domain,e.code,we.ntstatus);
}

inline bool equivalent_define(error_type_t<::fast_io::freestanding::errc>,error e,::fast_io::freestanding::errc we) noexcept
{
	return ::fast_io::details::posix_code_equivalent_impl(e.domain,e.code,static_cast<int>(we));
}

inline ::fast_io::freestanding::errc to_code_define(error_type_t<::fast_io::freestanding::errc>,error e) noexcept
{
	return ::fast_io::freestanding::errc{::fast_io::details::to_posix_code_impl(e.domain,e.code)};
}

inline win32_code to_code_define(error_type_t<win32_code>,error e) noexcept
{
	return win32_code{::fast_io::details::to_win32_code_impl(e.domain,e.code)};
}

using native_code = win32_code;

inline native_code print_alias_define(::fast_io::io_alias_t,::fast_io::error e) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
	return to_code_define(error_type_t<native_code>{},e);
#else
	return to_code_define(error_type<native_code>,e);
#endif
}

}
