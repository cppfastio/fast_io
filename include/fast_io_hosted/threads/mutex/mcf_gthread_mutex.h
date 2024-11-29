#pragma once

namespace fast_io
{

struct mcf_gthread_mutex
{
	using native_handle_type = __MCF_mutex;
	native_handle_type mutex{};
	inline explicit mcf_gthread_mutex() noexcept
	{
		_MCF_mutex_init(__builtin_addressof(mutex));
	}
	inline mcf_gthread_mutex(mcf_gthread_mutex const &) = delete;
	inline mcf_gthread_mutex &operator=(mcf_gthread_mutex const &) = delete;
	inline void lock() noexcept
	{
		_MCF_mutex_lock(__builtin_addressof(mutex), nullptr);
	}
	inline bool try_lock() noexcept
	{
		::std::int_least64_t timeout{};
		return !_MCF_mutex_lock(__builtin_addressof(mutex), __builtin_addressof(timeout));
	}
	inline void unlock() noexcept
	{
		_MCF_mutex_unlock(__builtin_addressof(mutex));
	}
};

} // namespace fast_io
