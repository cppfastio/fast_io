#pragma once

namespace fast_io::operations::decay::defines
{

/*
A scheduler observer: the non-owning view of an async submission/completion
engine (io_uring observer, IOCP handle, ...). Every async operation takes it
as the first parameter.
*/
template <typename T>
concept async_scheduler_observer = requires(::std::remove_cvref_t<T> sched) {
	typename ::std::remove_cvref_t<T>::native_handle_type;
	{
		sched.native_handle()
	} -> ::std::same_as<typename ::std::remove_cvref_t<T>::native_handle_type>;
};

/*
Completion callback contracts. `some` operations report bytes (or scatter
progress); `all` operations report only the terminal error. Callbacks are
invoked exactly once and are noexcept.
*/
template <typename func>
concept async_bytes_completion_callback =
	::std::is_nothrow_invocable_v<func, ::std::cxx_std_error, ::std::size_t>;

template <typename func>
concept async_completion_callback = ::std::is_nothrow_invocable_v<func, ::std::cxx_std_error>;

template <typename func>
concept async_scatter_completion_callback =
	::std::is_nothrow_invocable_v<func, ::std::cxx_std_error, ::fast_io::io_scatter_status_t>;

/*
Backend defines. Each scheduler provides these in an associated namespace of
its scheduler type so generic decay functions find them by ADL:

- async_pread_some_bytes_underflow_callback_define /
  async_pwrite_some_bytes_overflow_callback_define are required; everything
  else is built on them generically.
- the scatter defines are optional fast paths (single kernel operation, e.g.
  io_uring READV/WRITEV); the generic layer emulates them element-wise.
- async_transmit_some_bytes_overflow_underflow_callback_define is an
  optional fast path (e.g. io_uring SPLICE); the generic layer emulates it
  with read/write chains.

The _underflow/_overflow infix marks that these defines sit beneath the
buffering layer — for a buffered stream type the same-named define on the
buffered reference (io_buffer/{input,output}_async.h) handles the buffering
itself and delegates to the handle's define only when the device must be
touched.

Defines are noexcept: every error — including submission failures — is
delivered through the callback as ::std::cxx_std_error{domain, code};
domain == nullptr means success (not an error at all). `off` is
intfpos_opt (empty = current file position); `timeout` is
posix_statx_timestamp_opt (empty = none) — the timestamp type doubles as a
duration — and generic `all` loops forward it unchanged to every
resubmission.

Error ownership: a cxx_std_error parameter is an owning handle — the
callee takes ownership of the {domain, code} payload, exactly as
std::error::release() hands it out across the ABI boundary. A callback
that forwards the value transfers ownership onward; the terminal owner
either `throw throws`es it (the error channel reclaims it) or runs its
domain's do_cleanup — details::async_dispose_error / the
cxx_std_error_guard RAII helper do this. This matters for resource-
carrying domains (e.g. a boxed C++ exception handle): dropping such an
error without cleanup leaks the payload.
*/

template <typename schedulertype, typename instmtype, typename functype>
concept has_async_pread_some_bytes_underflow_callback_define =
	requires(schedulertype sched, instmtype instm, functype func) {
		{
			async_pread_some_bytes_underflow_callback_define(
				sched, ::fast_io::posix_statx_timestamp_opt{}, instm,
				static_cast<::std::byte *>(nullptr), ::std::size_t{0},
				::fast_io::intfpos_opt{}, func)
		} noexcept;
	};

template <typename schedulertype, typename outstmtype, typename functype>
concept has_async_pwrite_some_bytes_overflow_callback_define =
	requires(schedulertype sched, outstmtype outstm, functype func) {
		{
			async_pwrite_some_bytes_overflow_callback_define(
				sched, ::fast_io::posix_statx_timestamp_opt{}, outstm,
				static_cast<::std::byte const *>(nullptr), ::std::size_t{0},
				::fast_io::intfpos_opt{}, func)
		} noexcept;
	};

template <typename schedulertype, typename instmtype, typename functype>
concept has_async_scatter_pread_some_bytes_underflow_callback_define =
	requires(schedulertype sched, instmtype instm, functype func) {
		{
			async_scatter_pread_some_bytes_underflow_callback_define(
				sched, ::fast_io::posix_statx_timestamp_opt{}, instm,
				static_cast<::fast_io::io_scatter_t const *>(nullptr), ::std::size_t{0},
				::fast_io::intfpos_opt{}, func)
		} noexcept;
	};

template <typename schedulertype, typename outstmtype, typename functype>
concept has_async_scatter_pwrite_some_bytes_overflow_callback_define =
	requires(schedulertype sched, outstmtype outstm, functype func) {
		{
			async_scatter_pwrite_some_bytes_overflow_callback_define(
				sched, ::fast_io::posix_statx_timestamp_opt{}, outstm,
				static_cast<::fast_io::io_scatter_t const *>(nullptr), ::std::size_t{0},
				::fast_io::intfpos_opt{}, func)
		} noexcept;
	};

template <typename schedulertype, typename outstmtype, typename instmtype, typename functype>
concept has_async_transmit_some_bytes_overflow_underflow_callback_define =
	requires(schedulertype sched, outstmtype outstm, instmtype instm, functype func) {
		{
			async_transmit_some_bytes_overflow_underflow_callback_define(
				sched, ::fast_io::posix_statx_timestamp_opt{}, outstm, ::fast_io::intfpos_opt{},
				instm, ::fast_io::intfpos_opt{}, ::fast_io::size_t_opt{}, func)
		} noexcept;
	};

/*
async_accept_callback_define(sched, timeout, listenstm, mode, func):
asynchronous accept on a listening stream; func is invoked once as
func(::std::cxx_std_error, typename streamtype::native_handle_type) —
the raw accepted handle (fd on posix, SOCKET on win32). `mode` describes
the accepted socket: open_mode::no_block marks it async-capable
(overlapped machinery on win32; a plain fd on posix — io_uring needs no
socket flag). No peer-address capture yet.
*/
template <typename schedulertype, typename streamtype, typename functype>
concept has_async_accept_callback_define =
	::std::is_nothrow_invocable_v<functype, ::std::cxx_std_error,
								  typename streamtype::native_handle_type> &&
	requires(schedulertype sched, streamtype listenstm, functype func) {
		{
			async_accept_callback_define(sched, ::fast_io::posix_statx_timestamp_opt{}, listenstm,
										 ::fast_io::open_mode{}, func)
		} noexcept;
	};

/*
 * async_close_define(sched, timeout, stream, func): asynchronous close of
 * the stream's native handle — io_uring's IORING_OP_CLOSE is the model;
 * backends without a kernel close op emulate it (a thread-pool worker or
 * an inline close whose completion is ferried through the scheduler's
 * pump). The stream argument CARRIES ownership: whatever object held the
 * handle has released it before the define runs (the public wrappers do
 * this; a basic_io_buffer_ref's buffered define releases handle itself
 * after flushing), and the operation closes it exactly once — even on
 * submission failure the backend performs the close so it cannot leak.
 * func is invoked once as func(::std::cxx_std_error) noexcept. The
 * timeout is advisory — emulated closes may ignore it.
 */
template <typename schedulertype, typename stmtype, typename functype>
concept has_async_close_define =
	::fast_io::operations::decay::defines::async_completion_callback<functype> &&
	requires(schedulertype sched, stmtype stm, functype func) {
		{
			async_close_define(sched, ::fast_io::posix_statx_timestamp_opt{}, stm, func)
		} noexcept;
	};

/*
 * async_connect_define(sched, timeout, stream, addr, addrlen, func):
 * asynchronous connect on an existing unconnected stream socket —
 * io_uring's IORING_OP_CONNECT and winsock's ConnectEx are the models;
 * backends without a kernel connect op run a blocking connect() on a
 * worker. addr/addrlen are the peer address in the platform's sockaddr
 * wire layout; the backend copies them at submission, so the caller's
 * buffer need not outlive the call. Unlike async_close the stream is
 * NOT consumed — on success it is connected and usable for I/O; on
 * error its state follows the platform's failed connect() semantics
 * (close and recreate it). func is invoked once as
 * func(::std::cxx_std_error) noexcept.
 */
template <typename schedulertype, typename streamtype, typename functype>
concept has_async_connect_define =
	::fast_io::operations::decay::defines::async_completion_callback<functype> &&
	requires(schedulertype sched, streamtype stm, functype func) {
		{
			async_connect_define(sched, ::fast_io::posix_statx_timestamp_opt{}, stm,
								 static_cast<void const *>(nullptr), ::std::size_t{0}, func)
		} noexcept;
	};

/*
 * The owning file type an async accept yields for a given listen-stream
 * observer — each backend specializes this on its observer type (posix
 * io_observer -> basic_posix_family_file, win32 socket observer ->
 * basic_win32_family_socket_file). Deliberately no primary: a stream
 * without a backend cannot accept. Keeps the generic awaiter free of
 * hosted-layer names.
 */
template <typename streamtype>
struct async_accept_file_type;

template <typename streamtype>
using async_accept_file_t = typename async_accept_file_type<streamtype>::type;

/*
 * async_status_print_define<line>(sched, timeout, stream, args...) is the
 * async counterpart of status_print_define: streams carrying print state
 * (e.g. locale imbuer) implement it so async_print can format through the
 * status machinery into owned storage and write the bytes out
 * asynchronously. It returns an awaiter whose await_resume reports the
 * write result.
 */
template <typename schedulertype, typename outstmtype, typename... Args>
concept has_async_status_print_define =
	requires(schedulertype sched, outstmtype outstm, Args &&...args) {
		async_status_print_define<false>(sched, ::fast_io::posix_statx_timestamp_opt{}, outstm,
										 ::std::forward<Args>(args)...);
	};

/*
Buffered-stream defines — both live beneath their public wrappers and are
likewise noexcept with callback-delivered errors:

- async_ibuffer_underflow(sched, timeout, instm, cb): refill the input
  buffer's pending window; cb(err, n) reports the bytes made available
  (n == 0 = end of file). The buffering equivalent of
  ibuffer_underflow(iobref).
- async_output_stream_buffer_flush_define(sched, timeout, outstm, cb):
  write every pending output byte to the underlying handle; cb(err).
*/
template <typename schedulertype, typename instmtype, typename functype>
concept has_async_ibuffer_underflow = requires(schedulertype sched, instmtype instm, functype func) {
	{
		async_ibuffer_underflow(sched, ::fast_io::posix_statx_timestamp_opt{}, instm, func)
	} noexcept;
};

template <typename schedulertype, typename outstmtype, typename functype>
concept has_async_output_stream_buffer_flush_define =
	requires(schedulertype sched, outstmtype outstm, functype func) {
		{
			async_output_stream_buffer_flush_define(sched, ::fast_io::posix_statx_timestamp_opt{},
													outstm, func)
		} noexcept;
	};

template <typename schedulertype, typename instmtype>
concept async_preadable =
	::fast_io::operations::decay::defines::async_scheduler_observer<schedulertype> &&
	::fast_io::operations::decay::defines::has_async_pread_some_bytes_underflow_callback_define<
		schedulertype, instmtype,
		::fast_io::details::async_io_callback>;

template <typename schedulertype, typename outstmtype>
concept async_pwritable =
	::fast_io::operations::decay::defines::async_scheduler_observer<schedulertype> &&
	::fast_io::operations::decay::defines::has_async_pwrite_some_bytes_overflow_callback_define<
		schedulertype, outstmtype,
		::fast_io::details::async_io_callback>;

} // namespace fast_io::operations::decay::defines

namespace fast_io::operations
{

/*
 * Scheduler-to-observer reduction — the scheduler's analog of
 * input_stream_ref/output_stream_ref. Public async entry points take the
 * scheduler by forwarding reference and reduce it here: an owning
 * scheduler (e.g. linux_io_uring) decays through its
 * async_scheduler_ref_define to the trivially-copyable observer
 * (linux_io_uring_observer) the decay layer passes by value; a type that
 * is already an observer just copies through.
 */
template <typename T>
	requires(requires(T &&t) {
		async_scheduler_ref_define(::fast_io::freestanding::forward<T>(t));
	} ||
			 (::fast_io::operations::decay::defines::async_scheduler_observer<
				  ::std::remove_cvref_t<T>> &&
			  ::std::is_trivially_copyable_v<::std::remove_cvref_t<T>>))
inline constexpr decltype(auto) async_scheduler_ref(T &&t) noexcept
{
	if constexpr (requires { async_scheduler_ref_define(::fast_io::freestanding::forward<T>(t)); })
	{
		return async_scheduler_ref_define(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return ::std::remove_cvref_t<T>(t);
	}
}

} // namespace fast_io::operations
