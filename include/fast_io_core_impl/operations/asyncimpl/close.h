#pragma once

namespace fast_io::details
{

/*
 * Close accepts any stream direction — a file or a buffered stream is
 * closed as a whole, so the ref cascade tries the bidirectional ref
 * first (in&out streams) and falls back to whichever single-direction
 * ref the stream models. basic_io_buffer lands on basic_io_buffer_ref
 * under all three, so every buffer mode reaches the buffered define.
 */
template <typename T>
inline constexpr decltype(auto) async_close_stream_ref(T &&t) noexcept
{
	if constexpr (requires {
					  ::fast_io::operations::io_stream_ref(
						  ::fast_io::freestanding::forward<T>(t));
				  })
	{
		return ::fast_io::operations::io_stream_ref(::fast_io::freestanding::forward<T>(t));
	}
	else if constexpr (requires {
						   ::fast_io::operations::output_stream_ref(
							   ::fast_io::freestanding::forward<T>(t));
					   })
	{
		return ::fast_io::operations::output_stream_ref(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return ::fast_io::operations::input_stream_ref(::fast_io::freestanding::forward<T>(t));
	}
}

} // namespace fast_io::details

namespace fast_io::operations::decay
{

/*
 * Public entry point for an asynchronous close: submits the stream's
 * async_close_define, found by ADL on the scheduler/stream types. The
 * operation owns the handle from this point — callers have already
 * released it (public async_close/async_close_callback release the
 * stream themselves; the buffered define releases the io_buffer's
 * handle after flushing pending output).
 *
 * The functor is invoked once as callback(::std::cxx_std_error)
 * noexcept: domain == nullptr means the handle is closed.
 */
template <typename async_scheduler_type, typename stmtype, typename callback_type>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
				 ::std::remove_cvref_t<callback_type>> &&
			 ::fast_io::operations::decay::defines::has_async_close_define<
				 async_scheduler_type, stmtype, ::std::remove_cvref_t<callback_type>>
inline void async_close_decay_callback(async_scheduler_type scheduler,
									   ::fast_io::posix_statx_timestamp_opt timeout, stmtype stm,
									   callback_type callback) noexcept
{
	async_close_define(scheduler, timeout, stm, ::std::move(callback));
}

} // namespace fast_io::operations::decay

namespace fast_io::details
{

/* coroutine awaiter for async_close_decay */
template <typename scheduler, typename stmtype>
struct async_close_awaiter : async_awaiter_result<void>
{
	scheduler sched;
	stmtype stm;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_close_decay_callback(
			sched, timeout, stm,
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

/* Coroutine form of async_close: suspends until the handle is closed;
 * await_resume() rethrows the error through the channel. */
template <typename async_scheduler_type, typename stmtype>
inline auto async_close_decay(async_scheduler_type scheduler,
							  ::fast_io::posix_statx_timestamp_opt timeout, stmtype stm) noexcept
{
	return ::fast_io::details::async_close_awaiter<async_scheduler_type, stmtype>{
		{}, scheduler, stm, timeout};
}

} // namespace fast_io::operations::decay
