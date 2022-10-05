#pragma once

namespace fast_io::llvm
{

template<std::integral ch_type,typename T>
requires (std::derived_from<T,::llvm::raw_ostream>&&sizeof(ch_type)==1)
class basic_general_raw_ostream_io_observer
{
public:
	using char_type = ch_type;
	using native_handle_type = T*;
	native_handle_type os{};
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{os};
		os=nullptr;
		return temp;
	}
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return os;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return os;
	}
#if !defined(__AVR__)
	explicit operator basic_posix_io_observer<char_type>() const noexcept requires(std::derived_from<T,::llvm::raw_fd_ostream>)
	{
		return {::fast_io::llvm::details::hack_fd_from_llvm_raw_fd_ostream(this->os)};
	}
#if (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
	explicit operator basic_win32_io_observer<char_type>() const noexcept requires(std::derived_from<T,::llvm::raw_fd_ostream>)
	{
		return static_cast<basic_win32_io_observer<char_type>>
		(static_cast<basic_posix_io_observer<char_type>>(*this));
	}
	template<nt_family fam>
	explicit operator basic_nt_family_io_observer<fam,char_type>() const noexcept requires(std::derived_from<T,::llvm::raw_fd_ostream>)
	{
		return static_cast<basic_nt_family_io_observer<fam,char_type>>
		(static_cast<basic_posix_io_observer<char_type>>(*this));
	}
#endif
#endif
};

template<std::integral char_type,typename T>
inline constexpr basic_general_raw_ostream_io_observer<char_type,T> io_value_handle(basic_general_raw_ostream_io_observer<char_type,T> osiob) noexcept
{
	return osiob;
}

namespace details
{

inline void llvm_ostream_write_impl(::llvm::raw_ostream* os,void const* first_vptr,void const* last_vptr)
{
	char const* first{reinterpret_cast<char const*>(first_vptr)};
	char const* last{reinterpret_cast<char const*>(last_vptr)};
	std::size_t const n{static_cast<std::size_t>(last-first)};
	auto os_obuffer_curr_ptr{llvm_raw_ostream_obuffer_ptr<1,char>(os)};
	auto os_obuffer_end_ptr{llvm_raw_ostream_obuffer_ptr<2,char>(os)};
	std::size_t const remain_size{static_cast<std::size_t>(os_obuffer_end_ptr-os_obuffer_curr_ptr)};
	if(n<remain_size)[[likely]]
	{
		::fast_io::llvm::details::llvm_raw_ostream_set_obuffer_curr_ptr(os,::fast_io::details::non_overlapped_copy_n(first,n,os_obuffer_curr_ptr));
		return;
	}
	os->write(first,n);
}

}

template<std::integral char_type,typename T,::std::contiguous_iterator Iter>
inline constexpr void write(basic_general_raw_ostream_io_observer<char_type,T> osiob,Iter first,Iter last)
{
	::fast_io::llvm::details::llvm_ostream_write_impl(osiob.os,::std::to_address(first),::std::to_address(last));
}

template<std::integral char_type,typename T>
inline constexpr void flush(basic_general_raw_ostream_io_observer<char_type,T> osiob)
{
	osiob.os->flush();
}

template<std::integral char_type,typename T>
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* obuffer_begin(basic_general_raw_ostream_io_observer<char_type,T> osiob) noexcept
{
	return ::fast_io::llvm::details::llvm_raw_ostream_obuffer_ptr<0,char_type>(osiob.os);
}

template<std::integral char_type,typename T>
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* obuffer_curr(basic_general_raw_ostream_io_observer<char_type,T> osiob) noexcept
{
	return ::fast_io::llvm::details::llvm_raw_ostream_obuffer_ptr<1,char_type>(osiob.os);
}

template<std::integral char_type,typename T>
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* obuffer_end(basic_general_raw_ostream_io_observer<char_type,T> osiob) noexcept
{
	return ::fast_io::llvm::details::llvm_raw_ostream_obuffer_ptr<2,char_type>(osiob.os);
}

template<std::integral char_type,typename T>
inline void obuffer_set_curr(basic_general_raw_ostream_io_observer<char_type,T> osiob,
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
char_type* ptr) noexcept
{
	::fast_io::llvm::details::llvm_raw_ostream_set_obuffer_curr_ptr(osiob.os,reinterpret_cast<char*>(ptr));
}

template<std::integral char_type,typename T>
inline void obuffer_overflow(basic_general_raw_ostream_io_observer<char_type,T> osiob,char_type ch) noexcept
{
	::fast_io::llvm::details::llvm_raw_ostream_overflow(osiob.os,static_cast<char>(ch));
}

template<std::integral char_type>
using basic_raw_ostream_io_observer=basic_general_raw_ostream_io_observer<char_type,::llvm::raw_ostream>;
using raw_ostream_io_observer=basic_raw_ostream_io_observer<char>;
using u8raw_ostream_io_observer=basic_raw_ostream_io_observer<char8_t>;

template<std::integral char_type>
using basic_raw_fd_ostream_io_observer=basic_general_raw_ostream_io_observer<char_type,::llvm::raw_fd_ostream>;
using raw_fd_ostream_io_observer=basic_raw_fd_ostream_io_observer<char>;
using u8raw_fd_ostream_io_observer=basic_raw_fd_ostream_io_observer<char8_t>;

}
