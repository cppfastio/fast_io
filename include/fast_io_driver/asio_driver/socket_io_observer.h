#pragma once

namespace fast_io::asio_driver
{

template <typename T, ::std::integral ch_type>
class basic_socket_io_observer
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = T *;
	native_handle_type handle{};

	constexpr operator bool() const noexcept
	{
		return handle;
	}
	constexpr auto &native_handle() const noexcept
	{
		return handle;
	}

	constexpr auto &native_handle() noexcept
	{
		return handle;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle = nullptr;
		return temp;
	}
};
template <typename T, ::std::integral ch_type>
inline ::std::byte *read_some_bytes_underflow_define(basic_socket_io_observer<T, ch_type> iob, ::std::byte *first,
													 ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::error_code ec{}; // WTF??? WHY?? WHY??WHY??WHY??WHY??WHY??WHY??WHY?? FUCK YOU ASIO
	::std::size_t sz{iob.handle->read_some(asio::buffer(first, count), ec)};
	if (ec == asio::error::eof) // This is BRAINDEAD RETARDED. HOW COULD THIS SHIT GET ADDED INTO ISO C++?
	{
		return first;
	}
	else if (ec)
	{
		throw ::std::system_error(ec);
	}
	return first + sz;
}

template <typename T, ::std::integral ch_type>
inline ::std::byte const *write_some_bytes_overflow_define(basic_socket_io_observer<T, ch_type> iob,
														   ::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return first + iob.handle->write_some(asio::buffer(first, count));
}
template <typename T, ::std::integral ch_type>
inline constexpr void io_stream_buffer_flush_define(basic_socket_io_observer<T, ch_type>) noexcept
{
}
} // namespace fast_io::asio_driver
