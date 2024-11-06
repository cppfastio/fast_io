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

inline void close_nt_user_process_information(win32_user_process_information hnt_user_process_info) noexcept
{
	return details::close_win32_user_process_information_not_null(hnt_user_process_info);
}

inline ::std::uint_least32_t win32_wait_user_process_or_thread(void *hprocess_thread) noexcept
{
	return ::fast_io::win32::WaitForSingleObject(hprocess_thread, 0xFFFFFFFF);
}
#if 0
template <nt_family family, bool throw_eh = false>
inline void nt_wait_and_close_user_process_or_thread(void *&handle) noexcept(!throw_eh)
{
	if (handle == nullptr) [[unlikely]]
	{
		return;
	}
	auto status{nt_wait_user_process_or_thread<family>(handle)};
	auto status2{::fast_io::win32::nt::nt_close<family == nt_family::zw>(handle)};
	handle = nullptr;
	if constexpr (throw_eh)
	{
		if (status) [[unlikely]]
		{
			throw_nt_error(status);
		}
		if (status2) [[unlikely]]
		{
			throw_nt_error(status2);
		}
	}
}

template <nt_family family, bool throw_eh = false>
inline void
close_nt_user_process_information_and_wait(nt_user_process_information hnt_user_process_info) noexcept(!throw_eh)
{
	nt_wait_and_close_user_process_or_thread<family, throw_eh>(hnt_user_process_info.hthread);
	nt_wait_and_close_user_process_or_thread<family, throw_eh>(hnt_user_process_info.hprocess);
}

template <nt_family family>
inline void nt_duplicate_object_std(void *parent_process, void *&standard_io_handle, void *process_handle)
{
	if (standard_io_handle == nullptr)
	{
		return;
	}
	check_nt_status(::fast_io::win32::nt::nt_duplicate_object<family>(
		parent_process, standard_io_handle, process_handle, __builtin_addressof(standard_io_handle), 0, 0,
		0x00000002 | 0x00000004));
}

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline void check_nt_status(::std::uint_least32_t status)
{
	if (status) [[unlikely]]
	{
		throw_nt_error(status);
	}
}

template <bool zw>
inline void *nt_duplicate_process_std_handle_impl(void *__restrict hprocess, win32_io_redirection const &redi)
{
	void *const current_process{reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))};
	void *ptr{};
	if (redi.win32_handle) [[likely]]
	{
		check_nt_status(::fast_io::win32::nt::nt_duplicate_object<zw>(
			current_process, redi.win32_handle, hprocess, __builtin_addressof(ptr), 0, 0,
			/*DUPLICATE_SAME_ACCESS|DUPLICATE_SAME_ATTRIBUTES*/ 0x00000004 | 0x00000002));
	}
	return ptr;
}

struct rtl_guard
{
	rtl_user_process_parameters *rtl_up{};
	constexpr rtl_guard() noexcept = default;
	constexpr rtl_guard(rtl_user_process_parameters *r) noexcept
		: rtl_up{r} {};
	constexpr ~rtl_guard()
	{
		if (rtl_up) [[likely]]
		{
			::fast_io::win32::nt::RtlDestroyProcessParameters(rtl_up);
		}
	};
};

struct unicode_string_guard
{
	unicode_string *us{};
	constexpr unicode_string_guard() noexcept = default;
	constexpr unicode_string_guard(unicode_string *u) noexcept
		: us{u} {};
	constexpr ~unicode_string_guard()
	{
		if (us) [[likely]]
		{
			::fast_io::native_thread_local_allocator::deallocate(us);
		}
	};
};

template <bool zw>
inline void nt_3x_push_process_parameters_and_duplicate_process_std_handles(void *__restrict hprocess,
																			void *__restrict fhandle, char16_t const *args,
																			char16_t const *envs,
																			win32_process_io const &__restrict processio,
																			void *__restrict remote_peb)
{
	// Create the NtImagePath
	::std::uint_least32_t NtImagePath_len{};
	::fast_io::win32::nt::nt_query_object<zw>(fhandle, object_information_class::ObjectNameInformation, nullptr, 0,
											  __builtin_addressof(NtImagePath_len));
	auto NtImagePath{
		reinterpret_cast<unicode_string *>(::fast_io::native_thread_local_allocator::allocate(NtImagePath_len))};
	unicode_string_guard us_man{NtImagePath};
	check_nt_status(::fast_io::win32::nt::nt_query_object<zw>(fhandle, object_information_class::ObjectNameInformation,
															  NtImagePath, NtImagePath_len,
															  __builtin_addressof(NtImagePath_len)));

	unicode_string ps_para{};
	if (args)
	{
		ps_para.Buffer = const_cast<char16_t *>(args);
		ps_para.Length = static_cast<decltype(ps_para.Length)>(::fast_io::cstr_len(args) * sizeof(char16_t));
		ps_para.MaximumLength = static_cast<decltype(ps_para.MaximumLength)>(ps_para.Length + sizeof(char16_t));
	}
	rtl_user_process_parameters *rtl_up{};
	check_nt_status(::fast_io::win32::nt::RtlCreateProcessParameters(
		__builtin_addressof(rtl_up), NtImagePath, nullptr, nullptr, args ? __builtin_addressof(ps_para) : nullptr,
		(void *)(envs), nullptr, nullptr, nullptr, nullptr));
	rtl_guard rtlm{rtl_up};

	/* Duplicate Process Std Handles */
	rtl_up->StandardInput = nt_duplicate_process_std_handle_impl<zw>(hprocess, processio.in);
	rtl_up->StandardOutput = nt_duplicate_process_std_handle_impl<zw>(hprocess, processio.out);
	rtl_up->StandardError = nt_duplicate_process_std_handle_impl<zw>(hprocess, processio.err);

	/* Allocate and Initialize new Environment Block */
	rtl_user_process_parameters *RemoteParameters{};
	::std::size_t Size{rtl_up->Length};
	check_nt_status(::fast_io::win32::nt::nt_allocate_virtual_memory<zw>(
		hprocess, reinterpret_cast<void **>(__builtin_addressof(RemoteParameters)), 0, __builtin_addressof(Size),
		0x1000 /*MEM_COMMIT*/, 0x04 /*PAGE_READWRITE*/));

	/* Write the Parameter Block */
	check_nt_status(
		::fast_io::win32::nt::nt_write_virtual_memory<zw>(hprocess, RemoteParameters, rtl_up, rtl_up->Length, nullptr));

	/* Write the PEB Pointer */
	check_nt_status(::fast_io::win32::nt::nt_write_virtual_memory<zw>(
		hprocess, __builtin_addressof(reinterpret_cast<peb *>(remote_peb)->ProcessParameters),
		__builtin_addressof(RemoteParameters), sizeof(void *), nullptr));
}

template <nt_family family>
inline nt_user_process_information nt_3x_process_create_impl(void *__restrict fhandle, char16_t const *args,
															 char16_t const *envs,
															 win32_process_io const &__restrict processio)
{
	/****************************************************************************************************
	 * A large number of security related checks are conducted in kernel32, and this API is not secure. *
	 ****************************************************************************************************/

	constexpr bool zw{family == nt_family::zw};

	// Section
	void *hsection{};
	check_nt_status(::fast_io::win32::nt::nt_create_section<zw>(
		__builtin_addressof(hsection), 0xf001f /*SECTION_ALL_ACCESS*/, nullptr, nullptr, 0x02 /*PAGE_READ_ONLY*/,
		0x1000000 /*SEC_IMAGE*/, fhandle));
	basic_nt_family_file<family, char> section{hsection};
	void *const current_process{reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))};

	// Process
	void *hprocess{};
	check_nt_status(::fast_io::win32::nt::nt_create_process<zw>(
		__builtin_addressof(hprocess), 0x000F0000U | 0x00100000U | 0xFFF /*PROCESS_ALL_ACCESS*/, nullptr,
		current_process, true, hsection, nullptr, nullptr));
	basic_nt_family_file<family, char> process(hprocess);

	// PROCESS_BASIC_INFO
	process_basic_information pb_info{};
	check_nt_status(::fast_io::win32::nt::nt_query_information_process<zw>(
		hprocess, process_information_class::ProcessBasicInformation, __builtin_addressof(pb_info), sizeof(pb_info),
		nullptr));

	// Push Process Parameters and Duplicate Process Std Handles
	nt_3x_push_process_parameters_and_duplicate_process_std_handles<zw>(hprocess, fhandle, args, envs, processio,
																		pb_info.PebBaseAddress);

	// Thread
	void *hthread{};
	client_id cid{};
	section_image_information sec_info{};
	check_nt_status(
		::fast_io::win32::nt::nt_query_section<zw>(hsection, section_information_class::SectionImageInformation,
												   __builtin_addressof(sec_info), sizeof(sec_info), nullptr));

	check_nt_status(::fast_io::win32::nt::RtlCreateUserThread(
		hprocess, nullptr, true, sec_info.ZeroBits, sec_info.MaximumStackSize, sec_info.CommittedStackSize,
		sec_info.TransferAddress, pb_info.PebBaseAddress, __builtin_addressof(hthread), __builtin_addressof(cid)));
	basic_nt_family_file<family, char> thread(hthread);

	// Call Windows Server
	// to do

	// Resume Thread
	::std::uint_least32_t lprevcount{};
	check_nt_status(::fast_io::win32::nt::nt_resume_thread<zw>(hthread, __builtin_addressof(lprevcount)));
	return {process.release(), thread.release()};
}

template <nt_family family>
inline nt_user_process_information nt_6x_process_create_impl(void *__restrict fhandle, char16_t const *args,
															 char16_t const *envs,
															 win32_process_io const &__restrict processio)
{
	constexpr bool zw{family == nt_family::zw};

	// Create the NtImagePath
	::std::uint_least32_t NtImagePath_len{};
	::fast_io::win32::nt::nt_query_object<zw>(fhandle, object_information_class::ObjectNameInformation, nullptr, 0,
											  __builtin_addressof(NtImagePath_len));
	auto NtImagePath{
		reinterpret_cast<unicode_string *>(::fast_io::native_thread_local_allocator::allocate(NtImagePath_len))};
	unicode_string_guard us_man{NtImagePath};
	check_nt_status(::fast_io::win32::nt::nt_query_object<zw>(fhandle, object_information_class::ObjectNameInformation,
															  NtImagePath, NtImagePath_len,
															  __builtin_addressof(NtImagePath_len)));

	// Create the process parameters
	unicode_string ps_para{};
	if (args)
	{
		ps_para.Buffer = const_cast<char16_t *>(args);
		ps_para.Length = static_cast<decltype(ps_para.Length)>(::fast_io::cstr_len(args) * sizeof(char16_t));
		ps_para.MaximumLength = static_cast<decltype(ps_para.MaximumLength)>(ps_para.Length + sizeof(char16_t));
	}
	rtl_user_process_parameters *rtl_temp{};
	check_nt_status(::fast_io::win32::nt::RtlCreateProcessParametersEx(
		__builtin_addressof(rtl_temp), NtImagePath, nullptr, nullptr, args ? __builtin_addressof(ps_para) : nullptr,
		(void *)(envs), nullptr, nullptr, nullptr, nullptr, 0x01));
	rtl_guard rtlm{rtl_temp};

	// Duplicate Process Std Handles
	rtl_temp->StandardInput = processio.in.win32_handle;
	rtl_temp->StandardOutput = processio.out.win32_handle;
	rtl_temp->StandardError = processio.err.win32_handle;

	// Initialize the PS_CREATE_INFO structure
	ps_create_info CreateInfo{};
	CreateInfo.Size = sizeof(CreateInfo);
	CreateInfo.State = ps_create_state::PsCreateInitialState;

	// Initialize the PS_ATTRIBUTE_LIST structure
	ps_attribute_list AttributeList{};
	AttributeList.TotalLength = sizeof(ps_attribute_list) - sizeof(ps_attribute);
	AttributeList.Attributes[0].Attribute = 131077;
	AttributeList.Attributes[0].Size = NtImagePath->Length;
	AttributeList.Attributes[0].Value = reinterpret_cast<::std::size_t>(NtImagePath->Buffer);

	// Create the process
	void *hProcess{};
	void *hThread{};
	check_nt_status(::fast_io::win32::nt::nt_create_user_process<zw>(
		&hProcess, &hThread, 0x000F0000U | 0x00100000U | 0xFFFF, 0x000F0000U | 0x00100000U | 0xFFFF, nullptr, nullptr,
		0x00, 0x00, rtl_temp, &CreateInfo, &AttributeList));

	return {hProcess, hThread};
}

template <nt_family family>
inline nt_user_process_information nt_process_create_impl(void *__restrict fhandle, char16_t const *args,
														  char16_t const *envs,
														  win32_process_io const &__restrict processio)
{
#if !defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x600
	return nt_6x_process_create_impl<family>(fhandle, args, envs, processio);
#else
	return nt_3x_process_create_impl<family>(fhandle, args, envs, processio);
#endif
}

template <nt_family family, typename path_type>
inline nt_user_process_information nt_create_process_overloads(nt_at_entry entry, path_type const &filename,
															   nt_process_args const &args, nt_process_args const &envs,
															   win32_process_io const &processio)
{
	basic_nt_family_file<family, char> nf(entry, filename, open_mode::in | open_mode::excl);
	return nt_process_create_impl<family>(nf.handle, args.args, envs.args, processio);
}

template <nt_family family, typename path_type>
inline nt_user_process_information nt_create_process_overloads(path_type const &filename, nt_process_args const &args,
															   nt_process_args const &envs,
															   win32_process_io const &processio)
{
	basic_nt_family_file<family, char> nf(filename, open_mode::in | open_mode::excl);
	return nt_process_create_impl<family>(nf.handle, args.args, envs.args, processio);
}

template <nt_family family, typename path_type>
inline nt_user_process_information nt_create_process_overloads(::fast_io::nt_fs_dirent ent, nt_process_args const &args,
															   nt_process_args const &envs,
															   win32_process_io const &processio)
{
	basic_nt_family_file<family, char> nf(ent, open_mode::in | open_mode::excl);
	return nt_process_create_impl<family>(nf.handle, args.args, envs.args, processio);
}
#endif
} // namespace win32::details


struct win32_process
{
};

} // namespace fast_io
