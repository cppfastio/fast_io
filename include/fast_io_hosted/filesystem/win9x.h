#pragma once

namespace fast_io
{

struct win9x_DIR
{
	void *d_handle{};
	file_type d_type{};
	::std::uint_least64_t d_ino{};
	::fast_io::string path{};
};


namespace win32::details
{
inline ::fast_io::string get_file_full_path_from_path_handle(void *fhandle)
{
	char pszFilename[261];

	void *hFileMap{::fast_io::win32::CreateFileMappingA(fhandle, nullptr, 0x10000000 | 0x02, 0, 1, nullptr)};
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

		// win95 may not have this function, need check (?)
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
							goto next2;
						}
					}
				}
				throw_win32_error(0x3);
			next2:
				address_begin += NtPathLen - 2;
				address_begin[0] = DosDevice[0];
				address_begin[1] = ':';
			}
		}
	}
}

} // namespace win32::details

inline win9x_DIR *set_win9x_dirent(win9x_DIR *entry, bool start)
{
}

} // namespace fast_io
