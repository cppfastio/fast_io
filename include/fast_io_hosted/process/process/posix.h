#pragma once
#if __has_include(<initializer_list>)
#include <initializer_list>
#endif

namespace fast_io
{

namespace posix
{
#if defined(__DARWIN_C_LEVEL) || defined(__MSDOS__)
// extern int libc_faccessat(int dirfd, char const *pathname, int mode, int flags) noexcept __asm__("_faccessat");
extern int libc_fexecve(int fd, char *const *argv, char *const *envp) noexcept __asm__("_fexecve");
extern int libc_kill(pid_t pid, int sig) noexcept __asm__("_kill");
extern pid_t libc_fork() noexcept __asm__("_fork");
extern pid_t libc_vfork() noexcept __asm__("_vfork");
extern pid_t libc_waitpid(pid_t pid, int *status, int options) noexcept __asm__("_waitpid");
[[noreturn]] extern void libc_exit(int status) noexcept __asm__("__Exit");
[[noreturn]] extern void libc_exit2(int status) noexcept __asm__("__exit");
#else
// extern int libc_faccessat(int dirfd, char const *pathname, int mode, int flags) noexcept __asm__("faccessat");
extern int libc_fexecve(int fd, char *const *argv, char *const *envp) noexcept __asm__("fexecve");
extern int libc_kill(pid_t pid, int sig) noexcept __asm__("kill");
extern pid_t libc_fork() noexcept __asm__("fork");
extern pid_t libc_vfork() noexcept __asm__("vfork");
extern pid_t libc_waitpid(pid_t pid, int *status, int options) noexcept __asm__("waitpid");
[[noreturn]] extern void libc_exit(int status) noexcept __asm__("_Exit");
[[noreturn]] extern void libc_exit2(int status) noexcept __asm__("_exit");
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
	pid_t pid{::fast_io::posix::libc_fork()};
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
	if (::fast_io::posix::libc_waitpid(pid, __builtin_addressof(status.wait_loc), 0) == -1)
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
	::fast_io::posix::libc_waitpid(pid, nullptr, 0);
#endif
}

inline return_code posix_execveat(int dirfd, char const *cstr, char const *const *args, char const *const *envp) noexcept
{
#if defined(__linux__) && defined(__NR_execveat)
	auto ret{system_call<__NR_execveat, int>(dirfd, cstr, args, envp, AT_SYMLINK_NOFOLLOW)};
	if (linux_system_call_fails(ret))
	{
		return {-1, -ret};
	}
	return {0, ret};
#else
	int fd{::fast_io::details::my_posix_openat_noexcept(dirfd, cstr, O_RDONLY | O_NOFOLLOW, 0644)};
	if (fd != -1) [[likely]]
	{
		::fast_io::posix::libc_fexecve(fd, const_cast<char *const *>(args), const_cast<char *const *>(envp));
	}
	return {fd, errno};
#endif
}

struct io_redirector
{
	int fd_devnull{-1};

	inline io_redirector() = default;
	inline io_redirector(io_redirector const &) = delete;
	inline io_redirector &operator=(io_redirector const &) = delete;
	inline ~io_redirector()
	{
		if (fd_devnull != -1)
		{
			sys_close(fd_devnull);
		}
	}

	// only used by sub process
	inline return_code redirect_all(posix_process_io const &pio) noexcept
	{
		return_code rc;
		rc = redirect(0, pio.in);
		if (rc.fd == -1)
		{
			return rc;
		}
		rc = redirect(1, pio.out);
		if (rc.fd == -1)
		{
			return rc;
		}
		rc = redirect(2, pio.err);
		return rc;
	}

	inline return_code redirect(int target_fd, posix_io_redirection const &d) noexcept
	{
		if (!d)
		{
			return {};
		}
		bool const is_stdin{target_fd == 0};
		return_code rc;
		if (d.pipe_fds)
		{
			// the read/write ends of pipe are all open
			// the user shouldn't close them if they pass entire pipe as argument
			rc = sys_dup2_nothrow(d.pipe_fds[is_stdin ? 0 : 1], target_fd);
			if (rc.fd == -1)
			{
				return rc;
			}
			// it's actually OK to go without closing pipe ends since fast_io pipes are all CLOEXEC
			sys_close(d.pipe_fds[is_stdin ? 1 : 0]);
		}
		else if (d.dev_null)
		{
			rc = sys_dup2_nothrow(devnull(), target_fd);
		}
		else
		{
			rc = sys_dup2_nothrow(d.fd, target_fd);
		}
		return rc;
	}

	// only used by parent process
	static inline void close_pipe_ends(int target_fd, posix_io_redirection const &d) noexcept
	{
		if (!d)
		{
			return;
		}
		if (!d.pipe_fds)
		{
			return;
		}
		bool const is_stdin{target_fd == 0};
		sys_close(d.pipe_fds[is_stdin ? 0 : 1]);
	}

private:
	inline int devnull()
	{
		if (fd_devnull != -1)
		{
			return fd_devnull;
		}
#ifdef O_CLOEXEC
		fd_devnull = my_posix_open<true>(reinterpret_cast<char const *>(u8"/dev/null"), O_RDWR | O_CLOEXEC, 0644);
#else
		fd_devnull = my_posix_open<true>(reinterpret_cast<char const *>(u8"/dev/null"), O_RDWR, 0644);
		sys_fcntl(tmp_fd, F_SETFD, FD_CLOEXEC);
#endif
		return fd_devnull;
	}
};

inline pid_t pipefork_execveat_common_impl(int dirfd, char const *cstr, char const *const *args, char const *const *envp, posix_process_io const &pio)
{
	pid_t pid = posix_fork();
	if (pid == 0)
	{
		// subprocess
		return_code rc;
		// io redirection
		io_redirector r;
		rc = r.redirect_all(pio);

		if (rc.fd != -1)
		{
			posix_execveat(dirfd, cstr, args, envp);
		}
		// special exit code 127 indicates error of exec
#if defined(__linux__)
#ifdef __NR_exit_group
		::fast_io::system_call_no_return<__NR_exit_group>(127);
#else
		::fast_io::system_call_no_return<__NR_exit>(127);
#endif
#else
		::fast_io::posix::libc_exit(127);
#endif
	}
	posix_waitpid(pid);
	return 0;
}

template <typename path_type>
inline pid_t pipefork_execveat_impl(int dirfd, path_type const &csv, char const *const *args, char const *const *envp, posix_process_io const &pio)
{
	return ::fast_io::posix_api_common(csv, [&](char const *cstr) { return pipefork_execveat_common_impl(dirfd, cstr, args, envp, pio); });
}

template <typename path_type>
inline pid_t pipefork_execve_impl(path_type const &csv, char const *const *args, char const *const *envp, posix_process_io const &pio)
{
#if defined(AT_FDCWD)
	return pipefork_execveat_impl(AT_FDCWD, csv, args, envp, pio);
#else
	throw_posix_error(EINVAL);
	return -1;
#endif
}

struct fd_remapper
{
	struct entry
	{
		int original{-1};
		int backup{-1};
		int newfd{-1};
		int newfd_flag{};
		bool setfd_needed{};

		inline entry() = default;
		inline entry(entry const &) = delete;
		inline entry &operator=(entry const &) = delete;

		inline ~entry()
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

	inline fd_remapper() = default;
	inline fd_remapper(fd_remapper const &) = delete;
	inline fd_remapper &operator=(fd_remapper const &) = delete;

	inline ~fd_remapper()
	{
		if (fd_devnull != -1)
		{
			sys_close(fd_devnull);
		}
	}

	// fd in {0, 1, 2}
	inline void map(int fd, posix_io_redirection const &io)
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
	inline int devnull()
	{
		if (fd_devnull != -1)
		{
			return fd_devnull;
		}
#ifdef O_CLOEXEC
		fd_devnull = my_posix_open<true>(reinterpret_cast<char const *>(u8"/dev/null"), O_RDWR | O_CLOEXEC, 0644);
#else
		fd_devnull = my_posix_open<true>(reinterpret_cast<char const *>(u8"/dev/null"), O_RDWR, 0644);
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
#if defined(__linux__)
#ifdef __NR_exit_group
	::fast_io::system_call_no_return<__NR_exit_group>(127);
#else
	::fast_io::system_call_no_return<__NR_exit>(127);
#endif
#else
	::fast_io::posix::libc_exit2(127);
#endif
#else
	int fd{::fast_io::details::my_posix_openat_noexcept(dirfd, cstr, O_RDONLY | O_NOFOLLOW, 0644)};
	if (fd != -1) [[likely]]
	{
		::fast_io::posix::libc_fexecve(fd, const_cast<char *const *>(args), const_cast<char *const *>(envp));
	}
	t_errno = errno;
	::fast_io::posix::libc_exit2(127);
#endif
	__builtin_unreachable();
}

inline pid_t posix_vfork()
{
#if defined(__linux__) && defined(__NR_vfork)
	pid_t pid{system_call<__NR_vfork, pid_t>()};
	system_call_throw_error(pid);
#else
	pid_t pid{::fast_io::posix::libc_vfork()};
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
	inline constexpr auto &native_handle() noexcept
	{
		return pid;
	}
	inline constexpr auto &native_handle() const noexcept
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

inline constexpr bool operator==(posix_process_observer a, posix_process_observer b) noexcept
{
	return a.pid == b.pid;
}

inline constexpr auto operator<=>(posix_process_observer a, posix_process_observer b) noexcept
{
	return a.pid <=> b.pid;
}

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

inline void kill(posix_process_observer ppob, posix_wait_status exit_code)
{
#if defined(__linux__) && defined(__NR_kill)
	system_call_throw_error(system_call<__NR_kill, int>(ppob.pid, exit_code.wait_loc));
#else
	system_call_throw_error(::fast_io::posix::libc_kill(ppob.pid, exit_code.wait_loc));
#endif
}

class posix_process : public posix_process_observer
{
public:
	using native_handle_type = pid_t;
	inline explicit constexpr posix_process() noexcept = default;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr posix_process(native_hd pid1) noexcept
		: posix_process_observer{pid1}
	{
	}
	template <::fast_io::constructible_to_os_c_str path_type>
	inline posix_process(posix_at_entry pate, path_type const &filename, posix_process_args const &args,
						 posix_process_envs const &envp, posix_process_io const &pio)
		: posix_process_observer{
#ifdef __DARWIN_C_LEVEL
			  ::fast_io::details::pipefork_execveat_impl(pate.fd, filename, args.get(), envp.get(), pio)
#else
			  ::fast_io::details::vfork_execveat_impl(pate.fd, filename, args.get(), envp.get(), pio)
#endif
		  }
	{
	}

	template <::fast_io::constructible_to_os_c_str path_type>
	inline posix_process(path_type const &filename, posix_process_args const &args, posix_process_envs const &envp,
						 posix_process_io const &pio)
		: posix_process_observer{
#ifdef __DARWIN_C_LEVEL
			  ::fast_io::details::pipefork_execve_impl(filename, args.get(), envp.get(), pio)
#else
			  ::fast_io::details::vfork_execve_impl(filename, args.get(), envp.get(), pio)
#endif
		  }
	{
	}

	inline posix_process(::fast_io::posix_fs_dirent ent, posix_process_args const &args, posix_process_envs const &envp,
						 posix_process_io const &pio)
		: posix_process_observer{
#ifdef __DARWIN_C_LEVEL
			  ::fast_io::details::pipefork_execveat_common_impl(ent.fd, ent.filename, args.get(), envp.get(), pio)
#else
			  ::fast_io::details::vfork_execveat_common_impl(ent.fd, ent.filename, args.get(), envp.get(), pio)
#endif
		  }
	{
	}

	inline posix_process(posix_process const &) = delete;
	inline posix_process &operator=(posix_process const &) = delete;
	inline constexpr posix_process(posix_process &&__restrict other) noexcept
		: posix_process_observer{other.pid}
	{
		other.pid = -1;
	}
	inline posix_process &operator=(posix_process &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		::fast_io::details::posix_waitpid_noexcept(this->pid);
		this->pid = other.pid;
		other.pid = -1;
		return *this;
	}
	inline ~posix_process()
	{
		details::posix_waitpid_noexcept(this->pid);
		this->pid = -1;
	}
};

} // namespace fast_io
