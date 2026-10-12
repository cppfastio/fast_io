#pragma once

namespace fast_io::operations::decay
{

/*
 * Public entry point for an async output-buffer flush: submits the
 * stream's async_output_stream_buffer_flush_define, which writes every
 * pending output byte to the underlying handle before reporting. This is
 * the async counterpart of flush(output) — bytes buffered by
 * async_pwrite_some on a buffered stream stay buffered until this runs
 * (or a tied input op flushes them implicitly).
 *
 * The functor is invoked once as callback(::std::cxx_std_error) noexcept.
 * The stream object must outlive the operation.
 */
template <typename async_scheduler_type, typename outstmtype, typename callback_type>
	requires ::std::is_nothrow_invocable_v<callback_type, ::std::cxx_std_error> &&
			 ::fast_io::operations::decay::defines::
				 has_async_output_stream_buffer_flush_define<
					 async_scheduler_type,
					 outstmtype,
					 ::std::remove_cvref_t<callback_type>>
inline void async_output_stream_flush_decay_callback(
	async_scheduler_type scheduler, ::fast_io::posix_statx_timestamp_opt timeout,
	outstmtype outstm, callback_type callback) noexcept
{
	async_output_stream_buffer_flush_define(
		scheduler, timeout,
		outstm,
		::std::move(callback));
}

} // namespace fast_io::operations::decay

namespace fast_io::details
{

/* coroutine awaiter for async_output_stream_flush_decay */
template <typename scheduler, typename outstmtype>
struct async_output_stream_flush_awaiter : async_awaiter_result<void>
{
	scheduler sched;
	outstmtype outstm;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_output_stream_flush_decay_callback(
			sched, timeout, outstm,
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

/* Coroutine form of async_output_stream_flush: suspends until every
 * pending output byte reached the device; await_resume() rethrows the
 * error through the channel. */
template <typename async_scheduler_type, typename outstmtype>
inline auto async_output_stream_flush_decay(async_scheduler_type scheduler,
											::fast_io::posix_statx_timestamp_opt timeout,
											outstmtype outstm) noexcept
{
	return ::fast_io::details::async_output_stream_flush_awaiter<
		async_scheduler_type, outstmtype>{
		{}, scheduler, outstm, timeout};
}

} // namespace fast_io::operations::decay
