#pragma once

namespace fast_io
{

struct gcc_gthread_mutex
{
	using native_handle_type = __gthread_mutex_t;
	native_handle_type mutex{};
	constexpr explicit gcc_gthread_mutex() noexcept=default;
	gcc_gthread_mutex(gcc_gthread_mutex const&)=delete;
	gcc_gthread_mutex& operator=(gcc_gthread_mutex const&)=delete;
	void lock() noexcept
	{
		__gthread_mutex_lock(__builtin_addressof(mutex));
	}
	bool try_lock() noexcept
	{
		return !__gthread_mutex_trylock(__builtin_addressof(mutex));
	}
	void unlock() noexcept
	{
		__gthread_mutex_unlock(__builtin_addressof(mutex));
	}
};

}