#pragma once

namespace fast_io
{

namespace details
{

template<std::integral char_type,typename traits_type>
inline char_type* streambuf_read_report_eh_impl(std::basic_streambuf<char_type,traits_type>* fb,
	char_type* src, char_type* last)
{
	return src+fb->sgetn(src,static_cast<std::streamsize>(last-src));
}

template<std::integral char_type,typename traits_type>
inline char_type const* streambuf_write_report_eh_impl(std::basic_streambuf<char_type,traits_type>* fb,
	char_type const* src, char_type const* last)
{
	return src+fb->sputn(src,static_cast<std::streamsize>(last-src));
}

template<std::integral char_type,typename traits_type>
inline ::fast_io::intfpos_t streambuf_seek_common(
	std::basic_streambuf<char_type,traits_type>* fb,
	::fast_io::intfpos_t off,
	::fast_io::seekdir sdir,::std::ios_base::openmode md)
{
	using offtype = typename traits_type::off_type;
	using postype = typename traits_type::pos_type;
	offtype offtpoff;

	constexpr auto offmn{::std::numeric_limits<offtype>::min()};
	constexpr auto offmx{::std::numeric_limits<offtype>::max()};
	constexpr auto intfposmn{::std::numeric_limits<::fast_io::intfpos_t>::min()};
	constexpr auto intfposmx{::std::numeric_limits<::fast_io::intfpos_t>::max()};
	if constexpr(intfposmn<offmn||offmx<intfposmx)
	{
		if(off<offmn)
		{
			offtpoff=offmn;
		}
		else if(offmx<off)
		{
			offtpoff=offmx;
		}
		else
		{
			offtpoff=static_cast<offtype>(off);
		}
	}
	else
	{
		offtpoff=static_cast<offtype>(off);
	}
	postype ret{fb->pubseekoff(offtpoff,sdir,md)};
	::fast_io::intfpos_t retval;
	if constexpr(intfposmx<::std::numeric_limits<postype>::max())
	{
		if(intfposmx<ret)
		{
			retval = intfposmx;
		}
		else
		{
			retval = static_cast<::fast_io::intfpos_t>(ret);
		}
	}
	return retval;
}

template<std::integral char_type,typename traits_type>
inline ::fast_io::intfpos_t streambuf_seek_input(
	std::basic_streambuf<char_type,traits_type>* fb,
	::fast_io::intfpos_t off,
	::fast_io::seekdir sdir)
{
	return ::fast_io::details::streambuf_seek_common(fb,off,sdir,::std::ios_base::in);
}

template<std::integral char_type,typename traits_type>
inline ::fast_io::intfpos_t streambuf_seek_output(
	std::basic_streambuf<char_type,traits_type>* fb,
	::fast_io::intfpos_t off,
	::fast_io::seekdir sdir)
{
	return ::fast_io::details::streambuf_seek_common(fb,off,sdir,::std::ios_base::out);
}

template<std::integral char_type,typename traits_type>
inline ::fast_io::intfpos_t streambuf_seek_io(
	std::basic_streambuf<char_type,traits_type>* fb,
	::fast_io::intfpos_t off,
	::fast_io::seekdir sdir)
{
	return ::fast_io::details::streambuf_seek_common(fb,off,sdir,
		::std::ios_base::in|::std::ios_base::out);
}

template<std::integral char_type,typename traits_type>
inline void streambuf_flush_impl(std::basic_streambuf<char_type,traits_type>* fb)
{
	if(fb->pubsync()==-1)
		throw_posix_error(EIO);
}

}

template<typename T>
inline ::fast_io::intfpos_t io_stream_seek_define(
	::fast_io::basic_general_streambuf_io_observer<T> fb,
	::fast_io::intfpos_t off,
	::fast_io::seekdir sdir)
{
	return ::fast_io::details::streambuf_seek_io(fb.fb,off,sdir);
}

template<typename T>
inline ::fast_io::intfpos_t input_stream_seek_define(
	::fast_io::basic_general_streambuf_io_observer<T> fb,
	::fast_io::intfpos_t off,
	::fast_io::seekdir sdir)
{
	return ::fast_io::details::streambuf_seek_input(fb.fb,off,sdir);
}

template<typename T>
inline ::fast_io::intfpos_t output_stream_seek_define(
	::fast_io::basic_general_streambuf_io_observer<T> fb,
	::fast_io::intfpos_t off,
	::fast_io::seekdir sdir)
{
	return ::fast_io::details::streambuf_seek_output(fb.fb,off,sdir);
}

template<typename T>
inline typename T::char_type const* write_some_overflow_define(
	::fast_io::basic_general_streambuf_io_observer<T> fb,
	typename T::char_type const* first,
	typename T::char_type const* last)
{
	return ::fast_io::details::streambuf_write_report_eh_impl(fb.fb,first,last);
}

template<typename T>
inline typename T::char_type* read_some_overflow_define(
	::fast_io::basic_general_streambuf_io_observer<T> fb,
	typename T::char_type* first,
	typename T::char_type* last)
{
	return ::fast_io::details::streambuf_read_report_eh_impl(fb.fb,first,last);
}

template<typename T>
inline constexpr ::fast_io::basic_general_streambuf_io_observer<T> io_stream_ref_define(
	::fast_io::basic_general_streambuf_io_observer<T> other) noexcept
{
	return other;
}

template<typename T>
inline constexpr void io_stream_buffer_flush_define(
	::fast_io::basic_general_streambuf_io_observer<T> other)
{
	::fast_io::details::streambuf_flush_impl(other.fb);
}

}
