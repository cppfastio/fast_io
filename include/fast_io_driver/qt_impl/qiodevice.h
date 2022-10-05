#pragma once
namespace fast_io
{

template<std::integral ch_type,typename T>
class basic_general_qdevice_io_observer
{
public:
	using char_type = ch_type;
	using value_type = T;
	using native_handle_type = T*;
	native_handle_type qdevice{};

	inline constexpr native_handle_type native_handle() const noexcept
	{
		return qdevice;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{qdevice};
		qdevice=nullptr;
		return temp;
	}

	inline operator basic_posix_io_observer<char_type>() requires requires()
	{
		{this->qdevice->handle()}->std::same_as<int>;
	}
	{
		return basic_posix_io_observer<char_type>{this->qdevice->handle()};
	}
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
	inline operator basic_win32_io_observer<char_type>() requires requires()
	{
		{this->qdevice->handle()}->std::same_as<int>;
	}
	{
		return static_cast<basic_win32_io_observer<char_type>>(
			static_cast<basic_posix_io_observer<char_type>>(*this)
		);
	}
	template<nt_family family>
	inline operator basic_nt_family_io_observer<family,char_type>() requires requires()
	{
		{this->qdevice->handle()}->std::same_as<int>;
	}
	{
		return static_cast<basic_nt_family_io_observer<family,char_type>>(
			static_cast<basic_posix_io_observer<char_type>>(*this)
		);
	}
#endif
};

namespace details
{

inline std::size_t qio_device_write_impl(QIODevice* __restrict qdevice,void const* data,std::size_t bytes)
{
	std::int_least64_t res{qdevice->write(reinterpret_cast<char const*>(data),static_cast<std::int_least64_t>(bytes))};
	if(res<0)
		throw_qt_error(qdevice->errorString());
	return static_cast<std::size_t>(res);
}

inline std::size_t qio_device_read_impl(QIODevice* __restrict qdevice,void* data,std::size_t bytes)
{
	std::int_least64_t res{qdevice->read(reinterpret_cast<char*>(data),static_cast<std::int_least64_t>(bytes))};
	if(res<0)
		throw_qt_error(qdevice->errorString());
	return static_cast<std::size_t>(res);
}


inline std::uintmax_t qio_device_seek_impl(QIODevice* __restrict qdevice,std::intmax_t offset,seekdir dir)
{
	if(dir==seekdir::beg)
		offset=static_cast<std::intmax_t>(qdevice->pos())+offset;
	else if(dir==seekdir::end)
		offset=static_cast<std::intmax_t>(qdevice->size()-qdevice->pos())+offset;
	if(!qdevice->seek(static_cast<std::int_least64_t>(offset)))
		throw_qt_error(qdevice->errorString());
	return static_cast<std::uintmax_t>(qdevice->pos());
}

}

template<std::integral ch_type,typename T,::std::contiguous_iterator Iter>
inline Iter write(basic_general_qdevice_io_observer<ch_type,T> qiob,Iter begin,Iter end)
{
	return begin+details::qio_device_write_impl(qiob.qdevice,::std::to_address(begin),
		static_cast<std::size_t>(::std::to_address(end)-::std::to_address(begin))*sizeof(*begin))/sizeof(*begin);
}

template<std::integral ch_type,typename T,::std::contiguous_iterator Iter>
inline Iter read(basic_general_qdevice_io_observer<ch_type,T> qiob,Iter begin,Iter end)
{
	return begin+details::qio_device_read_impl(qiob.qdevice,::std::to_address(begin),
		static_cast<std::size_t>(::std::to_address(end)-::std::to_address(begin))*sizeof(*begin))/sizeof(*begin);
}

template<std::integral ch_type,typename T>
requires (sizeof(ch_type)==1)
inline std::pair<ch_type,bool> try_get(basic_general_qdevice_io_observer<ch_type,T> qiob)
{
	char ch;
	bool ef{qiob.qdevice->getChar(&ch)};
	return {static_cast<ch_type>(ch),ef};
}

template<std::integral ch_type,typename T>
requires (sizeof(ch_type)==1)
inline void try_unget(basic_general_qdevice_io_observer<ch_type,T> qiob,ch_type ch) noexcept
{
	qiob.qdevice->ungetChar(ch);
}

template<std::integral ch_type,typename T>
inline std::uintmax_t seek(basic_general_qdevice_io_observer<ch_type,T> qiob,std::intmax_t offset=0,seekdir s=seekdir::cur)
{
	return qio_device_seek_impl(qiob->qdevice,offset,s);
}
template<std::integral char_type>
using basic_qiodevice_io_observer = basic_general_qdevice_io_observer<char_type,QIODevice>;
template<std::integral char_type>
using basic_qt_io_observer = basic_general_qdevice_io_observer<char_type,QFile>;

template<std::integral ch_type,typename T>
inline constexpr basic_qiodevice_io_observer<ch_type> io_value_handle(basic_general_qdevice_io_observer<ch_type,T> gqiob) noexcept
{
	return {gqiob.qdevice};
}

template<typename T,std::integral ch_type>
requires requires(basic_general_qdevice_io_observer<T,ch_type> piob)
{
	{piob.qdevice->handle()}->std::same_as<int>;
}
inline constexpr posix_at_entry at(basic_general_qdevice_io_observer<T,ch_type> piob) noexcept
{
	return posix_at_entry{piob.qdevice->handle()};
}

using qiodevice_io_observer = basic_qiodevice_io_observer<char>;
using wqiodevice_io_observer = basic_qiodevice_io_observer<wchar_t>;
using u8qiodevice_io_observer = basic_qiodevice_io_observer<char8_t>;
using u16qiodevice_io_observer = basic_qiodevice_io_observer<char16_t>;
using u32qiodevice_io_observer = basic_qiodevice_io_observer<char32_t>;

using qt_io_observer = basic_qt_io_observer<char>;
using wqt_io_observer = basic_qt_io_observer<wchar_t>;
using u8qt_io_observer = basic_qt_io_observer<char8_t>;
using u16qt_io_observer = basic_qt_io_observer<char16_t>;
using u32qt_io_observer = basic_qt_io_observer<char32_t>;

}

