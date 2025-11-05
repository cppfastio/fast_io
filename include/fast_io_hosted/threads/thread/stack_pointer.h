#pragma once

#if defined (_MSC_VER) && !defined(__clang__)
#error "get_stack_pointer is not supported on MSVC"
#endif

namespace fast_io::details
{

/**
 * @brief Get the stack pointer.
 * @return void* The stack pointer.
 *
 * @note MSVC is not supported.
 */
[[nodiscard]]
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#else
#error "__gnu__::__always_inline__ is required"
#endif
#if __has_cpp_attribute(__gnu__::__artificial__)
[[__gnu__::__artificial__]]
#endif
inline void *get_stack_pointer() noexcept
{
	void *result
#if __has_cpp_attribute(indeterminate)
		[[indeterminate]]
#endif
		;

#if defined(__x86_64__) && !defined(__arm64ec__)
	__asm__ volatile("movq %%rsp, %0" : "=r"(result));
#elif defined(__i386__)
	__asm__ volatile("movl %%esp, %0" : "=r"(result));
#elif defined(__aarch64__) || defined(__arm64ec__)
	__asm__ volatile("mov %0, sp" : "=r"(result));
#elif defined(__arm__)
	__asm__ volatile("mov %0, sp" : "=r"(result));
#elif defined(__loongarch__)
	__asm__ volatile("move %0, $sp" : "=r"(result));
#elif defined(__riscv)
	__asm__ volatile("mv %0, sp" : "=r"(result));
#elif defined(__mips__)
	__asm__ volatile("move %0, $sp" : "=r"(result));
#elif defined(__powerpc__)
	__asm__ volatile("mr %0, 1" : "=r"(result));
#else
    ::fast_io::fast_terminate();
#endif
	return result;
}

} // namespace fast_io
