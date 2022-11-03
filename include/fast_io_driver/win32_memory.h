#pragma once
#include"../fast_io_hosted.h"
namespace fast_io
{

//https://github.com/MicrosoftDocs/windows-driver-docs/issues/1736

enum class win32_memory_page_protect:std::uint_least32_t
{
executed = 0x10,
executed_read = 0x20,
executed_readwrite = 0x40,
executed_writecopy = 0x80,
noaccess = 0x01,
readonly = 0x02,
readwrite = 0x04,
writecopy = 0x08,
graphics_noaccess = 0x0800,
graphics_readonly = 0x1000,
graphics_readwrite = 0x2000,
graphics_execute = 0x4000,
graphics_execute_read = 0x8000,
graphics_execute_readwrite = 0x10000,
graphics_coherent = 0x20000,
targets_invalid = 0x40000000,
targets_no_update = 0x40000000,
guard = 0x100,
nocache = 0x200,
writecombine = 0x400,
enclave_thread_control = 0x80000000,
revert_to_file_map = 0x80000000,
enclave_unvalidated = 0x20000000,
enclave_no_change = 0x20000000,
enclave_nodecommit = 0x10000000,
};
#if 0
template<reserve_output_stream output>
constexpr void print_define(output& out,win32_memory_page_protect info)
{
	switch(info)
	{
	case win32_memory_page_protect::executed:
		print_freestanding(out,"executed");
	break;
	case win32_memory_page_protect::executed_read:
		print_freestanding(out,"executed_read");
	break;
	case win32_memory_page_protect::executed_readwrite:
		print_freestanding(out,"executed_readwrite");
	break;
	case win32_memory_page_protect::executed_writecopy:
		print_freestanding(out,"executed_writecopy");
	break;
	case win32_memory_page_protect::noaccess:
		print_freestanding(out,"noaccess");
	break;
	case win32_memory_page_protect::readonly:
		print_freestanding(out,"readonly");
	break;
	case win32_memory_page_protect::readwrite:
		print_freestanding(out,"readwrite");
	break;
	case win32_memory_page_protect::writecopy:
		print_freestanding(out,"writecopy");
	break;

	case win32_memory_page_protect::graphics_noaccess:
		print_freestanding(out,"graphics_noaccess");
	break;
	case win32_memory_page_protect::graphics_readonly:
		print_freestanding(out,"graphics_readonly");
	break;
	case win32_memory_page_protect::graphics_readwrite:
		print_freestanding(out,"graphics_readwrite");
	break;
//	case win32_memory_page_protect::graphics_writecopy:
//		print_freestanding(out,"graphics_writecopy");
//	break;
	case win32_memory_page_protect::graphics_execute:
		print_freestanding(out,"graphics_execute");
	break;
	case win32_memory_page_protect::graphics_execute_read:
		print_freestanding(out,"graphics_execute_read");
	break;
	case win32_memory_page_protect::graphics_execute_readwrite:
		print_freestanding(out,"graphics_execute_readwrite");
	break;
	case win32_memory_page_protect::graphics_coherent:
		print_freestanding(out,"graphics_coherent");
	break;

	case win32_memory_page_protect::targets_invalid:
		print_freestanding(out,"targets_invalid/targets_no_update");
	break;

	case win32_memory_page_protect::guard:
		print_freestanding(out,"guard");
	break;
	case win32_memory_page_protect::nocache:
		print_freestanding(out,"nocache");
	break;

	case win32_memory_page_protect::writecombine:
		print_freestanding(out,"writecombine");
	break;

	case win32_memory_page_protect::enclave_thread_control:
		print_freestanding(out,"enclave_thread_control/revert_to_file_map/enclave_unvalidated");
	break;

	case win32_memory_page_protect::enclave_no_change:
		print_freestanding(out,"enclave_no_change/enclave_nodecommit");
	break;
	default:
		print_freestanding(out,"unknown(",static_cast<std::uint_least32_t>(info),")");
	}
}
#endif

constexpr win32_memory_page_protect operator&(win32_memory_page_protect x, win32_memory_page_protect y) noexcept
{
using utype = typename std::underlying_type<win32_memory_page_protect>::type;
return static_cast<win32_memory_page_protect>(static_cast<utype>(x) & static_cast<utype>(y));
}

constexpr win32_memory_page_protect operator|(win32_memory_page_protect x, win32_memory_page_protect y) noexcept
{
using utype = typename std::underlying_type<win32_memory_page_protect>::type;
return static_cast<win32_memory_page_protect>(static_cast<utype>(x) | static_cast<utype>(y));
}

constexpr win32_memory_page_protect operator^(win32_memory_page_protect x, win32_memory_page_protect y) noexcept
{
using utype = typename std::underlying_type<win32_memory_page_protect>::type;
return static_cast<win32_memory_page_protect>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

constexpr win32_memory_page_protect operator~(win32_memory_page_protect x) noexcept
{
using utype = typename std::underlying_type<win32_memory_page_protect>::type;
return static_cast<win32_memory_page_protect>(~static_cast<utype>(x));
}

inline constexpr win32_memory_page_protect& operator&=(win32_memory_page_protect& x, win32_memory_page_protect y) noexcept{return x=x&y;}

inline constexpr win32_memory_page_protect& operator|=(win32_memory_page_protect& x, win32_memory_page_protect y) noexcept{return x=x|y;}

inline constexpr win32_memory_page_protect& operator^=(win32_memory_page_protect& x, win32_memory_page_protect y) noexcept{return x=x^y;}

struct win32_memory_basic_information
{
	std::uintptr_t base_address{};
	std::uintptr_t allocation_base{};
	win32_memory_page_protect allocation_protect{};
	std::size_t region_size{};
	std::uint_least32_t state{};
	std::uint_least32_t protect{};
	std::uint_least32_t type{};
};
#if 0
template<reserve_output_stream output>
constexpr void print_define(output& out,win32_memory_basic_information const& info)
{
	print_freestanding(out,"win32_memory_basic_information:"
	"\nbase address:",info.base_address,
	"\nallocation base:",info.allocation_base,
	"\nallocation protect:",info.allocation_protect,
	"\nregion size:",info.region_size,
	"\nstate:",info.state,
	"\nprotect:",info.protect,
	"\ntype:",info.type);
}
#endif
namespace win32
{
extern "C" void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
OpenProcess(std::uint_least32_t,int,std::uint_least32_t);
extern "C" int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WriteProcessMemory(void*,void*,void const*,std::size_t,std::size_t*);
extern "C" int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
ReadProcessMemory(void*,void const*,void*,std::size_t,std::size_t*);
extern "C" int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
VirtualQueryEx(void*,void const*,win32_memory_basic_information*,std::size_t);
extern "C" int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
VirtualProtectEx(void*,void const*,std::size_t,std::uint_least32_t,std::uint_least32_t*);
extern "C" void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
FindWindowA(char const*,char const*);
extern "C" std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetWindowThreadProcessId(void*,std::uint_least32_t*);
}

template<std::integral ch_type>
class basic_win32_memory_io_observer
{
public:
	using native_handle_type = void*;
	using base_address_type = std::uintptr_t;
	using char_type = ch_type;
	native_handle_type handle{};
	base_address_type base_addr{};
	constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle=nullptr;
		return temp;
	}
	constexpr operator bool() const noexcept
	{
		return handle;
	}
	constexpr auto& native_handle() const noexcept
	{
		return handle;
	}
	constexpr auto& native_handle() noexcept
	{
		return handle;
	}
	constexpr auto& base_address() const noexcept
	{
		return base_addr;
	}
	constexpr auto& base_address() noexcept
	{
		return base_addr;
	}
};

template<std::integral ch_type>
class basic_win32_memory_io_handle: public basic_win32_memory_io_observer<ch_type>
{
public:
	using native_handle_type = basic_win32_memory_io_observer<ch_type>::native_handle_type;
	using base_address_type = basic_win32_memory_io_observer<ch_type>::base_address_type;
	using char_type = ch_type;
	constexpr basic_win32_memory_io_handle()=default;
	constexpr basic_win32_memory_io_handle(native_handle_type hd,base_address_type base):basic_win32_memory_io_observer<ch_type>(hd,base){}
	basic_win32_memory_io_handle(basic_win32_memory_io_handle const& other)
	{
		auto const current_process(win32::GetCurrentProcess());
		if (!win32::DuplicateHandle(current_process, other.native_handle(), current_process, __builtin_addressof(this->native_handle()), 0, true, 2/*DUPLICATE_SAME_ACCESS*/))
			throw_win32_error();
		this->base_address()=other.base_address();
	}
	basic_win32_memory_io_handle& operator=(basic_win32_memory_io_handle const& other)
	{
		auto const current_process(win32::GetCurrentProcess());
		void* new_handle{};
		if(!win32::DuplicateHandle(current_process,other.native_handle(),current_process,__builtin_addressof(new_handle), 0, true, 2/*DUPLICATE_SAME_ACCESS*/))
			throw_win32_error();
		if(this->native_handle())[[likely]]
			fast_io::win32::CloseHandle(this->native_handle());
		this->native_handle()=new_handle;
		this->base_address()=other.base_address();
		return *this;
	}
	basic_win32_memory_io_handle& operator=(basic_win32_memory_io_handle&& __restrict other) noexcept
	{
		if(this->native_handle())[[likely]]
			fast_io::win32::CloseHandle(this->native_handle());
		this->native_handle()=other.native_handle();
		this->base_address()=other.base_address();
		other.native_handle()=nullptr;
		other.base_address()={};
		return *this;
	}
	constexpr basic_win32_memory_io_handle(basic_win32_memory_io_handle&& __restrict other) noexcept:basic_win32_memory_io_observer<ch_type>(other.native_handle(),other.base_address())
	{
		other.native_handle()=nullptr;
		other.base_address()={};
	}
};

enum class win32_desired_access:std::uint_least32_t
{
del = 0x00010000L,
read_control = 0x00020000L,
synchroize = 0x00100000L,
write_dac = 0x00040000L,
write_owner = 0x00080000L,
process_all_access = 0x1F0FFF,
process_create_process = 0x0080,
process_create_thread = 0x0002,
process_dup_handle = 0x0040,
process_query_information = 0x0400,
process_query_limited_information = 0x1000,
process_set_information = 0x0200,
process_set_quota = 0x0100,
process_suspend_resume = 0x0800,
process_terminate = 0x0001,
process_vm_operation = 0x0008,
process_vm_read = 0x0010,
process_vm_write = 0x0020,
};

constexpr win32_desired_access operator&(win32_desired_access x, win32_desired_access y) noexcept
{
using utype = typename std::underlying_type<win32_desired_access>::type;
return static_cast<win32_desired_access>(static_cast<utype>(x) & static_cast<utype>(y));
}

constexpr win32_desired_access operator|(win32_desired_access x, win32_desired_access y) noexcept
{
using utype = typename std::underlying_type<win32_desired_access>::type;
return static_cast<win32_desired_access>(static_cast<utype>(x) | static_cast<utype>(y));
}

constexpr win32_desired_access operator^(win32_desired_access x, win32_desired_access y) noexcept
{
using utype = typename std::underlying_type<win32_desired_access>::type;
return static_cast<win32_desired_access>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

constexpr win32_desired_access operator~(win32_desired_access x) noexcept
{
using utype = typename std::underlying_type<win32_desired_access>::type;
return static_cast<win32_desired_access>(~static_cast<utype>(x));
}

inline constexpr win32_desired_access& operator&=(win32_desired_access& x, win32_desired_access y) noexcept{return x=x&y;}

inline constexpr win32_desired_access& operator|=(win32_desired_access& x, win32_desired_access y) noexcept{return x=x|y;}

inline constexpr win32_desired_access& operator^=(win32_desired_access& x, win32_desired_access y) noexcept{return x=x^y;}

inline std::uint_least32_t get_process_id_from_window_name(cstring_view name)
{
	void* hwnd {win32::FindWindowA(nullptr,name.data())};
	if(hwnd==nullptr)
		throw_win32_error();
	std::uint_least32_t process_id{};
	win32::GetWindowThreadProcessId(hwnd,__builtin_addressof(process_id));
	return process_id;
}

template<std::integral ch_type>
class basic_win32_memory_file: public basic_win32_memory_io_handle<ch_type>
{
public:
	using native_handle_type = basic_win32_memory_io_observer<ch_type>::native_handle_type;
	using base_address_type = basic_win32_memory_io_observer<ch_type>::base_address_type;
	using char_type = ch_type;
	constexpr basic_win32_memory_file()=default;
	constexpr basic_win32_memory_file(native_handle_type hd,base_address_type base):
		basic_win32_memory_io_handle<ch_type>(hd,base){}
	basic_win32_memory_file(win32_desired_access dw_desired_access,bool inherit_handle,std::uint_least32_t process_id,
		base_address_type base_addr={}):
		basic_win32_memory_io_handle<ch_type>(win32::OpenProcess(static_cast<std::uint_least32_t>(dw_desired_access),inherit_handle,process_id),base_addr)
	{
		if(this->native_handle()==nullptr)
			throw_win32_error();
	}
	void close()
	{
		if(this->native_handle())[[likely]]
		{
			if(!fast_io::win32::CloseHandle(this->native_handle()))[[unlikely]]
				throw_win32_error();
			this->native_handle()=nullptr;
			this->base_address()={};
		}
	}
	constexpr basic_win32_memory_file(basic_win32_memory_file const&)=default;
	constexpr basic_win32_memory_file& operator=(basic_win32_memory_file const&)=default;
	constexpr basic_win32_memory_file(basic_win32_memory_file&& __restrict) noexcept=default;
	constexpr basic_win32_memory_file& operator=(basic_win32_memory_file&& __restrict) noexcept=default;
	~basic_win32_memory_file()
	{
		if(this->native_handle())[[likely]]
			fast_io::win32::CloseHandle(this->native_handle());
	}
};

template<std::integral char_type,::std::contiguous_iterator Iter>
[[nodiscard]] inline Iter read(basic_win32_memory_io_observer<char_type>& iob,Iter begin,Iter end)
{
	std::size_t readed{};
	if(!win32::ReadProcessMemory(iob.handle,bit_cast<void const*>(iob.base_addr),::std::to_address(begin),(end-begin)*sizeof(*begin),__builtin_addressof(readed)))
		throw_win32_error();
	iob.base_addr+=readed;
	return begin+readed/sizeof(*begin);
}
template<std::integral char_type>
[[nodiscard]] inline auto find_read_start(basic_win32_memory_io_observer<char_type> iob)
{
	std::size_t readed{};
	for(std::byte ch{};!win32::ReadProcessMemory(iob.handle,bit_cast<void const*>(iob.base_addr),__builtin_addressof(ch),1,__builtin_addressof(readed));++iob.base_addr);
	return iob;
}
template<std::integral char_type,::std::contiguous_iterator Iter>
inline Iter write(basic_win32_memory_io_observer<char_type>& iob,Iter begin,Iter end)
{
	std::size_t written{};
	if(!win32::WriteProcessMemory(iob.handle,bit_cast<void*>(iob.base_addr),
		::std::to_address(begin),(end-begin)*sizeof(*begin),__builtin_addressof(written)))
		throw_win32_error();
	iob.base_addr+=written;
	return begin+written/sizeof(*begin);
}
using win32_memory_io_observer = basic_win32_memory_io_observer<char>;
using win32_memory_io_handle = basic_win32_memory_io_handle<char>;
using win32_memory_file = basic_win32_memory_file<char>;

static_assert(input_stream<win32_memory_file>);
static_assert(output_stream<win32_memory_file>);

template<std::integral char_type>
inline win32_memory_basic_information win32_virtual_query(basic_win32_memory_io_observer<char_type> iob)
{
	win32_memory_basic_information mem{};
	if(win32::VirtualQueryEx(iob.handle,bit_cast<void const*>(iob.base_addr),__builtin_addressof(mem),sizeof(mem))!=sizeof(mem))
		throw_win32_error();
	return mem;
}

template<std::integral char_type>
inline win32_memory_page_protect win32_virtual_protect(basic_win32_memory_io_observer<char_type> iob,
	std::size_t size,
	win32_memory_page_protect new_protect)
{
	std::uint_least32_t old_protect{};
	if(!win32::VirtualProtectEx(iob.handle,bit_cast<void const*>(iob.base_addr),
		size,static_cast<std::uint_least32_t>(new_protect),__builtin_addressof(old_protect)))
		throw_win32_error();
	return static_cast<win32_memory_page_protect>(old_protect);
}

template<std::integral char_type>
[[nodiscard]] inline auto find_usable_region(basic_win32_memory_io_observer<char_type> wmf)
{
	for(;;)
	{
		auto mem_info{fast_io::win32_virtual_query(wmf)};
		if(static_cast<std::uint_least32_t>(mem_info.allocation_protect)==0)
		{
			std::size_t rsize{mem_info.region_size};
			if(rsize==0)[[unlikely]]
				break;
			wmf.base_address()+=rsize;
		}
		else
			break;
	}
	return wmf;
}


class win32_virtual_protect_guard
{
	void* process_handle{};
	std::uintptr_t address{};
	std::size_t region_size{};
	win32_memory_page_protect oprotect{};
public:
	template<std::integral char_type>
	win32_virtual_protect_guard(basic_win32_memory_io_observer<char_type> iob,
	std::size_t size,
	win32_memory_page_protect new_protect):
		process_handle(iob.native_handle()),
		address(iob.base_addr),
		region_size(size),
		oprotect(win32_virtual_protect(iob,size,new_protect))
	{
	}
	constexpr auto old_protect() const noexcept
	{
		return oprotect;
	}
	win32_virtual_protect_guard(win32_virtual_protect_guard const&)=delete;
	win32_virtual_protect_guard& operator=(win32_virtual_protect_guard const&)=delete;
	~win32_virtual_protect_guard()
	{
		std::uint_least32_t useless{};
		win32::VirtualProtectEx(process_handle,bit_cast<void const*>(address),
		region_size,static_cast<std::uint_least32_t>(oprotect),__builtin_addressof(useless));
	}
};

}
