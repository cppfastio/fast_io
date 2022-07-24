#pragma once

namespace fast_io
{

struct nt_code
{
	std::uint_least32_t ntstatus{};
};

inline constexpr std::uintptr_t domain_define(error_type_t<nt_code>) noexcept
{
	if constexpr(sizeof(std::uintptr_t)<=sizeof(std::uint_least16_t))
		return 63612u;
	else if constexpr(sizeof(std::uintptr_t)<=sizeof(std::uint_least32_t))
		return 3776340491u;
	else
		return 17311375720795711021ULL;
}

inline constexpr std::uintptr_t nt_domain_value{domain_define(error_type<nt_code>)};

[[noreturn]] inline void throw_nt_error([[maybe_unused]] std::uint_least32_t err)
{
#ifdef __cpp_exceptions
#if defined(_MSC_VER) && (!defined(_HAS_EXCEPTIONS) || _HAS_EXCEPTIONS == 0)
	fast_terminate();
#else
	throw ::fast_io::error{nt_domain_value,static_cast<std::uintptr_t>(err)};
#endif
#else
	fast_terminate();
#endif
}

namespace details
{
inline constexpr ::fast_io::manipulators::scalar_flags nt_errorflags{.base=16,.full=true,.floating=::fast_io::manipulators::floating_format::fixed};

template<bool enable=true,::std::integral char_type>
requires (enable)
inline constexpr char_type* print_reserve_nt_error_impl(char_type* iter,std::uint_least32_t ntstatus) noexcept
{
	if constexpr(std::same_as<char_type,char>)
		iter=copy_string_literal("[nt:0x",iter);
	else if constexpr(std::same_as<char_type,wchar_t>)
		iter=copy_string_literal(L"[nt:0x",iter);
	else if constexpr(std::same_as<char_type,char16_t>)
		iter=copy_string_literal(u"[nt:0x",iter);
	else if constexpr(std::same_as<char_type,char32_t>)
		iter=copy_string_literal(U"[nt:0x",iter);
	else
		iter=copy_string_literal(u8"[nt:0x",iter);
	using namespace ::fast_io::manipulators;
	iter=print_reserve_define(io_reserve_type<char_type,scalar_manip_t<::fast_io::details::nt_errorflags,std::uint_least32_t>>,iter,{ntstatus});
	*iter=char_literal_v<u8']',char_type>;
	++iter;
	return print_reserve_define_win32_error_impl<win32_family::wide_nt>(iter,win32::nt::rtl_nt_status_to_dos_error(ntstatus));
}

}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,nt_code>) noexcept
{
	using namespace ::fast_io::manipulators;
	constexpr std::size_t full_size(print_reserve_size(io_reserve_type<char_type,scalar_manip_t<::fast_io::details::nt_errorflags,std::uint_least32_t>>)+print_reserve_size(io_reserve_type<char_type,nt_code>));
	if constexpr(std::same_as<char_type,char>)
	{
		return full_size+::fast_io::details::string_literal_size("[nt:0x]");
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		return full_size+::fast_io::details::string_literal_size(L"[nt:0x]");
	}
	else
	{
		return full_size+::fast_io::details::string_literal_size(u8"[nt:0x]");
	}
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,nt_code>, char_type* iter,nt_code e) noexcept
{
	return ::fast_io::details::print_reserve_nt_error_impl(iter,e.ntstatus);
}

}
