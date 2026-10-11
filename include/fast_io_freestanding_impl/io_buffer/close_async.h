#pragma once

namespace fast_io
{

namespace details::io_buffer
{

/*
 * Buffered async close state: survives the flush leg so the pending
 * output is on the device before the handle's own async close runs —
 * the async analog of basic_io_buffer::close(), which flushes, clears
 * the windows and then closes the handle.
 */
template <typename scheduler, typename io_buffer_type, typename T>
inline void async_iobuffer_close_handle(
	scheduler sched, ::fast_io::basic_io_buffer_ref<io_buffer_type> iobref,
	::fast_io::posix_statx_timestamp_opt timeout, T callback) noexcept;

template <typename scheduler, typename io_buffer_type, typename alloc_type>
struct async_iobuffer_close_state_base
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	scheduler sched;
	::fast_io::basic_io_buffer_ref<io_buffer_type> iobref;
	::fast_io::posix_statx_timestamp_opt timeout;
	/* flush failed -> free the derived state, deliver err; flush done ->
	 * free the state and chain into close_handle with the functor carried
	 * along -- both typed inside the derived state */
	void (*finish)(async_iobuffer_close_state_base *state,
				   ::std::cxx_std_error err) noexcept {};
	void (*proceed)(async_iobuffer_close_state_base *state) noexcept {};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename scheduler, typename io_buffer_type, typename alloc_type, typename T>
struct async_iobuffer_close_state;

template <typename scheduler, typename io_buffer_type, typename alloc_type, typename T>
inline void async_iobuffer_close_finish_cb(
	async_iobuffer_close_state_base<scheduler, io_buffer_type, alloc_type> *base,
	::std::cxx_std_error err) noexcept
{
	auto *state{static_cast<async_iobuffer_close_state<scheduler, io_buffer_type,
													   alloc_type, T> *>(base)};
	auto callback{::std::move(state->callback)};
	::fast_io::details::async_delete_state(state);
	callback(err);
}

template <typename scheduler, typename io_buffer_type, typename alloc_type, typename T>
inline void async_iobuffer_close_proceed_cb(
	async_iobuffer_close_state_base<scheduler, io_buffer_type, alloc_type> *base) noexcept
{
	auto *state{static_cast<async_iobuffer_close_state<scheduler, io_buffer_type,
													   alloc_type, T> *>(base)};
	auto sched{state->sched};
	auto iobref{state->iobref};
	auto timeout{state->timeout};
	auto callback{::std::move(state->callback)};
	::fast_io::details::async_delete_state(state);
	async_iobuffer_close_handle(sched, iobref, timeout, ::std::move(callback));
}

template <typename scheduler, typename io_buffer_type, typename alloc_type, typename T>
struct async_iobuffer_close_state
	: async_iobuffer_close_state_base<scheduler, io_buffer_type, alloc_type>
{
	using base_type =
		async_iobuffer_close_state_base<scheduler, io_buffer_type, alloc_type>;
	T callback;

	inline async_iobuffer_close_state(scheduler s,
									  ::fast_io::basic_io_buffer_ref<io_buffer_type> b,
									  ::fast_io::posix_statx_timestamp_opt tmo, T cb) noexcept
		: base_type{s, b, tmo}, callback{::std::move(cb)}
	{
		this->finish =
			&async_iobuffer_close_finish_cb<scheduler, io_buffer_type, alloc_type, T>;
		this->proceed =
			&async_iobuffer_close_proceed_cb<scheduler, io_buffer_type, alloc_type, T>;
	}
};

/*
 * The close leg, reached once the output window is drained (or when the
 * buffer has no output side). Mirrors basic_io_buffer::close(): clear
 * the window pointers, detach the native handle from the buffer's
 * handle object and hand it to the handle's own async_close_define —
 * which recurses into this same buffered define when the handle is
 * itself an io_buffer.
 */
template <typename scheduler, typename io_buffer_type, typename T>
inline void async_iobuffer_close_handle(
	scheduler sched, ::fast_io::basic_io_buffer_ref<io_buffer_type> iobref,
	::fast_io::posix_statx_timestamp_opt timeout, T callback) noexcept
{
	::fast_io::details::clear_basic_io_buffer_pointers(*iobref.iobptr);
	auto &handle{iobref.iobptr->handle};
	if constexpr (requires { handle.release(); })
	{
		/* owning handle: detach its native handle first, then close the
		 * ref made from it — the ref keeps the handle value across the
		 * release */
		using href_type = ::std::remove_cvref_t<decltype(::fast_io::details::async_close_stream_ref(handle))>;
		async_close_define(sched, timeout, href_type{handle.release()},
						   ::std::move(callback));
	}
	else
	{
		/* non-owning stream member (or a nested io_buffer): its own
		 * ref/define carries the close semantics */
		async_close_define(sched, timeout,
						   ::fast_io::details::async_close_stream_ref(handle),
						   ::std::move(callback));
	}
}

/* the flush leg's completion: failure finishes with the error, success
 * proceeds into the close leg */
template <typename scheduler, typename io_buffer_type, typename alloc_type>
inline void async_iobuffer_close_step(void *vst, ::std::cxx_std_error err) noexcept
{
	auto *state{static_cast<async_iobuffer_close_state_base<scheduler, io_buffer_type, alloc_type> *>(vst)};
	if (err.domain != nullptr) [[unlikely]]
	{
		/* flush failed — sync close() propagates
		 * the flush error and leaves the handle
		 * open; do the same so the caller may
		 * retry or destroy */
		state->finish(state, err);
		return;
	}
	state->proceed(state);
}

} // namespace details::io_buffer

/*
 * async_close_define for basic_io_buffer_ref: the async analog of
 * basic_io_buffer::close(). Pending output is flushed to the handle
 * first; the handle is then detached (release()) and closed through the
 * underlying async_close_define. The buffer allocation itself stays
 * with the object and is freed at destruction, same as sync close().
 * cb(::std::cxx_std_error) — domain == nullptr means the handle is
 * closed; a flush error is reported without closing, matching the sync
 * flush-throw path.
 */
template <typename io_buffer_type, typename scheduler, typename T>
	requires(::fast_io::operations::decay::defines::async_completion_callback<T> &&
			 ::fast_io::operations::decay::defines::has_async_close_define<
				 scheduler,
				 ::std::remove_cvref_t<decltype(::fast_io::details::async_close_stream_ref(
					 ::std::declval<typename io_buffer_type::handle_type &>()))>,
				 T>)
inline void async_close_define(
	scheduler sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_io_buffer_ref<io_buffer_type> iobref, T callback) noexcept
{
	if constexpr ((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) ==
				  ::fast_io::buffer_mode::out)
	{
		auto &obuffer{iobref.iobptr->output_buffer};
		::std::size_t const pending{
			static_cast<::std::size_t>(obuffer.buffer_curr - obuffer.buffer_begin)};
		if (pending != 0)
		{
			/* flush first — the device must see the pending bytes before
			 * the handle goes away, exactly like sync close() */
			using alloc_type = ::fast_io::details::async_scheduler_allocator_t<scheduler>;
			try
			{
				auto *state{::fast_io::details::async_new_state<
					::fast_io::details::io_buffer::async_iobuffer_close_state<
						scheduler, io_buffer_type, alloc_type, ::std::remove_cvref_t<T>>>(
					sched, iobref, timeout, ::std::move(callback))};
				::fast_io::operations::decay::async_pwrite_all_bytes_decay_callback(
					sched, timeout,
					::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
					reinterpret_cast<::std::byte const *>(obuffer.buffer_begin), pending,
					::fast_io::intfpos_opt{},
					::fast_io::details::async_err_step_cb{
						state,
						&::fast_io::details::io_buffer::async_iobuffer_close_step<
							scheduler, io_buffer_type, alloc_type>});
			}
			catch throws(::std::error e)
			{
				callback(e.release());
			}
			return;
		}
	}
	::fast_io::details::io_buffer::async_iobuffer_close_handle(sched, iobref, timeout,
															   ::std::move(callback));
}

} // namespace fast_io
