#pragma once
// https://www.ginac.de/~kreckel/fileno/
#include <ext/stdio_sync_filebuf.h>
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif

namespace fast_io::details
{

inline constexpr ::std::ios::openmode calculate_fstream_file_open_mode(open_mode) noexcept;

namespace streambuf_hack
{

template <typename CharT, typename Traits>
class hack_libstdcxx_basic_filebuf : private ::std::basic_filebuf<CharT, Traits>
{
public:
	using ::std::basic_filebuf<CharT, Traits>::_M_lock;
	using ::std::basic_filebuf<CharT, Traits>::_M_file;
	using ::std::basic_filebuf<CharT, Traits>::_M_mode;
	using ::std::basic_filebuf<CharT, Traits>::_M_buf_size;
	using ::std::basic_filebuf<CharT, Traits>::_M_set_buffer;
	using ::std::basic_filebuf<CharT, Traits>::_M_reading;
	using ::std::basic_filebuf<CharT, Traits>::_M_writing;
	using ::std::basic_filebuf<CharT, Traits>::_M_allocate_internal_buffer;
};

template <typename char_type, typename traits_type>
#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline FILE *fp_hack_impl(::std::basic_filebuf<char_type, traits_type> *fbuf) noexcept
{
	using hack_filebuf_type = hack_libstdcxx_basic_filebuf<char_type, traits_type>;
	auto pfbf{reinterpret_cast<char unsigned *>(fbuf)};
	return reinterpret_cast<decltype(hack_filebuf_type::_M_file) *>(pfbf +
																	__builtin_offsetof(hack_filebuf_type, _M_file))
		->file();
}

template <typename char_type, typename traits_type>
#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline FILE *fp_hack_fbf(::std::basic_filebuf<char_type, traits_type> *fbuf) noexcept
{
	if (fbuf == nullptr)
	{
		return nullptr;
	}
	return ::fast_io::details::streambuf_hack::fp_hack_impl(fbuf);
}

template <typename T>
#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline FILE *fp_hack(T *fb) noexcept
{
	using char_type = typename T::char_type;
	using traits_type = typename T::traits_type;
	if constexpr (::std::derived_from<T, ::std::basic_filebuf<char_type, traits_type>>)
	{
		return ::fast_io::details::streambuf_hack::fp_hack_fbf(fb);
	}
	else if constexpr (::std::same_as<T, ::std::basic_streambuf<char_type, traits_type>> &&
					   ::std::same_as<::std::char_traits<char_type>, traits_type>)
	{
		if (fb) [[likely]]
		{
#ifdef __cpp_rtti
			auto syncfb{dynamic_cast<::__gnu_cxx::stdio_sync_filebuf<char_type, traits_type> *>(fb)};
			if (syncfb)
			{
				return syncfb->file();
			}
			auto fbfb{dynamic_cast<::std::basic_filebuf<char_type, traits_type> *>(fb)};
			if (fbfb)
			{
				return ::fast_io::details::streambuf_hack::fp_hack_impl(fbfb);
			}
#else
			if constexpr (::std::same_as<char_type, char> || ::std::same_as<char_type, wchar_t> ||
						  ::std::same_as<char_type, char8_t> || ::std::same_as<char_type, char16_t> ||
						  ::std::same_as<char_type, char32_t>)
			{
				char const *my_type{::fast_io::rtti_hack::abi_type_info_name_or_nullptr(fb)};
				if (my_type)
				{
					::std::size_t mytypelen{::fast_io::cstr_len(my_type)};
					if (::fast_io::details::symbol_cmp_equal(::fast_io::details::libstdcxx_streambufname<2, char_type>, my_type, mytypelen))
					{
						return static_cast<::__gnu_cxx::stdio_sync_filebuf<char_type, traits_type> *>(fb)->file();
					}
					else if (::fast_io::details::symbol_cmp_equal(::fast_io::details::libstdcxx_streambufname<0, char_type>, my_type, mytypelen) ||
							 ::fast_io::details::symbol_cmp_equal(::fast_io::details::libstdcxx_streambufname<1, char_type>, my_type, mytypelen))
					{
						return ::fast_io::details::streambuf_hack::fp_hack_impl(static_cast<::std::basic_filebuf<char_type, traits_type> *>(fb));
					}
				}
			}
#endif
		}
	}
	return nullptr;
}

template <typename CharT, typename Traits>
inline void open_libstdcxx_basic_filebuf_ios_base_open_mode_common(::std::basic_filebuf<CharT, Traits> *ptr_fbf,
																   ::std::ios_base::openmode mode)
{
	using hack_filebuf_type = hack_libstdcxx_basic_filebuf<CharT, Traits>;
	constexpr auto libstdcxx_filebuf_M_set_buffer{&hack_filebuf_type::_M_set_buffer};
	constexpr auto libstdcxx_filebuf_M_allocate_internal_buffer_ptr{&hack_filebuf_type::_M_allocate_internal_buffer};
	auto &fbf_ref{*ptr_fbf};
	auto pfbf{reinterpret_cast<char unsigned *>(ptr_fbf)};
	auto &libstdcxx_filebuf_m_mode{*reinterpret_cast<decltype(hack_filebuf_type::_M_mode) *>(
		pfbf + __builtin_offsetof(hack_filebuf_type, _M_mode))};
	auto &libstdcxx_filebuf_m_buf_size{*reinterpret_cast<decltype(hack_filebuf_type::_M_buf_size) *>(
		pfbf + __builtin_offsetof(hack_filebuf_type, _M_buf_size))};

	auto &libstdcxx_filebuf_m_reading{*reinterpret_cast<decltype(hack_filebuf_type::_M_reading) *>(
		pfbf + __builtin_offsetof(hack_filebuf_type, _M_reading))};
	auto &libstdcxx_filebuf_m_writing{*reinterpret_cast<decltype(hack_filebuf_type::_M_writing) *>(
		pfbf + __builtin_offsetof(hack_filebuf_type, _M_writing))};

	libstdcxx_filebuf_m_mode = mode;
	constexpr ::std::size_t buffer_size{fast_io::details::cal_buffer_size<CharT, true>()};
	libstdcxx_filebuf_m_buf_size = buffer_size;
	(fbf_ref.*libstdcxx_filebuf_M_allocate_internal_buffer_ptr)();
	libstdcxx_filebuf_m_reading = false;
	libstdcxx_filebuf_m_writing = false;
	(fbf_ref.*libstdcxx_filebuf_M_set_buffer)(-1);
}

template <typename CharT, typename Traits>
#if __has_cpp_attribute(nodiscard)
[[nodiscard]]
#endif
inline ::std::basic_filebuf<CharT, Traits> *
open_libstdcxx_basic_filebuf_ios_base_open_mode(::std::__c_file *fp, ::std::ios_base::openmode mode)
{
	if (fp == nullptr)
	{
		throw_posix_error(EINVAL);
	}
	using hack_filebuf_type = hack_libstdcxx_basic_filebuf<CharT, Traits>;
	filebuf_guard<CharT, Traits> fbf_guard(new ::std::basic_filebuf<CharT, Traits>);
	auto const ptr_fbf{fbf_guard.new_filebuf};
	auto pfbf{reinterpret_cast<char unsigned *>(ptr_fbf)};
	auto &libstdcxx_filebuf_m_file{*reinterpret_cast<decltype(hack_filebuf_type::_M_file) *>(
		pfbf + __builtin_offsetof(hack_filebuf_type, _M_file))};
	libstdcxx_filebuf_m_file.sys_open(fp, mode);
	auto &libstdcxx_filebuf_c_file_m_cfile_created{
		*reinterpret_cast<bool *>(pfbf + __builtin_offsetof(hack_filebuf_type, _M_file) + sizeof(::std::__c_file *))};
	libstdcxx_filebuf_c_file_m_cfile_created = true;
	open_libstdcxx_basic_filebuf_ios_base_open_mode_common(ptr_fbf, mode);
	return fbf_guard.release();
}

template <typename CharT, typename Traits>
#if __has_cpp_attribute(nodiscard)
[[nodiscard]]
#endif
inline ::std::basic_filebuf<CharT, Traits> *open_hacked_basic_filebuf(::std::__c_file *fp, ::fast_io::open_mode mode)
{
	return open_libstdcxx_basic_filebuf_ios_base_open_mode<CharT, Traits>(
		fp, ::fast_io::details::calculate_fstream_file_open_mode(mode));
}

} // namespace streambuf_hack
} // namespace fast_io::details
