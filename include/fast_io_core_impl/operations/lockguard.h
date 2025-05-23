#pragma once

namespace fast_io::operations::decay
{

template <typename mtx_type>
struct stream_ref_decay_lock_guard
{
	using mutex_type = mtx_type;
	mutex_type device;
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline explicit constexpr stream_ref_decay_lock_guard(mutex_type d)
		: device(d)
	{
		device.lock();
	}
	inline stream_ref_decay_lock_guard(stream_ref_decay_lock_guard const &) = delete;
	inline stream_ref_decay_lock_guard &operator=(stream_ref_decay_lock_guard const &) = delete;
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr ~stream_ref_decay_lock_guard()
	{
		device.unlock();
	}
};

template <typename mtx_type>
struct unlock_stream_ref_decay_lock_guard
{
	using mutex_type = mtx_type;
	mutex_type device;
	#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline explicit constexpr unlock_stream_ref_decay_lock_guard(stream_ref_decay_lock_guard<mtx_type> const& lg)
		: device(lg.device)
	{
		device.unlock();
	}
	inline unlock_stream_ref_decay_lock_guard(unlock_stream_ref_decay_lock_guard const &) = delete;
	inline unlock_stream_ref_decay_lock_guard &operator=(unlock_stream_ref_decay_lock_guard const &) = delete;
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr ~unlock_stream_ref_decay_lock_guard()
	{
		device.lock();
	}
};

} // namespace fast_io::operations::decay
