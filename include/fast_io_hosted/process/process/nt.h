﻿#pragma once

namespace fast_io
{

using nt_user_process_information = win32_user_process_information;

namespace win32::nt::details
{
template <nt_family family>
inline void close_nt_user_process_information_not_null(nt_user_process_information hnt_user_process_info) noexcept
{
	if (hnt_user_process_info.hthread) [[likely]]
	{
		::fast_io::win32::nt::nt_close<family == nt_family::zw>(hnt_user_process_info.hthread);
		hnt_user_process_info.hthread = nullptr;
	}
	if (hnt_user_process_info.hprocess) [[likely]]
	{
		::fast_io::win32::nt::nt_close<family == nt_family::zw>(hnt_user_process_info.hprocess);
		hnt_user_process_info.hprocess = nullptr;
	}
}
template <nt_family family>
inline void close_nt_user_process_information(nt_user_process_information hnt_user_process_info) noexcept
{
	return details::close_nt_user_process_information_not_null<family>(hnt_user_process_info);
}
template <nt_family family>
inline ::std::uint_least32_t nt_wait_user_process_or_thread(void *hprocess_thread) noexcept
{
	return ::fast_io::win32::nt::nt_wait_for_single_object<family == nt_family::zw>(hprocess_thread, false, nullptr);
}

template <nt_family family, bool throw_eh = false>
inline void nt_wait_and_close_user_process_or_thread(void *handle) noexcept(!throw_eh)
{
	if (handle == nullptr) [[unlikely]]
	{
		return;
	}
	auto status{nt_wait_user_process_or_thread<family>(handle)};
	::fast_io::win32::nt::nt_close<family == nt_family::zw>(handle);

	if constexpr (throw_eh)
	{
		if (status) [[unlikely]]
		{
			throw_nt_error(status);
		}
	}
}

template <nt_family family, bool throw_eh = false>
inline void
close_nt_user_process_information_and_wait(nt_user_process_information hnt_user_process_info) noexcept(!throw_eh)
{
	// only need to wait hprocess
	nt_wait_and_close_user_process_or_thread<family, throw_eh>(hnt_user_process_info.hprocess);
	// close hthread (main thread)
	::fast_io::win32::nt::nt_close<family == nt_family::zw>(hnt_user_process_info.hthread);
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
struct rtl_guard
{
	rtl_user_process_parameters *rtl_up{};
	inline constexpr rtl_guard() noexcept = default;
	inline constexpr rtl_guard(rtl_user_process_parameters *r) noexcept
		: rtl_up{r} {};
	inline constexpr ~rtl_guard()
	{
		if (rtl_up) [[likely]]
		{
			::fast_io::win32::nt::RtlDestroyProcessParameters(rtl_up);
			rtl_up = nullptr;
		}
	};
	inline constexpr void clear() noexcept
	{
		if (rtl_up) [[likely]]
		{
			::fast_io::win32::nt::RtlDestroyProcessParameters(rtl_up);
			rtl_up = nullptr;
		}
	}
};

struct unicode_string_guard
{
	unicode_string *us{};
	inline constexpr unicode_string_guard() noexcept = default;
	inline constexpr unicode_string_guard(unicode_string *u) noexcept
		: us{u} {};
	inline constexpr ~unicode_string_guard()
	{
		if (us) [[likely]]
		{
			::fast_io::native_thread_local_allocator::deallocate(us);
			us = nullptr;
		}
	};
	inline constexpr void clear() noexcept
	{
		if (us) [[likely]]
		{
			::fast_io::native_thread_local_allocator::deallocate(us);
			us = nullptr;
		}
	}
};

// nt3x creation process, support is not considered temporarily, but do not delete the code
#if 0
template <nt_family family>
inline void nt_duplicate_object_std(void *parent_process, void *&standard_io_handle, void *process_handle)
{
	if (standard_io_handle == nullptr) [[unlikely]]
	{
		return;
	}
	check_nt_status(::fast_io::win32::nt::nt_duplicate_object<family>(
		parent_process, standard_io_handle, process_handle, __builtin_addressof(standard_io_handle), 0, 0,
		0x00000002 | 0x00000004));
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

	auto const current_peb{::fast_io::win32::nt::nt_get_current_peb()};

	if (!processio.in.is_dev_null)
	{
		if (processio.in.win32_pipe_in_handle && *processio.in.win32_pipe_in_handle)
		{
			rtl_temp->StandardInput = *processio.in.win32_pipe_in_handle;
			in_need_close = processio.in.win32_pipe_in_handle;

			::fast_io::win32::nt::object_handle_attribute_information ohaoi{.Inherit = 1};
			check_nt_status(::fast_io::win32::nt::nt_set_information_object<zw>(*processio.in.win32_pipe_out_handle,
																				::fast_io::win32::nt::object_information_class::ObjectHandleFlagInformation,
																				__builtin_addressof(ohaoi), static_cast<::std::uint_least32_t>(sizeof(ohaoi))));
		}
		else if (processio.in.win32_handle)
		{
			rtl_temp->StandardInput = processio.in.win32_handle;
		}
		else
		{
			rtl_temp->StandardInput = current_peb->ProcessParameters->StandardInput;
		}
	}
	else
	{
		rtl_temp->StandardInput = nullptr;
	}

	if (!processio.out.is_dev_null)
	{
		if (processio.out.win32_pipe_out_handle && *processio.out.win32_pipe_out_handle)
		{
			rtl_temp->StandardOutput = *processio.out.win32_pipe_out_handle;
			out_need_close = processio.out.win32_pipe_out_handle;

			::fast_io::win32::nt::object_handle_attribute_information ohaoi{.Inherit = 1};
			check_nt_status(::fast_io::win32::nt::nt_set_information_object<zw>(*processio.out.win32_pipe_in_handle,
																				::fast_io::win32::nt::object_information_class::ObjectHandleFlagInformation,
																				__builtin_addressof(ohaoi), static_cast<::std::uint_least32_t>(sizeof(ohaoi))));
		}
		else if (processio.out.win32_handle)
		{
			rtl_temp->StandardOutput = processio.out.win32_handle;
		}
		else
		{
			rtl_temp->StandardOutput = current_peb->ProcessParameters->StandardOutput;
		}
	}
	else
	{
		rtl_temp->StandardOutput = nullptr;
	}

	if (!processio.err.is_dev_null)
	{
		if (processio.err.win32_pipe_out_handle && *processio.err.win32_pipe_out_handle)
		{
			rtl_temp->StandardError = *processio.err.win32_pipe_out_handle;
			err_need_close = processio.err.win32_pipe_out_handle;

			::fast_io::win32::nt::object_handle_attribute_information ohaoi{.Inherit = 1};
			check_nt_status(::fast_io::win32::nt::nt_set_information_object<zw>(*processio.err.win32_pipe_in_handle,
																				::fast_io::win32::nt::object_information_class::ObjectHandleFlagInformation,
																				__builtin_addressof(ohaoi), static_cast<::std::uint_least32_t>(sizeof(ohaoi))));
		}
		else if (processio.err.win32_handle)
		{
			rtl_temp->StandardError = processio.err.win32_handle;
		}
		else
		{
			rtl_temp->StandardError = current_peb->ProcessParameters->StandardError;
		}
	}
	else
	{
		rtl_temp->StandardError = nullptr;
	}

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
		__builtin_addressof(hprocess), 0x2000000, nullptr,
		current_process, true, hsection, nullptr, nullptr));
	basic_nt_family_file<family, char> process(hprocess);

	// PROCESS_BASIC_INFO
	process_basic_information pb_info{};
	check_nt_status(::fast_io::win32::nt::nt_query_information_process<zw>(
		hprocess, process_information_class::ProcessBasicInformation, __builtin_addressof(pb_info), static_cast<::std::uint_least32_t>(sizeof(pb_info)),
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
	// to do !!!

	// Resume Thread
	::std::uint_least32_t lprevcount{};
	check_nt_status(::fast_io::win32::nt::nt_resume_thread<zw>(hthread, __builtin_addressof(lprevcount)));
	return {process.release(), thread.release()};
}
#endif

template <nt_family family>
inline nt_user_process_information nt_6x_process_create_impl(void *__restrict fhandle, char16_t const *args, char16_t const *envs,
															 win32_process_io const &__restrict processio, process_mode mode)
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

	if ((mode & process_mode::nt_absolute_path) != process_mode::nt_absolute_path) // dos or unc path
	{
		auto const NtImagePath_c16_buffer{NtImagePath->Buffer};
		auto const NtImagePath_u16_length{NtImagePath->Length / sizeof(char16_t)};
		unicode_string str_uni{};
		char16_t native_name[0x2001];

		if (NtImagePath_u16_length > 11 && ::fast_io::freestanding::my_memcmp(NtImagePath_c16_buffer, u"\\Device\\Mup", 11 * sizeof(char16_t)) == 0)
		{
			native_name[0] = u'\\';
			::fast_io::freestanding::non_overlapped_copy_n(NtImagePath_c16_buffer + 11, NtImagePath_u16_length - 11, native_name + 1);
			native_name[NtImagePath_u16_length - 10] = 0;
			str_uni.Buffer = native_name;
			str_uni.Length = static_cast<::std::uint_least16_t>((NtImagePath_u16_length - 10) * sizeof(char16_t));
			str_uni.MaximumLength = static_cast<::std::uint_least16_t>(0x2000 * sizeof(char16_t));
		}
		else if (NtImagePath_u16_length > 7 && ::fast_io::freestanding::my_memcmp(NtImagePath_c16_buffer, u"\\Device", 7 * sizeof(char16_t)) == 0)
		{
			constexpr nt_open_mode symbol_mode{
				.DesiredAccess = 0x00100000 | 0x0080, // SYNCHRONIZE | FILE_READ_ATTRIBUTES
				.FileAttributes = 0x80,               // FILE_ATTRIBUTE_NORMAL
				.ShareAccess = 0x00000003,            // FILE_SHARE_READ | FILE_SHARE_WRITE
				.CreateDisposition = 0x00000001,      // OPEN_EXISTING
				.CreateOptions = 0x00000020           // FILE_SYNCHRONOUS_IO_NONALERT
			};
			::fast_io::basic_nt_family_file<(zw ? nt_family::zw : nt_family::nt), char> MountPointManager(
				nt_call_callback(reinterpret_cast<void *>(::std::ptrdiff_t(-3)), u"\\Device\\MountPointManager", 25, true, nt_create_callback<zw>{symbol_mode}));

			::std::byte query_in_buffer_byte[1024 * sizeof(char16_t)];
			::std::byte query_out_buffer_byte[1024 * sizeof(char16_t)];

			using mounter_target_name_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= ::fast_io::win32::nt::mounter_target_name *;
			auto QueryInBuffer{reinterpret_cast<mounter_target_name_may_alias_ptr>(query_in_buffer_byte)};

			auto const NtImagePath_c16_buffer_end{NtImagePath_c16_buffer + NtImagePath_u16_length};
			auto find_next_rl_end{NtImagePath_c16_buffer_end};
			auto const find_next_rl{::fast_io::freestanding::find(NtImagePath_c16_buffer + 8, find_next_rl_end, u'\\')};
			if (find_next_rl != find_next_rl_end)
			{
				find_next_rl_end = find_next_rl;
			}

			auto const find_res_strlen{static_cast<::std::size_t>(find_next_rl_end - NtImagePath_c16_buffer)};
			QueryInBuffer->DeviceNameLength = static_cast<::std::uint_least16_t>(find_res_strlen) * sizeof(char16_t);
			::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte *>(NtImagePath_c16_buffer), QueryInBuffer->DeviceNameLength, reinterpret_cast<::std::byte *>(QueryInBuffer->DeviceName));
			QueryInBuffer->DeviceName[find_res_strlen] = 0;

			using mountmgr_volume_paths_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= ::fast_io::win32::nt::mountmgr_volume_paths *;
			auto QueryOutBuffer{reinterpret_cast<mountmgr_volume_paths_may_alias_ptr>(query_out_buffer_byte)};

			::fast_io::win32::nt::io_status_block isb;

			auto status{::fast_io::win32::nt::nt_device_io_control_file<zw>(
				MountPointManager.native_handle(),
				nullptr,
				nullptr,
				nullptr,
				__builtin_addressof(isb),
				0x006D0030,
				QueryInBuffer,
				1024,
				QueryOutBuffer,
				1024)};

			if (status) 
			{
				// Unmounted, use nt absolute path directly
				str_uni = *NtImagePath; 
			}
			else [[likely]]
			{
				auto const MultiSz_strlen{::fast_io::cstr_len(QueryOutBuffer->MultiSz)};
				auto native_name_curr{native_name};
				native_name_curr = ::fast_io::freestanding::non_overlapped_copy_n(QueryOutBuffer->MultiSz, MultiSz_strlen, native_name_curr);
				native_name_curr = ::fast_io::freestanding::non_overlapped_copy(NtImagePath_c16_buffer + find_res_strlen, NtImagePath_c16_buffer_end, native_name_curr);
				*native_name_curr = 0;
				
				str_uni.Buffer = native_name;
				str_uni.Length = static_cast<::std::uint_least16_t>((native_name_curr - native_name) * sizeof(char16_t));
				str_uni.MaximumLength = static_cast<::std::uint_least16_t>(0x2000 * sizeof(char16_t));
			}
		}
		else [[unlikely]]
		{
			// Unable to map to DOS or UNC path, use nt absolute path directly
			str_uni = *NtImagePath; 
		}

		check_nt_status(::fast_io::win32::nt::RtlCreateProcessParametersEx(
			__builtin_addressof(rtl_temp), __builtin_addressof(str_uni), nullptr, nullptr, args ? __builtin_addressof(ps_para) : nullptr,
			(void *)(envs), nullptr, nullptr, nullptr, nullptr, 0x01));
	}
	else
	{
		check_nt_status(::fast_io::win32::nt::RtlCreateProcessParametersEx(
			__builtin_addressof(rtl_temp), NtImagePath, nullptr, nullptr, args ? __builtin_addressof(ps_para) : nullptr,
			(void *)(envs), nullptr, nullptr, nullptr, nullptr, 0x01));
	}

	rtl_guard rtlm{rtl_temp}; // guard

	// Duplicate Process Std Handles
	auto const current_peb{::fast_io::win32::nt::nt_get_current_peb()};

	if (!processio.in.is_dev_null)
	{
		if (processio.in.win32_pipe_in_handle)
		{
			rtl_temp->StandardInput = processio.in.win32_pipe_in_handle;

			::fast_io::win32::nt::object_handle_attribute_information ohaoi{.Inherit = 1};
			check_nt_status(::fast_io::win32::nt::nt_set_information_object<zw>(processio.in.win32_pipe_out_handle,
																				::fast_io::win32::nt::object_information_class::ObjectHandleFlagInformation,
																				__builtin_addressof(ohaoi), static_cast<::std::uint_least32_t>(sizeof(ohaoi))));
		}
		else if (processio.in.win32_handle)
		{
			rtl_temp->StandardInput = processio.in.win32_handle;
		}
		else
		{
			rtl_temp->StandardInput = current_peb->ProcessParameters->StandardInput;
		}
	}
	else
	{
		rtl_temp->StandardInput = nullptr;
	}

	if (!processio.out.is_dev_null)
	{
		if (processio.out.win32_pipe_out_handle)
		{
			rtl_temp->StandardOutput = processio.out.win32_pipe_out_handle;

			::fast_io::win32::nt::object_handle_attribute_information ohaoi{.Inherit = 1};
			check_nt_status(::fast_io::win32::nt::nt_set_information_object<zw>(processio.out.win32_pipe_in_handle,
																				::fast_io::win32::nt::object_information_class::ObjectHandleFlagInformation,
																				__builtin_addressof(ohaoi), static_cast<::std::uint_least32_t>(sizeof(ohaoi))));
		}
		else if (processio.out.win32_handle)
		{
			rtl_temp->StandardOutput = processio.out.win32_handle;
		}
		else
		{
			rtl_temp->StandardOutput = current_peb->ProcessParameters->StandardOutput;
		}
	}
	else
	{
		rtl_temp->StandardOutput = nullptr;
	}

	if (!processio.err.is_dev_null)
	{
		if (processio.err.win32_pipe_out_handle)
		{
			rtl_temp->StandardError = processio.err.win32_pipe_out_handle;

			::fast_io::win32::nt::object_handle_attribute_information ohaoi{.Inherit = 1};
			check_nt_status(::fast_io::win32::nt::nt_set_information_object<zw>(processio.err.win32_pipe_in_handle,
																				::fast_io::win32::nt::object_information_class::ObjectHandleFlagInformation,
																				__builtin_addressof(ohaoi), static_cast<::std::uint_least32_t>(sizeof(ohaoi))));
		}
		else if (processio.err.win32_handle)
		{
			rtl_temp->StandardError = processio.err.win32_handle;
		}
		else
		{
			rtl_temp->StandardError = current_peb->ProcessParameters->StandardError;
		}
	}
	else
	{
		rtl_temp->StandardError = nullptr;
	}

	// 0 == create new window, 4 == current windows, 0xfffffffffffffffd == no windows (background)
	rtl_temp->ConsoleHandle = reinterpret_cast<void *>(0x4);

	// Initialize the PS_CREATE_INFO structure
	ps_create_info CreateInfo{};
	CreateInfo.Size = sizeof(CreateInfo);
	CreateInfo.State = ps_create_state::PsCreateInitialState;
	CreateInfo.stateunion.InitState.u1.s1.ProhibitedImageCharacteristics = 0x2000;
	CreateInfo.stateunion.InitState.AdditionalFileAccess = 129;

	// Initialize the PS_ATTRIBUTE_LIST structure
	ps_attribute_list AttributeList{};
	AttributeList.TotalLength = 5 * sizeof(ps_attribute) + sizeof(::std::size_t);

	AttributeList.Attributes[0].Attribute = 131077;
	AttributeList.Attributes[0].Size = NtImagePath->Length;
	AttributeList.Attributes[0].ReturnLength = 0;
	AttributeList.Attributes[0].Value = reinterpret_cast<::std::size_t>(NtImagePath->Buffer);

	AttributeList.Attributes[1].Attribute = 65539; // PS_ATTRIBUTE_CLIENT_ID
	::fast_io::win32::nt::client_id cid{};
	AttributeList.Attributes[1].Size = sizeof(cid);
	AttributeList.Attributes[1].ReturnLength = 0;
	AttributeList.Attributes[1].ValuePtr = __builtin_addressof(cid);

	AttributeList.Attributes[2].Attribute = 6; // PS_ATTRIBUTE_IMAGE_INFO
	::fast_io::win32::nt::section_image_information sif{};
	AttributeList.Attributes[2].Size = sizeof(sif);
	AttributeList.Attributes[2].ReturnLength = 0;
	AttributeList.Attributes[2].ValuePtr = __builtin_addressof(sif);

	AttributeList.Attributes[3].Attribute = 393242; // PS_ATTRIBUTE_CHPE
	AttributeList.Attributes[3].Size = 1;
	AttributeList.Attributes[3].ReturnLength = 0;
	AttributeList.Attributes[3].Value = 1;

	AttributeList.Attributes[4].Attribute = 131082; // PS_ATTRIBUTE_STD_HANDLE_INFO
	::fast_io::win32::nt::ps_std_handle_info pshi{};
	pshi.StdHandleSubsystemType = 3;        // IMAGE_SUBSYSTEM_WINDOWS_CUI
	pshi.detailed_flags.StdHandleState = 2; // PsAlwaysDuplicate == (win32) bInheritHandles
	pshi.detailed_flags.PseudoHandleMask = 0;
	AttributeList.Attributes[4].Size = sizeof(pshi);
	AttributeList.Attributes[4].ReturnLength = 0;
	AttributeList.Attributes[4].ValuePtr = __builtin_addressof(pshi);

	// Create the process
	void *hProcess{};
	void *hThread{};
	check_nt_status(::fast_io::win32::nt::nt_create_user_process<zw>(
		&hProcess, &hThread, 0x2000000, 0x2000000, nullptr, nullptr,
		512, 0x00, rtl_temp, &CreateInfo, &AttributeList));

	return {hProcess, hThread};
}

template <nt_family family>
inline nt_user_process_information nt_process_create_impl(void *__restrict fhandle, char16_t const *args,
														  char16_t const *envs,
														  win32_process_io const &__restrict processio, process_mode mode)
{
	// !defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x600
	// only support nt6x
	return nt_6x_process_create_impl<family>(fhandle, args, envs, processio, mode);
}

template <nt_family family, typename path_type>
inline nt_user_process_information nt_create_process_overloads(nt_at_entry entry, path_type const &filename,
															   nt_process_args const &args, nt_process_envs const &envs,
															   win32_process_io const &processio, process_mode mode)
{
	basic_nt_family_file<family, char> nf(entry, filename, open_mode::in | open_mode::excl);
	return nt_process_create_impl<family>(nf.handle, args.get(), envs.get(), processio, mode);
}

template <nt_family family, typename path_type>
inline nt_user_process_information nt_create_process_overloads(path_type const &filename, nt_process_args const &args,
															   nt_process_envs const &envs,
															   win32_process_io const &processio, process_mode mode)
{
	basic_nt_family_file<family, char> nf(filename, open_mode::in | open_mode::excl);
	return nt_process_create_impl<family>(nf.handle, args.get(), envs.get(), processio, mode);
}

template <nt_family family>
inline nt_user_process_information nt_create_process_overloads(::fast_io::nt_fs_dirent ent, nt_process_args const &args,
															   nt_process_envs const &envs,
															   win32_process_io const &processio, process_mode mode)
{
	basic_nt_family_file<family, char> nf(ent, open_mode::in | open_mode::excl);
	return nt_process_create_impl<family>(nf.handle, args.get(), envs.get(), processio, mode);
}

} // namespace win32::nt::details

template <nt_family family>
	requires(family == nt_family::nt || family == nt_family::zw)
class nt_family_process_observer
{
public:
	using native_handle_type = nt_user_process_information;
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

template <nt_family family>
inline void detach(nt_family_process_observer<family> &ppob) noexcept
{
	win32::nt::details::close_nt_user_process_information<family>(ppob.hnt_user_process_info);
	ppob.hnt_user_process_info = {};
}

struct nt_wait_status
{
	::std::uint_least32_t wait_loc{}; // exit code
};

inline constexpr int wait_status_to_int(nt_wait_status waits) noexcept
{
	return static_cast<int>(waits.wait_loc);
}

template <nt_family family, bool throw_eh = true>
	requires(family == nt_family::nt || family == nt_family::zw)
inline nt_wait_status wait(nt_family_process_observer<family> ppob) noexcept(!throw_eh)
{
	if (ppob.hnt_user_process_info.hprocess == nullptr) [[unlikely]]
	{
		if constexpr (throw_eh)
		{
			throw_nt_error(0xC0000008);
		}
		else
		{
			return {static_cast<::std::uint_least32_t>(-1)};
		}
	}

	// wait for process
	auto const status{win32::nt::details::nt_wait_user_process_or_thread<family>(ppob.hnt_user_process_info.hprocess)};
	if (status) [[unlikely]]
	{
		if constexpr (throw_eh)
		{
			throw_nt_error(status);
		}
		else
		{
			return {static_cast<::std::uint_least32_t>(-1)};
		}
	}

	// get exit code
	::fast_io::win32::nt::process_basic_information pbi{};
	auto const status2{
		::fast_io::win32::nt::nt_query_information_process<family == nt_family::zw>(ppob.hnt_user_process_info.hprocess,
																					::fast_io::win32::nt::process_information_class::ProcessBasicInformation,
																					__builtin_addressof(pbi),
																					static_cast<::std::uint_least32_t>(sizeof(pbi)),
																					nullptr)};
	if (status2) [[unlikely]]
	{
		if constexpr (throw_eh)
		{
			throw_nt_error(status);
		}
		else
		{
			return {static_cast<::std::uint_least32_t>(-1)};
		}
	}

	return {pbi.ExitStatus};
}

template <nt_family family>
inline void kill(nt_family_process_observer<family> ppob, nt_wait_status exit_code)
{
	auto const status{::fast_io::win32::nt::nt_terminate_process<family == nt_family::zw>(
		ppob.hnt_user_process_info.hprocess, static_cast<::std::int_least32_t>(exit_code.wait_loc))};

	if (status) [[unlikely]]
	{
		throw_nt_error(status);
	}
}

template <nt_family family>
	requires(family == nt_family::nt || family == nt_family::zw)
class nt_family_process : public nt_family_process_observer<family>
{
public:
	using native_handle_type = nt_user_process_information;
	inline constexpr nt_family_process() noexcept = default;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr nt_family_process(native_hd hd) noexcept
		: nt_family_process_observer<family>{hd}
	{
	}

	template <::fast_io::constructible_to_os_c_str path_type>
	inline explicit nt_family_process(nt_at_entry nate, path_type const &filename, nt_process_args const &args = {},
									  nt_process_envs const &envs = {}, win32_process_io const &processio = {}, process_mode mode = {})
		: nt_family_process_observer<family>{
			  win32::nt::details::nt_create_process_overloads<family>(nate, filename, args, envs, processio, mode)}
	{
	}

	template <::fast_io::constructible_to_os_c_str path_type>
	inline explicit nt_family_process(path_type const &filename, nt_process_args const &args = {}, nt_process_envs const &envs = {},
									  win32_process_io const &processio = {}, process_mode mode = {})
		: nt_family_process_observer<family>{
			  win32::nt::details::nt_create_process_overloads<family>(filename, args, envs, processio, mode)}
	{
	}

	inline explicit nt_family_process(::fast_io::nt_fs_dirent ent, nt_process_args const &args = {}, nt_process_envs const &envs = {},
									  win32_process_io const &processio = {}, process_mode mode = {})
		: nt_family_process_observer<family>{
			  win32::nt::details::nt_create_process_overloads<family>(ent, args, envs, processio, mode)}
	{
	}

	inline nt_family_process(nt_family_process const &b) = delete;
	inline nt_family_process &operator=(nt_family_process const &b) = delete;
	inline constexpr nt_family_process(nt_family_process &&__restrict b) noexcept
		: nt_family_process_observer<family>{b.release()}
	{
	}
	inline nt_family_process &operator=(nt_family_process &&__restrict b) noexcept
	{
		win32::nt::details::close_nt_user_process_information_and_wait<family>(this->hnt_user_process_info);
		this->hnt_user_process_info = b.release();
		return *this;
	}
	inline ~nt_family_process()
	{
		win32::nt::details::close_nt_user_process_information_and_wait<family>(this->hnt_user_process_info);
		this->hnt_user_process_info = {};
	}
};

using nt_process_observer = nt_family_process_observer<nt_family::nt>;
using nt_process = nt_family_process<nt_family::nt>;

using zw_process_observer = nt_family_process_observer<nt_family::zw>;
using zw_process = nt_family_process<nt_family::zw>;

} // namespace fast_io
