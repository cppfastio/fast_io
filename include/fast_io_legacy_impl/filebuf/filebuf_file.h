#pragma once

namespace fast_io
{

template<std::integral CharT,typename Traits = std::char_traits<CharT>>
class basic_filebuf_file:public basic_filebuf_io_observer<CharT,Traits>
{
public:
	using char_type = CharT;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using traits_type = Traits;
	using native_handle_type = std::basic_filebuf<char_type,traits_type>*;
	constexpr basic_filebuf_file() noexcept=default;
	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	constexpr basic_filebuf_file(native_hd fb) noexcept:basic_filebuf_io_observer<CharT,Traits>{fb}{}
	constexpr basic_filebuf_file(basic_filebuf_io_observer<CharT,Traits>) noexcept=delete;
	constexpr basic_filebuf_file& operator=(basic_filebuf_io_observer<CharT,Traits>) noexcept=delete;

	basic_filebuf_file(decltype(nullptr)) = delete;

	template<c_family family>
	basic_filebuf_file(::fast_io::io_construct_t,basic_c_family_io_observer<family,char_type> ciob,open_mode mode):
		basic_filebuf_io_observer<CharT,Traits>{::fast_io::details::streambuf_hack::open_hacked_basic_filebuf<CharT,Traits>(ciob.fp,mode)}
	{
	}
	template<c_family family>
	basic_filebuf_file(basic_c_family_file<family,char_type>&& chd,open_mode mode):
		basic_filebuf_io_observer<CharT,Traits>{::fast_io::details::streambuf_hack::open_hacked_basic_filebuf<CharT,Traits>(chd.fp,mode)}
	{
		chd.fp=nullptr;
	}

	basic_filebuf_file(io_temp_t):basic_filebuf_file(::fast_io::basic_c_file_unlocked<char_type>(::fast_io::io_temp),::fast_io::open_mode::in|::fast_io::open_mode::out)
	{
	}
#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__) || defined(_MSVC_STL_UPDATE)
#if !defined(__AVR__) && !defined(_GLIBCXX_USE_STDIO_PURE)
	basic_filebuf_file(basic_posix_file<char_type>&& piohd,open_mode mode):
		basic_filebuf_file(basic_c_file_unlocked<char_type>(::std::move(piohd),mode),mode){}
#if (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
//windows specific. open posix file from win32 io handle
	template<win32_family family>
	basic_filebuf_file(basic_win32_family_file<family,char_type>&& win32_handle,open_mode mode):
		basic_filebuf_file(basic_posix_file<char_type>(::std::move(win32_handle),mode),mode)
	{
	}
	template<nt_family family>
	basic_filebuf_file(basic_nt_family_file<family,char_type>&& nt_handle,open_mode mode):
		basic_filebuf_file(basic_posix_file<char_type>(::std::move(nt_handle),mode),mode)
	{
	}
#endif

	basic_filebuf_file(native_fs_dirent fsdirent,open_mode om,perms pm=static_cast<perms>(436)):
		basic_filebuf_file(basic_posix_file<char_type>(fsdirent,om,pm),om)
	{}
	template<::fast_io::constructible_to_os_c_str T>
	basic_filebuf_file(T const& file,open_mode om,perms pm=static_cast<perms>(436)):
		basic_filebuf_file(basic_posix_file<char_type>(file,om,pm),om)
	{}
	template<::fast_io::constructible_to_os_c_str T>
	basic_filebuf_file(native_at_entry nate,T const& file,open_mode om,perms pm=static_cast<perms>(436)):
		basic_filebuf_file(basic_posix_file<char_type>(nate,file,om,pm),om)
	{}
#endif
#endif
	basic_filebuf_file& operator=(basic_filebuf_file const&)=delete;
	basic_filebuf_file(basic_filebuf_file const&)=delete;
	basic_filebuf_file(basic_filebuf_file&& __restrict other) noexcept:basic_filebuf_io_observer<CharT,Traits>{other.release()}{}

	basic_filebuf_file& operator=(basic_filebuf_file&& __restrict bf) noexcept
	{
		delete this->fb;
		this->fb=bf.release();
		return *this;
	}
	void close()
	{
		if(this->fb)[[likely]]
		{
			struct delete_guard
			{
				native_handle_type& fbr;
				delete_guard(native_handle_type& fb):fbr(fb){}
				delete_guard(delete_guard const&)=delete;
				delete_guard& operator=(delete_guard const&)=delete;
				~delete_guard()
				{
					delete fbr;
					fbr=nullptr;
				}
			};
			delete_guard guard{this->fb};
			if(this->fb->close()==nullptr)[[unlikely]]
				throw_posix_error();
		}
	}
	void reset(native_handle_type fb=nullptr) noexcept
	{
		delete this->fb;
		this->fb=fb;
	}
	~basic_filebuf_file()
	{
		delete this->fb;
	}
};

using filebuf_file=basic_filebuf_file<char>;
using wfilebuf_file=basic_filebuf_file<wchar_t>;

namespace freestanding
{

template<std::integral CharT,typename Traits>
struct is_trivially_relocatable<basic_filebuf_file<CharT,Traits>>
{
	inline static constexpr bool value = true;
};

template<std::integral CharT,typename Traits>
struct is_zero_default_constructible<basic_filebuf_file<CharT,Traits>>
{
	inline static constexpr bool value = true;
};

}

}
