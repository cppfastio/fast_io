#pragma once

namespace fast_io::llvm
{

namespace details
{

inline ::llvm::raw_fd_ostream* open_llvm_raw_fd_ostream_from_fd(int fd)
{
	return new ::llvm::raw_fd_ostream(fd,true);
}

inline ::llvm::raw_fd_ostream* open_llvm_raw_fd_ostream_from_fd_dup(::llvm::raw_fd_ostream* f)
{
	return open_llvm_raw_fd_ostream_from_fd(::fast_io::details::sys_dup(hack_fd_from_llvm_raw_fd_ostream(f)));
}

inline ::llvm::raw_fd_ostream* open_llvm_raw_fd_ostream_from_fd_dup2(::llvm::raw_fd_ostream* thisfdos,::llvm::raw_fd_ostream* otherfdos)
{
	if(thisfdos==nullptr)
		return open_llvm_raw_fd_ostream_from_fd_dup(otherfdos);
	hack_set_fd_to_llvm_raw_fd_ostream(thisfdos,::fast_io::details::sys_dup2(hack_fd_from_llvm_raw_fd_ostream(otherfdos),hack_fd_from_llvm_raw_fd_ostream(thisfdos)));
	return thisfdos;
}

}

template<std::integral ch_type>
requires (sizeof(ch_type)==1)
class basic_raw_fd_ostream_file:public basic_raw_fd_ostream_io_observer<ch_type>
{
public:
	using char_type = ch_type;
	using native_handle_type = typename basic_raw_fd_ostream_io_observer<char_type>::native_handle_type;
	explicit constexpr basic_raw_fd_ostream_file() noexcept = default;

	constexpr basic_raw_fd_ostream_file(basic_raw_fd_ostream_io_observer<ch_type>) noexcept=delete;
	constexpr basic_raw_fd_ostream_file& operator=(basic_raw_fd_ostream_io_observer<ch_type>) noexcept=delete;

	template<typename T>
	requires std::same_as<T,::llvm::raw_fd_ostream>
	explicit constexpr basic_raw_fd_ostream_file(T* p) noexcept:basic_raw_fd_ostream_io_observer<char_type>{p}{}
	constexpr basic_raw_fd_ostream_file(basic_raw_fd_ostream_file&& __restrict other) noexcept:basic_raw_fd_ostream_io_observer<char_type>{other.os}
	{
		other.os=nullptr;
	}
	basic_raw_fd_ostream_file& operator=(basic_raw_fd_ostream_file&& __restrict other) noexcept
	{
		delete this->os;
		this->os=other.os;
		other.os=nullptr;
	}
	basic_raw_fd_ostream_file(basic_raw_fd_ostream_file const& other):basic_raw_fd_ostream_io_observer<char_type>{open_llvm_raw_fd_ostream_from_fd_dup(other.os)}{}
	basic_raw_fd_ostream_file& operator=(basic_raw_fd_ostream_file const& other)
	{
		this->os=::fast_io::llvm::details::open_llvm_raw_fd_ostream_from_fd_dup2(this->os,other.os);
		return *this;
	}
	basic_raw_fd_ostream_file(basic_posix_file<char_type>&& pioh,::fast_io::open_mode):
		basic_raw_fd_ostream_io_observer<char_type>{new ::llvm::raw_fd_ostream(pioh.fd,true)}
	{
		pioh.fd=-1;
	}
#if (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
//windows specific. open posix file from win32 io handle
	template<win32_family family>
	basic_raw_fd_ostream_file(basic_win32_family_file<family,char_type>&& win32_handle,open_mode mode):
		basic_raw_fd_ostream_file(basic_posix_file<char_type>(::std::move(win32_handle),mode),mode)
	{
	}
	template<nt_family family>
	basic_raw_fd_ostream_file(basic_nt_family_file<family,char_type>&& nt_handle,open_mode mode):
		basic_raw_fd_ostream_file(basic_posix_file<char_type>(::std::move(nt_handle),mode),mode)
	{
	}
#endif
	void close()
	{
		if(this->os)
		{
			struct destroy
			{
				native_handle_type& hd;
				

				~destroy()
				{
					delete hd;
					hd=nullptr;
				}
			}des{this->os};
			this->os.close();
		}
	}
	~basic_raw_fd_ostream_file()
	{
		delete this->os;
	}
	
	basic_raw_fd_ostream_file(native_fs_dirent fsdirent,open_mode om,perms pm=static_cast<perms>(436)):
		basic_raw_fd_ostream_file(basic_posix_file<char_type>(fsdirent,om,pm),om)
	{}
	template<::fast_io::constructible_to_os_c_str T>
	basic_raw_fd_ostream_file(T const& file,open_mode om,perms pm=static_cast<perms>(436)):
		basic_raw_fd_ostream_file(basic_posix_file<char_type>(file,om,pm),om)
	{}
	template<::fast_io::constructible_to_os_c_str T>
	basic_raw_fd_ostream_file(native_at_entry nate,T const& file,open_mode om,perms pm=static_cast<perms>(436)):
		basic_raw_fd_ostream_file(basic_posix_file<char_type>(nate,file,om,pm),om)
	{}
};

using raw_fd_ostream_file=basic_raw_fd_ostream_file<char>;
using u8raw_fd_ostream_file=basic_raw_fd_ostream_file<char8_t>;

}
