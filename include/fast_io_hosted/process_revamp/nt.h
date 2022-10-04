#pragma once

namespace fast_io
{

struct nt_user_process_information
{
	void* hprocess{};
	void* hthread{};
};

namespace win32::nt::details
{
template<nt_family family>
inline void close_nt_user_process_information_not_null(nt_user_process_information* hnt_user_process_info) noexcept
{
	if(hnt_user_process_info->hthread)[[likely]]
	{
		win32::nt::nt_close<family==nt_family::zw>(hnt_user_process_info->hthread);
		hnt_user_process_info->hthread=nullptr;
	}
	if(hnt_user_process_info->hprocess)[[likely]]
	{
		win32::nt::nt_close<family==nt_family::zw>(hnt_user_process_info->hprocess);
		hnt_user_process_info->hprocess=nullptr;
	}
	delete hnt_user_process_info;
}
template<nt_family family>
inline void close_nt_user_process_information(nt_user_process_information* hnt_user_process_info) noexcept
{
	if(hnt_user_process_info)[[likely]]
		return details::close_nt_user_process_information_not_null<family>(hnt_user_process_info);
}
template<nt_family family>
inline std::uint_least32_t nt_wait_user_process_or_thread(void* hprocess_thread) noexcept
{
	return win32::nt::nt_wait_for_single_object<family==nt_family::zw>(hprocess_thread,false,nullptr);
}

template<nt_family family,bool throw_eh=false>
inline void nt_wait_and_close_user_process_or_thread(void*& handle) noexcept(!throw_eh)
{
	if(handle==nullptr)
		return;
	auto status{nt_wait_user_process_or_thread<family>(handle)};
	auto status2{win32::nt::nt_close<family==nt_family::zw>(handle)};
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
inline void close_nt_user_process_information_and_wait(nt_user_process_information* hnt_user_process_info) noexcept(!throw_eh)
{
	if(hnt_user_process_info)[[likely]]
	{
		nt_wait_and_close_user_process_or_thread<family,throw_eh>(hnt_user_process_info->hthread);
		nt_wait_and_close_user_process_or_thread<family,throw_eh>(hnt_user_process_info->hprocess);
		delete hnt_user_process_info;
	}
}

template<nt_family family>
inline void nt_duplicate_object_std(void* parent_process,void*& standard_io_handle,void* process_handle)
{
	if(standard_io_handle==nullptr)
		return;
	check_nt_status(nt_duplicate_object<family>(parent_process,standard_io_handle,process_handle,__builtin_addressof(standard_io_handle),0,0,0x00000002|0x00000004));
}

inline void check_nt_status(std::uint_least32_t status)
{
	if(status)
		throw_nt_error(status);
}

template<bool zw>
inline void* nt_duplicate_process_std_handle_impl(void* __restrict hprocess,win32_io_redirection const& redi)
{
	void* const current_process{};
	void* ptr{};
	if(redi.win32_handle)[[likely]]
	{
		check_nt_status(nt_duplicate_object<zw>(hprocess,current_process,redi.win32_handle,__builtin_addressof(ptr),0,0,
		/*DUPLICATE_SAME_ACCESS|DUPLICATE_SAME_ATTRIBUTES*/0x00000004|0x00000002));
	}
	return ptr;
}

template<bool zw>
inline void nt_duplicate_process_std_handles_impl(void* __restrict hprocess,win32_process_io const& processio,rtl_user_process_parameters& para)
{
	para.StandardInput=nt_duplicate_process_std_handle_impl<zw>(hprocess,processio.in);
	para.StandardOutput=nt_duplicate_process_std_handle_impl<zw>(hprocess,processio.out);
	para.StandardError=nt_duplicate_process_std_handle_impl<zw>(hprocess,processio.err);
}

/*
Referenced from ReactOS
https://doxygen.reactos.org/d3/d4d/sdk_2lib_2rtl_2process_8c.html
*/

struct
#if __has_cpp_attribute(clang::trivially_relocatable)
[[clang::trivially_relocatable]]
#endif
nt_user_process_information_scoped_ptr
{
	nt_user_process_information* ptr{};
	constexpr nt_user_process_information_scoped_ptr()=default;
	explicit constexpr nt_user_process_information_scoped_ptr(nt_user_process_information* p):ptr(p){}
	nt_user_process_information_scoped_ptr(nt_user_process_information_scoped_ptr const&)=delete;
	nt_user_process_information_scoped_ptr& operator=(nt_user_process_information_scoped_ptr const&)=delete;
	inline constexpr nt_user_process_information& operator*() noexcept
	{
		return *ptr;
	}
	inline constexpr nt_user_process_information const& operator*() const noexcept
	{
		return *ptr;
	}
	inline constexpr nt_user_process_information* release() noexcept
	{
		auto temp{ptr};
		ptr=nullptr;
		return temp;
	}
	constexpr ~nt_user_process_information_scoped_ptr()
	{
		delete ptr;
	}
};

template<nt_family family>
inline nt_user_process_information* nt_process_create_impl(void* __restrict fhandle,win32_process_io const& processio)
{
	constexpr bool zw{family==nt_family::zw};
	nt_user_process_information_scoped_ptr uptr(new nt_user_process_information);
	void* hsection{};
	check_nt_status(nt_create_section<zw>(__builtin_addressof(hsection),0xf001f /*SECTION_ALL_ACCESS*/,
		nullptr,nullptr,0x02 /*PAGE_READONLY*/,0x1000000 /*SEC_IMAGE*/,fhandle));
	basic_nt_family_file<family,char> section(hsection);
	void* const current_process{reinterpret_cast<void*>(static_cast<std::intptr_t>(-1))};
	void* hprocess{};
	check_nt_status(nt_create_process<zw>(__builtin_addressof(hprocess),0x000F0000U|0x00100000U|0xFFF
		/*PROCESS_ALL_ACCESS==(STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0xFFF)*/,
		nullptr,current_process,true,hsection,nullptr,nullptr));
//	println_freestanding(fast_io::win32_stdout(),std::source_location::current());
	basic_nt_family_file<family,char> process(hprocess);
	process_basic_information pb_info{};
	check_nt_status(nt_query_information_process<zw>(hprocess,process_information_class::ProcessBasicInformation,
		__builtin_addressof(pb_info),sizeof(pb_info),nullptr));

//	println_freestanding(fast_io::win32_stdout(),std::source_location::current());
	section_image_information sec_info{};
	check_nt_status(nt_query_section<zw>(hsection,section_information_class::SectionImageInformation,
		__builtin_addressof(sec_info),sizeof(sec_info),nullptr));

	rtl_user_process_parameters rtl_up{};
	nt_duplicate_process_std_handles_impl<zw>(hprocess,processio,rtl_up);
//	check_nt_status(RtlpInitEnvironment(hprocess,pb_info.PebBaseAddress,__builtin_addressof(rtl_up)));
//	println_freestanding(fast_io::win32_stdout(),std::source_location::current());
	void* hthread{};
	client_id cid{};
//	println_freestanding(fast_io::win32_stdout(),std::source_location::current());
	check_nt_status(RtlCreateUserThread(hprocess,nullptr,true,sec_info.ZeroBits,sec_info.MaximumStackSize,
			sec_info.CommittedStackSize,sec_info.TransferAddress,pb_info.PebBaseAddress,__builtin_addressof(hthread),__builtin_addressof(cid)));
	basic_nt_family_file<family,char> thread(hthread);
//	println_freestanding(fast_io::win32_stdout(),std::source_location::current()," ",cid.hprocess," ",cid.hthread);
	std::uint_least32_t lprevcount{};
	check_nt_status(nt_resume_thread<zw>(hthread,__builtin_addressof(lprevcount)));
//	println_freestanding(fast_io::win32_stdout(),std::source_location::current()," ",lprevcount);
	*uptr={process.release(),thread.release()};
	return uptr.release();
}

template<nt_family family,typename path_type>
inline nt_user_process_information* nt_create_process_overloads(nt_at_entry entry,path_type const& filename,win32_process_io const& processio)
{
	basic_nt_family_file<family,char> nf(entry,filename,open_mode::in|open_mode::excl);
	return nt_process_create_impl<family>(nf.handle,processio);
}

template<nt_family family,typename path_type>
inline nt_user_process_information* nt_create_process_overloads(path_type const& filename,win32_process_io const& processio)
{
	basic_nt_family_file<family,char> nf(filename,open_mode::in|open_mode::excl);
	return nt_process_create_impl<family>(nf.handle,processio);
}

template<nt_family family,typename path_type>
inline nt_user_process_information* nt_create_process_overloads(::fast_io::nt_fs_dirent ent,win32_process_io const& processio)
{
	basic_nt_family_file<family,char> nf(ent,open_mode::in|open_mode::excl);
	return nt_process_create_impl<family>(nf.handle,processio);
}

}

template<nt_family family>
requires (family==nt_family::nt||family==nt_family::zw)
class nt_family_process_observer
{
public:
	using native_handle_type = nt_user_process_information*;
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
		return hnt_user_process_info;
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
	std::uint_least32_t exit_code{};
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


#if 0
struct nt_process_args
{
	char const* const* args{};
	bool is_dynamic_allocated{};
	inline constexpr nt_process_args(char const* const* envir) noexcept:args(envir){}
	template<::std::random_access_iterator Iter>
	requires (std::convertible_to<::std::iter_value_t<Iter>,char const*>||requires(::std::iter_value_t<Iter> v)
	{
		{v.c_str()}->std::convertible_to<char const*>;
	})
	inline constexpr nt_process_args(Iter begin,Iter end):
		args(details::dup_enviro_entry(begin,end)),is_dynamic_allocated(true)
	{}
	template<std::ranges::random_access_range range>
	requires (std::convertible_to<std::ranges::range_value_t<range>,char const*>||requires(std::ranges::range_value_t<range> v)
	{
		{v.c_str()}->std::convertible_to<char const*>;
	})
	inline constexpr nt_process_args(range&& rg):nt_process_args(std::ranges::cbegin(rg),std::ranges::cend(rg))
	{}
	inline constexpr nt_process_args(std::initializer_list<char const*> ilist):
		nt_process_args(ilist.begin(),ilist.end()){}
	nt_process_args(nt_process_args const&)=delete;
	nt_process_args& operator=(nt_process_args const&)=delete;
#if __cpp_constexpr_dynamic_alloc >= 201907L
	inline constexpr
#endif
	~nt_process_args()
	{
		if(is_dynamic_allocated)
			delete[] args;
	}
};
#endif

template<nt_family family>
class nt_family_process:public nt_family_process_observer<family>
{
public:
	using native_handle_type = nt_user_process_information*;
	constexpr nt_family_process() noexcept = default;
	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	explicit constexpr nt_family_process(native_hd hd) noexcept:nt_family_process_observer<family>{hd}{}

	template<::fast_io::constructible_to_os_c_str path_type>
	explicit nt_family_process(nt_at_entry nate,path_type const& filename,win32_process_io const& processio):
		nt_family_process_observer<family>{win32::nt::details::nt_create_process_overloads<family>(nate,filename,processio)}
	{}

	template<::fast_io::constructible_to_os_c_str path_type>
	explicit nt_family_process(path_type const& filename,win32_process_io const& processio):
		nt_family_process_observer<family>{win32::nt::details::nt_create_process_overloads<family>(filename,processio)}
	{}

	explicit nt_family_process(::fast_io::nt_fs_dirent ent,win32_process_io const& processio):
		nt_family_process_observer<family>{win32::nt::details::nt_create_process_overloads<family>(ent,processio)}
	{}

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
