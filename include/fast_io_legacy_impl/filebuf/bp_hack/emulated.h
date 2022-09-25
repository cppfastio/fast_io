#pragma once

namespace fast_io
{

namespace details::streambuf_hack
{
template<typename char_type,typename traits_type>
class emulated_basic_streambuf_hack_model : private ::std::basic_streambuf<char_type,traits_type>
{
public:
	using ::std::basic_streambuf<char_type,traits_type>::eback;
	using ::std::basic_streambuf<char_type,traits_type>::gptr;
	using ::std::basic_streambuf<char_type,traits_type>::egptr;
	using ::std::basic_streambuf<char_type,traits_type>::pbase;
	using ::std::basic_streambuf<char_type,traits_type>::pptr;
	using ::std::basic_streambuf<char_type,traits_type>::epptr;
	using ::std::basic_streambuf<char_type,traits_type>::gbump;
	using ::std::basic_streambuf<char_type,traits_type>::pbump;
};
}

template<typename T>
inline typename T::char_type* ibuffer_begin(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::emulated_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr auto methodptr{&model::eback};
	return (biob.fb->*methodptr)();
}

template<typename T>
inline typename T::char_type* ibuffer_curr(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::emulated_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr auto methodptr{&model::gptr};
	return (biob.fb->*methodptr)();
}

template<typename T>
inline typename T::char_type* ibuffer_end(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::emulated_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr auto methodptr{&model::egptr};
	return (biob.fb->*methodptr)();
}

template<typename T>
inline void ibuffer_set_curr(::fast_io::basic_general_streambuf_io_observer<T> biob,typename T::char_type* ptr) noexcept
{
/*
Work around bugs in issue 255
https://cplusplus.github.io/LWG/issue255
*/
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::emulated_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr auto curr_methodptr{&model::gptr};
	auto diff{ptr-(biob.fb->*curr_methodptr)()};
	if constexpr(sizeof(int)<sizeof(std::ptrdiff_t))
	{
		constexpr ::std::ptrdiff_t mn{static_cast<::std::ptrdiff_t>(::std::numeric_limits<int>::min())},mx{static_cast<::std::ptrdiff_t>(::std::numeric_limits<int>::max())};
		if(diff<mn||mx<diff)
		{
			fast_terminate();
		}
	}
	constexpr auto methodptr{&model::gbump};
	(biob.fb->*methodptr)(static_cast<int>(diff));
}

template<typename T>
inline typename T::char_type* obuffer_begin(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::emulated_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr auto methodptr{&model::pbase};
	return (biob.fb->*methodptr)();
}

template<typename T>
inline typename T::char_type* obuffer_curr(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::emulated_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr auto methodptr{&model::pptr};
	return (biob.fb->*methodptr)();
}

template<typename T>
inline typename T::char_type* obuffer_end(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::emulated_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr auto methodptr{&model::epptr};
	return (biob.fb->*methodptr)();
}

template<typename T>
inline void obuffer_set_curr(::fast_io::basic_general_streambuf_io_observer<T> biob,typename T::char_type* ptr) noexcept
{
/*
Work around bugs in issue 255
https://cplusplus.github.io/LWG/issue255
*/
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::emulated_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr auto curr_methodptr{&model::pptr};
	auto diff{ptr-(biob.fb->*curr_methodptr)()};
	if constexpr(sizeof(int)<sizeof(std::ptrdiff_t))
	{
		constexpr ::std::ptrdiff_t mn{static_cast<::std::ptrdiff_t>(::std::numeric_limits<int>::min())},mx{static_cast<::std::ptrdiff_t>(::std::numeric_limits<int>::max())};
		if(diff<mn||mx<diff)
		{
			fast_terminate();
		}
	}
	constexpr auto methodptr{&model::pbump};
	(biob.fb->*methodptr)(static_cast<int>(diff));
}

}

