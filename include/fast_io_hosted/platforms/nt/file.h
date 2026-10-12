#pragma once

namespace fast_io
{

template <nt_family family>
struct nt_family_file_factory FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
	using native_handle_type = void *;
	void *handle{};
	inline explicit constexpr nt_family_file_factory(void *hd) noexcept
		: handle(hd) {};
	inline nt_family_file_factory(nt_family_file_factory const &) = delete;
	inline nt_family_file_factory &operator=(nt_family_file_factory const &) = delete;
	inline ~nt_family_file_factory()
	{
		if (handle) [[likely]]
		{
			::fast_io::win32::nt::nt_close<family == nt_family::zw>(this->handle);
		}
	}
};

using nt_file_factory = nt_family_file_factory<nt_family::nt>;
using zw_file_factory = nt_family_file_factory<nt_family::zw>;

template <nt_family family, ::std::integral ch_type>
class basic_nt_family_file FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE : public basic_nt_family_io_observer<family, ch_type>
{
public:
	using typename basic_nt_family_io_observer<family, ch_type>::char_type;
	using typename basic_nt_family_io_observer<family, ch_type>::input_char_type;
	using typename basic_nt_family_io_observer<family, ch_type>::output_char_type;
	using typename basic_nt_family_io_observer<family, ch_type>::native_handle_type;
	using file_factory_type = nt_family_file_factory<family>;
	inline constexpr basic_nt_family_file() noexcept = default;
	inline constexpr basic_nt_family_file(basic_nt_family_io_observer<family, ch_type>) noexcept = delete;
	inline constexpr basic_nt_family_file &operator=(basic_nt_family_io_observer<family, ch_type>) noexcept = delete;

	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr basic_nt_family_file(native_hd hd) noexcept
		: basic_nt_family_io_observer<family, ch_type>{hd}
	{
	}
	inline constexpr basic_nt_family_file(decltype(nullptr)) noexcept = delete;
	inline explicit constexpr basic_nt_family_file(nt_family_file_factory<family> &&hd) noexcept
		: basic_nt_family_io_observer<family, ch_type>{hd}
	{
		hd.handle = nullptr;
	}
	inline explicit basic_nt_family_file(io_dup_t, basic_nt_family_io_observer<family, ch_type> wiob)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_nt_family_io_observer<family, ch_type>{
			  ::fast_io::win32::nt::details::nt_dup_impl<family == nt_family::zw>(wiob.handle)}
	{
	}
#if defined(FAST_IO_HAS_WINE_UNIX)
	template <::fast_io::wine_family fam>
	inline basic_nt_family_file(basic_wine_family_file<fam, char_type> &&wnf, open_mode) FAST_IO_HERBCEPTIONS_THROWS
		: basic_nt_family_io_observer<family, char_type>{reinterpret_cast<void *>(
			  static_cast<::std::uintptr_t>(::fast_io::wine::wine_unix_host_fd_to_nt_handle(wnf.release())))}
	{
	}
#endif
	inline explicit basic_nt_family_file(nt_fs_dirent fsdirent, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_nt_family_io_observer<family, char_type>{
			  ::fast_io::win32::nt::details::nt_family_create_file_fs_dirent_impl<family == nt_family::zw>(fsdirent.handle, fsdirent.filename.c_str(), fsdirent.filename.size(), {om, pm})}
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_nt_family_file(T const &t, open_mode om, perms pm = static_cast<perms>(436)) FAST_IO_HERBCEPTIONS_THROWS
		: basic_nt_family_io_observer<family, ch_type>{
			  ::fast_io::win32::nt::details::nt_create_file_impl<family == nt_family::zw>(t, {om, pm})}
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_nt_family_file(nt_at_entry ent, T const &t, open_mode om, perms pm = static_cast<perms>(436)) FAST_IO_HERBCEPTIONS_THROWS
		: basic_nt_family_io_observer<family, ch_type>{
			  ::fast_io::win32::nt::details::nt_create_file_at_impl<family == nt_family::zw>(ent.handle, t, {om, pm})}
	{
	}

	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_nt_family_file(io_kernel_t, T const &t, open_mode om, perms pm = static_cast<perms>(436)) FAST_IO_HERBCEPTIONS_THROWS
		: basic_nt_family_io_observer<family, ch_type>{
			  ::fast_io::win32::nt::details::nt_create_file_kernel_impl<family == nt_family::zw>(t, {om, pm})}
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_nt_family_file(io_kernel_t, nt_at_entry ent, T const &t, open_mode om,
										 perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_nt_family_io_observer<family, ch_type>{
			  ::fast_io::win32::nt::details::nt_create_file_at_impl<family == nt_family::zw, true>(ent.handle, t,
																								   {om, pm})}
	{
	}

	inline void close()
		FAST_IO_HERBCEPTIONS_THROWS
	{
		if (this->handle) [[likely]]
		{
			auto status{::fast_io::win32::nt::nt_close<family == nt_family::zw>(this->handle)};
			this->handle = nullptr; // POSIX standard says we should never call close(2) again even close syscall fails
			if (status) [[unlikely]]
			{
				::fast_io::herbceptions::throws_nt_errc_with_value(status);
			}
		}
	}
	inline void reset(native_handle_type newhandle = nullptr) noexcept
	{
		if (this->handle) [[likely]]
		{
			::fast_io::win32::nt::nt_close<family == nt_family::zw>(this->handle);
		}
		this->handle = newhandle;
	}
	inline basic_nt_family_file(basic_nt_family_file const &other)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_nt_family_io_observer<family, ch_type>(
			  ::fast_io::win32::nt::details::nt_dup_impl<family == nt_family::zw>(other.handle))
	{
	}
	inline basic_nt_family_file &operator=(basic_nt_family_file const &other)
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		this->handle = ::fast_io::win32::nt::details::nt_dup2_impl<family == nt_family::zw>(other.handle, this->handle);
		return *this;
	}
	inline constexpr basic_nt_family_file(basic_nt_family_file &&__restrict other) noexcept
		: basic_nt_family_io_observer<family, ch_type>{other.handle}
	{
		other.handle = nullptr;
	}
	inline basic_nt_family_file &operator=(basic_nt_family_file &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		if (this->handle) [[likely]]
		{
			::fast_io::win32::nt::nt_close<family == nt_family::zw>(this->handle);
		}
		this->handle = other.handle;
		other.handle = nullptr;
		return *this;
	}
	inline ~basic_nt_family_file()
	{
		if (this->handle) [[likely]]
		{
			::fast_io::win32::nt::nt_close<family == nt_family::zw>(this->handle);
		}
	}
};

} // namespace fast_io
