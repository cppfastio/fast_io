#pragma once

namespace fast_io
{

struct mcf_gthread_mutex
{
	using native_handle_type = __gthread_mutex_t;
	native_handle_type mutex{};
	constexpr mcf_gthread_mutex() noexcept=default;
	mcf_gthread_mutex(mcf_gthread_mutex const&)=delete;
	mcf_gthread_mutex& operator=(mcf_gthread_mutex const&)=delete;
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