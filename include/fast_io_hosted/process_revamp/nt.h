#pragma once

namespace fast_io
{

struct nt_user_process_information
{
	void* hprocess{};
	void* hthread{};
};

struct nt_process_args
{
	char const* const* args{};
	bool is_dynamic_allocated{};
	inline constexpr nt_process_args(char const* const* envir) noexcept :args(envir) {}
	template<::std::random_access_iterator Iter>
		requires (::std::convertible_to<::std::iter_value_t<Iter>, char const*> || requires(::std::iter_value_t<Iter> v)
	{
		{ v.c_str() }->::std::convertible_to<char const*>;
	})
		inline constexpr nt_process_args(Iter begin, Iter end) :
		args(details::dup_enviro_entry(begin, end)), is_dynamic_allocated(true)
	{}
	template<::std::ranges::random_access_range range>
		requires (::std::convertible_to<::std::ranges::range_value_t<range>, char const*> || requires(::std::ranges::range_value_t<range> v)
	{
		{ v.c_str() }->::std::convertible_to<char const*>;
	})
		inline constexpr nt_process_args(range&& rg) :nt_process_args(::std::ranges::cbegin(rg), ::std::ranges::cend(rg))
	{}
	inline constexpr nt_process_args(::std::initializer_list<char const*> ilist) :
		nt_process_args(ilist.begin(), ilist.end()) {}
	nt_process_args(nt_process_args const&) = delete;
	nt_process_args& operator=(nt_process_args const&) = delete;
#if __cpp_constexpr_dynamic_alloc >= 201907L
	inline constexpr
#endif
		~nt_process_args()
	{
		if (is_dynamic_allocated)
			delete[] args;
	}
};

namespace win32::nt::details
{
template<nt_family family>
inline void close_nt_user_process_information_not_null(nt_user_process_information hnt_user_process_info) noexcept
{
	if(hnt_user_process_info.hthread)[[likely]]
	{
		::fast_io::win32::nt::nt_close<family == nt_family::zw>(hnt_user_process_info.hthread);
		hnt_user_process_info.hthread=nullptr;
	}
	if(hnt_user_process_info.hprocess)[[likely]]
	{
		::fast_io::win32::nt::nt_close<family == nt_family::zw>(hnt_user_process_info.hprocess);
		hnt_user_process_info.hprocess=nullptr;
	}
}
template<nt_family family>
inline void close_nt_user_process_information(nt_user_process_information hnt_user_process_info) noexcept
{
	return details::close_nt_user_process_information_not_null<family>(hnt_user_process_info);
}
template<nt_family family>
inline ::std::uint_least32_t nt_wait_user_process_or_thread(void* hprocess_thread) noexcept
{
	return ::fast_io::win32::nt::nt_wait_for_single_object<family == nt_family::zw>(hprocess_thread, false, nullptr);
}

template<nt_family family,bool throw_eh=false>
inline void nt_wait_and_close_user_process_or_thread(void*& handle) noexcept(!throw_eh)
{
	if(handle==nullptr)
		return;
	auto status{nt_wait_user_process_or_thread<family>(handle)};
	auto status2{::fast_io::win32::nt::nt_close<family == nt_family::zw>(handle)};
	handle=nullptr;
	if constexpr(throw_eh)
	{
		if(status)
			throw_nt_error(status);
		if(status2)
			throw_nt_error(status2);
	}
}

template<nt_family family,bool throw_eh=false>
inline void close_nt_user_process_information_and_wait(nt_user_process_information hnt_user_process_info) noexcept(!throw_eh)
{
	nt_wait_and_close_user_process_or_thread<family,throw_eh>(hnt_user_process_info.hthread);
	nt_wait_and_close_user_process_or_thread<family,throw_eh>(hnt_user_process_info.hprocess);
}

template<nt_family family>
inline void nt_duplicate_object_std(void* parent_process,void*& standard_io_handle,void* process_handle)
{
	if(standard_io_handle==nullptr)
		return;
	check_nt_status(::fast_io::win32::nt::nt_duplicate_object<family>(
		parent_process, standard_io_handle, process_handle, 
		__builtin_addressof(standard_io_handle), 0, 0, 0x00000002 | 0x00000004));
}

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline void check_nt_status(::std::uint_least32_t status)
{
	if (status) [[unlikely]]
		throw_nt_error(status);
}

template<bool zw>
inline void* nt_duplicate_process_std_handle_impl(void* __restrict hprocess,nt_io_redirection const& redi)
{
	void* const current_process{};
	void* ptr{};
	if(redi.nt_handle)[[likely]]
	{
		check_nt_status(::fast_io::win32::nt::nt_duplicate_object<zw>(hprocess, current_process, redi.nt_handle, __builtin_addressof(ptr), 0, 0,
		/*DUPLICATE_SAME_ACCESS|DUPLICATE_SAME_ATTRIBUTES*/0x00000004|0x00000002));
	}
	return ptr;
}

template<bool zw>
inline void nt_duplicate_process_std_handles_impl(void* __restrict hprocess, nt_process_io const& processio, rtl_user_process_parameters* __restrict para)
{
	if (!para) [[unlikely]]
		return;
	void* ptr{};
	ptr = nt_duplicate_process_std_handle_impl<zw>(hprocess, processio.in);
	check_nt_status(::fast_io::win32::nt::nt_write_virtual_memory<zw>(hprocess, __builtin_addressof(para->StandardInput), __builtin_addressof(ptr), sizeof(ptr), nullptr));
	ptr = nt_duplicate_process_std_handle_impl<zw>(hprocess, processio.out);
	check_nt_status(::fast_io::win32::nt::nt_write_virtual_memory<zw>(hprocess, __builtin_addressof(para->StandardOutput), __builtin_addressof(ptr), sizeof(ptr), nullptr));
	ptr = nt_duplicate_process_std_handle_impl<zw>(hprocess, processio.err);
	check_nt_status(::fast_io::win32::nt::nt_write_virtual_memory<zw>(hprocess, __builtin_addressof(para->StandardError), __builtin_addressof(ptr), sizeof(ptr), nullptr));
}

inline void copy_parameter_string(char16_t** Ptr, unicode_string* Destination, unicode_string* Source, ::std::uint_least16_t Size) noexcept
{
	Destination->Length = Source->Length;
	Destination->MaximumLength = Size ? Size : Source->MaximumLength;
	Destination->Buffer = *Ptr;
	if (Source->Length)
		::fast_io::freestanding::my_memmove(Destination->Buffer, Source->Buffer, Source->Length);
	Destination->Buffer[Destination->Length / sizeof(char16_t)] = 0;
	*Ptr += Destination->MaximumLength / sizeof(char16_t);
}

inline constexpr ::std::uint_least32_t align(::std::uint_least32_t x, ::std::uint_least32_t align) noexcept {
	return (x + align - 1) & (~(align - 1UL));
}

template <bool zw>
inline void nt_push_process_parameters(void* __restrict hprocess, char const* const* args, char const* const* envs, rtl_user_process_parameters* __restrict para)
{
	if (!para) [[unlikely]]
		return;

	auto c_peb{nt_get_current_peb()};
	auto this_propara{c_peb->ProcessParameters};

	rtl_user_process_parameters rtl_up{};

	unicode_string EmptyString{};
	EmptyString.Length = 0;
	EmptyString.MaximumLength = sizeof(char16_t);
	EmptyString.Buffer = u"";

	::fast_io::win32::nt::RtlAcquirePebLock();

	::std::uint_least32_t Length{};
	inline constexpr ::std::uint_least32_t max_path{260};

	Length = sizeof(rtl_user_process_parameters);

	/* size of current directory buffer */
	Length += (max_path * sizeof(char16_t));

	/* add string lengths */
	Length += align(this_propara->DllPath.MaximumLength, sizeof(::std::uint_least32_t));
	Length += align(this_propara->ImagePathName->Length + sizeof(char16_t), sizeof(::std::uint_least32_t));
	Length += align(this_propara->CommandLine->Length + sizeof(char16_t), sizeof(::std::uint_least32_t));
	Length += align(this_propara->WindowTitle->MaximumLength, sizeof(::std::uint_least32_t));
	Length += align(this_propara->DesktopInfo->MaximumLength, sizeof(::std::uint_least32_t));
	Length += align(this_propara->ShellInfo->MaximumLength, sizeof(::std::uint_least32_t));
	Length += align(this_propara->RuntimeData->MaximumLength, sizeof(::std::uint_least32_t));

	rtl_up.MaximumLength = Length;
	rtl_up.Length = Length;
	rtl_up.Flags = 0x01 /*RTL_USER_PROCESS_PARAMETERS_NORMALIZED*/;
	if (envs) 
	{
		// to do
	} 
	else
		rtl_up.Environment = this_propara->Environment;
	rtl_up.CurrentDirectory.Handle = this_propara->CurrentDirectory.Handle;
	rtl_up.ConsoleHandle = this_propara->ConsoleHandle;
	rtl_up.ConsoleFlags = this_propara->ConsoleFlags;

	char16_t* Dest{reinterpret_cast<char16_t*>(reinterpret_cast<::std::byte*>(__builtin_addressof(rtl_up)) + sizeof(rtl_user_process_parameters))};
	
	/* copy current directory */
	copy_parameter_string(__builtin_addressof(Dest), __builtin_addressof(rtl_up.CurrentDirectory.DosPath), this_propara->CurrentDirectory.DosPath, max_path * sizeof(char16_t));

	/* make sure the current directory has a trailing backslash */
	if (rtl_up.CurrentDirectory.DosPath.Length > 0) {
		Length = rtl_up.CurrentDirectory.DosPath.Length / sizeof(char16_t);
		if (rtl_up.CurrentDirectory.DosPath.Buffer[Length - 1] != u'\\') {
			rtl_up.CurrentDirectory.DosPath.Buffer[Length] = u'\\';
			rtl_up.CurrentDirectory.DosPath.Buffer[Length + 1] = 0;
			rtl_up.CurrentDirectory.DosPath.Length += sizeof(char16_t);
		}
	}

	/* copy dll path */
	copy_parameter_string(__builtin_addressof(Dest), __builtin_addressof(rtl_up.DllPath), this_propara->DllPath,0);

	/* copy image path name */
	{
		unicode_string ImagePathName_temp{};
		// to do
		copy_parameter_string(__builtin_addressof(Dest), __builtin_addressof(rtl_up.ImagePathName), ImagePathName_temp, ImagePathName_temp.Length + sizeof(char16_t));
	}

	/* copy command line */
	if (args) 
	{
		// to do
	} 
	else 
		copy_parameter_string(__builtin_addressof(Dest), __builtin_addressof(rtl_up.CommandLine), EmptyString, EmptyString.Length + sizeof(char16_t));

	/* copy title */
	copy_parameter_string(__builtin_addressof(Dest), __builtin_addressof(rtl_up.WindowTitle), this_propara->WindowTitle, 0);

	/* copy desktop */
	copy_parameter_string(__builtin_addressof(Dest), __builtin_addressof(rtl_up.DesktopInfo), this_propara->DesktopInfo, 0);

	/* copy shell info */
	copy_parameter_string(__builtin_addressof(Dest), __builtin_addressof(rtl_up.ShellInfo), this_propara->ShellInfo, 0);

	/* copy runtime info */
	copy_parameter_string(__builtin_addressof(Dest), __builtin_addressof(rtl_up.RuntimeData), this_propara->RuntimeData, 0);

	::fast_io::win32::nt::RtlReleasePebLock();

	// to do
	// https://github.com/reactos/reactos/blob/89d8b472af5b5c3960cbe92f9521b9d866398798/dll/win32/kernel32/client/proc.c#L746
}

template <nt_family family>
inline nt_user_process_information nt_process_create_impl(void* __restrict fhandle, char const* const* args, char const* const* envs, nt_process_io const& __restrict processio)
{
	constexpr bool zw{family==nt_family::zw};

	// Section
	void* hsection{};
	check_nt_status(::fast_io::win32::nt::nt_create_section<zw>(__builtin_addressof(hsection), 0xf001f /*SECTION_ALL_ACCESS*/,
		nullptr, nullptr, 0x02 /*PAGE_READ_ONLY*/, 0x1000000 /*SEC_IMAGE*/, fhandle));
	basic_nt_family_file<family, char> section{hsection};
	void* const current_process{reinterpret_cast<void*>(static_cast<::std::ptrdiff_t>(-1))};

	// Process
	void* hprocess{};
	check_nt_status(::fast_io::win32::nt::nt_create_process<zw>(__builtin_addressof(hprocess),
		0x000F0000U | 0x00100000U | 0xFFF /*PROCESS_ALL_ACCESS*/,
		nullptr, current_process, true, hsection, nullptr, nullptr));
	basic_nt_family_file<family,char> process(hprocess);

	// PROCESS_BASIC_INFO
	process_basic_information pb_info{};
	check_nt_status(::fast_io::win32::nt::nt_query_information_process<zw>(hprocess, process_information_class::ProcessBasicInformation,
		__builtin_addressof(pb_info),sizeof(pb_info),nullptr));

	// PushProcessParameters
	rtl_user_process_parameters* rtl_up{};
	check_nt_status(::fast_io::win32::nt::nt_read_virtual_memory<zw>(hprocess,
																	 __builtin_addressof(reinterpret_cast<peb*>(pb_info.PebBaseAddress)->ProcessParameters),
																	 __builtin_addressof(rtl_up),
																	 sizeof(rtl_up),
																	 nullptr));
	nt_push_process_parameters<zw>(hprocess, args, envs, rtl_up);

	// Duplicate Process Std Handles
	nt_duplicate_process_std_handles_impl<zw>(hprocess, processio, rtl_up);
	
	// Thread
	void* hthread{};
	client_id cid{};
	section_image_information sec_info{};
	check_nt_status(::fast_io::win32::nt::nt_query_section<zw>(hsection, section_information_class::SectionImageInformation,
		__builtin_addressof(sec_info), sizeof(sec_info), nullptr));

	check_nt_status(::fast_io::win32::nt::RtlCreateUserThread(hprocess, nullptr, true, sec_info.ZeroBits, sec_info.MaximumStackSize,
			sec_info.CommittedStackSize,sec_info.TransferAddress,pb_info.PebBaseAddress,__builtin_addressof(hthread),__builtin_addressof(cid)));
	basic_nt_family_file<family,char> thread(hthread);

	// Call Windows Server

	// Resume Thread
	::std::uint_least32_t lprevcount{};
	check_nt_status(::fast_io::win32::nt::nt_resume_thread<zw>(hthread,__builtin_addressof(lprevcount)));
	return {process.release(), thread.release()};
}

template<nt_family family,typename path_type>
inline nt_user_process_information nt_create_process_overloads(nt_at_entry entry, path_type const& filename, nt_process_args const& args, nt_process_args const& envs, nt_process_io const& processio)
{
	basic_nt_family_file<family,char> nf(entry,filename, open_mode::in | open_mode::excl );
	return nt_process_create_impl<family>(nf.handle, args.args, envs.args, processio);
}

template<nt_family family,typename path_type>
inline nt_user_process_information nt_create_process_overloads(path_type const& filename, nt_process_args const& args, nt_process_args const& envs, nt_process_io const& processio) 
{
	basic_nt_family_file<family,char> nf(filename, open_mode::in | open_mode::excl);
	return nt_process_create_impl<family>(nf.handle, args.args, envs.args, processio);
}

template<nt_family family,typename path_type>
inline nt_user_process_information nt_create_process_overloads(::fast_io::nt_fs_dirent ent, nt_process_args const& args, nt_process_args const& envs, nt_process_io const& processio) 
{
	basic_nt_family_file<family,char> nf(ent, open_mode::in | open_mode::excl);
	return nt_process_create_impl<family>(nf.handle, args.args, envs.args, processio);
}

}

template<nt_family family>
requires (family==nt_family::nt||family==nt_family::zw)
class nt_family_process_observer
{
public:
	using native_handle_type = nt_user_process_information;
	native_handle_type hnt_user_process_info{};
	constexpr auto& native_handle() noexcept
	{
		return hnt_user_process_info;
	}
	constexpr auto& native_handle() const noexcept
	{
		return hnt_user_process_info;
	}
	explicit constexpr operator bool() const noexcept
	{
		return reinterpret_cast<::std::size_t>(hnt_user_process_info.hprocess) | reinterpret_cast<::std::size_t>(hnt_user_process_info.hthread);
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{hnt_user_process_info};
		hnt_user_process_info={};
		return temp;
	}
};

template<nt_family family>
inline void detach(nt_family_process_observer<family>& ppob) noexcept
{
	win32::nt::details::close_nt_user_process_information<family==nt_family::zw>(ppob.hnt_user_process_info);
	ppob.hnt_user_process_info=nullptr;
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

template<nt_family family>
class nt_family_process:public nt_family_process_observer<family>
{
public:
	using native_handle_type = nt_user_process_information;
	constexpr nt_family_process() noexcept = default;
	template<typename native_hd>
	requires ::std::same_as<native_handle_type,::std::remove_cvref_t<native_hd>>
	explicit constexpr nt_family_process(native_hd hd) noexcept:nt_family_process_observer<family>{hd}{}

	template<::fast_io::constructible_to_os_c_str path_type>
	explicit nt_family_process(nt_at_entry nate, path_type const& filename, nt_process_args const& args, nt_process_args const& envs, nt_process_io const& processio) : 
		nt_family_process_observer<family>{win32::nt::details::nt_create_process_overloads<family>(nate, filename, args, envs, processio)} {}

	template<::fast_io::constructible_to_os_c_str path_type>
	explicit nt_family_process(path_type const& filename, nt_process_args const& args, nt_process_args const& envs, nt_process_io const& processio) :
		nt_family_process_observer<family>{win32::nt::details::nt_create_process_overloads<family>(filename, args, envs, processio)}{}

	explicit nt_family_process(::fast_io::nt_fs_dirent ent, nt_process_args const& args, nt_process_args const& envs, nt_process_io const& processio) :
		nt_family_process_observer<family>{win32::nt::details::nt_create_process_overloads<family>(ent, args, envs, processio)}{}

	nt_family_process(nt_family_process const& b)=delete;
	nt_family_process& operator=(nt_family_process const& b)=delete;
	constexpr nt_family_process(nt_family_process&& __restrict b) noexcept:nt_family_process_observer<family>{b.release()}{}
	nt_family_process& operator=(nt_family_process&& __restrict b) noexcept
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

}
