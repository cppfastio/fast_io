#pragma once

namespace fast_io::details
{

/* Per-arg scan state: the incremental context state for
 * context_scannable args; precise-reserve args parse in one shot and
 * carry no state, so the member degenerates to empty. */
template <typename char_type, typename argtype, bool>
struct async_scan_arg_state
{
	using type = ::fast_io::details::empty;
};

template <typename char_type, typename argtype>
struct async_scan_arg_state<char_type, argtype, true>
{
	using type = typename ::std::remove_cvref_t<decltype(scan_context_type(
		::fast_io::io_reserve_type<char_type, argtype>))>::type;
};

/* Staging buffer for precise-reserve scans that must be fed through an
 * async read when the pending window is shorter than the fixed token
 * size; absent (size 0) for non-precise args. */
template <typename char_type, typename argtype, bool>
struct async_scan_precise_size
{
	static inline constexpr ::std::size_t value{0};
};

template <typename char_type, typename argtype>
struct async_scan_precise_size<char_type, argtype, true>
{
	static inline constexpr ::std::size_t value{
		scan_precise_reserve_size(::fast_io::io_reserve_type<char_type, argtype>)};
};

/*
 * Resumable per-argument scan awaiter: parses one arg from the input
 * stream's pending window, refilling it asynchronously when the window
 * runs out mid-token. This is a plain awaiter — no coroutine frame, no
 * allocation — so the common case (the whole token already buffered)
 * resolves entirely inside await_ready at the cost of a synchronous
 * scan. The multi-arg async_scan_some coroutine drives one of these per
 * argument and pays a single coroutine frame per call, not per arg.
 *
 * Semantics mirror the sync per-arg scan: await_resume reports
 * scan_some_result_t{0} on success and {1} when the stream hit
 * end_of_file mid-arg (or require_all turns it into an end_of_file
 * herbception); parse failures arrive through the error channel.
 *
 * Suspension/resumption is callback-driven: an exhausted window submits
 * async_ibuffer_underflow; a precise-reserve arg with a short window
 * submits async_pread_all_bytes into the staging buffer (the buffered
 * define serves pending bytes first, preserving order). The completion
 * re-enters pump() which either parses on or submits again — the user
 * coroutine resumes only when the arg is fully resolved.
 */
/* refill slots shared by both scan-refill completions: one non-template
   block so the (e) and (e, got) paths are a single callback type and the
   pread_all/underflow machinery beneath them instantiates once */
struct async_scan_refill_state
{
	async_awaiter_result_base *result{};
	void *awaiter{};
	void (*resume_pump)(void *awaiter) noexcept {};
	::std::cxx_std_error refill_err;
	::std::size_t refill_got;
	bool pumping{};
	bool refill_ready{};
};

inline void async_scan_refill_fire(async_scan_refill_state *r,
								   ::std::cxx_std_error e, ::std::size_t got) noexcept
{
	r->refill_err = e;
	r->refill_got = got;
	r->refill_ready = true;
	if (r->pumping || !r->result->suspended)
	{
		return;
	}
	r->resume_pump(r->awaiter);
}

/* one callable type for both refill submissions */
struct async_scan_refill_callback
{
	async_scan_refill_state *refill;
	inline void operator()(::std::cxx_std_error e) noexcept
	{
		async_scan_refill_fire(refill, e, 0);
	}
	inline void operator()(::std::cxx_std_error e, ::std::size_t got) noexcept
	{
		async_scan_refill_fire(refill, e, got);
	}
};

template <bool require_all, typename scheduler, typename instmtype, typename argtype>
struct async_scan_arg_awaiter
	: async_awaiter_result<::fast_io::scan_some_result_t>
{
	using char_type = typename instmtype::input_char_type;
	static constexpr ::std::size_t precise_n{
		async_scan_precise_size<char_type, argtype,
								::fast_io::precise_reserve_scannable<char_type, argtype>>::value};

	scheduler sched;
	instmtype instm;
	::fast_io::posix_statx_timestamp_opt timeout;
	argtype arg;
	FAST_IO_NO_UNIQUE_ADDRESS typename async_scan_arg_state<
		char_type, argtype,
		::fast_io::context_scannable<char_type, argtype>>::type state;
	char_type pread_buffer[precise_n == 0 ? 1 : precise_n];
	/* the parse-phase flags ride on await_ready's fast path and must be
	 * initialized at construction; the refill machinery below is only
	 * touched once a submission is in flight, so await_suspend does its
	 * initialization lazily instead of paying for it on every call */
	bool in_context{};
	bool eof_seen{};
	bool awaiting_pread{};
	bool pread_done{};
	async_scan_refill_state refill{};

	inline constexpr async_scan_arg_awaiter(scheduler s, instmtype i,
											::fast_io::posix_statx_timestamp_opt t,
											argtype a) noexcept
		: sched{s}, instm{i}, timeout{t}, arg{::std::move(a)}, state{}
	{
	}

	/* commit the consumed cursor — the iterator may differ in type from
	 * the stream's cursor on transcode refs */
	template <typename currtype, typename itertype>
	inline void commit_curr(currtype c, itertype it) noexcept
	{
		if constexpr (::std::same_as<currtype, itertype>)
		{
			ibuffer_set_curr(instm, it);
		}
		else
		{
			ibuffer_set_curr(instm, it - c + c);
		}
	}

	/* ec -> terminal: ok reports the arg scanned, end_of_file reports it
	 * unscanned, anything else lands in the error slot */
	inline bool resolve_ec(::fast_io::freestanding::parse_errc ec) noexcept
	{
		if (ec == ::fast_io::freestanding::parse_errc::ok)
		{
			this->value.remained_args = 0;
			return true;
		}
		if (ec == ::fast_io::freestanding::parse_errc::end_of_file)
		{
			this->value.remained_args = 1;
			return true;
		}
		this->err = ::fast_io::details::async_make_error(ec);
		return true;
	}

	inline bool resolve_precise(char_type const *p)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(scan_precise_reserve_define(
			::fast_io::io_reserve_type<char_type, argtype>, p, arg)))
	{
		if constexpr (::fast_io::precise_reserve_scannable_no_error<char_type, argtype>)
		{
			scan_precise_reserve_define(::fast_io::io_reserve_type<char_type, argtype>, p, arg);
			this->value.remained_args = 0;
			return true;
		}
		else
		{
			return resolve_ec(scan_precise_reserve_define(
				::fast_io::io_reserve_type<char_type, argtype>, p, arg));
		}
	}

	/* one synchronous parse step; true = resolved, false = needs device
	 * bytes (awaiting_pread selects the submission kind) */
	inline bool step()
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(resolve_precise(::std::declval<char_type const *>())))
	{
		if constexpr (::fast_io::precise_reserve_scannable<char_type, argtype>)
		{
			if (pread_done)
			{
				pread_done = false;
				return resolve_precise(pread_buffer);
			}
			auto curr{ibuffer_curr(instm)};
			if (static_cast<::std::size_t>(ibuffer_end(instm) - curr) >= precise_n)
			{
				ibuffer_set_curr(instm, curr + precise_n);
				return resolve_precise(curr);
			}
			if (eof_seen)
			{
				this->value.remained_args = 1;
				return true;
			}
			awaiting_pread = true;
			return false;
		}
		else if constexpr (::fast_io::context_scannable<char_type, argtype>)
		{
			if (!in_context)
			{
				in_context = true;
				if constexpr (::fast_io::contiguous_scannable<char_type, argtype>)
				{
					auto curr{ibuffer_curr(instm)};
					auto const end{ibuffer_end(instm)};
					auto [it, ec]{scan_contiguous_define(
						::fast_io::io_reserve_type<char_type, argtype>, curr, end, arg)};
					if (it != end)
					{
						commit_curr(curr, it);
						return resolve_ec(ec);
					}
				}
			}
			if (eof_seen)
			{
				return resolve_ec(scan_context_eof_define(
					::fast_io::io_reserve_type<char_type, argtype>, state, arg));
			}
			auto c{ibuffer_curr(instm)};
			auto const e{ibuffer_end(instm)};
			auto [it, ec]{scan_context_define(
				::fast_io::io_reserve_type<char_type, argtype>, state, c, e, arg)};
			commit_curr(c, it);
			if (ec == ::fast_io::freestanding::parse_errc::ok)
			{
				this->value.remained_args = 0;
				return true;
			}
			if (ec != ::fast_io::freestanding::parse_errc::partial)
			{
				return resolve_ec(ec);
			}
			/* partial consumed the whole window (it == e), so the refill
			 * cannot discard pending bytes */
			return false;
		}
		else
		{
			static_assert(::fast_io::context_scannable<char_type, argtype>,
						  "type not scannable. need context_scannable");
			return true;
		}
	}

	inline void consume_refill() noexcept
	{
		refill.refill_ready = false;
		if (refill.refill_err.domain != nullptr)
		{
			this->err = refill.refill_err;
			refill.refill_err = {};
			return;
		}
		if (awaiting_pread)
		{
			awaiting_pread = false;
			pread_done = true;
		}
		else if (refill.refill_got == 0)
		{
			eof_seen = true;
		}
	}

	/* fire the pending refill — inline completions loop back into pump,
	 * async completions re-enter through the callback */
	inline void submit() noexcept
	{
		/* the back-pointers track THIS live awaiter -- wiring them in the
		   ctor would capture a pre-materialization prvalue that dies when
		   the awaiter lands in the coroutine frame */
		refill.result = this;
		refill.awaiter = this;
		refill.resume_pump = [](void *a) noexcept {
			auto *self{static_cast<async_scan_arg_awaiter *>(a)};
			if (self->pump())
			{
				self->coro.resume();
			}
		};
		refill.refill_ready = false;
		if (awaiting_pread)
		{
			::fast_io::operations::decay::async_pread_all_bytes_decay_callback(
				sched, timeout, instm, reinterpret_cast<::std::byte *>(pread_buffer),
				precise_n * sizeof(char_type), ::fast_io::intfpos_opt{},
				async_scan_refill_callback{__builtin_addressof(refill)});
		}
		else
		{
			async_ibuffer_underflow(
				sched, timeout, instm,
				async_scan_refill_callback{__builtin_addressof(refill)});
		}
	}

	/* drive the state machine until resolved or a submission is genuinely
	 * pending; returns true when value/err is ready */
	inline bool pump() noexcept
	{
		if (this->err.domain != nullptr)
		{
			return true;
		}
		refill.pumping = true;
		for (;;)
		{
			if (refill.refill_ready)
			{
				consume_refill();
				if (this->err.domain != nullptr)
				{
					refill.pumping = false;
					return true;
				}
			}
			try
			{
				if (step())
				{
					refill.pumping = false;
					return true;
				}
			}
			catch throws(::std::error e)
			{
				this->err = e.release();
				refill.pumping = false;
				return true;
			}
			submit();
			if (!refill.refill_ready)
			{
				break;
			}
		}
		refill.pumping = false;
		return false;
	}

	/* the common case: the pending window already covers the token, so
	 * the whole scan resolves here without suspending or allocating */
	inline bool await_ready()
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(step()))
	{
		return step();
	}

	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		refill.refill_ready = false;
		if (pump())
		{
			return false;
		}
		this->suspended = true;
		return true;
	}

	inline ::fast_io::scan_some_result_t await_resume() FAST_IO_HERBCEPTIONS_THROWS
		requires(!require_all)
	{
		if (this->err.domain != nullptr)
		{
			auto e{this->err};
			this->err = {};
			throw throws e;
		}
		return this->value;
	}

	inline void await_resume() FAST_IO_HERBCEPTIONS_THROWS
		requires(require_all)
	{
		if (this->err.domain != nullptr)
		{
			auto e{this->err};
			this->err = {};
			throw throws e;
		}
		if (this->value.remained_args != 0)
		{
			::fast_io::herbceptions::throws_parse_errc(
				::fast_io::freestanding::parse_errc::end_of_file);
		}
	}
};

} // namespace fast_io::details

namespace fast_io::operations::decay
{

/*
 * Single-arg decay: returns the resumable awaiter — a plain object, no
 * coroutine frame and no allocation. co_await resolves it synchronously
 * whenever the pending window covers the token.
 */
template <bool require_all, typename async_scheduler_type, typename instmtype, typename argtype>
inline auto async_scan_arg_decay(async_scheduler_type sched,
								 ::fast_io::posix_statx_timestamp_opt timeout, instmtype instm,
								 argtype arg) noexcept
{
	return ::fast_io::details::async_scan_arg_awaiter<require_all, async_scheduler_type, instmtype,
													  argtype>{sched, instm, timeout, ::std::move(arg)};
}

/*
 * Multi-arg async scan_some: one coroutine frame per call driving the
 * per-arg awaiters — each of them suspends only when the pending window
 * runs out, so in-buffer multi-arg scans pay a single frame.
 */
template <typename async_scheduler_type, typename instmtype, typename... Args>
inline ::fast_io::io_async_task<
	::fast_io::scan_some_result_t,
	/* the scheduler decides coroutine scheduling and frame allocation;
	 * a status-allocator scheduler supplies .alloc_handle and the
	 * coroutine's promise operator new reads it from this first
	 * parameter — schedulers without one fail fast through
	 * native_global_allocator */
	::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>>
async_scan_some_freestanding_decay(async_scheduler_type sched,
								   ::fast_io::posix_statx_timestamp_opt timeout,
								   instmtype instm, Args... args) FAST_IO_HERBCEPTIONS_THROWS
{
	static_assert(::fast_io::operations::decay::defines::async_scheduler_observer<
				  async_scheduler_type>);
	static_assert(
		::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>,
		"If you want to scan this type of file, please add ::fast_io::basic_ibuf.");
	static_assert(
		::fast_io::operations::decay::defines::has_async_ibuffer_underflow<
			async_scheduler_type, instmtype,
			::fast_io::details::async_io_callback>,
		"async scan needs the stream's async_ibuffer_underflow");
	using char_type = typename instmtype::input_char_type;
	static_assert(sizeof(char_type) == 1,
				  "async scan is supported for sizeof(char_type) == 1 only");
	template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		if (!co_await ::fast_io::details::async_scan_arg_awaiter<
				false, async_scheduler_type, instmtype,
				::std::remove_cvref_t<Args... [i]>> { sched, instm, timeout,
														  ::std::move(args...[i]) })
		{
			co_return ::fast_io::scan_some_result_t{sizeof...(Args) - i};
		}
	}
	co_return ::fast_io::scan_some_result_t{};
}

} // namespace fast_io::operations::decay

namespace fast_io::operations
{

/*
 * async_scan_some_freestanding(sched, timeout, instm, args...) —
 * co_await-able async scan_some: parses each arg from the input
 * stream's buffer window, refilling asynchronously on partial scans.
 * Returns the same scan_some_result_t as sync scan_some: truthy means
 * every argument scanned; a nonzero remained_args reports the arguments
 * left unscanned.
 *
 * A single arg resolves to a plain awaiter — no coroutine frame — so the
 * in-buffer case costs no more than a synchronous scan; a multi-arg
 * call pays one coroutine frame for the driver, still suspending only
 * on real refills.
 *
 * The stream must be a buffered input (basic_ibuf et al.) — scanning a
 * byte stream without a buffer cannot put back the byte past the token.
 */
template <bool require_all = false, typename async_scheduler_type, typename instmtype,
		  typename... Args>
inline auto async_scan_some_freestanding(async_scheduler_type &&scheduler,
										 ::fast_io::posix_statx_timestamp_opt timeout,
										 instmtype &&instm, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
	requires(::fast_io::operations::decay::defines::async_scheduler_observer<
			 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>>)
{
	using instm_reftype =
		::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>;
	using char_type = typename instm_reftype::input_char_type;
	if constexpr (sizeof...(Args) == 1)
	{
		return ::fast_io::operations::decay::async_scan_arg_decay<require_all>(
			::fast_io::operations::async_scheduler_ref(scheduler), timeout,
			::fast_io::operations::input_stream_ref(instm),
			::fast_io::io_scan_forward<char_type>(::fast_io::io_scan_alias(args))...);
	}
	else
	{
		return ::fast_io::operations::decay::async_scan_some_freestanding_decay(
			::fast_io::operations::async_scheduler_ref(scheduler), timeout,
			::fast_io::operations::input_stream_ref(instm),
			::fast_io::io_scan_forward<char_type>(::fast_io::io_scan_alias(args))...);
	}
}

} // namespace fast_io::operations

namespace fast_io::operations::decay
{

/* multi-arg async_scan driver: awaits the scan_some result and raises
 * end_of_file through the channel when args remain unscanned */
template <typename async_scheduler_type, typename instmtype, typename... Args>
inline ::fast_io::io_async_task<
	void, ::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>>
async_scan_decay(async_scheduler_type sched,
				 ::fast_io::posix_statx_timestamp_opt timeout, instmtype &&instm,
				 Args &&...args) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!co_await ::fast_io::operations::async_scan_some_freestanding(
			sched, timeout, ::std::forward<instmtype>(instm), ::std::forward<Args>(args)...))
	{
		::fast_io::herbceptions::throws_parse_errc(
			::fast_io::freestanding::parse_errc::end_of_file);
	}
}

} // namespace fast_io::operations::decay

namespace fast_io
{

inline namespace io
{

/*
 * async_scan_some(sched, timeout, instm, args...) — the device-facing
 * entry: instm must be an async input stream carrying a buffer
 * (basic_ibuf et al.) — a byte past a token cannot be put back on an
 * unbuffered stream, so unlike sync scan there is no stdin default.
 * Returns scan_some_result_t — truthy when every argument scanned.
 */
template <typename async_scheduler_type, typename instmtype, typename... Args>
inline auto async_scan_some(async_scheduler_type &&scheduler,
							::fast_io::posix_statx_timestamp_opt timeout,
							instmtype &&instm, Args &&...args) noexcept
{
	static_assert(
		::fast_io::operations::decay::defines::has_ibuffer_basic_operations<
			::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>>,
		"async scan needs a buffered input stream (basic_ibuf et al.): a byte past "
		"the token cannot be put back on an unbuffered stream");
	return ::fast_io::operations::async_scan_some_freestanding(
		::std::forward<async_scheduler_type>(scheduler), timeout,
		::std::forward<instmtype>(instm), ::std::forward<Args>(args)...);
}

/*
 * async_scan(sched, timeout, instm, args...) — like sync scan: awaits the
 * async_scan_some awaiter/task and raises end_of_file through the
 * herbception channel when fewer than all args could be scanned.
 */
template <typename async_scheduler_type, typename instmtype, typename... Args>
inline auto async_scan(async_scheduler_type scheduler,
					   ::fast_io::posix_statx_timestamp_opt timeout, instmtype &&instm,
					   Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (sizeof...(Args) == 1)
	{
		return ::fast_io::operations::async_scan_some_freestanding<true>(
			::std::move(scheduler), timeout, ::std::forward<instmtype>(instm),
			::std::forward<Args>(args)...);
	}
	else
	{
		return ::fast_io::operations::decay::async_scan_decay(
			::std::move(scheduler), timeout, ::std::forward<instmtype>(instm),
			::std::forward<Args>(args)...);
	}
}

} // namespace io

} // namespace fast_io
