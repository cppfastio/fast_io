#pragma once

namespace fast_io
{

template<typename T,typename Mutex>
requires requires(Mutex&& m)
{
	m.lock();
	m.unlock();
	m.try_lock();
}
struct basic_general_iomutex
{
	using mutex_type = Mutex;
	using unlocked_handle_type = T;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	T handle;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	mutex_type mutex;
	
	template<typename... Args>
	requires std::constructible_from<T,Args...>
	explicit constexpr basic_general_iomutex(Args&& ...args):handle(::std::forward<Args>(args)...)
	{
	}
};

template<typename T,typename mutex_type=native_mutex>
using basic_iomutex = basic_general_iomutex<T,mutex_type>;

template<typename T,typename Mutex>
struct basic_general_iomutex_ref
{
	using input_char_type = typename T::input_char_type;
	using output_char_type = typename T::output_char_type;
	using native_handle_type = basic_general_iomutex<T,Mutex>*;
	native_handle_type ptr{};
};

template<typename T,typename mutex_type=native_mutex>
using basic_iomutex_ref = basic_general_iomutex_ref<T,mutex_type>;

template<typename T,typename Mutex>
requires ::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>
inline constexpr basic_general_iomutex_ref<T,Mutex> output_stream_ref_define(basic_general_iomutex<T,Mutex>& m) noexcept
{
	return {__builtin_addressof(m)};
}

template<typename T,typename Mutex>
requires ::fast_io::operations::defines::has_input_or_io_stream_ref_define<T>
inline constexpr basic_general_iomutex_ref<T,Mutex> input_stream_ref_define(basic_general_iomutex<T,Mutex>& m) noexcept
{
	return {__builtin_addressof(m)};
}

template<typename T,typename Mutex>
requires ::fast_io::operations::defines::has_io_stream_ref_define<T>
inline constexpr basic_general_iomutex_ref<T,Mutex> io_stream_ref_define(basic_general_iomutex<T,Mutex>& m) noexcept
{
	return {__builtin_addressof(m)};
}

template<typename Mutex>
struct basic_mutex_ref
{
	using mutex_type = Mutex;
	using native_handle_type = mutex_type *;
	mutex_type *pmutex{};
	inline constexpr void lock() noexcept(noexcept(pmutex->lock()))
	{
		pmutex->lock();
	}
	inline constexpr bool try_lock() noexcept(noexcept(pmutex->try_lock()))
	{
		return pmutex->try_lock();
	}
	inline constexpr void unlock() noexcept
	{
		pmutex->unlock();
	}
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return pmutex;
	}
};


template<typename T,typename Mutex>
inline constexpr basic_mutex_ref<Mutex> io_stream_mutex_ref_define(basic_general_iomutex_ref<T,Mutex> mtx)
{
	return {__builtin_addressof(mtx.ptr->mutex)};
}

template<typename T,typename Mutex>
requires ::fast_io::operations::defines::has_output_stream_ref_define<T>
inline constexpr decltype(auto) output_stream_unlocked_ref_define(basic_general_iomutex_ref<T,Mutex> mtx)
{
	return output_stream_ref_define(mtx.ptr->handle);
}

template<typename T,typename Mutex>
requires ::fast_io::operations::defines::has_input_stream_ref_define<T>
inline constexpr decltype(auto) input_stream_unlocked_ref_define(basic_general_iomutex_ref<T,Mutex> mtx)
{
	return input_stream_ref_define(mtx.ptr->handle);
}

template<typename T,typename Mutex>
requires ::fast_io::operations::defines::has_io_stream_ref_define<T>
inline constexpr decltype(auto) io_stream_unlocked_ref_define(basic_general_iomutex_ref<T,Mutex> mtx)
{
	return io_stream_ref_define(mtx.ptr->handle);
}

}
