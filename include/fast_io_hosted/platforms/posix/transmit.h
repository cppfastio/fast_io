#pragma once

namespace fast_io
{

/*
Handle type produced by input_transmit_handle_define/output_transmit_handle_define
for posix file descriptors. It merely wraps the fd and signals that the object
can participate in fd-level zero-copy transmits (copy_file_range/sendfile/
splice where the OS supports them, read/write emulation otherwise).
*/
struct posix_transmit_entry
{
	int fd{-1};
	inline explicit constexpr posix_transmit_entry() noexcept = default;
	inline explicit constexpr posix_transmit_entry(int fdd) noexcept
		: fd(fdd)
	{}
};

template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr posix_transmit_entry input_transmit_handle_define(
	basic_posix_family_io_observer<family, ch_type> piob) noexcept
{
	return posix_transmit_entry{piob.fd};
}

template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr posix_transmit_entry output_transmit_handle_define(
	basic_posix_family_io_observer<family, ch_type> piob) noexcept
{
	return posix_transmit_entry{piob.fd};
}

namespace details
{

#if defined(__linux__) && \
	(defined(__NR_copy_file_range) || defined(__NR_sendfile) || defined(__NR_sendfile64) || defined(__NR_splice))
#define FAST_IO_POSIX_TRANSMIT_LINUX_DISPATCHED 1

/*
Kernel bounce buffer for fpos_nullable_ptr. The kernel writes back through its
own fixed-width offset pointer types (loff_t/off_t, which on 32-bit targets are
narrower than intfpos_t), so we hand the syscall a local and copy the updated
value out on scope exit — before any thrown error propagates, matching the OS
semantics of updating *off even for partial transfers.
*/
template <::std::signed_integral inttype>
struct linux_transmit_fpos_writeback
{
	inttype val{};
	::fast_io::intfpos_t *dst{};
	inline linux_transmit_fpos_writeback(::fast_io::fpos_nullable_ptr p) noexcept
		: dst{p.ptr}
	{
		if (dst != nullptr)
		{
			val = static_cast<inttype>(*dst);
		}
	}
	inline inttype *get() noexcept
	{
		return dst == nullptr ? nullptr : __builtin_addressof(val);
	}
	inline ~linux_transmit_fpos_writeback() noexcept
	{
		if (dst != nullptr)
		{
			*dst = static_cast<::fast_io::intfpos_t>(val);
		}
	}
};

/*
errno values meaning "this method cannot do it, try the next one" rather than
a real I/O error. EBADF also covers kernel rejections for open-mode flags a
method cannot serve (e.g. O_APPEND on the output fd for copy_file_range/
sendfile); a truly invalid fd fails again in the emulation path.
*/
inline constexpr bool linux_transmit_syscall_unsupported(::std::ptrdiff_t ret) noexcept
{
	return ret == -ENOSYS || ret == -EINVAL || ret == -EXDEV || ret == -EOPNOTSUPP || ret == -EPERM ||
		   ret == -EBADF;
}

#endif

/*
Single-round fd emulation: read/pread into a temporary buffer then write_all/
pwrite_all. Bytes only count as transmitted once they actually reached the
output; off_in/off_out point past the last byte transferred on return.
*/
template <typename inobtype, typename outobtype>
inline ::std::size_t posix_transmit_bytes_emulate_impl(
	outobtype outob, ::fast_io::fpos_nullable_ptr off_out,
	inobtype inob, ::fast_io::fpos_nullable_ptr off_in, ::std::size_t size)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr ::std::size_t bfsz{::fast_io::details::transmit_buffer_size_cache<1>};
	::std::size_t this_round{size < bfsz ? size : bfsz};
	if (this_round == 0)
	{
		return 0;
	}
	::fast_io::details::local_operator_new_array_ptr<::std::byte> newptr(this_round);
	::std::byte *buffer_start{newptr.ptr};
	::std::byte *iter;
	if (off_in.ptr != nullptr)
	{
		if constexpr (::fast_io::operations::decay::defines::bytes_preadable<inobtype>)
		{
			iter = ::fast_io::operations::decay::pread_some_bytes_decay(inob, buffer_start, this_round, *off_in.ptr);
		}
		else
		{
			::fast_io::herbceptions::throws_errc(::std::errc::invalid_seek);
		}
	}
	else
	{
		iter = ::fast_io::operations::decay::read_some_bytes_decay(inob, buffer_start, this_round);
	}
	::std::size_t got{static_cast<::std::size_t>(iter - buffer_start)};
	if (got == 0)
	{
		return 0;
	}
	if (off_out.ptr != nullptr)
	{
		if constexpr (::fast_io::operations::decay::defines::bytes_pwritable<outobtype>)
		{
			::fast_io::operations::decay::pwrite_all_bytes_decay(outob, buffer_start, got, *off_out.ptr);
		}
		else
		{
			::fast_io::herbceptions::throws_errc(::std::errc::invalid_seek);
		}
	}
	else
	{
		::fast_io::operations::decay::write_all_bytes_decay(outob, buffer_start, got);
	}
	if (off_in.ptr != nullptr)
	{
		*off_in.ptr = ::fast_io::fposoffadd_nonegative(*off_in.ptr, got);
	}
	if (off_out.ptr != nullptr)
	{
		*off_out.ptr = ::fast_io::fposoffadd_nonegative(*off_out.ptr, got);
	}
	return got;
}

inline ::std::size_t posix_transmit_some_bytes_dispatch_impl(
	int fd_out, [[maybe_unused]] ::fast_io::file_type outtype, ::fast_io::fpos_nullable_ptr off_out,
	int fd_in, [[maybe_unused]] ::fast_io::file_type intype, ::fast_io::fpos_nullable_ptr off_in,
	::std::size_t size)
	FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(FAST_IO_POSIX_TRANSMIT_LINUX_DISPATCHED)
	// syscalls return ssize_t; a length above SSIZE_MAX is invalid
	constexpr auto mx{::std::numeric_limits<::std::ptrdiff_t>::max()};
	if (static_cast<::std::size_t>(mx) < size)
	{
		size = static_cast<::std::size_t>(mx);
	}
	// errno values returned by the methods below which mean "not applicable,
	// keep going down the chain"
	if (intype == ::fast_io::file_type::regular)
	{
		if (outtype == ::fast_io::file_type::regular)
		{
#if defined(__NR_copy_file_range)
			::fast_io::details::linux_transmit_fpos_writeback<::std::int_least64_t> win{off_in}, wout{off_out};
			auto ret{::fast_io::system_call<__NR_copy_file_range, ::std::ptrdiff_t>(fd_in, win.get(), fd_out,
																					wout.get(), size, 0u)};
			if (!::fast_io::linux_system_call_fails(ret)) [[likely]]
			{
				return static_cast<::std::size_t>(ret);
			}
			if (!::fast_io::details::linux_transmit_syscall_unsupported(ret))
			{
				::fast_io::linux_system_call_throw_error(ret);
			}
#endif
		}
		if (off_out.ptr == nullptr)
		{
#if defined(__NR_sendfile64) || defined(__NR_sendfile)
			/*
			sendfile(2) sends from the input fd at &off_in (or its position) to
			the output fd at its own position — there is no output offset, so an
			explicit off_out disqualifies this method.
			*/
			::fast_io::details::linux_transmit_fpos_writeback<
#if defined(__NR_sendfile64)
				::std::int_least64_t
#else
				::std::ptrdiff_t
#endif
				>
				win{off_in};
			auto ret{
#if defined(__NR_sendfile64)
				::fast_io::system_call<__NR_sendfile64, ::std::ptrdiff_t>
#else
				::fast_io::system_call<__NR_sendfile, ::std::ptrdiff_t>
#endif
				(fd_out, fd_in, win.get(), size)};
			if (!::fast_io::linux_system_call_fails(ret)) [[likely]]
			{
				return static_cast<::std::size_t>(ret);
			}
			if (!::fast_io::details::linux_transmit_syscall_unsupported(ret))
			{
				::fast_io::linux_system_call_throw_error(ret);
			}
#endif
		}
	}
	if (intype == ::fast_io::file_type::fifo || outtype == ::fast_io::file_type::fifo)
	{
#if defined(__NR_splice)
		/*
		splice(2) requires at least one end to be a pipe, and the pipe end must
		get a null offset — an explicit offset on a pipe is invalid and rejected
		the way the kernel would reject it.
		*/
		if ((intype == ::fast_io::file_type::fifo && off_in.ptr != nullptr) ||
			(outtype == ::fast_io::file_type::fifo && off_out.ptr != nullptr))
		{
			::fast_io::throw_posix_error(EINVAL);
		}
		::fast_io::details::linux_transmit_fpos_writeback<::std::int_least64_t> win{off_in}, wout{off_out};
		auto ret{::fast_io::system_call<__NR_splice, ::std::ptrdiff_t>(fd_in, win.get(), fd_out, wout.get(), size,
																	   0u)};
		if (!::fast_io::linux_system_call_fails(ret)) [[likely]]
		{
			return static_cast<::std::size_t>(ret);
		}
		if (!::fast_io::details::linux_transmit_syscall_unsupported(ret))
		{
			::fast_io::linux_system_call_throw_error(ret);
		}
#endif
	}
#endif
	return ::fast_io::details::posix_transmit_bytes_emulate_impl(
		::fast_io::basic_posix_io_observer<char>{fd_out}, off_out,
		::fast_io::basic_posix_io_observer<char>{fd_in}, off_in, size);
}

} // namespace details

inline ::std::size_t transmit_some_bytes_overflow_underflow_define(
	::fast_io::posix_transmit_entry oute, ::fast_io::fpos_nullable_ptr off_out,
	::fast_io::posix_transmit_entry ine, ::fast_io::fpos_nullable_ptr off_in, ::std::size_t size)
	FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(FAST_IO_POSIX_TRANSMIT_LINUX_DISPATCHED)
	auto const intype{::fast_io::details::fstat_impl(ine.fd).type};
	auto const outtype{::fast_io::details::fstat_impl(oute.fd).type};
#else
	constexpr ::fast_io::file_type intype{::fast_io::file_type::unknown};
	constexpr ::fast_io::file_type outtype{::fast_io::file_type::unknown};
#endif
	return ::fast_io::details::posix_transmit_some_bytes_dispatch_impl(oute.fd, outtype, off_out, ine.fd, intype,
																	   off_in, size);
}

inline void transmit_all_bytes_overflow_underflow_define(
	::fast_io::posix_transmit_entry oute, ::fast_io::fpos_nullable_ptr off_out,
	::fast_io::posix_transmit_entry ine, ::fast_io::fpos_nullable_ptr off_in, ::fast_io::size_t_opt totransmit)
	FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(FAST_IO_POSIX_TRANSMIT_LINUX_DISPATCHED)
	auto const intype{::fast_io::details::fstat_impl(ine.fd).type};
	auto const outtype{::fast_io::details::fstat_impl(oute.fd).type};
#else
	constexpr ::fast_io::file_type intype{::fast_io::file_type::unknown};
	constexpr ::fast_io::file_type outtype{::fast_io::file_type::unknown};
#endif
	for (;;)
	{
		::std::size_t want{totransmit.has_opt ? totransmit.opt : ::std::numeric_limits<::std::size_t>::max()};
		if (want == 0)
		{
			return;
		}
		::std::size_t got{::fast_io::details::posix_transmit_some_bytes_dispatch_impl(oute.fd, outtype, off_out,
																					  ine.fd, intype, off_in, want)};
		if (got == 0)
		{
			return;
		}
		if (totransmit.has_opt)
		{
			totransmit.opt -= got;
		}
	}
}

} // namespace fast_io
