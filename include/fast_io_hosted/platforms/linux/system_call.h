#pragma once

#if defined(__linux__)
#include<asm/unistd.h>
#ifdef __x86_64__
#include"amd64.h"
#elif defined(__arm64__) || defined(__aarch64__) 
#include"aarch64.h"
#elif defined(__riscv) && __SIZEOF_SIZE_T__==8
#include"riscv64.h"
#elif defined(__loongarch__) && __SIZEOF_SIZE_T__==8
#include"loongarch64.h"
#else
#include"generic.h"
#endif
#endif


namespace fast_io
{

#if defined(__linux__)

template<std::integral int_type>
inline constexpr bool linux_system_call_fails(int_type v) noexcept
{
	using unsigned_t = std::make_unsigned_t<int_type>;
	return static_cast<unsigned_t>(static_cast<unsigned_t>(v)+static_cast<unsigned_t>(4096))<static_cast<unsigned_t>(4096);
}

#endif

template<bool always_terminate=false,std::integral I>
requires(sizeof(I)>=1)
inline void system_call_throw_error(I v)
{
#if defined(__linux__)
	using unsigned_t = std::make_unsigned_t<I>;
	if(static_cast<unsigned_t>(static_cast<unsigned_t>(v)+static_cast<unsigned_t>(4096))<static_cast<unsigned_t>(4096))
	{
		if constexpr(always_terminate)
			fast_terminate();
		else
		{
			throw_posix_error(static_cast<int>(-v));
		}
	}
#else
	if(v<0)
	{
		if constexpr(always_terminate)
			fast_terminate();
		else
		{
			throw_posix_error();
		}
	}
#endif
}

}
