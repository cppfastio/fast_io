#pragma once

namespace fast_io
{

struct single_thread_noop_mutex
{
	inline constexpr void lock() noexcept
	{}
	
	inline constexpr void unlock() noexcept
	{}

	inline constexpr bool try_lock() noexcept
	{
		return true;
	}
};

} // namespace fast_io
