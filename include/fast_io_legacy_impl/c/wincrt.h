#pragma once

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning( disable : 4710 )
#pragma warning( disable : 4820 )
#endif

namespace fast_io
{

namespace win32
{
#if __has_cpp_attribute(__gnu__::__const__)
[[__gnu__::__const__]]
#endif
inline FILE* wincrt_acrt_iob_func(unsigned index) noexcept
{
#if defined(_MSC_VER) || defined(_UCRT)
	return noexcept_call(__acrt_iob_func,index);
#else
	return ::fast_io::win32::wincrt_iob_func()+index;
#endif
}
}

namespace details
{
/*
the entire stdio on windows is too broken. I want to add tie semantics to stdin and stdout to avoid troubles.
*/

#if defined(_MSC_VER) || defined(_UCRT)


struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
ucrt_iobuf
{
    char*	     _ptr;
    char*            _base;
    int              _cnt;
    long             _flag;
    long             _file;
    int              _charbuf;
    int              _bufsiz;
    char*            _tmpfname;
};

#endif

inline constexpr std::size_t wincrt_internal_buffer_size{4096};

template<typename fileptr>
inline constexpr void wincrt_fp_set_flag_dirty_impl(fileptr* __restrict fp) noexcept
{
	fp->_flag|=0x0002;
}

template<typename fileptr>
inline constexpr void wincrt_fp_set_flag_mybuf_impl(fileptr* __restrict fp) noexcept
{
#if defined(_MSC_VER) || defined(_UCRT)
	fp->_flag|=0x0040;
#else
	fp->_flag|=0x0008;
#endif
}

template<typename fileptr>
inline constexpr bool wincrt_fp_is_dirty_impl(fileptr* __restrict fp) noexcept
{
	constexpr unsigned mask{0x0002};
	return (static_cast<unsigned>(fp->_flag)&mask)==mask;
}

inline void* my_malloc_crt(std::size_t buffer_size) noexcept
{
	auto ptr{
#if defined(_DEBUG) && defined(_MSC_VER)

/*
https://docs.microsoft.com/en-us/visualstudio/debugger/crt-debug-heap-details
_NORMAL_BLOCK A call to malloc or calloc creates a Normal block. If you intend to use Normal blocks only, and have no need for Client blocks, you may want to define _CRTDBG_MAP_ALLOC, which causes all heap allocation calls to be mapped to their debug equivalents in Debug builds. This will allow file name and line number information about each allocation call to be stored in the corresponding block header.

_CRT_BLOCK The memory blocks allocated internally by many run-time library functions are marked as CRT blocks so they can be handled separately. As a result, leak detection and other operations need not be affected by them. An allocation must never allocate, reallocate, or free any block of CRT type.

_CLIENT_BLOCK An application can keep special track of a given group of allocations for debugging purposes by allocating them as this type of memory block, using explicit calls to the debug heap functions. MFC, for example, allocates all CObjects as Client blocks; other applications might keep different memory objects in Client blocks. Subtypes of Client blocks can also be specified for greater tracking granularity. To specify subtypes of Client blocks, shift the number left by 16 bits and OR it with _CLIENT_BLOCK. For example:

*/

	::fast_io::noexcept_call(_malloc_dbg,buffer_size,
#if defined(_CRT_BLOCK)
	_CRT_BLOCK
#else
	2 /*_CRT_BLOCK*/
#endif
,__FILE__,__LINE__)
// Provide Debugging information to this file so if people find out issues with hacking they can report to fast_io project
#else
/*
https://github.com/mirror/mingw-w64/blob/master/mingw-w64-headers/crt/crtdbg.h
CRT heap debugging does not exist on mingw-w64
*/
#if defined(__has_builtin)
#if __has_builtin(__builtin_malloc)
	__builtin_malloc(buffer_size)
#else
	malloc(buffer_size)
#endif
#else
	malloc(buffer_size)
#endif
#endif
	};
//handling allocation failure is a historical mistake and it never happens on windows. Just let it crash.
	if(ptr==nullptr)[[unlikely]]
		fast_terminate();
	return ptr;
}

inline void wincrt_fp_allocate_buffer_impl(FILE* __restrict fpp) noexcept
{
#if defined(_MSC_VER) || defined(_UCRT)
	ucrt_iobuf* fp{reinterpret_cast<ucrt_iobuf*>(fpp)};
#else
	FILE* fp{fpp};
#endif
	if(fp->_bufsiz<4)
	{
		fp->_bufsiz=wincrt_internal_buffer_size;
	}
	fp->_bufsiz>>=2;
	fp->_bufsiz<<=2;
	std::size_t allocated_buffer_size{static_cast<std::size_t>(static_cast<unsigned int>(fp->_bufsiz))};
	auto new_buffer{my_malloc_crt(allocated_buffer_size)};
	fp->_ptr=fp->_base=reinterpret_cast<char*>(new_buffer);
	fp->_cnt=0;
	wincrt_fp_set_flag_mybuf_impl(fp);
}

#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline void wincrt_fp_write_cold_malloc_case_impl(FILE* __restrict fpp,char const* __restrict first,std::size_t diff)
{
#if defined(_MSC_VER) || defined(_UCRT)
	ucrt_iobuf* fp{reinterpret_cast<ucrt_iobuf*>(fpp)};
#else
	FILE* fp{fpp};
#endif
	if(diff==0)
		return;
	std::size_t allocated_buffer_size{wincrt_internal_buffer_size};
	if(fp->_bufsiz>=4)
	{
		allocated_buffer_size=static_cast<std::size_t>(static_cast<unsigned>(fp->_bufsiz));
		allocated_buffer_size>>=2;
		allocated_buffer_size<<=2;
	}
	if(diff>=allocated_buffer_size)
	{
		posix_write_nolock_impl(static_cast<int>(fp->_file),first,diff);
		return;
	}
	auto newbuffer{my_malloc_crt(allocated_buffer_size)};
	my_memcpy(newbuffer,first,diff);
	fp->_ptr=(fp->_base=reinterpret_cast<char*>(newbuffer))+diff;
	wincrt_fp_set_flag_mybuf_impl(fp);
	fp->_bufsiz=static_cast<int>(allocated_buffer_size);
	fp->_cnt=fp->_bufsiz-static_cast<int>(diff);
	wincrt_fp_set_flag_dirty_impl(fp);	
}

inline void wincrt_fp_write_cold_normal_case_impl(FILE* __restrict fpp,char const* __restrict first,std::size_t diff)
{
#if defined(_MSC_VER) || defined(_UCRT)
	ucrt_iobuf* fp{reinterpret_cast<ucrt_iobuf*>(fpp)};
#else
	FILE* fp{fpp};
#endif
	std::size_t remain{static_cast<std::size_t>(static_cast<unsigned int>(fp->_cnt))};
	non_overlapped_copy_n(first,remain,fp->_ptr);
	diff-=remain;
	first+=remain;
	fp->_ptr+=remain;
	fp->_cnt=0;
	wincrt_fp_set_flag_dirty_impl(fp);
	posix_write_simple_impl(static_cast<int>(fp->_file),fp->_base,static_cast<std::size_t>(fp->_ptr-fp->_base));
	std::size_t const bufsiz{static_cast<std::size_t>(static_cast<unsigned int>(fp->_bufsiz))};	
	if(diff>=bufsiz)
		posix_write_nolock_impl(static_cast<int>(fp->_file),first,diff);
	else
	{
		fp->_ptr=non_overlapped_copy_n(first,diff,fp->_base);
		fp->_cnt=static_cast<int>(bufsiz-diff);
	}
}

#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline void wincrt_fp_write_cold_impl(FILE* __restrict fp,char const* __restrict first,std::size_t diff)
{
#if defined(_MSC_VER) || defined(_UCRT)
	ucrt_iobuf* fpp{reinterpret_cast<ucrt_iobuf*>(fp)};
#else
	FILE* fpp{fp};
#endif
	if(fpp->_base==nullptr)
		wincrt_fp_write_cold_malloc_case_impl(fp,first,diff);
	else
		wincrt_fp_write_cold_normal_case_impl(fp,first,diff);
}

template<c_family family,std::integral char_type>
inline void wincrt_fp_write_impl(FILE* __restrict fpp,char_type const* first,char_type const* last)
{
	if constexpr(family==c_family::standard)
	{
		c_io_observer ciob{fpp};
		io_lock_guard guard{ciob};
		wincrt_fp_write_impl<c_family::unlocked,char_type>(fpp,first,last);
	}
	else
	{
#if defined(_MSC_VER) || defined(_UCRT)
	ucrt_iobuf* fp{reinterpret_cast<ucrt_iobuf*>(fpp)};
#else
	FILE* fp{fpp};
#endif
	std::size_t diff{static_cast<std::size_t>(last-first)*sizeof(char_type)};
	std::size_t remain{static_cast<std::size_t>(static_cast<unsigned int>(fp->_cnt))};
	if(diff<remain)[[likely]]
	{
		if(diff)[[likely]]
		{
			my_memcpy(fp->_ptr,first,diff);
			auto intdiff{static_cast<int>(static_cast<unsigned int>(diff))};
			fp->_cnt-=intdiff;
			fp->_ptr+=intdiff;
		}
		return;
	}
	wincrt_fp_write_cold_impl(fpp,reinterpret_cast<char const*>(first),diff);
	}
}


template<std::integral char_type>
requires (sizeof(char_type)<=4)
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline void wincrt_fp_overflow_impl(FILE* __restrict fpp,char_type ch)
{
#if defined(_MSC_VER) || defined(_UCRT)
	ucrt_iobuf* fp{reinterpret_cast<ucrt_iobuf*>(fpp)};
#else
	FILE* fp{fpp};
#endif
	if(fp->_base==nullptr)
		wincrt_fp_allocate_buffer_impl(fpp);
	else
		posix_write_simple_impl(static_cast<int>(fp->_file),fp->_base,static_cast<std::size_t>(static_cast<unsigned>(fp->_bufsiz)));
	fp->_ptr=fp->_base;
	my_memcpy(fp->_ptr,__builtin_addressof(ch),sizeof(ch));
	fp->_ptr+=sizeof(ch);
	fp->_cnt=static_cast<int>(static_cast<unsigned int>(fp->_bufsiz-static_cast<int>(sizeof(ch))));
	wincrt_fp_set_flag_dirty_impl(fp);
}

#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline void wincrt_fp_flush_stdout_impl()
{
#if defined(_MSC_VER) || defined(_UCRT)
	ucrt_iobuf* fp{reinterpret_cast<ucrt_iobuf*>(::fast_io::win32::wincrt_acrt_iob_func(1))};
#else
	FILE* fp{::fast_io::win32::wincrt_acrt_iob_func(1)};
#endif
	std::size_t diff{static_cast<std::size_t>(fp->_ptr-fp->_base)};
//	if(diff==0||!wincrt_fp_is_dirty_impl(fp))
	if(diff==0)
		return;
	posix_write_simple_impl(static_cast<int>(fp->_file),fp->_base,diff);
	fp->_ptr=fp->_base;
}

#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline std::size_t wincrt_fp_read_cold_impl(FILE* __restrict fpp,char* first,std::size_t diff)
{
	if(fpp==::fast_io::win32::wincrt_acrt_iob_func(0))
		wincrt_fp_flush_stdout_impl();
#if defined(_MSC_VER) || defined(_UCRT)
	ucrt_iobuf* fp{reinterpret_cast<ucrt_iobuf*>(fpp)};
#else
	FILE* fp{fpp};
#endif
	std::size_t cnt{static_cast<std::size_t>(static_cast<unsigned int>(fp->_cnt))};
	non_overlapped_copy_n(fp->_ptr,cnt,first);
	auto first_temp{first};
	first+=cnt;
	diff-=cnt;
	std::size_t allocated_buffer_size{static_cast<std::size_t>(static_cast<unsigned int>(fp->_bufsiz))};
	if(allocated_buffer_size<4)
		allocated_buffer_size=wincrt_internal_buffer_size;

	if(diff>=allocated_buffer_size)
		return posix_read_impl(static_cast<int>(fp->_file),first,diff);
	else
	{
		if(fp->_base==nullptr)
		{
			auto new_buffer{my_malloc_crt(allocated_buffer_size)};
			fp->_ptr=fp->_base=reinterpret_cast<char*>(new_buffer);
			fp->_cnt=0;
			fp->_bufsiz=static_cast<int>(static_cast<unsigned int>(allocated_buffer_size));
			wincrt_fp_set_flag_mybuf_impl(fp);
		}
		std::size_t readed{posix_read_impl(static_cast<int>(fp->_file),fp->_base,static_cast<std::size_t>(static_cast<unsigned int>(fp->_bufsiz)))};
		fp->_cnt=static_cast<int>(static_cast<unsigned int>(readed));
		fp->_ptr=fp->_base;
		if(readed<diff)
			diff=readed;
		non_overlapped_copy_n(fp->_base,diff,first);
		fp->_ptr+=diff;
		fp->_cnt-=static_cast<int>(static_cast<unsigned int>(diff));
		return static_cast<std::size_t>(first+diff-first_temp);
	}
}

template<c_family family,std::integral char_type>
inline char_type* wincrt_fp_read_impl(FILE* __restrict fpp,char_type* first,char_type* last)
{
	if constexpr(family==c_family::standard)
	{
		c_io_observer ciob{fpp};
		io_lock_guard guard{ciob};
		return wincrt_fp_read_impl<c_family::unlocked,char_type>(fpp,first,last);
	}
	else
	{
#if defined(_MSC_VER) || defined(_UCRT)
	ucrt_iobuf* fp{reinterpret_cast<ucrt_iobuf*>(fpp)};
#else
	FILE* fp{fpp};
#endif
	std::size_t diff{static_cast<std::size_t>(last-first)*sizeof(char_type)};
	std::size_t remain{static_cast<std::size_t>(static_cast<unsigned int>(fp->_cnt))};
	if(diff<remain)[[likely]]
	{
		if(diff)[[likely]]
		{
			my_memcpy(first,fp->_ptr,diff);
			auto intdiff{static_cast<int>(static_cast<unsigned int>(diff))};
			fp->_cnt-=intdiff;
			fp->_ptr+=intdiff;
		}
		return last;
	}
	return first+wincrt_fp_read_cold_impl(fpp,reinterpret_cast<char*>(first),diff)/sizeof(char_type);
	}
}

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline bool wincrt_fp_underflow_impl(FILE* __restrict fpp)
{
	if(fpp==::fast_io::win32::wincrt_acrt_iob_func(0))
		wincrt_fp_flush_stdout_impl();
#if defined(_MSC_VER) || defined(_UCRT)
	ucrt_iobuf* fp{reinterpret_cast<ucrt_iobuf*>(fpp)};
#else
	FILE* fp{fpp};
#endif
	if(fp->_base==nullptr)
		wincrt_fp_allocate_buffer_impl(fpp);
	std::size_t size{posix_read_impl(static_cast<int>(fp->_file),fp->_base,static_cast<std::size_t>(static_cast<unsigned>(fp->_bufsiz)))};
	fp->_ptr=fp->_base;
	fp->_cnt=static_cast<int>(static_cast<unsigned int>(size));
	if constexpr(sizeof(char_type)==1)
		return static_cast<bool>(size);
	else
		return sizeof(char_type)<=size;
}

template<typename T,std::size_t num>
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline T* wincrt_get_buffer_ptr_impl(FILE* __restrict fpp) noexcept
{
	static_assert(num<4);
#if defined(_UCRT) || defined(_MSC_VER)
	ucrt_iobuf* fp{reinterpret_cast<ucrt_iobuf*>(fpp)};
#else
	FILE* fp{fpp};
#endif
	if constexpr(num==0)
		return reinterpret_cast<T*>(fp->_base);
	else if constexpr(num==1)
		return reinterpret_cast<T*>(fp->_ptr);
	else
		return reinterpret_cast<T*>(fp->_ptr+fp->_cnt);
}


template<typename T>
inline void wincrt_set_buffer_curr_ptr_impl(FILE* __restrict fpp,
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
T* ptr) noexcept
{
#if defined(_MSC_VER) || defined(_UCRT)
	ucrt_iobuf* fp{reinterpret_cast<ucrt_iobuf*>(fpp)};
#else
	FILE* fp{fpp};
#endif
	fp->_cnt-=static_cast<int>(static_cast<unsigned int>(static_cast<std::size_t>(reinterpret_cast<char*>(ptr)-fp->_ptr)/sizeof(T)));
	fp->_ptr=reinterpret_cast<char*>(ptr);
}
#if defined(_MSC_VER) || defined(_UCRT)
/*
WINE has not correctly implemented this yet. I am submitting patches.
*/
inline void ucrt_lock_file(FILE* __restrict fp) noexcept
{
	char* fp2{reinterpret_cast<char*>(fp)};
	::fast_io::win32::EnterCriticalSection(fp2+sizeof(ucrt_iobuf));
}
inline void ucrt_unlock_file(FILE* __restrict fp) noexcept
{
	char* fp2{reinterpret_cast<char*>(fp)};
	::fast_io::win32::LeaveCriticalSection(fp2+sizeof(ucrt_iobuf));
}
#endif
}

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* ibuffer_begin(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return details::wincrt_get_buffer_ptr_impl<char_type,0>(ciob.fp);
}

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* ibuffer_curr(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return details::wincrt_get_buffer_ptr_impl<char_type,1>(ciob.fp);
}

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* ibuffer_end(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return details::wincrt_get_buffer_ptr_impl<char_type,2>(ciob.fp);
}

template<std::integral char_type>
inline void ibuffer_set_curr(basic_c_io_observer_unlocked<char_type> ciob,char_type* ptr) noexcept
{
	details::wincrt_set_buffer_curr_ptr_impl(ciob.fp,ptr);
}

template<std::integral char_type>
inline bool ibuffer_underflow(basic_c_io_observer_unlocked<char_type> ciob)
{
	return details::wincrt_fp_underflow_impl<char_type>(ciob.fp);
}


template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* obuffer_begin(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return details::wincrt_get_buffer_ptr_impl<char_type,0>(ciob.fp);
}

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* obuffer_curr(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return details::wincrt_get_buffer_ptr_impl<char_type,1>(ciob.fp);
}

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* obuffer_end(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return details::wincrt_get_buffer_ptr_impl<char_type,2>(ciob.fp);
}

template<std::integral char_type>
inline void obuffer_set_curr(basic_c_io_observer_unlocked<char_type> ciob,char_type* ptr) noexcept
{
	details::wincrt_set_buffer_curr_ptr_impl(ciob.fp,ptr);
}

template<std::integral char_type>
inline void obuffer_overflow(basic_c_io_observer_unlocked<char_type> ciob,char_type ch)
{
	details::wincrt_fp_overflow_impl(ciob.fp,ch);
}

template<c_family family,std::integral char_type,::std::contiguous_iterator Iter>
requires (std::same_as<char_type,char>||std::same_as<::std::iter_value_t<Iter>,char_type>)
inline Iter read(basic_c_family_io_observer<family,char_type> ciob,Iter bg,Iter ed)
{
	if constexpr(!std::same_as<::std::iter_value_t<Iter>,char_type>||!std::is_pointer_v<Iter>)
		return read(ciob,reinterpret_cast<char_type*>(::std::to_address(bg)),
				reinterpret_cast<char_type*>(::std::to_address(ed)))-
				reinterpret_cast<char_type*>(::std::to_address(bg))+bg;
	else
		return details::wincrt_fp_read_impl<family>(ciob.fp,bg,ed);
}


template<c_family family,std::integral char_type,::std::contiguous_iterator Iter>
requires (std::same_as<char_type,char>||std::same_as<::std::iter_value_t<Iter>,char_type>)
inline void write(basic_c_family_io_observer<family,char_type> ciob,Iter bg,Iter ed)
{
	if constexpr(!std::same_as<::std::iter_value_t<Iter>,char_type>||!std::is_pointer_v<Iter>)
		write(ciob,reinterpret_cast<char_type*>(::std::to_address(bg)),
				reinterpret_cast<char_type*>(::std::to_address(ed)));
	else
		details::wincrt_fp_write_impl<family>(ciob.fp,bg,ed);
}

inline c_io_observer c_stdin() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(0)};
}

inline c_io_observer c_stdout() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(1)};
}

inline c_io_observer c_stderr() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(2)};
}

inline wc_io_observer wc_stdin() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(0)};
}

inline wc_io_observer wc_stdout() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(1)};
}

inline wc_io_observer wc_stderr() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(2)};
}

inline u8c_io_observer u8c_stdin() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(0)};
}

inline u8c_io_observer u8c_stdout() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(1)};
}

inline u8c_io_observer u8c_stderr() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(2)};
}

inline u16c_io_observer u16c_stdin() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(0)};
}

inline u16c_io_observer u16c_stdout() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(1)};
}

inline u16c_io_observer u16c_stderr() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(2)};
}

inline u32c_io_observer u32c_stdin() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(0)};
}

inline u32c_io_observer u32c_stdout() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(1)};
}

inline u32c_io_observer u32c_stderr() noexcept
{
	return {::fast_io::win32::wincrt_acrt_iob_func(2)};
}

}

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
