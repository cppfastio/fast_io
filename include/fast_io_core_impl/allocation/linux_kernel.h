#pragma once

namespace fast_io
{

extern void* linux_kernel_kmalloc(::std::size_t,int unsigned) noexcept __asm__("__kmalloc");

extern void* linux_kernel_krealloc(void const*,::std::size_t,int unsigned) noexcept __asm__("krealloc");

extern void linux_kernel_kfree(void const*) noexcept __asm__("kfree");

inline constexpr int unsigned linux_kernel_gfp_kernel{0x400u | 0x800u | 0x40u | 0x80u};

inline constexpr int unsigned linux_kernel_gfp_kernel_zero{linux_kernel_gfp_kernel|0x100u};

class linux_kmalloc_allocator
{
public:
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
	static inline void* allocate(::std::size_t n) noexcept
	{
		if(n==0)
		{
			n=1;
		}
		void* p = linux_kernel_kmalloc(n,linux_kernel_gfp_kernel);
		if(p==nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void* reallocate(void* p,::std::size_t n) noexcept
	{
		if(n==0)
		{
			n=1;
		}
		p=linux_kernel_krealloc(p,n,linux_kernel_gfp_kernel);
		if(p==nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void* allocate_zero(::std::size_t n) noexcept
	{
		if(n==0)
		{
			n=1;
		}
		void* p = linux_kernel_kmalloc(n,linux_kernel_gfp_kernel_zero);
		if(p==nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void deallocate(void* p) noexcept
	{
		if(p==nullptr)
		{
			return;
		}
		linux_kernel_kfree(p);
	}
};

}
