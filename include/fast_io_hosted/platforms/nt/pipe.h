#pragma once

namespace fast_io
{

namespace win32::nt::details
{

template <bool zw>
inline void nt_create_pipe(void **hReadPipe, void **hWritePipe)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::nt::io_status_block isb;
	constexpr decltype(auto) namedpipe_part{u"\\Device\\NamedPipe\\"};
	::fast_io::win32::nt::unicode_string us{
		.Length = static_cast<::std::uint_least16_t>(sizeof(namedpipe_part) - sizeof(char16_t)),
		.MaximumLength = ::fast_io::win32::nt::details::nt_filename_bytes_check(sizeof(namedpipe_part)),
		.Buffer = const_cast<char16_t *>(namedpipe_part)};

	::fast_io::win32::nt::object_attributes obj{.Length = sizeof(::fast_io::win32::nt::object_attributes),
												.RootDirectory = nullptr,
												.ObjectName = __builtin_addressof(us),
												.Attributes = 0,
												.SecurityDescriptor = nullptr,
												.SecurityQualityOfService = nullptr};

	void *namedpipedir;
	auto status = ::fast_io::win32::nt::nt_create_file<zw>(
		__builtin_addressof(namedpipedir), 0x80100000, __builtin_addressof(obj),
		__builtin_addressof(isb), nullptr, 0, 3, 0x00000001, 0x00000020, nullptr, 0u);

	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}

	::fast_io::basic_nt_family_file<zw ? nt_family::zw : nt_family::nt, char> file{namedpipedir};

	::std::int_least64_t DefaultTimeout{-1200000000};

	void *ReadPipeHandle;
	::fast_io::win32::nt::unicode_string us2{};
	::fast_io::win32::nt::object_attributes obj2{.Length = sizeof(::fast_io::win32::nt::object_attributes),
												 .RootDirectory = file.native_handle(),
												 .ObjectName = __builtin_addressof(us2),
												 .Attributes = 0x42 /* InheritHandle */,
												 .SecurityDescriptor = nullptr,
												 .SecurityQualityOfService = nullptr};

	status = ::fast_io::win32::nt::nt_create_named_pipe_file<zw>(__builtin_addressof(ReadPipeHandle),
																 0x80000000L | 0x0100 | 0x00100000L /*GENERIC_READ | FILE_WRITE_ATTRIBUTES | SYNCHRONIZE*/,
																 __builtin_addressof(obj2),
																 __builtin_addressof(isb),
																 0x01 | 0x02 /*FILE_SHARE_READ | FILE_SHARE_DELETE*/,
																 0x00000002 /*FILE_CREATE*/,
																 0x00000020 /*FILE_SYNCHRONOUS_IO_NONALERT*/,
																 0x00000000 /*FILE_PIPE_BYTE_STREAM_TYPE*/,
																 0x00000000 /*FILE_PIPE_BYTE_STREAM_MODE*/,
																 0x00000000 /*FILE_PIPE_QUEUE_OPERATION*/,
																 1,
																 0x4000 /*buffer size*/,
																 0x4000 /*buffer size*/,
																 __builtin_addressof(DefaultTimeout));

	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}

	obj2.RootDirectory = ReadPipeHandle;

	void *WritePipeHandle;
	status = ::fast_io::win32::nt::nt_create_file<zw>(
		__builtin_addressof(WritePipeHandle), 0x40100080, __builtin_addressof(obj2), __builtin_addressof(isb), nullptr,
		0, 3, 0x00000001, 0x00000020 | 0x00000040, nullptr, 0u);

	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}

	*hReadPipe = ReadPipeHandle;
	*hWritePipe = WritePipeHandle;
}
} // namespace win32::nt::details

template <nt_family family, ::std::integral ch_type>
class basic_nt_family_pipe
{
public:
	using char_type = ch_type;
	basic_nt_family_file<family, ch_type> pipes[2];
	inline basic_nt_family_pipe()
		FAST_IO_HERBCEPTIONS_THROWS
	{
		win32::nt::details::nt_create_pipe<family == nt_family::zw>(__builtin_addressof(pipes[0].handle), __builtin_addressof(pipes[1].handle));
	}
	inline constexpr auto &in() noexcept
	{
		return *pipes;
	}
	inline constexpr auto &out() noexcept
	{
		return pipes[1];
	}
};

template <nt_family family, ::std::integral ch_type>
inline constexpr win32_io_redirection redirect(basic_nt_family_pipe<family, ch_type> &hd) noexcept
{
	return {.win32_pipe_in_handle = hd.in().handle, .win32_pipe_out_handle = hd.out().handle};
}

template <nt_family family, ::std::integral ch_type>
inline constexpr basic_nt_family_io_observer<family, ch_type>
input_stream_ref_define(basic_nt_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.in().handle};
}

template <nt_family family, ::std::integral ch_type>
inline constexpr basic_nt_family_io_observer<family, char>
input_bytes_stream_ref_define(basic_nt_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.in().handle};
}

template <nt_family family, ::std::integral ch_type>
inline constexpr basic_nt_family_io_observer<family, ch_type>
output_stream_ref_define(basic_nt_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.out().handle};
}

template <nt_family family, ::std::integral ch_type>
inline constexpr basic_nt_family_io_observer<family, char>
output_bytes_stream_ref_define(basic_nt_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.out().handle};
}

} // namespace fast_io
