#pragma once

namespace fast_io
{

namespace details
{
template <typename T, typename... Args>
inline constexpr bool io_lockable_reopen_may_throw{[]() consteval -> bool {
	if constexpr (requires(T &h) { h.reopen(::std::declval<Args>()...); })
	{
		return !noexcept(::std::declval<T &>().reopen(::std::declval<Args>()...));
	}
	else
	{
		return !(::std::is_nothrow_constructible_v<T, Args...> &&
				 ::std::is_nothrow_move_assignable_v<T>);
	}
}()};


} // namespace details

template <typename T, typename Mutex>
	requires requires(Mutex &&m) {
		m.lock();
		m.unlock();
		m.try_lock();
	}
struct basic_general_io_lockable_nonmovable
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

	template <typename... Args>
		requires ::std::constructible_from<T, Args...>
	inline explicit constexpr basic_general_io_lockable_nonmovable(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_constructible_v<T, Args...>)
		: handle(::std::forward<Args>(args)...)
	{
	}

	template <typename... Args>
		requires(::std::constructible_from<T, Args...>)
	inline constexpr void reopen(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::io_lockable_reopen_may_throw<T, Args...>)
	{
		::fast_io::io_lock_guard guard(mutex);
		if constexpr (requires() {
						  handle.reopen(::std::forward<Args>(args)...);
					  })
		{
			handle.reopen(::std::forward<Args>(args)...);
		}
		else
		{
			this->handle = T(::std::forward<Args>(args)...);
		}
	}

	inline constexpr auto native_handle() noexcept(noexcept(handle.native_handle()))
		requires(requires() { handle.native_handle(); })
	{
		return handle.native_handle();
	}

	inline constexpr void close() noexcept(noexcept(handle.close()))
	{
		::fast_io::io_lock_guard guard(mutex);
		handle.close();
	}
};

namespace details
{
template <typename T, typename Allocator>
struct heap_typed_allocate_guard
{
	using value_type = T;
	using native_handle_type = T *;
	using allocator_type = Allocator;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, value_type>;
	native_handle_type ptr;
	inline explicit constexpr heap_typed_allocate_guard() noexcept
		: ptr{typed_allocator_type::allocate(1)}
	{
	}
	inline heap_typed_allocate_guard(heap_typed_allocate_guard const &) = delete;
	inline heap_typed_allocate_guard &operator=(heap_typed_allocate_guard const &) = delete;
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{this->ptr};
		this->ptr = nullptr;
		return temp;
	}
	inline constexpr void clear() noexcept
	{
		typed_allocator_type::deallocate_n(this->ptr, 1);
		this->ptr = nullptr;
	}
	inline constexpr ~heap_typed_allocate_guard() noexcept
	{
		clear();
	}
};

} // namespace details

template <typename T, typename Mutex>
struct basic_general_io_lockable_ref
{
	using input_char_type = typename T::input_char_type;
	using output_char_type = typename T::output_char_type;
	using native_handle_type = basic_general_io_lockable_nonmovable<T, Mutex> *;
	native_handle_type ptr{};
};

template <typename T, typename Mutex>
	requires ::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>
inline constexpr basic_general_io_lockable_ref<T, Mutex>
output_stream_ref_define(::fast_io::basic_general_io_lockable_nonmovable<T, Mutex> &m) noexcept
{
	return {__builtin_addressof(m)};
}

template <typename T, typename Mutex>
	requires ::fast_io::operations::defines::has_input_or_io_stream_ref_define<T>
inline constexpr basic_general_io_lockable_ref<T, Mutex>
input_stream_ref_define(::fast_io::basic_general_io_lockable_nonmovable<T, Mutex> &m) noexcept
{
	return {__builtin_addressof(m)};
}

template <typename T, typename Mutex>
	requires ::fast_io::operations::defines::has_io_stream_ref_define<T>
inline constexpr basic_general_io_lockable_ref<T, Mutex> io_stream_ref_define(::fast_io::basic_general_io_lockable_nonmovable<T, Mutex> &m) noexcept
{
	return {__builtin_addressof(m)};
}

template <typename Mutex, typename Allocator>
	requires ::std::default_initializable<Mutex>
struct basic_general_mutex_movable
{
	using mutex_type = Mutex;
	using allocator_type = Allocator;
	using native_handle_type = mutex_type *;
	mutex_type *pmutex{};

private:
	inline constexpr void construct_default() noexcept(::std::is_nothrow_default_constructible_v<mutex_type>)
	{
		::fast_io::details::heap_typed_allocate_guard<mutex_type, allocator_type> g;
		::std::construct_at(g.ptr);
		this->pmutex = g.release();
	}

public:
	inline explicit constexpr basic_general_mutex_movable() noexcept(::std::is_nothrow_default_constructible_v<mutex_type>)
	{
		this->construct_default();
	}

	inline explicit constexpr basic_general_mutex_movable(::fast_io::for_overwrite_t) noexcept
	{
	}

private:
	inline constexpr void destroy() noexcept
	{
		if (this->pmutex) [[likely]]
		{
			::std::destroy_at(this->pmutex);
		}
		::fast_io::typed_generic_allocator_adapter<allocator_type, mutex_type>::deallocate_n(this->pmutex, 1);
		this->pmutex = nullptr;
	}

public:
	inline constexpr void close() noexcept
	{
		this->destroy();
		this->pmutex = nullptr;
	}
	inline constexpr void reopen() noexcept(::std::is_nothrow_default_constructible_v<mutex_type>)
	{
		this->close();
		this->construct_default();
	}

	inline constexpr native_handle_type release() noexcept
	{
		auto temp{this->pmutex};
		this->pmutex = nullptr;
		return temp;
	}

	inline basic_general_mutex_movable(basic_general_mutex_movable const &) = delete;
	inline basic_general_mutex_movable &operator=(basic_general_mutex_movable const &) = delete;
	inline constexpr basic_general_mutex_movable(basic_general_mutex_movable &&other) noexcept
		: pmutex{other.pmutex}
	{
		other.pmutex = nullptr;
	}
	inline constexpr basic_general_mutex_movable &operator=(basic_general_mutex_movable &&other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		other.destroy();
		this->pmutex = other.pmutex;
		other.pmutex = nullptr;
		return *this;
	}

	inline constexpr ~basic_general_mutex_movable() noexcept
	{
		this->destroy();
	}
	inline constexpr void lock() FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(pmutex->lock())
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

template <typename Mutex>
struct basic_mutex_ref
{
	using mutex_type = Mutex;
	using native_handle_type = mutex_type *;
	mutex_type *pmutex{};
	inline constexpr void lock() FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(pmutex->lock())
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

template <typename Mutex, typename Allocator>
inline constexpr ::fast_io::basic_mutex_ref<Mutex> mutex_ref_define(::fast_io::basic_general_mutex_movable<Mutex, Allocator> &mtx) noexcept
{
	return {mtx.pmutex};
}

namespace operations::defines
{

template <typename T>
concept has_mutex_ref_define = requires(T t) {
	mutex_ref_define(t);
};

} // namespace operations::defines

namespace details
{
template <typename Mutex>
inline constexpr bool io_lockable_mutex_ref_may_throw{[]() consteval -> bool {
	if constexpr (::fast_io::operations::defines::has_mutex_ref_define<Mutex>)
	{
		return !noexcept(mutex_ref_define(::std::declval<Mutex &>()));
	}
	else
	{
		return false;
	}
}()};

} // namespace details

template <typename T, typename Mutex>
inline constexpr decltype(auto) io_stream_mutex_ref_define(::fast_io::basic_general_io_lockable_ref<T, Mutex> mtx)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::io_lockable_mutex_ref_may_throw<Mutex>)
{
	if constexpr (::fast_io::operations::defines::has_mutex_ref_define<Mutex>)
	{
		return mutex_ref_define(mtx.ptr->mutex);
	}
	else
	{
		return ::fast_io::basic_mutex_ref<Mutex>{__builtin_addressof(mtx.ptr->mutex)};
	}
}

template <typename T, typename Mutex>
	requires ::fast_io::operations::defines::has_output_stream_ref_define<T>
inline constexpr decltype(auto) output_stream_unlocked_ref_define(::fast_io::basic_general_io_lockable_ref<T, Mutex> mtx)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(output_stream_ref_define(mtx.ptr->handle)))
{
	return output_stream_ref_define(mtx.ptr->handle);
}

template <typename T, typename Mutex>
	requires ::fast_io::operations::defines::has_input_stream_ref_define<T>
inline constexpr decltype(auto) input_stream_unlocked_ref_define(::fast_io::basic_general_io_lockable_ref<T, Mutex> mtx)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(input_stream_ref_define(mtx.ptr->handle)))
{
	return input_stream_ref_define(mtx.ptr->handle);
}

template <typename T, typename Mutex>
	requires ::fast_io::operations::defines::has_io_stream_ref_define<T>
inline constexpr decltype(auto) io_stream_unlocked_ref_define(::fast_io::basic_general_io_lockable_ref<T, Mutex> mtx)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(io_stream_ref_define(mtx.ptr->handle)))
{
	return io_stream_ref_define(mtx.ptr->handle);
}

} // namespace fast_io
