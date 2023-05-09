#pragma once

namespace fast_io::operations
{

template<typename mutx_type>
struct output_stream_lock_guard
{
	using mutex_type = mutx_type;
	mutex_type device;
	explicit constexpr output_stream_lock_guard(mutex_type m) noexcept: device(m)
	{
		(device);
	}
	output_stream_lock_guard(output_stream_lock_guard const&)=delete;
	output_stream_lock_guard& operator=(output_stream_lock_guard const&)=delete;
	constexpr ~output_stream_lock_guard()
	{

	}

};

template<typename T>
struct input_stream_lock_guard
{

};

template<typename T>
struct io_stream_lock_guard
{

};

}
