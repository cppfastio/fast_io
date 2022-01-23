#pragma once
#include<unistd.h>
#include<sys/syscall.h>

namespace fast_io
{

template<std::size_t syscall_number,std::signed_integral return_value_type,typename... Args>
requires (std::is_trivially_copyable_v<Args>&&...)
inline return_value_type system_call(Args ...args) noexcept
{
	long ret{::syscall(syscall_number,args...)};
	if(ret==-1)
		return static_cast<return_value_type>(-errno);
	return static_cast<return_value_type>(ret);
}

template<std::size_t syscall_number>
[[noreturn]] inline void system_call_no_return(auto p1) noexcept
{
	::syscall(syscall_number,p1);
}

template<std::integral I>
[[noreturn]] inline void fast_exit(I ret) noexcept
{
	system_call_no_return<__NR_exit>(ret);
}
}