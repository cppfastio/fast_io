#pragma once
#if (!defined(__GNUC__)&&!defined(__clang__)||defined(__INTEL_COMPILER))
#include<intrin.h>
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
#elif defined(_MSC_VER)
	__fastfail(1);
#else
	std::abort();
#endif
}

}