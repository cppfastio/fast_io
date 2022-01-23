#pragma once
#include<thread>

namespace fast_io::win32
{

extern "C" int __stdcall GetQueuedCompletionStatus(void*,std::uint32_t*,std::uintptr_t,overlapped*,std::uint32_t);

namespace details
{

template<typename... Args>
requires (sizeof...(Args)==5)
inline void get_queued_completion_status(Args&& ..args)
{
	if(!win32::GetQueuedCompletionStatus(::fast_io::freestanding::forward<Args>(args)...))
		throw_win32_error();
}

template<std::integral ch_type>
struct async_result
{
	basic_win32_io_observer<ch_type> io_observer{};
	bool is_read{};
	std::size_t transferred{};
};

template<std::integral ch_type = char>
inline async_result<ch_type> async_wait(win32_io_observer port)
{
	std::uint32_t trans{};
	void* hd{};
	overlapped* over{};
	get_queued_completion_status(port.native_handle(),__builtin_addressof(trans),__builtin_addressof(hd),__builtin_addressof(over),-1);
	
}
}

}