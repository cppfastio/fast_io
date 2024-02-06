#pragma once
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#include "../fast_io_freestanding.h"

namespace fast_io
{

class pulse_audio_error
{
public:
	int error{};
	pulse_audio_error(int e = PA_OK)
		: error(e)
	{}
};

inline void throw_pulse_audio_error(int ec = PA_OK)
{
#ifdef __cpp_exceptions
	throw pulse_audio_error(ec);
#else
	fast_terminate();
#endif
}

template <::std::integral ch_type>
class basic_pulseaudio_simple_io_observer
{
public:
	using char_type = ch_type;
	using output_char_type = char_type;
	using input_char_type = char_type;
	using native_handle_type = pa_simple *;
	native_handle_type s{};
	constexpr native_handle_type native_handle() const noexcept
	{
		return s;
	}
	constexpr auto release() noexcept
	{
		auto temp{s};
		s = {};
		return temp;
	}
};

template <::std::integral ch_type>
class basic_pulseaudio_simple_file : public basic_pulseaudio_simple_io_observer<ch_type>
{
public:
	using char_type = ch_type;
	using native_handle_type = pa_simple *;
	constexpr basic_pulseaudio_simple_file()
	{}
	constexpr basic_pulseaudio_simple_file(native_handle_type sf)
		: basic_pulseaudio_simple_io_observer<char_type>{sf}
	{
	}

	basic_pulseaudio_simple_file(native_interface_t, char const *server, char const *name, pa_stream_direction_t dir,
								 char const *dev, char const *stream_name, pa_sample_spec const *ss,
								 pa_channel_map const *map, pa_buffer_attr const *attr)
	{
		int err{};
		this->s = ::fast_io::noexcept_call(pa_simple_new, server, name, dir, dev, stream_name, ss, map, attr,
										   __builtin_addressof(err));
		if (this->s == nullptr)
		{
			throw_pulse_audio_error(err);
		}
	}
	basic_pulseaudio_simple_file(basic_pulseaudio_simple_file const &) = delete;
	basic_pulseaudio_simple_file &operator=(basic_pulseaudio_simple_file const &) = delete;
	constexpr basic_pulseaudio_simple_file(basic_pulseaudio_simple_file &&__restrict hd) noexcept
		: basic_pulseaudio_simple_file(hd.s)
	{
		hd.s = nullptr;
	}
	basic_pulseaudio_simple_file &operator=(basic_pulseaudio_simple_file &&__restrict hd) noexcept
	{
		if (this->s) [[likely]]
		{
			::fast_io::noexcept_call(pa_simple_free, this->s);
		}
		this->s = hd.s;
		hd.s = nullptr;
		return *this;
	}
	inline void reset(native_handle_type newhandle = nullptr) noexcept
	{
		if (this->s) [[likely]]
		{
			::fast_io::noexcept_call(pa_simple_free, this->s);
		}
		this->s = newhandle;
	}
	~basic_pulseaudio_simple_file()
	{
		if (this->s) [[likely]]
		{
			::fast_io::noexcept_call(pa_simple_free, this->s);
		}
	}
};

namespace details
{

inline void pa_simple_read_underflow_impl(pa_simple *ps, ::std::byte *first, ::std::byte *last)
{
	int err{};
	if (::fast_io::noexcept_call(pa_simple_read, ps, first, static_cast<::std::size_t>(last - first),
								 __builtin_addressof(err)) < 0) [[unlikely]]
	{
		throw_pulse_audio_error(err);
	}
}

inline void pa_simple_write_overflow_impl(pa_simple *ps, ::std::byte const *first, ::std::byte const *last)
{
	int err{};
	if (::fast_io::noexcept_call(pa_simple_write, ps, first, static_cast<::std::size_t>(last - first),
								 __builtin_addressof(err)) < 0) [[unlikely]]
	{
		throw_pulse_audio_error(err);
	}
}

} // namespace details

template <::std::integral ch_type>
inline void read_all_bytes_underflow_define(basic_pulseaudio_simple_io_observer<ch_type> iob, ::std::byte *first,
											::std::byte *last)
{
	return ::fast_io::details::pa_simple_read_underflow_impl(iob.s, first, last);
}

template <::std::integral ch_type>
inline void write_all_bytes_overflow_define(basic_pulseaudio_simple_io_observer<ch_type> iob, ::std::byte const *first,
											::std::byte const *last)
{
	return ::fast_io::details::pa_simple_write_overflow_impl(iob.s, first, last);
}

template <::std::integral ch_type>
inline constexpr basic_pulseaudio_simple_io_observer<ch_type>
io_stream_ref_define(basic_pulseaudio_simple_io_observer<ch_type> other) noexcept
{
	return other;
}

template <::std::integral ch_type>
inline constexpr basic_pulseaudio_simple_io_observer<char>
io_bytes_stream_ref_define(basic_pulseaudio_simple_io_observer<ch_type> other) noexcept
{
	return other;
}

#if 0
template<::std::integral char_type>
inline void flush(basic_pulseaudio_simple_io_observer<char_type> bpsiob)
{
	int err{};
	if(pa_simple_flush(bpsiob.s,__builtin_addressof(err))<0)[[unlikely]]
		throw_pulse_audio_error(err);
}
#endif

using pulseaudio_simple_io_observer = basic_pulseaudio_simple_io_observer<char>;
using pulseaudio_simple_file = basic_pulseaudio_simple_file<char>;

} // namespace fast_io
