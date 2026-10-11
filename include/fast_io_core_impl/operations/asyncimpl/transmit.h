#pragma once

namespace fast_io::details
{

/*
 * Generic async transmit emulation state: a bounce buffer lives in the
 * state object; each round submits an async_pread_some into the buffer,
 * then pwrites everything that was read out the other side. Native
 * transmit (e.g. io_uring SPLICE) bypasses this entirely through
 * async_transmit_some_bytes_overflow_underflow_callback_define.
 */
inline constexpr ::std::size_t async_transmit_bounce_size{
	::fast_io::details::transmit_buffer_size_cache<1>};

template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
struct async_transmit_bytes_state_base
{
	static inline constexpr bool transmit_all{all};
	using allocator_type = alloc_type;
	using scheduler_type = scheduler;
	using instm_type = instmtype;
	using outstm_type = outstmtype;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	scheduler sched;
	outstmtype outstm;
	::fast_io::intfpos_opt off_out;
	instmtype instm;
	::fast_io::intfpos_opt off_in;
	/* bytes still permitted by the caller's bound; SIZE_MAX for "until EOF" */
	::std::size_t remaining;
	::fast_io::posix_statx_timestamp_opt timeout;
	/* staged: bytes read into buf during the current round; moved: total
	 * bytes delivered to the output; round_base: moved at round start, so
	 * moved - round_base is how much of the staged bytes was written */
	::std::size_t staged;
	::std::size_t moved;
	::std::size_t round_base;
	::std::byte buf[async_transmit_bounce_size];
	/* terminal delivery: frees the (derived) state, invokes the stored
	 * functor -- the pump machinery never sees the callback type */
	void (*finish)(async_transmit_bytes_state_base *state, ::std::cxx_std_error err,
				   ::std::size_t moved) noexcept {};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

/* the callback-carrying state: the pump's round/completion functions work
 * on the base, so they emit once per (all, scheduler, out, in, alloc)
 * instead of once per functor */
template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type, typename T>
struct async_transmit_bytes_state;

template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type, typename T>
inline void async_transmit_bytes_finish_cb(
	async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *b,
	::std::cxx_std_error err, ::std::size_t moved) noexcept
{
	auto *state{static_cast<
		async_transmit_bytes_state<all, scheduler, outstmtype, instmtype, alloc_type, T> *>(b)};
	auto callback{::std::move(state->callback)};
	::fast_io::details::async_delete_state(state);
	if constexpr (all)
	{
		callback(err);
	}
	else
	{
		callback(err, moved);
	}
}

template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type, typename T>
struct async_transmit_bytes_state
	: async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type>
{
	using base_type =
		async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type>;
	T callback;

	inline async_transmit_bytes_state(
		scheduler s, outstmtype o, ::fast_io::intfpos_opt oo, instmtype i,
		::fast_io::intfpos_opt oi, ::std::size_t rem,
		::fast_io::posix_statx_timestamp_opt tmo, T cb, ::std::size_t staged,
		::std::size_t moved, ::std::size_t round_base) noexcept
		: base_type{s, o, oo, i, oi, rem, tmo, staged, moved, round_base, {}},
		  callback{::std::move(cb)}
	{
		this->finish =
			&async_transmit_bytes_finish_cb<all, scheduler, outstmtype, instmtype, alloc_type, T>;
	}
};

template <typename state_type>
inline void async_transmit_bytes_deliver(state_type *state, ::std::cxx_std_error err,
										 ::std::size_t moved) noexcept
{
	state->finish(state, err, moved);
}

template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
inline void async_transmit_bytes_write_staged(async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *state) noexcept;

template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
inline void async_transmit_bytes_read_round(async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *state) noexcept;

/*
 * Shared read-round completion: advance off_in, stage the bytes and chain
 * into the write side; a zero-byte read is EOF and delivers. Used both as
 * the backend callback and as the inline completion for inputs that only
 * offer synchronous reads (e.g. a memory source — the sync call runs inside
 * the submit lambda so a throw is still converted by async_submit_catching).
 */
template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
inline void async_transmit_bytes_read_done(async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *state, ::std::cxx_std_error err,
										   ::std::size_t bytesread) noexcept
{
	if (err.domain == nullptr)
	{
		if (state->off_in.has_opt)
		{
			state->off_in.opt = ::fast_io::fposoffadd_nonegative(
				state->off_in.opt, static_cast<::std::ptrdiff_t>(bytesread));
		}
		if (bytesread != 0)
		{
			state->staged = bytesread;
			async_transmit_bytes_write_staged(state);
			return;
		}
	}
	::fast_io::details::async_transmit_bytes_deliver(state, err, state->moved);
}

/*
 * Shared write-round completion: account the bytes, resubmit on short
 * writes, and for transmit_all chain back into a read round. A zero-byte
 * partial write reports no_space_on_device.
 */
template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
inline void async_transmit_bytes_write_done(async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *state, ::std::cxx_std_error err,
											::std::size_t byteswritten) noexcept
{
	if (err.domain == nullptr)
	{
		state->moved += byteswritten;
		state->remaining -= byteswritten;
		if (state->off_out.has_opt)
		{
			state->off_out.opt = ::fast_io::fposoffadd_nonegative(
				state->off_out.opt, static_cast<::std::ptrdiff_t>(byteswritten));
		}
		if (state->moved - state->round_base != state->staged)
		{
			if (byteswritten == 0) [[unlikely]]
			{
				::fast_io::details::async_transmit_bytes_deliver(
					state,
					::fast_io::details::async_make_error(
						::std::errc::no_space_on_device),
					state->moved);
				return;
			}
			async_transmit_bytes_write_staged(state);
			return;
		}
		if constexpr (all)
		{
			if (state->remaining != 0)
			{
				state->round_base = state->moved;
				async_transmit_bytes_read_round(state);
				return;
			}
		}
	}
	::fast_io::details::async_transmit_bytes_deliver(state, err, state->moved);
}

/*
 * Synchronous read for inputs without an async pread define: pread_some
 * when an explicit offset was requested (streams that cannot pread report
 * invalid_seek, matching the sync transmit emulation), read_some
 * otherwise. A throw is delivered through the done path like any backend
 * error — it never escapes the noexcept state machine.
 */
template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
inline void async_transmit_bytes_sync_read(async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *state, ::std::size_t chunk) noexcept
{
	try
	{
		::std::byte *iter;
		if (state->off_in.has_opt)
		{
			if constexpr (::fast_io::operations::decay::defines::bytes_preadable<instmtype>)
			{
				iter = ::fast_io::operations::decay::pread_some_bytes_decay(
					state->instm, state->buf, chunk, state->off_in.opt);
			}
			else
			{
				::fast_io::herbceptions::throws_errc(::std::errc::invalid_seek);
			}
		}
		else
		{
			if constexpr (::fast_io::operations::decay::defines::bytes_readable<instmtype>)
			{
				iter = ::fast_io::operations::decay::read_some_bytes_decay(state->instm, state->buf,
																		   chunk);
			}
			else
			{
				::fast_io::herbceptions::throws_errc(::std::errc::invalid_seek);
			}
		}
		::fast_io::details::async_transmit_bytes_read_done(state, {},
														   static_cast<::std::size_t>(iter - state->buf));
	}
	catch throws(::std::error e)
	{
		::fast_io::details::async_transmit_bytes_read_done(state, e.release(), 0);
	}
}

/* the write-side mirror of async_transmit_bytes_sync_read */
template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
inline void async_transmit_bytes_sync_write(async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *state, ::std::byte const *first,
											::std::size_t left) noexcept
{
	try
	{
		::std::byte const *iter;
		if (state->off_out.has_opt)
		{
			if constexpr (::fast_io::operations::decay::defines::bytes_pwritable<outstmtype>)
			{
				iter = ::fast_io::operations::decay::pwrite_some_bytes_decay(state->outstm, first,
																			 left, state->off_out.opt);
			}
			else
			{
				::fast_io::herbceptions::throws_errc(::std::errc::invalid_seek);
			}
		}
		else
		{
			if constexpr (::fast_io::operations::decay::defines::bytes_writable<outstmtype>)
			{
				iter = ::fast_io::operations::decay::write_some_bytes_decay(state->outstm, first,
																			left);
			}
			else
			{
				::fast_io::herbceptions::throws_errc(::std::errc::invalid_seek);
			}
		}
		::fast_io::details::async_transmit_bytes_write_done(state, {},
															static_cast<::std::size_t>(iter - first));
	}
	catch throws(::std::error e)
	{
		::fast_io::details::async_transmit_bytes_write_done(state, e.release(), 0);
	}
}

/*
 * The generic engine is shared by transmit_some and transmit_all: `some`
 * delivers after the first fully-written round (bytes moved can be short of
 * the bound — the caller loops); `all` resubmits read rounds until the
 * bound is met or EOF. EOF is a normal stop for both (some reports 0, all
 * reports success). Each side dispatches independently: a stream with an
 * async pread/pwrite define goes through the scheduler; a stream with only
 * synchronous operations (a hash context, an in-memory source, a file not
 * opened for async) is driven inline — its "completion" is invoked directly
 * inside the submit lambda.
 */
template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
inline void async_transmit_bytes_read_done_step(void *vst, ::std::cxx_std_error err,
												::std::size_t bytesread) noexcept
{
	auto *state{static_cast<async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *>(vst)};
	::fast_io::details::async_transmit_bytes_read_done(state, err, bytesread);
}

template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
inline void async_transmit_bytes_deliver_step(void *vst, ::std::cxx_std_error err) noexcept
{
	auto *state{static_cast<async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *>(vst)};
	::fast_io::details::async_transmit_bytes_deliver(state, err, state->moved);
}

template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
inline void async_transmit_bytes_read_work(void *vst, ::std::size_t chunk) noexcept
{
	auto *state{static_cast<async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *>(vst)};
	if constexpr (::fast_io::operations::decay::defines::
					  has_async_pread_some_bytes_underflow_callback_define<
						  typename ::std::remove_cvref_t<decltype(*state)>::scheduler_type, instmtype,
						  ::fast_io::details::async_io_callback>)
	{
		async_pread_some_bytes_underflow_callback_define(
			state->sched, state->timeout, state->instm, state->buf, chunk, state->off_in,
			async_bytes_step_cb{state, &async_transmit_bytes_read_done_step<all, scheduler, outstmtype, instmtype, alloc_type>});
	}
	else if constexpr (::fast_io::operations::decay::defines::bytes_readable<instmtype> ||
					   ::fast_io::operations::decay::defines::bytes_preadable<instmtype>)
	{
		::fast_io::details::async_transmit_bytes_sync_read(state, chunk);
	}
	else
	{
		static_assert(!sizeof(decltype(*state)),
					  "async_transmit input supports neither async nor synchronous byte reads");
	}
}

template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
inline void async_transmit_bytes_read_round(async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *state) noexcept
{
	::std::size_t const chunk{state->remaining < ::fast_io::details::async_transmit_bounce_size
								  ? state->remaining
								  : ::fast_io::details::async_transmit_bounce_size};
	::fast_io::details::async_submit_catching(
		async_work_step_cb{state, chunk,
						   &async_transmit_bytes_read_work<all, scheduler, outstmtype, instmtype, alloc_type>},
		async_err_step_cb{state, &async_transmit_bytes_deliver_step<all, scheduler, outstmtype, instmtype, alloc_type>});
}

template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
inline void async_transmit_bytes_write_done_step(void *vst, ::std::cxx_std_error err,
												 ::std::size_t byteswritten) noexcept
{
	auto *state{static_cast<async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *>(vst)};
	::fast_io::details::async_transmit_bytes_write_done(state, err, byteswritten);
}

template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
inline void async_transmit_bytes_write_work(void *vst, ::std::size_t left) noexcept
{
	auto *state{static_cast<async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *>(vst)};
	::std::byte const *first{state->buf + (state->moved - state->round_base)};
	if constexpr (::fast_io::operations::decay::defines::
					  has_async_pwrite_some_bytes_overflow_callback_define<
						  typename ::std::remove_cvref_t<decltype(*state)>::scheduler_type, outstmtype,
						  ::fast_io::details::async_io_callback>)
	{
		async_pwrite_some_bytes_overflow_callback_define(
			state->sched, state->timeout, state->outstm, first, left,
			state->off_out,
			async_bytes_step_cb{state, &async_transmit_bytes_write_done_step<all, scheduler, outstmtype, instmtype, alloc_type>});
	}
	else if constexpr (::fast_io::operations::decay::defines::bytes_writable<outstmtype> ||
					   ::fast_io::operations::decay::defines::bytes_pwritable<outstmtype>)
	{
		::fast_io::details::async_transmit_bytes_sync_write(state, first, left);
	}
	else
	{
		static_assert(!sizeof(decltype(*state)),
					  "async_transmit output supports neither async nor synchronous byte writes");
	}
}

template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type>
inline void async_transmit_bytes_write_staged(async_transmit_bytes_state_base<all, scheduler, outstmtype, instmtype, alloc_type> *state) noexcept
{
	::std::size_t const left{state->staged - (state->moved - state->round_base)};
	::fast_io::details::async_submit_catching(
		async_work_step_cb{state, left,
						   &async_transmit_bytes_write_work<all, scheduler, outstmtype, instmtype, alloc_type>},
		async_err_step_cb{state, &async_transmit_bytes_deliver_step<all, scheduler, outstmtype, instmtype, alloc_type>});
}

/*
 * transmit_all over a backend's native some define: resubmit until the
 * bound is met or a zero-byte round reports EOF. Shares the loop shape
 * with the p{read,write}_all chains.
 */
template <typename scheduler, typename outstmtype, typename instmtype, typename alloc_type>
struct async_transmit_all_native_state_base
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	scheduler sched;
	outstmtype outstm;
	::fast_io::intfpos_opt off_out;
	instmtype instm;
	::fast_io::intfpos_opt off_in;
	::std::size_t remaining;
	::fast_io::posix_statx_timestamp_opt timeout;
	void (*finish)(async_transmit_all_native_state_base *state,
				   ::std::cxx_std_error err) noexcept {};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename scheduler, typename outstmtype, typename instmtype, typename alloc_type,
		  typename T>
struct async_transmit_all_native_state;

template <typename scheduler, typename outstmtype, typename instmtype, typename alloc_type,
		  typename T>
inline void async_transmit_all_native_finish_cb(
	async_transmit_all_native_state_base<scheduler, outstmtype, instmtype, alloc_type> *b,
	::std::cxx_std_error err) noexcept
{
	auto *state{static_cast<async_transmit_all_native_state<scheduler, outstmtype, instmtype,
															alloc_type, T> *>(b)};
	auto callback{::std::move(state->callback)};
	::fast_io::details::async_delete_state(state);
	callback(err);
}

template <typename scheduler, typename outstmtype, typename instmtype, typename alloc_type,
		  typename T>
struct async_transmit_all_native_state
	: async_transmit_all_native_state_base<scheduler, outstmtype, instmtype, alloc_type>
{
	using base_type =
		async_transmit_all_native_state_base<scheduler, outstmtype, instmtype, alloc_type>;
	T callback;

	inline async_transmit_all_native_state(
		scheduler s, outstmtype o, ::fast_io::intfpos_opt oo, instmtype i,
		::fast_io::intfpos_opt oi, ::std::size_t rem,
		::fast_io::posix_statx_timestamp_opt tmo, T cb) noexcept
		: base_type{s, o, oo, i, oi, rem, tmo}, callback{::std::move(cb)}
	{
		this->finish = &async_transmit_all_native_finish_cb<scheduler, outstmtype, instmtype,
															alloc_type, T>;
	}
};


template <typename scheduler, typename outstmtype, typename instmtype, typename alloc_type>
inline void async_transmit_all_native_moved_step(void *vst, ::std::cxx_std_error err,
												 ::std::size_t moved) noexcept
{
	auto *state{static_cast<async_transmit_all_native_state_base<scheduler, outstmtype, instmtype, alloc_type> *>(vst)};
	if (err.domain == nullptr)
	{
		state->remaining -= moved;
		if (state->off_out.has_opt)
		{
			state->off_out.opt = ::fast_io::fposoffadd_nonegative(
				state->off_out.opt, static_cast<::std::ptrdiff_t>(moved));
		}
		if (state->off_in.has_opt)
		{
			state->off_in.opt = ::fast_io::fposoffadd_nonegative(
				state->off_in.opt, static_cast<::std::ptrdiff_t>(moved));
		}
		if (moved != 0 && state->remaining != 0)
		{
			async_transmit_all_native_submit(state);
			return;
		}
	}
	state->finish(state, err);
}

template <typename scheduler, typename outstmtype, typename instmtype, typename alloc_type>
inline void async_transmit_all_native_err_step(void *vst, ::std::cxx_std_error err) noexcept
{
	auto *state{static_cast<async_transmit_all_native_state_base<scheduler, outstmtype, instmtype, alloc_type> *>(vst)};
	state->finish(state, err);
}

template <typename scheduler, typename outstmtype, typename instmtype, typename alloc_type>
inline void async_transmit_all_native_work(void *vst, ::std::size_t bound) noexcept
{
	auto *state{static_cast<async_transmit_all_native_state_base<scheduler, outstmtype, instmtype, alloc_type> *>(vst)};
	async_transmit_some_bytes_overflow_underflow_callback_define(
		state->sched, state->timeout, state->outstm, state->off_out, state->instm,
		state->off_in, ::fast_io::size_t_opt{bound},
		async_bytes_step_cb{state, &async_transmit_all_native_moved_step<scheduler, outstmtype, instmtype, alloc_type>});
}

template <typename scheduler, typename outstmtype, typename instmtype, typename alloc_type>
inline void async_transmit_all_native_submit(
	async_transmit_all_native_state_base<scheduler, outstmtype, instmtype, alloc_type> *state) noexcept
{
	::std::size_t const bound{state->remaining};
	::fast_io::details::async_submit_catching(
		async_work_step_cb{state, bound,
						   &async_transmit_all_native_work<scheduler, outstmtype, instmtype, alloc_type>},
		async_err_step_cb{state, &async_transmit_all_native_err_step<scheduler, outstmtype, instmtype, alloc_type>});
}

} // namespace fast_io::details

namespace fast_io::operations::decay
{

/*
 * async transmit_some: prefers a backend's native
 * async_transmit_some_bytes_overflow_underflow_callback_define (e.g. io_uring SPLICE);
 * otherwise emulates with a read->write chain through a bounce buffer.
 * The functor is invoked once as
 * callback(::std::cxx_std_error, ::std::size_t) noexcept — the size is the
 * bytes delivered to the output.
 */
template <typename async_scheduler_type, typename outstmtype, typename instmtype,
		  typename callback_type>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<callback_type>>
inline void async_transmit_some_bytes_decay_callback(
	async_scheduler_type scheduler, ::fast_io::posix_statx_timestamp_opt timeout,
	outstmtype outstm, ::fast_io::intfpos_opt off_out, instmtype instm,
	::fast_io::intfpos_opt off_in, ::fast_io::size_t_opt bound,
	callback_type callback) noexcept
{
	using outstm_reftype = outstmtype;
	using instm_reftype = instmtype;
	if constexpr (::fast_io::operations::decay::defines::
					  has_async_transmit_some_bytes_overflow_underflow_callback_define<
						  async_scheduler_type, outstm_reftype, instm_reftype,
						  ::std::remove_cvref_t<callback_type>>)
	{
		async_transmit_some_bytes_overflow_underflow_callback_define(
			scheduler, timeout,
			outstm,
			off_out,
			instm,
			off_in, bound,
			::std::move(callback));
		return;
	}
	else if constexpr (!::fast_io::operations::decay::defines::
						   has_async_pread_some_bytes_underflow_callback_define<
							   async_scheduler_type, instm_reftype,
							   ::fast_io::details::async_io_callback> &&
					   !::fast_io::operations::decay::defines::
						   has_async_pwrite_some_bytes_overflow_callback_define<
							   async_scheduler_type, outstm_reftype,
							   ::fast_io::details::async_io_callback> &&
					   (::fast_io::operations::decay::defines::bytes_readable<instm_reftype> ||
						::fast_io::operations::decay::defines::bytes_preadable<instm_reftype>) &&
					   (::fast_io::operations::decay::defines::bytes_writable<outstm_reftype> ||
						::fast_io::operations::decay::defines::bytes_pwritable<outstm_reftype>))
	{
		/* neither side speaks async — run the synchronous transmit inline
		 * and report through the callback contract */
		::fast_io::intfpos_t off_out_v{}, off_in_v{};
		::fast_io::fpos_nullable_ptr poff_out{}, poff_in{};
		if (off_out.has_opt)
		{
			off_out_v = off_out.opt;
			poff_out = {off_out_v};
		}
		if (off_in.has_opt)
		{
			off_in_v = off_in.opt;
			poff_in = {off_in_v};
		}
		::std::size_t const remaining{bound.has_opt ? bound.opt
													: ::std::numeric_limits<::std::size_t>::max()};
		try
		{
			callback(::std::cxx_std_error{},
					 ::fast_io::operations::decay::transmit_some_bytes_decay(outstm, poff_out, instm,
																			 poff_in, remaining));
		}
		catch throws(::std::error e)
		{
			callback(e.release(), 0);
		}
		return;
	}
	else
	{
		using alloc_type = ::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>;
		using state_type =
			::fast_io::details::async_transmit_bytes_state<false, async_scheduler_type,
														   outstm_reftype, instm_reftype, alloc_type,
														   ::std::remove_cvref_t<callback_type>>;
		::std::size_t const remaining{bound.has_opt ? bound.opt
													: ::std::numeric_limits<::std::size_t>::max()};
		if (remaining == 0)
		{
			callback(::std::cxx_std_error{}, 0);
			return;
		}
		try
		{
			::fast_io::details::async_transmit_bytes_read_round(
				::fast_io::details::async_new_state<state_type>(
					scheduler,
					outstm,
					off_out,
					instm,
					off_in, remaining, timeout,
					::std::move(callback), 0zu, 0zu, 0zu));
			return;
		}
		catch throws(::std::error e)
		{
			callback(e.release(), 0);
		}
	}
}

/*
 * async transmit_all: loops transmit_some until the bound is met or EOF.
 * With a native backend define it resubmits native rounds; otherwise it
 * uses the bounce-buffer engine. The functor is invoked once as
 * callback(::std::cxx_std_error) noexcept.
 */
template <typename async_scheduler_type, typename outstmtype, typename instmtype,
		  typename callback_type>
	requires ::std::is_nothrow_invocable_v<callback_type, ::std::cxx_std_error>
inline void async_transmit_all_bytes_decay_callback(
	async_scheduler_type scheduler, ::fast_io::posix_statx_timestamp_opt timeout,
	outstmtype outstm, ::fast_io::intfpos_opt off_out, instmtype instm,
	::fast_io::intfpos_opt off_in, ::fast_io::size_t_opt bound,
	callback_type callback) noexcept
{
	using outstm_reftype = outstmtype;
	using instm_reftype = instmtype;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>;
	::std::size_t const remaining{bound.has_opt ? bound.opt
												: ::std::numeric_limits<::std::size_t>::max()};
	if (remaining == 0)
	{
		callback(::std::cxx_std_error{});
		return;
	}
	try
	{
		if constexpr (::fast_io::operations::decay::defines::
						  has_async_transmit_some_bytes_overflow_underflow_callback_define<
							  async_scheduler_type, outstm_reftype, instm_reftype,
							  ::fast_io::details::async_io_callback>)
		{
			using state_type =
				::fast_io::details::async_transmit_all_native_state<async_scheduler_type,
																	outstm_reftype, instm_reftype,
																	alloc_type,
																	::std::remove_cvref_t<callback_type>>;
			::fast_io::details::async_transmit_all_native_submit(
				::fast_io::details::async_new_state<state_type>(
					scheduler,
					outstm,
					off_out,
					instm,
					off_in, remaining, timeout, ::std::move(callback)));
		}
		else if constexpr (!::fast_io::operations::decay::defines::
							   has_async_pread_some_bytes_underflow_callback_define<
								   async_scheduler_type, instm_reftype,
								   ::fast_io::details::async_io_callback> &&
						   !::fast_io::operations::decay::defines::
							   has_async_pwrite_some_bytes_overflow_callback_define<
								   async_scheduler_type, outstm_reftype,
								   ::fast_io::details::async_io_callback> &&
						   (::fast_io::operations::decay::defines::bytes_readable<instm_reftype> ||
							::fast_io::operations::decay::defines::bytes_preadable<instm_reftype>) &&
						   (::fast_io::operations::decay::defines::bytes_writable<outstm_reftype> ||
							::fast_io::operations::decay::defines::bytes_pwritable<outstm_reftype>))
		{
			/* neither side speaks async — synchronous transmit inline */
			::fast_io::intfpos_t off_out_v{}, off_in_v{};
			::fast_io::fpos_nullable_ptr poff_out{}, poff_in{};
			if (off_out.has_opt)
			{
				off_out_v = off_out.opt;
				poff_out = {off_out_v};
			}
			if (off_in.has_opt)
			{
				off_in_v = off_in.opt;
				poff_in = {off_in_v};
			}
			::fast_io::operations::decay::transmit_all_bytes_decay(outstm, poff_out, instm, poff_in,
																   bound);
			callback(::std::cxx_std_error{});
		}
		else
		{
			using state_type =
				::fast_io::details::async_transmit_bytes_state<true, async_scheduler_type,
															   outstm_reftype, instm_reftype,
															   alloc_type,
															   ::std::remove_cvref_t<callback_type>>;
			::fast_io::details::async_transmit_bytes_read_round(
				::fast_io::details::async_new_state<state_type>(
					scheduler,
					outstm,
					off_out,
					instm,
					off_in, remaining, timeout, ::std::move(callback), 0zu, 0zu, 0zu));
		}
		return;
	}
	catch throws(::std::error e)
	{
		callback(e.release());
	}
}

} // namespace fast_io::operations::decay

namespace fast_io::details
{

/* coroutine awaiters for the transmit decays */
template <typename scheduler, typename outstmtype, typename instmtype>
struct async_transmit_some_bytes_awaiter : async_awaiter_result<::std::size_t>
{
	scheduler sched;
	outstmtype outstm;
	::fast_io::intfpos_opt off_out;
	instmtype instm;
	::fast_io::intfpos_opt off_in;
	::fast_io::size_t_opt bound;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_transmit_some_bytes_decay_callback(
			sched, timeout, outstm, off_out, instm, off_in, bound,
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

template <typename scheduler, typename outstmtype, typename instmtype>
struct async_transmit_all_bytes_awaiter : async_awaiter_result<void>
{
	scheduler sched;
	outstmtype outstm;
	::fast_io::intfpos_opt off_out;
	instmtype instm;
	::fast_io::intfpos_opt off_in;
	::fast_io::size_t_opt bound;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_transmit_all_bytes_decay_callback(
			sched, timeout, outstm, off_out, instm, off_in, bound,
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

/* Coroutine forms of the transmit decays. The some form yields the bytes
 * transmitted; the all form yields void. */
template <typename async_scheduler_type, typename outstmtype, typename instmtype>
inline auto async_transmit_some_bytes_decay(async_scheduler_type scheduler,
											::fast_io::posix_statx_timestamp_opt timeout,
											outstmtype outstm,
											::fast_io::intfpos_opt off_out, instmtype instm,
											::fast_io::intfpos_opt off_in,
											::fast_io::size_t_opt bound) noexcept
{
	return ::fast_io::details::async_transmit_some_bytes_awaiter<
		async_scheduler_type, outstmtype,
		instmtype>{
		{}, scheduler, outstm, off_out, instm, off_in, bound, timeout};
}

template <typename async_scheduler_type, typename outstmtype, typename instmtype>
inline auto async_transmit_all_bytes_decay(async_scheduler_type scheduler,
										   ::fast_io::posix_statx_timestamp_opt timeout,
										   outstmtype outstm,
										   ::fast_io::intfpos_opt off_out, instmtype instm,
										   ::fast_io::intfpos_opt off_in,
										   ::fast_io::size_t_opt bound) noexcept
{
	return ::fast_io::details::async_transmit_all_bytes_awaiter<
		async_scheduler_type, outstmtype,
		instmtype>{
		{}, scheduler, outstm, off_out, instm, off_in, bound, timeout};
}

} // namespace fast_io::operations::decay
