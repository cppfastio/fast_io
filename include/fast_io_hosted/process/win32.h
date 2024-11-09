#pragma once

namespace fast_io
{

struct win32_user_process_information
{
	void *hprocess{};
	void *hthread{};
};

template <::fast_io::win32_family family>
struct win32_process_args
{
	inline static constexpr bool is_nt{family == ::fast_io::win32_family::wide_nt};
	using char_type = ::std::conditional_t<is_nt, char16_t, char>;
	using replace_char_type = ::std::conditional_t<is_nt, char16_t, char8_t>;
	using char_type_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type *;
	using char_type_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type const *;

	char_type const *args{};

	constexpr win32_process_args() noexcept = default;
	constexpr win32_process_args(char_type const *a) noexcept
		: args{a} {};
};

namespace win32::details
{

inline void close_win32_user_process_information_not_null(win32_user_process_information hnt_user_process_info) noexcept
{
	if (hnt_user_process_info.hthread) [[likely]]
	{
		::fast_io::win32::CloseHandle(hnt_user_process_info.hthread);
		hnt_user_process_info.hthread = nullptr;
	}
	if (hnt_user_process_info.hprocess) [[likely]]
	{
		::fast_io::win32::CloseHandle(hnt_user_process_info.hprocess);
		hnt_user_process_info.hprocess = nullptr;
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
	};
}

template <win32_family family>
using win32_process_char_type = ::std::conditional_t<family == win32_family::wide_nt, char16_t, char>;

struct map_guard
{
	void *map{};
	constexpr map_guard() noexcept = default;
	constexpr map_guard(void *r) noexcept
		: map{r} {};
	constexpr ~map_guard()
	{
		if (map) [[likely]]
		{
			::fast_io::win32::UnmapViewOfFile(map);
			map = nullptr;
		}
	};
	constexpr void clear() noexcept
	{
		if (map) [[likely]]
		{
			::fast_io::win32::UnmapViewOfFile(map);
			map = nullptr;
		}
	}
};

struct handle_guard
{
	void *h{};
	constexpr handle_guard() noexcept = default;
	constexpr handle_guard(void *r) noexcept
		: h{r} {};
	constexpr ~handle_guard()
	{
		if (h) [[likely]]
		{
			::fast_io::win32::CloseHandle(h);
			h = nullptr;
		}
	};
	constexpr void clear() noexcept
	{
		if (h) [[likely]]
		{
			::fast_io::win32::CloseHandle(h);
			h = nullptr;
		}
	}
};

template <win32_family family>
inline win32_user_process_information win32_process_create_impl(void *__restrict fhandle, win32_process_char_type<family> const *args,
																win32_process_char_type<family> const *envs,
																win32_process_io const &__restrict processio)
{
	constexpr bool is_nt{family == win32_family::wide_nt};
	if constexpr (is_nt)
	{
		char16_t pszFilename[261];
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
			::fast_io::win32::K32GetMappedFileNameW(
#else
			::fast_io::win32::GetMappedFileNameW(
#endif
				reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1)),
				pMem,
				pszFilename,
				260)) [[unlikely]]
		{
			throw_win32_error();
		}

		mg1.clear();
		hg1.clear();

		auto address_begin{pszFilename};

		// change nt path to dos path
		if (pszFilename[0] == u'\\')
		{
			char16_t DosDevice[4]{0, u':', 0, 0};
			char16_t NtPath[64];
			char16_t *RetStr{};
			::std::size_t NtPathLen{};
			for (char16_t i{65}; i < static_cast<char16_t>(26 + 65); i++)
			{
				DosDevice[0] = i;
				if (::fast_io::win32::QueryDosDeviceW(DosDevice, NtPath, 64))
				{
					NtPathLen = ::fast_io::cstr_len(NtPath);

					if (::fast_io::freestanding::my_memcmp(pszFilename, NtPath, NtPathLen * sizeof(char16_t)) == 0) [[unlikely]]
					{
						goto next;
					}
				}
			}
			throw_win32_error(0x3);
		next:
			address_begin += NtPathLen - 2;
			address_begin[0] = DosDevice[0];
			address_begin[1] = u':';
		}

		// create process
		::fast_io::win32::startupinfow si{sizeof(si)};

		si.hStdInput = processio.in.win32_handle ? processio.in.win32_handle : ::fast_io::win32_stdin().handle;
		si.hStdOutput = processio.out.win32_handle ? processio.out.win32_handle : ::fast_io::win32_stdout().handle;
		si.hStdError = processio.err.win32_handle ? processio.err.win32_handle : ::fast_io::win32_stderr().handle;
		si.dwFlags = 0x00000100;

		::fast_io::win32::process_information pi{};
		if (!::fast_io::win32::CreateProcessW(address_begin, const_cast<char16_t *>(args), nullptr, nullptr, 1, 0, (void *)envs, nullptr, __builtin_addressof(si), __builtin_addressof(pi)))
		{
			throw_win32_error();
		}

		return {pi.hProcess, pi.hThread};
	}
	else
	{
		char pszFilename[261];
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
			::fast_io::win32::K32GetMappedFileNameA(
#else
			::fast_io::win32::GetMappedFileNameA(
#endif
				reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1)),
				pMem,
				pszFilename,
				260)) [[unlikely]]
		{
			throw_win32_error();
		}
		map_guard mg1{pMem};

		mg1.clear();
		hg1.clear();

		// change nt path to dos path (9x)
		auto address_begin{pszFilename};

		// change nt path to dos path
		auto k32_module{::fast_io::win32::GetModuleHandleA("Kernel32.dll")};
		if (k32_module)
		{
			using QueryDosDeviceA_t = ::std::uint_least32_t (*)(char const *, char *, ::std::uint_least32_t) noexcept;

			auto QueryDosDeviceA_p{reinterpret_cast<QueryDosDeviceA_t>(::fast_io::win32::GetProcAddress(k32_module, "QueryDosDeviceA"))};
			if (QueryDosDeviceA_p)
			{
				if (pszFilename[0] == '\\')
				{
					char DosDevice[4]{0, ':', 0, 0};
					char NtPath[64];
					char *RetStr{};
					::std::size_t NtPathLen{};
					for (char i{65}; i < static_cast<char>(26 + 65); i++)
					{
						DosDevice[0] = i;
						if (QueryDosDeviceA_p(DosDevice, NtPath, 64))
						{
							NtPathLen = ::fast_io::cstr_len(NtPath);

							if (::fast_io::freestanding::my_memcmp(pszFilename, NtPath, NtPathLen * sizeof(char)) == 0) [[unlikely]]
							{
								goto next;
							}
						}
					}
					throw_win32_error(0x3);
				next:
					address_begin += NtPathLen - 2;
					address_begin[0] = DosDevice[0];
					address_begin[1] = ':';
				}
			}
		}
		// create process
		::fast_io::win32::startupinfoa si{sizeof(si)};
		si.hStdInput = processio.in.win32_handle ? processio.in.win32_handle : ::fast_io::win32_stdin().handle;
		si.hStdOutput = processio.out.win32_handle ? processio.out.win32_handle : ::fast_io::win32_stdout().handle;
		si.hStdError = processio.err.win32_handle ? processio.err.win32_handle : ::fast_io::win32_stderr().handle;
		si.dwFlags = 0x00000100;

		::fast_io::win32::process_information pi{};
		if (!::fast_io::win32::CreateProcessA(address_begin, const_cast<char *>(args), nullptr, nullptr, 1, 0, (void *)envs, nullptr, __builtin_addressof(si), __builtin_addressof(pi)))
		{
			throw_win32_error();
		}

		return {pi.hProcess, pi.hThread};
	}
}

template <win32_family family, typename path_type>
inline win32_user_process_information win32_create_process_overloads(nt_at_entry entry, path_type const &filename,
																	 win32_process_args<family> const &args, win32_process_args<family> const &envs,
																	 win32_process_io const &processio)
{
	basic_win32_family_file<family, char> nf(entry, filename, open_mode::in | open_mode::excl);
	return win32_process_create_impl<family>(nf.handle, args.args, envs.args, processio);
}

template <win32_family family, typename path_type>
inline win32_user_process_information win32_create_process_overloads(path_type const &filename, win32_process_args<family> const &args,
																	 win32_process_args<family> const &envs,
																	 win32_process_io const &processio)
{
	basic_win32_family_file<family, char> nf(filename, open_mode::in | open_mode::excl);
	return win32_process_create_impl<family>(nf.handle, args.args, envs.args, processio);
}

template <win32_family family, typename path_type>
inline win32_user_process_information win32_create_process_overloads(::fast_io::nt_fs_dirent ent, win32_process_args<family> const &args,
																	 win32_process_args<family> const &envs,
																	 win32_process_io const &processio)
{
	basic_win32_family_file<family, char> nf(ent, open_mode::in | open_mode::excl);
	return win32_process_create_impl<family>(nf.handle, args.args, envs.args, processio);
}

} // namespace win32::details

template <win32_family family>
class win32_family_process_observer
{
public:
	using native_handle_type = win32_user_process_information;
	native_handle_type hnt_user_process_info{};
	constexpr auto &native_handle() noexcept
	{
		return hnt_user_process_info;
	}
	constexpr auto &native_handle() const noexcept
	{
		return hnt_user_process_info;
	}
	explicit constexpr operator bool() const noexcept
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
		throw_win32_error(status);
	}


	return {exit_code};
}

template <win32_family family>
class win32_family_process : public win32_family_process_observer<family>
{
public:
	using native_handle_type = win32_user_process_information;
	constexpr win32_family_process() noexcept = default;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	explicit constexpr win32_family_process(native_hd hd) noexcept
		: win32_family_process_observer<family>{hd}
	{
	}

	template <::fast_io::constructible_to_os_c_str path_type>
	explicit win32_family_process(nt_at_entry nate, path_type const &filename, win32_process_args<family> const &args,
								  win32_process_args<family> const &envs, win32_process_io const &processio)
		: win32_family_process_observer<family>{
			  win32::details::win32_create_process_overloads<family>(nate, filename, args, envs, processio)}
	{
	}

	template <::fast_io::constructible_to_os_c_str path_type>
	explicit win32_family_process(path_type const &filename, win32_process_args<family> const &args, win32_process_args<family> const &envs,
								  win32_process_io const &processio)
		: win32_family_process_observer<family>{
			  win32::details::win32_create_process_overloads<family>(filename, args, envs, processio)}
	{
	}

	explicit win32_family_process(::fast_io::nt_fs_dirent ent, win32_process_args<family> const &args, win32_process_args<family> const &envs,
								  win32_process_io const &processio)
		: win32_family_process_observer<family>{
			  win32::details::win32_create_process_overloads<family>(ent, args, envs, processio)}
	{
	}

	win32_family_process(win32_family_process const &b) = delete;
	win32_family_process &operator=(win32_family_process const &b) = delete;
	constexpr win32_family_process(win32_family_process &&__restrict b) noexcept
		: win32_family_process_observer<family>{b.release()}
	{
	}
	win32_family_process &operator=(win32_family_process &&__restrict b) noexcept
	{
		win32::details::close_win32_user_process_information_and_wait(this->hnt_user_process_info);
		this->hnt_user_process_info = b.release();
		return *this;
	}
	~win32_family_process()
	{
		win32::details::close_win32_user_process_information_and_wait(this->hnt_user_process_info);
		this->hnt_user_process_info = {};
	}
};

using win32_process_observer_9xa = win32_family_process_observer<win32_family::ansi_9x>;
using win32_process_9xa = win32_family_process<win32_family::ansi_9x>;

using win32_process_observer_ntw = win32_family_process_observer<win32_family::wide_nt>;
using win32_process_ntw = win32_family_process<win32_family::wide_nt>;

using win32_process_observer = win32_family_process_observer<win32_family::native>;
using win32_process = win32_family_process<win32_family::native>;


} // namespace fast_io
