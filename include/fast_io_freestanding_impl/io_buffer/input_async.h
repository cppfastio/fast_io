#pragma once

namespace fast_io
{

namespace details::io_buffer
{

/*
 * Buffered-stream async input machinery — the async analog of the sync
 * read_some/pread_some underflow defines on basic_io_buffer_ref.
 *
 * Ordering rule shared by every input path below: on an in|out|tie buffer
 * pending output must reach the device before an input op waits on the
 * peer (a prompt must be sent before the answer can arrive). The sync
 * defines enforce that with a blocking output_stream_buffer_flush_define;
 * here it is an async pwrite_all chain on the handle, after which the
 * state resubmits itself.
 *
 * `output_flushed` records that the flush stage already ran so a
 * resubmission does not loop on it.
 */
template <typename scheduler, typename io_buffer_type, typename alloc_type>
struct async_iobuffer_pread_state_base
{
	using allocator_type = alloc_type;
	using io_buffer_type_t = io_buffer_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	scheduler sched;
	::fast_io::basic_io_buffer_ref<io_buffer_type> iobref;
	::std::byte *first;
	::std::size_t count;
	::fast_io::intfpos_opt off;
	::fast_io::posix_statx_timestamp_opt timeout;
	/* finish: free the derived state, invoke the functor (err, n);
	 * resume: re-enter the submit after a tied-output flush;
	 * relay: free the state and hand the functor to the handle's own
	 * async define -- all typed inside the derived state */
	void (*finish)(async_iobuffer_pread_state_base *state, ::std::cxx_std_error err,
				   ::std::size_t n) noexcept {};
	void (*resume)(async_iobuffer_pread_state_base *state) noexcept {};
	void (*relay)(async_iobuffer_pread_state_base *state, bool positional) noexcept {};
	bool output_flushed{};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename scheduler, typename io_buffer_type, typename alloc_type>
inline void async_iobuffer_pread_submit(
	async_iobuffer_pread_state_base<scheduler, io_buffer_type, alloc_type> *state) noexcept;

template <typename scheduler, typename io_buffer_type, typename alloc_type, typename T>
struct async_iobuffer_pread_state;

template <typename scheduler, typename io_buffer_type, typename alloc_type, typename T>
inline void async_iobuffer_pread_finish_cb(
	async_iobuffer_pread_state_base<scheduler, io_buffer_type, alloc_type> *base,
	::std::cxx_std_error err, ::std::size_t n) noexcept
{
	auto *state{static_cast<async_iobuffer_pread_state<scheduler, io_buffer_type,
													   alloc_type, T> *>(base)};
	auto callback{::std::move(state->callback)};
	::fast_io::details::async_delete_state(state);
	callback(err, n);
}

template <typename scheduler, typename io_buffer_type, typename alloc_type, typename T>
inline void async_iobuffer_pread_relay_cb(
	async_iobuffer_pread_state_base<scheduler, io_buffer_type, alloc_type> *base,
	bool positional) noexcept
{
	auto *state{static_cast<async_iobuffer_pread_state<scheduler, io_buffer_type,
													   alloc_type, T> *>(base)};
	auto sched{state->sched};
	auto instm{::fast_io::operations::input_stream_ref(state->iobref.iobptr->handle)};
	auto first{state->first};
	auto count{state->count};
	auto off{positional ? state->off : ::fast_io::intfpos_opt{}};
	auto timeout{state->timeout};
	auto callback{::std::move(state->callback)};
	::fast_io::details::async_delete_state(state);
	async_pread_some_bytes_underflow_callback_define(sched, timeout, instm, first, count,
													 off, ::std::move(callback));
}

template <typename scheduler, typename io_buffer_type, typename alloc_type, typename T>
struct async_iobuffer_pread_state
	: async_iobuffer_pread_state_base<scheduler, io_buffer_type, alloc_type>
{
	using base_type =
		async_iobuffer_pread_state_base<scheduler, io_buffer_type, alloc_type>;
	T callback;

	inline async_iobuffer_pread_state(
		scheduler s, ::fast_io::basic_io_buffer_ref<io_buffer_type> b,
		::std::byte *f, ::std::size_t c, ::fast_io::intfpos_opt o,
		::fast_io::posix_statx_timestamp_opt tmo, T cb) noexcept
		: base_type{s, b, f, c, o, tmo}, callback{::std::move(cb)}
	{
		this->resume = &async_iobuffer_pread_submit<scheduler, io_buffer_type, alloc_type>;
		this->finish =
			&async_iobuffer_pread_finish_cb<scheduler, io_buffer_type, alloc_type, T>;
		this->relay =
			&async_iobuffer_pread_relay_cb<scheduler, io_buffer_type, alloc_type, T>;
	}
};

template <typename state_type>
inline void async_iobuffer_pread_deliver(state_type *state, ::std::cxx_std_error err,
										 ::std::size_t n) noexcept
{
	state->finish(state, err, n);
}

/* whether the traits mark this buffer in|out|tie */
template <typename io_buffer_type>
inline constexpr bool async_iobuffer_tied{
	(io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) ==
		::fast_io::buffer_mode::out &&
	(io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::tie) ==
		::fast_io::buffer_mode::tie};

/* whether this op must suspend on a tied-output flush first */
template <typename io_buffer_type, typename iobref_type>
inline constexpr bool async_iobuffer_output_pending(iobref_type iobref) noexcept
{
	if constexpr (async_iobuffer_tied<io_buffer_type>)
	{
		auto &obuffer{iobref.iobptr->output_buffer};
		return obuffer.buffer_curr != obuffer.buffer_begin;
	}
	else
	{
		return false;
	}
}

/* submit the tied-output flush leg: pwrite_all the pending obuffer bytes
 * on the handle, then re-enter the state machine */
template <typename state_type>
inline void async_iobuffer_flush_step(void *vst, ::std::cxx_std_error err) noexcept
{
	auto *state{static_cast<state_type *>(vst)};
	if (err.domain == nullptr)
	{
		auto &ob{state->iobref.iobptr->output_buffer};
		ob.buffer_curr = ob.buffer_begin;
	}
	if (err.domain != nullptr) [[unlikely]]
	{
		state->finish(state, err, 0);
		return;
	}
	state->resume(state);
}

template <typename state_type>
inline void async_iobuffer_flush_submit(state_type *state) noexcept
{
	auto iobref{state->iobref};
	auto &obuffer{iobref.iobptr->output_buffer};
	state->output_flushed = true;
	::fast_io::operations::decay::async_pwrite_all_bytes_decay_callback(
		state->sched, state->timeout,
		::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
		reinterpret_cast<::std::byte const *>(obuffer.buffer_begin),
		static_cast<::std::size_t>(obuffer.buffer_curr - obuffer.buffer_begin) *
			sizeof(typename state_type::io_buffer_type_t::output_char_type),
		::fast_io::intfpos_opt{},
		::fast_io::details::async_err_step_cb{
			state, &async_iobuffer_flush_step<state_type>});
}

/* the dispatch behind the async pread define's slow path */
template <typename scheduler, typename io_buffer_type, typename alloc_type>
inline void async_iobuffer_pread_step(void *vst, ::std::cxx_std_error err,
									  ::std::size_t got) noexcept
{
	auto *state{static_cast<async_iobuffer_pread_state_base<scheduler, io_buffer_type, alloc_type> *>(vst)};
	if (err.domain != nullptr) [[unlikely]]
	{
		async_iobuffer_pread_deliver(state, err, 0);
		return;
	}
	auto &ibuffer{state->iobref.iobptr->input_buffer};
	ibuffer.buffer_curr = ibuffer.buffer_begin;
	ibuffer.buffer_end = ibuffer.buffer_begin + got;
	::std::size_t const n{got < state->count ? got : state->count};
	::fast_io::details::non_overlapped_copy_n(
		reinterpret_cast<::std::byte const *>(ibuffer.buffer_begin), n, state->first);
	ibuffer.buffer_curr += n;
	async_iobuffer_pread_deliver(state, ::std::cxx_std_error{}, n);
}

template <typename scheduler, typename io_buffer_type, typename alloc_type>
inline void async_iobuffer_pread_submit(
	async_iobuffer_pread_state_base<scheduler, io_buffer_type, alloc_type> *state) noexcept
{
	if constexpr (async_iobuffer_tied<io_buffer_type>)
	{
		if (!state->output_flushed &&
			state->iobref.iobptr->output_buffer.buffer_curr !=
				state->iobref.iobptr->output_buffer.buffer_begin)
		{
			async_iobuffer_flush_submit(state);
			return;
		}
	}
	auto iobref{state->iobref};
	if (state->off.has_opt)
	{
		/* explicit offset: the buffer window is unrelated to it — delegate
		 * to the handle, matching the sync pread underflow define */
		state->relay(state, true);
		return;
	}
	try
	{
		auto &ibuffer{iobref.iobptr->input_buffer};
		using char_type = typename io_buffer_type::input_char_type;
		constexpr ::std::size_t bfsz{io_buffer_type::traits_type::input_buffer_size};
		::std::size_t const pending{
			static_cast<::std::size_t>(ibuffer.buffer_end - ibuffer.buffer_curr)};
		if (pending != 0)
		{
			/* still drained after the flush: serve from the window */
			::std::size_t const n{pending < state->count ? pending : state->count};
			::fast_io::details::non_overlapped_copy_n(
				reinterpret_cast<::std::byte const *>(ibuffer.buffer_curr), n, state->first);
			ibuffer.buffer_curr += n;
			async_iobuffer_pread_deliver(state, ::std::cxx_std_error{}, n);
			return;
		}
		if (state->count >= bfsz)
		{
			/* a read covering the whole buffer talks to the device
			 * directly — the buffer stays empty */
			state->relay(state, false);
			return;
		}
		/* underflow: refill the window asynchronously, then serve the
		 * caller's range from what arrived */
		if (ibuffer.buffer_begin == nullptr)
		{
			ibuffer.buffer_end = ibuffer.buffer_curr = ibuffer.buffer_begin =
				::fast_io::details::io_buffer::iobuffer_allocate<char_type,
																 typename io_buffer_type::traits_type::allocator_type>(
					iobref.iobptr->allocator_handle, bfsz);
		}
		async_pread_some_bytes_underflow_callback_define(
			state->sched, state->timeout,
			::fast_io::operations::input_stream_ref(iobref.iobptr->handle),
			reinterpret_cast<::std::byte *>(ibuffer.buffer_begin), bfsz, ::fast_io::intfpos_opt{},
			::fast_io::details::async_bytes_step_cb{
				state, &async_iobuffer_pread_step<scheduler, io_buffer_type, alloc_type>});
	}
	catch throws(::std::error e)
	{
		async_iobuffer_pread_deliver(state, e.release(), 0);
	}
}

/*
 * State for async_ibuffer_underflow: refill the input window with one
 * async read on the handle and report how many bytes became pending.
 */
template <typename scheduler, typename io_buffer_type, typename alloc_type>
struct async_iobuffer_underflow_state_base
{
	using allocator_type = alloc_type;
	using io_buffer_type_t = io_buffer_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	scheduler sched;
	::fast_io::basic_io_buffer_ref<io_buffer_type> iobref;
	::fast_io::posix_statx_timestamp_opt timeout;
	void (*finish)(async_iobuffer_underflow_state_base *state, ::std::cxx_std_error err,
				   ::std::size_t n) noexcept {};
	void (*resume)(async_iobuffer_underflow_state_base *state) noexcept {};
	bool output_flushed{};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename scheduler, typename io_buffer_type, typename alloc_type>
inline void async_iobuffer_underflow_submit(
	async_iobuffer_underflow_state_base<scheduler, io_buffer_type, alloc_type> *state) noexcept;

template <typename scheduler, typename io_buffer_type, typename alloc_type, typename T>
struct async_iobuffer_underflow_state;

template <typename scheduler, typename io_buffer_type, typename alloc_type, typename T>
inline void async_iobuffer_underflow_finish_cb(
	async_iobuffer_underflow_state_base<scheduler, io_buffer_type, alloc_type> *base,
	::std::cxx_std_error err, ::std::size_t n) noexcept
{
	auto *state{static_cast<async_iobuffer_underflow_state<scheduler, io_buffer_type,
														   alloc_type, T> *>(base)};
	auto callback{::std::move(state->callback)};
	::fast_io::details::async_delete_state(state);
	callback(err, n);
}

template <typename scheduler, typename io_buffer_type, typename alloc_type, typename T>
struct async_iobuffer_underflow_state
	: async_iobuffer_underflow_state_base<scheduler, io_buffer_type, alloc_type>
{
	using base_type =
		async_iobuffer_underflow_state_base<scheduler, io_buffer_type, alloc_type>;
	T callback;

	inline async_iobuffer_underflow_state(
		scheduler s, ::fast_io::basic_io_buffer_ref<io_buffer_type> b,
		::fast_io::posix_statx_timestamp_opt tmo, T cb) noexcept
		: base_type{s, b, tmo}, callback{::std::move(cb)}
	{
		this->resume = &async_iobuffer_underflow_submit<scheduler, io_buffer_type, alloc_type>;
		this->finish =
			&async_iobuffer_underflow_finish_cb<scheduler, io_buffer_type, alloc_type, T>;
	}
};

template <typename state_type>
inline void async_iobuffer_underflow_deliver(state_type *state,
											 ::std::cxx_std_error err, ::std::size_t n) noexcept
{
	state->finish(state, err, n);
}

template <typename scheduler, typename io_buffer_type, typename alloc_type>
inline void async_iobuffer_underflow_step(void *vst, ::std::cxx_std_error err,
										  ::std::size_t got) noexcept
{
	auto *state{static_cast<async_iobuffer_underflow_state_base<scheduler, io_buffer_type, alloc_type> *>(vst)};
	if (err.domain == nullptr)
	{
		auto &ibuffer{state->iobref.iobptr->input_buffer};
		ibuffer.buffer_curr = ibuffer.buffer_begin;
		ibuffer.buffer_end = ibuffer.buffer_begin + got;
	}
	async_iobuffer_underflow_deliver(state, err, got);
}

template <typename scheduler, typename io_buffer_type, typename alloc_type>
inline void async_iobuffer_underflow_submit(
	async_iobuffer_underflow_state_base<scheduler, io_buffer_type, alloc_type> *state) noexcept
{
	if constexpr (async_iobuffer_tied<io_buffer_type>)
	{
		if (!state->output_flushed &&
			state->iobref.iobptr->output_buffer.buffer_curr !=
				state->iobref.iobptr->output_buffer.buffer_begin)
		{
			async_iobuffer_flush_submit(state);
			return;
		}
	}
	auto iobref{state->iobref};
	try
	{
		auto &ibuffer{iobref.iobptr->input_buffer};
		using char_type = typename io_buffer_type::input_char_type;
		constexpr ::std::size_t bfsz{io_buffer_type::traits_type::input_buffer_size};
		if (ibuffer.buffer_begin == nullptr)
		{
			ibuffer.buffer_end = ibuffer.buffer_curr = ibuffer.buffer_begin =
				::fast_io::details::io_buffer::iobuffer_allocate<char_type,
																 typename io_buffer_type::traits_type::allocator_type>(
					iobref.iobptr->allocator_handle, bfsz);
		}
		async_pread_some_bytes_underflow_callback_define(
			state->sched, state->timeout,
			::fast_io::operations::input_stream_ref(iobref.iobptr->handle),
			reinterpret_cast<::std::byte *>(ibuffer.buffer_begin), bfsz, ::fast_io::intfpos_opt{},
			::fast_io::details::async_bytes_step_cb{
				state, &async_iobuffer_underflow_step<scheduler, io_buffer_type, alloc_type>});
	}
	catch throws(::std::error e)
	{
		async_iobuffer_underflow_deliver(state, e.release(), 0);
	}
}

} // namespace details::io_buffer

/*
 * async_ibuffer_underflow(sched, timeout, iobref, cb): the async form of
 * ibuffer_underflow — refill the input window with one async read on the
 * handle. cb(err, n) reports the bytes made pending; n == 0 on EOF.
 * Flushes a tied output buffer first, like the sync form.
 */
template <typename io_buffer_type, typename scheduler, typename T>
	requires((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::in) ==
				 ::fast_io::buffer_mode::in &&
			 sizeof(typename io_buffer_type::input_char_type) == 1 &&
			 ::fast_io::operations::decay::defines::async_bytes_completion_callback<T>)
inline void async_ibuffer_underflow(scheduler sched,
									::fast_io::posix_statx_timestamp_opt timeout,
									::fast_io::basic_io_buffer_ref<io_buffer_type> iobref,
									T callback) noexcept
{
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<scheduler>;
	try
	{
		::fast_io::details::io_buffer::async_iobuffer_underflow_submit(
			::fast_io::details::async_new_state<
				::fast_io::details::io_buffer::async_iobuffer_underflow_state<
					scheduler, io_buffer_type, alloc_type, ::std::remove_cvref_t<T>>>(
				sched, iobref, timeout, ::std::move(callback)));
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0);
	}
}

/*
 * Buffered async pread define: for a current-position read, pending
 * window bytes are served inline with a copy — the async call reports
 * them immediately without touching the device. An empty buffer issues an
 * underflow refill when count fits the buffer, or talks to the handle
 * directly when it does not. An explicit offset delegates to the handle
 * without disturbing the window — the same dispatch the sync pread
 * underflow define performs. On a tied in|out buffer, pending output is
 * flushed asynchronously before any of the read paths run.
 */
template <typename io_buffer_type, typename scheduler, typename T>
	requires((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::in) ==
				 ::fast_io::buffer_mode::in &&
			 sizeof(typename io_buffer_type::input_char_type) == 1 &&
			 ::fast_io::operations::decay::defines::async_bytes_completion_callback<T>)
inline void async_pread_some_bytes_underflow_callback_define(
	scheduler sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_io_buffer_ref<io_buffer_type> iobref, ::std::byte *first,
	::std::size_t count, ::fast_io::intfpos_opt off, T callback) noexcept
{
	if (count == 0)
	{
		callback(::std::cxx_std_error{}, 0);
		return;
	}
	constexpr ::std::size_t bfsz{io_buffer_type::traits_type::input_buffer_size};
	if (!::fast_io::details::io_buffer::async_iobuffer_output_pending<io_buffer_type>(iobref))
	{
		if (off.has_opt)
		{
			async_pread_some_bytes_underflow_callback_define(
				sched, timeout, ::fast_io::operations::input_stream_ref(iobref.iobptr->handle),
				first, count, off, ::std::move(callback));
			return;
		}
		auto &ibuffer{iobref.iobptr->input_buffer};
		::std::size_t const pending{
			static_cast<::std::size_t>(ibuffer.buffer_end - ibuffer.buffer_curr)};
		if (pending != 0)
		{
			::std::size_t const n{pending < count ? pending : count};
			::fast_io::details::non_overlapped_copy_n(
				reinterpret_cast<::std::byte const *>(ibuffer.buffer_curr), n, first);
			ibuffer.buffer_curr += n;
			callback(::std::cxx_std_error{}, n);
			return;
		}
		if (count >= bfsz)
		{
			async_pread_some_bytes_underflow_callback_define(
				sched, timeout, ::fast_io::operations::input_stream_ref(iobref.iobptr->handle),
				first, count, off, ::std::move(callback));
			return;
		}
	}
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<scheduler>;
	try
	{
		::fast_io::details::io_buffer::async_iobuffer_pread_submit(
			::fast_io::details::async_new_state<
				::fast_io::details::io_buffer::async_iobuffer_pread_state<
					scheduler, io_buffer_type, alloc_type, ::std::remove_cvref_t<T>>>(
				sched, iobref, first, count, off, timeout, ::std::move(callback)));
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0);
	}
}

} // namespace fast_io
