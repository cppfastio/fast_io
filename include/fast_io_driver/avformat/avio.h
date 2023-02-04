#pragma once

extern "C" {
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
}

namespace fast_io
{

template<std::integral ch_type>
requires (sizeof(ch_type) == 1)
class basic_avio_context_io_observer
{
public:
	using char_type = ch_type;
	using native_handle_type = ::AVIOContext*;
	native_handle_type avios{};

	constexpr native_handle_type native_handle() const noexcept
	{
		return avios;
	}
	constexpr ::AVIOContext* release() noexcept
	{
		auto temp{this->avios};
		this->avios=nullptr;
		return temp;
	}
};

template<std::integral char_type>
inline void write(basic_avio_context_io_observer<char_type> baciob,char_type const* first,char_type const* last)
{
	baciob.avios->error=0;
	if constexpr(INT_MAX<SIZE_MAX)
	{
		for(;first!=last;)
		{
			constexpr
				::std::size_t mx{INT_MAX};
			int this_round{INT_MAX};
			::std::size_t diff{static_cast<std::size_t>(last-first)};
			if(diff<mx)
#if __has_cpp_attribute(likely)
			[[likely]]
#endif
			{
				this_round=diff;
			}
			::fast_io::noexcept_call(::avio_write,baciob.avios,first,last);
			first+=this_round;
		}
	}
	else
	{
		::fast_io::noexcept_call(::avio_write,baciob.avios,first,static_cast<int>(last-first));
	}
	if(baciob.avios->error)
	{
		throw_posix_error(EINVAL);
	}
}

template<std::integral char_type>
inline char_type* read(basic_avio_context_io_observer<char_type> baciob,char_type* first,char_type* last)
{
	baciob.avios->error=0;
	int ret;
	if constexpr(INT_MAX<SIZE_MAX)
	{
		constexpr
			::std::size_t mx{INT_MAX};
		int this_round{INT_MAX};
		::std::size_t diff{static_cast<std::size_t>(last-first)};
		if(diff<mx)
#if __has_cpp_attribute(likely)
		[[likely]]
#endif
		{
			this_round=diff;
		}
		ret = ::fast_io::noexcept_call(::avio_read,baciob.avios,first,this_round);
	}
	else
	{
		ret = ::fast_io::noexcept_call(::avio_read,baciob.avios,first,static_cast<int>(last-first));
	}
	if(baciob.avios->error)
	{
		throw_posix_error(EINVAL);
	}
	return ret;
}

template<std::integral ch_type>
requires (sizeof(ch_type) == 1)
inline constexpr basic_avio_context_io_observer<ch_type> io_value_handle(basic_avio_context_io_observer<ch_type> baciob) noexcept
{
	return baciob;
}

#if 0

template<std::integral char_type>
inline char_type* ibuffer_begin(basic_avio_context_io_observer<char_type> baciob) noexcept
{
	return reinterpret_cast<char_type*>(baciob.avios.buffer);
}

template<std::integral char_type>
inline char_type* ibuffer_curr(basic_avio_context_io_observer<char_type> baciob) noexcept
{
	return reinterpret_cast<char_type*>(baciob.avios.buf_ptr);
}

template<std::integral char_type>
inline char_type* ibuffer_end(basic_avio_context_io_observer<char_type> baciob) noexcept
{
	return reinterpret_cast<char_type*>(baciob.avios.buf_end);
}

template<std::integral char_type>
inline bool ibuffer_underflow(basic_avio_context_io_observer<char_type> baciob) noexcept
{
	baciob.avios.buf_ptr = baciob.avios.buf_end<=baciob.avios.buf_ptr;

	return;	
}

template<std::integral char_type>
inline char_type* ibuffer_begin(basic_avio_context_io_observer<char_type> baciob) noexcept
{
	return reinterpret_cast<char_type*>(baciob.avios.buffer);
}

template<std::integral char_type>
inline char_type* ibuffer_curr(basic_avio_context_io_observer<char_type> baciob) noexcept
{
	return reinterpret_cast<char_type*>(baciob.avios.buf_ptr);
}

template<std::integral char_type>
inline char_type* ibuffer_end(basic_avio_context_io_observer<char_type> baciob) noexcept
{
	return reinterpret_cast<char_type*>(baciob.avios.buf_end);
}

#endif

template<std::integral ch_type>
requires (sizeof(ch_type) == 1)
class basic_avio_context_file:public basic_avio_context_io_observer<ch_type>
{
public:
	using char_type = ch_type;
	using native_handle_type = ::AVIOContext*;
	explicit constexpr basic_avio_context_file(::AVIOContext *ctxx) noexcept:basic_avio_context_io_observer<ch_type>{ctxx}{}
	void close() noexcept
	{
		if(this->avios==nullptr)
		{
			return;
		}
		fast_io::noexcept_call(::av_free,this->avios);
		this->avios=nullptr;
	}
	basic_avio_context_file(basic_avio_context_file const&)=delete;
	basic_avio_context_file& operator=(basic_avio_context_file const&)=delete;
	basic_avio_context_file(basic_avio_context_file&& other) noexcept:basic_avio_context_io_observer<ch_type>{other.avios}
	{
		other.avios=nullptr;
	}
	basic_avio_context_file& operator=(basic_avio_context_file&& other) noexcept
	{
		this->close();
		this->avios=other.avios;
		other.avios=nullptr;
		return *this;
	}
	~basic_avio_context_file()
	{
		this->close();
	}
};

struct avio_buffer
{
	void* buffer{};
	explicit constexpr avio_buffer() = default;
	explicit constexpr avio_buffer(void* bf) noexcept:buffer(bf){}
	void close() noexcept
	{
		if(buffer==nullptr)
		{
			return;
		}
		::fast_io::noexcept_call(::av_free,buffer);
		this->buffer=nullptr;
	}
	avio_buffer(avio_buffer const&)=delete;
	avio_buffer& operator=(avio_buffer const&)=delete;
	avio_buffer(avio_buffer&& other):buffer(other.buffer)
	{
		other.buffer=nullptr;
	}
	avio_buffer& operator=(avio_buffer&& other) noexcept
	{
		this->close();
		this->buffer=other.buffer;
		other.buffer=nullptr;
		return *this;
	}
	~avio_buffer()
	{
		this->close();
	}
};

template<std::integral char_type>
requires (sizeof(char_type) == 1)
struct basic_avio_buffer_context
{
	basic_avio_context_file<char_type> avioctx;
	avio_buffer aviobuffer;
	inline constexpr ::AVIOContext* context() const noexcept
	{
		return avioctx.native_handle();
	}
};

namespace details
{

template<typename T>
concept has_file_status_impl = requires(T t)
{
	{status(t)}->std::same_as<::fast_io::posix_file_status>;
};

template<typename rftype>
inline basic_avio_buffer_context<typename rftype::char_type> create_avio_context_impl(rftype rf)
{
	using char_type = typename rftype::char_type;
	constexpr int buffersize{16384};
	auto bfptr{::fast_io::noexcept_call(::av_malloc,buffersize)};
	if(bfptr==nullptr)
	{
		::fast_io::fast_terminate();
	}
	avio_buffer bufferwrap(bfptr);
	::AVIOContext* ctx;
	using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= char_type*;
	using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= char_type const*;
	static_assert(sizeof(rftype)<=sizeof(void*));
	void *ptr{};
	::memcpy(__builtin_addressof(ptr),__builtin_addressof(rf),sizeof(rftype));
	ctx=::avio_alloc_context(reinterpret_cast<char unsigned*>(bfptr),buffersize,AVIO_FLAG_WRITE,ptr,
	[](void* opaque,uint8_t* ptr,int bufsize) noexcept ->int
	{
		rftype rft;
		::memcpy(__builtin_addressof(rft),__builtin_addressof(opaque),sizeof(rftype));
		try
		{
			auto ret{read(rft,reinterpret_cast<char_type_ptr>(ptr),reinterpret_cast<char_type_ptr>(ptr)+bufsize)-reinterpret_cast<char_type_ptr>(ptr)};
			return static_cast<int>(ret);
		}
		catch(...)
		{
			return -1;
		}
	},
	[](void* opaque,uint8_t* ptr,int bufsize) noexcept ->int
	{
		rftype rft;
		::memcpy(__builtin_addressof(rft),__builtin_addressof(opaque),sizeof(rftype));
		try
		{
			write(rft,reinterpret_cast<char_type_const_ptr>(ptr),reinterpret_cast<char_type_const_ptr>(ptr)+bufsize);
		}
		catch(...)
		{
			return -1;
		}
		return bufsize;
	},
	[](void* opaque,int64_t offset,int whence) noexcept ->::std::int64_t
	{
		rftype rft;
		::memcpy(__builtin_addressof(rft),__builtin_addressof(opaque),sizeof(rftype));
		try
		{
			if(whence==AVSEEK_SIZE)
			{
				if constexpr(has_file_status_impl<rftype>)
				{
					auto sz{status(rft).size};
					if constexpr(std::numeric_limits<decltype(sz)>::max()>INT64_MAX)
					{
						if(sz>INT64_MAX)
						{
							return -1;
						}
					}
					return static_cast<::std::int64_t>(sz);
				}
				else
				{
					return -1;
				}
			}
			return seek(rft,offset,static_cast<::fast_io::seekdir>(whence));
		}
		catch(...)
		{
			return -1;
		}
	});
	if(ctx==nullptr)
	{
		::fast_io::fast_terminate();
	}
	return basic_avio_buffer_context{basic_avio_context_file<char_type>(ctx),::std::move(bufferwrap)};
}

}

template<stream stm>
requires (io_stream<::std::remove_cvref_t<stm>>&&random_access_stream<::std::remove_cvref_t<stm>>&&
	sizeof(typename ::std::remove_cvref_t<stm>::char_type)==1)
inline basic_avio_buffer_context<typename ::std::remove_cvref_t<stm>::char_type> create_avio_context(stm& sm)
{
	return ::fast_io::details::create_avio_context_impl(io_ref(sm));
}

using avio_context_io_observer = basic_avio_context_io_observer<char>;
using u8avio_context_io_observer = basic_avio_context_io_observer<char8_t>;
using avio_context_file = basic_avio_context_file<char>;
using u8avio_context_file = basic_avio_context_file<char8_t>;

struct avformat_context_guard
{
	::AVFormatContext* pFormatCtx{};

	explicit constexpr avformat_context_guard(::AVFormatContext* p) noexcept:pFormatCtx{p}{}

	avformat_context_guard(avformat_context_guard const&)=delete;
	avformat_context_guard& operator=(avformat_context_guard const&)=delete;

	constexpr ::AVFormatContext* release() noexcept
	{
		auto temp{this->pFormatCtx};
		this->pFormatCtx=nullptr;
		return temp;
	}

	void close() noexcept
	{
		if(this->pFormatCtx==nullptr)
		{
			return;
		}
		::fast_io::noexcept_call(avformat_free_context,pFormatCtx);
		this->pFormatCtx=nullptr;
	}

	~avformat_context_guard()
	{
		this->close();
	}	
};

struct avformat_input_guard
{
	::AVFormatContext** ppFormatCtx{};

	explicit constexpr avformat_input_guard(::AVFormatContext** pp) noexcept:ppFormatCtx{pp}{}

	avformat_input_guard(avformat_input_guard const&)=delete;
	avformat_input_guard& operator=(avformat_input_guard const&)=delete;

	constexpr ::AVFormatContext** release() noexcept
	{
		auto temp{this->ppFormatCtx};
		this->ppFormatCtx=nullptr;
		return temp;
	}

	void close() noexcept
	{
		if(ppFormatCtx==nullptr)
		{
			return;
		}
		::fast_io::noexcept_call(avformat_close_input,ppFormatCtx);
		this->ppFormatCtx=nullptr;
	}

	~avformat_input_guard()
	{
		this->close();
	}	
};

}
