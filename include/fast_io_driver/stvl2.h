#pragma once
#include"../fast_io_concept.h"
#include"bootloader/stvl2.h"

namespace fast_io
{

namespace details
{

template<std::integral char_type>
inline constexpr std::size_t cal_stvl2_cstr_reserved_size(std::size_t sz) noexcept
{
	constexpr bool ebcdic{::fast_io::details::is_ebcdic<char_type>};
	if constexpr(sizeof(char_type)==1&&!ebcdic)
		return sz;
	else
		return cal_full_reserve_size<1,sizeof(char_type)>(sz);
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_header_tag(char_type* iter,stvl2::stvl2_header_tag tg) noexcept
{
	using enum stvl2::stvl2_header_tag;
	if constexpr(std::same_as<char_type,char>)
	{
		switch(tg)
		{
		case framebuffer_id:
			return copy_string_literal("framebuffer_id",iter);
		case fb_mtrr_id:
			return copy_string_literal("fb_mtrr_id",iter);
		case smp_id:
			return copy_string_literal("smp_id",iter);
		default:
			return copy_string_literal("unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		switch(tg)
		{
		case framebuffer_id:
			return copy_string_literal(L"framebuffer_id",iter);
		case fb_mtrr_id:
			return copy_string_literal(L"fb_mtrr_id",iter);
		case smp_id:
			return copy_string_literal(L"smp_id",iter);
		default:
			return copy_string_literal(L"unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		switch(tg)
		{
		case framebuffer_id:
			return copy_string_literal(u"framebuffer_id",iter);
		case fb_mtrr_id:
			return copy_string_literal(u"fb_mtrr_id",iter);
		case smp_id:
			return copy_string_literal(u"smp_id",iter);
		default:
			return copy_string_literal(u"unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		switch(tg)
		{
		case framebuffer_id:
			return copy_string_literal(U"framebuffer_id",iter);
		case fb_mtrr_id:
			return copy_string_literal(U"fb_mtrr_id",iter);
		case smp_id:
			return copy_string_literal(U"smp_id",iter);
		default:
			return copy_string_literal(U"unknown",iter);
		}
	}
	else
	{
		switch(tg)
		{
		case framebuffer_id:
			return copy_string_literal(u8"framebuffer_id",iter);
		case fb_mtrr_id:
			return copy_string_literal(u8"fb_mtrr_id",iter);
		case smp_id:
			return copy_string_literal(u8"smp_id",iter);
		default:
			return copy_string_literal(u8"unknown",iter);
		}
	}
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_tag(char_type* iter,stvl2::stvl2_tag tg) noexcept
{
	if constexpr(std::same_as<char_type,char>)
	{
		iter=copy_string_literal("identifier: ",iter);
		iter=print_reserve_define_impl_for_stvl2_header_tag(iter,tg.identifier);
		iter=copy_string_literal("\nnext: ",iter);
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		iter=copy_string_literal(L"identifier: ",iter);
		iter=print_reserve_define_impl_for_stvl2_header_tag(iter,tg.identifier);
		iter=copy_string_literal(L"\nnext: ",iter);
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		iter=copy_string_literal(u"identifier: ",iter);
		iter=print_reserve_define_impl_for_stvl2_header_tag(iter,tg.identifier);
		iter=copy_string_literal(u"\nnext: ",iter);
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		iter=copy_string_literal(U"identifier: ",iter);
		iter=print_reserve_define_impl_for_stvl2_header_tag(iter,tg.identifier);
		iter=copy_string_literal(U"\nnext: ",iter);
	}
	else
	{
		iter=copy_string_literal(u8"identifier: ",iter);
		iter=print_reserve_define_impl_for_stvl2_header_tag(iter,tg.identifier);
		iter=copy_string_literal(u8"\nnext: ",iter);
	}
	return print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.next);
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_header(char_type* iter,stvl2::stvl2_header tg) noexcept
{
	if constexpr(std::same_as<char_type,char>)
	{
		iter=copy_string_literal("entry_point: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.entry_point);
		iter=copy_string_literal("\nstack: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.stack);
		iter=copy_string_literal("\nflags: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,tg.flags);
		iter=copy_string_literal("\ntags_root: ",iter);
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		iter=copy_string_literal(L"entry_point: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.entry_point);
		iter=copy_string_literal(L"\nstack: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.stack);
		iter=copy_string_literal(L"\nflags: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,tg.flags);
		iter=copy_string_literal(L"\ntags_root: ",iter);
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		iter=copy_string_literal(u"entry_point: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.entry_point);
		iter=copy_string_literal(u"\nstack: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.stack);
		iter=copy_string_literal(u"\nflags: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,tg.flags);
		iter=copy_string_literal(u"\ntags_root: ",iter);
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		iter=copy_string_literal(U"entry_point: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.entry_point);
		iter=copy_string_literal(U"\nstack: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.stack);
		iter=copy_string_literal(U"\nflags: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,tg.flags);
		iter=copy_string_literal(U"\ntags_root: ",iter);
	}
	else
	{
		iter=copy_string_literal(u8"entry_point: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.entry_point);
		iter=copy_string_literal(u8"\nstack: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.stack);
		iter=copy_string_literal(u8"\nflags: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,tg.flags);
		iter=copy_string_literal(u8"\ntags_root: ",iter);
	}
	return print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.tags_root);
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_header_tag_framebuffer(char_type* iter,stvl2::stvl2_header_tag_framebuffer tg) noexcept
{
	iter=print_reserve_define_impl_for_stvl2_tag(iter,tg);
	if constexpr(std::same_as<char_type,char>)
	{
		iter=copy_string_literal("\nframebuffer_width: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_width);
		iter=copy_string_literal("\nframebuffer_height: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_height);
		iter=copy_string_literal("\nframebuffer_bpp: ",iter);
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		iter=copy_string_literal(L"\nframebuffer_width: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_width);
		iter=copy_string_literal(L"\nframebuffer_height: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_height);
		iter=copy_string_literal(L"\nframebuffer_bpp: ",iter);
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		iter=copy_string_literal(u"\nframebuffer_width: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_width);
		iter=copy_string_literal(u"\nframebuffer_height: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_height);
		iter=copy_string_literal(u"\nframebuffer_bpp: ",iter);
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		iter=copy_string_literal(U"\nframebuffer_width: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_width);
		iter=copy_string_literal(U"\nframebuffer_height: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_height);
		iter=copy_string_literal(U"\nframebuffer_bpp: ",iter);
	}
	else
	{
		iter=copy_string_literal(u8"\nframebuffer_width: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_width);
		iter=copy_string_literal(u8"\nframebuffer_height: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_height);
		iter=copy_string_literal(u8"\nframebuffer_bpp: ",iter);
	}
	return print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_bpp);
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_header_tag_smp(char_type* iter,stvl2::stvl2_header_tag_smp tg) noexcept
{
	iter=print_reserve_define_impl_for_stvl2_tag(iter,tg);
	if constexpr(std::same_as<char_type,char>)
		iter=copy_string_literal("\nflags: ",iter);
	else if constexpr(std::same_as<char_type,wchar_t>)
		iter=copy_string_literal(L"\nflags: ",iter);
	else if constexpr(std::same_as<char_type,char16_t>)
		iter=copy_string_literal(u"\nflags: ",iter);
	else if constexpr(std::same_as<char_type,char32_t>)
		iter=copy_string_literal(U"\nflags: ",iter);
	else
		iter=copy_string_literal(u8"\nflags: ",iter);
	return print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,tg.flags);
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_struct_tag(char_type* iter,stvl2::stvl2_struct_tag tg) noexcept
{
	using enum stvl2::stvl2_struct_tag;
	if constexpr(std::same_as<char_type,char>)
	{
		switch(tg)
		{
		case cmdline_id:return copy_string_literal("cmdline_id",iter);
		case memmap_id:return copy_string_literal("memmap_id",iter);
		case framebuffer_id:return copy_string_literal("framebuffer_id",iter);
		case fb_mtrr_id:return copy_string_literal("fb_mtrr_id",iter);
		case modules_id:return copy_string_literal("modules_id",iter);
		case rsdp_id:return copy_string_literal("rsdp_id",iter);
		case epoch_id:return copy_string_literal("epoch_id",iter);
		case firmware_id:return copy_string_literal("firmware_id",iter);
		case smp_id:return copy_string_literal("smp_id",iter);
		case pxe_server_info:return copy_string_literal("pxe_server_info",iter);
		case mmio32_uart:return copy_string_literal("mmio32_uart",iter);
		case dtb:return copy_string_literal("dtb",iter);
		case vmap:return copy_string_literal("vmap",iter);
		default:return copy_string_literal("unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		switch(tg)
		{
		case cmdline_id:return copy_string_literal(L"cmdline_id",iter);
		case memmap_id:return copy_string_literal(L"memmap_id",iter);
		case framebuffer_id:return copy_string_literal(L"framebuffer_id",iter);
		case fb_mtrr_id:return copy_string_literal(L"fb_mtrr_id",iter);
		case modules_id:return copy_string_literal(L"modules_id",iter);
		case rsdp_id:return copy_string_literal(L"rsdp_id",iter);
		case epoch_id:return copy_string_literal(L"epoch_id",iter);
		case firmware_id:return copy_string_literal(L"firmware_id",iter);
		case smp_id:return copy_string_literal(L"smp_id",iter);
		case pxe_server_info:return copy_string_literal(L"pxe_server_info",iter);
		case mmio32_uart:return copy_string_literal(L"mmio32_uart",iter);
		case dtb:return copy_string_literal(L"dtb",iter);
		case vmap:return copy_string_literal(L"vmap",iter);
		default:return copy_string_literal(L"unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		switch(tg)
		{
		case cmdline_id:return copy_string_literal(u"cmdline_id",iter);
		case memmap_id:return copy_string_literal(u"memmap_id",iter);
		case framebuffer_id:return copy_string_literal(u"framebuffer_id",iter);
		case fb_mtrr_id:return copy_string_literal(u"fb_mtrr_id",iter);
		case modules_id:return copy_string_literal(u"modules_id",iter);
		case rsdp_id:return copy_string_literal(u"rsdp_id",iter);
		case epoch_id:return copy_string_literal(u"epoch_id",iter);
		case firmware_id:return copy_string_literal(u"firmware_id",iter);
		case smp_id:return copy_string_literal(u"smp_id",iter);
		case pxe_server_info:return copy_string_literal(u"pxe_server_info",iter);
		case mmio32_uart:return copy_string_literal(u"mmio32_uart",iter);
		case dtb:return copy_string_literal(u"dtb",iter);
		case vmap:return copy_string_literal(u"vmap",iter);
		default:return copy_string_literal(u"unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		switch(tg)
		{
		case cmdline_id:return copy_string_literal(U"cmdline_id",iter);
		case memmap_id:return copy_string_literal(U"memmap_id",iter);
		case framebuffer_id:return copy_string_literal(U"framebuffer_id",iter);
		case fb_mtrr_id:return copy_string_literal(U"fb_mtrr_id",iter);
		case modules_id:return copy_string_literal(U"modules_id",iter);
		case rsdp_id:return copy_string_literal(U"rsdp_id",iter);
		case epoch_id:return copy_string_literal(U"epoch_id",iter);
		case firmware_id:return copy_string_literal(U"firmware_id",iter);
		case smp_id:return copy_string_literal(U"smp_id",iter);
		case pxe_server_info:return copy_string_literal(U"pxe_server_info",iter);
		case mmio32_uart:return copy_string_literal(U"mmio32_uart",iter);
		case dtb:return copy_string_literal(U"dtb",iter);
		case vmap:return copy_string_literal(U"vmap",iter);
		default:return copy_string_literal(U"unknown",iter);
		}
	}
	else
	{
		switch(tg)
		{
		case cmdline_id:return copy_string_literal(u8"cmdline_id",iter);
		case memmap_id:return copy_string_literal(u8"memmap_id",iter);
		case framebuffer_id:return copy_string_literal(u8"framebuffer_id",iter);
		case fb_mtrr_id:return copy_string_literal(u8"fb_mtrr_id",iter);
		case modules_id:return copy_string_literal(u8"modules_id",iter);
		case rsdp_id:return copy_string_literal(u8"rsdp_id",iter);
		case epoch_id:return copy_string_literal(u8"epoch_id",iter);
		case firmware_id:return copy_string_literal(u8"firmware_id",iter);
		case smp_id:return copy_string_literal(u8"smp_id",iter);
		case pxe_server_info:return copy_string_literal(u8"pxe_server_info",iter);
		case mmio32_uart:return copy_string_literal(u8"mmio32_uart",iter);
		case dtb:return copy_string_literal(u8"dtb",iter);
		case vmap:return copy_string_literal(u8"vmap",iter);
		default:return copy_string_literal(u8"unknown",iter);
		}
	}
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_mmap_type(char_type* iter,stvl2::stvl2_mmap_type tg) noexcept
{
	using enum stvl2::stvl2_mmap_type;
	if constexpr(std::same_as<char_type,char>)
	{
		switch(tg)
		{
		case usable:return copy_string_literal("usable",iter);
		case reserved:return copy_string_literal("reserved",iter);
		case acpi_reclaimable:return copy_string_literal("acpi_reclaimable",iter);
		case acpi_nvs:return copy_string_literal("acpi_nvs",iter);
		case bad_memory:return copy_string_literal("bad_memory",iter);
		case bootloader_reclaimable:return copy_string_literal("bootloader_reclaimable",iter);
		case kernel_and_modules:return copy_string_literal("kernel_and_modules",iter);
		default:return copy_string_literal("unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		switch(tg)
		{
		case usable:return copy_string_literal(L"usable",iter);
		case reserved:return copy_string_literal(L"reserved",iter);
		case acpi_reclaimable:return copy_string_literal(L"acpi_reclaimable",iter);
		case acpi_nvs:return copy_string_literal(L"acpi_nvs",iter);
		case bad_memory:return copy_string_literal(L"bad_memory",iter);
		case bootloader_reclaimable:return copy_string_literal(L"bootloader_reclaimable",iter);
		case kernel_and_modules:return copy_string_literal(L"kernel_and_modules",iter);
		default:return copy_string_literal(L"unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		switch(tg)
		{
		case usable:return copy_string_literal(u"usable",iter);
		case reserved:return copy_string_literal(u"reserved",iter);
		case acpi_reclaimable:return copy_string_literal(u"acpi_reclaimable",iter);
		case acpi_nvs:return copy_string_literal(u"acpi_nvs",iter);
		case bad_memory:return copy_string_literal(u"bad_memory",iter);
		case bootloader_reclaimable:return copy_string_literal(u"bootloader_reclaimable",iter);
		case kernel_and_modules:return copy_string_literal(u"kernel_and_modules",iter);
		default:return copy_string_literal(u"unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		switch(tg)
		{
		case usable:return copy_string_literal(U"usable",iter);
		case reserved:return copy_string_literal(U"reserved",iter);
		case acpi_reclaimable:return copy_string_literal(U"acpi_reclaimable",iter);
		case acpi_nvs:return copy_string_literal(U"acpi_nvs",iter);
		case bad_memory:return copy_string_literal(U"bad_memory",iter);
		case bootloader_reclaimable:return copy_string_literal(U"bootloader_reclaimable",iter);
		case kernel_and_modules:return copy_string_literal(U"kernel_and_modules",iter);
		default:return copy_string_literal(U"unknown",iter);
		}
	}
	else
	{
		switch(tg)
		{
		case usable:return copy_string_literal(u8"usable",iter);
		case reserved:return copy_string_literal(u8"reserved",iter);
		case acpi_reclaimable:return copy_string_literal(u8"acpi_reclaimable",iter);
		case acpi_nvs:return copy_string_literal(u8"acpi_nvs",iter);
		case bad_memory:return copy_string_literal(u8"bad_memory",iter);
		case bootloader_reclaimable:return copy_string_literal(u8"bootloader_reclaimable",iter);
		case kernel_and_modules:return copy_string_literal(u8"kernel_and_modules",iter);
		default:return copy_string_literal(u8"unknown",iter);
		}
	}
}


template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_firmware_flags(char_type* iter,stvl2::stvl2_firmware_flags tg) noexcept
{
	using enum stvl2::stvl2_firmware_flags;\
	if constexpr(std::same_as<char_type,char>)
	{
		switch(tg)
		{
		case bios:return copy_string_literal("bios",iter);
		default:return copy_string_literal("unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		switch(tg)
		{
		case bios:return copy_string_literal(L"bios",iter);
		default:return copy_string_literal(L"unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		switch(tg)
		{
		case bios:return copy_string_literal(u"bios",iter);
		default:return copy_string_literal(u"unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		switch(tg)
		{
		case bios:return copy_string_literal(U"bios",iter);
		default:return copy_string_literal(U"unknown",iter);
		}
	}
	else
	{
		switch(tg)
		{
		case bios:return copy_string_literal(u8"bios",iter);
		default:return copy_string_literal(u8"unknown",iter);
		}
	}
}

inline constexpr std::size_t constexpr_stvl2_u8strlen(char8_t const* strlen) noexcept
{
	if(std::is_constant_evaluated())
	{
		std::size_t len{};
		for(;strlen[len];++len);
		return len;
	}
	else
	{
		return std::strlen(reinterpret_cast<char const*>(strlen));
	}
}

template<::std::integral char_type>
inline constexpr char_type* deal_with_stvl2_cstr(char_type* iter,char8_t const* u8cstr) noexcept
{
	constexpr bool ebcdic{::fast_io::details::is_ebcdic<char_type>};
	if constexpr(sizeof(char_type)==1&&!ebcdic)
		return non_overlapped_copy_n(u8cstr,constexpr_stvl2_u8strlen(u8cstr),iter);
	else
	{
		std::size_t sz{constexpr_stvl2_u8strlen(u8cstr)};
		return codecvt::general_code_cvt_full(u8cstr,u8cstr+sz,iter);
	}
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_struct(char_type* iter,stvl2::stvl2_struct const& st) noexcept
{
	if constexpr(std::same_as<char_type,char>)
	{
		iter=copy_string_literal("bootloader_brand: ",iter);
		iter=deal_with_stvl2_cstr(iter,st.bootloader_brand);
		iter=copy_string_literal("\nbootloader_version: ",iter);
		iter=deal_with_stvl2_cstr(iter,st.bootloader_version);
		iter=copy_string_literal("\ntags_root: ",iter);
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		iter=copy_string_literal(L"bootloader_brand: ",iter);
		iter=deal_with_stvl2_cstr(iter,st.bootloader_brand);
		iter=copy_string_literal(L"\nbootloader_version: ",iter);
		iter=deal_with_stvl2_cstr(iter,st.bootloader_version);
		iter=copy_string_literal(L"\ntags_root: ",iter);
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		iter=copy_string_literal(u"bootloader_brand: ",iter);
		iter=deal_with_stvl2_cstr(iter,st.bootloader_brand);
		iter=copy_string_literal(u"\nbootloader_version: ",iter);
		iter=deal_with_stvl2_cstr(iter,st.bootloader_version);
		iter=copy_string_literal(u"\ntags_root: ",iter);
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		iter=copy_string_literal(U"bootloader_brand: ",iter);
		iter=deal_with_stvl2_cstr(iter,st.bootloader_brand);
		iter=copy_string_literal(U"\nbootloader_version: ",iter);
		iter=deal_with_stvl2_cstr(iter,st.bootloader_version);
		iter=copy_string_literal(U"\ntags_root: ",iter);
	}
	else
	{
		iter=copy_string_literal(u8"bootloader_brand: ",iter);
		iter=deal_with_stvl2_cstr(iter,st.bootloader_brand);
		iter=copy_string_literal(u8"\nbootloader_version: ",iter);
		iter=deal_with_stvl2_cstr(iter,st.bootloader_version);
		iter=copy_string_literal(u8"\ntags_root: ",iter);
	}
	return print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.tags_root);
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_struct_tag_cmdline(char_type* iter,stvl2::stvl2_struct_tag_cmdline st) noexcept
{
	iter=print_reserve_define_impl_for_stvl2_tag(iter,st);
	if constexpr(std::same_as<char_type,char>)
		iter=copy_string_literal("\ncmdline: ",iter);
	else if constexpr(std::same_as<char_type,wchar_t>)
		iter=copy_string_literal(L"\ncmdline: ",iter);
	else if constexpr(std::same_as<char_type,char16_t>)
		iter=copy_string_literal(u"\ncmdline: ",iter);
	else if constexpr(std::same_as<char_type,char32_t>)
		iter=copy_string_literal(U"\ncmdline: ",iter);
	else
		iter=copy_string_literal(u8"\ncmdline: ",iter);
	return print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,st.cmdline);
}

template<::std::integral char_type,typename T>
requires (
	std::same_as<T,stvl2::stvl2_struct_tag_modules>||
	std::same_as<T,stvl2::stvl2_struct_tag_memmap>||
	std::same_as<T,stvl2::stvl2_struct_tag_rsdp>||
	std::same_as<T,stvl2::stvl2_struct_tag_epoch>||
	std::same_as<T,stvl2::stvl2_struct_tag_firmware>||
	std::same_as<T,stvl2::stvl2_struct_tag_pxe_server_info>||
	std::same_as<T,stvl2::stvl2_struct_tag_mmio32_uart>||
	std::same_as<T,stvl2::stvl2_struct_vmap>||
	std::same_as<T,stvl2::stvl2_struct_tag_dtb>)
inline constexpr char_type* print_reserve_define_impl_for_stvl2_struct_tag_single(char_type* iter,T st) noexcept
{
	iter=print_reserve_define_impl_for_stvl2_tag(iter,st);
	if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_modules>)
	{
		if constexpr(std::same_as<char_type,char>)
			iter=copy_string_literal("\nmodule_count: ",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			iter=copy_string_literal(L"\nmodule_count: ",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			iter=copy_string_literal(u"\nmodule_count: ",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			iter=copy_string_literal(U"\nmodule_count: ",iter);
		else
			iter=copy_string_literal(u8"\nmodule_count: ",iter);
		return print_reserve_define(io_reserve_type<char_type,std::size_t>,iter,st.module_count);		
	}
	else if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_memmap>)
	{
		if constexpr(std::same_as<char_type,char>)
			iter=copy_string_literal("\nentries: ",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			iter=copy_string_literal(L"\nentries: ",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			iter=copy_string_literal(u"\nentries: ",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			iter=copy_string_literal(U"\nentries: ",iter);
		else
			iter=copy_string_literal(u8"\nentries: ",iter);
		return print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,st.entries);
	}
	else if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_rsdp>)
	{
		if constexpr(std::same_as<char_type,char>)
			iter=copy_string_literal("\nrdsp: ",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			iter=copy_string_literal(L"\nrdsp: ",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			iter=copy_string_literal(u"\nrdsp: ",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			iter=copy_string_literal(U"\nrdsp: ",iter);
		else
			iter=copy_string_literal(u8"\nrdsp: ",iter);
		return print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,st.rdsp);
	}
	else if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_epoch>)
	{
		if constexpr(std::same_as<char_type,char>)
			iter=copy_string_literal("\nepoch: ",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			iter=copy_string_literal(L"\nepoch: ",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			iter=copy_string_literal(u"\nepoch: ",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			iter=copy_string_literal(U"\nepoch: ",iter);
		else
			iter=copy_string_literal(u8"\nepoch: ",iter);
		return print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,st.epoch);
	}
	else if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_firmware>)
	{
		if constexpr(std::same_as<char_type,char>)
			iter=copy_string_literal("\ntag: ",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			iter=copy_string_literal(L"\ntag: ",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			iter=copy_string_literal(u"\ntag: ",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			iter=copy_string_literal(U"\ntag: ",iter);
		else
			iter=copy_string_literal(u8"\ntag: ",iter);
		return print_reserve_define_impl_for_stvl2_firmware_flags(iter,st.tag);
	}
	else if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_pxe_server_info>)
	{
		if constexpr(std::same_as<char_type,char>)
			iter=copy_string_literal("\nserver_ip: ",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			iter=copy_string_literal(L"\nserver_ip: ",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			iter=copy_string_literal(u"\nserver_ip: ",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			iter=copy_string_literal(U"\nserver_ip: ",iter);
		else
			iter=copy_string_literal(u8"\nserver_ip: ",iter);
		return print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.server_ip);
	}
	else if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_mmio32_uart>||
	std::same_as<T,stvl2::stvl2_struct_vmap>||
	std::same_as<T,stvl2::stvl2_struct_tag_dtb>)
	{
		if constexpr(std::same_as<char_type,char>)
			iter=copy_string_literal("\naddr: ",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			iter=copy_string_literal(L"\naddr: ",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			iter=copy_string_literal(u"\naddr: ",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			iter=copy_string_literal(U"\naddr: ",iter);
		else
			iter=copy_string_literal(u8"\naddr: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.addr);
		if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_dtb>)
		{
		if constexpr(std::same_as<char_type,char>)
			iter=copy_string_literal("\nsize: ",iter);
		else if constexpr(std::same_as<char_type,wchar_t>)
			iter=copy_string_literal(L"\nsize: ",iter);
		else if constexpr(std::same_as<char_type,char16_t>)
			iter=copy_string_literal(u"\nsize: ",iter);
		else if constexpr(std::same_as<char_type,char32_t>)
			iter=copy_string_literal(U"\nsize: ",iter);
		else
			iter=copy_string_literal(u8"\nsize: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::size_t>,iter,st.size);
		}
		return iter;
	}
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_struct_tag_framebuffer(char_type* iter,stvl2::stvl2_struct_tag_framebuffer tg) noexcept
{
	iter=print_reserve_define_impl_for_stvl2_tag(iter,tg);
	if constexpr(std::same_as<char_type,char>)
	{
		iter=copy_string_literal("\nframebuffer_addr: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.framebuffer_addr);
		iter=copy_string_literal("\nframebuffer_width: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_width);
		iter=copy_string_literal("\nframebuffer_height: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_height);
		iter=copy_string_literal("\nframebuffer_pitch: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_pitch);
		iter=copy_string_literal("\nframebuffer_bpp: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_bpp);
		iter=copy_string_literal("\nmemory_model: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.memory_model);
		iter=copy_string_literal("\nred_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.red_mask_size);
		iter=copy_string_literal("\nred_mask_shift: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.red_mask_shift);
		iter=copy_string_literal("\ngreen_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.green_mask_size);
		iter=copy_string_literal("\ngreen_mask_shift: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.green_mask_shift);
		iter=copy_string_literal("\nblue_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.blue_mask_size);
		iter=copy_string_literal("\nblue_mask_shift: ",iter);
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		iter=copy_string_literal(L"\nframebuffer_addr: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.framebuffer_addr);
		iter=copy_string_literal(L"\nframebuffer_width: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_width);
		iter=copy_string_literal(L"\nframebuffer_height: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_height);
		iter=copy_string_literal(L"\nframebuffer_pitch: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_pitch);
		iter=copy_string_literal(L"\nframebuffer_bpp: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_bpp);
		iter=copy_string_literal(L"\nmemory_model: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.memory_model);
		iter=copy_string_literal(L"\nred_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.red_mask_size);
		iter=copy_string_literal(L"\nred_mask_shift: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.red_mask_shift);
		iter=copy_string_literal(L"\ngreen_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.green_mask_size);
		iter=copy_string_literal(L"\ngreen_mask_shift: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.green_mask_shift);
		iter=copy_string_literal(L"\nblue_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.blue_mask_size);
		iter=copy_string_literal(L"\nblue_mask_shift: ",iter);
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		iter=copy_string_literal(u"\nframebuffer_addr: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.framebuffer_addr);
		iter=copy_string_literal(u"\nframebuffer_width: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_width);
		iter=copy_string_literal(u"\nframebuffer_height: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_height);
		iter=copy_string_literal(u"\nframebuffer_pitch: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_pitch);
		iter=copy_string_literal(u"\nframebuffer_bpp: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_bpp);
		iter=copy_string_literal(u"\nmemory_model: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.memory_model);
		iter=copy_string_literal(u"\nred_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.red_mask_size);
		iter=copy_string_literal(u"\nred_mask_shift: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.red_mask_shift);
		iter=copy_string_literal(u"\ngreen_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.green_mask_size);
		iter=copy_string_literal(u"\ngreen_mask_shift: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.green_mask_shift);
		iter=copy_string_literal(u"\nblue_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.blue_mask_size);
		iter=copy_string_literal(u"\nblue_mask_shift: ",iter);
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		iter=copy_string_literal(U"\nframebuffer_addr: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.framebuffer_addr);
		iter=copy_string_literal(U"\nframebuffer_width: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_width);
		iter=copy_string_literal(U"\nframebuffer_height: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_height);
		iter=copy_string_literal(U"\nframebuffer_pitch: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_pitch);
		iter=copy_string_literal(U"\nframebuffer_bpp: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_bpp);
		iter=copy_string_literal(U"\nmemory_model: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.memory_model);
		iter=copy_string_literal(U"\nred_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.red_mask_size);
		iter=copy_string_literal(U"\nred_mask_shift: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.red_mask_shift);
		iter=copy_string_literal(U"\ngreen_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.green_mask_size);
		iter=copy_string_literal(U"\ngreen_mask_shift: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.green_mask_shift);
		iter=copy_string_literal(U"\nblue_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.blue_mask_size);
		iter=copy_string_literal(U"\nblue_mask_shift: ",iter);
	}
	else
	{
		iter=copy_string_literal(u8"\nframebuffer_addr: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,tg.framebuffer_addr);
		iter=copy_string_literal(u8"\nframebuffer_width: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_width);
		iter=copy_string_literal(u8"\nframebuffer_height: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_height);
		iter=copy_string_literal(u8"\nframebuffer_pitch: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_pitch);
		iter=copy_string_literal(u8"\nframebuffer_bpp: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least16_t>,iter,tg.framebuffer_bpp);
		iter=copy_string_literal(u8"\nmemory_model: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.memory_model);
		iter=copy_string_literal(u8"\nred_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.red_mask_size);
		iter=copy_string_literal(u8"\nred_mask_shift: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.red_mask_shift);
		iter=copy_string_literal(u8"\ngreen_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.green_mask_size);
		iter=copy_string_literal(u8"\ngreen_mask_shift: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.green_mask_shift);
		iter=copy_string_literal(u8"\nblue_mask_size: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.blue_mask_size);
		iter=copy_string_literal(u8"\nblue_mask_shift: ",iter);
	}
	return print_reserve_define(io_reserve_type<char_type,std::uint_least8_t>,iter,tg.blue_mask_shift);
}


template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_module(char_type* iter,stvl2::stvl2_module st) noexcept
{
	if constexpr(std::same_as<char_type,char>)
	{
		iter=copy_string_literal("module_begin: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.module_begin);
		iter=copy_string_literal("\nmodule_end: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.module_end);
		iter=copy_string_literal("\nmodule_string: ",iter);
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		iter=copy_string_literal(L"module_begin: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.module_begin);
		iter=copy_string_literal(L"\nmodule_end: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.module_end);
		iter=copy_string_literal(L"\nmodule_string: ",iter);
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		iter=copy_string_literal(u"module_begin: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.module_begin);
		iter=copy_string_literal(u"\nmodule_end: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.module_end);
		iter=copy_string_literal(u"\nmodule_string: ",iter);
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		iter=copy_string_literal(U"module_begin: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.module_begin);
		iter=copy_string_literal(U"\nmodule_end: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.module_end);
		iter=copy_string_literal(U"\nmodule_string: ",iter);
	}
	else
	{
		iter=copy_string_literal(u8"module_begin: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.module_begin);
		iter=copy_string_literal(u8"\nmodule_end: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.module_end);
		iter=copy_string_literal(u8"\nmodule_string: ",iter);
	}
	return deal_with_stvl2_cstr(iter,st.module_string);
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_mmap_entry(char_type* iter,stvl2::stvl2_mmap_entry st) noexcept
{
	if constexpr(std::same_as<char_type,char>)
	{
		iter=copy_string_literal("base: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.base);
		iter=copy_string_literal("\nlength: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::size_t>,iter,st.length);
		iter=copy_string_literal("\ntype: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,stvl2::stvl2_mmap_type>,iter,st.type);
		iter=copy_string_literal("\nunused: ",iter);
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		iter=copy_string_literal(L"base: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.base);
		iter=copy_string_literal(L"\nlength: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::size_t>,iter,st.length);
		iter=copy_string_literal(L"\ntype: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,stvl2::stvl2_mmap_type>,iter,st.type);
		iter=copy_string_literal(L"\nunused: ",iter);
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		iter=copy_string_literal(u"base: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.base);
		iter=copy_string_literal(u"\nlength: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::size_t>,iter,st.length);
		iter=copy_string_literal(u"\ntype: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,stvl2::stvl2_mmap_type>,iter,st.type);
		iter=copy_string_literal(u"\nunused: ",iter);
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		iter=copy_string_literal(U"base: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.base);
		iter=copy_string_literal(U"\nlength: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::size_t>,iter,st.length);
		iter=copy_string_literal(U"\ntype: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,stvl2::stvl2_mmap_type>,iter,st.type);
		iter=copy_string_literal(U"\nunused: ",iter);
	}
	else
	{
		iter=copy_string_literal(u8"base: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.base);
		iter=copy_string_literal(u8"\nlength: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::size_t>,iter,st.length);
		iter=copy_string_literal(u8"\ntype: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,stvl2::stvl2_mmap_type>,iter,st.type);
		iter=copy_string_literal(u8"\nunused: ",iter);
	}
	return print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.unused);
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_smp_info(char_type* iter,stvl2::stvl2_smp_info st) noexcept
{
	if constexpr(std::same_as<char_type,char>)
	{
		iter=copy_string_literal("process_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.process_id);
		iter=copy_string_literal("\nlapic_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.lapic_id);
		iter=copy_string_literal("\ntarget_stack: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.target_stack);
		iter=copy_string_literal("\ngoto_address: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.goto_address);
		iter=copy_string_literal("\nextra_argument: ",iter);
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		iter=copy_string_literal(L"process_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.process_id);
		iter=copy_string_literal(L"\nlapic_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.lapic_id);
		iter=copy_string_literal(L"\ntarget_stack: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.target_stack);
		iter=copy_string_literal(L"\ngoto_address: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.goto_address);
		iter=copy_string_literal(L"\nextra_argument: ",iter);
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		iter=copy_string_literal(u"process_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.process_id);
		iter=copy_string_literal(u"\nlapic_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.lapic_id);
		iter=copy_string_literal(u"\ntarget_stack: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.target_stack);
		iter=copy_string_literal(u"\ngoto_address: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.goto_address);
		iter=copy_string_literal(u"\nextra_argument: ",iter);
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		iter=copy_string_literal(U"process_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.process_id);
		iter=copy_string_literal(U"\nlapic_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.lapic_id);
		iter=copy_string_literal(U"\ntarget_stack: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.target_stack);
		iter=copy_string_literal(U"\ngoto_address: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.goto_address);
		iter=copy_string_literal(U"\nextra_argument: ",iter);
	}
	else
	{
		iter=copy_string_literal(u8"process_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.process_id);
		iter=copy_string_literal(u8"\nlapic_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.lapic_id);
		iter=copy_string_literal(u8"\ntarget_stack: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.target_stack);
		iter=copy_string_literal(u8"\ngoto_address: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.goto_address);
		iter=copy_string_literal(u8"\nextra_argument: ",iter);
	}
	return print_reserve_define(io_reserve_type<char_type,void const*>,iter,st.extra_argument);
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_impl_for_stvl2_struct_tag_smp(char_type* iter,stvl2::stvl2_struct_tag_smp st) noexcept
{
	iter=print_reserve_define_impl_for_stvl2_tag(iter,st);
	if constexpr(std::same_as<char_type,char>)
	{
		iter=copy_string_literal("\nflags: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,st.flags);
		iter=copy_string_literal("\nbsp_lapic_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.bsp_lapic_id);
		iter=copy_string_literal("\nunused: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.unused);
		iter=copy_string_literal("\ncpu_count: ",iter);
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		iter=copy_string_literal(L"\nflags: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,st.flags);
		iter=copy_string_literal(L"\nbsp_lapic_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.bsp_lapic_id);
		iter=copy_string_literal(L"\nunused: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.unused);
		iter=copy_string_literal(L"\ncpu_count: ",iter);
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		iter=copy_string_literal(u"\nflags: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,st.flags);
		iter=copy_string_literal(u"\nbsp_lapic_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.bsp_lapic_id);
		iter=copy_string_literal(u"\nunused: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.unused);
		iter=copy_string_literal(u"\ncpu_count: ",iter);
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		iter=copy_string_literal(U"\nflags: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,st.flags);
		iter=copy_string_literal(U"\nbsp_lapic_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.bsp_lapic_id);
		iter=copy_string_literal(U"\nunused: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.unused);
		iter=copy_string_literal(U"\ncpu_count: ",iter);
	}
	else
	{
		iter=copy_string_literal(u8"\nflags: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,st.flags);
		iter=copy_string_literal(u8"\nbsp_lapic_id: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.bsp_lapic_id);
		iter=copy_string_literal(u8"\nunused: ",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uint_least32_t>,iter,st.unused);
		iter=copy_string_literal(u8"\ncpu_count: ",iter);
	}
	return print_reserve_define(io_reserve_type<char_type,std::uint_least64_t>,iter,st.cpu_count);
}

}

template<std::integral char_type,typename T>
requires (std::same_as<T,stvl2::stvl2_mmap_type>||std::same_as<T,stvl2::stvl2_struct_tag>||
	std::same_as<T,stvl2::stvl2_header_tag>||std::same_as<T,stvl2::stvl2_firmware_flags>)
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,T>) noexcept
{
	return 32;
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,stvl2::stvl2_tag>) noexcept
{
	constexpr std::size_t number_size
	{
		print_reserve_size(io_reserve_type<char_type,stvl2::stvl2_header_tag>)+
		print_reserve_size(io_reserve_type<char_type,void const*>)
	};
	if constexpr(std::same_as<char_type,char>)
		return number_size+details::string_literal_size("identifier: \nnext: ");
	else if constexpr(std::same_as<char_type,wchar_t>)
		return number_size+details::string_literal_size(L"identifier: \nnext: ");
	else
		return number_size+details::string_literal_size(u8"identifier: \nnext: ");
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,stvl2::stvl2_header>) noexcept
{
	constexpr std::size_t tag_size{print_reserve_size(io_reserve_type<char_type,stvl2::stvl2_header_tag>)
	+print_reserve_size(io_reserve_type<char_type,void const*>)*3
	+print_reserve_size(io_reserve_type<char_type,std::uint_least64_t>)};
	if constexpr(std::same_as<char_type,char>)
		return tag_size+details::string_literal_size("entry_point: \nstack: \nflags: \ntags_root: ");
	else if constexpr(std::same_as<char_type,wchar_t>)
		return tag_size+details::string_literal_size(L"entry_point: \nstack: \nflags: \ntags_root: ");
	else
		return tag_size+details::string_literal_size(u8"entry_point: \nstack: \nflags: \ntags_root: ");
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,stvl2::stvl2_header_tag_framebuffer>) noexcept
{
	constexpr std::size_t tag_size
	{
		print_reserve_size(io_reserve_type<char_type,stvl2::stvl2_tag>)+
		print_reserve_size(io_reserve_type<char_type,std::uint_least16_t>)*3
	};
	if constexpr(std::same_as<char_type,char>)
		return tag_size+details::string_literal_size("\nframebuffer_width: \nframebuffer_height: \nframebuffer_bpp: ");
	else if constexpr(std::same_as<char_type,wchar_t>)
		return tag_size+details::string_literal_size(L"\nframebuffer_width: \nframebuffer_height: \nframebuffer_bpp: ");
	else
		return tag_size+details::string_literal_size(u8"\nframebuffer_width: \nframebuffer_height: \nframebuffer_bpp: ");
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,stvl2::stvl2_header_tag_smp>) noexcept
{
	constexpr std::size_t tag_size
	{
		print_reserve_size(io_reserve_type<char_type,stvl2::stvl2_tag>)+
		print_reserve_size(io_reserve_type<char_type,std::uint_least64_t>)
	};
	if constexpr(std::same_as<char_type,char>)
		return tag_size+details::string_literal_size("\nflags: ");
	else if constexpr(std::same_as<char_type,wchar_t>)
		return tag_size+details::string_literal_size(L"\nflags: ");
	else
		return tag_size+details::string_literal_size(u8"\nflags: ");
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,stvl2::stvl2_struct>) noexcept
{
	constexpr std::size_t tag_size{
		print_reserve_size(io_reserve_type<char_type,void const*>)+
		details::cal_stvl2_cstr_reserved_size<char_type>(stvl2::stvl2_struct::bootloader_brand_size)*2
		};
	if constexpr(std::same_as<char_type,char>)
		return tag_size+details::string_literal_size("bootloader_brand: \nbootloader_version: \ntags_root: ");
	else if constexpr(std::same_as<char_type,wchar_t>)
		return tag_size+details::string_literal_size(L"bootloader_brand: \nbootloader_version: \ntags_root: ");
	else if constexpr(std::same_as<char_type,char16_t>)
		return tag_size+details::string_literal_size(u"bootloader_brand: \nbootloader_version: \ntags_root: ");
	else if constexpr(std::same_as<char_type,char32_t>)
		return tag_size+details::string_literal_size(U"bootloader_brand: \nbootloader_version: \ntags_root: ");
	else
		return tag_size+details::string_literal_size(u8"bootloader_brand: \nbootloader_version: \ntags_root: ");
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,stvl2::stvl2_struct_tag_cmdline>) noexcept
{
	constexpr std::size_t tag_size{print_reserve_size(io_reserve_type<char_type,stvl2::stvl2_tag>)
	+print_reserve_size(io_reserve_type<char_type,std::uint_least64_t>)};
	if constexpr(std::same_as<char_type,char>)
		return tag_size+details::string_literal_size("\ncmdline: ");
	else if constexpr(std::same_as<char_type,wchar_t>)
		return tag_size+details::string_literal_size(L"\ncmdline: ");
	else if constexpr(std::same_as<char_type,char16_t>)
		return tag_size+details::string_literal_size(u"\ncmdline: ");
	else if constexpr(std::same_as<char_type,char32_t>)
		return tag_size+details::string_literal_size(U"\ncmdline: ");
	else
		return tag_size+details::string_literal_size(u8"\ncmdline: ");
}

template<std::integral char_type,typename T>
requires (std::same_as<T,stvl2::stvl2_struct_tag_modules>||
	std::same_as<T,stvl2::stvl2_struct_tag_memmap>||
	std::same_as<T,stvl2::stvl2_struct_tag_rsdp>||
	std::same_as<T,stvl2::stvl2_struct_tag_epoch>||
	std::same_as<T,stvl2::stvl2_struct_tag_firmware>||
	std::same_as<T,stvl2::stvl2_struct_tag_pxe_server_info>||
	std::same_as<T,stvl2::stvl2_struct_tag_mmio32_uart>||
	std::same_as<T,stvl2::stvl2_struct_vmap>||
	std::same_as<T,stvl2::stvl2_struct_tag_dtb>)
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,T>) noexcept
{
	constexpr std::size_t tagbase_size{print_reserve_size(io_reserve_type<char_type,stvl2::stvl2_tag>)};
	if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_modules>)
	{
		constexpr std::size_t tag_size{tagbase_size+print_reserve_size(io_reserve_type<char_type,std::size_t>)};
		if constexpr(std::same_as<char_type,char>)
			return tag_size+details::string_literal_size("\nmodule_count: ");
		else if constexpr(std::same_as<char_type,wchar_t>)
			return tag_size+details::string_literal_size(L"\nmodule_count: ");
		else if constexpr(std::same_as<char_type,char16_t>)
			return tag_size+details::string_literal_size(u"\nmodule_count: ");
		else if constexpr(std::same_as<char_type,char32_t>)
			return tag_size+details::string_literal_size(U"\nmodule_count: ");
		else
			return tag_size+details::string_literal_size(u8"\nmodule_count: ");
	}
	else if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_memmap>||
	std::same_as<T,stvl2::stvl2_struct_tag_rsdp>||
	std::same_as<T,stvl2::stvl2_struct_tag_epoch>
	)
	{
		constexpr std::size_t tag_size{tagbase_size+print_reserve_size(io_reserve_type<char_type,std::uint_least64_t>)};
		if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_memmap>)
		{
		if constexpr(std::same_as<char_type,char>)
			return tag_size+details::string_literal_size("\nentries: ");
		else if constexpr(std::same_as<char_type,wchar_t>)
			return tag_size+details::string_literal_size(L"\nentries: ");
		else if constexpr(std::same_as<char_type,char16_t>)
			return tag_size+details::string_literal_size(u"\nentries: ");
		else if constexpr(std::same_as<char_type,char32_t>)
			return tag_size+details::string_literal_size(U"\nentries: ");
		else
			return tag_size+details::string_literal_size(u8"\nentries: ");
		}
		else if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_rsdp>)
		{
		if constexpr(std::same_as<char_type,char>)
			return tag_size+details::string_literal_size("\nrdsp: ");
		else if constexpr(std::same_as<char_type,wchar_t>)
			return tag_size+details::string_literal_size(L"\nrdsp: ");
		else if constexpr(std::same_as<char_type,char16_t>)
			return tag_size+details::string_literal_size(u"\nrdsp: ");
		else if constexpr(std::same_as<char_type,char32_t>)
			return tag_size+details::string_literal_size(U"\nrdsp: ");
		else
			return tag_size+details::string_literal_size(u8"\nrdsp: ");
		}
		else if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_epoch>)
		{
		if constexpr(std::same_as<char_type,char>)
			return tag_size+details::string_literal_size("\nepoch: ");
		else if constexpr(std::same_as<char_type,wchar_t>)
			return tag_size+details::string_literal_size(L"\nepoch: ");
		else if constexpr(std::same_as<char_type,char16_t>)
			return tag_size+details::string_literal_size(u"\nepoch: ");
		else if constexpr(std::same_as<char_type,char32_t>)
			return tag_size+details::string_literal_size(U"\nepoch: ");
		else
			return tag_size+details::string_literal_size(u8"\nepoch: ");
		}
	}
	else if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_firmware>)
	{
		constexpr std::size_t tag_size{tagbase_size+print_reserve_size(io_reserve_type<char_type,stvl2::stvl2_firmware_flags>)};
		if constexpr(std::same_as<char_type,char>)
			return tag_size+details::string_literal_size("\ntag: ");
		else if constexpr(std::same_as<char_type,wchar_t>)
			return tag_size+details::string_literal_size(L"\ntag: ");
		else if constexpr(std::same_as<char_type,char16_t>)
			return tag_size+details::string_literal_size(u"\ntag: ");
		else if constexpr(std::same_as<char_type,char32_t>)
			return tag_size+details::string_literal_size(U"\ntag: ");
		else
			return tag_size+details::string_literal_size(u8"\ntag: ");
	}
	else if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_pxe_server_info>)
	{
		constexpr std::size_t tag_size{tagbase_size+print_reserve_size(io_reserve_type<char_type,std::uint_least32_t>)};
		if constexpr(std::same_as<char_type,char>)
			return tag_size+details::string_literal_size("\nserver_ip: ");
		else if constexpr(std::same_as<char_type,wchar_t>)
			return tag_size+details::string_literal_size(L"\nserver_ip: ");
		else if constexpr(std::same_as<char_type,char16_t>)
			return tag_size+details::string_literal_size(u"\nserver_ip: ");
		else if constexpr(std::same_as<char_type,char32_t>)
			return tag_size+details::string_literal_size(U"\nserver_ip: ");
		else
			return tag_size+details::string_literal_size(u8"\nserver_ip: ");
	}
	else if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_mmio32_uart>||
	std::same_as<T,stvl2::stvl2_struct_vmap>)
	{
		constexpr std::size_t tag_size{tagbase_size+print_reserve_size(io_reserve_type<char_type,void const*>)};
		if constexpr(std::same_as<char_type,char>)
			return tag_size+details::string_literal_size("\naddr: ");
		else if constexpr(std::same_as<char_type,wchar_t>)
			return tag_size+details::string_literal_size(L"\naddr: ");
		else if constexpr(std::same_as<char_type,char16_t>)
			return tag_size+details::string_literal_size(u"\naddr: ");
		else if constexpr(std::same_as<char_type,char32_t>)
			return tag_size+details::string_literal_size(U"\naddr: ");
		else
			return tag_size+details::string_literal_size(u8"\naddr: ");
	}
	else if constexpr(std::same_as<T,stvl2::stvl2_struct_tag_dtb>)
	{
		constexpr std::size_t tag_size{tagbase_size+print_reserve_size(io_reserve_type<char_type,std::size_t>)};
		if constexpr(std::same_as<char_type,char>)
			return tag_size+details::string_literal_size("\naddr: \nsize: ");
		else if constexpr(std::same_as<char_type,wchar_t>)
			return tag_size+details::string_literal_size(L"\naddr: \nsize: ");
		else if constexpr(std::same_as<char_type,char16_t>)
			return tag_size+details::string_literal_size(u"\naddr: \nsize: ");
		else if constexpr(std::same_as<char_type,char32_t>)
			return tag_size+details::string_literal_size(U"\naddr: \nsize: ");
		else
			return tag_size+details::string_literal_size(u8"\naddr: \nsize: ");
	}
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,stvl2::stvl2_struct_tag_framebuffer>) noexcept
{
	constexpr std::size_t tag_size{
	print_reserve_size(io_reserve_type<char_type,stvl2::stvl2_tag>)
	+print_reserve_size(io_reserve_type<char_type,std::uint_least8_t>)*7+
	+print_reserve_size(io_reserve_type<char_type,std::uint_least16_t>)*4
	+print_reserve_size(io_reserve_type<char_type,void const*>)};
	if constexpr(std::same_as<char_type,char>)
		return tag_size+details::string_literal_size("\nframebuffer_addr: \nframebuffer_width: \nframebuffer_height: \nframebuffer_pitch: \nframebuffer_bpp: \nmemory_model: \nred_mask_size: \nred_mask_shift: \ngreen_mask_size: \ngreen_mask_shift: \nblue_mask_size: \nblue_mask_shift: ");
	else if constexpr(std::same_as<char_type,wchar_t>)
		return tag_size+details::string_literal_size(L"\nframebuffer_addr: \nframebuffer_width: \nframebuffer_height: \nframebuffer_pitch: \nframebuffer_bpp: \nmemory_model: \nred_mask_size: \nred_mask_shift: \ngreen_mask_size: \ngreen_mask_shift: \nblue_mask_size: \nblue_mask_shift: ");
	else if constexpr(std::same_as<char_type,char16_t>)
		return tag_size+details::string_literal_size(u"\nframebuffer_addr: \nframebuffer_width: \nframebuffer_height: \nframebuffer_pitch: \nframebuffer_bpp: \nmemory_model: \nred_mask_size: \nred_mask_shift: \ngreen_mask_size: \ngreen_mask_shift: \nblue_mask_size: \nblue_mask_shift: ");
	else if constexpr(std::same_as<char_type,char32_t>)
		return tag_size+details::string_literal_size(U"\nframebuffer_addr: \nframebuffer_width: \nframebuffer_height: \nframebuffer_pitch: \nframebuffer_bpp: \nmemory_model: \nred_mask_size: \nred_mask_shift: \ngreen_mask_size: \ngreen_mask_shift: \nblue_mask_size: \nblue_mask_shift: ");
	else
		return tag_size+details::string_literal_size(u8"\nframebuffer_addr: \nframebuffer_width: \nframebuffer_height: \nframebuffer_pitch: \nframebuffer_bpp: \nmemory_model: \nred_mask_size: \nred_mask_shift: \ngreen_mask_size: \ngreen_mask_shift: \nblue_mask_size: \nblue_mask_shift: ");
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,stvl2::stvl2_module>) noexcept
{
	constexpr std::size_t tag_size{
	details::cal_stvl2_cstr_reserved_size<char_type>(stvl2::stvl2_module::module_string_size)
	+print_reserve_size(io_reserve_type<char_type,void const*>)*2};
	if constexpr(std::same_as<char_type,char>)
		return tag_size+details::string_literal_size("module_begin: \nmodule_end: \nmodule_string: ");
	else if constexpr(std::same_as<char_type,wchar_t>)
		return tag_size+details::string_literal_size(L"module_begin: \nmodule_end: \nmodule_string: ");
	else if constexpr(std::same_as<char_type,char16_t>)
		return tag_size+details::string_literal_size(u"module_begin: \nmodule_end: \nmodule_string: ");
	else if constexpr(std::same_as<char_type,char32_t>)
		return tag_size+details::string_literal_size(U"module_begin: \nmodule_end: \nmodule_string: ");
	else
		return tag_size+details::string_literal_size(u8"module_begin: \nmodule_end: \nmodule_string: ");
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,stvl2::stvl2_mmap_entry>) noexcept
{
	constexpr std::size_t tag_size{
	print_reserve_size(io_reserve_type<char_type,std::size_t>)
	+print_reserve_size(io_reserve_type<char_type,void const*>)
	+print_reserve_size(io_reserve_type<char_type,stvl2::stvl2_mmap_type>)
	+print_reserve_size(io_reserve_type<char_type,std::uint_least32_t>)};
	if constexpr(std::same_as<char_type,char>)
		return tag_size+details::string_literal_size("base: \nlength: \ntype: \nunused: ");
	else if constexpr(std::same_as<char_type,wchar_t>)
		return tag_size+details::string_literal_size(L"base: \nlength: \ntype: \nunused: ");
	else if constexpr(std::same_as<char_type,char16_t>)
		return tag_size+details::string_literal_size(u"base: \nlength: \ntype: \nunused: ");
	else if constexpr(std::same_as<char_type,char32_t>)
		return tag_size+details::string_literal_size(U"base: \nlength: \ntype: \nunused: ");
	else
		return tag_size+details::string_literal_size(u8"base: \nlength: \ntype: \nunused: ");
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,stvl2::stvl2_smp_info>) noexcept
{
	constexpr std::size_t tag_size{
	print_reserve_size(io_reserve_type<char_type,void const*>)*3
	+print_reserve_size(io_reserve_type<char_type,std::uint_least32_t>)*2};
	if constexpr(std::same_as<char_type,char>)
		return tag_size+details::string_literal_size("process_id: \nlapic_id: \ntarget_stack: \ngoto_address: \nextra_argument: ");
	else if constexpr(std::same_as<char_type,wchar_t>)
		return tag_size+details::string_literal_size(L"process_id: \nlapic_id: \ntarget_stack: \ngoto_address: \nextra_argument: ");
	else if constexpr(std::same_as<char_type,char16_t>)
		return tag_size+details::string_literal_size(u"process_id: \nlapic_id: \ntarget_stack: \ngoto_address: \nextra_argument: ");
	else if constexpr(std::same_as<char_type,char32_t>)
		return tag_size+details::string_literal_size(U"process_id: \nlapic_id: \ntarget_stack: \ngoto_address: \nextra_argument: ");
	else
		return tag_size+details::string_literal_size(u8"process_id: \nlapic_id: \ntarget_stack: \ngoto_address: \nextra_argument: ");
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,stvl2::stvl2_struct_tag_smp>) noexcept
{
	constexpr std::size_t tag_size{
	print_reserve_size(io_reserve_type<char_type,stvl2::stvl2_tag>)
	+print_reserve_size(io_reserve_type<char_type,std::uint_least32_t>)*2
	+print_reserve_size(io_reserve_type<char_type,std::uint_least64_t>)*2};
	if constexpr(std::same_as<char_type,char>)
		return tag_size+details::string_literal_size("\nflags: \nbsp_lapic_id: \nunused: \ncpu_count: ");
	else if constexpr(std::same_as<char_type,wchar_t>)
		return tag_size+details::string_literal_size(L"\nflags: \nbsp_lapic_id: \nunused: \ncpu_count: ");
	else if constexpr(std::same_as<char_type,char16_t>)
		return tag_size+details::string_literal_size(u"\nflags: \nbsp_lapic_id: \nunused: \ncpu_count: ");
	else if constexpr(std::same_as<char_type,char32_t>)
		return tag_size+details::string_literal_size(U"\nflags: \nbsp_lapic_id: \nunused: \ncpu_count: ");
	else
		return tag_size+details::string_literal_size(u8"\nflags: \nbsp_lapic_id: \nunused: \ncpu_count: ");
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_tag>,char_type* iter,stvl2::stvl2_tag tg) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_tag(iter,tg);
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_header_tag>,char_type* iter,stvl2::stvl2_header_tag hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_header_tag(iter,hd);
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_header>,char_type* iter,stvl2::stvl2_header hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_header(iter,hd);
}

static_assert(reserve_printable<char8_t,stvl2::stvl2_header>);

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_header_tag_framebuffer>,char_type* iter,stvl2::stvl2_header_tag_framebuffer hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_header_tag_framebuffer(iter,hd);
}
static_assert(reserve_printable<char8_t,stvl2::stvl2_header_tag_framebuffer>);

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_header_tag_smp>,char_type* iter,stvl2::stvl2_header_tag_smp hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_header_tag_smp(iter,hd);
}
static_assert(reserve_printable<char8_t,stvl2::stvl2_header_tag_smp>);

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_struct_tag>,char_type* iter,stvl2::stvl2_struct_tag hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_struct_tag(iter,hd);
}
static_assert(reserve_printable<char8_t,stvl2::stvl2_struct_tag>);

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_struct>,char_type* iter,stvl2::stvl2_struct const& hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_struct(iter,hd);
}
static_assert(reserve_printable<char8_t,stvl2::stvl2_struct>);

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_struct_tag_cmdline>,char_type* iter,stvl2::stvl2_struct_tag_cmdline hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_struct_tag_cmdline(iter,hd);
}
static_assert(reserve_printable<char8_t,stvl2::stvl2_struct_tag_cmdline>);

template<::std::integral char_type,typename T>
requires (std::same_as<T,stvl2::stvl2_struct_tag_modules>||
	std::same_as<T,stvl2::stvl2_struct_tag_memmap>||
	std::same_as<T,stvl2::stvl2_struct_tag_rsdp>||
	std::same_as<T,stvl2::stvl2_struct_tag_epoch>||
	std::same_as<T,stvl2::stvl2_struct_tag_firmware>||
	std::same_as<T,stvl2::stvl2_struct_tag_pxe_server_info>||
	std::same_as<T,stvl2::stvl2_struct_tag_mmio32_uart>||
	std::same_as<T,stvl2::stvl2_struct_vmap>||
	std::same_as<T,stvl2::stvl2_struct_tag_dtb>)
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,T>,char_type* iter,T hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_struct_tag_single(iter,hd);
}
static_assert(reserve_printable<char8_t,stvl2::stvl2_struct_tag_memmap>);

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_struct_tag_framebuffer>,char_type* iter,stvl2::stvl2_struct_tag_framebuffer hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_struct_tag_framebuffer(iter,hd);
}
static_assert(reserve_printable<char8_t,stvl2::stvl2_struct_tag_framebuffer>);

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_mmap_type>,char_type* iter,stvl2::stvl2_mmap_type hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_mmap_type(iter,hd);
}
static_assert(reserve_printable<char8_t,stvl2::stvl2_mmap_type>);

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_firmware_flags>,char_type* iter,stvl2::stvl2_firmware_flags hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_firmware_flags(iter,hd);
}
static_assert(reserve_printable<char8_t,stvl2::stvl2_firmware_flags>);

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_module>,char_type* iter,stvl2::stvl2_module hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_module(iter,hd);
}
static_assert(reserve_printable<char8_t,stvl2::stvl2_module>);
static_assert(reserve_printable<char8_t,stvl2::stvl2_struct_tag_dtb>);

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_mmap_entry>,char_type* iter,stvl2::stvl2_mmap_entry hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_mmap_entry(iter,hd);
}
static_assert(reserve_printable<char8_t,stvl2::stvl2_mmap_entry>);

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_smp_info>,Iter iter,stvl2::stvl2_smp_info hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_smp_info(iter,hd);
}
static_assert(reserve_printable<char8_t,stvl2::stvl2_smp_info>);

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,stvl2::stvl2_struct_tag_smp>,char_type* iter,stvl2::stvl2_struct_tag_smp hd) noexcept
{
	return details::print_reserve_define_impl_for_stvl2_struct_tag_smp(iter,hd);
}
static_assert(reserve_printable<char8_t,stvl2::stvl2_struct_tag_smp>);
}
