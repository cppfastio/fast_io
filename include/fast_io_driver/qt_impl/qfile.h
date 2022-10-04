#pragma once

namespace fast_io
{

namespace details
{

inline constexpr int to_qt_open_mode(open_mode mode) noexcept
{
	int res{};
	if((mode&open_mode::in)==open_mode::in)
		res|=QIODevice::ReadOnly;
	if((mode&open_mode::out)==open_mode::out)
		res|=QIODevice::WriteOnly;
	if((mode&open_mode::app)==open_mode::app)
		res|=QIODevice::Append;
	if((mode&open_mode::trunc)==open_mode::trunc)
		res|=QIODevice::Truncate;
	if((mode&open_mode::text)==open_mode::text)
		res|=QIODevice::Text;
	if((mode&open_mode::excl)==open_mode::excl)
		res|=QIODevice::NewOnly;
	if((mode&open_mode::directory)==open_mode::directory)
	{
		if((mode&open_mode::in)!=open_mode::in)
			res|=QIODevice::ReadOnly;
	}
	return res;
}

struct qfile_scoped_guard
{
	QFile* qf{new QFile};
	explicit qfile_scoped_guard() = default;
	qfile_scoped_guard(qfile_scoped_guard const&)=delete;
	qfile_scoped_guard& operator=(qfile_scoped_guard const&)=delete;
	inline constexpr QFile* release() noexcept
	{
		auto temp{qf};
		qf=nullptr;
		return temp;
	}
	~qfile_scoped_guard()
	{
		if(qf)
			delete qf;
	}
};

inline QFile* open_qfile_with_fp_internal(FILE* fp,QIODevice::OpenModeFlag mode)
{
	qfile_scoped_guard guard;
	guard.qf->open(fp,mode,QFileDevice::AutoCloseHandle);
	return guard.release();
}

inline QFile* open_qfile_with_fp(FILE* fp,open_mode mode)
{
	return open_qfile_with_fp_internal(fp,static_cast<QIODevice::OpenModeFlag>(to_qt_open_mode(mode)));
}

inline QFile* open_qfile_with_fd_internal(int fd,QIODevice::OpenModeFlag mode)
{
	qfile_scoped_guard guard;
	guard.qf->open(fd,mode,QFileDevice::AutoCloseHandle);
	return guard.release();
}

inline QFile* open_qfile_with_fd(int fd,open_mode mode)
{
	return open_qfile_with_fd_internal(fd,static_cast<QIODevice::OpenModeFlag>(to_qt_open_mode(mode)));
}

}

template<std::integral ch_type>
class basic_qt_file:public basic_qt_io_observer<ch_type>
{
public:
	using char_type = ch_type;
	using native_handle_type = typename basic_qt_io_observer<ch_type>::native_handle_type;
	constexpr basic_qt_file()=default;
	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	explicit constexpr basic_qt_file(native_hd qdevice):basic_qt_io_observer<char_type>{qdevice}{}

	explicit constexpr basic_qt_file(decltype(nullptr)) noexcept=delete;

	constexpr basic_qt_file(basic_qt_io_observer<ch_type>) noexcept=delete;
	constexpr basic_qt_file& operator=(basic_qt_io_observer<ch_type>) noexcept=delete;

	template<c_family family>
	basic_qt_file(basic_c_family_file<family,ch_type>&& cioh,open_mode mode):basic_qt_io_observer<char_type>{::fast_io::details::open_qfile_with_fp(cioh.fp,mode)}
	{
		cioh.fp=nullptr;
	}
#if !defined(__AVR__)
	basic_qt_file(basic_posix_file<ch_type>&& pioh,open_mode mode):basic_qt_io_observer<char_type>{::fast_io::details::open_qfile_with_fd(pioh.fd,mode)}
	{
		pioh.fd=-1;
	}
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
	template<win32_family family>
	basic_qt_file(basic_win32_family_file<family,ch_type>&& wioh,open_mode mode):
		basic_qt_file(basic_posix_file<ch_type>(::std::move(wioh),mode),mode)
	{}
	template<nt_family family>
	basic_qt_file(basic_nt_family_file<family,ch_type>&& wioh,open_mode mode):
		basic_qt_file(basic_posix_file<ch_type>(::std::move(wioh),mode),mode)
	{}
#endif
	basic_qt_file(native_fs_dirent fsdirent,open_mode om,perms pm=static_cast<perms>(436)):
		basic_qt_file(basic_posix_file<char_type>(fsdirent,om,pm),om)
	{}
	template<::fast_io::constructible_to_os_c_str T>
	basic_qt_file(T const& file,open_mode om,perms pm=static_cast<perms>(436)):
		basic_qt_file(basic_posix_file<char_type>(file,om,pm),om)
	{}
	template<::fast_io::constructible_to_os_c_str T>
	basic_qt_file(native_at_entry nate,T const& file,open_mode om,perms pm=static_cast<perms>(436)):
		basic_qt_file(basic_posix_file<char_type>(nate,file,om,pm),om)
	{}
#endif
	basic_qt_file(basic_qt_file const&)=delete;
	basic_qt_file& operator=(basic_qt_file const&)=delete;
	
	void close()
	{
		if(this->qdevice)[[likely]]
		{
			struct delete_guard
			{
				native_handle_type& qdevicer;
				delete_guard(native_handle_type& qdevice):qdevicer(qdevice){}
				delete_guard(delete_guard const&)=delete;
				delete_guard& operator=(delete_guard const&)=delete;
				~delete_guard()
				{
					delete qdevicer;
					qdevicer=nullptr;
				}
			}guard{this->qdevice};
			this->qdevice->close();
		}
	}
private:
	void close_nothrow() noexcept
	{
#ifdef __cpp_exceptions
		try
		{
#endif
			this->close();
#ifdef __cpp_exceptions
		}
		catch(...)
		{
		}
#endif
	}
public:

	constexpr basic_qt_file(basic_qt_file&& __restrict other) noexcept:
		basic_qt_io_observer<ch_type>{other.release()}
	{}
	basic_qt_file& operator=(basic_qt_file&& __restrict other) noexcept
	{
		close_nothrow();
		this->qdevice=other.release();
		return *this;
	}
	~basic_qt_file()
	{
		close_nothrow();
	}
};

using qt_file = basic_qt_file<char>;
using wqt_file = basic_qt_file<wchar_t>;
using u8qt_file = basic_qt_file<char8_t>;
using u16qt_file = basic_qt_file<char16_t>;
using u32qt_file = basic_qt_file<char32_t>;

}
