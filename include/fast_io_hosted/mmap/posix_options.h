#pragma once

namespace fast_io
{

struct posix_mmap_options
{
	void *addr{};
	int prot{};
	int flags{};
	inline explicit constexpr posix_mmap_options() noexcept = default;
	inline constexpr posix_mmap_options(int protv, int flagsv) noexcept
		: prot(protv), flags(flagsv)
	{}
#ifdef __linux__
	inline constexpr posix_mmap_options(::fast_io::mmap_prot protv, ::fast_io::mmap_flags flagsv) noexcept
		: prot(static_cast<int>(protv)), flags(static_cast<int>(flagsv))
	{
#ifdef MAP_UNINITIALIZED
		if constexpr (MAP_UNINITIALIZED == 0)
		{
			if ((flagsv & ::fast_io::mmap_flags::map_uninitialized) == ::fast_io::mmap_flags::map_uninitialized)
			{
				// 0x4000000u on pdp11 is ul, on pdp11 mask is all poped
				constexpr unsigned mask{~static_cast<unsigned>(0x4000000u)};
				flags = staic_cast<int>(staic_cast<unsigned>(flags) & mask);
			}
		}
#endif
	}
#else
	inline constexpr posix_mmap_options(::fast_io::mmap_prot protv, ::fast_io::mmap_flags flagsv) noexcept
	{
		int prottemp{};
#ifdef PROT_READ
		if ((protv & ::fast_io::mmap_prot::prot_read) == ::fast_io::mmap_prot::prot_read)
		{
			prottemp |= PROT_READ;
		}
#endif
#ifdef PROT_WRITE
		if ((protv & ::fast_io::mmap_prot::prot_write) == ::fast_io::mmap_prot::prot_write)
		{
			prottemp |= PROT_WRITE;
		}
#endif
#ifdef PROT_EXEC
		if ((protv & ::fast_io::mmap_prot::prot_exec) == ::fast_io::mmap_prot::prot_exec)
		{
			prottemp |= PROT_EXEC;
		}
#endif
#ifdef PROT_MTE
		if ((protv & ::fast_io::mmap_prot::prot_mte) == ::fast_io::mmap_prot::prot_mte)
		{
			prottemp |= PROT_MTE;
		}
#endif
#ifdef PROT_SEM
		if ((protv & ::fast_io::mmap_prot::prot_sem) == ::fast_io::mmap_prot::prot_sem)
		{
			prottemp |= PROT_SEM;
		}
#endif
#ifdef PROT_GROWSDOWN
		if ((protv & ::fast_io::mmap_prot::prot_growsdown) == ::fast_io::mmap_prot::prot_growsdown)
		{
			prottemp |= PROT_GROWSDOWN;
		}
#endif

		int flagstemp{};

#ifdef MAP_FIXED
		if ((flagsv & ::fast_io::mmap_flags::map_fixed) == ::fast_io::mmap_flags::map_fixed)
		{
			flagstemp |= MAP_FIXED;
		}
#endif
#if defined(MAP_ANONYMOUS) || defined(MAP_ANON)
		if ((flagsv & ::fast_io::mmap_flags::map_anonymous) == ::fast_io::mmap_flags::map_anonymous)
		{
#ifdef MAP_ANONYMOUS
			flagstemp |= MAP_ANONYMOUS;
#else
			flagstemp |= MAP_ANON;
#endif
		}
#endif
#ifdef MAP_32BIT
		if ((flagsv & ::fast_io::mmap_flags::map_32bit) == ::fast_io::mmap_flags::map_32bit)
		{
			flagstemp |= MAP_32BIT;
		}
#endif
#ifdef MAP_ABOVE4G
		if ((flagsv & ::fast_io::mmap_flags::map_above4g) == ::fast_io::mmap_flags::map_above4g)
		{
			flagstemp |= MAP_ABOVE4G;
		}
#endif

#ifdef MAP_HASSEMAPHORE
		if ((flagsv & ::fast_io::mmap_flags::map_hassemaphore) == ::fast_io::mmap_flags::map_hassemaphore)
		{
			flagstemp |= MAP_HASSEMAPHORE;
		}
#endif

#ifdef MAP_INHERIT
		if ((flagsv & ::fast_io::mmap_flags::map_inherit) == ::fast_io::mmap_flags::map_inherit)
		{
			flagstemp |= MAP_INHERIT;
		}
#endif

#ifdef MAP_UNALIGNED
		if ((flagsv & ::fast_io::mmap_flags::map_unaligned) == ::fast_io::mmap_flags::map_unaligned)
		{
			flagstemp |= MAP_UNALIGNED;
		}
#endif

#ifdef MAP_GROWSDOWN
		if ((flagsv & ::fast_io::mmap_flags::map_growsdown) == ::fast_io::mmap_flags::map_growsdown)
		{
			flagstemp |= MAP_GROWSDOWN;
		}
#endif

#ifdef MAP_DENYWRITE
		if ((flagsv & ::fast_io::mmap_flags::map_denywrite) == ::fast_io::mmap_flags::map_denywrite)
		{
			flagstemp |= MAP_DENYWRITE;
		}
#endif

#ifdef MAP_EXECUTABLE
		if ((flagsv & ::fast_io::mmap_flags::map_executable) == ::fast_io::mmap_flags::map_executable)
		{
			flagstemp |= MAP_EXECUTABLE;
		}
#endif

#ifdef MAP_LOCKED
		if ((flagsv & ::fast_io::mmap_flags::map_locked) == ::fast_io::mmap_flags::map_locked)
		{
			flagstemp |= MAP_LOCKED;
		}
#endif

#ifdef MAP_NORESERVE
		if ((flagsv & ::fast_io::mmap_flags::map_noreserve) == ::fast_io::mmap_flags::map_noreserve)
		{
			flagstemp |= MAP_NORESERVE;
		}
#endif

#ifdef MAP_POPULATE
		if ((flagsv & ::fast_io::mmap_flags::map_populate) == ::fast_io::mmap_flags::map_populate)
		{
			flagstemp |= MAP_POPULATE;
		}
#endif

#ifdef MAP_NONBLOCK
		if ((flagsv & ::fast_io::mmap_flags::map_nonblock) == ::fast_io::mmap_flags::map_nonblock)
		{
			flagstemp |= MAP_NONBLOCK;
		}
#endif

#ifdef MAP_STACK
		if ((flagsv & ::fast_io::mmap_flags::map_stack) == ::fast_io::mmap_flags::map_stack)
		{
			flagstemp |= MAP_STACK;
		}
#endif

#ifdef MAP_HUGETLB
		if ((flagsv & ::fast_io::mmap_flags::map_hugetlb) == ::fast_io::mmap_flags::map_hugetlb)
		{
			flagstemp |= MAP_HUGETLB;
		}
#endif

#ifdef MAP_FIXED_NOREPLACE
		if ((flagsv & ::fast_io::mmap_flags::map_fixed_noreplace) == ::fast_io::mmap_flags::map_fixed_noreplace)
		{
			flagstemp |= MAP_FIXED_NOREPLACE;
		}
#endif

#ifdef MAP_HUGE_2MB
		if ((flagsv & ::fast_io::mmap_flags::map_huge_2mb) == ::fast_io::mmap_flags::map_huge_2mb)
		{
			flagstemp |= MAP_HUGE_2MB;
		}
#endif

#ifdef MAP_HUGE_1GB
		if ((flagsv & ::fast_io::mmap_flags::map_huge_1gb) == ::fast_io::mmap_flags::map_huge_1gb)
		{
			flagstemp |= MAP_HUGE_1GB;
		}
#endif
		auto exclusiveflags{static_cast<int>(flagsv & ::fast_io::mmap_flags::map_type)};
		switch (exclusiveflags)
		{
#ifdef MAP_SHARED_VALIDATE
		case 3:
			flagstemp |= MAP_SHARED_VALIDATE;
			break;
#endif
		case 2:
			flagstemp |= MAP_PRIVATE;
			break;
		case 1:
			flagstemp |= MAP_SHARED;
			break;
		}
		this->prot = prottemp;
		this->flags = flagstemp;
	}
#endif
};

} // namespace fast_io
