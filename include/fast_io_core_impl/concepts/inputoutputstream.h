#pragma once

namespace fast_io::details::streamreflect
{

template<typename T>
concept has_io_stream_seek = requires(T&& iostm)
{
	::fast_io::details::io_stream_seek_impl(::fast_io::manipulators::io_stream_ref(iostm),0,::fast_io::seekdir::cur);
};

template<typename T>
concept has_io_stream_seek_bytes = requires(T&& iostm)
{
	::fast_io::details::io_stream_seek_bytes_impl(::fast_io::manipulators::io_stream_ref(iostm),0,::fast_io::seekdir::cur);
};

template<typename T>
concept has_io_stream_mutex_ref = requires(T&& iostm)
{
	::fast_io::details::io_stream_mutex_ref_impl(::fast_io::manipulators::io_stream_ref(iostm));
};

template<typename T>
concept statusiostreamdef = requires(T&& instm)
{
	{::fast_io::manipulators::status_io_stream_ref(instm)} noexcept;
};

}
