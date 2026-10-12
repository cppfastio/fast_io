#pragma once

namespace fast_io
{

namespace win32::nt::details
{

template <bool zw>
inline ::std::uint_least32_t nt_family_file_lock_common_impl(void *__restrict handle, flock_request_l64 &req,
															 bool failedimmediately) noexcept
{
	auto [status, file_position] = nt_get_file_position_impl<zw>(handle, req.start, req.whence);
	if (status)
	{
		return status;
	}
	::fast_io::win32::nt::io_status_block block;
	::std::int_least64_t len{req.len};
	if (len == 0)
	{
		len = INT_LEAST64_MAX;
	}
	req.whence = seekdir::beg;
	req.start = file_position;
	req.len = len;
	if (req.type == file_lock_mode::unlock)
	{
		status = ::fast_io::win32::nt::nt_unlock_file<zw>(handle, __builtin_addressof(block), __builtin_addressof(file_position),
														  __builtin_addressof(len), 0u);
	}
	else
	{
		status = ::fast_io::win32::nt::nt_lock_file<zw>(handle, nullptr, nullptr, nullptr, __builtin_addressof(block),
														__builtin_addressof(file_position), __builtin_addressof(len), 0u,
														failedimmediately, req.type != file_lock_mode::shared_lock);
	}
	return status;
}

template <bool zw>
inline void nt_family_file_unlock_common_impl(void *__restrict handle, flock_request_l64 &req) noexcept
{
	auto [status, file_position] = nt_get_file_position_impl<zw>(handle, req.start, req.whence);
	::std::int_least64_t len{req.len};
	if (status)
	{
		file_position = 0;
		len = 0;
	}
	if (len == 0)
	{
		len = INT_LEAST64_MAX;
	}
	req.whence = seekdir::beg;
	req.start = file_position;
	req.len = len;
	::fast_io::win32::nt::io_status_block block;
	::fast_io::win32::nt::nt_unlock_file<zw>(handle, __builtin_addressof(block), __builtin_addressof(file_position),
											 __builtin_addressof(len), 0u);
}

template <bool zw>
inline void nt_family_file_lock_common_impl(void *__restrict handle, flock_request_l64 &req)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto status{nt_family_file_lock_common_impl<zw>(handle, req, false)};
	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
}

template <bool zw, ::std::integral int_type>
inline void nt_family_file_lock_impl(void *__restrict handle, basic_flock_request<int_type> &__restrict t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (sizeof(int_type) >= sizeof(::std::int_least64_t))
	{
		constexpr int_type mn{::std::numeric_limits<::std::int_least64_t>::min()};
		constexpr int_type mx{::std::numeric_limits<::std::int_least64_t>::max()};
		if (t.start < mn || mx < t.start || t.len < mn || mx < t.len)
		{
			::fast_io::herbceptions::throws_nt_errc_with_value(0xC0000095); // STATUS_INTEGER_OVERFLOW
		}
	}
	nt_family_file_lock_common_impl<zw>(handle, t);
}

template <bool zw, ::std::integral int_type>
inline void nt_family_file_unlock_impl(void *__restrict handle, basic_flock_request<int_type> &__restrict t) noexcept
{
	if constexpr (sizeof(int_type) >= sizeof(::std::int_least64_t))
	{
		constexpr int_type mn{::std::numeric_limits<::std::int_least64_t>::min()};
		constexpr int_type mx{::std::numeric_limits<::std::int_least64_t>::max()};
		if (t.start < mn || mx < t.start || t.len < mn || mx < t.len)
		{
			fast_terminate();
		}
	}
	nt_family_file_unlock_common_impl<zw>(handle, t);
}

template <bool zw, ::std::integral int_type>
inline bool nt_family_file_try_lock_impl(void *__restrict handle, basic_flock_request<int_type> &__restrict t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (sizeof(int_type) >= sizeof(::std::int_least64_t))
	{
		constexpr int_type mn{::std::numeric_limits<::std::int_least64_t>::min()};
		constexpr int_type mx{::std::numeric_limits<::std::int_least64_t>::max()};
		if (t.start < mn || mx < t.start || t.len < mn || mx < t.len)
		{
			return false;
		}
	}
	if constexpr (::std::same_as<int_type, ::std::int_least64_t>)
	{
		return nt_family_file_lock_common_impl<zw>(handle, t, true) == 0;
	}
	else
	{
		static_assert(::std::same_as<int_type, ::std::int_least64_t>);
		return false;
#if 0
		return nt_family_file_lock_common_impl<zw>(handle,flock_request_l64{t.type,t.whence,static_cast<::std::int_least64_t>(t.start),static_cast<::std::int_least64_t>(t.len)},true)==0;
#endif
	}
}

} // namespace win32::nt::details

template <nt_family family>
struct nt_family_file_lock
{
	void *handle{};
	template <::std::signed_integral int_type>
		requires(sizeof(int_type) >= sizeof(::std::int_least64_t))
	inline void lock(basic_flock_request<int_type> &__restrict t)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		::fast_io::win32::nt::details::nt_family_file_lock_impl<family == nt_family::zw>(this->handle, t);
	}
	template <::std::signed_integral int_type>
		requires(sizeof(int_type) >= sizeof(::std::int_least64_t))
	inline void unlock(basic_flock_request<int_type> &__restrict t) noexcept
	{
		::fast_io::win32::nt::details::nt_family_file_unlock_impl<family == nt_family::zw>(this->handle, t);
	}
	template <::std::signed_integral int_type>
		requires(sizeof(int_type) >= sizeof(::std::int_least64_t))
	inline bool try_lock(basic_flock_request<int_type> &__restrict t) noexcept
	{
		return ::fast_io::win32::nt::details::nt_family_file_try_lock_impl<family == nt_family::zw>(this->handle, t);
	}
};

using nt_file_lock = nt_family_file_lock<nt_family::nt>;
using zw_file_lock = nt_family_file_lock<nt_family::zw>;

template <nt_family family, ::std::integral char_type>
inline constexpr nt_family_file_lock<family> file_lock(basic_nt_family_io_observer<family, char_type> niob) noexcept
{
	return {niob};
}

} // namespace fast_io
