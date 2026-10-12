#pragma once

namespace fast_io
{

enum class wine_family : ::std::uint_least32_t
{
	unspecified_host = 0,
	native = unspecified_host
};

struct wine_at_entry
{
	using native_handle_type = ::fast_io::wine_host_fd_t;
	native_handle_type host_fd{};
	inline explicit constexpr wine_at_entry() noexcept = default;
	inline explicit constexpr wine_at_entry(native_handle_type hfd) noexcept
		: host_fd(hfd)
	{
	}
};

/*
the dll's at_fdcwd token for host_dirfd: AT_FDCWD encoded as a host_fd on
the unixcall impl, fast_io's -3 sentinel on the nt impl.
*/
inline wine_at_entry wine_at_fdcwd() noexcept
{
	return wine_at_entry{::fast_io::wine::wine_unix_at_fdcwd()};
}

template <::fast_io::wine_family family, ::std::integral ch_type>
class basic_wine_family_io_observer
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = ::fast_io::wine_host_fd_t;
	native_handle_type host_fd{}; // unixcall impl: unix fd + 1. nt impl: raw HANDLE. 0 = no fd.
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

template <::fast_io::wine_family family, ::std::integral ch_type>
inline constexpr bool operator==(basic_wine_family_io_observer<family, ch_type> a,
								 basic_wine_family_io_observer<family, ch_type> b) noexcept
{
	return a.host_fd == b.host_fd;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <::fast_io::wine_family family, ::std::integral ch_type>
inline constexpr auto operator<=>(basic_wine_family_io_observer<family, ch_type> a,
								  basic_wine_family_io_observer<family, ch_type> b) noexcept
{
	return a.host_fd <=> b.host_fd;
}
#endif

template <::fast_io::wine_family family, ::std::integral ch_type>
inline constexpr wine_at_entry at(basic_wine_family_io_observer<family, ch_type> wiob) noexcept
{
	return wine_at_entry{wiob.host_fd};
}

template <::fast_io::wine_family family, ::std::integral ch_type>
inline constexpr basic_wine_family_io_observer<family, ch_type>
io_stream_ref_define(basic_wine_family_io_observer<family, ch_type> other) noexcept
{
	return other;
}

template <::fast_io::wine_family family, ::std::integral ch_type>
inline constexpr basic_wine_family_io_observer<family, char>
io_bytes_stream_ref_define(basic_wine_family_io_observer<family, ch_type> other) noexcept
{
	return {other.host_fd};
}

namespace details
{

using wine_iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= ::fast_io::wine_unix::iovec_t const *;

} // namespace details

} // namespace fast_io
