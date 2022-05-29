#pragma once

namespace fast_io
{

inline void io_async_wait(win32_io_observer iocp)
{
	std::uint_least32_t transferred{};
	std::uintptr_t completionkey{};
	win32::overlapped *over{};
	if(!GetQueuedCompletionStatus(iocp.native_handle(),__builtin_addressof(transferred),__builtin_addressof(completionkey),
		__builtin_addressof(over),std::numeric_limits<std::uint_least32_t>::max()))
		throw_win32_error();
	static_cast<iocp_overlapped_base*>(over)->invoke(static_cast<std::size_t>(transferred));
}

namespace details
{
inline bool iocp_io_async_wait_timeout_detail(win32_io_observer iocp,std::uint_least32_t millseconds)
{
	std::uint_least32_t transferred{};
	std::uintptr_t completionkey{};
	win32::overlapped *over{};
	if(!GetQueuedCompletionStatus(iocp.native_handle(),__builtin_addressof(transferred),__builtin_addressof(completionkey),
		__builtin_addressof(over),millseconds))
	{
		auto errc{win32::GetLastError()};
		if(errc==258)
			return false;
		throw_win32_error(errc);
	}
	static_cast<iocp_overlapped_base*>(over)->invoke(static_cast<std::size_t>(transferred));
	return true;
}
}

template<typename Rep,typename Period>
inline bool io_async_wait_timeout(win32_io_observer iocp,std::chrono::duration<Rep,Period> duration)
{
	return details::iocp_io_async_wait_timeout_detail(iocp,std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

}
