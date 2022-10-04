#pragma once
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#include"../fast_io_hosted.h"

namespace fast_io
{

class pulse_audio_error
{
public:
	int error{};
	pulse_audio_error(int e=PA_OK):error(e){}
#if 0
	void report(error_reporter& err) const override
	{
		print_freestanding(err,manipulators::os_c_str(pa_strerror(error)));
	}
#endif
};

inline void throw_pulse_audio_error(int ec=PA_OK)
{
#ifdef __cpp_exceptions
	throw pulse_audio_error(ec);
#else
	fast_terminate();
#endif
}

template<std::integral ch_type>
class basic_pulseaudio_simple_io_observer
{
public:
	using char_type = ch_type;
	using native_handle_type = pa_simple*;
	native_handle_type s{};
	constexpr auto& native_handle() const noexcept
	{
		return s;
	}
	constexpr auto& native_handle() noexcept
	{
		return s;
	}
	constexpr auto release() noexcept
	{
		auto temp{s};
		s={};
		return temp;
	}
};

template<std::integral ch_type>
class basic_pulseaudio_simple_file:public basic_pulseaudio_simple_io_observer<ch_type>
{
public:
	using char_type = ch_type;
	using native_handle_type = pa_simple*;
	constexpr basic_pulseaudio_simple_file(){}
	constexpr basic_pulseaudio_simple_file(native_handle_type sf):basic_pulseaudio_simple_io_observer<char_type>{sf}{}

	basic_pulseaudio_simple_file(native_interface_t,
		char const* 	server,
		char const* 	name,
		pa_stream_direction_t 	dir,
		char const * 	dev,
		char const * 	stream_name,
		pa_sample_spec const * 	ss,
		pa_channel_map const * 	map,
		pa_buffer_attr const * 	attr)
	{
		int err{};
		this->native_handle()=pa_simple_new(server,name,dir,dev,stream_name,ss,map,attr,__builtin_addressof(err));
		if(this->native_handle()==nullptr)
			throw_pulse_audio_error(err);
	}
	basic_pulseaudio_simple_file(basic_pulseaudio_simple_file const&)=delete;
	basic_pulseaudio_simple_file& operator=(basic_pulseaudio_simple_file const&)=delete;
	constexpr basic_pulseaudio_simple_file(basic_pulseaudio_simple_file&& __restrict hd) noexcept:basic_pulseaudio_simple_file(hd.native_handle())
	{
		hd.native_handle()=nullptr;
	}
	basic_pulseaudio_simple_file& operator=(basic_pulseaudio_simple_file&& __restrict hd) noexcept
	{
		if(this->native_handle())[[likely]]
			pa_simple_free(this->native_handle());
		this->native_handle()=hd.native_handle();
		hd.native_handle()=nullptr;
		return *this;
	}
	inline void reset(native_handle_type newhandle=nullptr) noexcept
	{
		if(this->native_handle())[[likely]]
			pa_simple_free(this->native_handle());
		this->native_handle()=newhandle;
	}
	~basic_pulseaudio_simple_file()
	{
		if(this->native_handle())[[likely]]
			pa_simple_free(this->native_handle());
	}
};

template<std::integral char_type,::std::contiguous_iterator Iter>
inline Iter read(basic_pulseaudio_simple_io_observer<char_type> bpsiob,Iter begin,Iter end)
{
	int err{};
	if(pa_simple_read(bpsiob.native_handle(),::std::to_address(begin),sizeof(*begin)*(end-begin),__builtin_addressof(err))<0)[[unlikely]]
		throw_pulse_audio_error(err);
	return end;
}

template<std::integral char_type,::std::contiguous_iterator Iter>
inline Iter write(basic_pulseaudio_simple_io_observer<char_type> bpsiob,Iter begin,Iter end)
{
	int err{};
	if(pa_simple_write(bpsiob.native_handle(),::std::to_address(begin),sizeof(*begin)*(end-begin),__builtin_addressof(err))<0)[[unlikely]]
		throw_pulse_audio_error(err);
	return end;
}


template<std::integral char_type,::std::contiguous_iterator Iter>
inline void flush(basic_pulseaudio_simple_io_observer<char_type> bpsiob)
{
	int err{};
	if(pa_simple_flush(bpsiob.native_handle(),__builtin_addressof(err))<0)[[unlikely]]
		throw_pulse_audio_error(err);
}

using pulseaudio_simple_io_observer = basic_pulseaudio_simple_io_observer<char>;
using pulseaudio_simple_file = basic_pulseaudio_simple_file<char>;

}
