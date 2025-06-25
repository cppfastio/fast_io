#pragma once

namespace fast_io
{
struct win32_user_process_information
{
	void *hprocess{};
	void *hthread{};
};

namespace win32::details
{

inline void close_win32_user_process_information_not_null(win32_user_process_information hnt_user_process_info) noexcept
{
	if (hnt_user_process_info.hthread) [[likely]]
	{
		::fast_io::win32::CloseHandle(hnt_user_process_info.hthread);
	}
	if (hnt_user_process_info.hprocess) [[likely]]
	{
		::fast_io::win32::CloseHandle(hnt_user_process_info.hprocess);
	}
}

inline void close_win32_user_process_information(win32_user_process_information hnt_user_process_info) noexcept
{
	return details::close_win32_user_process_information_not_null(hnt_user_process_info);
}

inline ::std::uint_least32_t win32_wait_user_process_or_thread(void *hprocess_thread) noexcept
{
	return ::fast_io::win32::WaitForSingleObject(hprocess_thread, 0xFFFFFFFF);
}

template <bool throw_eh = false>
inline void win32_wait_and_close_user_process_or_thread(void *handle) noexcept(!throw_eh)
{
	if (handle == nullptr) [[unlikely]]
	{
		return;
	}
	auto status{win32_wait_user_process_or_thread(handle)};
	::fast_io::win32::CloseHandle(handle);
	if constexpr (throw_eh)
	{
		if (status) [[unlikely]]
		{
			throw_win32_error(status);
		}
	}
}

template <bool throw_eh = false>
inline void
close_win32_user_process_information_and_wait(win32_user_process_information hnt_user_process_info) noexcept(!throw_eh)
{
	// only need to wait hprocess
	win32_wait_and_close_user_process_or_thread<throw_eh>(hnt_user_process_info.hprocess);
	// close hthread (main thread)
	::fast_io::win32::CloseHandle(hnt_user_process_info.hthread);
}

inline void win32_duplicate_object_std(void *parent_process, void *&standard_io_handle, void *process_handle)
{
	if (standard_io_handle == nullptr) [[unlikely]]
	{
		return;
	}
	if (!::fast_io::win32::DuplicateHandle(
			parent_process, standard_io_handle, process_handle, __builtin_addressof(standard_io_handle), 0, 0,
			0x00000002 | 0x00000004)) [[unlikely]]
	{
		throw_win32_error();
	}
}

template <win32_family family>
using win32_process_char_type = ::std::conditional_t<family == win32_family::wide_nt, char16_t, char>;

// Applicable to unicode and ansi versions on winnt
template <win32_family family>
inline win32_user_process_information win32_winnt_process_create_from_handle_impl(void *__restrict fhandle, win32_process_char_type<family> const *args,
																				  win32_process_char_type<family> const *envs,
																				  win32_process_io const &__restrict processio, process_mode mode)
{
	constexpr bool is_nt{family == win32_family::wide_nt};
	if constexpr (is_nt)
	{
		// The GetAppedFileName function cannot obtain the required space size and directly allocates a large stack area
		char16_t pszFilename[0x2001];
		void *hFileMap{::fast_io::win32::CreateFileMappingW(fhandle, nullptr, 0x02, 0, 1, nullptr)};
		if (!hFileMap) [[unlikely]]
		{
			throw_win32_error();
		}
		handle_guard hg1{hFileMap};

		// Create a file mapping to get the file name.
		void *pMem{::fast_io::win32::MapViewOfFile(hFileMap, 0x04, 0, 0, 1)};
		if (!pMem) [[unlikely]]
		{
			throw_win32_error();
		}
		map_guard mg1{pMem};

		if (!
#if !defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x601
			::fast_io::win32::K32GetMappedFileNameW
#else
			::fast_io::win32::GetMappedFileNameW
#endif
			(reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1)),
			 pMem,
			 pszFilename,
			 0x2000)) [[unlikely]]
		{
			throw_win32_error();
		}

		mg1.clear();
		hg1.clear();

		auto address_begin{pszFilename};

		// change nt path to dos path
		if (::fast_io::freestanding::my_memcmp(pszFilename, u"\\Device\\Mup\\", 12 * sizeof(char16_t)) == 0)
		{
			address_begin += 10;
			*address_begin = u'\\';
			goto next;
		}
		{
			char16_t DosDevice[4]{0, u':', 0, 0};
			char16_t NtPath[1025];

			::std::size_t NtPathLen{};
			constexpr char16_t bg{u'A'};
			constexpr char16_t ed{u'Z' + 1};
			for (char16_t i{bg}; i != ed; ++i)
			{
				DosDevice[0] = i;
				if (::fast_io::win32::QueryDosDeviceW(DosDevice, NtPath, 1024))
				{
					NtPathLen = ::fast_io::cstr_len(NtPath);

					if (::fast_io::freestanding::my_memcmp(pszFilename, NtPath, NtPathLen * sizeof(char16_t)) == 0) [[unlikely]]
					{
						address_begin += NtPathLen - 2;
						address_begin[0] = DosDevice[0];
						address_begin[1] = u':';
						goto next;
					}
				}
			}

			throw_win32_error(0x3);
		}
	next:
		// create process
		::fast_io::win32::startupinfow si{};
		si.cb = sizeof(si);

		if (!processio.in.is_dev_null)
		{
			if (processio.in.win32_pipe_in_handle)
			{
				si.hStdInput = processio.in.win32_pipe_in_handle;

				if (!::fast_io::win32::SetHandleInformation(processio.in.win32_pipe_out_handle, 0x00000001 /*HANDLE_FLAG_INHERIT*/, 0)) [[unlikely]]
				{
					throw_win32_error();
				}
			}
			else if (processio.in.win32_handle)
			{
				si.hStdInput = processio.in.win32_handle;
			}
			else
			{
				si.hStdInput = ::fast_io::win32_stdin().handle;
			}
		}
		else
		{
			si.hStdInput = nullptr;
		}

		if (!processio.out.is_dev_null)
		{
			if (processio.out.win32_pipe_out_handle)
			{
				si.hStdOutput = processio.out.win32_pipe_out_handle;

				if (!::fast_io::win32::SetHandleInformation(processio.out.win32_pipe_in_handle, 0x00000001 /*HANDLE_FLAG_INHERIT*/, 0)) [[unlikely]]
				{
					throw_win32_error();
				}
			}
			else if (processio.out.win32_handle)
			{
				si.hStdOutput = processio.out.win32_handle;
			}
			else
			{
				si.hStdOutput = ::fast_io::win32_stdout().handle;
			}
		}
		else
		{
			si.hStdOutput = nullptr;
		}

		if (!processio.err.is_dev_null)
		{
			if (processio.err.win32_pipe_out_handle)
			{
				si.hStdError = processio.err.win32_pipe_out_handle;

				if (!::fast_io::win32::SetHandleInformation(processio.err.win32_pipe_in_handle, 0x00000001 /*HANDLE_FLAG_INHERIT*/, 0)) [[unlikely]]
				{
					throw_win32_error();
				}
			}
			else if (processio.err.win32_handle)
			{
				si.hStdError = processio.err.win32_handle;
			}
			else
			{
				si.hStdError = ::fast_io::win32_stderr().handle;
			}
		}
		else
		{
			si.hStdError = nullptr;
		}

		si.dwFlags = 0x00000100;

		::std::uint_least32_t dwCreationFlags{0x00000400 /*CREATE_UNICODE_ENVIRONMENT*/};

		if ((mode & process_mode::new_session) == process_mode::new_session)
		{
			dwCreationFlags |= 0x00000200; // CREATE_NEW_PROCESS_GROUP
		}
		if ((mode & process_mode::alloc_new_console) == process_mode::alloc_new_console)
		{
			dwCreationFlags |= 0x00000010; // CREATE_NEW_CONSOLE
		}

		::fast_io::win32::process_information pi{};
		if (!::fast_io::win32::CreateProcessW(address_begin, const_cast<char16_t *>(args), nullptr, nullptr, 1,
											  dwCreationFlags, (void *)envs, nullptr, __builtin_addressof(si), __builtin_addressof(pi)))
		{
			throw_win32_error();
		}

		return {pi.hProcess, pi.hThread};
	}
	else
	{
		// The GetAppedFileName function cannot obtain the required space size and directly allocates a large stack area
		char8_t pszFilename[0x8004];
		void *hFileMap{::fast_io::win32::CreateFileMappingA(fhandle, nullptr, 0x02, 0, 1, nullptr)};
		if (!hFileMap) [[unlikely]]
		{
			throw_win32_error();
		}
		handle_guard hg1{hFileMap};

		// Create a file mapping to get the file name.
		void *pMem{::fast_io::win32::MapViewOfFile(hFileMap, 0x04, 0, 0, 1)};
		if (!pMem) [[unlikely]]
		{
			throw_win32_error();
		}

		if (!
#if !defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x601
			::fast_io::win32::K32GetMappedFileNameA
#else
			::fast_io::win32::GetMappedFileNameA
#endif
			(reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1)),
			 pMem,
			 reinterpret_cast<char *>(pszFilename),
			 0x8000)) [[unlikely]]
		{
			throw_win32_error();
		}
		map_guard mg1{pMem};

		mg1.clear();
		hg1.clear();

		// change nt path to dos path (9x)
		auto address_begin{pszFilename};

		// change nt path to dos path
		if (::fast_io::freestanding::my_memcmp(pszFilename, u8"\\Device\\Mup\\", 12 * sizeof(char8_t)) == 0)
		{
			address_begin += 10;
			*address_begin = u8'\\';
			goto next2;
		}
		{
			char8_t DosDevice[4]{0, u8':', 0, 0};
			char8_t NtPath[1025];

			::std::size_t NtPathLen{};
			constexpr char8_t bg{u8'A'};
			constexpr char8_t ed{u8'Z' + 1};
			for (char8_t i{bg}; i != ed; ++i)
			{
				*DosDevice = i;
				if (::fast_io::win32::QueryDosDeviceA(reinterpret_cast<char const *>(DosDevice), reinterpret_cast<char *>(NtPath), 1024))
				{
					NtPathLen = ::fast_io::cstr_len(NtPath);

					if (::fast_io::freestanding::my_memcmp(pszFilename, NtPath, NtPathLen * sizeof(char8_t)) == 0) [[unlikely]]
					{
						address_begin += NtPathLen - 2;
						address_begin[0] = DosDevice[0];
						address_begin[1] = u8':';
						goto next2;
					}
				}
			}

			throw_win32_error(0x3);
		}
	next2:
		// create process
		::fast_io::win32::startupinfoa si{};
		si.cb = sizeof(si);

		if (!processio.in.is_dev_null)
		{
			if (processio.in.win32_pipe_in_handle)
			{
				si.hStdInput = processio.in.win32_pipe_in_handle;

				if (!::fast_io::win32::SetHandleInformation(processio.in.win32_pipe_out_handle, 0x00000001 /*HANDLE_FLAG_INHERIT*/, 0)) [[unlikely]]
				{
					throw_win32_error();
				}
			}
			else if (processio.in.win32_handle)
			{
				si.hStdInput = processio.in.win32_handle;
			}
			else
			{
				si.hStdInput = ::fast_io::win32_stdin().handle;
			}
		}
		else
		{
			si.hStdInput = nullptr;
		}

		if (!processio.out.is_dev_null)
		{
			if (processio.out.win32_pipe_out_handle)
			{
				si.hStdOutput = processio.out.win32_pipe_out_handle;

				if (!::fast_io::win32::SetHandleInformation(processio.out.win32_pipe_in_handle, 0x00000001 /*HANDLE_FLAG_INHERIT*/, 0)) [[unlikely]]
				{
					throw_win32_error();
				}
			}
			else if (processio.out.win32_handle)
			{
				si.hStdOutput = processio.out.win32_handle;
			}
			else
			{
				si.hStdOutput = ::fast_io::win32_stdout().handle;
			}
		}
		else
		{
			si.hStdOutput = nullptr;
		}

		if (!processio.err.is_dev_null)
		{
			if (processio.err.win32_pipe_out_handle)
			{
				si.hStdError = processio.err.win32_pipe_out_handle;

				if (!::fast_io::win32::SetHandleInformation(processio.err.win32_pipe_in_handle, 0x00000001 /*HANDLE_FLAG_INHERIT*/, 0)) [[unlikely]]
				{
					throw_win32_error();
				}
			}
			else if (processio.err.win32_handle)
			{
				si.hStdError = processio.err.win32_handle;
			}
			else
			{
				si.hStdError = ::fast_io::win32_stderr().handle;
			}
		}
		else
		{
			si.hStdError = nullptr;
		}

		si.dwFlags = 0x00000100;

		::std::uint_least32_t dwCreationFlags{};

		if ((mode & process_mode::new_session) == process_mode::new_session)
		{
			dwCreationFlags |= 0x00000200; // CREATE_NEW_PROCESS_GROUP
		}
		if ((mode & process_mode::alloc_new_console) == process_mode::alloc_new_console)
		{
			dwCreationFlags |= 0x00000010; // CREATE_NEW_CONSOLE
		}

		::fast_io::win32::process_information pi{};
		if (!::fast_io::win32::CreateProcessA(reinterpret_cast<char *>(address_begin), const_cast<char *>(args), nullptr, nullptr, 1,
											  dwCreationFlags, (void *)envs, nullptr, __builtin_addressof(si), __builtin_addressof(pi)))
		{
			throw_win32_error();
		}

		return {pi.hProcess, pi.hThread};
	}
}

// Applicable to ansi version on win9x
inline win32_user_process_information win32_9xa_win9x_process_create_from_filepath_impl(char const *filepath,
																						char const *args,
																						char const *envs,
																						win32_process_io const &__restrict processio,
																						process_mode mode)
{
	// there are no psapi.dll on windows 9x
	::fast_io::win32::startupinfoa si{};
	si.cb = sizeof(si);

	if (!processio.in.is_dev_null)
	{
		if (processio.in.win32_pipe_in_handle)
		{
			si.hStdInput = processio.in.win32_pipe_in_handle;

			if (!::fast_io::win32::SetHandleInformation(processio.in.win32_pipe_out_handle, 0x00000001 /*HANDLE_FLAG_INHERIT*/, 0)) [[unlikely]]
			{
				throw_win32_error();
			}
		}
		else if (processio.in.win32_handle)
		{
			si.hStdInput = processio.in.win32_handle;
		}
		else
		{
			si.hStdInput = ::fast_io::win32_stdin().handle;
		}
	}
	else
	{
		si.hStdInput = nullptr;
	}

	if (!processio.out.is_dev_null)
	{
		if (processio.out.win32_pipe_out_handle)
		{
			si.hStdOutput = processio.out.win32_pipe_out_handle;

			if (!::fast_io::win32::SetHandleInformation(processio.out.win32_pipe_in_handle, 0x00000001 /*HANDLE_FLAG_INHERIT*/, 0)) [[unlikely]]
			{
				throw_win32_error();
			}
		}
		else if (processio.out.win32_handle)
		{
			si.hStdOutput = processio.out.win32_handle;
		}
		else
		{
			si.hStdOutput = ::fast_io::win32_stdout().handle;
		}
	}
	else
	{
		si.hStdOutput = nullptr;
	}

	if (!processio.err.is_dev_null)
	{
		if (processio.err.win32_pipe_out_handle)
		{
			si.hStdError = processio.err.win32_pipe_out_handle;

			if (!::fast_io::win32::SetHandleInformation(processio.err.win32_pipe_in_handle, 0x00000001 /*HANDLE_FLAG_INHERIT*/, 0)) [[unlikely]]
			{
				throw_win32_error();
			}
		}
		else if (processio.err.win32_handle)
		{
			si.hStdError = processio.err.win32_handle;
		}
		else
		{
			si.hStdError = ::fast_io::win32_stderr().handle;
		}
	}
	else
	{
		si.hStdError = nullptr;
	}

	si.dwFlags = 0x00000100;

	::std::uint_least32_t dwCreationFlags{};

	if ((mode & process_mode::new_session) == process_mode::new_session)
	{
		dwCreationFlags |= 0x00000200; // CREATE_NEW_PROCESS_GROUP
	}
	if ((mode & process_mode::alloc_new_console) == process_mode::alloc_new_console)
	{
		dwCreationFlags |= 0x00000010; // CREATE_NEW_CONSOLE
	}

	::fast_io::win32::process_information pi{};
	if (!::fast_io::win32::CreateProcessA(filepath, const_cast<char *>(args), nullptr, nullptr, 1,
										  dwCreationFlags, (void *)envs, nullptr, __builtin_addressof(si), __builtin_addressof(pi)))
	{
		throw_win32_error();
	}

	return {pi.hProcess, pi.hThread};
}

struct win32_9xa_win9x_create_process_at_fs_dirent
{
	using family_char_type = char;
	win32_9xa_dir_handle const *directory_handle{};
	win32_process_args_9xa const *args_p{};
	win32_process_envs_9xa const *envs_p{};
	win32_process_io const *processio_p{};
	process_mode mode{};

	inline win32_user_process_information operator()(family_char_type const *filename, ::std::size_t filename_c_str_len)
	{
		if (directory_handle == nullptr) // absoluated path
		{
			return win32_9xa_win9x_process_create_from_filepath_impl(filename, args_p->get(), envs_p->get(), *processio_p, mode);
		}
		else
		{
			// check filename
			using char8_t_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= char8_t const *;

			auto const beg{reinterpret_cast<char8_t_const_may_alias_ptr>(filename)};
			if (::fast_io::details::is_invalid_dos_filename_with_size(beg, filename_c_str_len)) [[unlikely]]
			{
				throw_win32_error(3221225530);
			}

			// check path handle
			::fast_io::win32::details::check_win32_9xa_dir_is_valid(*directory_handle);
			auto str{::fast_io::win32::details::concat_tlc_win32_9xa_dir_handle_path_str(directory_handle->path, u8"\\", ::fast_io::mnp::os_c_str_with_known_size(reinterpret_cast<char8_t_const_may_alias_ptr>(filename), filename_c_str_len))};

			return win32_9xa_win9x_process_create_from_filepath_impl(reinterpret_cast<char const *>(str.c_str()), args_p->get(), envs_p->get(), *processio_p, mode);
		}
	}
};

template <win32_family family, typename path_type>
inline win32_user_process_information win32_winnt_create_process_overloads(nt_at_entry entry, path_type const &filename,
																		   basic_win32_process_args<family> const &args, basic_win32_process_envs<family> const &envs,
																		   win32_process_io const &processio, process_mode mode)
{
	basic_win32_family_file<family, char> nf(entry, filename, open_mode::in | open_mode::excl);
	return win32_winnt_process_create_from_handle_impl<family>(nf.handle, args.get(), envs.get(), processio, mode);
}


template <typename path_type>
inline win32_user_process_information win32_9xa_win9x_create_process_overloads(win32_9xa_at_entry entry, path_type const &filename,
																			   win32_process_args_9xa const &args, win32_process_envs_9xa const &envs,
																			   win32_process_io const &processio, process_mode mode)
{
	return win32_api_common_9xa(filename,
								win32_9xa_win9x_create_process_at_fs_dirent{
									__builtin_addressof(entry.handle),
									__builtin_addressof(args),
									__builtin_addressof(envs),
									__builtin_addressof(processio),
									mode});
}

template <win32_family family, typename path_type>
inline win32_user_process_information win32_winnt_create_process_overloads(path_type const &filename, basic_win32_process_args<family> const &args,
																		   basic_win32_process_envs<family> const &envs,
																		   win32_process_io const &processio, process_mode mode)
{
	basic_win32_family_file<family, char> nf(filename, open_mode::in | open_mode::excl);
	return win32_winnt_process_create_from_handle_impl<family>(nf.handle, args.get(), envs.get(), processio, mode);
}

template <typename path_type>
inline win32_user_process_information win32_9xa_win9x_create_process_overloads(path_type const &filename, win32_process_args_9xa const &args,
																			   win32_process_envs_9xa const &envs,
																			   win32_process_io const &processio, process_mode mode)
{
	return win32_api_common_9xa(filename,
								win32_9xa_win9x_create_process_at_fs_dirent{
									nullptr,
									__builtin_addressof(args),
									__builtin_addressof(envs),
									__builtin_addressof(processio),
									mode});
}

template <win32_family family>
inline win32_user_process_information win32_winnt_create_process_overloads(::fast_io::nt_fs_dirent const &ent, basic_win32_process_args<family> const &args,
																		   basic_win32_process_envs<family> const &envs,
																		   win32_process_io const &processio, process_mode mode)
{
	basic_win32_family_file<family, char> nf(ent, open_mode::in | open_mode::excl);
	return win32_winnt_process_create_from_handle_impl<family>(nf.handle, args.get(), envs.get(), processio, mode);
}

inline win32_user_process_information win32_9xa_win9x_create_process_overloads(::fast_io::win32_9xa_fs_dirent const &ent, win32_process_args_9xa const &args,
																			   win32_process_envs_9xa const &envs, win32_process_io const &processio, process_mode mode)
{
	return win32_api_common_9xa(ent.filename,
								win32_9xa_win9x_create_process_at_fs_dirent{
									__builtin_addressof(ent.handle),
									__builtin_addressof(args),
									__builtin_addressof(envs),
									__builtin_addressof(processio),
									mode});
}
} // namespace win32::details

template <win32_family family>
class win32_family_process_observer
{
public:
	using native_handle_type = win32_user_process_information;
	native_handle_type hnt_user_process_info{};
	inline constexpr auto &native_handle() noexcept
	{
		return hnt_user_process_info;
	}
	inline constexpr auto &native_handle() const noexcept
	{
		return hnt_user_process_info;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return reinterpret_cast<::std::size_t>(hnt_user_process_info.hprocess) |
			   reinterpret_cast<::std::size_t>(hnt_user_process_info.hthread);
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{hnt_user_process_info};
		hnt_user_process_info = {};
		return temp;
	}
};

template <win32_family family>
inline void detach(win32_family_process_observer<family> &ppob) noexcept
{
	win32::details::close_win32_user_process_information(ppob.hnt_user_process_info);
	ppob.hnt_user_process_info = {};
}

struct win32_wait_status
{
	::std::uint_least32_t wait_loc{}; // exit code
};

inline constexpr int wait_status_to_int(win32_wait_status waits) noexcept
{
	return static_cast<int>(waits.wait_loc);
}

template <win32_family family, bool throw_eh = true>
inline win32_wait_status wait(win32_family_process_observer<family> ppob) noexcept(!throw_eh)
{
	if (ppob.hnt_user_process_info.hprocess == nullptr) [[unlikely]]
	{
		if constexpr (throw_eh)
		{
			throw_win32_error(0xC0000008);
		}
		else
		{
			return {static_cast<::std::uint_least32_t>(-1)};
		}
	}

	// wait for process
	auto const status{win32::details::win32_wait_user_process_or_thread(ppob.hnt_user_process_info.hprocess)};
	if (status) [[unlikely]]
	{
		if constexpr (throw_eh)
		{
			throw_win32_error(status);
		}
		else
		{
			return {static_cast<::std::uint_least32_t>(-1)};
		}
	}

	// get exit code
	::std::uint_least32_t exit_code{};
	if (!::fast_io::win32::GetExitCodeProcess(ppob.hnt_user_process_info.hprocess, __builtin_addressof(exit_code)))
	{
		throw_win32_error();
	}


	return {exit_code};
}

template <win32_family family>
inline void kill(win32_family_process_observer<family> ppob, win32_wait_status exit_code)
{
	if (!::fast_io::win32::TerminateProcess(ppob.hnt_user_process_info.hprocess, exit_code.wait_loc)) [[unlikely]]
	{
		throw_win32_error();
	}
}

struct win32_process_id
{
	::std::uint_least32_t process_id{};
};

template <win32_family family>
inline win32_process_id get_process_id(win32_family_process_observer<family> ppob) noexcept
{
	auto pid{::fast_io::win32::GetProcessId(ppob.native_handle)};
	if (pid == 0) [[unlikely]]
	{
		throw_win32_error();
	}
	return {pid};
}

template <win32_family family>
class win32_family_process FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE : public win32_family_process_observer<family>
{
public:
	using native_handle_type = win32_user_process_information;
	inline constexpr win32_family_process() noexcept = default;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr win32_family_process(native_hd hd) noexcept
		: win32_family_process_observer<family>{hd}
	{
	}

	template <::fast_io::constructible_to_os_c_str path_type>
	inline explicit win32_family_process(nt_at_entry nate, path_type const &filename, basic_win32_process_args<family> const &args = {},
										 basic_win32_process_envs<family> const &envs = {}, win32_process_io const &processio = {}, process_mode mode = {})
		: win32_family_process_observer<family>{
			  win32::details::win32_winnt_create_process_overloads<family>(nate, filename, args, envs, processio, mode)}
	{
	}

	template <::fast_io::constructible_to_os_c_str path_type>
	inline explicit win32_family_process(win32_9xa_at_entry nate, path_type const &filename, basic_win32_process_args<family> const &args = {},
										 basic_win32_process_envs<family> const &envs = {}, win32_process_io const &processio = {}, process_mode mode = {})
		: win32_family_process_observer<family>{
			  win32::details::win32_9xa_win9x_create_process_overloads(nate, filename, args, envs, processio, mode)}
	{
	}

	template <::fast_io::constructible_to_os_c_str path_type>
	inline explicit win32_family_process(path_type const &filename, basic_win32_process_args<family> const &args = {}, basic_win32_process_envs<family> const &envs = {},
										 win32_process_io const &processio = {}, process_mode mode = {})
		: win32_family_process_observer<family>{
#if defined(_WIN32_WINDOWS)
			  win32::details::win32_9xa_win9x_create_process_overloads(filename, args, envs, processio, mode)
#else
			  win32::details::win32_winnt_create_process_overloads<family>(filename, args, envs, processio, mode)
#endif
		  }
	{
	}

	inline explicit win32_family_process(::fast_io::nt_fs_dirent ent, basic_win32_process_args<family> const &args = {}, basic_win32_process_envs<family> const &envs = {},
										 win32_process_io const &processio = {}, process_mode mode = {})
		: win32_family_process_observer<family>{
			  win32::details::win32_winnt_create_process_overloads<family>(ent, args, envs, processio, mode)}
	{
	}

	inline explicit win32_family_process(::fast_io::win32_9xa_fs_dirent ent, basic_win32_process_args<family> const &args = {}, basic_win32_process_envs<family> const &envs = {},
										 win32_process_io const &processio = {}, process_mode mode = {})
		: win32_family_process_observer<family>{
			  win32::details::win32_9xa_win9x_create_process_overloads(ent, args, envs, processio, mode)}
	{
	}

	inline win32_family_process(win32_family_process const &b) = delete;
	inline win32_family_process &operator=(win32_family_process const &b) = delete;
	inline constexpr win32_family_process(win32_family_process &&__restrict b) noexcept
		: win32_family_process_observer<family>{b.release()}
	{
	}
	inline win32_family_process &operator=(win32_family_process &&__restrict b) noexcept
	{
		if (__builtin_addressof(b) == this) [[unlikely]]
		{
			return *this;
		}
		win32::details::close_win32_user_process_information_and_wait(this->hnt_user_process_info);
		this->hnt_user_process_info = b.release();
		return *this;
	}
	inline ~win32_family_process()
	{
		win32::details::close_win32_user_process_information_and_wait(this->hnt_user_process_info);
	}
};

using win32_process_observer_9xa = win32_family_process_observer<win32_family::ansi_9x>;
using win32_process_9xa = win32_family_process<win32_family::ansi_9x>;

using win32_process_observer_ntw = win32_family_process_observer<win32_family::wide_nt>;
using win32_process_ntw = win32_family_process<win32_family::wide_nt>;

using win32_process_observer = win32_family_process_observer<win32_family::native>;
using win32_process = win32_family_process<win32_family::native>;

namespace freestanding
{
template <win32_family fm>
struct is_zero_default_constructible<win32_family_process_observer<fm>>
{
	inline static constexpr bool value = true;
};

template <win32_family fm>
struct is_trivially_copyable_or_relocatable<win32_family_process<fm>>
{
	inline static constexpr bool value = true;
};

template <win32_family fm>
struct is_zero_default_constructible<win32_family_process<fm>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding

} // namespace fast_io
