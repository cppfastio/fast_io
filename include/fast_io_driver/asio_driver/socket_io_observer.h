#pragma once

namespace fast_io::asio_driver
{

template<typename T,std::integral ch_type>
class basic_socket_io_observer
{
public:
	using char_type = ch_type;
	using native_handle_type = T*;
	native_handle_type handle{};

	constexpr operator bool() const noexcept
	{
		return handle;
	}
	constexpr auto& native_handle() const noexcept
	{
		return handle;
	}

	constexpr auto& native_handle() noexcept
	{
		return handle;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle=nullptr;
		return temp;
	}
};
template<typename T,std::integral ch_type,::std::contiguous_iterator Iter>
inline Iter read(basic_socket_io_observer<T,ch_type> iob,Iter begin,Iter end)
{
	std::error_code ec{};//WTF??? WHY?? WHY??WHY??WHY??WHY??WHY??WHY??WHY?? FUCK YOU ASIO
	std::size_t sz{iob.handle->read_some(asio::buffer(::std::to_address(begin), (end-begin)*sizeof(*begin)),ec)};
	if(ec==asio::error::eof)	//This is BRAINDEAD RETARDED. HOW COULD THIS SHIT GET ADDED INTO ISO C++?
		return begin;
	else if(ec)
		throw std::system_error(ec);
	return begin+sz/sizeof(*begin);
}

template<typename T,std::integral ch_type,::std::contiguous_iterator Iter>
inline Iter write(basic_socket_io_observer<T,ch_type> iob,Iter begin,Iter end)
{
	return begin+iob.handle->write_some(asio::buffer(::std::to_address(begin), (end-begin)*sizeof(*begin)))/sizeof(*begin);
}
template<typename T,std::integral ch_type>
inline constexpr void flush(basic_socket_io_observer<T,ch_type>){}
}
