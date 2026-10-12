#pragma once

namespace fast_io
{

namespace win32::details
{

struct handle_guard
{
	void *h{};
	inline constexpr handle_guard() noexcept = default;
	inline constexpr handle_guard(void *r) noexcept
		: h{r} {};
	handle_guard(handle_guard const &) = delete;
	handle_guard &operator=(handle_guard const &) = delete;
	inline constexpr ~handle_guard()
	{
		if (h) [[likely]]
		{
			::fast_io::win32::CloseHandle(h);
		}
	};
	inline constexpr void clear() noexcept
	{
		if (h) [[likely]]
		{
			::fast_io::win32::CloseHandle(h);
			h = nullptr;
		}
	}
};

struct map_guard
{
	void *map{};
	inline constexpr map_guard() noexcept = default;
	inline constexpr map_guard(void *r) noexcept
		: map{r} {};
	map_guard(map_guard const &) = delete;
	map_guard &operator=(map_guard const &) = delete;
	inline constexpr ~map_guard()
	{
		if (map) [[likely]]
		{
			::fast_io::win32::UnmapViewOfFile(map);
		}
	};
	inline constexpr void clear() noexcept
	{
		if (map) [[likely]]
		{
			::fast_io::win32::UnmapViewOfFile(map);
			map = nullptr;
		}
	}
};

inline file_type file_type_impl(void *handle)
	FAST_IO_HERBCEPTIONS_THROWS
{
	/*
	https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfiletype
	*/
	::std::uint_least32_t ftvalue{::fast_io::win32::GetFileType(handle)};
	switch (ftvalue)
	{
	case 0x0002:
		return file_type::character;
	case 0x0001:
		return file_type::regular;
	case 0x0003:
		return file_type::fifo;
	case 0x8000:
		return file_type::remote;
	case 0x0000:
	{
		auto errcd = ::fast_io::win32::GetLastError();
		if (errcd == 0)
		{
			return file_type::unknown;
		}
		throw_win32_error(errcd);
		[[fallthrough]];
	}
	default:
		return file_type::unknown;
	};
}

inline posix_file_status win32_status_impl(void *__restrict handle)
	FAST_IO_HERBCEPTIONS_THROWS
{
	file_type ft{file_type_impl(handle)};
	if (ft == file_type::fifo || ft == file_type::character)
	{
		return posix_file_status{0,
								 0,
								 static_cast<perms>(436),
								 ft,
								 1,
								 0,
								 0,
								 static_cast<::std::uintmax_t>(reinterpret_cast<::std::size_t>(handle)),
								 0,
								 131072,
								 0,
								 {},
								 {},
								 {},
								 {},
								 0,
								 0};
	}
	by_handle_file_information bhdi;
	if (!::fast_io::win32::GetFileInformationByHandle(handle, __builtin_addressof(bhdi)))
	{
		throw_win32_error();
	}
	::std::uintmax_t file_size{static_cast<::std::uintmax_t>(
		(static_cast<::std::uint_least64_t>(bhdi.nFileSizeHigh) << 32) | bhdi.nFileSizeLow)};
	::std::underlying_type_t<perms> pm{0444};
	if ((bhdi.dwFileAttributes & 0x1) == 0x0)
	{
		pm |= 0222;
	}
	if ((bhdi.dwFileAttributes & 0x400) == 0x400)
	{
		ft = file_type::symlink;
	}
	else if ((bhdi.dwFileAttributes & 0x10) == 0x10)
	{
		ft = file_type::directory;
	}
	return posix_file_status{static_cast<::std::uintmax_t>(bhdi.dwVolumeSerialNumber),
							 static_cast<::std::uintmax_t>(
								 (static_cast<::std::uint_least64_t>(bhdi.nFileIndexHigh) << 32) | bhdi.nFileIndexLow),
							 static_cast<perms>(pm),
							 ft,
							 static_cast<::std::uintmax_t>(bhdi.nNumberOfLinks),
							 0,
							 0,
							 0,
							 file_size,
							 131072,
							 file_size >> 9,
							 to_unix_timestamp(bhdi.ftLastAccessTime),
							 to_unix_timestamp(bhdi.ftLastWriteTime),
							 to_unix_timestamp(bhdi.ftLastWriteTime),
							 to_unix_timestamp(bhdi.ftCreationTime),
							 0,
							 0};
}

inline posix_file_status win32_9xa_dir_file_status_impl(win32_9xa_dir_handle const &handle)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::posix_file_status tmp_file{};

	// find data
	::fast_io::win32::win32_find_dataa wfda{};
	win32_9xa_dir_handle_path_str temp_find_path{concat_win32_9xa_dir_handle_path_str(handle.path, u8"\\*")};
	auto find_struct{::fast_io::win32::FindFirstFileA(reinterpret_cast<char const *>(temp_find_path.c_str()), __builtin_addressof(wfda))};
	if (find_struct == reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))) [[unlikely]]
	{
		throw_win32_error(0x5);
	}
	else
	{
		find_struct_guard guard{find_struct};
		// The first piece of information obtained by findfirstfile is current path ('.')

		::std::underlying_type_t<perms> pm{0444};
		if ((wfda.dwFileAttributes & 0x1) == 0x0)
		{
			pm |= 0222;
		}

		tmp_file.perm = static_cast<perms>(pm);
		tmp_file.type = ::fast_io::file_type::directory;
		tmp_file.nlink = 1u;
		tmp_file.size = static_cast<::std::uintmax_t>((static_cast<::std::uint_least64_t>(wfda.nFileSizeHigh) << 32u) | wfda.nFileSizeLow); // always zero
		tmp_file.blksize = 512u;                                                                                                            // default
		tmp_file.blocks = (tmp_file.size + 511u) / 512u;                                                                                    // default
		tmp_file.atim = to_unix_timestamp(wfda.ftLastAccessTime);
		tmp_file.mtim = to_unix_timestamp(wfda.ftLastWriteTime);
		tmp_file.ctim = tmp_file.mtim;
		tmp_file.btim = to_unix_timestamp(wfda.ftCreationTime);

		// find_struct destructor will close the handle
	}

	// dev
	using char_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char const *;

	// Use A APIs and a char buffer; avoid multiplying by sizeof(char8_t)
	constexpr ::std::size_t tmp_path_char_size{260u};
	char tmp_path_char[tmp_path_char_size];

	auto const full_len{::fast_io::win32::GetFullPathNameA(reinterpret_cast<char_const_may_alias_ptr>(handle.path.c_str()), tmp_path_char_size, tmp_path_char, nullptr)};

	if (full_len > 2u && full_len <= tmp_path_char_size && ::fast_io::char_category::is_c_alpha(tmp_path_char[0])) [[likely]]
	{
		// Build root like "C:\\" safely
		// The Character size of the tmp_path_char_size can accommodate the size
		tmp_path_char[3u] = static_cast<char>(u8'\0');

		// get dev
		::std::uint_least32_t serial{};
		if (::fast_io::win32::GetVolumeInformationA(tmp_path_char, nullptr, 0u, __builtin_addressof(serial), nullptr, nullptr, nullptr, 0u)) [[likely]]
		{
			tmp_file.dev = static_cast<::std::uintmax_t>(serial);
		}

		// get blocksize
		::std::uint_least32_t sector_per_cluster{};
		::std::uint_least32_t bytes_per_sector{};
		if (::fast_io::win32::GetDiskFreeSpaceA(tmp_path_char, __builtin_addressof(sector_per_cluster), __builtin_addressof(bytes_per_sector), nullptr, nullptr)) [[likely]]
		{
			tmp_file.blksize = static_cast<::std::uintmax_t>(bytes_per_sector) * static_cast<::std::uintmax_t>(sector_per_cluster);
			tmp_file.blocks = (tmp_file.blksize + 511u) / 512u;
		}
	}

	return tmp_file;
}

} // namespace win32::details

template <win32_family family, ::std::integral ch_type>
inline posix_file_status status(basic_win32_family_io_observer<family, ch_type> wiob)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32::details::win32_status_impl(wiob.handle);
}

inline posix_file_status status(win32_9xa_dir_io_observer w9xiob)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32::details::win32_9xa_dir_file_status_impl(w9xiob.handle);
}

} // namespace fast_io
