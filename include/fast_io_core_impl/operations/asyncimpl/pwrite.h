#pragma once

namespace fast_io::details
{

/* Shared state for the async_pwrite_all_bytes chain: one allocation lives
 * across every partial-write resubmission and is freed when the user's
 * callback is finally invoked. */
template <typename scheduler, typename outstmtype, typename alloc_type>
struct async_pwrite_all_bytes_state_base
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	scheduler sched;
	outstmtype outstm;
	::std::byte const *first;
	::std::size_t remaining;
	::fast_io::intfpos_opt off;
	::fast_io::posix_statx_timestamp_opt timeout;
	void (*finish)(async_pwrite_all_bytes_state_base *state,
				   ::std::cxx_std_error err) noexcept {};
	/* status allocator handle copied from the scheduler at submission so
	 * the state can be freed without it; kept last to preserve the
	 * aggregate initialization order */
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename scheduler, typename outstmtype, typename alloc_type, typename T>
struct async_pwrite_all_bytes_state;

template <typename scheduler, typename outstmtype, typename alloc_type, typename T>
inline void async_pwrite_all_bytes_finish_cb(
	async_pwrite_all_bytes_state_base<scheduler, outstmtype, alloc_type> *b,
	::std::cxx_std_error err) noexcept
{
	auto *state{static_cast<async_pwrite_all_bytes_state<scheduler, outstmtype, alloc_type, T> *>(b)};
	auto callback{::std::move(state->callback)};
	::fast_io::details::async_delete_state(state);
	callback(err);
}

template <typename scheduler, typename outstmtype, typename alloc_type, typename T>
struct async_pwrite_all_bytes_state
	: async_pwrite_all_bytes_state_base<scheduler, outstmtype, alloc_type>
{
	using base_type = async_pwrite_all_bytes_state_base<scheduler, outstmtype, alloc_type>;
	T callback;

	inline async_pwrite_all_bytes_state(scheduler s, outstmtype o, ::std::byte const *f,
										::std::size_t rem, ::fast_io::intfpos_opt of,
										::fast_io::posix_statx_timestamp_opt tmo, T cb) noexcept
		: base_type{s, o, f, rem, of, tmo}, callback{::std::move(cb)}
	{
		this->finish = &async_pwrite_all_bytes_finish_cb<scheduler, outstmtype, alloc_type, T>;
	}
};

/*
 * One round of the pwrite_all chain: submit a pwrite_some for the
 * remaining bytes; the backend callback receives
 * (err, bytes_written_this_round). On error or completion the user
 * callback runs once as callback(::std::cxx_std_error) noexcept and the
 * state frees itself; otherwise resubmit. A zero-byte write would loop
 * forever, so it is delivered as errc::no_space_on_device.
 */
template <typename scheduler, typename outstmtype, typename alloc_type>
inline void async_pwrite_all_bytes_step(void *vst, ::std::cxx_std_error err,
										::std::size_t byteswritten) noexcept
{
	auto *state{static_cast<async_pwrite_all_bytes_state_base<scheduler, outstmtype, alloc_type> *>(vst)};
	if (err.domain != nullptr) [[unlikely]]
	{
		state->finish(state, err);
		return;
	}
	state->first += byteswritten;
	state->remaining -= byteswritten;
	if (state->off.has_opt)
	{
		state->off.opt = ::fast_io::fposoffadd_nonegative(
			state->off.opt, static_cast<::std::ptrdiff_t>(byteswritten));
	}
	if (state->remaining == 0)
	{
		state->finish(state, {});
		return;
	}
	if (byteswritten == 0) [[unlikely]]
	{
		state->finish(state, ::fast_io::details::async_make_error(
								 ::std::errc::no_space_on_device));
		return;
	}
	async_pwrite_all_bytes_submit(state);
}

template <typename scheduler, typename outstmtype, typename alloc_type>
inline void async_pwrite_all_bytes_submit(
	async_pwrite_all_bytes_state_base<scheduler, outstmtype, alloc_type> *state) noexcept
{
	async_pwrite_some_bytes_overflow_callback_define(
		state->sched, state->timeout, state->outstm, state->first, state->remaining, state->off,
		async_bytes_step_cb{state, &async_pwrite_all_bytes_step<scheduler, outstmtype, alloc_type>});
}

} // namespace fast_io::details

namespace fast_io::operations::decay
{

/*
 * Public entry point for a single async positional write: submits the
 * backend's async_pwrite_some_bytes_overflow_callback_define, which is
 * found by ADL on the scheduler/stream types — backends implement only
 * the define and callers only ever name this. outstm is reduced to its
 * output_stream_ref first, so buffered streams arrive as
 * basic_io_buffer_ref and their buffer-aware define runs instead of the
 * backend's.
 *
 * The functor is invoked once as
 * callback(::std::cxx_std_error, ::std::size_t) noexcept: the size is the
 * bytes accepted this round — for a buffered stream that may mean they
 * went into the buffer without touching the device. The stream object
 * must outlive the operation.
 */
template <typename async_scheduler_type, typename outstmtype, typename callback_type>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
				 ::std::remove_cvref_t<callback_type>> &&
			 ::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 async_scheduler_type,
					 outstmtype,
					 ::std::remove_cvref_t<callback_type>>
inline void async_pwrite_some_bytes_decay_callback(async_scheduler_type scheduler,
												   ::fast_io::posix_statx_timestamp_opt timeout,
												   outstmtype outstm, ::std::byte const *first,
												   ::std::size_t count, ::fast_io::intfpos_opt off,
												   callback_type callback) noexcept
{
	async_pwrite_some_bytes_overflow_callback_define(
		scheduler, timeout,
		outstm,
		first, count, off,
		::std::move(callback));
}

/*
 * write_all built on async_pwrite_some_bytes_decay_callback: partial
 * writes are resubmitted until every byte is written. The functor is
 * invoked once as callback(::std::cxx_std_error) noexcept:
 * err.domain == nullptr means every byte was written, otherwise domain
 * and code describe the failure. `timeout` is forwarded unchanged
 * to every resubmission.
 */
template <typename async_scheduler_type, typename outstmtype, typename callback_type>
	requires ::std::is_nothrow_invocable_v<callback_type, ::std::cxx_std_error> &&
			 ::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 async_scheduler_type,
					 outstmtype,
					 ::fast_io::details::async_io_callback>
inline void async_pwrite_all_bytes_decay_callback(async_scheduler_type scheduler,
												  ::fast_io::posix_statx_timestamp_opt timeout,
												  outstmtype outstm, ::std::byte const *first,
												  ::std::size_t count, ::fast_io::intfpos_opt off,
												  callback_type callback) noexcept
{
	using outstm_reftype = outstmtype;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>;
	using state_type =
		::fast_io::details::async_pwrite_all_bytes_state<async_scheduler_type, outstm_reftype,
														 alloc_type,
														 ::std::remove_cvref_t<callback_type>>;
	::std::cxx_std_error err{};

	if (count) [[likely]]
	{
		try
		{
			::fast_io::details::async_pwrite_all_bytes_submit(
				::fast_io::details::async_new_state<state_type>(
					scheduler,
					outstm,
					first, count, off, timeout, ::std::move(callback)));
			return;
		}
		catch throws(::std::error e)
		{
			err = e.release();
		}
	}
	callback(err);
}

} // namespace fast_io::operations::decay

namespace fast_io::details
{

/* coroutine awaiter for async_pwrite_some_bytes_decay */
template <typename scheduler, typename outstmtype>
struct async_pwrite_some_bytes_awaiter : async_awaiter_result<::std::size_t>
{
	scheduler sched;
	outstmtype outstm;
	::std::byte const *first;
	::std::size_t count;
	::fast_io::intfpos_opt off;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_pwrite_some_bytes_decay_callback(
			sched, timeout, outstm, first, count, off,
			::fast_io::details::async_awaiter_callback<::std::size_t>{this});
		return this->async_suspend_done();
	}
	inline ::std::size_t await_resume() FAST_IO_HERBCEPTIONS_THROWS
	{
		if (this->err.domain != nullptr)
		{
			auto e{this->err};
			this->err = {};
			throw throws e;
		}
		return this->value;
	}
};

/* coroutine awaiter for async_pwrite_all_bytes_decay */
template <typename scheduler, typename outstmtype>
struct async_pwrite_all_bytes_awaiter : async_awaiter_result<void>
{
	scheduler sched;
	outstmtype outstm;
	::std::byte const *first;
	::std::size_t count;
	::fast_io::intfpos_opt off;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_pwrite_all_bytes_decay_callback(
			sched, timeout, outstm, first, count, off,
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

/* Coroutine forms: co_await submits the operation and suspends until the
 * completion runs. await_resume() rethrows the error through the channel;
 * the some form yields the bytes written. */
template <typename async_scheduler_type, typename outstmtype>
inline auto async_pwrite_some_bytes_decay(async_scheduler_type scheduler,
										  ::fast_io::posix_statx_timestamp_opt timeout,
										  outstmtype outstm, ::std::byte const *first,
										  ::std::size_t count,
										  ::fast_io::intfpos_opt off) noexcept
{
	return ::fast_io::details::async_pwrite_some_bytes_awaiter<
		async_scheduler_type, outstmtype>{
		{}, scheduler, outstm, first, count, off, timeout};
}

template <typename async_scheduler_type, typename outstmtype>
inline auto async_pwrite_all_bytes_decay(async_scheduler_type scheduler,
										 ::fast_io::posix_statx_timestamp_opt timeout,
										 outstmtype outstm, ::std::byte const *first,
										 ::std::size_t count,
										 ::fast_io::intfpos_opt off) noexcept
{
	return ::fast_io::details::async_pwrite_all_bytes_awaiter<
		async_scheduler_type, outstmtype>{
		{}, scheduler, outstm, first, count, off, timeout};
}

} // namespace fast_io::operations::decay
