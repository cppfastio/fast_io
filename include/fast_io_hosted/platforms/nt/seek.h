#pragma once

namespace fast_io
{

namespace win32::nt::details
{

struct nt_file_position_status
{
	::std::uint_least32_t status;
	::std::int_least64_t file_position;
};

template <bool zw>
inline nt_file_position_status nt_get_file_position_impl(void *__restrict handle, ::std::int_least64_t offset,
														 seekdir s) noexcept
{
	::std::uint_least64_t file_position{static_cast<::std::uint_least64_t>(offset)};
	::fast_io::win32::nt::io_status_block block;
	switch (s)
	{
	case seekdir::cur:
	{
		::std::uint_least64_t fps{};
		auto status{::fast_io::win32::nt::nt_query_information_file<zw>(
			handle, __builtin_addressof(block), __builtin_addressof(fps),
			static_cast<::std::uint_least32_t>(sizeof(::std::uint_least64_t)),
			::fast_io::win32::nt::file_information_class::FilePositionInformation)};
		if (status)
		{
			return {.status = status, .file_position = 0u};
		}
		file_position += fps;
	}
	break;
	case seekdir::end:
	{
		::fast_io::win32::nt::file_standard_information fsi;
		auto status{::fast_io::win32::nt::nt_query_information_file<zw>(
			handle, __builtin_addressof(block), __builtin_addressof(fsi),
			static_cast<::std::uint_least32_t>(sizeof(::fast_io::win32::nt::file_standard_information)),
			::fast_io::win32::nt::file_information_class::FileStandardInformation)};
		if (status)
		{
			return {.status = status, .file_position = 0u};
		}
		file_position += fsi.end_of_file;
	}
	break;
	default:;
	}
	if (static_cast<::std::uint_least64_t>(::std::numeric_limits<::std::int_least64_t>::max()) < file_position)
	{
		file_position = 0;
	}
	return {0, static_cast<::std::int_least64_t>(file_position)};
}

template <bool zw>
inline ::std::int_least64_t nt_seek64_impl(void *__restrict handle, ::std::int_least64_t offset, seekdir s)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto [status, file_position] = nt_get_file_position_impl<zw>(handle, offset, s);
	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
	::fast_io::win32::nt::io_status_block block;
	status = ::fast_io::win32::nt::nt_set_information_file<zw>(handle, __builtin_addressof(block),
															   __builtin_addressof(file_position), sizeof(::std::uint_least64_t),
															   ::fast_io::win32::nt::file_information_class::FilePositionInformation);
	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
	return file_position;
}

template <bool zw>
inline ::fast_io::intfpos_t nt_seek_impl(void *__restrict handle, ::fast_io::intfpos_t offset, seekdir s)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return static_cast<::fast_io::intfpos_t>(nt_seek64_impl<zw>(handle, static_cast<::std::int_least64_t>(offset), s));
}

template <bool zw>
inline void *nt_dup_impl(void *handle)
	FAST_IO_HERBCEPTIONS_THROWS
{
	void *current_process{reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))};
	void *new_handle{};
	auto status{::fast_io::win32::nt::nt_duplicate_object<zw>(current_process, handle, current_process,
															  __builtin_addressof(new_handle), 0, 0x00000002L, 2)};
	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
	return new_handle;
}

template <bool zw>
inline void *nt_dup2_impl(void *handle, void *newhandle)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto temp{nt_dup_impl<zw>(handle)};
	if (newhandle) [[likely]]
	{
		::fast_io::win32::nt::nt_close<zw>(newhandle);
	}
	return temp;
}

template <bool zw>
inline void nt_truncate_impl(void *handle, ::std::uintmax_t newfilesizem)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::uint_least64_t newfilesize{static_cast<::std::uint_least64_t>(newfilesizem)};
	::fast_io::win32::nt::io_status_block block;
	auto status{::fast_io::win32::nt::nt_set_information_file<zw>(handle, __builtin_addressof(block),
																  __builtin_addressof(newfilesize), sizeof(::std::uint_least64_t),
																  ::fast_io::win32::nt::file_information_class::FileEndOfFileInformation)};
	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
}

} // namespace win32::nt::details

template <nt_family family, ::std::integral ch_type>
inline ::fast_io::intfpos_t io_stream_seek_bytes_define(basic_nt_family_io_observer<family, ch_type> handle,
														::fast_io::intfpos_t offset, seekdir s)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::nt::details::nt_seek_impl<family == nt_family::zw>(handle.handle, offset, s);
}

template <nt_family family, ::std::integral ch_type>
inline void truncate(basic_nt_family_io_observer<family, ch_type> handle, ::std::uintmax_t newfilesize)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::nt::details::nt_truncate_impl<family == nt_family::zw>(handle.handle, newfilesize);
}

} // namespace fast_io
