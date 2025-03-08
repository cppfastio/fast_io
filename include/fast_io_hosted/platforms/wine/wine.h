#pragma once

namespace fast_io
{

enum class wine_family : ::std::uint_least32_t
{
	unspecified_host = 0,
	native = unspecified_host
};

using wine_host_fd_t = ::std::common_type_t<::std::size_t, ::std::uint_least32_t>;

template <wine_family family, ::std::integral ch_type>
class basic_wine_family_io_observer
{
public:
	using char_type = ch_type;
	using native_handle_type = ::fast_io::wine_host_fd_t;
	native_handle_type host_fd{}; // host_fd has +1 offset on unix-like systems, the same value as nt handle on windows
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return host_fd;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return host_fd;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{host_fd};
		host_fd = 0;
		return temp;
	}
};

template <wine_family family>
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
			::__wine_unix_ret_close(host_fd);
		}
	}
};

using wine_file_factory = wine_family_file_factory<wine_family::native>;

template <wine_family family, ::std::integral ch_type>
class basic_wine_family_file : public basic_wine_family_io_observer<family, ch_type>
{
public:
	using typename basic_wine_family_io_observer<family, ch_type>::char_type;
	using typename basic_wine_family_io_observer<family, ch_type>::native_handle_type;
	using file_factory_type = wine_family_file_factory<family>;
	inline constexpr basic_wine_family_file() noexcept = default;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr basic_wine_family_file(native_hd host_fd1) noexcept
		: basic_wine_family_io_observer<family, ch_type>{host_fd1}
	{
	}
	inline constexpr basic_wine_family_file(decltype(nullptr)) noexcept = delete;
	inline constexpr basic_wine_family_file(basic_wine_family_file<family, ch_type>) noexcept = delete;
	inline constexpr basic_wine_family_file &operator=(basic_wine_family_file<family, ch_type>) noexcept = delete;
	inline ~basic_wine_family_file()
	{
		if (host_fd) [[likely]]
		{
			::__wine_unix_ret_close(host_fd);
		}
	}
};

namespace freestanding
{
template <wine_family family>
struct is_trivially_copyable_or_relocatable<wine_family_file_factory<family>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding
} // namespace fast_io
