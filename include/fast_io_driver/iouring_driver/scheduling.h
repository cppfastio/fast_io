#pragma once

namespace fast_io
{

namespace details
{


inline void deal_with_cqe(io_uring_observer ring,io_uring_cqe *cqe)
{
	if(!cqe)
		return;
	void* data{io_uring_cqe_get_data(cqe)};
	std::int32_t res{cqe->res};
	io_uring_cqe_seen(ring.ring,cqe);
	if(res<0)
		static_cast<io_uring_overlapped_base*>(data)->invoke(0,-res);
	else
		static_cast<io_uring_overlapped_base*>(data)->invoke(res,0);
}

}


inline void io_async_wait(io_uring_observer ring)
{
	io_uring_cqe *cqe{};
	int ret{io_uring_wait_cqe(ring.ring,__builtin_addressof(cqe))};
	details::deal_with_cqe(ring,cqe);
}

inline bool io_async_peek(io_uring_observer ring)
{
	io_uring_cqe *cqe{};
	int ret{io_uring_wait_cqe(ring.ring,__builtin_addressof(cqe))};
	if(ret<0)
	{
		if(ret!=-EAGAIN)
			details::deal_with_cqe(ring,cqe);
		return false;
	}
	details::deal_with_cqe(ring,cqe);
	return true;
}

namespace details
{

inline bool io_uring_io_async_wait_timeout_detail(io_uring_observer ring,__kernel_timespec ts)
{
	io_uring_cqe *cqe{};
	int ret{io_uring_wait_cqe_timeout(ring.ring,__builtin_addressof(cqe),__builtin_addressof(ts))};
	if(ret<0)
	{
		if(ret!=-ETIME)
			details::deal_with_cqe(ring,cqe);
		return false;
	}
	details::deal_with_cqe(ring,cqe);
	return true;
}

}

template<typename Rep,typename Period>
inline auto io_async_wait_timeout(io_uring_observer ring,std::chrono::duration<Rep,Period> duration)
{
	auto val{std::chrono::duration_cast<std::chrono::nanoseconds>(duration)};
	return details::io_uring_io_async_wait_timeout_detail(ring,{std::chrono::duration_cast<std::chrono::seconds>(duration).count(),std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()%1000000000});
}

}
