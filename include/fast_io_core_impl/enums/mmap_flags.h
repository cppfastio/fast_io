#pragma once

namespace fast_io
{

enum class mmap_prot : ::std::uint_least32_t
{
	prot_read = 0x1,             /* page can be read */
	prot_write = 0x2,            /* page can be written */
	prot_exec = 0x4,             /* page can be executed */
	prot_sem = 0x8,              /* page may be used for atomic ops */
	prot_none = 0x0,             /* page can not be accessed */
	prot_mte = 0x20,             /* page is protected with Memory Tagging (ARM MTE for example) */
	prot_growsdown = 0x01000000, /* mprotect flag: extend change to start of growsdown vma */
	prot_growsup = 0x02000000,   /* mprotect flag: extend change to end of growsup vma */
};

inline constexpr mmap_prot operator&(mmap_prot x, mmap_prot y) noexcept
{
	using utype = typename ::std::underlying_type<mmap_prot>::type;
	return static_cast<mmap_prot>(static_cast<utype>(x) & static_cast<utype>(y));
}

inline constexpr mmap_prot operator|(mmap_prot x, mmap_prot y) noexcept
{
	using utype = typename ::std::underlying_type<mmap_prot>::type;
	return static_cast<mmap_prot>(static_cast<utype>(x) | static_cast<utype>(y));
}

inline constexpr mmap_prot operator^(mmap_prot x, mmap_prot y) noexcept
{
	using utype = typename ::std::underlying_type<mmap_prot>::type;
	return static_cast<mmap_prot>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

inline constexpr mmap_prot operator~(mmap_prot x) noexcept
{
	using utype = typename ::std::underlying_type<mmap_prot>::type;
	return static_cast<mmap_prot>(~static_cast<utype>(x));
}

inline constexpr mmap_prot &operator&=(mmap_prot &x, mmap_prot y) noexcept
{
	return x = x & y;
}

inline constexpr mmap_prot &operator|=(mmap_prot &x, mmap_prot y) noexcept
{
	return x = x | y;
}

inline constexpr mmap_prot &operator^=(mmap_prot &x, mmap_prot y) noexcept
{
	return x = x ^ y;
}

enum class mmap_flags : ::std::uint_least32_t
{
	map_file = 0, /* compatibility flags */
	map_nocore = map_file,
	map_nosync = map_file,
	map_shared = 1,
	map_private = 2,
	map_shared_validate = 3,
	map_type = 0x0f,
	map_fixed = 0x10,
	map_anonymous = 0x20,
	map_anon = map_anonymous,
	map_32bit = 0x40,   /* only give out 32bit addresses */
	map_above4g = 0x80, /* only map above 4GB */
	map_hassemaphore = 0x0200,
	map_inherit = 0x0400,
	map_unaligned = 0x0800,
	map_growsdown = 0x01000,        /* stack-like segment */
	map_denywrite = 0x02000,        /* ETXTBSY */
	map_executable = 0x04000,       /* mark it as an executable */
	map_locked = 0x08000,           /* lock the mapping */
	map_noreserve = 0x10000,        /* don't check for reservations */
	map_populate = 0x20000,         /* populate (prefault) pagetables */
	map_nonblock = 0x40000,         /* do not block on IO */
	map_stack = 0x80000,            /* give out an address that is best suited for process/thread stacks */
	map_hugetlb = 0x100000,         /* create a huge page mapping */
	map_fixed_noreplace = 0x200000, /* MAP_FIXED which doesn't unmap underlying mapping */
	map_uninitialized = 0x4000000,  /* For anonymous mmap, memory could be uninitialized */
	map_huge_2mb = static_cast<::std::uint_least32_t>(21u) << 26,
	map_huge_1gb = static_cast<::std::uint_least32_t>(30u) << 26,
};

inline constexpr mmap_flags operator&(mmap_flags x, mmap_flags y) noexcept
{
	using utype = typename ::std::underlying_type<mmap_flags>::type;
	return static_cast<mmap_flags>(static_cast<utype>(x) & static_cast<utype>(y));
}

inline constexpr mmap_flags operator|(mmap_flags x, mmap_flags y) noexcept
{
	using utype = typename ::std::underlying_type<mmap_flags>::type;
	return static_cast<mmap_flags>(static_cast<utype>(x) | static_cast<utype>(y));
}

inline constexpr mmap_flags operator^(mmap_flags x, mmap_flags y) noexcept
{
	using utype = typename ::std::underlying_type<mmap_flags>::type;
	return static_cast<mmap_flags>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

inline constexpr mmap_flags operator~(mmap_flags x) noexcept
{
	using utype = typename ::std::underlying_type<mmap_flags>::type;
	return static_cast<mmap_flags>(~static_cast<utype>(x));
}

inline constexpr mmap_flags &operator&=(mmap_flags &x, mmap_flags y) noexcept
{
	return x = x & y;
}

inline constexpr mmap_flags &operator|=(mmap_flags &x, mmap_flags y) noexcept
{
	return x = x | y;
}

inline constexpr mmap_flags &operator^=(mmap_flags &x, mmap_flags y) noexcept
{
	return x = x ^ y;
}

} // namespace fast_io
