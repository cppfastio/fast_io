#pragma once

namespace fast_io
{

struct win32_file_factory FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
	using native_handle_type = void *;
	void *handle{};
	inline explicit constexpr win32_file_factory(void *hd) noexcept
		: handle(hd) {};
	inline win32_file_factory(win32_file_factory const &) = delete;
	inline win32_file_factory &operator=(win32_file_factory const &) = delete;
	inline ~win32_file_factory()
	{
		if (handle) [[likely]]
		{
			::fast_io::win32::CloseHandle(handle);
		}
	}
};

template <win32_family family, ::std::integral ch_type>
class basic_win32_family_file FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE : public basic_win32_family_io_observer<family, ch_type>
{
public:
	using typename basic_win32_family_io_observer<family, ch_type>::char_type;
	using typename basic_win32_family_io_observer<family, ch_type>::input_char_type;
	using typename basic_win32_family_io_observer<family, ch_type>::output_char_type;
	using typename basic_win32_family_io_observer<family, ch_type>::native_handle_type;
	using basic_win32_family_io_observer<family, ch_type>::native_handle;
	using file_factory_type = win32_file_factory;
	inline explicit constexpr basic_win32_family_file() noexcept = default;

	inline constexpr basic_win32_family_file(basic_win32_family_io_observer<family, ch_type>) noexcept = delete;
	inline constexpr basic_win32_family_file &operator=(basic_win32_family_io_observer<family, ch_type>) noexcept = delete;

	inline basic_win32_family_file(basic_win32_family_file const &other)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_win32_family_io_observer<family, ch_type>{::fast_io::win32::details::win32_dup_impl(other.handle)}
	{
	}
	inline basic_win32_family_file &operator=(basic_win32_family_file const &other)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		this->handle = ::fast_io::win32::details::win32_dup2_impl(other.handle, this->handle);
		return *this;
	}
	inline basic_win32_family_file(basic_win32_family_file &&__restrict b) noexcept
		: basic_win32_family_io_observer<family, ch_type>{b.release()}
	{
	}
	inline basic_win32_family_file &operator=(basic_win32_family_file &&__restrict b) noexcept
	{
		if (__builtin_addressof(b) == this) [[unlikely]]
		{
			return *this;
		}
		if (*this) [[likely]]
		{
			::fast_io::win32::CloseHandle(this->handle);
		}
		this->handle = b.handle;
		b.handle = nullptr;
		return *this;
	}
	inline void reset(native_handle_type newhandle = nullptr) noexcept
	{
		if (*this) [[likely]]
		{
			::fast_io::win32::CloseHandle(this->handle);
		}
		this->handle = newhandle;
	}
	inline void close()
		FAST_IO_HERBCEPTIONS_THROWS
	{
		if (*this) [[likely]]
		{
			auto error{::fast_io::win32::CloseHandle(this->handle)};
			this->handle = nullptr; // POSIX standard says we should never call close(2) again even close syscall fails
			if (!error) [[unlikely]]
			{
				throw_win32_error();
			}
		}
	}

	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr basic_win32_family_file(native_hd handle1) noexcept
		: basic_win32_family_io_observer<family, ch_type>{handle1}
	{
	}

	inline basic_win32_family_file(io_dup_t, basic_win32_family_io_observer<family, ch_type> wiob)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_win32_family_io_observer<family, ch_type>{::fast_io::win32::details::win32_dup_impl(wiob.handle)}
	{
	}

#if defined(FAST_IO_HAS_WINE_UNIX)
	template <::fast_io::wine_family fam>
	inline basic_win32_family_file(basic_wine_family_file<fam, char_type> &&wnf, open_mode) FAST_IO_HERBCEPTIONS_THROWS
		: basic_win32_family_io_observer<family, char_type>{reinterpret_cast<void *>(
			  static_cast<::std::uintptr_t>(::fast_io::wine::wine_unix_host_fd_to_nt_handle(wnf.release())))}
	{
	}
#endif

	inline explicit constexpr basic_win32_family_file(win32_file_factory &&fact) noexcept
		: basic_win32_family_io_observer<family, ch_type>{fact.handle}
	{
		fact.handle = nullptr;
	}

	inline explicit constexpr basic_win32_family_file(decltype(nullptr)) noexcept = delete;

	inline explicit basic_win32_family_file(io_temp_t)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_win32_family_io_observer<family, char_type>{::fast_io::details::create_win32_temp_file_impl<family>()}
	{
	}

	inline explicit basic_win32_family_file(nt_fs_dirent fsdirent, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_win32_family_io_observer<family, char_type>{
			  ::fast_io::details::win32_create_file_at_fs_dirent_impl<family>(
				  fsdirent.handle, fsdirent.filename.c_str(), fsdirent.filename.size(), {om, pm})}
	{
	}

	inline explicit basic_win32_family_file(win32_9xa_fs_dirent fsdirent, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_win32_family_io_observer<family, char_type>{
			  ::fast_io::win32::details::win32_9xa_create_file_at_fs_dirent_impl(
				  fsdirent.handle, fsdirent.filename.c_str(), fsdirent.filename.size(), {om, pm})}
	{
	}

	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_win32_family_file(T const &filename, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_win32_family_io_observer<family, char_type>{
			  ::fast_io::details::win32_create_file_impl<family>(filename, {om, pm})}
	{
	}

	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_win32_family_file(nt_at_entry nate, T const &filename, open_mode om,
											perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_win32_family_io_observer<family, char_type>{
			  ::fast_io::details::win32_create_file_at_impl<family>(nate.handle, filename, {om, pm})}
	{
	}

	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_win32_family_file(win32_9xa_at_entry nate, T const &filename, open_mode om,
											perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_win32_family_io_observer<family, char_type>{
			  ::fast_io::win32::details::win32_9xa_create_file_at_impl(nate.handle, filename, {om, pm})}
	{
	}

	inline explicit basic_win32_family_file(io_async_t) FAST_IO_HERBCEPTIONS_THROWS
		requires(::std::same_as<char_type, char>)
		: basic_win32_family_io_observer<family, char_type>{details::create_io_completion_port_impl()}
	{
	}

	inline ~basic_win32_family_file()
	{
		if (*this) [[likely]]
		{
			::fast_io::win32::CloseHandle(this->handle);
		}
	}
};

template <win32_family family, ::std::integral ch_type>
inline void truncate(basic_win32_family_io_observer<family, ch_type> handle, ::fast_io::uintfpos_t size)
	FAST_IO_HERBCEPTIONS_THROWS
{
	win32::details::seek_impl(handle.handle, size, seekdir::beg);
	if (!::fast_io::win32::SetEndOfFile(handle.handle))
	{
		throw_win32_error();
	}
}

} // namespace fast_io
