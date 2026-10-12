#pragma once

namespace fast_io
{

namespace details
{

/*
Whether the handle_type's output stream operations may throw. The requires
expression inside the concept must stay: unlike a requires-clause, the
subexpression of a noexcept-specifier does not short-circuit, so the decltype
would be a hard error for handle types without output_stream_ref.
*/
template <typename T>
concept io_buffer_output_operations_may_throw =
	requires { ::fast_io::operations::output_stream_ref(::std::declval<T &>()); } &&
	!::fast_io::operations::decay::defines::output_stream_operations_nothrow<
		decltype(::fast_io::operations::output_stream_ref(::std::declval<T &>()))>;

template <typename T>
inline constexpr void close_basic_io_buffer(T &)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		(T::traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
		io_buffer_output_operations_may_throw<typename T::handle_type>);
template <typename T>
inline constexpr void clear_basic_io_buffer_pointers(T &) noexcept;
template <typename T>
inline constexpr void destroy_basic_io_buffer(T &) noexcept;

template <typename T, typename... Args>
concept has_reopen_impl = requires(T &&t, Args &&...args) { t.reopen(::std::forward<Args>(args)...); };

template <typename T>
concept has_close_impl = requires(T &&t) { t.close(); };

namespace io_buffer
{

template <typename allocator_type, typename char_type>
using iobuffer_alloc_handle_t = ::std::conditional_t<
	::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::has_status,
	typename ::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::handle_type,
	::fast_io::details::empty>;

template <::std::integral char_type, typename allocator_type>
inline constexpr char_type *
iobuffer_allocate(iobuffer_alloc_handle_t<allocator_type, char_type> allochdl, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::throws_on_allocation_failure)
{
	if constexpr (::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::has_status)
	{
		return ::fast_io::details::allocate_iobuf_space<char_type, allocator_type>(allochdl, n);
	}
	else
	{
		return ::fast_io::details::allocate_iobuf_space<char_type, allocator_type>(n);
	}
}

template <::std::integral char_type, typename allocator_type>
inline constexpr void iobuffer_deallocate_n(iobuffer_alloc_handle_t<allocator_type, char_type> allochdl,
											char_type *ptr, ::std::size_t n) noexcept
{
	if constexpr (::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::has_status)
	{
		::fast_io::details::deallocate_iobuf_space<false, char_type, allocator_type>(allochdl, ptr, n);
	}
	else
	{
		::fast_io::details::deallocate_iobuf_space<false, char_type, allocator_type>(ptr, n);
	}
}

/*
 * RAII owner of a buffer detached from basic_io_buffer by
 * detach_output_buffer. The pending bytes [buffer_begin, buffer_curr)
 * can be handed to an async write while the stream buffers into a fresh
 * allocation — no copying of already-buffered data is needed. The
 * allocation is released when the guard dies; pending_bytes() exposes
 * the byte range to write.
 */
template <typename iobuffertraits>
struct io_detached_output_buffer
{
	using traits_type = iobuffertraits;
	using char_type = typename traits_type::output_char_type;
	using allocator_type = typename traits_type::allocator_type;
	using allocator_handle_type = iobuffer_alloc_handle_t<allocator_type, char_type>;
	allocator_handle_type allocator_handle{};
	char_type *buffer_begin{}, *buffer_curr{}, *buffer_end{};

	inline constexpr io_detached_output_buffer() noexcept = default;
	inline io_detached_output_buffer(io_detached_output_buffer const &) = delete;
	inline io_detached_output_buffer &operator=(io_detached_output_buffer const &) = delete;
	inline constexpr io_detached_output_buffer(io_detached_output_buffer &&other) noexcept
		: allocator_handle(other.allocator_handle),
		  buffer_begin(other.buffer_begin),
		  buffer_curr(other.buffer_curr),
		  buffer_end(other.buffer_end)
	{
		other.buffer_begin = other.buffer_curr = other.buffer_end = nullptr;
	}
	inline constexpr io_detached_output_buffer &operator=(io_detached_output_buffer &&other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		release();
		allocator_handle = other.allocator_handle;
		buffer_begin = other.buffer_begin;
		buffer_curr = other.buffer_curr;
		buffer_end = other.buffer_end;
		other.buffer_begin = other.buffer_curr = other.buffer_end = nullptr;
		return *this;
	}
	inline constexpr void release() noexcept
	{
		if (buffer_begin != nullptr)
		{
			iobuffer_deallocate_n<char_type, allocator_type>(allocator_handle, buffer_begin,
															 traits_type::output_buffer_size);
			buffer_begin = buffer_curr = buffer_end = nullptr;
		}
	}
	inline constexpr ~io_detached_output_buffer() noexcept
	{
		release();
	}
	/* pending bytes as a scatter {base, len} */
	inline constexpr ::fast_io::io_scatter_t pending_bytes() const noexcept
	{
		return {reinterpret_cast<::std::byte const *>(buffer_begin),
				static_cast<::std::size_t>(buffer_curr - buffer_begin) * sizeof(char_type)};
	}
};

} // namespace io_buffer

} // namespace details

template <typename handletype, typename iobuffertraits>
class basic_io_buffer
{
public:
	using handle_type = handletype;
	using traits_type = iobuffertraits;
	using input_char_type = typename traits_type::input_char_type;
	using output_char_type = typename traits_type::output_char_type;
	using allocator_type = typename traits_type::allocator_type;
	static inline constexpr bool allocator_has_status{allocator_type::has_status};
	using allocator_handle_type =
		::std::conditional_t<allocator_has_status, typename allocator_type::handle_type, ::fast_io::details::empty>;

	using input_buffer_type = ::std::conditional_t<(traits_type::mode & buffer_mode::in) == buffer_mode::in,
												   basic_io_buffer_pointers<input_char_type>, empty_buffer_pointers>;
	using output_buffer_type = ::std::conditional_t<(traits_type::mode & buffer_mode::out) == buffer_mode::out,
													basic_io_buffer_pointers<output_char_type>, empty_buffer_pointers>;

#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	input_buffer_type input_buffer;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	output_buffer_type output_buffer;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	handle_type handle;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	allocator_handle_type allocator_handle{};

	inline explicit constexpr basic_io_buffer() noexcept
		requires(!allocator_has_status)
	= default;
	inline explicit constexpr basic_io_buffer(allocator_handle_type allochdl) noexcept
		requires(allocator_has_status && ::std::is_default_constructible_v<handle_type>)
		: allocator_handle(allochdl)
	{
	}
	template <typename... Args>
		requires(allocator_has_status && 0 < sizeof...(Args) &&
				 ::std::constructible_from<handle_type, Args...>)
	inline constexpr basic_io_buffer(allocator_handle_type allochdl, Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(handle_type(::std::forward<Args>(args)...))
		: handle(::std::forward<Args>(args)...), allocator_handle(allochdl)
	{
	}
	template <typename... Args>
		requires(!allocator_has_status && ::std::constructible_from<handle_type, Args...>)
	inline explicit constexpr basic_io_buffer(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(handle_type(::std::forward<Args>(args)...))
		: handle(::std::forward<Args>(args)...)
	{
	}
	inline basic_io_buffer &operator=(basic_io_buffer const &) = delete;
	inline basic_io_buffer(basic_io_buffer const &) = delete;
	inline constexpr basic_io_buffer(basic_io_buffer &&__restrict other) noexcept
		: input_buffer(::std::move(other.input_buffer)), output_buffer(::std::move(other.output_buffer)),
		  handle(::std::move(other.handle)), allocator_handle(other.allocator_handle)
	{
		other.input_buffer = {};
		other.output_buffer = {};
	}
	template <typename... Args>
		requires(::std::constructible_from<handle_type, Args...>)
	inline constexpr void reopen(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(
			!noexcept(handle_type(::std::declval<Args>()...)) ||
			requires { requires !noexcept(::std::declval<handle_type &>().reopen(::std::declval<Args>()...)); } ||
			requires { requires !noexcept(::std::declval<handle_type &>() = ::std::declval<handle_type>()); } ||
			requires { requires !noexcept(::std::declval<handle_type &>().close()); } ||
			((traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
			 ::fast_io::details::io_buffer_output_operations_may_throw<handle_type>))
	{
		::fast_io::details::close_basic_io_buffer(*this);
		::fast_io::details::clear_basic_io_buffer_pointers(*this);
		if constexpr (::fast_io::details::has_reopen_impl<handle_type, Args...>)
		{
			handle.reopen(::std::forward<Args>(args)...);
		}
		else
		{
			handle = handle_type(::std::forward<Args>(args)...);
		}
	}

	inline constexpr void close()
		FAST_IO_HERBCEPTIONS_THROWS_IF(
			requires { requires !noexcept(::std::declval<handle_type &>().close()); } ||
			((traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
			 ::fast_io::details::io_buffer_output_operations_may_throw<handle_type>))
	{
		::fast_io::details::close_basic_io_buffer(*this);
		::fast_io::details::clear_basic_io_buffer_pointers(*this);
		if constexpr (::fast_io::details::has_close_impl<handle_type>)
		{
			handle.close();
		}
		else
		{
			handle = handle_type();
		}
	}
	inline constexpr basic_io_buffer &operator=(basic_io_buffer &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		::fast_io::details::destroy_basic_io_buffer(*this);
		input_buffer = ::std::move(other.input_buffer);
		output_buffer = ::std::move(other.output_buffer);
		handle = ::std::move(other.handle);
		allocator_handle = other.allocator_handle;
		other.input_buffer = {};
		other.output_buffer = {};
		return *this;
	}

	using detached_output_buffer_type =
		::fast_io::details::io_buffer::io_detached_output_buffer<traits_type>;
	/* Hands the pending output buffer allocation to the returned RAII
	 * guard, so an async writer can send the detached bytes while this
	 * stream keeps buffering — the pending bytes are never copied. The
	 * stream's buffer pointers are reset to nullptr; the next buffered
	 * write allocates lazily, the same as a never-allocated buffer. */
	[[nodiscard]] inline constexpr detached_output_buffer_type detach_output_buffer() noexcept
		requires((traits_type::mode & ::fast_io::buffer_mode::out) ==
				 ::fast_io::buffer_mode::out)
	{
		detached_output_buffer_type detached;
		detached.allocator_handle = allocator_handle;
		detached.buffer_begin = output_buffer.buffer_begin;
		detached.buffer_curr = output_buffer.buffer_curr;
		detached.buffer_end = output_buffer.buffer_end;
		output_buffer.buffer_begin = nullptr;
		output_buffer.buffer_curr = nullptr;
		output_buffer.buffer_end = nullptr;
		return detached;
	}

	inline constexpr ~basic_io_buffer() noexcept
	{
		::fast_io::details::destroy_basic_io_buffer(*this);
	}
};

} // namespace fast_io
