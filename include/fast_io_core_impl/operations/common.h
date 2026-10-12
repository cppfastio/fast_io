#pragma once

namespace fast_io
{

template <typename T>
inline constexpr io_scatter_status_t scatter_size_to_status(::std::size_t sz, basic_io_scatter_t<T> const *base,
															::std::size_t len) noexcept
{
	::std::size_t total{sz};
	for (::std::size_t i{}; i != len; ++i)
	{
		::std::size_t blen{base[i].len};
		if (total < blen) [[unlikely]]
		{
			return {i, total};
		}
		total -= blen;
	}
	return {len, 0};
}

namespace details
{
// A buffer holds `inbuf` bytes which are a contiguous slice of the scatter stream ending
// `lastendpos` bytes into scatter `base[n - 1]` (pass `lastendpos == base[n - 1].len` when
// the slice ends at that scatter's end). `unwritten` bytes at the buffer tail were not
// transferred. Walk backwards to locate the scatter position of the first untransferred
// byte.
template <typename T>
inline constexpr io_scatter_status_t scatter_locate_backward(basic_io_scatter_t<T> const *base, ::std::size_t n,
															 ::std::size_t lastendpos, ::std::size_t unwritten,
															 ::std::size_t inbuf) noexcept
{
	::std::size_t endpos{lastendpos};
	for (auto pj{base + n};;)
	{
		--pj;
		::std::size_t copied{endpos < inbuf ? endpos : inbuf};
		if (unwritten <= copied)
		{
			return {static_cast<::std::size_t>(pj - base), endpos - unwritten};
		}
		unwritten -= copied;
		inbuf -= copied;
		endpos = pj[-1].len;
	}
}

template <typename T>
struct basic_scatter_total_size_overflow_result
{
	T total_size{};
	::std::size_t position{};
};

// Emulates scatter-write on platforms without a vectored-write syscall.
// buftype is a buffer_alloc_arr_ptr-like type; op(handle, first, count) performs one
// chunk transfer and returns the end of the written range (carrying offset state itself).
template <typename buftype, typename func>
inline io_scatter_status_t scatter_write_pwrite_some_bytes_common(void *__restrict handle,
																  io_scatter_t const *__restrict pscatters,
																  ::std::size_t n, func op)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (!n)
	{
		return {};
	}
	constexpr ::std::size_t buffersz{4096zu};
	constexpr ::std::size_t directsz{(buffersz >> 2) * 3};
	buftype buffer;
	::std::byte *buf_currptr{}, *buf_endptr{};
	auto psci{pscatters}, psce{psci + n};
	for (; psci != psce; ++psci)
	{
		::std::size_t len{psci->len};
		if (!len)
		{
			continue;
		}
		auto base{reinterpret_cast<::std::byte const *>(psci->base)};
		::std::size_t to_copied{len};
		::std::size_t remained_size{static_cast<::std::size_t>(buf_endptr - buf_currptr)};
		if (remained_size < to_copied)
		{
			to_copied = remained_size;
		}
		buf_currptr = ::fast_io::freestanding::nonoverlapped_bytes_copy_n(base, to_copied, buf_currptr);
		if (buf_currptr != buf_endptr)
		{
			continue;
		}
		if (buf_currptr != buffer.ptr)
		{
			auto written{op(handle, buffer.ptr, static_cast<::std::size_t>(buf_currptr - buffer.ptr))};
			if (written != buf_currptr)
			{
				return scatter_locate_backward(pscatters, static_cast<::std::size_t>(psci - pscatters + 1), to_copied,
											   static_cast<::std::size_t>(buf_currptr - written),
											   static_cast<::std::size_t>(buf_currptr - buffer.ptr));
			}
			buf_currptr = buffer.ptr;
		}
		auto newbase{base + to_copied};
		auto pied{base + len};
		if (newbase == pied)
		{
			continue;
		}
		bool islast{psci == psce - 1};
		::std::size_t after_to_copied{len - to_copied};
		if (directsz <= after_to_copied || islast)
		{
			auto written{op(handle, newbase, static_cast<::std::size_t>(pied - newbase))};
			if (written != pied)
			{
				return {static_cast<::std::size_t>(psci - pscatters), static_cast<::std::size_t>(written - base)};
			}
			if (islast)
			{
				return {n, 0zu};
			}
			continue;
		}
		if (buffer.ptr == nullptr)
		{
			buf_endptr = (buf_currptr = buffer.allocate_new(buffersz)) + buffersz;
		}
		buf_currptr = ::fast_io::freestanding::nonoverlapped_bytes_copy_n(newbase, after_to_copied, buf_currptr);
	}
	if (buffer.ptr != buf_currptr)
	{
		auto written{op(handle, buffer.ptr, static_cast<::std::size_t>(buf_currptr - buffer.ptr))};
		if (written != buf_currptr)
		{
			return scatter_locate_backward(pscatters, n, psce[-1].len,
										   static_cast<::std::size_t>(buf_currptr - written),
										   static_cast<::std::size_t>(buf_currptr - buffer.ptr));
		}
	}
	return {n, 0zu};
}

// write-all variant of scatter_write_pwrite_some_bytes_common: op(handle, first, count) must
// transfer the whole chunk or throw, so no partial-write location tracking is needed.
template <typename buftype, typename func>
inline void scatter_write_pwrite_all_bytes_common(void *__restrict handle,
												  io_scatter_t const *__restrict pscatters,
												  ::std::size_t n, func op) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!n)
	{
		return;
	}
	constexpr ::std::size_t buffersz{4096zu};
	constexpr ::std::size_t directsz{(buffersz >> 2) * 3};
	buftype buffer;
	::std::byte *buf_currptr{}, *buf_endptr{};
	auto psci{pscatters}, psce{psci + n};
	for (; psci != psce; ++psci)
	{
		::std::size_t len{psci->len};
		if (!len)
		{
			continue;
		}
		auto base{reinterpret_cast<::std::byte const *>(psci->base)};
		::std::size_t to_copied{len};
		::std::size_t remained_size{static_cast<::std::size_t>(buf_endptr - buf_currptr)};
		if (remained_size < to_copied)
		{
			to_copied = remained_size;
		}
		buf_currptr = ::fast_io::freestanding::nonoverlapped_bytes_copy_n(base, to_copied, buf_currptr);
		if (buf_currptr != buf_endptr)
		{
			continue;
		}
		if (buf_currptr != buffer.ptr)
		{
			op(handle, buffer.ptr, static_cast<::std::size_t>(buf_currptr - buffer.ptr));
			buf_currptr = buffer.ptr;
		}
		auto newbase{base + to_copied};
		auto pied{base + len};
		if (newbase == pied)
		{
			continue;
		}
		bool islast{psci == psce - 1};
		::std::size_t after_to_copied{len - to_copied};
		if (directsz <= after_to_copied || islast)
		{
			op(handle, newbase, static_cast<::std::size_t>(pied - newbase));
			if (islast)
			{
				return;
			}
			continue;
		}
		if (buffer.ptr == nullptr)
		{
			buf_endptr = (buf_currptr = buffer.allocate_new(buffersz)) + buffersz;
		}
		buf_currptr = ::fast_io::freestanding::nonoverlapped_bytes_copy_n(newbase, after_to_copied, buf_currptr);
	}
	if (buffer.ptr != buf_currptr)
	{
		op(handle, buffer.ptr, static_cast<::std::size_t>(buf_currptr - buffer.ptr));
	}
}
} // namespace details
using scatter_total_size_overflow_result = ::fast_io::details::basic_scatter_total_size_overflow_result<::std::size_t>;

namespace details
{

template <::std::unsigned_integral U, typename T>
inline constexpr ::fast_io::details::basic_scatter_total_size_overflow_result<U>
find_scatter_total_size_overflow_impl(basic_io_scatter_t<T> const *base, U len) noexcept
{
	constexpr U mx{static_cast<::std::size_t>(::std::numeric_limits<U>::max())};
	U total{};
	auto i{base}, e{base + len};
	for (; i != e; ++i)
	{
		/// @todo mx ~= max / 2, mx - i.len may underflow. I'm guessing the author meant to check for overflow.
		if (static_cast<U>(static_cast<::std::size_t>(mx) - static_cast<::std::size_t>(i->len)) < total) [[unlikely]]
		{
			break;
		}
		total += i->len;
	}
	return {total, static_cast<::std::size_t>(i - base)};
}

} // namespace details

template <typename T>
inline constexpr scatter_total_size_overflow_result find_scatter_total_size_overflow(basic_io_scatter_t<T> const *base,
																					 ::std::size_t len) noexcept
{
	if (__builtin_is_constant_evaluated())
	{
		return ::fast_io::details::find_scatter_total_size_overflow_impl<::std::size_t>(base, len);
	}
	else
	{
		using io_scatter_alias_ptr

#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= io_scatter_t const *;
		return ::fast_io::details::find_scatter_total_size_overflow_impl<::std::size_t>(
			reinterpret_cast<io_scatter_alias_ptr>(base), len);
	}
}

namespace details
{
inline constexpr ::std::size_t scatter_status_one_size_impl(::std::size_t position, ::std::size_t position_in_scatter,
															::std::size_t n) noexcept
{
	if (position)
	{
		return n;
	}
	return position_in_scatter;
}
} // namespace details

inline constexpr ::std::size_t scatter_status_one_size(io_scatter_status_t status, ::std::size_t n) noexcept
{
	return ::fast_io::details::scatter_status_one_size_impl(status.position, status.position_in_scatter, n);
}

template <::std::integral dftype>
inline constexpr ::fast_io::intfpos_t fposoffadd_nonegative(::fast_io::intfpos_t off, dftype df) noexcept
{
#if __has_cpp_attribute(assume)
	[[assume(0 <= df)]];
#endif

	constexpr ::fast_io::intfpos_t mxv{::std::numeric_limits<::fast_io::intfpos_t>::max()};
	constexpr ::fast_io::uintfpos_t umxv{static_cast<::fast_io::uintfpos_t>(mxv)};
	if constexpr (mxv < ::std::numeric_limits<dftype>::max())
	{
		if (mxv < df)
		{
			return mxv;
		}
	}
	::fast_io::intfpos_t mx{static_cast<::fast_io::intfpos_t>(umxv - static_cast<::fast_io::uintfpos_t>(df))};
	if (mx < off)
	{
		return mxv;
	}
	else
	{
		return off + static_cast<::fast_io::intfpos_t>(df);
	}
}

template <::std::integral dftype>
inline constexpr ::fast_io::intfpos_t fposoffadd(::fast_io::intfpos_t off, dftype df) noexcept
{
	constexpr ::fast_io::intfpos_t mxv{::std::numeric_limits<::fast_io::intfpos_t>::max()};
	constexpr ::fast_io::uintfpos_t umxv{static_cast<::fast_io::uintfpos_t>(mxv)};
	if constexpr (mxv < ::std::numeric_limits<dftype>::max())
	{
		if (mxv < df)
		{
			return mxv;
		}
	}
	if constexpr (::std::signed_integral<dftype>)
	{
		if (df < 0)
		{
			constexpr ::fast_io::intfpos_t mnv{::std::numeric_limits<::fast_io::intfpos_t>::min()};
			if constexpr (::std::numeric_limits<dftype>::min() < mxv)
			{
				if (df < mxv)
				{
					return mnv;
				}
			}
			auto ddf{mnv - df};
			if (off < ddf)
			{
				return mnv;
			}
			else
			{
				return off + df;
			}
		}
	}
	::fast_io::intfpos_t mx{static_cast<::fast_io::intfpos_t>(umxv - static_cast<::fast_io::uintfpos_t>(df))};
	if (mx < off)
	{
		return mxv;
	}
	else
	{
		return off + df;
	}
}

namespace details
{

template <typename T>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr ::fast_io::intfpos_t
fposoffadd_scatters_impl(::fast_io::intfpos_t off, basic_io_scatter_t<T> const *base, ::std::size_t position,
						 ::std::size_t position_in_scatter) noexcept
{
	auto res{::fast_io::details::find_scatter_total_size_overflow_impl<::fast_io::uintfpos_t>(base, position)};
	constexpr ::fast_io::intfpos_t mxv{::std::numeric_limits<::fast_io::intfpos_t>::max()};
	if (res.position != position)
	{
		return mxv;
	}
	return fposoffadd_nonegative(fposoffadd_nonegative(off, res.total_size), position_in_scatter);
}
} // namespace details

template <typename T>
inline constexpr ::fast_io::intfpos_t fposoffadd_scatters(::fast_io::intfpos_t off, basic_io_scatter_t<T> const *base,
														  io_scatter_status_t status) noexcept
{
	if (__builtin_is_constant_evaluated())
	{
		return ::fast_io::details::fposoffadd_scatters_impl(off, base, status.position, status.position_in_scatter);
	}
	else
	{
		using io_scatter_alias_ptr

#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= io_scatter_t const *;
		return ::fast_io::details::fposoffadd_scatters_impl(off, reinterpret_cast<io_scatter_alias_ptr>(base),
															status.position, status.position_in_scatter);
	}
}

namespace details
{

template <::std::integral char_type>
inline constexpr ::fast_io::intfpos_t scatter_fpos_mul(::fast_io::intfpos_t ofd) noexcept
{
	constexpr ::fast_io::intfpos_t mx{::std::numeric_limits<::fast_io::intfpos_t>::max()};
	constexpr ::fast_io::intfpos_t ofs{mx / sizeof(char_type)};
	if (ofd > ofs)
	{
		return mx;
	}
	else
	{
		return ofd * static_cast<intfpos_t>(sizeof(char_type));
	}
}

inline constexpr ::fast_io::intfpos_t adjust_instm_offset(::std::ptrdiff_t remainspace,
														  ::fast_io::intfpos_t requested) noexcept
{
#if __has_cpp_attribute(assume)
	[[assume(remainspace >= 0)]];
#endif
	constexpr auto ptrdfmn{::std::numeric_limits<::fast_io::intfpos_t>::min()};
	if (requested < ptrdfmn + remainspace)
	{
		return ptrdfmn;
	}
	return requested - remainspace;
}

// read counterpart of scatter_write_pwrite_some_bytes_common: op(handle, first, count)
// fills [first, last) and returns the end of the filled range; a short or empty return
// is EOF, not an error. [buf_currptr, buf_endptr) holds fetched-but-undistributed data.
// Buffered fills are demand-bounded: fetching past the scatters' total size would
// consume bytes from the handle and silently discard them.
template <typename buftype, typename func>
inline io_scatter_status_t scatter_read_pread_some_bytes_common(void *__restrict handle,
																io_scatter_t const *__restrict pscatters,
																::std::size_t n, func op)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (!n)
	{
		return {};
	}
	constexpr ::std::size_t buffersz{4096zu};
	constexpr ::std::size_t directsz{(buffersz >> 2) * 3};
	::std::size_t remained{::fast_io::find_scatter_total_size_overflow(pscatters, n).total_size};
	buftype buffer;
	::std::byte *buf_currptr{}, *buf_endptr{};
	auto psci{pscatters}, psce{psci + n};
	for (; psci != psce; ++psci)
	{
		::std::size_t len{psci->len};
		if (!len)
		{
			continue;
		}
		auto base{static_cast<::std::byte *>(const_cast<void *>(psci->base))};
		::std::size_t to_copied{len};
		::std::size_t available{static_cast<::std::size_t>(buf_endptr - buf_currptr)};
		if (available < to_copied)
		{
			to_copied = available;
		}
		::fast_io::freestanding::nonoverlapped_bytes_copy_n(buf_currptr, to_copied, base);
		buf_currptr += to_copied;
		if (to_copied == len)
		{
			continue;
		}
		auto newbase{base + to_copied};
		auto pied{base + len};
		::std::size_t after_to_copied{len - to_copied};
		bool islast{psci == psce - 1};
		if (directsz <= after_to_copied || islast)
		{
			auto written{op(handle, newbase, static_cast<::std::size_t>(pied - newbase))};
			remained -= static_cast<::std::size_t>(written - newbase);
			if (written != pied)
			{
				return {static_cast<::std::size_t>(psci - pscatters),
						to_copied + static_cast<::std::size_t>(written - newbase)};
			}
			if (islast)
			{
				return {n, 0zu};
			}
			continue;
		}
		if (buffer.ptr == nullptr)
		{
			static_cast<void>(buffer.allocate_new(buffersz));
		}
		::std::size_t request{remained < buffersz ? remained : buffersz};
		auto written{op(handle, buffer.ptr, request)};
		remained -= static_cast<::std::size_t>(written - buffer.ptr);
		buf_currptr = buffer.ptr;
		buf_endptr = written;
		::std::size_t copied{after_to_copied};
		available = static_cast<::std::size_t>(buf_endptr - buf_currptr);
		if (available < copied)
		{
			copied = available;
		}
		::fast_io::freestanding::nonoverlapped_bytes_copy_n(buf_currptr, copied, newbase);
		buf_currptr += copied;
		if (copied < after_to_copied)
		{
			return {static_cast<::std::size_t>(psci - pscatters), to_copied + copied};
		}
	}
	return {n, 0zu};
}

// read-all variant of scatter_read_pread_some_bytes_common: op(handle, first, count)
// fills the whole range or throws, so no partial-read location tracking is needed.
// Fills stay demand-bounded for the same reason as the some variant.
template <typename buftype, typename func>
inline void scatter_read_pread_all_bytes_common(void *__restrict handle,
												io_scatter_t const *__restrict pscatters,
												::std::size_t n, func op) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!n)
	{
		return;
	}
	constexpr ::std::size_t buffersz{4096zu};
	constexpr ::std::size_t directsz{(buffersz >> 2) * 3};
	::std::size_t remained{::fast_io::find_scatter_total_size_overflow(pscatters, n).total_size};
	buftype buffer;
	::std::byte *buf_currptr{}, *buf_endptr{};
	auto psci{pscatters}, psce{psci + n};
	for (; psci != psce; ++psci)
	{
		::std::size_t len{psci->len};
		if (!len)
		{
			continue;
		}
		auto base{static_cast<::std::byte *>(const_cast<void *>(psci->base))};
		::std::size_t to_copied{len};
		::std::size_t available{static_cast<::std::size_t>(buf_endptr - buf_currptr)};
		if (available < to_copied)
		{
			to_copied = available;
		}
		::fast_io::freestanding::nonoverlapped_bytes_copy_n(buf_currptr, to_copied, base);
		buf_currptr += to_copied;
		if (to_copied == len)
		{
			continue;
		}
		auto newbase{base + to_copied};
		auto pied{base + len};
		::std::size_t after_to_copied{len - to_copied};
		bool islast{psci == psce - 1};
		if (directsz <= after_to_copied || islast)
		{
			op(handle, newbase, static_cast<::std::size_t>(pied - newbase));
			remained -= after_to_copied;
			if (islast)
			{
				return;
			}
			continue;
		}
		if (buffer.ptr == nullptr)
		{
			static_cast<void>(buffer.allocate_new(buffersz));
		}
		::std::size_t request{remained < buffersz ? remained : buffersz};
		op(handle, buffer.ptr, request);
		remained -= request;
		buf_currptr = buffer.ptr;
		buf_endptr = buffer.ptr + request;
		::fast_io::freestanding::nonoverlapped_bytes_copy_n(buf_currptr, after_to_copied, newbase);
		buf_currptr += after_to_copied;
	}
}

} // namespace details

} // namespace fast_io
