#pragma once

namespace fast_io::operations
{

/*
 * Public async surface. Users name these; the decay infix names in
 * fast_io::operations::decay are for implementations that already hold
 * reduced types. Exactly like the sync layer, the handles are reduced
 * here — scheduler through async_scheduler_ref (owning ring → its
 * trivially-copyable observer), streams through input_stream_ref/
 * output_stream_ref (buffered stream → basic_io_buffer_ref) — and the
 * decay layer sees only small by-value types, so && qualifications never
 * multiply template instantiations.
 */

/* ------------------------------ pread ------------------------------ */

template <typename async_scheduler_type, typename instmtype, typename callback_type>
inline void async_pread_some_bytes_callback(async_scheduler_type &&scheduler,
											::fast_io::posix_statx_timestamp_opt timeout,
											instmtype &&instm, ::std::byte *first,
											::std::size_t count, ::fast_io::intfpos_opt off,
											callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 ::std::remove_cvref_t<callback_type>>)
{
	::fast_io::operations::decay::async_pread_some_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::input_stream_ref(instm), first, count, off,
		::std::move(callback));
}

template <typename async_scheduler_type, typename instmtype, typename callback_type>
inline void async_pread_all_bytes_callback(async_scheduler_type &&scheduler,
										   ::fast_io::posix_statx_timestamp_opt timeout,
										   instmtype &&instm, ::std::byte *first,
										   ::std::size_t count, ::fast_io::intfpos_opt off,
										   callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 ::fast_io::details::async_io_callback>)
{
	::fast_io::operations::decay::async_pread_all_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::input_stream_ref(instm), first, count, off,
		::std::move(callback));
}

template <typename async_scheduler_type, typename instmtype>
inline auto async_pread_some_bytes(async_scheduler_type &&scheduler,
								   ::fast_io::posix_statx_timestamp_opt timeout,
								   instmtype &&instm, ::std::byte *first,
								   ::std::size_t count, ::fast_io::intfpos_opt off) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 ::fast_io::details::async_io_callback>)
{
	return ::fast_io::operations::decay::async_pread_some_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::input_stream_ref(instm), first, count, off);
}

template <typename async_scheduler_type, typename instmtype>
inline auto async_pread_all_bytes(async_scheduler_type &&scheduler,
								  ::fast_io::posix_statx_timestamp_opt timeout,
								  instmtype &&instm, ::std::byte *first,
								  ::std::size_t count, ::fast_io::intfpos_opt off) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 ::fast_io::details::async_io_callback>)
{
	return ::fast_io::operations::decay::async_pread_all_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::input_stream_ref(instm), first, count, off);
}

/* ------------------------------ pwrite ----------------------------- */

template <typename async_scheduler_type, typename outstmtype, typename callback_type>
inline void async_pwrite_some_bytes_callback(async_scheduler_type &&scheduler,
											 ::fast_io::posix_statx_timestamp_opt timeout,
											 outstmtype &&outstm, ::std::byte const *first,
											 ::std::size_t count, ::fast_io::intfpos_opt off,
											 callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::std::remove_cvref_t<callback_type>>)
{
	::fast_io::operations::decay::async_pwrite_some_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm), first, count, off,
		::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype, typename callback_type>
inline void async_pwrite_all_bytes_callback(async_scheduler_type &&scheduler,
											::fast_io::posix_statx_timestamp_opt timeout,
											outstmtype &&outstm, ::std::byte const *first,
											::std::size_t count, ::fast_io::intfpos_opt off,
											callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::fast_io::details::async_io_callback>)
{
	::fast_io::operations::decay::async_pwrite_all_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm), first, count, off,
		::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype>
inline auto async_pwrite_some_bytes(async_scheduler_type &&scheduler,
									::fast_io::posix_statx_timestamp_opt timeout,
									outstmtype &&outstm, ::std::byte const *first,
									::std::size_t count, ::fast_io::intfpos_opt off) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::fast_io::details::async_io_callback>)
{
	return ::fast_io::operations::decay::async_pwrite_some_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm), first, count, off);
}

template <typename async_scheduler_type, typename outstmtype>
inline auto async_pwrite_all_bytes(async_scheduler_type &&scheduler,
								   ::fast_io::posix_statx_timestamp_opt timeout,
								   outstmtype &&outstm, ::std::byte const *first,
								   ::std::size_t count, ::fast_io::intfpos_opt off) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::fast_io::details::async_io_callback>)
{
	return ::fast_io::operations::decay::async_pwrite_all_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm), first, count, off);
}

/* ------------------------------ scatter ---------------------------- */

template <typename async_scheduler_type, typename instmtype, typename callback_type>
inline void async_scatter_pread_some_bytes_callback(async_scheduler_type &&scheduler,
													::fast_io::posix_statx_timestamp_opt timeout,
													instmtype &&instm,
													::fast_io::io_scatter_t const *pscatters,
													::std::size_t n, ::fast_io::intfpos_opt off,
													callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_scatter_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 ::std::remove_cvref_t<callback_type>> ||
			 ::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 ::fast_io::details::async_io_callback>)
{
	::fast_io::operations::decay::async_scatter_pread_some_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::input_stream_ref(instm), pscatters, n, off,
		::std::move(callback));
}

template <typename async_scheduler_type, typename instmtype, typename callback_type>
inline void async_scatter_pread_all_bytes_callback(async_scheduler_type &&scheduler,
												   ::fast_io::posix_statx_timestamp_opt timeout,
												   instmtype &&instm,
												   ::fast_io::io_scatter_t const *pscatters,
												   ::std::size_t n, ::fast_io::intfpos_opt off,
												   callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 ::fast_io::details::async_io_callback>)
{
	::fast_io::operations::decay::async_scatter_pread_all_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::input_stream_ref(instm), pscatters, n, off,
		::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype, typename callback_type>
inline void async_scatter_pwrite_some_bytes_callback(async_scheduler_type &&scheduler,
													 ::fast_io::posix_statx_timestamp_opt timeout,
													 outstmtype &&outstm,
													 ::fast_io::io_scatter_t const *pscatters,
													 ::std::size_t n, ::fast_io::intfpos_opt off,
													 callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_scatter_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::std::remove_cvref_t<callback_type>> ||
			 ::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::fast_io::details::async_io_callback>)
{
	::fast_io::operations::decay::async_scatter_pwrite_some_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm), pscatters, n, off,
		::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype, typename callback_type>
inline void async_scatter_pwrite_all_bytes_callback(async_scheduler_type &&scheduler,
													::fast_io::posix_statx_timestamp_opt timeout,
													outstmtype &&outstm,
													::fast_io::io_scatter_t const *pscatters,
													::std::size_t n, ::fast_io::intfpos_opt off,
													callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::fast_io::details::async_io_callback>)
{
	::fast_io::operations::decay::async_scatter_pwrite_all_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm), pscatters, n, off,
		::std::move(callback));
}

template <typename async_scheduler_type, typename instmtype>
inline auto async_scatter_pread_some_bytes(async_scheduler_type &&scheduler,
										   ::fast_io::posix_statx_timestamp_opt timeout,
										   instmtype &&instm,
										   ::fast_io::io_scatter_t const *pscatters,
										   ::std::size_t n,
										   ::fast_io::intfpos_opt off) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_scatter_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 ::fast_io::details::async_io_scatter_callback> ||
			 ::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 ::fast_io::details::async_io_callback>)
{
	return ::fast_io::operations::decay::async_scatter_pread_some_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::input_stream_ref(instm), pscatters, n, off);
}

template <typename async_scheduler_type, typename instmtype>
inline auto async_scatter_pread_all_bytes(async_scheduler_type &&scheduler,
										  ::fast_io::posix_statx_timestamp_opt timeout,
										  instmtype &&instm,
										  ::fast_io::io_scatter_t const *pscatters,
										  ::std::size_t n,
										  ::fast_io::intfpos_opt off) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 ::fast_io::details::async_io_callback>)
{
	return ::fast_io::operations::decay::async_scatter_pread_all_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::input_stream_ref(instm), pscatters, n, off);
}

template <typename async_scheduler_type, typename outstmtype>
inline auto async_scatter_pwrite_some_bytes(async_scheduler_type &&scheduler,
											::fast_io::posix_statx_timestamp_opt timeout,
											outstmtype &&outstm,
											::fast_io::io_scatter_t const *pscatters,
											::std::size_t n,
											::fast_io::intfpos_opt off) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_scatter_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::fast_io::details::async_io_scatter_callback> ||
			 ::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::fast_io::details::async_io_callback>)
{
	return ::fast_io::operations::decay::async_scatter_pwrite_some_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm), pscatters, n, off);
}

template <typename async_scheduler_type, typename outstmtype>
inline auto async_scatter_pwrite_all_bytes(async_scheduler_type &&scheduler,
										   ::fast_io::posix_statx_timestamp_opt timeout,
										   outstmtype &&outstm,
										   ::fast_io::io_scatter_t const *pscatters,
										   ::std::size_t n,
										   ::fast_io::intfpos_opt off) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::fast_io::details::async_io_callback>)
{
	return ::fast_io::operations::decay::async_scatter_pwrite_all_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm), pscatters, n, off);
}

/* ----------------------------- transmit ---------------------------- */

template <typename async_scheduler_type, typename outstmtype, typename instmtype,
		  typename callback_type>
inline void async_transmit_some_bytes_callback(async_scheduler_type &&scheduler,
											   ::fast_io::posix_statx_timestamp_opt timeout,
											   outstmtype &&outstm,
											   ::fast_io::intfpos_opt off_out, instmtype &&instm,
											   ::fast_io::intfpos_opt off_in,
											   ::fast_io::size_t_opt bound,
											   callback_type callback) noexcept
{
	::fast_io::operations::decay::async_transmit_some_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm), off_out,
		::fast_io::operations::input_stream_ref(instm), off_in, bound,
		::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype, typename instmtype,
		  typename callback_type>
inline void async_transmit_all_bytes_callback(async_scheduler_type &&scheduler,
											  ::fast_io::posix_statx_timestamp_opt timeout,
											  outstmtype &&outstm,
											  ::fast_io::intfpos_opt off_out, instmtype &&instm,
											  ::fast_io::intfpos_opt off_in,
											  ::fast_io::size_t_opt bound,
											  callback_type callback) noexcept
{
	::fast_io::operations::decay::async_transmit_all_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm), off_out,
		::fast_io::operations::input_stream_ref(instm), off_in, bound,
		::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype, typename instmtype>
inline auto async_transmit_some_bytes(async_scheduler_type &&scheduler,
									  ::fast_io::posix_statx_timestamp_opt timeout,
									  outstmtype &&outstm,
									  ::fast_io::intfpos_opt off_out, instmtype &&instm,
									  ::fast_io::intfpos_opt off_in,
									  ::fast_io::size_t_opt bound) noexcept
{
	return ::fast_io::operations::decay::async_transmit_some_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm), off_out,
		::fast_io::operations::input_stream_ref(instm), off_in, bound);
}

template <typename async_scheduler_type, typename outstmtype, typename instmtype>
inline auto async_transmit_all_bytes(async_scheduler_type &&scheduler,
									 ::fast_io::posix_statx_timestamp_opt timeout,
									 outstmtype &&outstm,
									 ::fast_io::intfpos_opt off_out, instmtype &&instm,
									 ::fast_io::intfpos_opt off_in,
									 ::fast_io::size_t_opt bound) noexcept
{
	return ::fast_io::operations::decay::async_transmit_all_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm), off_out,
		::fast_io::operations::input_stream_ref(instm), off_in, bound);
}

/* ------------------------------- flush ----------------------------- */

template <typename async_scheduler_type, typename outstmtype, typename callback_type>
inline void async_output_stream_flush_callback(async_scheduler_type &&scheduler,
											   ::fast_io::posix_statx_timestamp_opt timeout,
											   outstmtype &&outstm,
											   callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_output_stream_buffer_flush_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::std::remove_cvref_t<callback_type>>)
{
	::fast_io::operations::decay::async_output_stream_flush_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm), ::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype>
inline auto async_output_stream_flush(async_scheduler_type &&scheduler,
									  ::fast_io::posix_statx_timestamp_opt timeout,
									  outstmtype &&outstm) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_output_stream_buffer_flush_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::fast_io::details::async_io_error_callback>)
{
	return ::fast_io::operations::decay::async_output_stream_flush_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::output_stream_ref(outstm));
}

/* ------------------------------ accept ------------------------------ */

/*
 * async_accept: co_await suspends until a connection arrives and yields
 * the accepted socket's native handle — int on posix, SOCKET on win32.
 * Wrap it in native_socket_file:
 *     fi::native_socket_file s{co_await fop::async_accept(sched, {}, listener)};
 * mode describes the accepted socket (no_block marks it async-capable;
 * required for IOCP sockets, ignored on posix — io_uring needs no flag).
 */
template <typename async_scheduler_type, typename streamtype>
inline auto async_accept(async_scheduler_type &&scheduler,
						 ::fast_io::posix_statx_timestamp_opt timeout, streamtype &&listenstm,
						 ::fast_io::open_mode mode = ::fast_io::open_mode{}) noexcept
	requires(::fast_io::operations::decay::defines::has_async_accept_callback_define<
			 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
			 ::std::remove_cvref_t<decltype(::fast_io::operations::io_stream_ref(listenstm))>,
			 ::fast_io::details::async_io_accept_callback<
				 typename ::std::remove_cvref_t<
					 decltype(::fast_io::operations::io_stream_ref(listenstm))>::
					 native_handle_type>>)
{
	return ::fast_io::operations::decay::async_accept_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::io_stream_ref(listenstm), mode);
}

template <typename async_scheduler_type, typename streamtype, typename callback_type>
inline void async_accept_callback(async_scheduler_type &&scheduler,
								  ::fast_io::posix_statx_timestamp_opt timeout,
								  streamtype &&listenstm, ::fast_io::open_mode mode,
								  callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::has_async_accept_callback_define<
			 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
			 ::std::remove_cvref_t<decltype(::fast_io::operations::io_stream_ref(listenstm))>,
			 ::std::remove_cvref_t<callback_type>>)
{
	::fast_io::operations::decay::async_accept_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::io_stream_ref(listenstm), mode, ::std::move(callback));
}

/* ------------------------------ connect ----------------------------- */

/*
 * async_connect: connects an existing unconnected stream socket to a
 * peer address without blocking the caller — io_uring's
 * IORING_OP_CONNECT and winsock's ConnectEx are the models. addr/addrlen
 * are the peer's sockaddr in the platform's wire layout and are copied
 * at submission. The socket is NOT consumed: on success it is connected
 * and usable for I/O; on error it follows the platform's failed
 * connect() semantics (close it and create a fresh socket to retry).
 *
 * Callback form invokes callback(::std::cxx_std_error) once; the
 * coroutine form suspends until the connect resolves and
 * await_resume() rethrows the error through the channel.
 */
template <typename async_scheduler_type, typename streamtype>
inline auto async_connect(async_scheduler_type &&scheduler,
						  ::fast_io::posix_statx_timestamp_opt timeout, streamtype &&stm,
						  void const *addr, ::std::size_t addrlen) noexcept
	requires(::fast_io::operations::decay::defines::has_async_connect_define<
			 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
			 ::std::remove_cvref_t<decltype(::fast_io::operations::io_stream_ref(stm))>,
			 ::fast_io::details::async_io_error_callback>)
{
	return ::fast_io::operations::decay::async_connect_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::io_stream_ref(stm), addr, addrlen);
}

template <typename async_scheduler_type, typename streamtype, typename callback_type>
inline void async_connect_callback(async_scheduler_type &&scheduler,
								   ::fast_io::posix_statx_timestamp_opt timeout, streamtype &&stm,
								   void const *addr, ::std::size_t addrlen,
								   callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::has_async_connect_define<
			 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
			 ::std::remove_cvref_t<decltype(::fast_io::operations::io_stream_ref(stm))>,
			 ::std::remove_cvref_t<callback_type>>)
{
	::fast_io::operations::decay::async_connect_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::io_stream_ref(stm), addr, addrlen, ::std::move(callback));
}

/* ------------------------------- close ------------------------------ */

/*
 * async_close: closes the stream's native handle asynchronously. The
 * operation TAKES the handle — any stream exposing release() (file,
 * socket, observer) is detached immediately at this call, so the
 * object's destructor cannot close it again; a buffered stream keeps
 * owning its handle object but the buffered define releases it after
 * flushing pending output.
 *
 * Callback form invokes callback(::std::cxx_std_error) once; the
 * coroutine form suspends until the handle is closed and await_resume()
 * rethrows the error through the channel.
 */
template <typename async_scheduler_type, typename streamtype>
inline auto async_close(async_scheduler_type &&scheduler,
						::fast_io::posix_statx_timestamp_opt timeout, streamtype &&stm) noexcept
	requires(::fast_io::operations::decay::defines::has_async_close_define<
			 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
			 ::std::remove_cvref_t<decltype(::fast_io::details::async_close_stream_ref(stm))>,
			 ::fast_io::details::async_io_error_callback>)
{
	auto stmref{::fast_io::details::async_close_stream_ref(stm)};
	if constexpr (requires { stm.release(); })
	{
		stm.release();
	}
	return ::fast_io::operations::decay::async_close_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout, stmref);
}

template <typename async_scheduler_type, typename streamtype, typename callback_type>
inline void async_close_callback(async_scheduler_type &&scheduler,
								 ::fast_io::posix_statx_timestamp_opt timeout, streamtype &&stm,
								 callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::has_async_close_define<
			 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
			 ::std::remove_cvref_t<decltype(::fast_io::details::async_close_stream_ref(stm))>,
			 ::std::remove_cvref_t<callback_type>>)
{
	auto stmref{::fast_io::details::async_close_stream_ref(stm)};
	if constexpr (requires { stm.release(); })
	{
		stm.release();
	}
	::fast_io::operations::decay::async_close_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout, stmref,
		::std::move(callback));
}

} // namespace fast_io::operations
