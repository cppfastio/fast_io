#pragma once
namespace fast_io
{

namespace details
{
inline int fcntl_file_lock(int fd,int cmd,struct flock const* lockp)
{
	return ::fcntl(fd,cmd,__builtin_addressof(lockp));
}

inline void posix_file_lock_lock_common_impl(int fd,struct flock const& lockp)
{
#if defined(__linux__) && defined(__NR_fcntl)
	system_call_throw_error(system_call<__NR_fcntl,int>(fd,F_SETLKW,__builtin_addressof(lockp)));
#else
	int ret{noexcept_call(fcntl_file_lock,fd,F_SETLKW,__builtin_addressof(lockp))};
	if(ret<0)
	{
		throw_posix_error();
	}
#endif
}

inline constexpr short flock_type_to_native(file_lock_mode t) noexcept
{
	switch(t)
	{
	case file_lock_mode::shared_lock:
	{
		return F_RDLCK;
	}
	case file_lock_mode::unlock:
	{
		return F_UNLCK;
	}
	default:
	{
		return F_WRLCK;
	}
	}
}

template<std::integral int_type>
inline void posix_file_lock_lock_impl(int fd,basic_flock_request<int_type> const& __restrict t)
{
	using flock_off_type = decltype(flock::l_start);
	if constexpr(sizeof(flock_off_type)<sizeof(int_type))
	{
		constexpr int_type flock_off_min{std::numeric_limits<flock_off_type>::min()};
		constexpr int_type flock_off_max{std::numeric_limits<flock_off_type>::max()};
		if(t.start<flock_off_min||flock_off_max<t.start||
			t.len<flock_off_min||flock_off_max<t.len)
		{
			throw_posix_error(EINVAL);
		}
	}
	struct flock temp{};
	temp.l_type = flock_type_to_native(t.type);
	temp.l_start=static_cast<flock_off_type>(t.start);
	temp.l_whence=static_cast<short>(t.whence);
	temp.l_pid = -1;
	temp.l_len = static_cast<flock_off_type>(t.len);
	posix_file_lock_lock_common_impl(fd,temp);
}

inline bool posix_file_lock_try_lock_common_impl(int fd,struct flock const& lockp) noexcept
{
#if defined(__linux__) && defined(__NR_fcntl)
	return !linux_system_call_fails(system_call<__NR_fcntl,int>(fd,F_SETLK,__builtin_addressof(lockp)));
#else
	int ret{noexcept_call(fcntl_file_lock,fd,F_SETLK,__builtin_addressof(lockp))};
	return 0<=ret;
#endif
}

template<std::integral int_type>
inline bool posix_file_lock_try_lock_impl(int fd,basic_flock_request<int_type> const& __restrict t) noexcept
{
	using flock_off_type = decltype(flock::l_start);
	if constexpr(sizeof(flock_off_type)<sizeof(int_type))
	{
		constexpr int_type flock_off_min{std::numeric_limits<flock_off_type>::min()};
		constexpr int_type flock_off_max{std::numeric_limits<flock_off_type>::max()};
		if(t.start<flock_off_min||flock_off_max<t.start||
			t.len<flock_off_min||flock_off_max<t.len)
		{
			return false;
		}
	}
	struct flock temp{};
	temp.l_type = flock_type_to_native(t.type);
	temp.l_start=static_cast<flock_off_type>(t.start);
	temp.l_whence=static_cast<short>(t.whence);
	temp.l_pid = -1;
	temp.l_len = static_cast<flock_off_type>(t.len);
	return posix_file_lock_try_lock_common_impl(fd,temp);
}


template<std::integral int_type>
inline void posix_file_lock_unlock_impl(int fd,basic_flock_request<int_type> const& __restrict t) noexcept
{
	using flock_off_type = decltype(flock::l_start);
	if constexpr(sizeof(flock_off_type)<sizeof(int_type))
	{
		constexpr int_type flock_off_min{std::numeric_limits<flock_off_type>::min()};
		constexpr int_type flock_off_max{std::numeric_limits<flock_off_type>::max()};
		if(t.start<flock_off_min||flock_off_max<t.start||
			t.len<flock_off_min||flock_off_max<t.len)
		{
			return;
		}
	}
	struct flock temp{};
	temp.l_type = flock_type_to_native(t.type);
	temp.l_start=static_cast<flock_off_type>(t.start);
	temp.l_whence=static_cast<short>(t.whence);
	temp.l_pid = -1;
	temp.l_len = static_cast<flock_off_type>(t.len);
#if defined(__linux__) && defined(__NR_fcntl)
	system_call<__NR_fcntl,int>(fd,F_SETLKW,__builtin_addressof(temp));
#else
	noexcept_call(fcntl_file_lock,fd,F_SETLKW,__builtin_addressof(temp));
#endif
}

}

struct posix_file_lock
{
	int fd{-1};
	template<std::integral int_type>
	inline void lock(basic_flock_request<int_type> const& __restrict t)
	{
		::fast_io::details::posix_file_lock_lock_impl(this->fd,t);
	}
	template<std::integral int_type>
	inline void unlock(basic_flock_request<int_type> const& __restrict t) noexcept
	{
		::fast_io::details::posix_file_lock_unlock_impl(this->fd,t);
	}
	template<std::integral int_type>
	inline void try_lock(basic_flock_request<int_type> const& __restrict t)
	{
		::fast_io::details::posix_file_lock_lock_impl(this->fd,t);
	}
};

}
