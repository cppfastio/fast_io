#pragma once

namespace fast_io
{

namespace win32::nt::details
{

inline constexpr posix_statx_timestamp64 to_unix_timestamp(::std::uint_least64_t date_time) noexcept
{
	/*
	116444736000000000
	18446744073709551616
	 999999999
	1000000000
	*/

	constexpr ::std::uint_least64_t gap{11644473600000ULL * 10000ULL};
	::std::uint_least64_t unix_time{date_time - gap};
	if (date_time < gap) [[unlikely]]
	{
		unix_time = 0;
	}
	return {static_cast<::std::int_least64_t>(unix_time / 10000000ULL),
			static_cast<::std::uint_least32_t>(unix_time % 10000000ULL) * 100u};
}

inline constexpr file_type file_type_impl(::std::uint_least32_t DeviceType) noexcept
{
	if (DeviceType > 0x2B)
	{
		if (DeviceType != 80) [[unlikely]]
		{
			return file_type::unknown;
		}
		return file_type::character;
	}
	if (DeviceType == 43 || DeviceType == 21)
	{
		return file_type::character;
	}
	if (DeviceType > 0x15)
	{
		if (DeviceType > 0x1D)
		{
			if (DeviceType != 31 && DeviceType != 32 && DeviceType != 33 && DeviceType != 34 && DeviceType == 36) [[likely]]
			{
				return file_type::regular;
			}
			return file_type::unknown;
		}
		switch (DeviceType)
		{
		case 0x1D:
			[[fallthrough]];
		case 0x16:
			return file_type::character;
		case 0x17:
			[[unlikely]] return file_type::unknown;
		case 0x18:
			[[fallthrough]];
		case 0x1B:
			[[fallthrough]];
		case 0x28:
			return file_type::character;
		default:
			[[unlikely]] return file_type::unknown;
		}
	}
	if (DeviceType > 8)
	{
		if (DeviceType != 11 && DeviceType != 15)
		{
			if (DeviceType == 17) [[likely]]
			{
				return file_type::fifo;
			}
			return file_type::unknown;
		}
		return file_type::character;
	}
	return file_type::regular;
}

template <nt_family family>
inline posix_file_status nt_status_impl(void *__restrict handle)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::nt::io_status_block isb;

	::fast_io::win32::nt::file_fs_device_type ffdt;
	auto status{::fast_io::win32::nt::nt_query_volume_information_file<family == ::fast_io::nt_family::zw>(handle, __builtin_addressof(isb), __builtin_addressof(ffdt),
																										   static_cast<::std::uint_least32_t>(sizeof(ffdt)),
																										   ::fast_io::win32::nt::fs_information_class::FileFsDeviceInformation)};
	if (status) [[unlikely]]
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}

	auto ft{file_type_impl(ffdt.DeviceType)};

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

	::fast_io::win32::nt::file_basic_information fbi;
	status = ::fast_io::win32::nt::nt_query_information_file<family == ::fast_io::nt_family::zw>(handle, __builtin_addressof(isb), __builtin_addressof(fbi),
																								 static_cast<::std::uint_least32_t>(sizeof(fbi)),
																								 ::fast_io::win32::nt::file_information_class::FileBasicInformation);

	if (status) [[unlikely]]
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}

	::fast_io::win32::nt::file_internal_information fii;
	status = ::fast_io::win32::nt::nt_query_information_file<family == ::fast_io::nt_family::zw>(handle, __builtin_addressof(isb), __builtin_addressof(fii),
																								 static_cast<::std::uint_least32_t>(sizeof(fii)),
																								 ::fast_io::win32::nt::file_information_class::FileInternalInformation);

	if (status) [[unlikely]]
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}

	struct
	{
		::fast_io::win32::nt::file_fs_volume_information FileFsVolume;
		char16_t Name[255];
	} ffvi;

	status = ::fast_io::win32::nt::nt_query_volume_information_file<family == ::fast_io::nt_family::zw>(handle, __builtin_addressof(isb),
																										__builtin_addressof(ffvi), static_cast<::std::uint_least32_t>(sizeof(ffvi)),
																										::fast_io::win32::nt::fs_information_class::FileFsVolumeInformation);
	if (status) [[unlikely]]
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}

	::fast_io::win32::nt::file_standard_information fsi;
	status = ::fast_io::win32::nt::nt_query_information_file<family == ::fast_io::nt_family::zw>(handle, __builtin_addressof(isb), __builtin_addressof(fsi),
																								 static_cast<::std::uint_least32_t>(sizeof(fsi)),
																								 ::fast_io::win32::nt::file_information_class::FileStandardInformation);

	if (status) [[unlikely]]
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}

	::std::uintmax_t file_size{static_cast<::std::uintmax_t>(fsi.end_of_file)};
	::std::underlying_type_t<perms> pm{0444};
	if ((fbi.FileAttributes & 0x1) == 0x0)
	{
		pm |= 0222;
	}
	if ((fbi.FileAttributes & 0x400) == 0x400)
	{
		ft = file_type::symlink;
	}
	else if ((fbi.FileAttributes & 0x10) == 0x10)
	{
		ft = file_type::directory;
	}
	return posix_file_status{static_cast<::std::uintmax_t>(ffvi.FileFsVolume.VolumeSerialNumber),
							 static_cast<::std::uintmax_t>(fii.IndexNumber),
							 static_cast<perms>(pm),
							 ft,
							 static_cast<::std::uintmax_t>(fsi.number_of_links),
							 0,
							 0,
							 0,
							 file_size,
							 131072,
							 file_size >> 9,
							 to_unix_timestamp(fbi.LastAccessTime),
							 to_unix_timestamp(fbi.LastWriteTime),
							 to_unix_timestamp(fbi.ChangeTime),
							 to_unix_timestamp(fbi.CreationTime),
							 0,
							 0};
}

} // namespace win32::nt::details

template <nt_family family, ::std::integral ch_type>
inline posix_file_status status(basic_nt_family_io_observer<family, ch_type> wiob) FAST_IO_HERBCEPTIONS_THROWS
{
	return win32::nt::details::nt_status_impl<family>(wiob.handle);
}

} // namespace fast_io
