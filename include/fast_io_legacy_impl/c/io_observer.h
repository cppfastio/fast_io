#pragma once

namespace fast_io
{

template <c_family family, ::std::integral ch_type>
	requires(family == c_family::native || family == c_family::native_unlocked)
class basic_c_family_io_observer
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = FILE *;
	native_handle_type fp{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return fp;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return fp;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{fp};
		fp = nullptr;
		return temp;
	}
#if !defined(__AVR__)
	template <posix_family fam>
	inline explicit operator basic_posix_family_io_observer<fam, char_type>() const noexcept
	{
		return basic_posix_family_io_observer<fam, char_type>{details::my_fileno_impl<family>(fp)};
	}
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) || defined(__CYGWIN__)
	template <win32_family fam>
	inline explicit operator basic_win32_family_io_observer<fam, char_type>() const noexcept
	{
		return {::fast_io::details::my_fp_to_win32_handle_impl<family>(fp)};
	}
	template <nt_family fam>
	inline explicit operator basic_nt_family_io_observer<fam, char_type>() const noexcept
	{
		return {::fast_io::details::my_fp_to_win32_handle_impl<family>(fp)};
	}
#if defined(FAST_IO_HAS_WINE_UNIX)
	template <wine_family fam>
	inline explicit operator basic_wine_family_io_observer<fam, char_type>() const
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return {::fast_io::details::my_fp_to_wine_host_fd_impl<family>(fp)};
	}
#endif
#endif
#endif
	inline void lock() const noexcept
		requires(family == c_family::standard || family == c_family::emulated)
	{
#if (defined(_MSC_VER) || defined(_UCRT)) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
		if constexpr (false)
		{
			::fast_io::details::ucrt_lock_file(fp);
		}
		else
		{
			noexcept_call(_lock_file, fp);
		}
#elif (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__) && !defined(__WINE__)
		win32::my_msvcrt_lock_file(fp);
#elif !defined(__SINGLE_THREAD__)
#if defined(__NEWLIB__)
#if defined(__CYGWIN__)
		details::my_cygwin_flockfile(fp);
#elif !defined(__SINGLE_THREAD__)
//	_flockfile(fp);	//TO FIX undefined reference to `__cygwin_lock_lock' why?
#endif
#elif defined(__MSDOS__) || (defined(__wasi__) && !defined(__wasilibc_unmodified_upstream) && !defined(_REENTRANT)) || \
	defined(__AVR__) || defined(_PICOLIBC__)
#else
		noexcept_call(flockfile, fp);
#endif
#endif
	}
	inline void unlock() const noexcept
		requires(family == c_family::standard || family == c_family::emulated)
	{
#if (defined(_MSC_VER) || defined(_UCRT)) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
		if constexpr (false)
		{
			::fast_io::details::ucrt_unlock_file(fp);
		}
		else
		{
			noexcept_call(_unlock_file, fp);
		}
#elif (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
		win32::my_msvcrt_unlock_file(fp);
#elif !defined(__SINGLE_THREAD__)
#if defined(__NEWLIB__)
#if defined(__CYGWIN__)
		details::my_cygwin_funlockfile(fp);
#elif !defined(__SINGLE_THREAD__)
//	_funlockfile(fp); //TO FIX
#endif
#elif defined(__MSDOS__) || (defined(__wasi__) && !defined(__wasilibc_unmodified_upstream) && !defined(_REENTRANT)) || \
	defined(__AVR__) || defined(_PICOLIBC__)
#else
		noexcept_call(funlockfile, fp);
#endif
#endif
	}
	inline constexpr basic_c_family_io_observer<c_family::native_unlocked, ch_type> unlocked_handle() const noexcept
		requires(family == c_family::standard)
	{
		return {fp};
	}
	inline constexpr basic_c_family_io_observer<c_family::native_unlocked, ch_type> unlocked_handle() const noexcept
		requires(family == c_family::emulated)
	{
		return {fp};
	}
};

template <c_family family, ::std::integral ch_type>
inline constexpr basic_c_family_io_observer<family, ch_type>
io_stream_ref_define(basic_c_family_io_observer<family, ch_type> other) noexcept
{
	return other;
}

template <c_family family, ::std::integral ch_type>
inline constexpr basic_c_family_io_observer<family, char>
io_bytes_stream_ref_define(basic_c_family_io_observer<family, ch_type> other) noexcept
{
	return {other.fp};
}

template <c_family family, ::std::integral ch_type>
	requires(family == c_family::standard || family == c_family::emulated)
inline constexpr basic_c_family_io_observer<family, ch_type>
io_stream_mutex_ref_define(basic_c_family_io_observer<family, ch_type> other) noexcept
{
	return other;
}

template <c_family family, ::std::integral ch_type>
	requires(family == c_family::standard || family == c_family::emulated)
inline constexpr basic_c_family_io_observer<c_family::native_unlocked, ch_type>
io_stream_unlocked_ref_define(basic_c_family_io_observer<family, ch_type> other) noexcept
{
	return {other.fp};
}

#if defined(__AVR__)
template <c_family family, ::std::integral ch_type>
inline constexpr posix_file_status status(basic_c_family_io_observer<family, ch_type> ciob)
	FAST_IO_HERBCEPTIONS_THROWS
{
	details::avr_libc_nosup_impl();
}

#else

template <c_family family, ::std::integral ch_type>
inline constexpr posix_at_entry at(basic_c_family_io_observer<family, ch_type> other) noexcept
{
	return posix_at_entry{details::my_fileno_impl<family>(other.fp)};
}

template <c_family family, ::std::integral ch_type>
	requires requires(basic_posix_io_observer<ch_type> piob) { status(piob); }
inline constexpr posix_file_status status(basic_c_family_io_observer<family, ch_type> ciob)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return status(static_cast<basic_posix_io_observer<ch_type>>(ciob));
}

template <c_family family, ::std::integral ch_type>
inline ::std::size_t file_size(::fast_io::basic_c_family_io_observer<family, ch_type> ciob)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::posix_loader_get_file_size(details::my_fileno_impl<family>(ciob.fp));
}
#endif

template <c_family family, ::std::integral ch_type>
inline void io_stream_buffer_flush_define(basic_c_family_io_observer<family, ch_type> cfhd)
	FAST_IO_HERBCEPTIONS_THROWS
{
	details::my_c_io_flush_impl<family>(cfhd.fp);
}

template <c_family family, ::std::integral ch_type>
inline ::fast_io::intfpos_t io_stream_seek_bytes_define(basic_c_family_io_observer<family, ch_type> cfhd,
														::fast_io::intfpos_t offset, seekdir s)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return details::my_c_io_seek_impl<family>(cfhd.fp, offset, s);
}

template <c_family family, ::std::integral ch_type>
inline constexpr bool operator==(basic_c_family_io_observer<family, ch_type> a,
								 basic_c_family_io_observer<family, ch_type> b) noexcept
{
	return a.fp == b.fp;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <c_family family, ::std::integral ch_type>
inline constexpr auto operator<=>(basic_c_family_io_observer<family, ch_type> a,
								  basic_c_family_io_observer<family, ch_type> b) noexcept
{
	return a.fp <=> b.fp;
}
#endif

#if !defined(__AVR__)
template <c_family family, ::std::integral ch_type>
	requires requires(basic_c_family_io_observer<family, ch_type> h) {
		redirect_handle(static_cast<basic_posix_io_observer<ch_type>>(h));
	}
inline decltype(auto) redirect_handle(basic_c_family_io_observer<family, ch_type> h)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(
		redirect_handle(static_cast<basic_posix_io_observer<ch_type>>(h))))
{
	return redirect_handle(static_cast<basic_posix_io_observer<ch_type>>(h));
}

/*
A FILE*-based stream can only expose its fd for zero-copy transmit when its
buffered read-ahead is inspectable (input) or flushable (output) — otherwise
fd-level transfers would skip data still sitting in the libc buffer.
*/
template <c_family family, ::std::integral ch_type>
	requires(::fast_io::operations::decay::defines::has_ibuffer_basic_operations<
				 basic_c_family_io_observer<family, ch_type>> &&
			 ::fast_io::operations::decay::defines::has_input_transmit_handle_define<
				 basic_posix_io_observer<ch_type>>)
inline constexpr decltype(auto) input_transmit_handle_define(basic_c_family_io_observer<family, ch_type> h) noexcept
{
	return input_transmit_handle_define(static_cast<basic_posix_io_observer<ch_type>>(h));
}

template <c_family family, ::std::integral ch_type>
	requires(::fast_io::operations::decay::defines::has_output_or_io_stream_buffer_flush_define<
				 basic_c_family_io_observer<family, ch_type>> &&
			 ::fast_io::operations::decay::defines::has_output_transmit_handle_define<
				 basic_posix_io_observer<ch_type>>)
inline constexpr decltype(auto) output_transmit_handle_define(basic_c_family_io_observer<family, ch_type> h) noexcept
{
	return output_transmit_handle_define(static_cast<basic_posix_io_observer<ch_type>>(h));
}
#endif
} // namespace fast_io
