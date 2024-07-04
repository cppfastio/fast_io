#pragma once

namespace fast_io
{

namespace operations::decay
{

template <typename T>
inline constexpr ::fast_io::transcode_bytes_result transcode_bytes_decay(T t,
		::std::byte const *fromfirst, ::std::byte const *fromlast,
		::std::byte *tofirst, ::std::byte *tolast)
{
	return transcode_bytes_decay_define(t,fromfirst,fromlast,tofirst,tolast);
}

}

}