#pragma once

namespace fast_io
{

template <::std::integral ch_type>
class basic_win32_crypt_gen_random_io_observer
{
public:
	using native_handle_type = ::std::size_t;
	using input_char_type = ch_type;
	native_handle_type hprov{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return hprov;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto tmp{hprov};
		hprov = 0;
		return tmp;
	}
};

template <::std::integral char_type>
inline constexpr basic_win32_crypt_gen_random_io_observer<char_type>
input_stream_ref_define(basic_win32_crypt_gen_random_io_observer<char_type> giob) noexcept
{
	return giob;
}

template <::std::integral char_type>
inline constexpr void
input_stream_require_secure_clear_define(basic_win32_crypt_gen_random_io_observer<char_type>) noexcept
{
}

namespace win32::details
{

inline ::std::byte *win32_crypt_gen_random_some_impl(::std::size_t hprov, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (sizeof(::std::size_t) <= sizeof(::std::uint_least32_t))
	{
		::std::size_t sz{count};
		if (!::fast_io::win32::CryptGenRandom(hprov, static_cast<::std::uint_least32_t>(sz),
											  reinterpret_cast<char unsigned *>(first)))
		{
			throw_win32_error();
		}
		return first + count;
	}
	else
	{
		constexpr ::std::size_t uintleast32mx{
			static_cast<::std::size_t>(::std::numeric_limits<::std::uint_least32_t>::max())};
		auto const e{first + count};
		while (first != e)
		{
			::std::size_t toreadthisround{static_cast<::std::size_t>(e - first)};
			if (uintleast32mx < toreadthisround)
			{
				toreadthisround = uintleast32mx;
			}
			if (!::fast_io::win32::CryptGenRandom(hprov, static_cast<::std::uint_least32_t>(toreadthisround),
												  reinterpret_cast<char unsigned *>(first)))
			{
				return first;
			}
			first += toreadthisround;
		}
		return first + count;
	}
}

inline void win32_crypt_gen_random_all_impl(::std::size_t hprov, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto ret{win32_crypt_gen_random_some_impl(hprov, first, count)};
	if constexpr (sizeof(::std::uint_least32_t) < sizeof(::std::size_t))
	{
		if (ret != first + count)
		{
			throw_win32_error();
		}
	}
}

} // namespace win32::details

template <::std::integral char_type>
	requires(sizeof(::std::uint_least32_t) < sizeof(::std::size_t))
inline ::std::byte *read_some_bytes_underflow_define(basic_win32_crypt_gen_random_io_observer<char_type> bcgiob,
													 ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::win32_crypt_gen_random_some_impl(bcgiob.hprov, first, count);
}

template <::std::integral char_type>
inline void read_all_bytes_underflow_define(basic_win32_crypt_gen_random_io_observer<char_type> bcgiob,
											::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::details::win32_crypt_gen_random_all_impl(bcgiob.hprov, first, count);
}

template <win32_family family, ::std::integral ch_type>
class basic_win32_family_crypt_gen_random_file : public basic_win32_crypt_gen_random_io_observer<ch_type>
{
public:
	using native_handle_type = ::std::size_t;
	using input_char_type = ch_type;
	inline basic_win32_family_crypt_gen_random_file()
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_win32_crypt_gen_random_io_observer<ch_type>{
			  ::fast_io::details::win32::crypt_acquire_context_fallback<family>()}
	{
	}
	inline explicit constexpr basic_win32_family_crypt_gen_random_file(::std::nullptr_t)
		: basic_win32_crypt_gen_random_io_observer<ch_type>{}
	{
	}
	inline basic_win32_family_crypt_gen_random_file(basic_win32_family_crypt_gen_random_file const &) = delete;
	inline basic_win32_family_crypt_gen_random_file &operator=(basic_win32_family_crypt_gen_random_file const &) = delete;
	inline constexpr basic_win32_family_crypt_gen_random_file(
		basic_win32_family_crypt_gen_random_file &&__restrict other) noexcept
		: basic_win32_crypt_gen_random_io_observer<ch_type>{other.hprov}
	{
		other.hprov = 0;
	}
	inline basic_win32_family_crypt_gen_random_file &
	operator=(basic_win32_family_crypt_gen_random_file &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		if (this->hprov) [[likely]]
		{
			::fast_io::win32::CryptReleaseContext(this->hprov, 0);
		}
		this->hprov = other.hprov;
		other.hprov = 0;
		return *this;
	}

	inline ~basic_win32_family_crypt_gen_random_file()
	{
		if (this->hprov) [[likely]]
		{
			::fast_io::win32::CryptReleaseContext(this->hprov, 0);
		}
	}

	inline void close()
		FAST_IO_HERBCEPTIONS_THROWS
	{
		if (this->hprov) [[likely]]
		{
			if (!::fast_io::win32::CryptReleaseContext(this->hprov, 0))
			{
				throw_win32_error();
			}
		}
	}
};

template <::std::integral char_type>
using basic_win32_crypt_gen_random_file_9xa =
	basic_win32_family_crypt_gen_random_file<win32_family::ansi_9x, char_type>;
template <::std::integral char_type>
using basic_win32_crypt_gen_random_file_ntw =
	basic_win32_family_crypt_gen_random_file<win32_family::wide_nt, char_type>;
template <::std::integral char_type>
using basic_win32_crypt_gen_random_file = basic_win32_family_crypt_gen_random_file<win32_family::native, char_type>;

} // namespace fast_io
