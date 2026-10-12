#pragma once

namespace fast_io::operations::decay
{

/*
 * Public entry point for an asynchronous connect: submits the stream's
 * async_connect_define, found by ADL on the scheduler/stream types.
 * addr/addrlen are the peer address in sockaddr wire layout; the backend
 * copies them synchronously at submission. The socket stays with the
 * caller — on success it is connected, on error it is left in the
 * platform's failed-connect() state.
 *
 * The functor is invoked once as callback(::std::cxx_std_error)
 * noexcept: domain == nullptr means the socket is connected.
 */
template <typename async_scheduler_type, typename streamtype, typename callback_type>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
				 ::std::remove_cvref_t<callback_type>> &&
			 ::fast_io::operations::decay::defines::has_async_connect_define<
				 async_scheduler_type, streamtype, ::std::remove_cvref_t<callback_type>>
inline void async_connect_decay_callback(async_scheduler_type scheduler,
										 ::fast_io::posix_statx_timestamp_opt timeout,
										 streamtype stm, void const *addr, ::std::size_t addrlen,
										 callback_type callback) noexcept
{
	async_connect_define(scheduler, timeout, stm, addr, addrlen, ::std::move(callback));
}

} // namespace fast_io::operations::decay

namespace fast_io::details
{

/*
 * coroutine awaiter for async_connect_decay. The peer address is copied
 * into the awaiter — the co_await expression's temporaries are gone by
 * the time await_suspend submits, and 128 bytes covers every sockaddr
 * variant (sockaddr_storage size).
 */
template <typename scheduler, typename streamtype>
struct async_connect_awaiter : async_awaiter_result<void>
{
	scheduler sched;
	streamtype stm;
	alignas(16)::std::byte addr[128]{};
	::std::size_t addrlen{};
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_connect_decay_callback(
			sched, timeout, stm, __builtin_addressof(addr), addrlen,
			::fast_io::details::async_awaiter_callback<void>{this});
		return this->async_suspend_done();
	}
	inline constexpr void await_resume() FAST_IO_HERBCEPTIONS_THROWS
	{
		if (this->err.domain != nullptr)
		{
			auto e{this->err};
			this->err = {};
			throw throws e;
		}
	}
};

} // namespace fast_io::details

namespace fast_io::operations::decay
{

/*
 * Coroutine form of async_connect: suspends until the socket is
 * connected; await_resume() rethrows the error through the channel.
 * addrlen is clamped to the storage size — an oversized address reports
 * invalid_argument through the error channel rather than truncating.
 */
template <typename async_scheduler_type, typename streamtype>
inline auto async_connect_decay(async_scheduler_type scheduler,
								::fast_io::posix_statx_timestamp_opt timeout, streamtype stm,
								void const *addr, ::std::size_t addrlen) noexcept
{
	::fast_io::details::async_connect_awaiter<async_scheduler_type, streamtype> awaiter{
		{}, scheduler, stm};
	awaiter.timeout = timeout;
	if (addr != nullptr && addrlen <= sizeof(awaiter.addr)) [[likely]]
	{
		awaiter.addrlen = addrlen;
		__builtin_memcpy(__builtin_addressof(awaiter.addr), addr, addrlen);
	}
	else
	{
		/* route the bad address through the same callback path so the
		 * coroutine observes it as an error, not a silent no-op */
		awaiter.addrlen = 0;
	}
	return awaiter;
}

} // namespace fast_io::operations::decay
