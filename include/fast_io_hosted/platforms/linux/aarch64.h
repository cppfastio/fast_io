#pragma once

namespace fast_io
{

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call() noexcept
{
	register std::uint_least64_t x8 __asm__("x8") = syscall_number;
	register std::uint_least64_t x0 __asm__("x0");
	__asm__ __volatile__
	("svc 0"
	: "=r" (x0)
	: "r"(x8)
	: "memory", "cc"
	);
	return static_cast<return_value_type>(x0);
}

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1) noexcept
{
	register std::uint_least64_t x8 __asm__("x8") = syscall_number;
	register std::uint_least64_t x0 __asm__("x0") = (std::uint_least64_t)p1;
	__asm__ __volatile__
	("svc 0"
	: "=r" (x0)
	: "r"(x8), "0"(x0)
	: "memory", "cc"
	);
	return static_cast<return_value_type>(x0);
}

template<std::size_t syscall_number>
inline void system_call_no_return(auto p1) noexcept
{
	register std::uint_least64_t x8 __asm__("x8") = syscall_number;
	register std::uint_least64_t x0 __asm__("x0") = (std::uint_least64_t)p1;
	__asm__ __volatile__
	("svc 0"
	: "=r" (x0)
	: "r"(x8), "0"(x0)
	: "memory", "cc"
	);
}

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1,auto p2) noexcept
{
	register std::uint_least64_t x8 __asm__("x8") = syscall_number;
	register std::uint_least64_t x0 __asm__("x0") = (std::uint_least64_t)p1;
	register std::uint_least64_t x1 __asm__("x1") = (std::uint_least64_t)p2;
	__asm__ __volatile__
	("svc 0"
	: "=r" (x0)
	: "r"(x8), "0"(x0), "r"(x1)
	: "memory", "cc"
	);
	return static_cast<return_value_type>(x0);
}

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1,auto p2,auto p3) noexcept
{
	register std::uint_least64_t x8 __asm__("x8") = syscall_number;
	register std::uint_least64_t x0 __asm__("x0") = (std::uint_least64_t)p1;
	register std::uint_least64_t x1 __asm__("x1") = (std::uint_least64_t)p2;
	register std::uint_least64_t x2 __asm__("x2") = (std::uint_least64_t)p3;
	__asm__ __volatile__
	("svc 0"
	: "=r" (x0)
	: "r"(x8), "0"(x0), "r"(x1), "r"(x2)
	: "memory", "cc"
	);
	return static_cast<return_value_type>(x0);
}

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1,auto p2,auto p3,auto p4) noexcept
{
	register std::uint_least64_t x8 __asm__("x8") = syscall_number;
	register std::uint_least64_t x0 __asm__("x0") = (std::uint_least64_t)p1;
	register std::uint_least64_t x1 __asm__("x1") = (std::uint_least64_t)p2;
	register std::uint_least64_t x2 __asm__("x2") = (std::uint_least64_t)p3;
	register std::uint_least64_t x3 __asm__("x3") = (std::uint_least64_t)p4;
	__asm__ __volatile__
	("svc 0"
	: "=r" (x0)
	: "r"(x8), "0"(x0), "r"(x1), "r"(x2), "r"(x3)
	: "memory", "cc"
	);
	return static_cast<return_value_type>(x0);
}

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1,auto p2,auto p3,auto p4,auto p5) noexcept
{
	register std::uint_least64_t x8 __asm__("x8") = syscall_number;
	register std::uint_least64_t x0 __asm__("x0") = (std::uint_least64_t)p1;
	register std::uint_least64_t x1 __asm__("x1") = (std::uint_least64_t)p2;
	register std::uint_least64_t x2 __asm__("x2") = (std::uint_least64_t)p3;
	register std::uint_least64_t x3 __asm__("x3") = (std::uint_least64_t)p4;
	register std::uint_least64_t x4 __asm__("x4") = (std::uint_least64_t)p5;
	__asm__ __volatile__
	("svc 0"
	: "=r" (x0)
	: "r"(x8), "0"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x4)
	: "memory", "cc"
	);
	return static_cast<return_value_type>(x0);
}

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1,auto p2,auto p3,auto p4,auto p5,auto p6) noexcept
{
	register std::uint_least64_t x8 __asm__("x8") = syscall_number;
	register std::uint_least64_t x0 __asm__("x0") = (std::uint_least64_t)p1;
	register std::uint_least64_t x1 __asm__("x1") = (std::uint_least64_t)p2;
	register std::uint_least64_t x2 __asm__("x2") = (std::uint_least64_t)p3;
	register std::uint_least64_t x3 __asm__("x3") = (std::uint_least64_t)p4;
	register std::uint_least64_t x4 __asm__("x4") = (std::uint_least64_t)p5;
	register std::uint_least64_t x5 __asm__("x5") = (std::uint_least64_t)p6;
	__asm__ __volatile__
	("svc 0"
	: "=r" (x0)
	: "r"(x8), "0"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5)
	: "memory", "cc"
	);
	return static_cast<return_value_type>(x0);
}

template<std::integral I>
inline void fast_exit(I ret) noexcept
{
	system_call_no_return<__NR_exit>(ret);
}

}
