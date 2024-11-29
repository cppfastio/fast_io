#pragma once

namespace fast_io
{

struct gcc_gthread_mutex
{
	using native_handle_type = __gthread_mutex_t;
	native_handle_type mutex{};
	inline constexpr explicit gcc_gthread_mutex() noexcept = default;
	inline gcc_gthread_mutex(gcc_gthread_mutex const &) = delete;
	inline gcc_gthread_mutex &operator=(gcc_gthread_mutex const &) = delete;
	inline void lock() noexcept
	{
		__gthread_mutex_lock(__builtin_addressof(mutex));
	}
	inline bool try_lock() noexcept
	{
		return !__gthread_mutex_trylock(__builtin_addressof(mutex));
	}
	inline void unlock() noexcept
	{
		__gthread_mutex_unlock(__builtin_addressof(mutex));
	}
};

} // namespace fast_io
