#pragma once
#if __has_include(<initializer_list>)
#include <initializer_list>
#endif

namespace fast_io
{

namespace posix
{
#ifdef __DARWIN_C_LEVEL
extern int libc_faccessat(int dirfd, char const *pathname, int mode, int flags) noexcept __asm__("_faccessat");
extern int libc_fexecve(int fd, char *const *argv, char *const *envp) noexcept __asm__("_fexecve");
#else
extern int libc_faccessat(int dirfd, char const *pathname, int mode, int flags) noexcept __asm__("faccessat");
extern int libc_fexecve(int fd, char *const *argv, char *const *envp) noexcept __asm__("fexecve");
#endif
} // namespace posix

struct posix_wait_status
{
	int wait_loc{};
};
#if 0
inline constexpr posix_wait_reason reason(posix_wait_status pws) noexcept
{
#ifdef WIFEXITED
	if(WIFEXITED(pws.wait_loc))
		return posix_wait_reason::if_exited;
#endif
#ifdef WIFSIGNALED
	if(WIFSIGNALED(pws.wait_loc))
		return posix_wait_reason::if_signaled;
#endif
#ifdef WIFCORED
	if(WIFCORED(pws.wait_loc))
		return posix_wait_reason::if_core_dump;
#endif
#ifdef WIFSTOPPED
	if(WIFSTOPPED(pws.wait_loc))
		return posix_wait_reason::if_stopped;
#endif
	return posix_wait_reason::none;
}

inline constexpr int native_code(posix_wait_status pws) noexcept
{
	return pws.wait_loc;
}

inline constexpr ::std::uintmax_t code(posix_wait_status pws) noexcept
{
	return static_cast<::std::uintmax_t>(pws.wait_loc);

template<::std::integral char_type>
inline constexpr ::std::size_t print_reserve_size(io_reserve_type_t<char_type,posix_wait_status>) noexcept
{
	return sizeof(u8"reason:")+print_reserve_size(io_reserve_type<char_type,posix_wait_reason>)
		+sizeof(u8" native_code:")+print_reserve_size(io_reserve_type<char_type,int>);
}

template<::std::integral char_type,::std::random_access_iterator Iter>
inline constexpr Iter print_reserve_define(io_reserve_type_t<char_type,posix_wait_status>,Iter iter,posix_wait_status pws) noexcept
{
	if constexpr(::std::same_as<char_type,char>)
		iter=details::copy_string_literal("reason:",iter);
	else if constexpr(::std::same_as<char_type,wchar_t>)
		iter=details::copy_string_literal(L"reason:",iter);
	else if constexpr(::std::same_as<char_type,char16_t>)
		iter=details::copy_string_literal(u"reason:",iter);
	else if constexpr(::std::same_as<char_type,char32_t>)
		iter=details::copy_string_literal(U"reason:",iter);
	else
		iter=details::copy_string_literal(u8"reason:",iter);
	iter=print_reserve_define(io_reserve_type<char_type,posix_wait_reason>,iter,reason(pws));
	if constexpr(::std::same_as<char_type,char>)
		iter=details::copy_string_literal(" native_code:",iter);
	else if constexpr(::std::same_as<char_type,wchar_t>)
		iter=details::copy_string_literal(L" native_code:",iter);
	else if constexpr(::std::same_as<char_type,char16_t>)
		iter=details::copy_string_literal(u" native_code:",iter);
	else if constexpr(::std::same_as<char_type,char32_t>)
		iter=details::copy_string_literal(U" native_code:",iter);
	else
		iter=details::copy_string_literal(u8" native_code:",iter);
	return print_reserve_define(io_reserve_type<char_type,int>,iter,pws.wait_loc);
}
#endif

namespace details
{
inline pid_t posix_fork()
{

#if defined(__linux__) && defined(__NR_fork)
	pid_t pid{system_call<__NR_fork, pid_t>()};
	system_call_throw_error(pid);
#else
	pid_t pid{noexcept_call(::fork)};
	if (pid == -1) [[unlikely]]
	{
		throw_posix_error();
	}
#endif
	return pid;
}

inline posix_wait_status posix_waitpid(pid_t pid)
{
	posix_wait_status status;
#if defined(__linux__) && defined(__NR_wait4)
	system_call_throw_error(system_call<__NR_wait4, int>(pid, __builtin_addressof(status.wait_loc), 0, nullptr));
#else
	if (noexcept_call(waitpid, pid, __builtin_addressof(status.wait_loc), 0) == -1)
	{
		throw_posix_error();
	}
#endif
	return status;
}

inline void posix_waitpid_noexcept(pid_t pid) noexcept
{
	if (pid == -1) [[unlikely]]
	{
		return; // -1 indicates an empty process object
	}
#if defined(__linux__) && defined(__NR_wait4)
	system_call<__NR_wait4, int>(pid, nullptr, 0, nullptr);
#else
	noexcept_call(waitpid, pid, nullptr, 0);
#endif
}

#if 0
[[noreturn]] inline void posix_execveat(int dirfd, char const *path, char const *const *argv,
										char const *const *envp) noexcept
{
#if defined(__linux__) && defined(__NR_execveat)
	system_call<__NR_execveat, int>(dirfd, path, argv, envp, AT_SYMLINK_NOFOLLOW);
	fast_terminate();
#else
	int fd{::openat(dirfd, path, O_RDONLY | O_EXCL, 0644)};
	if (fd == -1) [[unlikely]]
	{
		fast_terminate();
	}
	::fast_io::posix::libc_fexecve(fd, const_cast<char *const *>(argv), const_cast<char *const *>(envp));
	fast_terminate();
#endif
}

inline int child_process_deal_with_process_io(posix_io_redirection const &red, int fd) noexcept
{
	bool is_stdin{fd == 0};
	if (red.pipe_fds)
	{
		auto v{red.pipe_fds[!is_stdin]};
		if (v != -1)
		{
			fd = v;
		}
		int &closefd{red.pipe_fds[is_stdin]};
		if (closefd != -1)
		{
			sys_close(closefd);
			closefd = -1;
		}
	}
	else if (red.fd != -1)
	{
		fd = red.fd;
	}
	else if (red.dev_null)
	{
		fd = -1;
	}
	return fd;
}

inline void child_process_execveat(int dirfd, char const *cstr, char const *const *args_ptr,
								   char const *const *envp_ptr, posix_process_io const &pio) noexcept
{
	int in_fd{child_process_deal_with_process_io(pio.in, 0)};
	int out_fd{child_process_deal_with_process_io(pio.out, 1)};
	int err_fd{child_process_deal_with_process_io(pio.err, 2)};
	if ((in_fd == -1) | (out_fd == -1) | (err_fd == -1))
	{
		posix_file pf{my_posix_open<true>("/dev/null", O_RDWR, 0644)};
		if (in_fd == -1)
		{
			sys_dup2<true>(pf.fd, 0);
		}
		if (out_fd == -1)
		{
			sys_dup2<true>(pf.fd, 1);
		}
		if (err_fd == -1)
		{
			sys_dup2<true>(pf.fd, 2);
		}
		sys_close(pf.fd);
	}
	if ((in_fd != -1) & (in_fd != 0))
	{
		sys_dup2<true>(in_fd, 0);
		sys_close(in_fd);
	}
	if ((out_fd != -1) & (out_fd != 1))
	{
		sys_dup2<true>(out_fd, 1);
		sys_close(out_fd);
	}
	if ((err_fd != -1) & (err_fd != 2))
	{
		sys_dup2<true>(err_fd, 2);
		sys_close(err_fd);
	}
	posix_execveat(dirfd, cstr, args_ptr, envp_ptr);
};

template <bool is_stdin>
inline void parent_process_deal_with_process_io(posix_io_redirection const &red) noexcept
{
	if (red.pipe_fds)
	{
		int &fd{red.pipe_fds[!is_stdin]};
		if (fd != -1)
		{
			sys_close(fd);
			fd = -1;
		}
	}
}

inline pid_t posix_fork_execveat_common_impl(int dirfd, char const *cstr, char const *const *args,
											 char const *const *envp, posix_process_io const &pio)
{
	system_call_throw_error(::fast_io::posix::libc_faccessat(dirfd, cstr, X_OK, AT_SYMLINK_NOFOLLOW));

	pid_t pid{posix_fork()};
	if (pid)
	{
		parent_process_deal_with_process_io<true>(pio.in);
		parent_process_deal_with_process_io<false>(pio.out);
		parent_process_deal_with_process_io<false>(pio.err);
		return pid;
	}
	child_process_execveat(dirfd, cstr, args, envp, pio);
	fast_terminate();
}

template <typename path_type>
inline pid_t posix_fork_execveat_impl(int dirfd, path_type const &csv, char const *const *args, char const *const *envp,
									  posix_process_io const &pio)
{
	return ::fast_io::posix_api_common(csv, [&](char const *cstr) { return posix_fork_execveat_common_impl(dirfd, cstr, args, envp, pio); });
}

template <typename path_type>
inline pid_t posix_fork_execve_impl(path_type const &csv, char const *const *args, char const *const *envp,
									posix_process_io const &pio)
{
#if defined(AT_FDCWD)
	return posix_fork_execveat_impl(AT_FDCWD, csv, args, envp, pio);
#else
	throw_posix_error(EINVAL);
	return -1;
#endif
}
#endif

struct fd_remapper
{
	struct entry
	{
		int original{-1};
		int backup{-1};
		int newfd{-1};
		int newfd_flag{};
		bool setfd_needed{};

		entry() = default;
		entry(entry const &) = delete;
		entry &operator=(entry const &) = delete;

		~entry()
		{
			if (original == -1)
			{
				return;
			}
			sys_dup2(backup, original);
			sys_close(backup);
			if (setfd_needed)
			{
				sys_fcntl(newfd, F_SETFD, newfd_flag);
			}
		}
	};

	entry fds[3]{};
	int fd_devnull{-1};

	fd_remapper() = default;
	fd_remapper(fd_remapper const &) = delete;
	fd_remapper &operator=(fd_remapper const &) = delete;

	~fd_remapper()
	{
		if (fd_devnull != -1)
		{
			sys_close(fd_devnull);
		}
	}

	// fd in {0, 1, 2}
	void map(int fd, posix_io_redirection const &io)
	{
		if (!io)
		{
			return;
		}
		auto &m = fds[fd];
		m.original = fd;
		m.backup = sys_dup(m.original);
		sys_fcntl(m.backup, F_SETFD, FD_CLOEXEC);
		bool const is_stdin{fd == 0};
		if (io.pipe_fds) // fastio pipes are always with CLOEXEC
		{
			sys_dup2(io.pipe_fds[is_stdin ? 0 : 1], m.original);
		}
		else if (io.dev_null)
		{
			sys_dup2(devnull(), m.original);
		}
		else
		{
			m.newfd = io.fd;
			m.newfd_flag = sys_fcntl(m.newfd, F_GETFD);
			sys_fcntl(m.newfd, F_SETFD, m.newfd_flag | FD_CLOEXEC);
			m.setfd_needed = true;
			sys_dup2(io.fd, m.original);
		}
	}

private:
	int devnull()
	{
		if (fd_devnull != -1)
		{
			return fd_devnull;
		}
#ifdef O_CLOEXEC
		fd_devnull = my_posix_open<true>(reinterpret_cast<char const*>(u8"/dev/null"), O_RDWR | O_CLOEXEC, 0644);
#else
		fd_devnull = my_posix_open<true>(reinterpret_cast<char const*>(u8"/dev/null"), O_RDWR, 0644);
		sys_fcntl(tmp_fd, F_SETFD, FD_CLOEXEC);
#endif
		return fd_devnull;
	}
};

// only used in vfork_execveat_common_impl()
inline void execveat_inside_vfork(int dirfd, char const *cstr, char const *const *args, char const *const *envp, int volatile &t_errno) noexcept
{
#if defined(__linux__) && defined(__NR_execveat)
	auto ret{system_call<__NR_execveat, int>(dirfd, cstr, args, envp, AT_SYMLINK_NOFOLLOW)};
	if (::fast_io::linux_system_call_fails(ret))
	{
		t_errno = -ret;
	}
	else
	{
		t_errno = 0;
	}
	::fast_io::fast_exit(127);
#else
	int fd{noexcept_call(::openat, dirfd, cstr, O_RDONLY | O_NOFOLLOW, 0644)};
	if (fd != -1) [[likely]]
	{
		::fast_io::posix::libc_fexecve(fd, const_cast<char *const *>(argv), const_cast<char *const *>(envp));
	}
	t_errno = errno;
	noexcept_call(::_exit, 127);
#endif
	__builtin_unreachable();
}

inline pid_t posix_vfork()
{
#if defined(__linux__) && defined(__NR_vfork)
	pid_t pid{system_call<__NR_vfork, pid_t>()};
	system_call_throw_error(pid);
#else
	pid_t pid{noexcept_call(::vfork)};
	if (pid == -1) [[unlikely]]
	{
		throw_posix_error();
	}
#endif
	return pid;
}

inline pid_t vfork_execveat_common_impl(int dirfd, char const *cstr, char const *const *args, char const *const *envp, posix_process_io const &pio)
{
	pid_t pid{};
	int volatile t_errno{}; // receive error from vfork subproc
	{
		fd_remapper fm;
		fm.map(0, pio.in);
		fm.map(1, pio.out);
		fm.map(2, pio.err);

		pid = ::fast_io::details::posix_vfork();
		if (pid == 0)
		{
			execveat_inside_vfork(dirfd, cstr, args, envp, t_errno); // never return
		}
	}
	// resume from vfork
	if (t_errno)
	{
		posix_waitpid(pid);
		throw_posix_error(t_errno);
	}
	return pid;
}

template <typename path_type>
inline pid_t vfork_execveat_impl(int dirfd, path_type const &csv, char const *const *args, char const *const *envp, posix_process_io const &pio)
{
	return ::fast_io::posix_api_common(csv, [&](char const *cstr) { return vfork_execveat_common_impl(dirfd, cstr, args, envp, pio); });
}

template <typename path_type>
inline pid_t vfork_execve_impl(path_type const &csv, char const *const *args, char const *const *envp, posix_process_io const &pio)
{
#if defined(AT_FDCWD)
	return vfork_execveat_impl(AT_FDCWD, csv, args, envp, pio);
#else
	throw_posix_error(EINVAL);
	return -1;
#endif
}

} // namespace details

class posix_process_observer
{
public:
	using native_handle_type = pid_t;
	pid_t pid{-1};
	constexpr auto &native_handle() noexcept
	{
		return pid;
	}
	constexpr auto &native_handle() const noexcept
	{
		return pid;
	}
	explicit inline constexpr operator bool() const noexcept
	{
		return pid != -1;
	}
	inline constexpr pid_t release() noexcept
	{
		auto temp{pid};
		pid = -1;
		return temp;
	}
};

inline constexpr void detach(posix_process_observer &ppob) noexcept
{
	ppob.pid = -1;
}

inline posix_wait_status wait(posix_process_observer &ppob)
{
	posix_wait_status status{details::posix_waitpid(ppob.pid)};
	ppob.pid = -1;
	return status;
}

inline constexpr bool operator==(posix_process_observer a, posix_process_observer b) noexcept
{
	return a.pid == b.pid;
}

inline constexpr auto operator<=>(posix_process_observer a, posix_process_observer b) noexcept
{
	return a.pid <=> b.pid;
}

class posix_process : public posix_process_observer
{
public:
	using native_handle_type = pid_t;
	explicit constexpr posix_process() noexcept = default;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	explicit constexpr posix_process(native_hd pid1) noexcept
		: posix_process_observer{pid1}
	{
	}
	template <::fast_io::constructible_to_os_c_str path_type>
	posix_process(posix_at_entry pate, path_type const &filename, posix_process_args const &args,
				  posix_process_envs const &envp, posix_process_io const &pio)
		: posix_process_observer{details::vfork_execveat_impl(pate.fd, filename, args.get(), envp.get(), pio)}
	{
	}

	template <::fast_io::constructible_to_os_c_str path_type>
	posix_process(path_type const &filename, posix_process_args const &args, posix_process_envs const &envp,
				  posix_process_io const &pio)
		: posix_process_observer{::fast_io::details::vfork_execve_impl(filename, args.get(), envp.get(), pio)}
	{
	}

	posix_process(::fast_io::posix_fs_dirent ent, posix_process_args const &args, posix_process_envs const &envp,
				  posix_process_io const &pio)
		: posix_process_observer{
			  ::fast_io::details::vfork_execveat_common_impl(ent.fd, ent.filename, args.get(), envp.get(), pio)}
	{
	}

	posix_process(posix_process const &) = delete;
	posix_process &operator=(posix_process const &) = delete;
	constexpr posix_process(posix_process &&__restrict other) noexcept
		: posix_process_observer{other.pid}
	{
		other.pid = -1;
	}
	posix_process &operator=(posix_process &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) != this)
		{
			return *this;
		}
		::fast_io::details::posix_waitpid_noexcept(this->pid);
		this->pid = other.pid;
		other.pid = -1;
		return *this;
	}
	~posix_process()
	{
		details::posix_waitpid_noexcept(this->pid);
		this->pid = -1;
	}
};

} // namespace fast_io
