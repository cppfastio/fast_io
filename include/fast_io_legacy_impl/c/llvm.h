#pragma once
/*
This implementation is an untested guessing implementation since i do not know how to build LLVM libc.

If you see this does not work, please file an issue on github to cppfastio;
Referenced from
https://github.com/llvm/llvm-project/blob/main/libc/src/__support/CPP/atomic.h
https://github.com/llvm/llvm-project/blob/main/libc/src/__support/threads/linux/futex_word.h
https://github.com/llvm/llvm-project/blob/main/libc/src/__support/File/file.h

*/

namespace fast_io
{

namespace details::llvmlibc_hack
{

template<typename T>
struct llvmlibc_atomic
{
using value_type = T;
static inline constexpr int llvmlibc_atomic_alignment = sizeof(T) > alignof(T) ? sizeof(T)
						: alignof(T);

// We keep the internal value public so that it can be addressable.
// This is useful in places like the Linux futex operations where
// we need pointers to the memory of the atomic values. Load and store
// operations should be performed using the atomic methods however.
alignas(llvmlibc_atomic_alignment) value_type val;
};

using FutexWordType = unsigned int;

struct Mutex {
unsigned char timed;
unsigned char recursive;
unsigned char robust;

void *owner;
unsigned long long lock_count;

llvmlibc_atomic<FutexWordType> futex_word;
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
File
{

using LockFunc = void(File *) noexcept;
using UnlockFunc = void(File *) noexcept;

using WriteFunc = size_t(File *, void const *, size_t) noexcept;
using ReadFunc = size_t(File *, void *, size_t) noexcept;
using SeekFunc = int(File *, long, int) noexcept;	//why seek offset long???
using CloseFunc = int(File *) noexcept;
using FlushFunc = int(File *) noexcept;

using ModeFlags = ::std::uint_least32_t;
enum class FileOp : ::std::uint_least8_t { NONE, READ, WRITE, SEEK };
WriteFunc *platform_write;
ReadFunc *platform_read;
SeekFunc *platform_seek;
CloseFunc *platform_close;
FlushFunc *platform_flush;

Mutex mutex;

void *buf;      // Pointer to the stream buffer for buffered streams
size_t bufsize; // Size of the buffer pointed to by |buf|.

// Buffering mode to used to buffer.
int bufmode;

// If own_buf is true, the |buf| is owned by the stream and will be
// free-ed when close method is called on the stream.
bool own_buf;

// The mode in which the file was opened.
ModeFlags mode;

// Current read or write pointer.
size_t pos;

// Represents the previous operation that was performed.
FileOp prev_op;

// When the buffer is used as a read buffer, read_limit is the upper limit
// of the index to which the buffer can be read until.
size_t read_limit;

bool eof;
bool err;

};

inline void* llvm_libc_buffer_begin(FILE* fpp) noexcept
{
	auto fp{reinterpret_cast<File*>(fpp)};
	return reinterpret_cast<char*>(fp->buf);
}

inline void* llvm_libc_buffer_curr(FILE* fpp) noexcept
{
	auto fp{reinterpret_cast<File*>(fpp)};
	return reinterpret_cast<char*>(fp->buf)+fp->pos;
}

inline void* llvm_libc_ibuffer_end(FILE* fpp) noexcept
{
	auto fp{reinterpret_cast<File*>(fpp)};
	return reinterpret_cast<char*>(fp->buf)+fp->read_limit;
}

inline void* llvm_libc_obuffer_end(FILE* fpp) noexcept
{
	auto fp{reinterpret_cast<File*>(fpp)};
	return reinterpret_cast<char*>(fp->buf)+fp->bufsize;
}

inline void llvm_libc_set_buffer_curr(FILE* fpp,void* currptr) noexcept
{
	auto fp{reinterpret_cast<File*>(fpp)};
	fp->pos=static_cast<std::size_t>(reinterpret_cast<char*>(currptr)-reinterpret_cast<char*>(fp->buf));
}

inline bool llvm_libc_underflow(FILE* fpp) noexcept
{
	auto fp{reinterpret_cast<File*>(fpp)};
	auto pr{fp->platform_read};
	if(pr==nullptr)
	{
		return false;
	}
	fp->read_limit=fp->platform_read(fp,fp->buf,fp->bufsize);
	fp->pos=0;
	return fp->read_limit;
}

inline void llvm_libc_overflow(FILE* fpp,char ch) noexcept
{
	auto fp{reinterpret_cast<File*>(fpp)};
	auto pr{fp->platform_write};
	if(pr==nullptr)
	{
		return;
	}
	fp->platform_write(fp,fp->buf,fp->pos);
	fp->pos=0;
	if(fp->bufsize)
	{
		*reinterpret_cast<char*>(fp->buf)=ch;
		++fp->pos;
	}
}

}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* ibuffer_begin(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return reinterpret_cast<char_type*>(::fast_io::details::llvmlibc_hack::llvm_libc_buffer_begin(ciob.fp));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* obuffer_begin(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return reinterpret_cast<char_type*>(::fast_io::details::llvmlibc_hack::llvm_libc_buffer_begin(ciob.fp));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* ibuffer_curr(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return reinterpret_cast<char_type*>(::fast_io::details::llvmlibc_hack::llvm_libc_buffer_curr(ciob.fp));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* obuffer_curr(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return reinterpret_cast<char_type*>(::fast_io::details::llvmlibc_hack::llvm_libc_buffer_curr(ciob.fp));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* ibuffer_end(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return reinterpret_cast<char_type*>(::fast_io::details::llvmlibc_hack::llvm_libc_ibuffer_end(ciob.fp));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* obuffer_end(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return reinterpret_cast<char_type*>(::fast_io::details::llvmlibc_hack::llvm_libc_obuffer_end(ciob.fp));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
inline void ibuffer_set_curr(basic_c_io_observer_unlocked<char_type> ciob,char_type* ptr) noexcept
{
	::fast_io::details::llvmlibc_hack::llvm_libc_set_buffer_curr(ciob.fp,ptr);
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
inline void obuffer_set_curr(basic_c_io_observer_unlocked<char_type> ciob,char_type* ptr) noexcept
{
	::fast_io::details::llvmlibc_hack::llvm_libc_set_buffer_curr(ciob.fp,ptr);
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
inline bool ibuffer_underflow(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return ::fast_io::details::llvmlibc_hack::llvm_libc_underflow(ciob.fp);
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
inline void obuffer_overflow(basic_c_io_observer_unlocked<char_type> ciob,char_type ch) noexcept
{
	::fast_io::details::llvmlibc_hack::llvm_libc_overflow(ciob.fp,static_cast<char>(ch));
}

}
