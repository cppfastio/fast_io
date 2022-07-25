#pragma once
#if defined(_MSC_VER)&&!defined(__clang__)
#include<cstdlib>
#endif

namespace fast_io
{


[[noreturn]] inline void fast_terminate() noexcept
{
//https://llvm.org/doxygen/Compiler_8h_source.html
#if defined(__has_builtin)
#if __has_builtin(__builtin_trap)
	__builtin_trap();
#elif __has_builtin(__builtin_abort)
	__builtin_abort();
#else
	std::abort();
#endif
#else
	std::abort();
#endif
}

}
