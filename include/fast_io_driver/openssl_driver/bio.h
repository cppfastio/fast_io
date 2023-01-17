#pragma once
struct bio_method_st
{
int type;
char const *name;
int (*bwrite) (BIO *, const char *, size_t, size_t *) noexcept;
int (*bwrite_old) (BIO *, const char *, int) noexcept;
int (*bread) (BIO *, char *, size_t, size_t *) noexcept;
int (*bread_old) (BIO *, char *, int) noexcept;
int (*bputs) (BIO *, const char *) noexcept;
int (*bgets) (BIO *, char *, int) noexcept;
long (*ctrl) (BIO *, int, long, void *) noexcept;
int (*create) (BIO *) noexcept;
int (*destroy) (BIO *) noexcept;
long (*callback_ctrl) (BIO *, int, BIO_info_cb *) noexcept;
};

namespace fast_io
{

namespace details
{
template<bool detach>
inline constexpr int calculate_bio_new_fp_flags(open_mode om)
{
	int flag{BIO_CLOSE};
	if constexpr(!detach)
		flag=BIO_NOCLOSE;
	if((om&open_mode::text)!=open_mode::none)
		flag|=BIO_FP_TEXT;
	return flag;
}

template<open_mode om,bool detach>
struct bio_new_fp_flags
{
	inline static constexpr int value=calculate_bio_new_fp_flags<detach>(om);
};


inline FILE* bio_to_fp(BIO* bio) noexcept
{
	FILE* fp{};
	::fast_io::noexcept_call(BIO_ctrl,bio,BIO_C_GET_FILE_PTR,0,reinterpret_cast<char*>(__builtin_addressof(fp)));
	return fp;
}

inline int bio_to_fd(BIO* bio) noexcept
{
	auto fp{bio_to_fp(bio)};
	if(fp==nullptr)
	{
		int fd{-1};
		::fast_io::noexcept_call(::BIO_ctrl,bio,BIO_C_GET_FD,0,reinterpret_cast<char*>(__builtin_addressof(fd)));
		return fd;
	}
	return ::fast_io::details::fp_to_fd(fp);
}

template<typename stm>
inline decltype(auto) get_cookie_data_from_void_ptr_data(void* data) noexcept
{
	if constexpr(::std::is_trivially_copyable_v<stm>&&sizeof(stm)<=sizeof(void*))
	{
#if __cpp_lib_bit_cast >= 201806L
		alignas(alignof(stm)) ::fast_io::freestanding::array<char,sizeof(stm)> a;
		my_memcpy(a.data(),__builtin_addressof(data),sizeof(stm));
		return __builtin_bit_cast(stm,a);
#else
		stm s;
		my_memcpy(s,__builtin_addressof(data),sizeof(stm));
		return s;
#endif
	}
	else
	{
		return *reinterpret_cast<stm*>(data);
	}
}

template<typename stm>
inline decltype(auto) get_cookie_data_from_bio_data(BIO* bio) noexcept
{
	return get_cookie_data_from_void_ptr_data<stm>(noexcept_call(BIO_get_data,bio));
}

}

template<typename stm>
requires (stream<std::remove_reference_t<stm>>)
struct bio_io_cookie_functions_t
{
	using native_functions_type = bio_method_st;
	native_functions_type functions{};
	explicit bio_io_cookie_functions_t()
	{
		using value_type = std::remove_reference_t<stm>;
		if constexpr(input_stream<value_type>)
		{
			functions.bread=[](BIO* bbio,char* buf,std::size_t size,std::size_t* readd) noexcept->int
			{
#ifdef __cpp_exceptions
				try
				{
#endif
					*readd=read(::fast_io::details::get_cookie_data_from_bio_data<value_type>(bbio),buf,buf+size)-buf;
					return 1;
#ifdef __cpp_exceptions
				}
				catch(...)
				{
					return -1;
				}
#endif
			};
		}
		if constexpr(output_stream<value_type>)
		{
			functions.bwrite=[](BIO* bbio,char const* buf,std::size_t size,std::size_t* written) noexcept->int
			{
#ifdef __cpp_exceptions
				try
				{
#endif
					decltype(auto) v{::fast_io::details::get_cookie_data_from_bio_data<value_type>(bbio)};
					if constexpr(std::same_as<decltype(write(v,buf,buf+size)),void>)
					{
						write(v,buf,buf+size);
						*written=size;
					}
					else
						*written=write(v,buf,buf+size)-buf;
					return 1;
#ifdef __cpp_exceptions
				}
				catch(...)
				{
					return -1;
				}
#endif
			};
		}
		if constexpr(!std::is_reference_v<stm>&&!std::is_trivially_copyable_v<value_type>)
			functions.destroy=[](BIO* bbio) noexcept -> int
			{
				delete reinterpret_cast<value_type*>(noexcept_call(BIO_get_data,bbio));
				return 1;
			};
#if __cpp_rtti
		functions.name=typeid(stm).name();
		constexpr int value(BIO_TYPE_DESCRIPTOR-BIO_TYPE_START);
		static_assert(0<value);
		functions.type=static_cast<int>(typeid(stm).hash_code()%value+BIO_TYPE_START);
#else
		functions.name=reinterpret_cast<char const*>(u8"unknown");
		constexpr int value(BIO_TYPE_DESCRIPTOR-BIO_TYPE_START);
		static_assert(0<value);
		functions.type=static_cast<int>(BIO_TYPE_START);
#endif
	}
};

template<typename stm>
inline bio_io_cookie_functions_t<stm> const bio_io_cookie_functions{};

namespace details
{

inline BIO* bio_new_stream_type(bio_method_st const* methods)
{
	auto bio{::fast_io::noexcept_call(BIO_new,methods)};
	if(bio==nullptr)
		throw_openssl_error();
	return bio;
}


template<stream stm>
inline BIO* construct_bio_by_t(void* ptr)
{
	auto bp{bio_new_stream_type(__builtin_addressof(bio_io_cookie_functions<stm>.functions))};
	::fast_io::noexcept_call(BIO_set_data,bp,ptr);
	return bp;
}

template<stream stm,typename... Args>
requires (std::is_trivially_copyable_v<stm>&&sizeof(stm)<=sizeof(void*))
inline void* construct_cookie_by_args_trivial(Args&& ...args)
{
	stm s{::std::forward<Args>(args)...};
#if __cpp_lib_bit_cast >= 201806L
	if constexpr(sizeof(stm)==sizeof(void*))
		return __builtin_bit_cast(void*,s);
	else
#endif
	{
		void* p{};
		__builtin_memcpy(__builtin_addressof(p),s,sizeof(stm));
		return p;
	}
}

template<typename stream>
struct bio_stm_scope_ptr
{
	stream* s{};
	explicit constexpr bio_stm_scope_ptr() noexcept = default;
	explicit constexpr bio_stm_scope_ptr(stream *p) noexcept:s{p}{}
	bio_stm_scope_ptr(bio_stm_scope_ptr const&)=delete;
	bio_stm_scope_ptr& operator=(bio_stm_scope_ptr const&)=delete;
	~bio_stm_scope_ptr()
	{
		delete s;
	}
};

template<typename stream>
inline BIO* construct_bio_by_phase2(stream* smptr)
{
	bio_stm_scope_ptr<stream> s(smptr);
	return construct_bio_by_t<stream>(smptr);
}

template<stream stm,typename... Args>
inline BIO* construct_bio_by_args(Args&&... args)
{
	if constexpr(std::is_trivially_copyable_v<stm>&&sizeof(stm)<=sizeof(void*))
	{
		return construct_bio_by_t<stm>(construct_cookie_by_args_trivial<stm>(::std::forward<Args>(args)...));
	}
	else
	{
		return construct_bio_by_phase2<stm>(new stm(::std::forward<Args>(args)...));
	}
}

inline BIO* open_bio_with_fp_phase2(FILE* fp,int om)
{
	auto bio{::fast_io::noexcept_call(BIO_new_fp,fp,om)};
	if(bio==nullptr)[[unlikely]]
		throw_openssl_error();
	return bio;
}

inline BIO* open_bio_with_fp(FILE* fp,::fast_io::open_mode om)
{
	return open_bio_with_fp_phase2(fp,calculate_bio_new_fp_flags<true>(om));
}

}

template<std::integral ch_type>
class basic_bio_io_observer
{
public:
	using native_handle_type = BIO*;
	using char_type = ch_type;
	native_handle_type bio{};
	explicit constexpr operator bool() const noexcept
	{
		return bio;
	}
	constexpr native_handle_type native_handle() const noexcept
	{
		return bio;
	}
	constexpr native_handle_type release() noexcept
	{
		auto temp{bio};
		bio=nullptr;
		return temp;
	}
	template<::fast_io::c_family family>
	explicit operator basic_c_family_io_observer<family,char_type>() const noexcept
	{
		return {details::bio_to_fp(bio)};
	}
#if !defined(__AVR__)
	explicit operator basic_posix_io_observer<char_type>() const noexcept
	{
		return {details::bio_to_fd(bio)};
	}
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
	template<::fast_io::win32_family family>
	explicit operator basic_win32_family_io_observer<family,char_type>() const noexcept
	{
		return basic_win32_io_observer<char_type>(static_cast<basic_posix_io_observer<char_type>>(*this));
	}
	template<nt_family fam>
	explicit operator basic_nt_family_io_observer<fam,char_type>() const noexcept
	{
		return basic_nt_family_io_observer<fam,char_type>(static_cast<basic_posix_io_observer<char_type>>(*this));
	}
#endif
#endif
};
template<std::integral ch_type>
class basic_bio_file:public basic_bio_io_observer<ch_type>
{
public:
	using native_handle_type = BIO*;
	using char_type = ch_type;
	constexpr basic_bio_file()=default;
	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	explicit constexpr basic_bio_file(native_hd bio) noexcept:basic_bio_io_observer<char_type>{bio}{}

	explicit constexpr basic_bio_file(decltype(nullptr)) noexcept=delete;

	constexpr basic_bio_file(basic_bio_io_observer<ch_type>) noexcept=delete;
	constexpr basic_bio_file& operator=(basic_bio_io_observer<ch_type>) noexcept=delete;

	template<stream stm,typename ...Args>
	requires (!std::is_reference_v<stm>&&std::constructible_from<stm,Args...>)
	basic_bio_file(::fast_io::io_cookie_type_t<stm>,Args&& ...args):
		basic_bio_io_observer<char_type>{details::construct_bio_by_args<stm>(::std::forward<Args>(args)...)}
	{
	}
	template<c_family family>
	basic_bio_file(basic_c_family_file<family,char_type>&& bmv,fast_io::open_mode om):
		basic_bio_io_observer<char_type>{::fast_io::details::open_bio_with_fp(bmv.fp,om)}
	{
		bmv.fp=nullptr;
	}
	basic_bio_file(basic_posix_file<char_type>&& bmv,fast_io::open_mode om):
		basic_bio_file(basic_c_file<char_type>(::std::move(bmv),om),om){}
#if (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
	template<win32_family family>
	basic_bio_file(basic_win32_family_file<family,char_type>&& win32_handle,fast_io::open_mode om):
		basic_bio_file(basic_posix_file<char_type>(::std::move(win32_handle),om),om)
	{
	}
	template<nt_family family>
	basic_bio_file(basic_nt_family_file<family,char_type>&& nt_handle,open_mode om):
		basic_bio_file(basic_posix_file<char_type>(::std::move(nt_handle),om),to_native_c_mode(om))
	{
	}
#endif
	template<::fast_io::constructible_to_os_c_str T>
	basic_bio_file(T const& file,open_mode om,perms pm=static_cast<perms>(436)):
		basic_bio_file(basic_c_file<char_type>(file,om,pm),om)
	{}
	template<::fast_io::constructible_to_os_c_str T>
	basic_bio_file(native_at_entry nate,T const& file,open_mode om,perms pm=static_cast<perms>(436)):
		basic_bio_file(basic_c_file<char_type>(nate,file,om,pm),om)
	{}
	inline constexpr void reset(native_handle_type newhandle=nullptr) noexcept
	{
		if(this->bio)[[likely]]
			noexcept_call(BIO_free,this->bio);
		this->bio=newhandle;
	}
	basic_bio_file(basic_bio_file const&)=delete;
	basic_bio_file& operator=(basic_bio_file const&)=delete;
	constexpr basic_bio_file(basic_bio_file&& __restrict bf) noexcept:basic_bio_io_observer<char_type>(bf.bio)
	{
		bf.bio=nullptr;
	}
	basic_bio_file& operator=(basic_bio_file&& __restrict bf) noexcept
	{
		if(this->bio)[[likely]]
			noexcept_call(BIO_free,this->bio);
		this->bio=bf.bio;
		bf.bio=nullptr;
		return *this;
	}
	void close()
	{
		if(this->bio)[[likely]]
		{
			int ret{noexcept_call(BIO_free,this->bio)};
			this->bio=nullptr;
			if(!ret)[[unlikely]]
				throw_openssl_error();
		}
	}
	~basic_bio_file()
	{
		if(this->bio)[[likely]]
			noexcept_call(BIO_free,this->bio);
	}
};

using bio_io_observer =  basic_bio_io_observer<char>;
using bio_file =  basic_bio_file<char>;
using wbio_io_observer =  basic_bio_io_observer<wchar_t>;
using wbio_file =  basic_bio_file<wchar_t>;
using u8bio_io_observer =  basic_bio_io_observer<char8_t>;
using u8bio_file =  basic_bio_file<char8_t>;
using u16bio_io_observer =  basic_bio_io_observer<char16_t>;
using u16bio_file =  basic_bio_file<char16_t>;
using u32bio_io_observer =  basic_bio_io_observer<char32_t>;
using u32bio_file =  basic_bio_file<char32_t>;

namespace details
{
inline std::size_t bio_read_impl(BIO* bio,void* address,std::size_t size)
{
	std::size_t read_bytes{};
	if(::fast_io::noexcept_call(BIO_read_ex,bio,address,size,__builtin_addressof(read_bytes))==-1)
		throw_openssl_error();
	return read_bytes;
}

inline std::size_t bio_write_impl(BIO* bio,void const* address,std::size_t size)
{
	std::size_t written_bytes{};
	if(::fast_io::noexcept_call(BIO_write_ex,bio,address,size,__builtin_addressof(written_bytes))==-1)
		throw_openssl_error();
	return written_bytes;
}
#if 0
inline posix_file_status bio_status_impl(BIO* bio)
{
	return status(posix_io_observer{::fast_io::details::bio_to_fd(bio)});
}
#endif
}

template<std::integral ch_type,::std::contiguous_iterator Iter>
inline Iter read(basic_bio_io_observer<ch_type> iob,Iter begin,Iter end)
{
	return begin+details::bio_read_impl(iob.bio,::std::to_address(begin),static_cast<std::size_t>(end-begin)*sizeof(*begin))/sizeof(*begin);
}

template<std::integral ch_type,::std::contiguous_iterator Iter>
inline Iter write(basic_bio_io_observer<ch_type> iob,Iter begin,Iter end)
{
	return begin+details::bio_write_impl(iob.bio,::std::to_address(begin),static_cast<std::size_t>(end-begin)*sizeof(*begin))/sizeof(*begin);
}

#if __cpp_lib_three_way_comparison >= 201907L
template<std::integral ch_type>
inline constexpr bool operator==(basic_bio_io_observer<ch_type> a,basic_bio_io_observer<ch_type> b) noexcept
{
	return a.bio==b.bio;
}

template<std::integral ch_type>
inline constexpr auto operator<=>(basic_bio_io_observer<ch_type> a,basic_bio_io_observer<ch_type> b) noexcept
{
	return a.bio<=>b.bio;
}
#endif

template<std::integral ch_type>
inline constexpr basic_bio_io_observer<ch_type> io_value_handle(basic_bio_io_observer<ch_type> other) noexcept
{
	return other;
}

template<std::integral ch_type>
inline constexpr posix_at_entry at(basic_bio_io_observer<ch_type> bio) noexcept
{
	return posix_at_entry{details::bio_to_fd(bio.bio)};
}
#if 0
template<std::integral ch_type>
inline constexpr posix_file_status status(basic_bio_io_observer<ch_type> bio)
{
	return details::bio_status_impl(bio.bio);
}
#endif
namespace details
{

template<output_stream output>
inline void print_define_openssl_error(output out)
{
	if constexpr(std::same_as<output,bio_io_observer>)
	{
		::fast_io::noexcept_call(ERR_print_errors,out.bio);
	}
	else
	{
		bio_file bf(io_cookie_type<output>,out);
		::fast_io::noexcept_call(ERR_print_errors,bf.bio);		
	}
}

}

template<output_stream output>
requires (std::is_trivially_copyable_v<output>&&std::same_as<typename output::char_type,char>)
inline void print_define(io_reserve_type_t<char,openssl_error>,output out,openssl_error)
{
	::fast_io::details::print_define_openssl_error(out);
}

}
