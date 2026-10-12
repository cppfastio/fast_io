#pragma once

namespace fast_io
{

template <::std::integral T>
class basic_mfc_io_observer
{
public:
	using char_type = T;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = CFile *;
	native_handle_type phandle{};
	explicit constexpr operator bool() const noexcept
	{
		return phandle;
	}
	template <win32_family family>
	explicit operator basic_win32_family_io_observer<family, char_type>() const noexcept
	{
		return {phandle->m_hFile};
	}

	template <nt_family family>
	explicit operator basic_nt_family_io_observer<family, char_type>() const noexcept
	{
		return {phandle->m_hFile};
	}
	constexpr native_handle_type native_handle() const noexcept
	{
		return phandle;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{phandle};
		phandle = nullptr;
		return temp;
	}
};

template <::std::integral T>
inline constexpr basic_mfc_io_observer<T> io_value_handle(basic_mfc_io_observer<T> t) noexcept
{
	return t;
}

namespace details
{
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline void mfc_write_n_impl(CFile *cfp, ::std::byte const *first_ptr, ::std::size_t n)
{
	if constexpr (sizeof(::std::size_t) > sizeof(::std::uint_least32_t))
	{
		while (n)
		{
			constexpr ::std::size_t sz_max{static_cast<::std::size_t>(UINT_LEAST32_MAX)};
			::std::size_t write_this_round{n};
			if (sz_max < write_this_round)
			{
				write_this_round = sz_max;
			}
			cfp->Write(first_ptr, static_cast<::std::uint_least32_t>(write_this_round));
			n -= write_this_round;
		}
	}
	else
	{
		cfp->Write(first_ptr, static_cast<::std::uint_least32_t>(n));
	}
}

inline ::std::size_t mfc_read_impl(CFile *cfp, void *first, ::std::size_t to_read)
{
	if constexpr (sizeof(::std::size_t) > 4)
	{
		if (static_cast<::std::size_t>(UINT_LEAST32_MAX) < to_read)
		{
			to_read = static_cast<::std::size_t>(UINT_LEAST32_MAX);
		}
	}
	return cfp->Read(first, static_cast<::std::uint_least32_t>(to_read));
}

#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
struct mfc_scatter_write_chunk_impl
{
	CFile *cfp;
	inline ::std::byte const *operator()(void *, ::std::byte const *first, ::std::size_t count) const
		FAST_IO_HERBCEPTIONS_THROWS
	{
		mfc_write_n_impl(cfp, first, count);
		return first + count;
	}
};

struct mfc_scatter_read_chunk_impl
{
	CFile *cfp;
	inline ::std::byte *operator()(void *, ::std::byte *first, ::std::size_t count) const FAST_IO_HERBCEPTIONS_THROWS
	{
		return first + mfc_read_impl(cfp, first, count);
	}
};

template <typename T>
using mfc_scatter_buffer_alloc_ptr = ::fast_io::details::buffer_alloc_arr_ptr<
	T,
	false,
	::fast_io::generic_allocator_adapter<::fast_io::native_thread_local_allocator,
										 ::fast_io::allocator_adapter_flags::throws_on_allocation_failure>>;

} // namespace details

template <::std::integral T>
inline ::std::byte *read_some_bytes_underflow_define(basic_mfc_io_observer<T> hd, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return first + ::fast_io::details::mfc_read_impl(hd.phandle, first, count);
}

template <::std::integral T>
inline ::std::byte const *write_some_bytes_overflow_define(basic_mfc_io_observer<T> hd, ::std::byte const *first,
														   ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::mfc_write_n_impl(hd.phandle, first, count);
	return first + count;
}

template <::std::integral T>
inline ::fast_io::io_scatter_status_t
scatter_read_some_bytes_underflow_define(basic_mfc_io_observer<T> hd, ::fast_io::io_scatter_t const *pscatters,
										 ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::scatter_read_pread_some_bytes_common<
		::fast_io::details::mfc_scatter_buffer_alloc_ptr<::std::byte>>(
		nullptr, pscatters, n, ::fast_io::details::mfc_scatter_read_chunk_impl{hd.phandle});
}

template <::std::integral T>
inline ::fast_io::io_scatter_status_t
scatter_write_some_bytes_overflow_define(basic_mfc_io_observer<T> hd, ::fast_io::io_scatter_t const *pscatters,
										 ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::scatter_write_pwrite_some_bytes_common<
		::fast_io::details::mfc_scatter_buffer_alloc_ptr<::std::byte>>(
		nullptr, pscatters, n, ::fast_io::details::mfc_scatter_write_chunk_impl{hd.phandle});
}

template <::std::integral T>
inline void output_stream_buffer_flush_define(basic_mfc_io_observer<T> hd) FAST_IO_HERBCEPTIONS_THROWS
{
	hd.phandle->Flush();
}

template <::std::integral ch_type>
class basic_mfc_file : public basic_mfc_io_observer<ch_type>
{
public:
	using char_type = ch_type;
	using native_handle_type = CFile *;
	constexpr basic_mfc_file() noexcept = default;
	template <typename T>
		requires ::std::same_as<T, native_handle_type>
	explicit constexpr basic_mfc_file(T hd) noexcept
		: basic_mfc_io_observer<ch_type>{hd}
	{
	}
	explicit constexpr basic_mfc_file(decltype(nullptr)) noexcept = delete;

	constexpr basic_mfc_file(basic_mfc_io_observer<char_type>) noexcept = delete;
	constexpr basic_mfc_file &operator=(basic_mfc_io_observer<char_type>) noexcept = delete;

	basic_mfc_file(basic_mfc_file const &mcf)
		: basic_mfc_io_observer<char_type>{mcf.phandle->Duplicate()}
	{}
	basic_mfc_file &operator=(basic_mfc_file const &mcf)
	{
		auto temp{mcf.phandle->Duplicate()};
		delete this->phandle;
		this->phandle = temp;
		return *this;
	}
	basic_mfc_file(basic_mfc_file &&__restrict mcf) noexcept
		: basic_mfc_io_observer<char_type>{mcf.phandle}
	{
		mcf.phandle = nullptr;
	}
	basic_mfc_file &operator=(basic_mfc_file &&__restrict mcf) noexcept
	{
		delete this->phandle;
		this->phandle = mcf.phandle;
		mcf.phandle = nullptr;
		return *this;
	}
	void close() noexcept
	{
		delete this->phandle;
		this->phandle = nullptr;
	}
	inline constexpr void reset(native_handle_type newhandle = nullptr) noexcept
	{
		delete this->phandle;
		this->phandle = newhandle;
	}
	template <win32_family family>
	explicit basic_mfc_file(basic_win32_family_file<family, char_type> &&hd, open_mode)
		: basic_mfc_io_observer<char_type>{new CFile(hd.handle)}
	{
		hd.release();
	}
	template <nt_family family>
	explicit basic_mfc_file(basic_nt_family_file<family, char_type> &&hd, open_mode)
		: basic_mfc_io_observer<char_type>{new CFile(hd.handle)}
	{
		hd.release();
	}
	basic_mfc_file(nt_fs_dirent fsdirent, open_mode om, perms pm = static_cast<perms>(436))
		: basic_mfc_file(basic_win32_file<char_type>(fsdirent, om, pm), om)
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	basic_mfc_file(T const &t, open_mode om, perms pm = static_cast<perms>(436))
		: basic_mfc_file(basic_win32_file<char_type>(t, om, pm), om)
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	basic_mfc_file(nt_at_entry nate, T const &t, open_mode om, perms pm = static_cast<perms>(436))
		: basic_mfc_file(basic_win32_file<char_type>(nate, t, om, pm), om)
	{
	}
	~basic_mfc_file()
	{
		delete this->phandle;
	}
};

using mfc_io_observer = basic_mfc_io_observer<char>;
using mfc_file = basic_mfc_file<char>;
using u8mfc_io_observer = basic_mfc_io_observer<char8_t>;
using u8mfc_file = basic_mfc_file<char8_t>;
using wmfc_io_observer = basic_mfc_io_observer<wchar_t>;
using wmfc_file = basic_mfc_file<wchar_t>;
using u16mfc_io_observer = basic_mfc_io_observer<char16_t>;
using u16mfc_file = basic_mfc_file<char16_t>;
using u32mfc_io_observer = basic_mfc_io_observer<char32_t>;
using u32mfc_file = basic_mfc_file<char32_t>;

} // namespace fast_io
