#pragma once

namespace fast_io
{

template<std::integral char_type>
inline constexpr io_type_t<io_uring_observer> async_scheduler_type(basic_posix_io_observer<char_type>)
{
	return {};
}

template<std::integral char_type>
inline constexpr io_type_t<io_uring_overlapped> async_overlapped_type(basic_posix_io_observer<char_type>)
{
	return {};
}

namespace details
{

inline auto ensure_io_uring_sqe(io_uring_observer ring)
{
	auto sqe{io_uring_get_sqe(ring.ring)};
	for(;sqe==nullptr;sqe=io_uring_get_sqe(ring.ring))
		submit(ring);
	return sqe;
}

}

template<std::integral char_type,::fast_io::freestanding::contiguous_iterator Iter>
inline void async_write_callback(io_uring_observer ring,basic_posix_io_observer<char_type> piob,
	Iter b,Iter e,io_uring_overlapped_observer callback,std::ptrdiff_t offset)
{
	auto sqe{details::ensure_io_uring_sqe(ring)};
	io_uring_prep_write(sqe,piob.fd,::fast_io::freestanding::to_address(b),(e-b)*sizeof(*b),callback,offset);
	io_uring_sqe_set_data(sqe,callback.native_handle());
}

template<std::integral char_type,::fast_io::freestanding::contiguous_iterator Iter>
inline void async_read_callback(io_uring_observer ring,basic_posix_io_observer<char_type> piob,
	Iter b,Iter e,io_uring_overlapped_observer callback,std::ptrdiff_t offset)
{
	auto sqe{details::ensure_io_uring_sqe(ring)};
	io_uring_prep_read(sqe,piob.fd,::fast_io::freestanding::to_address(b),(e-b)*sizeof(*b),callback,offset);
	io_uring_sqe_set_data(sqe,callback.native_handle());
}


template<std::integral char_type>
inline void async_scatter_write_callback(io_uring_observer ring,basic_posix_io_observer<char_type> piob,
		std::span<io_scatter_t const> span,io_uring_overlapped_observer callback,std::ptrdiff_t offset)
{
	auto sqe{details::ensure_io_uring_sqe(ring)};
	io_uring_prep_writev(sqe,piob.fd,reinterpret_cast<details::iovec_may_alias const*>(span.data()),span.size(),offset);
	io_uring_sqe_set_data(sqe,callback.native_handle());
}


template<std::integral char_type>
inline void async_scatter_read_callback(io_uring_observer ring,basic_posix_io_observer<char_type> piob,
		std::span<io_scatter_t const> span,io_uring_overlapped_observer callback,std::ptrdiff_t offset)
{
	auto sqe{details::ensure_io_uring_sqe(ring)};
	io_uring_prep_readv(sqe,piob.fd,reinterpret_cast<details::iovec_may_alias const*>(span.data()),span.size(),offset);
	io_uring_sqe_set_data(sqe,callback.native_handle());
}


}

