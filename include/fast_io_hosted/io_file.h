#pragma once

namespace fast_io
{

template <::std::integral inchar_type, ::std::integral outchar_type>
class basic_io_io_base
{
public:
	inline virtual constexpr ::fast_io::io_scatter_status_t
	scatter_read_some_underflow_def(::fast_io::basic_io_scatter_t<inchar_type> const *, ::std::size_t)
		FAST_IO_HERBCEPTIONS_THROWS = 0;
	inline virtual constexpr ::fast_io::io_scatter_status_t
	scatter_pread_some_underflow_def(::fast_io::basic_io_scatter_t<inchar_type> const *, ::std::size_t,
									 ::fast_io::intfpos_t)
		FAST_IO_HERBCEPTIONS_THROWS = 0;
	inline virtual constexpr ::fast_io::intfpos_t input_stream_seek_def(::fast_io::intfpos_t, ::fast_io::seekdir)
		FAST_IO_HERBCEPTIONS_THROWS = 0;

	inline virtual constexpr ::fast_io::io_scatter_status_t
	scatter_write_some_overflow_def(::fast_io::basic_io_scatter_t<outchar_type> const *, ::std::size_t)
		FAST_IO_HERBCEPTIONS_THROWS = 0;
	inline virtual constexpr ::fast_io::io_scatter_status_t
	scatter_pwrite_some_overflow_def(::fast_io::basic_io_scatter_t<outchar_type> const *, ::std::size_t,
									 ::fast_io::intfpos_t)
		FAST_IO_HERBCEPTIONS_THROWS = 0;
	inline virtual constexpr ::fast_io::intfpos_t output_stream_seek_def(::fast_io::intfpos_t, ::fast_io::seekdir)
		FAST_IO_HERBCEPTIONS_THROWS = 0;
	inline virtual constexpr ~basic_io_io_base() noexcept = default;
};

template <::std::integral inchar_type, ::std::integral outchar_type, typename allocatortype, typename T>
class basic_io_io_derived : public basic_io_io_base<inchar_type, outchar_type>
{
public:
	T value;
	static constexpr inline void operator delete(void *ptr, ::std::size_t n) noexcept
	{
		allocatortype::deallocate_n(ptr, n);
	}
	template <typename... Args>
	inline constexpr basic_io_io_derived(io_cookie_t, Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_constructible_v<T, Args...>)
		: value(::fast_io::freestanding::forward<Args>(args)...)
	{
	}
	inline virtual constexpr ::fast_io::io_scatter_status_t
	scatter_read_some_underflow_def(::fast_io::basic_io_scatter_t<inchar_type> const *base, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS override
	{
		if constexpr (::fast_io::operations::defines::has_input_or_io_stream_ref_define<T>)
		{
			using decaytype = decltype(::fast_io::operations::input_stream_ref(value));
			if constexpr (::std::same_as<typename decaytype::input_char_type, inchar_type> &&
						  ::fast_io::operations::decay::defines::readable<decaytype>)
			{
				return ::fast_io::operations::decay::scatter_read_some_decay(
					::fast_io::operations::input_stream_ref(value), base, n);
			}
			else
			{
				throw_posix_error(EINVAL);
			}
		}
		else
		{
			throw_posix_error(EINVAL);
		}
	}
	inline virtual constexpr ::fast_io::io_scatter_status_t
	scatter_pread_some_underflow_def(::fast_io::basic_io_scatter_t<inchar_type> const *base, ::std::size_t n,
									 ::fast_io::intfpos_t off)
		FAST_IO_HERBCEPTIONS_THROWS override
	{
		if constexpr (::fast_io::operations::defines::has_input_or_io_stream_ref_define<T>)
		{
			using decaytype = decltype(::fast_io::operations::input_stream_ref(value));
			if constexpr (::std::same_as<typename decaytype::input_char_type, inchar_type> &&
						  ::fast_io::operations::decay::defines::preadable<decaytype>)
			{
				return ::fast_io::operations::decay::scatter_pread_some_decay(
					::fast_io::operations::input_stream_ref(value), base, n, off);
			}
			else
			{
				throw_posix_error(EINVAL);
			}
		}
		else
		{
			throw_posix_error(EINVAL);
		}
	}
	inline virtual constexpr ::fast_io::intfpos_t input_stream_seek_def(::fast_io::intfpos_t off,
																		::fast_io::seekdir sdir)
		FAST_IO_HERBCEPTIONS_THROWS override
	{
		if constexpr (::fast_io::operations::defines::has_input_or_io_stream_ref_define<T>)
		{
			using decaytype = decltype(::fast_io::operations::input_stream_ref(value));
			if constexpr (::std::same_as<typename decaytype::input_char_type, inchar_type> &&
						  ::fast_io::operations::decay::defines::has_input_stream_seek_define<decaytype>)
			{
				return ::fast_io::operations::decay::input_stream_seek_decay(
					::fast_io::operations::input_stream_ref(value), off, sdir);
			}
			else
			{
				throw_posix_error(EINVAL);
			}
		}
		else
		{
			throw_posix_error(EINVAL);
		}
	}

	inline virtual constexpr ::fast_io::io_scatter_status_t
	scatter_write_some_overflow_def(::fast_io::basic_io_scatter_t<outchar_type> const *base, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS override
	{
		if constexpr (::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>)
		{
			using decaytype = decltype(::fast_io::operations::output_stream_ref(value));
			if constexpr (::std::same_as<typename decaytype::output_char_type, outchar_type> &&
						  ::fast_io::operations::decay::defines::writable<decaytype>)
			{
				return ::fast_io::operations::decay::scatter_write_some_decay(
					::fast_io::operations::output_stream_ref(value), base, n);
			}
			else
			{
				throw_posix_error(EINVAL);
			}
		}
		else
		{
			throw_posix_error(EINVAL);
		}
	}
	inline virtual constexpr ::fast_io::io_scatter_status_t
	scatter_pwrite_some_overflow_def(::fast_io::basic_io_scatter_t<outchar_type> const *base, ::std::size_t n,
									 ::fast_io::intfpos_t off)
		FAST_IO_HERBCEPTIONS_THROWS override
	{
		if constexpr (::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>)
		{
			using decaytype = decltype(::fast_io::operations::output_stream_ref(value));
			if constexpr (::std::same_as<typename decaytype::output_char_type, outchar_type> &&
						  ::fast_io::operations::decay::defines::pwritable<decaytype>)
			{
				return ::fast_io::operations::decay::scatter_pwrite_some_decay(
					::fast_io::operations::output_stream_ref(value), base, n, off);
			}
			else
			{
				throw_posix_error(EINVAL);
			}
		}
		else
		{
			throw_posix_error(EINVAL);
		}
	}
	inline virtual constexpr ::fast_io::intfpos_t output_stream_seek_def(::fast_io::intfpos_t off,
																		 ::fast_io::seekdir sdir)
		FAST_IO_HERBCEPTIONS_THROWS override
	{
		if constexpr (::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>)
		{
			using decaytype = decltype(::fast_io::operations::output_stream_ref(value));
			if constexpr (::std::same_as<typename decaytype::output_char_type, outchar_type> &&
						  ::fast_io::operations::decay::defines::has_output_stream_seek_define<decaytype>)
			{
				return ::fast_io::operations::decay::output_stream_seek_decay(
					::fast_io::operations::output_stream_ref(value), off, sdir);
			}
			else
			{
				throw_posix_error(EINVAL);
			}
		}
		else
		{
			throw_posix_error(EINVAL);
		}
	}
};

template <::std::integral inchar_type, ::std::integral outchar_type>
class basic_general_io_io_observer
{
public:
	using input_char_type = inchar_type;
	using output_char_type = outchar_type;
	using native_handle_type = basic_io_io_base<inchar_type, outchar_type> *;
	native_handle_type handle{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return handle;
	}
	inline constexpr native_handle_type release() noexcept
	{
		native_handle_type temp{this->handle};
		this->handle = nullptr;
		return temp;
	}
};

template <::std::integral inchar_type, ::std::integral outchar_type>
inline constexpr basic_general_io_io_observer<inchar_type, outchar_type>
io_stream_ref_define(basic_general_io_io_observer<inchar_type, outchar_type> biob) noexcept
{
	return biob;
}

template <::std::integral inchar_type, ::std::integral outchar_type>
inline constexpr ::fast_io::io_scatter_status_t
scatter_read_some_underflow_define(basic_general_io_io_observer<inchar_type, outchar_type> biob,
								   ::fast_io::basic_io_scatter_t<inchar_type> const *pst, ::std::size_t n)

	FAST_IO_HERBCEPTIONS_THROWS
{
	return biob.handle->scatter_read_some_underflow_def(pst, n);
}

template <::std::integral inchar_type, ::std::integral outchar_type>
inline constexpr ::fast_io::io_scatter_status_t
scatter_pread_some_underflow_define(basic_general_io_io_observer<inchar_type, outchar_type> biob,
									::fast_io::basic_io_scatter_t<inchar_type> const *pst, ::std::size_t n,
									intfpos_t off)

	FAST_IO_HERBCEPTIONS_THROWS
{
	return biob.handle->scatter_pread_some_underflow_def(pst, n, off);
}

template <::std::integral inchar_type, ::std::integral outchar_type>
inline constexpr ::fast_io::intfpos_t
input_stream_seek_define(basic_general_io_io_observer<inchar_type, outchar_type> biob, ::fast_io::intfpos_t off,
						 ::fast_io::seekdir sdir)

	FAST_IO_HERBCEPTIONS_THROWS
{
	return biob.handle->input_stream_seek_def(off, sdir);
}

template <::std::integral inchar_type, ::std::integral outchar_type>
inline constexpr ::fast_io::io_scatter_status_t
scatter_write_some_overflow_define(basic_general_io_io_observer<inchar_type, outchar_type> biob,
								   ::fast_io::basic_io_scatter_t<inchar_type> const *pst, ::std::size_t n)

	FAST_IO_HERBCEPTIONS_THROWS
{
	return biob.handle->scatter_write_some_overflow_def(pst, n);
}

template <::std::integral inchar_type, ::std::integral outchar_type>
inline constexpr ::fast_io::io_scatter_status_t
scatter_pwrite_some_overflow_define(basic_general_io_io_observer<inchar_type, outchar_type> biob,
									::fast_io::basic_io_scatter_t<inchar_type> const *pst, ::std::size_t n,
									intfpos_t off)

	FAST_IO_HERBCEPTIONS_THROWS
{
	return biob.handle->scatter_pwrite_some_overflow_def(pst, n, off);
}

template <::std::integral inchar_type, ::std::integral outchar_type>
inline constexpr ::fast_io::intfpos_t
output_stream_seek_define(basic_general_io_io_observer<inchar_type, outchar_type> biob, ::fast_io::intfpos_t off,
						  ::fast_io::seekdir sdir)

	FAST_IO_HERBCEPTIONS_THROWS
{
	return biob.handle->output_stream_seek_def(off, sdir);
}

namespace details
{

template <typename allocatortype, typename T>
struct biobd_allocate_guard
{
	T *ptr{};
	inline constexpr biobd_allocate_guard() noexcept = default;

	inline explicit constexpr biobd_allocate_guard(T *p) noexcept
		: ptr(p)
	{}
	inline biobd_allocate_guard(biobd_allocate_guard const &) = delete;
	inline biobd_allocate_guard &operator=(biobd_allocate_guard const &) = delete;
	inline constexpr ~biobd_allocate_guard() noexcept
	{
		using typedallocator = typed_generic_allocator_adapter<allocatortype, T>;
		typedallocator::deallocate_n(ptr, 1);
	}
};

template <::std::integral inchartype, ::std::integral outchartype, typename allocatortype, typename T, typename... Args>
inline constexpr ::fast_io::basic_io_io_base<inchartype, outchartype> *create_io_file_handle_impl(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		!noexcept(::fast_io::typed_generic_allocator_adapter<allocatortype,
															 ::fast_io::basic_io_io_derived<inchartype, outchartype,
																							allocatortype, T>>::allocate(::std::size_t{1})) ||
		!::std::is_nothrow_constructible_v<
			::fast_io::basic_io_io_derived<inchartype, outchartype, allocatortype, T>,
			::fast_io::io_cookie_t, Args...>)
{
	using type = basic_io_io_derived<inchartype, outchartype, allocatortype, T>;
	using typedallocator = typed_generic_allocator_adapter<allocatortype, type>;
	type *ptr{typedallocator::allocate(1)};
	::fast_io::details::biobd_allocate_guard<allocatortype, type> g(ptr);
	::new (ptr) type(::fast_io::io_cookie, ::fast_io::freestanding::forward<Args>(args)...);
	g.ptr = nullptr;
	return ptr;
}

} // namespace details

template <::std::integral inchar_type, ::std::integral outchar_type, typename allocatortype>
class basic_general_io_file : public basic_general_io_io_observer<inchar_type, outchar_type>
{
public:
	using input_char_type = inchar_type;
	using output_char_type = outchar_type;
	using native_handle_type = basic_io_io_base<inchar_type, outchar_type> *;
	inline explicit constexpr basic_general_io_file() noexcept = default;
	template <typename T, typename... Args>
	inline explicit constexpr basic_general_io_file(io_cookie_type_t<T>, Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(
			::fast_io::details::create_io_file_handle_impl<inchar_type, outchar_type, allocatortype, T>(
				::std::declval<Args>()...)))
		: basic_general_io_io_observer<inchar_type, outchar_type>{
			  ::fast_io::details::create_io_file_handle_impl<inchar_type, outchar_type, allocatortype, T>(
				  ::fast_io::freestanding::forward<Args>(args)...)}
	{
	}
	template <typename P>
	inline constexpr basic_general_io_file(io_cookie_t, P &&p)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(
			::fast_io::details::create_io_file_handle_impl<inchar_type, outchar_type, allocatortype,
														   ::std::remove_reference_t<P>>(
				::std::declval<P>())))
		: basic_general_io_io_observer<inchar_type, outchar_type>{
			  ::fast_io::details::create_io_file_handle_impl<inchar_type, outchar_type, allocatortype,
															 ::std::remove_reference_t<P>>(
				  ::fast_io::freestanding::forward<P>(p))}
	{
	}
	inline basic_general_io_file(basic_general_io_file const &) = delete;
	inline basic_general_io_file &operator=(basic_general_io_file const &) = delete;
	inline constexpr basic_general_io_file(basic_general_io_file &&other) noexcept
		: basic_general_io_io_observer<inchar_type, outchar_type>{other.handle}
	{
		other.handle = nullptr;
	}
	inline constexpr basic_general_io_file &operator=(basic_general_io_file &&other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		delete this->handle;
		this->handle = other.handle;
		other.handle = nullptr;
		return *this;
	}
	inline constexpr ~basic_general_io_file() noexcept
	{
		delete this->handle;
	}
};

template <typename T>
class basic_general_io_file_ref
{
public:
	using general_io_file_type = T;
	using input_char_type = typename general_io_file_type::input_char_type;
	using output_char_type = typename general_io_file_type::output_char_type;
	general_io_file_type *giofptr{};
};

template <::std::integral char_type>
using basic_io_io_observer = basic_general_io_io_observer<char_type, char_type>;

template <::std::integral char_type, typename allocatortype = ::fast_io::native_thread_local_allocator>
using basic_io_file = basic_general_io_file<char_type, char_type, allocatortype>;

using io_io_observer = basic_io_io_observer<char>;
using io_file = basic_io_file<char>;
using wio_io_observer = basic_io_io_observer<wchar_t>;
using wio_file = basic_io_file<wchar_t>;
using u8io_io_observer = basic_io_io_observer<char8_t>;
using u8io_file = basic_io_file<char8_t>;
using u16io_io_observer = basic_io_io_observer<char16_t>;
using u16io_file = basic_io_file<char16_t>;
using u32io_io_observer = basic_io_io_observer<char32_t>;
using u32io_file = basic_io_file<char32_t>;

template <::std::integral char_type, typename allocatortype = ::fast_io::native_thread_local_allocator>
using basic_iobuf_io_file = basic_iobuf<basic_io_file<char_type, allocatortype>, allocatortype>;

using iobuf_io_file = basic_iobuf_io_file<char>;
using wiobuf_io_file = basic_iobuf_io_file<wchar_t>;
using u8iobuf_io_file = basic_iobuf_io_file<char8_t>;
using u16iobuf_io_file = basic_iobuf_io_file<char16_t>;
using u32iobuf_io_file = basic_iobuf_io_file<char32_t>;

} // namespace fast_io
