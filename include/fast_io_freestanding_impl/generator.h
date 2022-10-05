#pragma once
//https://github.com/lewissbaker/cppcoro/blob/master/include/cppcoro/generator.hpp
namespace fast_io
{

template<typename T>
class generator;

namespace details
{
template<typename T>
class generator_promise
{
public:
	using element_type = T;
	using value_type = std::remove_cvref_t<element_type>;
	using reference = std::remove_reference_t<T>&;
	using const_reference = std::remove_reference_t<T> const&;
	using pointer = value_type*;
	using const_pointer = value_type const*;
	element_type ptr{};
#ifdef __cpp_exceptions
	std::exception_ptr ex_ptr;
#endif
	generator<T> get_return_object() noexcept;

	constexpr std::suspend_always initial_suspend() const noexcept { return {}; }
	constexpr std::suspend_always final_suspend() const noexcept { return {}; }
//	template<typename U>
//	requires std::same_as<std::remove_reference_t<U>,std::remove_reference_t<T>>
	constexpr std::suspend_always yield_value(T value) noexcept
	{
		ptr=value;
//		ptr = __builtin_addressof(value);
		return {};
	}
	constexpr void unhandled_exception() noexcept
	{
#ifdef __cpp_exceptions
		ex_ptr = std::current_exception();
#else
		std::terminate();
#endif
	}
	constexpr void return_void() noexcept
	{
	}
	template<typename U>
	std::suspend_never await_transform(U&& value) = delete;

	void rethrow_if_exception()
	{
#ifdef __cpp_exceptions
		if (ex_ptr)
			std::rethrow_exception(ex_ptr);
#endif
	}
};

template<typename T>
class generator_iterator
{
public:
	using coroutine_handle_type = std::coroutine_handle<generator_promise<T>>;
	using iterator_category = ::std::output_iterator_tag;
	coroutine_handle_type handle{};
/*
	inline constexpr auto operator->()
	{
		handle.resume();
#ifdef __cpp_exceptions
		if (handle.done())
			handle.promise().rethrow_if_exception();
#endif
		return handle.promise().ptr;
	}
*/
	inline constexpr auto operator*()
	{
		return handle.promise().ptr;
	}

};

template<typename T>
inline constexpr bool operator==(::std::default_sentinel_t, generator_iterator<T> const& b) noexcept
{
	return b.handle.done();
}

template<typename T>
inline constexpr bool operator==(generator_iterator<T> const& b,::std::default_sentinel_t) noexcept
{
	return b.handle.done();
}
template<typename T>
inline constexpr bool operator!=(::std::default_sentinel_t s, generator_iterator<T> const& b) noexcept
{
	return !(s==b);
}

template<typename T>
inline constexpr bool operator!=(generator_iterator<T> const& b,::std::default_sentinel_t s) noexcept
{
	return !(s==b);
}

template<typename T>
inline constexpr generator_iterator<T>& operator++(generator_iterator<T>& b)
{
	b.handle.resume();
#ifdef __cpp_exceptions
	if (b.handle.done())
		b.handle.promise().rethrow_if_exception();
#endif
	return b;
}
template<typename T>
inline constexpr void operator++(generator_iterator<T>& b,int)
{
	b.handle.resume();
#ifdef __cpp_exceptions
	if (b.handle.done())
		b.handle.promise().rethrow_if_exception();
#endif
}

}

template<typename T>
class [[nodiscard]] generator
{
public:
	using promise_type = details::generator_promise<T>;
	std::coroutine_handle<promise_type> handle;
	constexpr generator(std::coroutine_handle<promise_type> v):handle(v){}
	constexpr generator(generator const&) noexcept=delete;
	constexpr generator& operator=(generator const&) noexcept=delete;
	constexpr ~generator()
	{
		handle.destroy();
	}
};
template<typename T>
inline constexpr details::generator_iterator<T> begin(generator<T>& gen)
{
	gen.handle.resume();
#ifdef __cpp_exceptions
	if (gen.handle.done())
		gen.handle.promise().rethrow_if_exception();
#endif
	return {gen.handle};
}
template<typename T>
inline constexpr ::std::default_sentinel_t end(generator<T>& gen)
{
	return {};
}
template<typename T>
inline constexpr details::generator_iterator<T> cbegin(generator<T> const& gen)
{
	gen.handle.resume();
#ifdef __cpp_exceptions
	if (gen.handle.done())
		gen.handle.promise().rethrow_if_exception();
#endif
	return {gen.handle};
}
template<typename T>
inline constexpr ::std::default_sentinel_t cend(generator<T> const& gen)
{
	return {};
}
template<typename T>
inline constexpr details::generator_iterator<T> begin(generator<T> const& gen)
{
	gen.handle.resume();
#ifdef __cpp_exceptions
	if (gen.handle.done())
		gen.handle.promise().rethrow_if_exception();
#endif
	return {gen.handle};
}
template<typename T>
inline constexpr ::std::default_sentinel_t end(generator<T> const& gen)
{
	return {};
}

namespace details
{
template<typename T>
inline generator<T> generator_promise<T>::get_return_object() noexcept
{
	using coroutine_handle = std::coroutine_handle<generator_promise<T>>;
	return { coroutine_handle::from_promise(*this) };
}
}
}