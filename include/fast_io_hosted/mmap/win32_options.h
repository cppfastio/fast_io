#pragma once

namespace fast_io
{

struct win32_mmap_options
{
	::std::uint_least32_t flProtect{};
	::std::uint_least32_t dwDesiredAccess{};
	void *lpFileMappingAttributes{};
	void *lpName{};

	inline explicit constexpr win32_mmap_options() noexcept = default;
	inline constexpr win32_mmap_options(::fast_io::mmap_prot protv, ::fast_io::mmap_flags flagsv) noexcept
	{
		::std::uint_least32_t flprotecttemp{};
		::std::uint_least32_t dwDesiredAccesstemp{};

		int exclusiveflags{static_cast<int>(flagsv & ::fast_io::mmap_flags::map_type)};
		if (exclusiveflags == 3)
		{
			exclusiveflags = 1;
		}

		if (exclusiveflags == 2)
		{
			dwDesiredAccesstemp |= 0x00000001 /*FILE_MAP_COPY*/;
		}

		if ((protv & ::fast_io::mmap_prot::prot_exec) == ::fast_io::mmap_prot::prot_exec)
		{
			if ((protv & ::fast_io::mmap_prot::prot_read) == ::fast_io::mmap_prot::prot_read)
			{
				if ((protv & ::fast_io::mmap_prot::prot_write) == ::fast_io::mmap_prot::prot_write)
				{
					flprotecttemp |= 0x40 /*PAGE_EXECUTE_READWRITE*/;
				}
				else
				{
					flprotecttemp |= 0x20 /*PAGE_EXECUTE_READ*/;
				}
			}
			else if ((protv & ::fast_io::mmap_prot::prot_write) == ::fast_io::mmap_prot::prot_write)
			{
				if (exclusiveflags == 2)
				{
					flprotecttemp |= 0x80 /*PAGE_EXECUTE_WRITECOPY*/;
				}
				else
				{
					flprotecttemp |= 0x40 /*PAGE_EXECUTE_READWRITE*/;
				}
			}
			dwDesiredAccesstemp |= 0x00000020 /*FILE_MAP_EXECUTE*/;
		}
		else
		{
			if ((protv & ::fast_io::mmap_prot::prot_read) == ::fast_io::mmap_prot::prot_read)
			{
				if ((protv & ::fast_io::mmap_prot::prot_write) == ::fast_io::mmap_prot::prot_write)
				{
					flprotecttemp |= 0x4 /*PAGE_READWRITE*/;
				}
				else
				{
					flprotecttemp |= 0x2 /*PAGE_READONLY*/;
				}
			}
			else if ((protv & ::fast_io::mmap_prot::prot_write) == ::fast_io::mmap_prot::prot_write)
			{
				if (exclusiveflags == 2)
				{
					flprotecttemp |= 0x8 /*PAGE_WRITECOPY*/;
				}
				else
				{
					flprotecttemp |= 0x4 /*PAGE_READWRITE*/;
				}
			}
		}

		if ((protv & ::fast_io::mmap_prot::prot_read) == ::fast_io::mmap_prot::prot_read &&
			(protv & ::fast_io::mmap_prot::prot_write) == ::fast_io::mmap_prot::prot_write)
		{
			dwDesiredAccesstemp |= 0x000F001F /*FILE_MAP_ALL_ACCESS*/;
		}
		else
		{
			if ((protv & ::fast_io::mmap_prot::prot_write) == ::fast_io::mmap_prot::prot_write)
			{
				dwDesiredAccesstemp |= 0x00000002 /*FILE_MAP_WRITE*/;
			}
			if ((protv & ::fast_io::mmap_prot::prot_read) == ::fast_io::mmap_prot::prot_read)
			{
				dwDesiredAccesstemp |= 0x00000004 /*FILE_MAP_READ*/;
			}
		}

		if ((flagsv & ::fast_io::mmap_flags::map_hugetlb) == ::fast_io::mmap_flags::map_hugetlb)
		{
			flprotecttemp |= 0x80000000;
			dwDesiredAccesstemp |= 0x20000000;
		}
		this->flProtect = flprotecttemp;
		this->dwDesiredAccess = dwDesiredAccesstemp;
	}
};

} // namespace fast_io
