#pragma once

#include "../../../fast_io_dsal/impl/misc/push_macros.h"

namespace fast_io
{

template <::fast_io::wine_family family>
struct wine_family_file_factory FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
	using native_handle_type = ::fast_io::wine_host_fd_t;
	native_handle_type host_fd{};

	inline explicit constexpr wine_family_file_factory(native_handle_type v) noexcept
		: host_fd(v) {};
	inline wine_family_file_factory(wine_family_file_factory const &) = delete;
	inline wine_family_file_factory &operator=(wine_family_file_factory const &) = delete;
	inline ~wine_family_file_factory()
	{
		if (host_fd) [[likely]]
		{
			::fast_io::wine::wine_unix_close_returns_status(host_fd);
		}
	}
};

using wine_file_factory = wine_family_file_factory<::fast_io::wine_family::native>;

template <::fast_io::wine_family family, ::std::integral ch_type>
class basic_wine_family_file : public basic_wine_family_io_observer<family, ch_type>
{
public:
	using typename basic_wine_family_io_observer<family, ch_type>::char_type;
	using typename basic_wine_family_io_observer<family, ch_type>::input_char_type;
	using typename basic_wine_family_io_observer<family, ch_type>::output_char_type;
	using typename basic_wine_family_io_observer<family, ch_type>::native_handle_type;
	using file_factory_type = wine_family_file_factory<family>;
	using basic_wine_family_io_observer<family, ch_type>::native_handle;
	inline constexpr basic_wine_family_file() noexcept = default;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr basic_wine_family_file(native_hd host_fd1) noexcept
		: basic_wine_family_io_observer<family, ch_type>{host_fd1}
	{
	}
	inline explicit constexpr basic_wine_family_file(wine_family_file_factory<family> &&factory) noexcept
		: basic_wine_family_io_observer<family, ch_type>{factory.host_fd}
	{
		factory.host_fd = 0;
	}

	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_wine_family_file(T const &filename, open_mode om,
										   perms pm = static_cast<perms>(436)) FAST_IO_HERBCEPTIONS_THROWS
		: basic_wine_family_io_observer<family, ch_type>{::fast_io::details::wine_open_file_impl(filename, om, pm)}
	{
	}

	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_wine_family_file(wine_at_entry wate, T const &filename, open_mode om,
										   perms pm = static_cast<perms>(436)) FAST_IO_HERBCEPTIONS_THROWS
		: basic_wine_family_io_observer<family, ch_type>{::fast_io::details::wine_openat_file_impl(
			  wate.host_fd, filename, om, pm)}
	{
	}

	inline constexpr basic_wine_family_file(basic_wine_family_file const &) = delete;
	inline constexpr basic_wine_family_file &operator=(basic_wine_family_file const &) = delete;

	inline constexpr basic_wine_family_file(basic_wine_family_file &&__restrict other) noexcept
		: basic_wine_family_io_observer<family, ch_type>{other.release()}
	{
	}
	inline basic_wine_family_file &operator=(basic_wine_family_file &&__restrict other) noexcept
	{
		if (this->host_fd) [[likely]]
		{
			::fast_io::wine::wine_unix_close_returns_status(this->host_fd);
		}
		this->host_fd = other.release();
		return *this;
	}
	inline constexpr basic_wine_family_file(decltype(nullptr)) noexcept = delete;
	inline constexpr basic_wine_family_file(basic_wine_family_io_observer<family, ch_type>) noexcept = delete;
	inline constexpr basic_wine_family_file &operator=(basic_wine_family_io_observer<family, ch_type>) noexcept = delete;
	inline ~basic_wine_family_file()
	{
		if (this->host_fd) [[likely]]
		{
			::fast_io::wine::wine_unix_close_returns_status(this->host_fd);
		}
	}
};

} // namespace fast_io

#include "../../../fast_io_dsal/impl/misc/pop_macros.h"
