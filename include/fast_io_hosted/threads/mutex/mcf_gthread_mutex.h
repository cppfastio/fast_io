#pragma once

namespace fast_io
{

struct mcf_gthread_mutex
{
	using native_handle_type = __MCF_mutex;
	native_handle_type mutex{};
	explicit mcf_gthread_mutex() noexcept
	{
		_MCF_mutex_init(__builtin_addressof(mutex));
	}
	mcf_gthread_mutex(mcf_gthread_mutex const&)=delete;
	mcf_gthread_mutex& operator=(mcf_gthread_mutex const&)=delete;
	void lock() noexcept
	{
		_MCF_mutex_lock(__builtin_addressof(mutex),nullptr);
	}
	bool try_lock() noexcept
	{
		::std::int_least64_t timeout{};
		return !_MCF_mutex_lock(__builtin_addressof(mutex),__builtin_addressof(timeout));
	}
	void unlock() noexcept
	{
		_MCF_mutex_unlock(__builtin_addressof(mutex));
	}
};

}
