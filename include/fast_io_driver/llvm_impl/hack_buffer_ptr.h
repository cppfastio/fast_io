#pragma once

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif

namespace fast_io::llvm::details
{

class llvm_raw_ostream_model {
public:
	void* ptr{};
	::llvm::raw_ostream::OStreamKind Kind;
	char *OutBufStart, *OutBufEnd, *OutBufCur;
	bool ColorEnabled = false;

	/// Optional stream this stream is tied to. If this stream is written to, the
	/// tied-to stream will be flushed first.
	::llvm::raw_ostream *TiedStream = nullptr;
};

template<std::size_t position>
requires (position<3u)
inline constexpr std::size_t llvm_raw_ostream_cal_obuffer_ptr_position() noexcept
{
	constexpr std::size_t offset{__builtin_offsetof(llvm_raw_ostream_model,OutBufStart)};
	if constexpr(position==0u)
		return offset;
	else if constexpr(position==1u)
		return offset+2*sizeof(char*);
	else
		return offset+sizeof(char*);
}

template<std::size_t position,std::integral char_type>
requires (position<3u)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* llvm_raw_ostream_obuffer_ptr(::llvm::raw_ostream* os) noexcept
{
	constexpr std::size_t offset{llvm_raw_ostream_cal_obuffer_ptr_position<position>()};
	char_type* ptr;
	::fast_io::details::my_memcpy(__builtin_addressof(ptr),reinterpret_cast<char*>(os)+offset,sizeof(char_type*));
	return ptr;
}

inline void llvm_raw_ostream_set_obuffer_curr_ptr(::llvm::raw_ostream* os,char* vdptr)
{
	constexpr std::size_t offset{llvm_raw_ostream_cal_obuffer_ptr_position<1>()};
	::fast_io::details::my_memcpy(reinterpret_cast<char*>(os)+offset,__builtin_addressof(vdptr),sizeof(char*));
}

inline void llvm_raw_ostream_overflow(::llvm::raw_ostream* os,char ch)
{
	os->flush();
	constexpr std::size_t one{1u};
	os->write(__builtin_addressof(ch),one);
}

class raw_fd_ostream_model : public ::llvm::raw_pwrite_stream
{
public:
int FD;
bool ShouldClose;
bool SupportsSeeking = false;
};
inline constexpr std::size_t fdoffset{__builtin_offsetof(raw_fd_ostream_model,FD)};

inline int hack_fd_from_llvm_raw_fd_ostream(::llvm::raw_fd_ostream* os) noexcept
{
	if(os==nullptr)
		return -1;
	int fd;
	::fast_io::details::my_memcpy(__builtin_addressof(fd),reinterpret_cast<char*>(os)+fdoffset,sizeof(int));
	return fd;
}

inline void hack_set_fd_to_llvm_raw_fd_ostream(::llvm::raw_fd_ostream* os,int fd) noexcept
{
	::fast_io::details::my_memcpy(reinterpret_cast<char*>(os)+fdoffset,__builtin_addressof(fd),sizeof(int));
}

}
