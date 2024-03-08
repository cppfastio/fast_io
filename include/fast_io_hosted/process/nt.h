#pragma once

namespace fast_io
{

struct nt_user_process_information
{
	void *hprocess{};
	void *hthread{};
};

struct nt_process_args
{
	char16_t const *args{};

	constexpr nt_process_args() noexcept = default;
	constexpr nt_process_args(char16_t const *a) noexcept
		: args{a} {};
};

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
inline void nt_wait_and_close_user_process_or_thread(void *&handle) noexcept(!throw_eh)
{
	if (handle == nullptr)
	{
		return;
	}
	auto status{nt_wait_user_process_or_thread<family>(handle)};
	auto status2{::fast_io::win32::nt::nt_close<family == nt_family::zw>(handle)};
	handle = nullptr;
	if constexpr (throw_eh)
	{
		if (status)
		{
			throw_nt_error(status);
		}
		if (status2)
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
		ps_para.Length = ::fast_io::cstr_len(args) * sizeof(char16_t);
		ps_para.MaximumLength = ps_para.Length + sizeof(char16_t);
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
		ps_para.Length = ::fast_io::cstr_len(args) * sizeof(char16_t);
		ps_para.MaximumLength = ps_para.Length + sizeof(char16_t);
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

} // namespace win32::nt::details

template <nt_family family>
	requires(family == nt_family::nt || family == nt_family::zw)
class nt_family_process_observer
{
public:
	using native_handle_type = nt_user_process_information;
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

template <nt_family family>
inline void detach(nt_family_process_observer<family> &ppob) noexcept
{
	win32::nt::details::close_nt_user_process_information<family == nt_family::zw>(ppob.hnt_user_process_info);
	ppob.hnt_user_process_info = nullptr;
}

#if 0
struct nt_wait_status
{
	::std::uint_least32_t exit_code{};
};

inline nt_wait_status query_process_basic_info(nt_process_observer ppob)
{
	
}

inline nt_wait_status wait(nt_process_observer ppob)
{
	if(handle==nullptr)
		throw_nt_status(0xC0000008);
	win32::nt::details_wait_and_close_user_process_or_thread<true>(ppob.hnt_user_process_info->hthread);
	auto& hprocess=ppob.hnt_user_process_info->hprocess;
	auto status{nt_wait_user_process_or_thread(hprocess)};

	auto status2{win32::nt::nt_close(hprocess)};

	hprocess=nullptr;
	if constexpr(throw_eh)
	{
		if(status)
			throw_nt_error(status);
		if(status2)
			throw_nt_error(status2);
	}
}
#endif

template <nt_family family>
class nt_family_process : public nt_family_process_observer<family>
{
public:
	using native_handle_type = nt_user_process_information;
	constexpr nt_family_process() noexcept = default;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	explicit constexpr nt_family_process(native_hd hd) noexcept
		: nt_family_process_observer<family>{hd}
	{
	}

	template <::fast_io::constructible_to_os_c_str path_type>
	explicit nt_family_process(nt_at_entry nate, path_type const &filename, nt_process_args const &args,
							   nt_process_args const &envs, win32_process_io const &processio)
		: nt_family_process_observer<family>{
			  win32::nt::details::nt_create_process_overloads<family>(nate, filename, args, envs, processio)}
	{
	}

	template <::fast_io::constructible_to_os_c_str path_type>
	explicit nt_family_process(path_type const &filename, nt_process_args const &args, nt_process_args const &envs,
							   win32_process_io const &processio)
		: nt_family_process_observer<family>{
			  win32::nt::details::nt_create_process_overloads<family>(filename, args, envs, processio)}
	{
	}

	explicit nt_family_process(::fast_io::nt_fs_dirent ent, nt_process_args const &args, nt_process_args const &envs,
							   win32_process_io const &processio)
		: nt_family_process_observer<family>{
			  win32::nt::details::nt_create_process_overloads<family>(ent, args, envs, processio)}
	{
	}

	nt_family_process(nt_family_process const &b) = delete;
	nt_family_process &operator=(nt_family_process const &b) = delete;
	constexpr nt_family_process(nt_family_process &&__restrict b) noexcept
		: nt_family_process_observer<family>{b.release()}
	{
	}
	nt_family_process &operator=(nt_family_process &&__restrict b) noexcept
	{
		win32::nt::details::close_nt_user_process_information_and_wait<family>(this->hnt_user_process_info);
		this->hnt_user_process_info = b.release();
		return *this;
	}
	~nt_family_process()
	{
		win32::nt::details::close_nt_user_process_information_and_wait<family>(this->hnt_user_process_info);
	}
};

using nt_process_observer = nt_family_process_observer<nt_family::nt>;
using nt_process = nt_family_process<nt_family::nt>;

using zw_process_observer = nt_family_process_observer<nt_family::zw>;
using zw_process = nt_family_process<nt_family::zw>;

} // namespace fast_io
