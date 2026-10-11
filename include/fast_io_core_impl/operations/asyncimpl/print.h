#pragma once

/*
 * async_print — coroutine-facing print for async output streams.
 *
 *   co_await operations::async_print(sched, timeout, out, "x=", 42, '\n');
 *
 * Formatting always runs synchronously at call time so temporaries never
 * cross a suspension point; only the byte write is asynchronous. The
 * formatting destination follows sync print:
 *
 *  - strlike targets (obuffer_flush_reserve streams) get the formatted
 *    text directly — no I/O is submitted at all;
 *  - buffered output streams get the fast path when the obuffer has
 *    room: bytes land in the buffer, nothing is submitted;
 *  - when the obuffer cannot take the text, an iobuf's pending bytes are
 *    handed to the write through detach_output_buffer — they are never
 *    copied — while the stream buffers into a fresh allocation;
 *  - buffer streams without a detachable buffer format into
 *    a basic_string and submit through the stream's own async
 *    write, which orders the payload behind whatever is still pending;
 *  - unbuffered streams format into a string and write it out.
 */


namespace fast_io::details
{

/* detached pending-buffer guard for a stream ref: the define's return
 * type when the stream can detach, empty when it cannot */
template <typename outstmtype,
		  bool = ::fast_io::operations::decay::defines::
			  has_output_stream_buffer_detach_define<outstmtype>>
struct async_print_detached_type
{
	using type = ::fast_io::details::empty;
};

template <typename outstmtype>
struct async_print_detached_type<outstmtype, true>
{
	using type = decltype(output_stream_buffer_detach_define(
		::std::declval<outstmtype>()));
};

/*
 * Handle-carrying strlike payload for streams whose allocator has
 * status: containers::basic_string stores no allocator handle, so a
 * handle-based allocator cannot back it. This buffer keeps the handle
 * alongside the pointers, and its strlike defines let the ordinary
 * print machinery format into it — including print_define arguments.
 * It doubles as the generic async-print payload for non-status
 * allocators: the handle sits empty, the calls dispatch to the plain
 * entry points.
 */
template <typename typed_allocator_type>
inline constexpr auto async_print_strlike_alloc(
	typename typed_allocator_type::handle_type hdl, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(typed_allocator_type::throws_on_allocation_failure)
{
	if constexpr (typed_allocator_type::has_status)
	{
		return typed_allocator_type::handle_allocate_at_least(hdl, n);
	}
	else
	{
		return typed_allocator_type::allocate_at_least(n);
	}
}

template <typename typed_allocator_type, ::std::integral char_type>
inline constexpr void async_print_strlike_dealloc(
	typename typed_allocator_type::handle_type hdl,
	char_type *p, ::std::size_t n) noexcept
{
	if constexpr (typed_allocator_type::has_status)
	{
		typed_allocator_type::handle_deallocate_n(hdl, p, n);
	}
	else
	{
		typed_allocator_type::deallocate_n(p, n);
	}
}

template <::std::integral char_type, typename allocator_type>
struct async_print_strlike_buffer
{
	using value_type = char_type;
	using typed_allocator_type =
		::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>;
	using handle_type = typename typed_allocator_type::handle_type;
	FAST_IO_NO_UNIQUE_ADDRESS handle_type allochdl;
	char_type *begin_ptr{}, *curr_ptr{}, *end_ptr{};

	/* the buffer_strlike concept requires default constructibility;
	 * construction without a handle is only used for dead branches */
	inline constexpr async_print_strlike_buffer() noexcept
		: allochdl{}
	{
	}
	inline constexpr async_print_strlike_buffer(handle_type hdl) noexcept
		: allochdl{hdl}
	{
	}
	async_print_strlike_buffer(async_print_strlike_buffer const &) = delete;
	async_print_strlike_buffer &operator=(async_print_strlike_buffer const &) = delete;
	inline constexpr async_print_strlike_buffer(async_print_strlike_buffer &&other) noexcept
		: allochdl{other.allochdl}, begin_ptr{other.begin_ptr}, curr_ptr{other.curr_ptr},
		  end_ptr{other.end_ptr}
	{
		other.begin_ptr = nullptr;
		other.curr_ptr = nullptr;
		other.end_ptr = nullptr;
	}
	inline async_print_strlike_buffer &operator=(async_print_strlike_buffer &&other) noexcept
	{
		if (this == __builtin_addressof(other)) [[unlikely]]
		{
			return *this;
		}
		if (begin_ptr != nullptr)
		{
			async_print_strlike_dealloc<typed_allocator_type, char_type>(
				allochdl, begin_ptr,
				static_cast<::std::size_t>(end_ptr - begin_ptr));
		}
		allochdl = other.allochdl;
		begin_ptr = other.begin_ptr;
		curr_ptr = other.curr_ptr;
		end_ptr = other.end_ptr;
		other.begin_ptr = nullptr;
		other.curr_ptr = nullptr;
		other.end_ptr = nullptr;
		return *this;
	}
	inline ~async_print_strlike_buffer()
	{
		if (begin_ptr != nullptr)
		{
			async_print_strlike_dealloc<typed_allocator_type, char_type>(
				allochdl, begin_ptr,
				static_cast<::std::size_t>(end_ptr - begin_ptr));
		}
	}
	inline constexpr char_type const *data() const noexcept
	{
		return begin_ptr;
	}
	inline constexpr ::std::size_t size() const noexcept
	{
		return static_cast<::std::size_t>(curr_ptr - begin_ptr);
	}
	inline constexpr bool empty() const noexcept
	{
		return curr_ptr == begin_ptr;
	}
};

template <::std::integral char_type, typename allocator_type>
inline constexpr char_type *
strlike_begin(::fast_io::io_strlike_type_t<char_type, async_print_strlike_buffer<char_type, allocator_type>>,
			  async_print_strlike_buffer<char_type, allocator_type> &str) noexcept
{
	return str.begin_ptr;
}

template <::std::integral char_type, typename allocator_type>
inline constexpr char_type *
strlike_curr(::fast_io::io_strlike_type_t<char_type, async_print_strlike_buffer<char_type, allocator_type>>,
			 async_print_strlike_buffer<char_type, allocator_type> &str) noexcept
{
	return str.curr_ptr;
}

template <::std::integral char_type, typename allocator_type>
inline constexpr char_type *
strlike_end(::fast_io::io_strlike_type_t<char_type, async_print_strlike_buffer<char_type, allocator_type>>,
			async_print_strlike_buffer<char_type, allocator_type> &str) noexcept
{
	return str.end_ptr;
}

template <::std::integral char_type, typename allocator_type>
inline constexpr void
strlike_set_curr(::fast_io::io_strlike_type_t<char_type, async_print_strlike_buffer<char_type, allocator_type>>,
				 async_print_strlike_buffer<char_type, allocator_type> &str, char_type *p) noexcept
{
	str.curr_ptr = p;
}

template <::std::integral char_type, typename allocator_type>
inline void
strlike_reserve(::fast_io::io_strlike_type_t<char_type, async_print_strlike_buffer<char_type, allocator_type>>,
				async_print_strlike_buffer<char_type, allocator_type> &str, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		(::fast_io::details::adapter_flags_or_default<allocator_type>() & ::fast_io::allocator_adapter_flags::throws_on_allocation_failure) != ::fast_io::allocator_adapter_flags::none)
{
	using typed_allocator_type =
		typename async_print_strlike_buffer<char_type, allocator_type>::typed_allocator_type;
	::std::size_t const capacity{static_cast<::std::size_t>(str.end_ptr - str.begin_ptr)};
	if (n <= capacity)
	{
		return;
	}
	::std::size_t const used{static_cast<::std::size_t>(str.curr_ptr - str.begin_ptr)};
	auto [newptr, newcap]{async_print_strlike_alloc<typed_allocator_type>(str.allochdl, n)};
	if (used != 0)
	{
		::fast_io::details::non_overlapped_copy_n(str.begin_ptr, used, newptr);
	}
	if (str.begin_ptr != nullptr)
	{
		async_print_strlike_dealloc<typed_allocator_type, char_type>(str.allochdl, str.begin_ptr, capacity);
	}
	str.begin_ptr = newptr;
	str.curr_ptr = newptr + used;
	str.end_ptr = newptr + static_cast<::std::size_t>(newcap);
}

template <::std::integral char_type, typename allocator_type>
inline constexpr ::fast_io::io_strlike_reference_wrapper<char_type, async_print_strlike_buffer<char_type, allocator_type>>
io_strlike_ref(::fast_io::io_alias_t,
			   async_print_strlike_buffer<char_type, allocator_type> &str) noexcept
{
	return {__builtin_addressof(str)};
}

template <::std::integral char_type, typename allocator_type>
inline constexpr auto strlike_construct_define(
	io_strlike_type_t<char_type, async_print_strlike_buffer<char_type, allocator_type>>,
	char_type const *first, char_type const *last)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		(::fast_io::details::adapter_flags_or_default<allocator_type>() &
		 ::fast_io::allocator_adapter_flags::throws_on_allocation_failure) !=
		::fast_io::allocator_adapter_flags::none)
{
	using buffer_type = async_print_strlike_buffer<char_type, allocator_type>;
	buffer_type str;
	auto const n{static_cast<::std::size_t>(last - first)};
	if (n != 0)
	{
		using typed_allocator_type = typename buffer_type::typed_allocator_type;
		auto [newptr, newcap]{
			async_print_strlike_alloc<typed_allocator_type>(str.allochdl, n)};
		str.begin_ptr = newptr;
		str.curr_ptr = ::fast_io::details::non_overlapped_copy_n(first, n, newptr);
		str.end_ptr = newptr + static_cast<::std::size_t>(newcap);
	}
	return str;
}

template <::std::integral char_type, typename allocator_type>
inline constexpr auto strlike_construct_single_character_define(
	io_strlike_type_t<char_type, async_print_strlike_buffer<char_type, allocator_type>>,
	char_type ch)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		(::fast_io::details::adapter_flags_or_default<allocator_type>() &
		 ::fast_io::allocator_adapter_flags::throws_on_allocation_failure) !=
		::fast_io::allocator_adapter_flags::none)
{
	return strlike_construct_define(
		::fast_io::io_strlike_type<char_type,
								   async_print_strlike_buffer<char_type, allocator_type>>,
		__builtin_addressof(ch), __builtin_addressof(ch) + 1);
}

/* pending byte range owned by a detached output buffer; the empty
 * placeholder used on non-detached paths contributes no bytes */
inline constexpr ::fast_io::io_scatter_t
async_print_detached_pending(::fast_io::details::empty const &) noexcept
{
	return {nullptr, 0};
}

template <typename detachedtype>
	requires requires(detachedtype const &d) { d.pending_bytes(); }
inline constexpr ::fast_io::io_scatter_t
async_print_detached_pending(detachedtype const &detached) noexcept
{
	return detached.pending_bytes();
}

/* format one argument at the cursor — used by the direct (in-buffer)
 * paths: reserve/dynamic_reserve/scatter_printable or a ready-made
 * scatter. Mirrors the sync print write-out switch. */
/* whether formatting one argument can throw: copy_scatter is noexcept;
 * the reserve/scatter defines decide per type */
template <::std::integral char_type, typename T>
inline constexpr bool async_print_format_one_may_throw{
	[]() consteval -> bool {
		using arg_type = ::std::remove_cvref_t<T>;
		if constexpr (::std::same_as<arg_type,
									 ::fast_io::basic_io_scatter_t<char_type>>)
		{
			return false;
		}
		else if constexpr (::fast_io::scatter_printable<char_type, arg_type>)
		{
			return !noexcept(::fast_io::details::copy_scatter(
				print_scatter_define(::fast_io::io_reserve_type<char_type, arg_type>,
									 ::std::declval<arg_type>()),
				::std::declval<char_type *&>()));
		}
		else
		{
			return !noexcept(print_reserve_define(
				::fast_io::io_reserve_type<char_type, arg_type>,
				::std::declval<char_type *&>(), ::std::declval<arg_type>()));
		}
	}()};

template <::std::integral char_type, typename T>
inline constexpr char_type *async_print_format_one(char_type *curr, T &&arg)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		::fast_io::details::async_print_format_one_may_throw<char_type, T>)
{
	using arg_type = ::std::remove_cvref_t<T>;
	if constexpr (::std::same_as<arg_type, ::fast_io::basic_io_scatter_t<char_type>>)
	{
		return ::fast_io::details::copy_scatter(arg, curr);
	}
	else if constexpr (::fast_io::scatter_printable<char_type, arg_type>)
	{
		return ::fast_io::details::copy_scatter(
			print_scatter_define(::fast_io::io_reserve_type<char_type, arg_type>, arg),
			curr);
	}
	else
	{
		return print_reserve_define(::fast_io::io_reserve_type<char_type, arg_type>, curr,
									arg);
	}
}

/*
 * Awaiter for async_print_decay. Owns the formatted payload and the
 * detached pending buffer (when used); await_suspend submits a single
 * pwrite_all for one range or a two-scatter pwrite_all when detached
 * bytes precede the payload; await_resume rethrows a recorded error
 * through the herbception channel. A fully buffered print leaves nothing
 * to submit, so await_ready skips the suspend entirely.
 */
/* The bytes an async print actually submits — detached pending buffer,
 * formatted payload and the scatter table referencing both — live in a
 * state object allocated through the scheduler's allocator, created only
 * when a call really has work to submit. The scatter submission retains
 * the array by pointer, so it must live here, not in the awaiter. */
template <typename scheduler, typename detachedtype, typename stringtype>
struct async_print_work_state
{
	using allocator_type = async_scheduler_allocator_t<scheduler>;
	static inline constexpr bool alloc_with_status{allocator_type::has_status};
	detachedtype detached{};
	stringtype payload{};
	::fast_io::io_scatter_t scatters[2]{};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status,
												   typename allocator_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

/* write completion for a suspended print awaiter: records the error,
 * frees the work state and resumes (or marks done when the suspend was
 * skipped). Typed -- the pair it carries is awaiter-specific anyway. */
template <typename awaiter_type>
struct async_print_write_cb
{
	awaiter_type *awaiter;
	typename awaiter_type::work_state_type *work;
	inline void operator()(::std::cxx_std_error e) noexcept
	{
		awaiter->err = e;
		::fast_io::details::async_delete_state(work);
		if (awaiter->suspended)
		{
			awaiter->coro.resume();
		}
		else
		{
			awaiter->done = true;
		}
	}
};

/*
 * The co_await face of async_print: formatting ran eagerly at call time,
 * so the awaiter carries only the submission parameters plus a work
 * pointer — nullptr when the formatted text landed in the stream's
 * buffer and nothing needs the device, which makes the common buffered
 * case a trivially-ready await on a small object.
 */
template <typename scheduler, typename outstmtype, typename detachedtype, typename stringtype>
struct async_print_awaiter : async_awaiter_result<void>
{
	using char_type = typename stringtype::value_type;
	using work_state_type = async_print_work_state<scheduler, detachedtype, stringtype>;
	scheduler sched;
	outstmtype outstm;
	::fast_io::posix_statx_timestamp_opt timeout;
	work_state_type *work{};

	inline constexpr async_print_awaiter() noexcept = default;
	async_print_awaiter(async_print_awaiter const &) = delete;
	async_print_awaiter &operator=(async_print_awaiter const &) = delete;
	/* the work pointer is owned — move transfers it, never copies */
	inline constexpr async_print_awaiter(async_print_awaiter &&other) noexcept
		: sched{other.sched}, outstm{other.outstm}, timeout{other.timeout}, work{other.work}
	{
		other.work = nullptr;
	}

	/* a never-co_awaited awaiter still owns its unsubmitted state */
	inline ~async_print_awaiter()
	{
		if (work != nullptr)
		{
			::fast_io::details::async_delete_state(work);
		}
	}

	inline constexpr bool await_ready() const noexcept
	{
		return work == nullptr;
	}
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		auto *w{this->work};
		this->work = nullptr;
		::std::size_t nsc{};
		auto [pfirst, pcount]{async_print_detached_pending(w->detached)};
		if (pcount != 0)
		{
			w->scatters[nsc++] = {pfirst, pcount};
		}
		if (::std::size_t const scount{w->payload.size() * sizeof(char_type)}; scount != 0)
		{
			w->scatters[nsc++] = {w->payload.data(), scount};
		}
		async_print_write_cb<async_print_awaiter> callback{this, w};
		if (nsc == 1)
		{
			::fast_io::operations::decay::async_pwrite_all_bytes_decay_callback(
				sched, timeout, outstm, static_cast<::std::byte const *>(w->scatters[0].base),
				w->scatters[0].len, ::fast_io::intfpos_opt{}, callback);
		}
		else
		{
			::fast_io::operations::decay::async_scatter_pwrite_all_bytes_decay_callback(
				sched, timeout, outstm, w->scatters, nsc, ::fast_io::intfpos_opt{},
				callback);
		}
		return this->async_suspend_done();
	}
	inline void await_resume() throws
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

template <bool line, typename async_scheduler_type, typename outstmtype, typename... Args>
inline auto async_print_decay(async_scheduler_type sched,
							  ::fast_io::posix_statx_timestamp_opt timeout, outstmtype outstm,
							  Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		::fast_io::details::decayed_output_stream_print_may_throw<line, outstmtype, Args...> ||
		::fast_io::typed_generic_allocator_adapter<
			::fast_io::operations::decay::output_stream_allocator_t<
				outstmtype, ::fast_io::native_global_allocator>,
			typename outstmtype::output_char_type>::throws_on_allocation_failure)
{
	using char_type = typename outstmtype::output_char_type;
	constexpr bool all_reserve{
		(::fast_io::reserve_printable<char_type, ::std::remove_cvref_t<Args>> && ...)};

	if constexpr (::fast_io::operations::decay::defines::
					  has_async_status_print_define<async_scheduler_type, outstmtype, Args...>)
	{
		/* status streams (e.g. locale imbuer) own their formatting:
		   the define runs the status machinery synchronously into
		   owned storage and returns the submission awaiter */
		return async_status_print_define<line>(sched, timeout, outstm,
											   ::std::forward<Args>(args)...);
	}
	else if constexpr (::fast_io::operations::decay::defines::
						   has_obuffer_flush_reserve_define<outstmtype>)
	{
		/* in-memory strlike target: formatting is the whole job */
		print_freestanding_decay<line>(outstm, ::std::forward<Args>(args)...);
		return ::fast_io::details::async_print_awaiter<
			async_scheduler_type, outstmtype, ::fast_io::details::empty,
			::fast_io::details::async_print_strlike_buffer<char_type,
														   ::fast_io::native_global_allocator>>{};
	}
	else if constexpr (requires {
						   async_iobuffer_print_decay<line>(
							   sched, timeout, outstm, ::std::forward<Args>(args)...);
					   })
	{
		/* basic_io_buffer_ref streams: the buffer-aware path lives with
		 * the io_buffer layer — found by ADL, no freestanding dependency
		 * here (same boundary async_ibuffer_underflow uses) */
		return async_iobuffer_print_decay<line>(sched, timeout, outstm,
												::std::forward<Args>(args)...);
	}

	else
	{
		using payload_alloc_type =
			::fast_io::operations::decay::output_stream_allocator_t<
				outstmtype, ::fast_io::native_global_allocator>;
		using payload_string_type =
			::fast_io::details::async_print_strlike_buffer<char_type, payload_alloc_type>;
		constexpr bool is_buffered{
			::fast_io::operations::decay::defines::has_obuffer_basic_operations<outstmtype>};
		if constexpr (all_reserve && is_buffered)
		{
			constexpr ::std::size_t needed{
				::fast_io::details::compute_total_normal_reserved_size<char_type, line,
																	   Args...>()};
			auto *curr{obuffer_curr(outstm)};
			if (obuffer_begin(outstm) != nullptr &&
				needed <= static_cast<::std::size_t>(obuffer_end(outstm) - curr))
			{
				/* generic buffered stream with room — format into the
				 * obuffer directly */
				template for (constexpr auto i :
							  ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
				{
					curr = ::fast_io::print_reserve_define(
						::fast_io::io_reserve_type<char_type,
												   ::std::remove_cvref_t<Args...[i]>>,
						curr, args...[i]);
				}
				if constexpr (line)
				{
					*curr = ::fast_io::char_literal_v<u8'\n', char_type>;
					++curr;
				}
				obuffer_set_curr(outstm, curr);
				return ::fast_io::details::async_print_awaiter<
					async_scheduler_type, outstmtype, ::fast_io::details::empty,
					payload_string_type>{};
			}
		}
		/* no room or unbuffered: format into a string and submit through
		 * the stream's async write — a buffered stream's define orders it
		 * behind any pending data */
		payload_string_type payload{
			::fast_io::details::basic_general_concat_phase1_decay1_impl<
				line, char_type, payload_string_type>(args...)};
		using ret_awaiter = ::fast_io::details::async_print_awaiter<
			async_scheduler_type, outstmtype, ::fast_io::details::empty, payload_string_type>;
		ret_awaiter ret;
		ret.sched = sched;
		ret.outstm = outstm;
		ret.timeout = timeout;
		ret.work = ::fast_io::details::async_new_state_plain<
			typename ret_awaiter::work_state_type>(sched);
		ret.work->payload = ::std::move(payload);
		return ret;
	}
}

} // namespace fast_io::operations::decay

namespace fast_io::operations
{

/* co_await-able print: formats args at call time, then asynchronously
 * writes the bytes — into the output buffer when it fits, else a
 * detached-buffer/string submission. The async counterpart of
 * print_freestanding: the stream is always explicit here. */
template <bool line = false, typename async_scheduler_type, typename outstmtype, typename... Args>
inline auto async_print_freestanding(async_scheduler_type &&scheduler,
									 ::fast_io::posix_statx_timestamp_opt timeout,
									 outstmtype &&outstm, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	using char_type =
		typename ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(
			outstm))>::output_char_type;
	return ::fast_io::operations::decay::async_print_decay<line>(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm),
		::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
}

} // namespace fast_io::operations

namespace fast_io
{

namespace details
{

/* whether decayed T is an async-print device: an output stream that
 * either carries a strlike buffer (pure formatting target), has the
 * async byte-write define, or owns its status formatting and returns
 * the submission awaiter (e.g. lc_imbuer) */
template <typename schedulertype, typename T, typename... Args>
concept async_print_device = requires {
	::fast_io::operations::output_stream_ref(::std::declval<T &>());
} && (::fast_io::operations::decay::defines::has_obuffer_flush_reserve_define<::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(::std::declval<T &>()))>> || ::fast_io::operations::decay::defines::has_async_pwrite_some_bytes_overflow_callback_define<schedulertype, ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(::std::declval<T &>()))>, ::fast_io::details::async_io_callback> || ::fast_io::operations::decay::defines::has_async_status_print_define<schedulertype, ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(::std::declval<T &>()))>, Args...>);

} // namespace details

inline namespace io
{

/*
 * async_print(sched, timeout, outstm, args...) — co_await-able print on
 * an async output stream: the device is always explicit here, unlike
 * sync print's stdout default — there is no async-capable c_stdout yet.
 * timeout sits ahead of the stream so the two can never be confused.
 */
template <bool line = false, typename async_scheduler_type, typename outstmtype, typename... Args>
inline auto async_print(async_scheduler_type &&scheduler,
						::fast_io::posix_statx_timestamp_opt timeout, outstmtype &&outstm,
						Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
	requires(
		::fast_io::details::async_print_device<
			::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
			outstmtype,
			decltype(::fast_io::io_print_forward<
					 typename ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(
						 outstm))>::output_char_type>(::fast_io::io_print_alias(args)))...>)
{
	return ::fast_io::operations::async_print_freestanding<line>(
		::std::forward<async_scheduler_type>(scheduler), timeout,
		::std::forward<outstmtype>(outstm), ::std::forward<Args>(args)...);
}

template <typename async_scheduler_type, typename outstmtype, typename... Args>
inline auto async_println(async_scheduler_type &&scheduler,
						  ::fast_io::posix_statx_timestamp_opt timeout, outstmtype &&outstm,
						  Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return async_print<true>(::std::forward<async_scheduler_type>(scheduler), timeout,
							 ::std::forward<outstmtype>(outstm), ::std::forward<Args>(args)...);
}

} // namespace io

} // namespace fast_io
