#pragma once

#include<coroutine>

namespace fast_io
{
struct task
{
struct promise_type
{
constexpr auto get_return_object() noexcept { return task{}; }
constexpr auto initial_suspend() noexcept { return std::suspend_never{}; }
constexpr auto final_suspend() noexcept { return std::suspend_never{}; }
void unhandled_exception() noexcept { __builtin_trap(); }
constexpr void return_void() noexcept {}
};
};

#if defined(_WIN32)&&!defined(__WINE__) || defined(__CYGWIN__)
template<win32_family fam>
inline constexpr auto async_write(basic_win32_family_io_observer<fam,char> wiob,nt_at_entry ent,void const* buffer,std::size_t buffersize,std::ptrdiff_t diff)
{
	struct awaiter
	{
		void* handle;
		void const* buffer;
		std::size_t buffer_size;
		std::ptrdiff_t diff;
		std::size_t wrtn{};
		constexpr bool await_ready() const noexcept { return false; }
		constexpr void await_suspend(std::coroutine_handle<> co_handle) noexcept
		{
			try
			{
			::fast_io::details::iocp_async_write_define(handle,buffer,buffer_size,diff,[this,co_handle](std::size_t written)
			{
				this->wrtn=written;
				co_handle();
			});
			}
			catch(fast_io::win32_error e)
			{
				perrln(handle," ",e);
			}
		}
		constexpr std::size_t await_resume() const noexcept
		{
			return wrtn;
		}
	};
	return awaiter{ent.handle,buffer,buffersize,diff};
}
#endif
}