#pragma once

namespace fast_io
{

template <c_family family>
struct c_family_file_factory FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
	using native_handle_type = FILE *;
	FILE *fp{};
	inline explicit constexpr c_family_file_factory(FILE *fpp) noexcept
		: fp(fpp) {};
	inline c_family_file_factory(c_family_file_factory const &) = delete;
	inline c_family_file_factory &operator=(c_family_file_factory const &) = delete;
	inline ~c_family_file_factory() noexcept
	{
		if (fp) [[likely]]
		{
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
			struct _reent ent{};
			noexcept_call(_fclose_r, __builtin_addressof(ent), this->fp);
#else
			details::my_fclose_impl<family>(this->fp);
#endif
		}
	}
};

template <c_family family, ::std::integral ch_type>
class basic_c_family_file FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE : public basic_c_family_io_observer<family, ch_type>
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = FILE *;
	inline constexpr basic_c_family_file() noexcept = default;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr basic_c_family_file(native_hd ffp) noexcept
		: basic_c_family_io_observer<family, ch_type>{ffp}
	{
	}
	template <c_family family2>
	inline explicit constexpr basic_c_family_file(c_family_file_factory<family2> &&other) noexcept
		: basic_c_family_io_observer<family, ch_type>{other.fp}
	{
		other.fp = nullptr;
	}
	inline constexpr basic_c_family_file(basic_c_family_io_observer<family, ch_type>) noexcept = delete;
	inline constexpr basic_c_family_file &operator=(basic_c_family_io_observer<family, ch_type>) noexcept = delete;

	inline basic_c_family_file(basic_c_family_file const &) = delete;
	inline basic_c_family_file &operator=(basic_c_family_file const &) = delete;
	inline constexpr basic_c_family_file(decltype(nullptr)) noexcept = delete;
	inline constexpr basic_c_family_file(basic_c_family_file &&other) noexcept
		: basic_c_family_io_observer<family, ch_type>{other.fp}
	{
		other.fp = nullptr;
	}
	inline basic_c_family_file &operator=(basic_c_family_file &&other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		if (this->fp) [[likely]]
		{
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
			struct _reent ent{};
			noexcept_call(_fclose_r, __builtin_addressof(ent), this->fp);
#else
			details::my_fclose_impl<family>(this->fp);
#endif
		}
		this->fp = other.fp;
		other.fp = nullptr;
		return *this;
	}
	inline void close()
		FAST_IO_HERBCEPTIONS_THROWS
	{
		if (this->fp == nullptr) [[unlikely]]
		{
			return;
		}
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
		struct _reent ent{};
		int ret{noexcept_call(_fclose_r, __builtin_addressof(ent), this->fp)};
		this->fp = nullptr;
		if (ret == EOF)
		{
			throw_posix_error(ent._errno);
		}
#else
		int ret{details::my_fclose_impl<family>(this->fp)};
		this->fp = nullptr;
		if (ret == EOF)
		{
			throw_posix_error(errno);
		}
#endif
	}
	inline ~basic_c_family_file() noexcept
	{
		if (this->fp) [[likely]]
		{
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
			struct _reent ent{};
			noexcept_call(_fclose_r, __builtin_addressof(ent), this->fp);
#else
			details::my_fclose_impl<family>(this->fp);
#endif
		}
	}
#if !defined(__AVR__)
	template <::fast_io::constructible_to_os_c_str T>
	inline basic_c_family_file(T const &file, open_mode om, perms pm = static_cast<perms>(436)) FAST_IO_HERBCEPTIONS_THROWS
		: basic_c_family_file(basic_posix_file<char_type>(file, om, pm), om)
	{
	}
	template <posix_family pfamily>
	inline basic_c_family_file(basic_posix_family_file<pfamily, char_type> &&phd, open_mode om) FAST_IO_HERBCEPTIONS_THROWS
		: basic_c_family_io_observer<family, ch_type>{::fast_io::details::my_c_file_open_impl(phd.fd, om)}
	{
		phd.fd = -1;
	}
	template <posix_family pfamily>
	inline explicit constexpr basic_c_family_file(io_construct_t, basic_posix_family_io_observer<pfamily, ch_type> piob, open_mode om) FAST_IO_HERBCEPTIONS_THROWS
		: basic_c_family_io_observer<family, char_type>{::fast_io::details::my_c_file_open_impl(piob.fd, om)}
	{
	}
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) || defined(__CYGWIN__)
	// windows specific. open posix file from win32 io handle
	template <win32_family wfamily>
	inline basic_c_family_file(basic_win32_family_file<wfamily, char_type> &&win32_handle, open_mode om)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_c_family_file(basic_posix_file<char_type>(::std::move(win32_handle), om), om)
	{
	}
	template <nt_family nfamily>
	inline basic_c_family_file(basic_nt_family_file<nfamily, char_type> &&nt_handle, open_mode om)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_c_family_file(basic_posix_file<char_type>(::std::move(nt_handle), om), om)
	{
	}
#if defined(FAST_IO_HAS_WINE_UNIX)
	template <::fast_io::wine_family wfamily>
	inline basic_c_family_file(basic_wine_family_file<wfamily, char_type> &&wnf, open_mode om)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_c_family_file(basic_posix_file<char_type>(::std::move(wnf), om), om)
	{
	}
#endif
	inline basic_c_family_file(nt_fs_dirent ent, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_c_family_file(basic_posix_file<char_type>(ent, om, pm), om)
	{
	}
	inline basic_c_family_file(win32_9xa_fs_dirent ent, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_c_family_file(basic_posix_file<char_type>(ent, om, pm), om)
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline basic_c_family_file(nt_at_entry nate, T const &file, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_c_family_file(basic_posix_file<char_type>(nate, file, om, pm), om)
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline basic_c_family_file(win32_9xa_at_entry nate, T const &file, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_c_family_file(basic_posix_file<char_type>(nate, file, om, pm), om)
	{
	}
#else
	inline basic_c_family_file(posix_fs_dirent ent, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_c_family_file(basic_posix_file<char_type>(ent, om, pm), om)
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline basic_c_family_file(posix_at_entry nate, T const &file, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_c_family_file(basic_posix_file<char_type>(nate, file, om, pm), om)
	{
	}
#endif
#endif
	inline basic_c_family_file(io_temp_t)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_c_family_io_observer<family, ch_type>{::fast_io::details::my_c_open_tmp_file()}
	{
	}
};

namespace freestanding
{
template <c_family fm>
struct is_trivially_copyable_or_relocatable<c_family_file_factory<fm>>
{
	inline static constexpr bool value = true;
};

template <c_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_c_family_io_observer<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <c_family fm, ::std::integral char_type>
struct is_trivially_copyable_or_relocatable<basic_c_family_file<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <c_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_c_family_file<fm, char_type>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding
} // namespace fast_io
