#pragma once

namespace fast_io
{

extern void *linux_kernel_kmalloc(::std::size_t, int unsigned) noexcept __asm__("__kmalloc");

extern void *linux_kernel_krealloc(void const *, ::std::size_t, int unsigned) noexcept __asm__("krealloc");

extern void linux_kernel_kfree(void const *) noexcept __asm__("kfree");

inline constexpr int unsigned linux_kernel_gfp_zero{0x100u};

inline constexpr int unsigned linux_kernel_gfp_kernel{0x400u | 0x800u | 0x40u | 0x80u};

inline constexpr int unsigned linux_kernel_gfp_kernel_zero{linux_kernel_gfp_kernel | linux_kernel_gfp_zero};

class linux_kmalloc_allocator
{
public:
	using handle_type = int unsigned;
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *handle_allocate_die(handle_type gfp, ::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p = linux_kernel_kmalloc(n, gfp);
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *handle_allocate_try(handle_type gfp, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p = linux_kernel_kmalloc(n, gfp);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *handle_reallocate_die(handle_type gfp, void *p, ::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		p = linux_kernel_krealloc(p, n, gfp);
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *handle_reallocate_try(handle_type gfp, void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		if (n == 0)
		{
			n = 1;
		}
		p = linux_kernel_krealloc(p, n, gfp);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *handle_allocate_zero_die(handle_type gfp, ::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p = linux_kernel_kmalloc(n, gfp | linux_kernel_gfp_zero);
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *handle_allocate_zero_try(handle_type gfp, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p = linux_kernel_kmalloc(n, gfp | linux_kernel_gfp_zero);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
	static inline void handle_deallocate([[maybe_unused]] handle_type gfp, void *p) noexcept
	{
		if (p == nullptr) [[unlikely]]
		{
			return;
		}
		linux_kernel_kfree(p);
	}
};

} // namespace fast_io
