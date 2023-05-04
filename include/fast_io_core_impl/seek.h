#pragma once

namespace fast_io
{

template<typename T>
struct seek_type_t
{
inline constexpr explicit seek_type_t() = default;
};

template<typename T>
inline constexpr seek_type_t<T> seek_type{};

template<fast_io::random_access_stream stream>
inline constexpr void rewind(stream&& stm)
{
	seek(stm,0,fast_io::seekdir::beg);
}


template<typename T> struct io_cookie_type_t{};

template<typename T>
inline constexpr io_cookie_type_t<T> io_cookie_type{};


}
