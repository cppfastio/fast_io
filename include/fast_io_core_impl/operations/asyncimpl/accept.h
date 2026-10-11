#pragma once

namespace fast_io::operations::decay
{

/*
 * async accept: the backend's async_accept_callback_define delivers the
 * accepted socket's native handle through cb(::std::cxx_std_error,
 * native_handle_type). mode describes the accepted socket —
 * open_mode::no_block marks it async-capable (overlapped machinery on
 * win32; posix backends need nothing from it). The functor is invoked
 * once and must be noexcept.
 */
template <typename async_scheduler_type, typename streamtype, typename callback_type>
	requires ::fast_io::operations::decay::defines::has_async_accept_callback_define<
		async_scheduler_type, streamtype, callback_type>
inline void async_accept_decay_callback(async_scheduler_type scheduler,
										::fast_io::posix_statx_timestamp_opt timeout,
										streamtype listenstm, ::fast_io::open_mode mode,
										callback_type callback) noexcept
{
	async_accept_callback_define(scheduler, timeout, listenstm, mode, ::std::move(callback));
}

} // namespace fast_io::operations::decay

namespace fast_io::details
{

/* coroutine awaiter for async_accept_decay — await_resume returns the
 * accepted socket as its owning file type (native_socket_file): no
 * handle ever escapes, so nothing can leak it */
template <typename scheduler, typename streamtype>
struct async_accept_awaiter
	: async_awaiter_result<typename streamtype::native_handle_type>
{
	using file_type = ::fast_io::operations::decay::defines::async_accept_file_t<streamtype>;
	scheduler sched;
	streamtype listenstm;
	::fast_io::open_mode mode;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_accept_decay_callback(
			sched, timeout, listenstm, mode,
			::fast_io::details::async_awaiter_callback<typename streamtype::native_handle_type>{this});
		return this->async_suspend_done();
	}
	inline file_type await_resume() FAST_IO_HERBCEPTIONS_THROWS
	{
		if (this->err.domain != nullptr)
		{
			auto e{this->err};
			this->err = {};
			throw throws e;
		}
		return file_type{this->value};
	}
};

} // namespace fast_io::details

namespace fast_io::operations::decay
{

template <typename async_scheduler_type, typename streamtype>
inline auto async_accept_decay(async_scheduler_type scheduler,
							   ::fast_io::posix_statx_timestamp_opt timeout, streamtype listenstm,
							   ::fast_io::open_mode mode) noexcept
{
	return ::fast_io::details::async_accept_awaiter<async_scheduler_type, streamtype>{
		{}, scheduler, listenstm, mode, timeout};
}

} // namespace fast_io::operations::decay
