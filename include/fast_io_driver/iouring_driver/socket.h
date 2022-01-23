#pragma once

namespace fast_io
{

template<std::integral char_type>
inline constexpr io_type_t<io_uring_observer> async_scheduler_type(basic_socket_io_observer<char_type>)
{
	return {};
}

template<std::integral char_type>
inline constexpr io_type_t<io_uring_overlapped> async_overlapped_type(basic_socket_io_observer<char_type>)
{
	return {};
}


template<std::integral char_type>
inline void async_connect_callback(io_uring_observer ring,basic_socket_io_observer<char_type> sio,endpoint& ep,io_uring_overlapped_observer callback)
{
	auto sqe{details::ensure_io_uring_sqe(ring)};
	io_uring_prep_connect(sqe,sio.soc,__builtin_addressof(ep.storage.sock),ep.storage_size);
	io_uring_sqe_set_data(sqe,callback.native_handle());
}

template<std::integral char_type>
inline void async_accept_callback(io_uring_observer ring,basic_socket_io_observer<char_type> sio,endpoint& ep,io_uring_overlapped_observer callback)
{
	auto sqe{details::ensure_io_uring_sqe(ring)};
	io_uring_prep_accept(sqe,sio.soc,__builtin_addressof(ep.storage.sock),__builtin_addressof(ep.storage_size),0);//,__builtin_addressof(ep.storage.sock),ep.storage_size);
	io_uring_sqe_set_data(sqe,callback.native_handle());
}

/*
template<std::integral char_type>
inline void async_scatter_write_callback(io_uring_observer ring,basic_socket_io_observer<char_type> sio,std::span<io_scatter_t const> sp,io_uring_overlapped_observer callback,std::ptrdiff_t)
{
	basic_posix_io_observer<char_type> pio(sio);
	async_scatter_write_callback(ring,pio,sp,callback,offset);
}

template<std::integral char_type>
inline void async_scatter_read_callback(io_uring_observer ring,basic_socket_io_observer<char_type> sio,std::span<io_scatter_t const> sp,io_uring_overlapped_observer callback,std::ptrdiff_t)
{
	basic_posix_io_observer<char_type> pio(sio);
	async_scatter_read_callback(ring,pio,sp,callback,offset);
}
*/
template<std::integral char_type,::fast_io::freestanding::contiguous_iterator Iter>
inline void async_read_callback(io_uring_observer ring,basic_socket_io_observer<char_type> sio,Iter b,Iter e,io_uring_overlapped_observer callback,std::ptrdiff_t)
{
	auto sqe{details::ensure_io_uring_sqe(ring)};
	io_uring_prep_recv(sqe,sio.soc,::fast_io::freestanding::to_address(b),(e-b)*sizeof(*b),0);
	io_uring_sqe_set_data(sqe,callback.native_handle());
}

template<std::integral char_type,::fast_io::freestanding::contiguous_iterator Iter>
inline void async_write_callback(io_uring_observer ring,basic_socket_io_observer<char_type> sio,Iter b,Iter e,io_uring_overlapped_observer callback,std::ptrdiff_t)
{
	auto sqe{details::ensure_io_uring_sqe(ring)};
	io_uring_prep_send(sqe,sio.soc,::fast_io::freestanding::to_address(b),(e-b)*sizeof(*b),0);
	io_uring_sqe_set_data(sqe,callback.native_handle());
}


}
