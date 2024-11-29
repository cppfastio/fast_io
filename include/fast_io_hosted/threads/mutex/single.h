#pragma once

namespace fast_io
{

struct single_thread_noop_mutex
{
	inline constexpr void lock() noexcept
	{}
	inline constexpr void unlock() noexcept
	{}
	inline constexpr void try_lock() noexcept
	{}
};

} // namespace fast_io
