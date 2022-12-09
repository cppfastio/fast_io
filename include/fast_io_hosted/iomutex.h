#pragma once

namespace fast_io
{

template<stream T,typename Mutex>
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
	using char_type = typename unlocked_handle_type::char_type;
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
	inline constexpr void lock() noexcept(noexcept(mutex.lock()))
	{
		mutex.lock();
	}
	inline constexpr bool try_lock() noexcept(noexcept(mutex.try_lock()))
	{
		return mutex.try_lock();
	}
	inline constexpr void unlock() noexcept
	{
		mutex.unlock();
	}
	inline constexpr unlocked_handle_type const& unlocked_handle() const noexcept
	{
		return handle;
	}
	inline constexpr unlocked_handle_type& unlocked_handle() noexcept
	{
		return handle;
	}
	template<typename... Args>
	inline constexpr void reopen(Args&& ...args)
	{
		io_lock_guard gd{mutex};
		handle.reopen(::std::forward<Args>(args)...);
	}
};

template<input_stream T,typename mutex_type,std::input_or_output_iterator Iter>
inline constexpr decltype(auto) read(basic_general_iomutex<T,mutex_type>& mtx,Iter begin,Iter end)
{
	io_lock_guard gd{mtx.mutex};
	return read(mtx.handle,begin,end);
}

template<output_stream T,typename mutex_type,std::input_or_output_iterator Iter>
inline constexpr decltype(auto) write(basic_general_iomutex<T,mutex_type>& mtx,Iter begin,Iter end)
{
	io_lock_guard gd{mtx.mutex};
	return write(mtx.handle,begin,end);
}

template<flush_output_stream T,typename mutex_type>
inline constexpr void flush(basic_general_iomutex<T,mutex_type>& mtx)
{
	io_lock_guard gd{mtx.mutex};
	flush(mtx.handle);
}

template<stream T,typename mutex_type=native_mutex>
using basic_iomutex = basic_general_iomutex<T,mutex_type>;

}

