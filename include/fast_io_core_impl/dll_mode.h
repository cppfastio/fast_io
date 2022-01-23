#pragma once

namespace fast_io
{

enum class dll_mode:std::uint_least32_t
{
none=0,
posix_rtld_lazy=static_cast<std::uint_least32_t>(1)<<0,
posix_rtld_now=static_cast<std::uint_least32_t>(1)<<1,
posix_rtld_binding_mask=static_cast<std::uint_least32_t>(1)<<2,
posix_rtld_global=static_cast<std::uint_least32_t>(1)<<3,
posix_rtld_local=static_cast<std::uint_least32_t>(1)<<4,
posix_rtld_nodelete=static_cast<std::uint_least32_t>(1)<<5,
posix_rtld_noload=static_cast<std::uint_least32_t>(1)<<6,
posix_rtld_deepbind=static_cast<std::uint_least32_t>(1)<<7,
win32_dont_resolve_dll_references=static_cast<std::uint_least32_t>(1)<<10,
win32_load_ignore_code_authz_level=static_cast<std::uint_least32_t>(1)<<11,
win32_load_library_as_datafile=static_cast<std::uint_least32_t>(1)<<12,
win32_load_library_as_datafile_exclusive=static_cast<std::uint_least32_t>(1)<<13,
win32_load_library_as_image_resource=static_cast<std::uint_least32_t>(1)<<14,
win32_load_library_search_application_dir=static_cast<std::uint_least32_t>(1)<<15,
win32_load_library_search_default_dirs=static_cast<std::uint_least32_t>(1)<<16,
win32_load_library_search_dll_load_dir=static_cast<std::uint_least32_t>(1)<<17,
win32_load_library_search_system32=static_cast<std::uint_least32_t>(1)<<18,
win32_load_library_search_user_dirs=static_cast<std::uint_least32_t>(1)<<19,
win32_load_with_altered_search_path=static_cast<std::uint_least32_t>(1)<<20,
win32_load_library_require_signed_target=static_cast<std::uint_least32_t>(1)<<21,
win32_load_library_safe_current_dirs=static_cast<std::uint_least32_t>(1)<<22
};


constexpr dll_mode operator&(dll_mode x, dll_mode y) noexcept
{
using utype = typename std::underlying_type<dll_mode>::type;
return static_cast<dll_mode>(static_cast<utype>(x) & static_cast<utype>(y));
}

constexpr dll_mode operator|(dll_mode x, dll_mode y) noexcept
{
using utype = typename std::underlying_type<dll_mode>::type;
return static_cast<dll_mode>(static_cast<utype>(x) | static_cast<utype>(y));
}

constexpr dll_mode operator^(dll_mode x, dll_mode y) noexcept
{
using utype = typename std::underlying_type<dll_mode>::type;
return static_cast<dll_mode>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

constexpr dll_mode operator~(dll_mode x) noexcept
{
using utype = typename std::underlying_type<dll_mode>::type;
return static_cast<dll_mode>(~static_cast<utype>(x));
}

inline constexpr dll_mode& operator&=(dll_mode& x, dll_mode y) noexcept{return x=x&y;}

inline constexpr dll_mode& operator|=(dll_mode& x, dll_mode y) noexcept{return x=x|y;}

inline constexpr dll_mode& operator^=(dll_mode& x, dll_mode y) noexcept{return x=x^y;}
}