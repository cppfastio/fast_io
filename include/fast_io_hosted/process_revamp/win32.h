#pragma once

namespace fast_io
{

class win32_process_observer
{
public:
	using native_handle_type = pid_t;
	pid_t pid{-1};
	constexpr auto& native_handle() noexcept
	{
		return pid;
	}
	constexpr auto& native_handle() const noexcept
	{
		return pid;
	}
	explicit inline constexpr operator bool() const noexcept
	{
		return pid!=-1;
	}
	constexpr pid_t release() noexcept
	{
		auto temp{pid};
		pid=-1;
		return temp;
	}
};

struct win32_process
{

};

}