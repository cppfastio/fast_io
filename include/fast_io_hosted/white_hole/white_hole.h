#pragma once
#if (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
#include"rtl_gen_random.h"
#include"win32_crypt_gen_random.h"
#endif
#if defined(__wasi__)
#include"wasi_random_get.h"
#endif

namespace fast_io::details
{
template<typename T>
concept has_entroy_method_impl = requires(T&& handle)
{
	{random_entropy(handle)}->std::convertible_to<double>;
};

template<typename input,std::size_t N>
inline constexpr bool minimum_buffer_input_stream_require_size_constant_impl =
	(N<ibuffer_minimum_size_define(::fast_io::io_reserve_type<typename input::input_char_type,input>));

template<typename input,std::size_t N>
concept minimum_buffer_input_stream_require_size_impl = ::fast_io::operations::decay::defines::has_ibuffer_minimum_size_operations<input>
	&& minimum_buffer_input_stream_require_size_constant_impl<input,N>;

}
#if ((defined(__linux__) && defined(__NR_getrandom)) || (!defined(__linux__)&&__has_include(<sys/random.h>))) && !defined(__wasi__) && !defined(__DARWIN_C_LEVEL)
#include"linux_getrandom.h"
#endif
#include"posix_dev_urandom.h"

namespace fast_io
{
#if (defined(_WIN32)&&!defined(__WINE__)&& (defined(RNDGETENTCNT))) || defined(__linux__)
namespace details
{
#if defined(RNDGETENTCNT) || defined(__linux__)
inline int my_random_entropy(int fd) noexcept
{
	int ent{};
#if defined(__linux__) && defined(__NR_ioctl)
	if(system_call<__NR_ioctl,std::ptrdiff_t>(fd,static_cast<std::uint_least32_t>(u8'R')<<8u,__builtin_addressof(ent))!=0)
		return 0.0;
#else
	if(::fast_io::posix::ioctl(fd,RNDGETENTCNT,__builtin_addressof(ent))!=0)
		return 0.0;
#endif
	return ent;
}
#endif
}

template<std::integral ch_type>
inline int random_entropy(basic_posix_io_observer<ch_type> piob) noexcept
{
	return ::fast_io::details::my_random_entropy(piob.fd);
}
#endif

template<std::integral char_type>
using basic_native_white_hole =
#if defined(_WIN32)&&!defined(__WINE__)
#if defined(_WIN32_WINDOWS) || (defined(_WIN32_WINNT)&&_WIN32_WINNT <= 0x0500)
basic_win32_crypt_gen_random_file<char_type>;
#else
basic_rtl_gen_random<char_type>;
#endif
#elif defined(__wasi__)
basic_wasi_random_get<char_type>;
#elif (defined(__linux__) && defined(__NR_getrandom)) || (!defined(__linux__)&&__has_include(<sys/random.h>)) && !defined(__DARWIN_C_LEVEL)
basic_linux_getrandom<char_type>;
#else
basic_posix_dev_urandom<char_type>;
#endif

template<std::integral char_type,typename allocator_type=::fast_io::native_global_allocator>
using basic_ibuf_white_hole = basic_io_buffer<basic_native_white_hole<char_type>,
	::fast_io::basic_io_buffer_traits<buffer_mode::in|buffer_mode::secure_clear,
		allocator_type,char_type,
		void,8192u,0>>;

using native_white_hole = basic_native_white_hole<char>;
using ibuf_white_hole = basic_ibuf_white_hole<char>;

using wnative_white_hole = basic_native_white_hole<wchar_t>;
using wibuf_white_hole = basic_ibuf_white_hole<wchar_t>;

using u8native_white_hole = basic_native_white_hole<char8_t>;
using u8ibuf_white_hole = basic_ibuf_white_hole<char8_t>;

using u16native_white_hole = basic_native_white_hole<char16_t>;
using u16ibuf_white_hole = basic_ibuf_white_hole<char16_t>;

using u32native_white_hole = basic_native_white_hole<char32_t>;
using u32ibuf_white_hole = basic_ibuf_white_hole<char32_t>;

template<typename handletype>
struct basic_white_hole_engine
{
	using handle_type = handletype;
	using result_type = ::std::size_t;
	handle_type handle;
	static inline constexpr result_type min() noexcept
	{
		return 0;
	}
	static inline constexpr result_type max() noexcept
	{
		return SIZE_MAX;
	}
	inline constexpr double entropy() const noexcept
	{
		if constexpr(::fast_io::details::has_entroy_method_impl<handletype>)
		{
			auto v{random_entropy(handle)};
			constexpr std::size_t mx_value{static_cast<std::size_t>(::std::numeric_limits<::std::size_t>::digits)};
			if(v>mx_value)
				v=mx_value;
			return static_cast<double>(v);
		}
		else
		{
			return 0.0;
		}
	}
	inline result_type operator()()
	{
		result_type type;
		auto instmref{::fast_io::operations::input_stream_ref(handle)};
		if constexpr(::fast_io::details::minimum_buffer_input_stream_require_size_impl<decltype(instmref),sizeof(result_type)>)
		{
			auto currptr{ibuffer_curr(instmref)},edptr{ibuffer_end(instmref)};
			::std::size_t diff{static_cast<::std::size_t>(edptr-currptr)};
			constexpr
				::std::size_t objsz{sizeof(result_type)};
			if(diff<=objsz)
#if __has_cpp_attribute(unlikely)
			[[unlikely]]
#endif
			{
				ibuffer_minimum_size_underflow_all_prepare_define(instmref);
				currptr=ibuffer_curr(instmref);
			}
			::fast_io::freestanding::my_memcpy(__builtin_addressof(type),currptr,objsz);
			ibuffer_set_curr(instmref,currptr+objsz);
		}
		else
		{
			::fast_io::operations::decay::read_all_bytes_decay(instmref,
				reinterpret_cast<::std::byte*>(__builtin_addressof(type)),
				reinterpret_cast<::std::byte*>(__builtin_addressof(type)+1));
		}
		return type;
	}
};

using native_white_hole_engine = basic_white_hole_engine<native_white_hole>;
using ibuf_white_hole_engine = basic_white_hole_engine<ibuf_white_hole>;

}
