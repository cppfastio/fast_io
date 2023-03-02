#pragma once

namespace fast_io
{

namespace details
{

namespace win32
{
/*
Reference from crypto++'s osrng.
Based on information from cryptopp, microsoft patched KB238187 because this api has issues before Windows Server 2012 R2
We use an implementation referenced form crypto++'s implementation.
To avoid name collision, we use itanium abi of name mangling for i686-w64-mingw32-g++ -D_WIN32_WINDOWS=0x0400
_ZN7fast_io7details5win3230crypt_acquire_context_fallbackILNS_12win32_familyE0EEEjv
*/
template<::fast_io::win32_family family>
inline std::uintptr_t crypt_acquire_context_fallback()
{
	std::uintptr_t hprov{};
	if constexpr(family==::fast_io::win32_family::ansi_9x)
	{
		if (!::fast_io::win32::CryptAcquireContextA(__builtin_addressof(hprov), nullptr, nullptr, 0x1, 0xf0000000))
		{
			std::uint_least32_t firsterr{::fast_io::win32::GetLastError()};
			if (!::fast_io::win32::CryptAcquireContextA(__builtin_addressof(hprov), u8"_ZN7fast_io7details5win3230crypt_acquire_context_fallbackILNS_12win32_familyE0EEEjv", nullptr, 0x1, 0x8))
			{
				if(!::fast_io::win32::CryptAcquireContextA(__builtin_addressof(hprov), u8"_ZN7fast_io7details5win3230crypt_acquire_context_fallbackILNS_12win32_familyE0EEEjv", nullptr, 0x1, 0x20|0x8))
				{
					throw_win32_error(firsterr);
				}
			}
		}
	}
	else
	{
		if (!::fast_io::win32::CryptAcquireContextW(__builtin_addressof(hprov), nullptr, nullptr,0x1, 0xf0000000))
		{
			std::uint_least32_t firsterr{::fast_io::win32::GetLastError()};
			if (!::fast_io::win32::CryptAcquireContextW(__builtin_addressof(hprov), u"_ZN7fast_io7details5win3230crypt_acquire_context_fallbackILNS_12win32_familyE0EEEjv", nullptr, 0x1, 0x8))
			{
				if(!::fast_io::win32::CryptAcquireContextW(__builtin_addressof(hprov), u"_ZN7fast_io7details5win3230crypt_acquire_context_fallbackILNS_12win32_familyE0EEEjv", nullptr, 0x1, 0x20|0x8))
				{
					throw_win32_error(firsterr);
				}
			}
		}
	}
	return hprov;
}

template<::fast_io::win32_family family>
struct win32_family_hcryptprov_guard
{
	::std::uintptr_t hprov{};
	win32_family_hcryptprov_guard():hprov{crypt_acquire_context_fallback<family>()}
	{}
	win32_family_hcryptprov_guard(win32_family_hcryptprov_guard const&)=delete;
	win32_family_hcryptprov_guard& operator=(win32_family_hcryptprov_guard const&)=delete;
	~win32_family_hcryptprov_guard()
	{
		::fast_io::win32::CryptReleaseContext(hprov,0);
	}
};

}

template<win32_family family>
inline void* create_win32_temp_file_impl()
{
	constexpr bool is_nt{family==win32_family::wide_nt};
	using char_type = std::conditional_t<is_nt,char16_t,char>;
	using replace_char_type = std::conditional_t<is_nt,char16_t,char8_t>;
	using char_type_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= char_type*;
	using char_type_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= char_type const*;
	constexpr std::uint_least32_t maximum_temp_path_size{261};
	constexpr std::uint_least32_t uuid_size{36};
	constexpr std::uint_least32_t literal_tmp_size{5};
	constexpr std::uint_least32_t total_buffer_size{maximum_temp_path_size+uuid_size+literal_tmp_size};
	replace_char_type arr[total_buffer_size];
	std::uint_least32_t temp_path_size;
	if constexpr(is_nt)
	{
		temp_path_size=::fast_io::win32::GetTempPathW(maximum_temp_path_size,reinterpret_cast<char_type_may_alias_ptr>(arr));
	}
	else
	{
		temp_path_size=::fast_io::win32::GetTempPathA(maximum_temp_path_size,reinterpret_cast<char_type_may_alias_ptr>(arr));
	}
	if(temp_path_size==0)
		throw_win32_error();
	auto arrp{arr+temp_path_size};
	constexpr bool winver_support_rtl_gen_random{
#if !defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x0501
//RtlGenRandom support starts since Windows XP
is_nt
#endif
	};
	::std::conditional_t<winver_support_rtl_gen_random,::fast_io::details::empty,::fast_io::details::win32::win32_family_hcryptprov_guard<family>> hcryptguard;
	for(std::size_t retries{};retries!=100;++retries)//retry 100 times. Or reporting error
	{
		constexpr std::uint_least32_t uuid_buffer_sz{16};
		std::byte uuid_buffer[uuid_buffer_sz];
		if constexpr(winver_support_rtl_gen_random)
		{
			if(!::fast_io::win32::SystemFunction036(uuid_buffer,uuid_buffer_sz))
			{
				throw_win32_error();
			}
		}
		else
		{
			if(!::fast_io::win32::CryptGenRandom(hcryptguard.hprov,uuid_buffer_sz,reinterpret_cast<char unsigned*>(uuid_buffer)))
			{
				throw_win32_error();
			}
		}
		auto p{::fast_io::details::pr_rsv_uuid<false>(arrp,uuid_buffer)};
		if constexpr(is_nt)
		{
			copy_string_literal(u".tmp\0",p);
		}
		else
		{
			copy_string_literal(u8".tmp\0",p);
		}
		void* handle;
		if constexpr(is_nt)
		{
			handle=::fast_io::win32::CreateFileW(reinterpret_cast<char_type_may_alias_const_ptr>(arr),
			0x40000000|0x80000000,	//GENERIC_READ|GENERIC_WRITE
			0,		//Prevents other processes from opening a file or device if they request delete, read, or write access.
			nullptr,
			1,		//CREATE_NEW
			0x100|0x04000000|0x01000000|0x08000000,	//FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE|FILE_FLAG_POSIX_SEMANTICS|FILE_FLAG_SEQUENTIAL_SCAN
			nullptr);
		}
		else
		{
			handle=::fast_io::win32::CreateFileA(reinterpret_cast<char_type_may_alias_const_ptr>(arr),
			0x40000000|0x80000000,	//GENERIC_READ|GENERIC_WRITE
			0,		//Prevents other processes from opening a file or device if they request delete, read, or write access.
			nullptr,
			1,		//CREATE_NEW
			0x100|0x04000000|0x01000000|0x08000000,	//FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE|FILE_FLAG_POSIX_SEMANTICS|FILE_FLAG_SEQUENTIAL_SCAN
			nullptr);
		}
		if(handle==(void*) (std::intptr_t)-1)[[unlikely]]
		{
			std::uint_least32_t code{::fast_io::win32::GetLastError()};
			if(code==80u)	//ERROR_FILE_EXISTS
				continue;
			throw_win32_error(code);
		}
		return handle;
	}
	throw_win32_error(80u);	//ERROR_FILE_EXISTS
}

inline void* create_io_completion_port(void* filehandle,void* existing_completionport,std::uintptr_t completionkey,std::uint_least32_t numberofconcurrentthreads)
{
	void* ptr{fast_io::win32::CreateIoCompletionPort(filehandle,existing_completionport,completionkey,numberofconcurrentthreads)};
	if(ptr==nullptr)[[unlikely]]
		throw_win32_error();
	return ptr;
}

inline void* create_io_completion_port_impl()
{
	return create_io_completion_port(reinterpret_cast<void*>(static_cast<std::uintptr_t>(-1)),nullptr,0,0);
}

struct win32_open_mode
{
::std::uint_least32_t dwDesiredAccess{};
::std::uint_least32_t dwShareMode{};
bool inherit{};
::std::uint_least32_t dwCreationDisposition{};	//depends on EXCL
::std::uint_least32_t dwFlagsAndAttributes{};//=128|0x10000000;//FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS
};

template<win32_family family>
inline void* win32_family_create_file_internal_impl(std::conditional_t<family==win32_family::wide_nt,char16_t,char> const* lpFileName,win32_open_mode const& mode)
{
	if constexpr(family==win32_family::wide_nt)
	{
		::fast_io::win32::security_attributes sec_attr{sizeof(::fast_io::win32::security_attributes),nullptr,true};
		auto handle(::fast_io::win32::CreateFileW(lpFileName,
		mode.dwDesiredAccess,
		mode.dwShareMode,
		mode.inherit?__builtin_addressof(sec_attr):nullptr,
		mode.dwCreationDisposition,
		mode.dwFlagsAndAttributes,
		nullptr));
		if(handle==((void*) (std::intptr_t)-1))
			throw_win32_error();
		return handle;
	}
	else
	{
		auto handle(::fast_io::win32::CreateFileA(lpFileName,
		mode.dwDesiredAccess,
		mode.dwShareMode,
		nullptr,//9x kernel does not support security attributes
		mode.dwCreationDisposition,
		mode.dwFlagsAndAttributes,
		nullptr));
		if(handle==((void*) (std::intptr_t)-1))
			throw_win32_error();
		return handle;
	}
}

inline constexpr win32_open_mode calculate_win32_open_mode(open_mode_perms ompm)
{
	open_mode value{ompm.om};
	perms pm{ompm.pm};
	win32_open_mode mode;
	if((value&open_mode::no_shared_read)==open_mode::none)
		mode.dwShareMode|=1;//FILE_SHARE_READ
	if((value&open_mode::no_shared_write)==open_mode::none)
		mode.dwShareMode|=2;//FILE_SHARE_DELETE
	if((value&open_mode::shared_delete)!=open_mode::none)
		mode.dwShareMode|=4;//FILE_SHARE_WRITE
	if((value&open_mode::app)!=open_mode::none)
		mode.dwDesiredAccess|=4;//FILE_APPEND_DATA
	else if((value&open_mode::out)!=open_mode::none)
		mode.dwDesiredAccess|=0x40000000;//GENERIC_WRITE
	if(((value&open_mode::in)!=open_mode::none)||((value&open_mode::app)!=open_mode::none))
	{
		mode.dwDesiredAccess|=0x80000000;//GENERIC_READ
		if((value&open_mode::out)!=open_mode::none&&((value&open_mode::app)!=open_mode::none&&(value&open_mode::trunc)!=open_mode::none))
			mode.dwDesiredAccess|=0x40000000;//GENERIC_WRITE
	}
/*
Referenced partially from ReactOS
https://github.com/changloong/msvcrt/blob/master/io/wopen.c



https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew

CREATE_ALWAYS
2
Creates a new file, always.
If the specified file exists and is writable, the function overwrites the file, the function succeeds, and last-error code is set to ERROR_ALREADY_EXISTS (183).

If the specified file does not exist and is a valid path, a new file is created, the function succeeds, and the last-error code is set to zero.

For more information, see the Remarks section of this topic.

CREATE_NEW
1
Creates a new file, only if it does not already exist.
If the specified file exists, the function fails and the last-error code is set to ERROR_FILE_EXISTS (80).

If the specified file does not exist and is a valid path to a writable location, a new file is created.

OPEN_ALWAYS
4
Opens a file, always.
If the specified file exists, the function succeeds and the last-error code is set to ERROR_ALREADY_EXISTS (183).

If the specified file does not exist and is a valid path to a writable location, the function creates a file and the last-error code is set to zero.

OPEN_EXISTING
3
Opens a file or device, only if it exists.
If the specified file or device does not exist, the function fails and the last-error code is set to ERROR_FILE_NOT_FOUND (2).

For more information about devices, see the Remarks section.

TRUNCATE_EXISTING
5
Opens a file and truncates it so that its size is zero bytes, only if it exists.
If the specified file does not exist, the function fails and the last-error code is set to ERROR_FILE_NOT_FOUND (2).

The calling process must open the file with the GENERIC_WRITE bit set as part of the dwDesiredAccess parameter.



File access
mode string	Meaning	Explanation	Action if file
already exists	Action if file
does not exist
"r"	read	Open a file for reading	read from start	failure to open
"w"	write	Create a file for writing	destroy contents	create new
"a"	append	Append to a file	write to end	create new
"r+"	read extended	Open a file for read/write	read from start	error
"w+"	write extended	Create a file for read/write	destroy contents	create new
"a+"	append extended	Open a file for read/write	write to end	create new
*/
	if ((value&open_mode::trunc)!=open_mode::none)
	{
		if((value&open_mode::excl)!=open_mode::none)
			mode.dwCreationDisposition=1;// CREATE_NEW
		else
			mode.dwCreationDisposition=2;// CREATE_ALWAYS
	}
	else if((value&open_mode::in)==open_mode::none)
	{
		if((value&open_mode::app)!=open_mode::none)
			mode.dwCreationDisposition=4;//OPEN_ALWAYS
		else if((value&open_mode::out)!=open_mode::none)
		{
			if((value&open_mode::excl)!=open_mode::none)
				mode.dwCreationDisposition=1;// CREATE_NEW
			else
				mode.dwCreationDisposition=2;// CREATE_ALWAYS
		}
	}
	else if((value&open_mode::app)!=open_mode::none)
		mode.dwCreationDisposition=4;//OPEN_ALWAYS
	else
		mode.dwCreationDisposition=3;//OPEN_EXISTING
	if((value&open_mode::creat)!=open_mode::none)
	{
		if((value&open_mode::excl)!=open_mode::none)
			mode.dwCreationDisposition=1;//CREATE_NEW
		else
			mode.dwCreationDisposition=4;//OPEN_ALWAYS
	}
	if((value&open_mode::direct)!=open_mode::none)
		mode.dwFlagsAndAttributes|=0x20000000;//FILE_FLAG_NO_BUFFERING
	if((value&open_mode::sync)!=open_mode::none)
		mode.dwFlagsAndAttributes|=0x80000000;//FILE_FLAG_WRITE_THROUGH
	if((value&open_mode::no_block)!=open_mode::none)
		mode.dwFlagsAndAttributes|=0x40000000;//FILE_FLAG_OVERLAPPED
	if((value&open_mode::follow)!=open_mode::none)
		mode.dwFlagsAndAttributes|=0x00200000;	//FILE_FLAG_OPEN_REPARSE_POINT
/*
Referenced from:
https://stackoverflow.com/questions/4495767/create-a-directory-and-get-the-handle-by-issuing-one-irp

CreateFile actually can create a directory. Set the disposition to CREATE_NEW and the flags to FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_POSIX_SEMANTICS | FILE_ATTRIBUTE_DIRECTORY. Since this isn't documented, it probably isn't implemented in ReactOS and Wine. Note that if you're in a scripting environment such as Python, make sure to use the correct value of FILE_FLAG_POSIX_SEMANTICS from the headers (0x01000000). The value listed on MSDN has always been wrong.
– Eryk Sun Jan 25 '17 at 4:14

I tried this. Oh no. It cannot

*/
	if((value&open_mode::directory)!=open_mode::none)
	{
		mode.dwFlagsAndAttributes|=0x02000000;	//FILE_FLAG_BACKUP_SEMANTICS
		mode.dwFlagsAndAttributes|=0x10;	//FILE_ATTRIBUTE_DIRECTORY
		if(mode.dwCreationDisposition==0)
		{
			mode.dwDesiredAccess|=UINT32_C(0x120116)|UINT32_C(0x120089);	//GENERIC_WRITE|GENERIC_READ
			mode.dwCreationDisposition=3;		//OPEN_EXISTING
		}
	}
	bool set_normal{true};
	if((value&open_mode::archive)!=open_mode::none)
	{
		mode.dwFlagsAndAttributes|=0x20;		//FILE_ATTRIBUTE_ARCHIVE
		set_normal={};
	}
	if((value&open_mode::encrypted)!=open_mode::none)
	{
		mode.dwFlagsAndAttributes|=0x4000;		//FILE_ATTRIBUTE_ENCRYPTED
		set_normal={};
	}
	if((value&open_mode::hidden)!=open_mode::none)
	{
		mode.dwFlagsAndAttributes|=0x2;			//FILE_ATTRIBUTE_HIDDEN
		set_normal={};
	}
	if((value&open_mode::compressed)!=open_mode::none)
	{
		mode.dwFlagsAndAttributes|=0x800;		//FILE_ATTRIBUTE_COMPRESSED
		set_normal={};
	}
	if((value&open_mode::system)!=open_mode::none)
	{
		mode.dwFlagsAndAttributes|=0x4;							//FILE_ATTRIBUTE_SYSTEM
		set_normal={};
	}
	if((value&open_mode::offline)!=open_mode::none)
	{
		mode.dwFlagsAndAttributes|=0x1000;						//FILE_ATTRIBUTE_OFFLINE
		set_normal={};
	}
	if(set_normal)[[likely]]
		mode.dwFlagsAndAttributes|=0x80;						//FILE_ATTRIBUTE_NORMAL
	if((value&open_mode::random_access)==open_mode::none)
	{
		if((value&open_mode::directory)==open_mode::none)
			mode.dwFlagsAndAttributes|=0x10000000;		//FILE_FLAG_SEQUENTIAL_SCAN
	}
	else
		mode.dwFlagsAndAttributes|=0x08000000;		//FILE_FLAG_RANDOM_ACCESS
	if((value&open_mode::no_recall)!=open_mode::none)
		mode.dwFlagsAndAttributes|=0x00100000;					//FILE_FLAG_OPEN_NO_RECALL
	if((value&open_mode::case_insensitive)==open_mode::none)
		mode.dwFlagsAndAttributes|=0x01000000;					//FILE_FLAG_POSIX_SEMANTICS
	if((value&open_mode::session_aware)!=open_mode::none)
		mode.dwFlagsAndAttributes|=0x00800000;					//FILE_FLAG_SESSION_AWARE
	if((value&open_mode::temporary)!=open_mode::none)
	{
		mode.dwFlagsAndAttributes|=0x04000000;					//FILE_FLAG_DELETE_ON_CLOSE
		mode.dwFlagsAndAttributes|=0x100;					//FILE_ATTRIBUTE_TEMPORARY
	}
	if((pm&perms::owner_write)==perms::none)
		mode.dwFlagsAndAttributes|=1;						//FILE_ATTRIBUTE_READONLY
	return mode;
}

template<win32_family family>
inline void* win32_family_create_file_impl(std::conditional_t<family==win32_family::wide_nt,char16_t,char> const* filename_c_str,open_mode_perms ompm)
{
	return win32_family_create_file_internal_impl<family>(filename_c_str,calculate_win32_open_mode(ompm));
}

template<win32_family family>
struct win32_family_open_file_parameter
{
	using family_char_type = std::conditional_t<family==win32_family::wide_nt,char16_t,char>;
	open_mode_perms ompm{};
	inline void* operator()(family_char_type const* filename)
	{
		return win32_family_create_file_impl<family>(filename,ompm);
	}
};

template<win32_family family,typename T>
requires (::fast_io::constructible_to_os_c_str<T>)
inline void* win32_create_file_impl(T const& t,open_mode_perms ompm)
{
	return ::fast_io::win32_family_api_common<family>(t,win32_family_open_file_parameter<family>{ompm});
}

template<win32_family,typename T>
requires (::fast_io::constructible_to_os_c_str<T>)
inline void* win32_create_file_at_impl(void* directory_handle,T const& t,open_mode_perms ompm)
{
	return ::fast_io::win32::nt::details::nt_create_file_at_impl<false>(directory_handle,t,ompm);
}

template<win32_family,std::integral char_type>
inline void* win32_create_file_at_fs_dirent_impl(void* directory_handle,char_type const* filename_c_str,std::size_t filename_c_str_len,open_mode_perms ompm)
{
	return ::fast_io::win32::nt::details::nt_family_create_file_fs_dirent_impl<false>(directory_handle,filename_c_str,filename_c_str_len,ompm);
}

}

struct win32_io_redirection
{
	void *win32_pipe_in_handle{};
	void *win32_pipe_out_handle{};
	void *win32_handle{};
	bool is_dev_null{};
};

struct win32_io_redirection_std:win32_io_redirection
{
	constexpr win32_io_redirection_std() noexcept=default;
	template<typename T>
	requires requires(T&& t)
	{
		{redirect(::std::forward<T>(t))}->std::same_as<win32_io_redirection>;
	}
	constexpr win32_io_redirection_std(T&& t) noexcept:win32_io_redirection(redirect(::std::forward<T>(t))){}
};

struct win32_process_io
{
	win32_io_redirection_std in;
	win32_io_redirection_std out;
	win32_io_redirection_std err;
};

template<win32_family family,std::integral ch_type>
class basic_win32_family_io_observer
{
public:
	using native_handle_type = void*;
	using char_type = ch_type;
	native_handle_type handle{};
	constexpr native_handle_type native_handle() const noexcept
	{
		return handle;
	}
	explicit constexpr operator bool() const noexcept
	{
		return handle!=nullptr;
	}
	template<nt_family family2>
	explicit constexpr operator basic_nt_family_io_observer<family2,char_type>() const noexcept
	{
		return basic_nt_family_io_observer<family2,char_type>{handle};
	}
	template<win32_family family2>
	explicit constexpr operator basic_win32_family_io_observer<family2,char_type>() const noexcept
	{
		return basic_win32_family_io_observer<family2,char_type>{handle};
	}
	constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle=nullptr;
		return temp;
	}
};

#if __cpp_lib_three_way_comparison >= 201907L

template<win32_family family,std::integral ch_type>
inline constexpr bool operator==(basic_win32_family_io_observer<family,ch_type> a,basic_win32_family_io_observer<family,ch_type> b) noexcept
{
	return a.handle==b.handle;
}

template<win32_family family,std::integral ch_type>
inline constexpr auto operator<=>(basic_win32_family_io_observer<family,ch_type> a,basic_win32_family_io_observer<family,ch_type> b) noexcept
{
	return a.handle<=>b.handle;
}

#endif

template<win32_family family,std::integral ch_type>
inline constexpr nt_at_entry at(basic_win32_family_io_observer<family,ch_type> wiob) noexcept
{
	return nt_at_entry{wiob.handle};
}

template<win32_family family,std::integral ch_type>
inline constexpr basic_win32_family_io_observer<family,ch_type> io_value_handle(basic_win32_family_io_observer<family,ch_type> other) noexcept
{
	return other;
}

template<win32_family family,std::integral ch_type>
inline constexpr win32_io_redirection redirect(basic_win32_family_io_observer<family,ch_type> other) noexcept
{
	return {.win32_handle=other.handle};
}

namespace win32::details
{

inline void* win32_dup_impl(void* handle)
{
	void* current_process{reinterpret_cast<void*>(static_cast<intptr_t>(-1))};
	void* new_handle{};
	if(!DuplicateHandle(current_process,handle,current_process,__builtin_addressof(new_handle), 0, true, 2/*DUPLICATE_SAME_ACCESS*/))
		throw_win32_error();
	return handle;
}

inline void* win32_dup2_impl(void* handle,void* newhandle)
{
	auto temp{win32_dup_impl(handle)};
	if(newhandle)[[likely]]
		CloseHandle(newhandle);
	return temp;
}

inline void win32_flush_impl(void* __restrict handle)
{
	if(!FlushFileBuffers(handle))
		throw_win32_error();
}

inline void win32_data_sync_impl(void* __restrict handle,data_sync_flags flags [[maybe_unused]])
{
#if defined(_WIN32_WINDOWS)
	win32_flush_impl(handle);
#else
	::fast_io::win32::nt::details::nt_data_sync_impl<false>(handle,flags);
#endif
}

}

template<win32_family family,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#endif
inline void flush(basic_win32_family_io_observer<family,char_type> wiob)
{
	::fast_io::win32::details::win32_flush_impl(wiob.handle);
}

template<win32_family family,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#endif
inline void data_sync(basic_win32_family_io_observer<family,char_type> wiob,data_sync_flags flags)
{
	::fast_io::win32::details::win32_data_sync_impl(wiob.handle,flags);
}

namespace win32::details
{

inline std::size_t read_impl(void* __restrict handle,void* __restrict begin,std::size_t to_read)
{
	std::uint_least32_t number_of_bytes_read{};
	if constexpr(4<sizeof(std::size_t))
		if(static_cast<std::size_t>(UINT_LEAST32_MAX)<to_read)
			to_read=static_cast<std::size_t>(UINT_LEAST32_MAX);
	if(!win32::ReadFile(handle,begin,static_cast<std::uint_least32_t>(to_read),__builtin_addressof(number_of_bytes_read),nullptr))
	{
		auto err(win32::GetLastError());
		if(err==109)
			return 0;
		throw_win32_error(err);
	}
	return number_of_bytes_read;
}

inline std::size_t pread_impl(void* __restrict handle,void* __restrict begin,std::size_t to_read,std::uintmax_t u64off)
{
	std::uint_least32_t number_of_bytes_read{};
	if constexpr(4<sizeof(std::size_t))
		if(static_cast<std::size_t>(UINT_LEAST32_MAX)<to_read)
			to_read=static_cast<std::size_t>(UINT_LEAST32_MAX);
	win32::overlapped overlap{};
	overlap.dummy_union_name.dummy_struct_name={static_cast<std::uint_least32_t>(u64off),static_cast<std::uint_least32_t>(u64off>>32)};
	if(!win32::ReadFile(handle,begin,static_cast<std::uint_least32_t>(to_read),__builtin_addressof(number_of_bytes_read),__builtin_addressof(overlap)))
	{
		auto err(win32::GetLastError());
		if(err==109)
			return 0;
		throw_win32_error(err);
	}
	return number_of_bytes_read;
}

inline io_scatter_status_t scatter_read_impl(void* __restrict handle,io_scatter_t const* scatters,std::size_t n)
{
	std::size_t total_size{};
	for(std::size_t i{};i!=n;++i)
	{
		std::size_t pos_in_span{read_impl(handle,const_cast<void*>(scatters[i].base),scatters[i].len)};
		total_size+=pos_in_span;
		if(pos_in_span<scatters[i].len)[[unlikely]]
			return {total_size,i,pos_in_span};
	}
	return {total_size,n,0};
}

inline std::uint_least32_t write_simple_impl(void* __restrict handle,void const* __restrict cbegin,std::size_t to_write)
{
	std::uint_least32_t number_of_bytes_written{};
	if(!win32::WriteFile(handle,cbegin,static_cast<std::uint_least32_t>(to_write),__builtin_addressof(number_of_bytes_written),nullptr))
		throw_win32_error();
	return number_of_bytes_written;
}

inline io_scatter_status_t scatter_pread_impl(void* __restrict handle,io_scatters_t sp,std::uintmax_t offset)
{
	std::size_t total_size{};
	for(std::size_t i{};i!=sp.len;++i)
	{
		std::size_t pos_in_span{pread_impl(handle,const_cast<void*>(sp.base[i].base),sp.base[i].len,offset)};
		total_size+=pos_in_span;
		offset+=pos_in_span;
		if(pos_in_span<sp.base[i].len)[[unlikely]]
			return {total_size,i,pos_in_span};
	}
	return {total_size,sp.len,0};
}

inline std::uint_least32_t pwrite_simple_impl(void* __restrict handle,void const* __restrict cbegin,std::size_t to_write,std::uintmax_t offset)
{
	std::uint_least64_t u64off(static_cast<std::uint_least64_t>(offset));
	if constexpr(sizeof(std::uintmax_t)>sizeof(std::uint_least64_t))
	{
		if(static_cast<std::uintmax_t>(std::numeric_limits<std::uint_least64_t>::max())<offset)
			throw_win32_error(0x00000057);
	}
	std::uint_least32_t number_of_bytes_written{};
	win32::overlapped overlap{};
	overlap.dummy_union_name.dummy_struct_name={static_cast<std::uint_least32_t>(u64off),static_cast<std::uint_least32_t>(u64off>>32)};
	if(!win32::WriteFile(handle,cbegin,static_cast<std::uint_least32_t>(to_write),__builtin_addressof(number_of_bytes_written),__builtin_addressof(overlap)))
		throw_win32_error();
	return number_of_bytes_written;
}

inline std::size_t pwrite_impl(void* __restrict handle,void const* __restrict cbegin,std::size_t to_write,std::uintmax_t offset)
{
	if constexpr(4<sizeof(std::size_t))
	{
		std::size_t written{};
		for(;to_write;)
		{
			std::uint_least32_t to_write_this_round{UINT_LEAST32_MAX};
			if(to_write<static_cast<std::size_t>(UINT_LEAST32_MAX))
				to_write_this_round=static_cast<std::uint_least32_t>(to_write);
			std::uint_least32_t number_of_bytes_written{pwrite_simple_impl(handle,cbegin,to_write_this_round,offset)};
			written+=number_of_bytes_written;
			offset+=number_of_bytes_written;
			if(number_of_bytes_written<to_write_this_round)
				break;
			to_write-=to_write_this_round;
		}
		return written;
	}
	else
		return pwrite_simple_impl(handle,cbegin,to_write,offset);
}

inline io_scatter_status_t scatter_pwrite_impl(void* __restrict handle,io_scatters_t sp,std::uintmax_t offset)
{
	std::size_t total_size{};
	for(std::size_t i{};i!=sp.len;++i)
	{
		std::size_t written{pwrite_impl(handle,sp.base[i].base,sp.base[i].len,offset)};
		total_size+=written;
		offset+=written;
		if(sp.base[i].len<written)[[unlikely]]
			return {total_size,i,written};
	}
	return {total_size,sp.len,0};
}

inline std::size_t write_nolock_impl(void* __restrict handle,void const* __restrict cbegin,std::size_t to_write)
{
	if constexpr(4<sizeof(std::size_t))		//above the size of std::uint_least32_t, unfortunately, we cannot guarantee the atomicity of syscall
	{
		std::size_t written{};
		for(;to_write;)
		{
			std::uint_least32_t to_write_this_round{UINT_LEAST32_MAX};
			if(to_write<static_cast<std::size_t>(UINT_LEAST32_MAX))
				to_write_this_round=static_cast<std::uint_least32_t>(to_write);
			std::uint_least32_t number_of_bytes_written{write_simple_impl(handle,cbegin,to_write_this_round)};
			written+=number_of_bytes_written;
			if(number_of_bytes_written<to_write_this_round)
				break;
			to_write-=to_write_this_round;
		}
		return written;
	}
	else
	{
		return write_simple_impl(handle,cbegin,to_write);
	}
}

inline std::size_t write_impl(void* __restrict handle,void const* __restrict cbegin,std::size_t to_write)
{
	if constexpr(4<sizeof(std::size_t))
	{
		return write_nolock_impl(handle,cbegin,to_write);
	}
	else
		return write_simple_impl(handle,cbegin,to_write);
}

inline std::uintmax_t seek_impl(void* handle,std::intmax_t offset,seekdir s)
{
#if (defined(_WIN32_WINNT)&&_WIN32_WINNT <= 0x0500) || defined(_WIN32_WINDOWS)
	if constexpr(sizeof(std::intmax_t)>sizeof(std::int_least32_t))
	{
		constexpr std::intmax_t l32mx{INT_LEAST32_MAX};
		constexpr std::intmax_t l32mn{INT_LEAST32_MIN};
		if(offset>l32mx||offset<l32mn)
			throw_win32_error(0x00000057);
	}
	std::int_least32_t distance_to_move_high{};
	constexpr std::uint_least32_t invalid{UINT_LEAST32_MAX};
	if(win32::SetFilePointer(handle,static_cast<std::int_least32_t>(offset),__builtin_addressof(distance_to_move_high),static_cast<std::uint_least32_t>(s))==invalid)
	{
		throw_win32_error();
	}
	return static_cast<std::uintmax_t>(static_cast<std::uint_least32_t>(distance_to_move_high));
#else
	if constexpr(sizeof(std::intmax_t)>sizeof(std::int_least64_t))
	{
		constexpr std::intmax_t l64mx{INT_LEAST64_MAX};
		constexpr std::intmax_t l64mn{INT_LEAST64_MIN};
		if(offset>l64mx||offset<l64mn)
			throw_win32_error(0x00000057);
	}
	std::int_least64_t distance_to_move_high{};
	if(!win32::SetFilePointerEx(handle,static_cast<std::int_least64_t>(offset),__builtin_addressof(distance_to_move_high),static_cast<std::uint_least32_t>(s)))
	{
		throw_win32_error();
	}
	return static_cast<std::uintmax_t>(static_cast<std::uint_least64_t>(distance_to_move_high));
#endif
}

inline io_scatter_status_t scatter_write_impl(void* __restrict handle,io_scatter_t const* scatters,std::size_t n)
{
	std::size_t total_size{};
	for(std::size_t i{};i!=n;++i)
	{
		std::size_t written{write_nolock_impl(handle,scatters[i].base,scatters[i].len)};
		total_size+=written;
		if(scatters[i].len<written)[[unlikely]]
			return {total_size,i,written};
	}
	return {total_size,n,0};
}

}

template<win32_family family,std::integral ch_type>
inline std::uintmax_t seek(basic_win32_family_io_observer<family,ch_type> handle,std::intmax_t offset=0,seekdir s=seekdir::cur)
{
	return win32::details::seek_impl(handle.handle,offset,s);
}

template<win32_family family,std::integral ch_type,::std::contiguous_iterator Iter>
[[nodiscard]] inline Iter read(basic_win32_family_io_observer<family,ch_type> handle,Iter begin,Iter end)
{
	return begin+win32::details::read_impl(handle.handle,::std::to_address(begin),static_cast<std::size_t>(end-begin)*sizeof(*begin))/sizeof(*begin);
}

template<win32_family family,std::integral ch_type,::std::contiguous_iterator Iter>
inline Iter write(basic_win32_family_io_observer<family,ch_type> handle,Iter cbegin,Iter cend)
{
	return cbegin+win32::details::write_impl(handle.handle,::std::to_address(cbegin),static_cast<std::size_t>(cend-cbegin)*sizeof(*cbegin))/sizeof(*cbegin);
}

template<win32_family family,std::integral ch_type>
inline io_scatter_status_t scatter_read(basic_win32_family_io_observer<family,ch_type> handle,io_scatters_t sp)
{
	return win32::details::scatter_read_impl(handle.handle,sp.base,sp.len);
}

template<win32_family family,std::integral ch_type>
inline io_scatter_status_t scatter_write(basic_win32_family_io_observer<family,ch_type> handle,io_scatters_t sp)
{
	return win32::details::scatter_write_impl(handle.handle,sp.base,sp.len);
}

template<win32_family family,::std::integral char_type>
inline constexpr ::std::conditional_t<family==win32_family::ansi_9x,nop_file_lock,nt_file_lock> file_lock(basic_win32_family_io_observer<family,char_type> wiob) noexcept
{
	if constexpr(family==win32_family::ansi_9x)
	{
		return {};
	}
	else
	{
		return {wiob.handle};
	}
}

#if 0
namespace details
{

struct iocp_overlapped_base:public ::fast_io::win32::overlapped
{
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	virtual void invoke(std::size_t) = 0;
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	virtual ~iocp_overlapped_base() noexcept=default;
};

template<typename Func>
struct iocp_overlapped_derived:iocp_overlapped_base
{
	Func func;
	template<typename... Args>
	requires std::constructible_from<Func,Args...>
	iocp_overlapped_derived(Args&& ...args):func(::std::forward<Args>(args)...){}
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	void invoke(std::size_t value) override
	{
		func(value);
	}
};

inline void iocp_async_write_define_impl(void* handle,void const* data,std::size_t to_write,std::ptrdiff_t offset,iocp_overlapped_base* callback)
{

	if constexpr(4<sizeof(std::size_t))
		if(static_cast<std::size_t>(UINT_LEAST32_MAX)<to_write)
			to_write=static_cast<std::size_t>(UINT_LEAST32_MAX);
	if constexpr(4<sizeof(std::size_t))
	{
		callback->dummy_union_name.dummy_struct_name.Offset=static_cast<std::size_t>(offset)&std::numeric_limits<std::uint_least32_t>::max();
		callback->dummy_union_name.dummy_struct_name.OffsetHigh=static_cast<std::size_t>(offset)>>static_cast<std::size_t>(32);
	}
	else
	{
		callback->dummy_union_name.dummy_struct_name.Offset=static_cast<std::uint_least32_t>(offset);
		callback->dummy_union_name.dummy_struct_name.OffsetHigh=0;
	}
	if(!win32::WriteFile(handle,data,static_cast<std::uint_least32_t>(to_write),nullptr,callback))[[likely]]
	{
		auto err(win32::GetLastError());
		if(err==997)[[likely]]
			return;
		delete callback;
		throw_win32_error(err);
	}
}

template<typename Func>
inline void iocp_async_write_define(void* handle,void const* data,std::size_t bytes,std::ptrdiff_t offset,Func callback)
{
	iocp_async_write_define_impl(handle,data,bytes,offset,new iocp_overlapped_derived<Func>{callback});
}

inline void iocp_set_async_context(void* iocp_handle,void* filehandle)
{
	if(iocp_handle==nullptr||filehandle==nullptr)
		throw_win32_error(0x000000A0);
	create_io_completion_port(filehandle,iocp_handle,0,0);
}

inline void iocp_async_wait_impl(void* handle)
{
	std::uint_least32_t transferred{};
	std::uintptr_t completionkey{};
	::fast_io::win32::overlapped *over{};
	if(!::fast_io::win32::GetQueuedCompletionStatus(handle,__builtin_addressof(transferred),__builtin_addressof(completionkey),
		over,std::numeric_limits<std::uint_least32_t>::max()))
		throw_win32_error();
	static_cast<iocp_overlapped_base*>(over)->invoke(static_cast<std::size_t>(transferred));
}

}

template<win32_family fam,typename Func>
inline void async_write_define(basic_win32_family_io_observer<fam,char>,nt_at_entry h,void const* data,std::size_t bytes,std::ptrdiff_t offset,Func callback)
{
	::fast_io::details::iocp_async_write_define(h.handle,data,bytes,offset,std::move(callback));
}

template<win32_family fam>
inline void set_async_context(basic_win32_family_io_observer<fam,char> iocp,nt_at_entry h)
{
	::fast_io::details::iocp_set_async_context(iocp.handle,h.handle);
}

template<win32_family fam>
inline void io_async_wait(basic_win32_family_io_observer<fam,char> iocp)
{
	::fast_io::details::iocp_async_wait_impl(iocp.handle);
}
#endif

template<win32_family family,std::integral ch_type>
inline void cancel(basic_win32_family_io_observer<family,ch_type> h)
{
	if(!fast_io::win32::CancelIo(h.handle))
		throw_win32_error();
}

template<win32_family family,std::integral ch_type,typename... Args>
requires requires(basic_win32_family_io_observer<family,ch_type> h,Args&& ...args)
{
	fast_io::win32::DeviceIoControl(h.handle,::std::forward<Args>(args)...);
}
inline void io_control(basic_win32_family_io_observer<family,ch_type> h,Args&& ...args)
{
	if(!fast_io::win32::DeviceIoControl(h.handle,::std::forward<Args>(args)...))
		throw_win32_error();
}

struct
#if __has_cpp_attribute(clang::trivially_relocatable)
[[clang::trivially_relocatable]]
#endif
win32_file_factory
{
	using native_handle_type=void*;
	void* handle{};
	explicit constexpr win32_file_factory(void* hd) noexcept:handle(hd){};
	win32_file_factory(win32_file_factory const&)=delete;
	win32_file_factory& operator=(win32_file_factory const&)=delete;
	~win32_file_factory()
	{
		if(handle)[[likely]]
			fast_io::win32::CloseHandle(handle);
	}
};

template<win32_family family,std::integral ch_type>
class basic_win32_family_file:public basic_win32_family_io_observer<family,ch_type>
{
public:
	using typename basic_win32_family_io_observer<family,ch_type>::char_type;
	using typename basic_win32_family_io_observer<family,ch_type>::native_handle_type;
	using basic_win32_family_io_observer<family,ch_type>::native_handle;
	using file_factory_type = win32_file_factory;
	explicit constexpr basic_win32_family_file() noexcept=default;

	constexpr basic_win32_family_file(basic_win32_family_io_observer<family,ch_type>) noexcept=delete;
	constexpr basic_win32_family_file& operator=(basic_win32_family_io_observer<family,ch_type>) noexcept=delete;

	basic_win32_family_file(basic_win32_family_file const& other):basic_win32_family_io_observer<family,ch_type>{win32::details::win32_dup_impl(other.handle)}{}
	basic_win32_family_file& operator=(basic_win32_family_file const& other)
	{
		this->handle=win32::details::win32_dup2_impl(other.handle,this->handle);
		return *this;
	}
	basic_win32_family_file(basic_win32_family_file&& __restrict b) noexcept:
		basic_win32_family_io_observer<family,ch_type>{b.release()}
	{}
	basic_win32_family_file& operator=(basic_win32_family_file&& __restrict b) noexcept
	{
		if(this->handle)[[likely]]
			::fast_io::win32::CloseHandle(this->handle);
		this->handle = b.handle;
		b.handle=nullptr;
		return *this;
	}
	void reset(native_handle_type newhandle=nullptr) noexcept
	{
		if(this->handle)[[likely]]
			::fast_io::win32::CloseHandle(this->handle);
		this->handle=newhandle;
	}
	void close()
	{
		if(this->handle)[[likely]]
		{
			auto error{::fast_io::win32::CloseHandle(this->handle)};
			this->handle=nullptr;//POSIX standard says we should never call close(2) again even close syscall fails
			if(!error)[[unlikely]]
				throw_win32_error();
		}
	}

	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	explicit constexpr basic_win32_family_file(native_hd handle1) noexcept:basic_win32_family_io_observer<family,ch_type>{handle1}{}

	basic_win32_family_file(io_dup_t,basic_win32_family_io_observer<family,ch_type> wiob):basic_win32_family_io_observer<family,ch_type>{win32::details::win32_dup_impl(wiob.handle)}
	{}

	explicit constexpr basic_win32_family_file(win32_file_factory&& fact) noexcept: basic_win32_family_io_observer<family,ch_type>{fact.handle}
	{
		fact.handle=nullptr;
	}
	explicit constexpr basic_win32_family_file(decltype(nullptr)) noexcept = delete;
	explicit basic_win32_family_file(io_temp_t):basic_win32_family_io_observer<family,char_type>{::fast_io::details::create_win32_temp_file_impl<family>()}{}
	explicit basic_win32_family_file(nt_fs_dirent fsdirent,open_mode om,perms pm=static_cast<perms>(436)):
		basic_win32_family_io_observer<family,char_type>{::fast_io::details::win32_create_file_at_fs_dirent_impl<family>(fsdirent.handle,fsdirent.filename.c_str(),fsdirent.filename.size(),{om,pm})}{}

	template<::fast_io::constructible_to_os_c_str T>
	explicit basic_win32_family_file(T const& filename,open_mode om,perms pm=static_cast<perms>(436)):
			basic_win32_family_io_observer<family,char_type>{::fast_io::details::win32_create_file_impl<family>(filename,{om,pm})}
	{}

	template<::fast_io::constructible_to_os_c_str T>
	explicit basic_win32_family_file(nt_at_entry nate,T const& filename,open_mode om,perms pm=static_cast<perms>(436)):
			basic_win32_family_io_observer<family,char_type>{::fast_io::details::win32_create_file_at_impl<family>(nate.handle,filename,{om,pm})}
	{}

	explicit basic_win32_family_file(io_async_t) requires(std::same_as<char_type,char>):basic_win32_family_io_observer<family,char_type>{details::create_io_completion_port_impl()}
	{
	}
	~basic_win32_family_file()
	{
		if(this->handle)[[likely]]
			::fast_io::win32::CloseHandle(this->handle);
	}
};

template<win32_family family,std::integral ch_type>
inline void truncate(basic_win32_family_io_observer<family,ch_type> handle,std::uintmax_t size)
{
	seek(handle,size,seekdir::beg);
	if(!win32::SetEndOfFile(handle.handle))
		throw_win32_error();
}

template<win32_family family,std::integral char_type,::std::contiguous_iterator Iter>
inline constexpr Iter pwrite(basic_win32_family_io_observer<family,char_type> wpioent,Iter begin,Iter end,std::intmax_t offset)
{
	return begin+win32::details::pwrite_impl(wpioent.handle,::std::to_address(begin),(end-begin)*sizeof(*begin),offset)/sizeof(*begin);
}

template<win32_family family,std::integral ch_type>
inline io_scatter_status_t scatter_pwrite(basic_win32_family_io_observer<family,ch_type> wpioent,io_scatters_t sp,std::intmax_t offset)
{
	return win32::details::scatter_pwrite_impl(wpioent.handle,sp,offset);
}

template<win32_family family,std::integral char_type,::std::contiguous_iterator Iter>
inline constexpr Iter pread(basic_win32_family_io_observer<family,char_type> wpioent,Iter begin,Iter end,std::intmax_t offset)
{
	return begin+win32::details::pread_impl(wpioent.handle,::std::to_address(begin),(end-begin)*sizeof(*begin),offset)/sizeof(*begin);
}

template<win32_family family,std::integral ch_type>
inline io_scatter_status_t scatter_pread(basic_win32_family_io_observer<family,ch_type> wpioent,io_scatters_t sp,std::intmax_t offset)
{
	return win32::details::scatter_pread_impl(wpioent.handle,sp,offset);
}

namespace win32::details
{

inline file_type file_type_impl(void* handle)
{
/*
https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfiletype
*/
	std::uint_least32_t ftvalue{GetFileType(handle)};
	switch(ftvalue)
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
		auto errcd=GetLastError();
		if(errcd==0)
			return file_type::unknown;
		throw_win32_error(errcd);
		[[fallthrough]];
	}
	default:
		return file_type::unknown;
	};
}

inline posix_file_status win32_status_impl(void* __restrict handle)
{
	file_type ft{file_type_impl(handle)};
	if(ft==file_type::fifo||ft==file_type::character)
		return posix_file_status{0,0,static_cast<perms>(436),ft,1,0,0,
			static_cast<std::uintmax_t>(reinterpret_cast<std::uintptr_t>(handle)),
			0,131072,0,{},{},{},{},0,0};
	by_handle_file_information bhdi;
	if(!GetFileInformationByHandle(handle,__builtin_addressof(bhdi)))
		throw_win32_error();
	std::uintmax_t file_size{static_cast<std::uintmax_t>((static_cast<std::uint_least64_t>(bhdi.nFileSizeHigh)<<32)|bhdi.nFileSizeLow)};
	std::underlying_type_t<perms> pm{0444};
	if((bhdi.dwFileAttributes&0x1)==0x0)
		pm|=0222;
	if((bhdi.dwFileAttributes&0x400)==0x400)
		ft=file_type::symlink;
	else if((bhdi.dwFileAttributes&0x10)==0x10)
		ft=file_type::directory;
	return posix_file_status{static_cast<std::uintmax_t>(bhdi.dwVolumeSerialNumber),
	static_cast<std::uintmax_t>((static_cast<std::uint_least64_t>(bhdi.nFileIndexHigh)<<32)|bhdi.nFileIndexLow),
	static_cast<perms>(pm),
	ft,
	static_cast<std::uintmax_t>(bhdi.nNumberOfLinks),
	0,0,0,
	file_size,
	131072,file_size>>9,
	to_unix_timestamp(bhdi.ftLastAccessTime),
	to_unix_timestamp(bhdi.ftLastWriteTime),
	to_unix_timestamp(bhdi.ftLastWriteTime),
	to_unix_timestamp(bhdi.ftCreationTime),
	0,0};
}


/*
Thanks Fseuio for providing source code.
*/
inline bool win32_is_character_device(void* handle) noexcept
{
	return (win32::GetFileType(handle) & 0xFFFF7FFF) ==2;
}

struct win32_console_mode_guard
{
	void *out_hdl{};
	std::uint_least32_t mode{};
	win32_console_mode_guard(void* hd):out_hdl{hd}
	{
		if(!GetConsoleMode(out_hdl,__builtin_addressof(mode)))
			throw_win32_error();
		if(!SetConsoleMode(out_hdl,mode|0x0004))
			throw_win32_error();
	}
	win32_console_mode_guard(win32_console_mode_guard const&)=delete;
	win32_console_mode_guard& operator=(win32_console_mode_guard const&)=delete;
	~win32_console_mode_guard()
	{
		SetConsoleMode(out_hdl,mode);
	}
};

inline void win32_clear_screen_main(void* out_hdl)
{
/*
Since many people are using console like msys2, we need to first write something to this console
*/
	constexpr char8_t const str[] = u8"\x1B[H\x1B[2J\x1B[3J";
	constexpr std::uint_least32_t written_chars{static_cast<std::uint_least32_t>(::fast_io::details::string_literal_size(str))};
//not bytes, but chars
	win32_console_mode_guard guard{out_hdl};
	if(!win32::WriteFile(out_hdl, str, written_chars, nullptr, nullptr))
		throw_win32_error();
}


inline void win32_clear_screen_impl(void* handle)
{
	if(!win32_is_character_device(handle))
		return;
	win32_clear_screen_main(handle);
}

}

template<win32_family family,std::integral ch_type>
inline posix_file_status status(basic_win32_family_io_observer<family,ch_type> wiob)
{
	return win32::details::win32_status_impl(wiob.handle);
}

template<win32_family family,std::integral ch_type>
class basic_win32_family_pipe
{
public:
	using char_type = ch_type;
	basic_win32_family_file<family,ch_type> pipes[2];
	basic_win32_family_pipe()
	{
		win32::security_attributes sec_attr{sizeof(win32::security_attributes),nullptr,true};
		if(!win32::CreatePipe(
			__builtin_addressof(pipes.front().handle),
			__builtin_addressof(pipes.back().handle),
			__builtin_addressof(sec_attr),0))
			throw_win32_error();
	}
	constexpr auto& in() noexcept
	{
		return *pipes;
	}
	constexpr auto& out() noexcept
	{
		return pipes[1];
	}
};

template<win32_family family,std::integral ch_type,::std::contiguous_iterator Iter>
inline Iter read(basic_win32_family_pipe<family,ch_type>& h,Iter begin,Iter end)
{
	return read(h.in(),begin,end);
}

template<win32_family family,std::integral ch_type,::std::contiguous_iterator Iter>
inline Iter write(basic_win32_family_pipe<family,ch_type>& h,Iter begin,Iter end)
{
	return write(h.out(),begin,end);
}

template<win32_family family,std::integral ch_type>
inline io_scatter_status_t scatter_read(basic_win32_family_pipe<family,ch_type>& h,io_scatters_t sp)
{
	return scatter_read(h.in(),sp);
}

template<win32_family family,std::integral ch_type>
inline io_scatter_status_t scatter_write(basic_win32_family_pipe<family,ch_type>& h,io_scatters_t sp)
{
	return scatter_write(h.out(),sp);
}

template<win32_family family,std::integral ch_type>
inline ::fast_io::freestanding::array<void*,2> redirect(basic_win32_family_pipe<family,ch_type>& hd)
{
	return {hd.in().handle,hd.out().handle};
}

template<win32_family family,std::integral ch_type>
inline void clear_screen(basic_win32_family_io_observer<family,ch_type> wiob)
{
	win32::details::win32_clear_screen_impl(wiob.handle);
}

template<win32_family family,std::integral ch_type>
inline bool is_character_device(basic_win32_family_io_observer<family,ch_type> wiob) noexcept
{
	return win32::details::win32_is_character_device(wiob.handle);
}

template<nt_family family,std::integral ch_type>
inline void clear_screen(basic_nt_family_io_observer<family,ch_type> niob)
{
	win32::details::win32_clear_screen_impl(niob.handle);
}

template<nt_family family,std::integral ch_type>
inline bool is_character_device(basic_nt_family_io_observer<family,ch_type> niob) noexcept
{
	return win32::details::win32_is_character_device(niob.handle);
}

template<win32_family family,std::integral char_type>
inline void flush(basic_win32_family_pipe<family,char_type>& pipe)
{
	::fast_io::win32::details::win32_flush_impl(pipe.out().handle);
	::fast_io::win32::details::win32_flush_impl(pipe.in().handle);
}

template<std::integral char_type>
using basic_win32_io_observer_9xa = basic_win32_family_io_observer<win32_family::ansi_9x,char_type>;
template<std::integral char_type>
using basic_win32_file_9xa = basic_win32_family_file<win32_family::ansi_9x,char_type>;
template<std::integral char_type>
using basic_win32_pipe_9xa = basic_win32_family_pipe<win32_family::ansi_9x,char_type>;

using win32_io_observer_9xa=basic_win32_io_observer_9xa<char>;
using win32_file_9xa=basic_win32_file_9xa<char>;
using win32_pipe_9xa=basic_win32_pipe_9xa<char>;

using wwin32_io_observer_9xa=basic_win32_io_observer_9xa<wchar_t>;
using wwin32_file_9xa=basic_win32_file_9xa<wchar_t>;
using wwin32_pipe_9xa=basic_win32_pipe_9xa<wchar_t>;

using u8win32_io_observer_9xa=basic_win32_io_observer_9xa<char8_t>;
using u8win32_file_9xa=basic_win32_file_9xa<char8_t>;
using u8win32_pipe_9xa=basic_win32_pipe_9xa<char8_t>;

using u16win32_io_observer_9xa=basic_win32_io_observer_9xa<char16_t>;
using u16win32_file_9xa=basic_win32_file_9xa<char16_t>;
using u16win32_pipe_9xa=basic_win32_pipe_9xa<char16_t>;

using u32win32_io_observer_9xa=basic_win32_io_observer_9xa<char32_t>;
using u32win32_file_9xa=basic_win32_file_9xa<char32_t>;
using u32win32_pipe_9xa=basic_win32_pipe_9xa<char32_t>;


template<std::integral char_type>
using basic_win32_io_observer_ntw = basic_win32_family_io_observer<win32_family::wide_nt,char_type>;
template<std::integral char_type>
using basic_win32_file_ntw = basic_win32_family_file<win32_family::wide_nt,char_type>;
template<std::integral char_type>
using basic_win32_pipe_ntw = basic_win32_family_pipe<win32_family::wide_nt,char_type>;

using win32_io_observer_ntw=basic_win32_io_observer_ntw<char>;
using win32_file_ntw=basic_win32_file_ntw<char>;
using win32_pipe_ntw=basic_win32_pipe_ntw<char>;

using wwin32_io_observer_ntw=basic_win32_io_observer_ntw<wchar_t>;
using wwin32_file_ntw=basic_win32_file_ntw<wchar_t>;
using wwin32_pipe_ntw=basic_win32_pipe_ntw<wchar_t>;

using u8win32_io_observer_ntw=basic_win32_io_observer_ntw<char8_t>;
using u8win32_file_ntw=basic_win32_file_ntw<char8_t>;
using u8win32_pipe_ntw=basic_win32_pipe_ntw<char8_t>;

using u16win32_io_observer_ntw=basic_win32_io_observer_ntw<char16_t>;
using u16win32_file_ntw=basic_win32_file_ntw<char16_t>;
using u16win32_pipe_ntw=basic_win32_pipe_ntw<char16_t>;

using u32win32_io_observer_ntw=basic_win32_io_observer_ntw<char32_t>;
using u32win32_file_ntw=basic_win32_file_ntw<char32_t>;
using u32win32_pipe_ntw=basic_win32_pipe_ntw<char32_t>;


template<std::integral char_type>
using basic_win32_io_observer = basic_win32_family_io_observer<win32_family::native,char_type>;
template<std::integral char_type>
using basic_win32_file = basic_win32_family_file<win32_family::native,char_type>;
template<std::integral char_type>
using basic_win32_pipe = basic_win32_family_pipe<win32_family::native,char_type>;

using win32_io_observer=basic_win32_io_observer<char>;
using win32_file=basic_win32_file<char>;
using win32_pipe=basic_win32_pipe<char>;

using wwin32_io_observer=basic_win32_io_observer<wchar_t>;
using wwin32_file=basic_win32_file<wchar_t>;
using wwin32_pipe=basic_win32_pipe<wchar_t>;

using u8win32_io_observer=basic_win32_io_observer<char8_t>;
using u8win32_file=basic_win32_file<char8_t>;
using u8win32_pipe=basic_win32_pipe<char8_t>;

using u16win32_io_observer=basic_win32_io_observer<char16_t>;
using u16win32_file=basic_win32_file<char16_t>;
using u16win32_pipe=basic_win32_pipe<char16_t>;

using u32win32_io_observer=basic_win32_io_observer<char32_t>;
using u32win32_file=basic_win32_file<char32_t>;
using u32win32_pipe=basic_win32_pipe<char32_t>;

#if 0
using io_async_observer=win32_io_observer;
using io_async_scheduler=win32_file;
#endif

template<std::integral char_type=char>
inline basic_win32_io_observer_9xa<char_type> win32_stdin_9xa() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stdin_number)};
}
template<std::integral char_type=char>
inline basic_win32_io_observer_9xa<char_type> win32_stdout_9xa() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stdout_number)};
}
template<std::integral char_type=char>
inline basic_win32_io_observer_9xa<char_type> win32_stderr_9xa() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stderr_number)};
}

template<std::integral char_type=char>
inline basic_win32_io_observer_ntw<char_type> win32_stdin_ntw() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stdin_number)};
}
template<std::integral char_type=char>
inline basic_win32_io_observer_ntw<char_type> win32_stdout_ntw() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stdout_number)};
}
template<std::integral char_type=char>
inline basic_win32_io_observer_ntw<char_type> win32_stderr_ntw() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stderr_number)};
}

template<std::integral char_type=char>
inline basic_win32_io_observer<char_type> win32_stdin() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stdin_number)};
}
template<std::integral char_type=char>
inline basic_win32_io_observer<char_type> win32_stdout() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stdout_number)};
}
template<std::integral char_type=char>
inline basic_win32_io_observer<char_type> win32_stderr() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stderr_number)};
}

#if !defined(__CYGWIN__) && !defined(__WINE__)
template<std::integral char_type=char>
inline basic_win32_io_observer<char_type> native_stdin() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stdin_number)};
}
template<std::integral char_type=char>
inline basic_win32_io_observer<char_type> native_stdout() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stdout_number)};
}
template<std::integral char_type=char>
inline basic_win32_io_observer<char_type> native_stderr() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stderr_number)};
}
#endif

namespace freestanding
{
template<win32_family fm,std::integral char_type>
struct is_trivially_relocatable<basic_win32_family_file<fm,char_type>>
{
	inline static constexpr bool value = true;
};

template<win32_family fm,std::integral char_type>
struct is_zero_default_constructible<basic_win32_family_file<fm,char_type>>
{
	inline static constexpr bool value = true;
};

}

}
