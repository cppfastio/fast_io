#pragma once

namespace fast_io
{

template<std::integral ch_type>
class basic_io_io_base
{
public:
	using char_type = ch_type;
	virtual
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	char_type* read_internal(char_type*,char_type*)=0;

	virtual
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	char_type const* write_internal(char_type const*,char_type const*)=0;

	virtual
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	std::uintmax_t seek_internal(std::intmax_t,seekdir)=0;

	virtual
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	basic_io_io_base* clone()=0;

	virtual
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	~basic_io_io_base() = default;
};
template<std::integral ch_type,stream stm>
class basic_io_io_derived:public basic_io_io_base<ch_type>
{
public:
	using char_type = ch_type;
	using handle_type = stm;
	using value_type = std::remove_reference_t<stm>;
	handle_type handle;
	template<typename... Args>
	requires std::constructible_from<handle_type,Args...>
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	basic_io_io_derived(io_cookie_type_t<handle_type>,Args&& ...args):handle(::fast_io::freestanding::forward<Args>(args)...){}
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	char_type* read_internal(char_type* b,char_type* e)
	{
		if constexpr(input_stream<value_type>)
			return read(handle,b,e);
		else
			throw_posix_error(EINVAL);
	}
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	char_type const* write_internal(char_type const* b,char_type const* e)
	{
		if constexpr(output_stream<value_type>)
		{
			if constexpr(std::same_as<decltype(write(handle,b,e)),void>)
			{
				write(handle,b,e);
				return e;
			}
			else
			{
				return write(handle,b,e);
			}
		}
		else
		{
			throw_posix_error(EINVAL);
		}
	}

#if __cpp_constexpr >= 201907L
	constexpr
#endif
	std::uintmax_t seek_internal(std::intmax_t off,seekdir dir)
	{
		if constexpr(output_stream<value_type>)
		{
			return seek(handle,off,dir);
		}
		else
		{
			throw_posix_error(EINVAL);
		}
	}
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	basic_io_io_base<char_type>* clone()
	{
		if constexpr(std::copyable<stm>)
		{
			return new basic_io_io_derived(*this);
		}
		else
		{
			throw_posix_error(EINVAL);
		}
	}
};

template<std::integral ch_type>
class basic_io_io_observer
{
public:
	using char_type = ch_type;
	using native_handle_type = basic_io_io_base<char_type>*;
	native_handle_type io_ptr{};
	constexpr auto release() noexcept
	{
		auto temp{io_ptr};
		io_ptr=nullptr;
		return temp;
	}
	constexpr auto& native_handle() noexcept
	{
		return io_ptr;
	}
	constexpr auto& native_handle() const noexcept
	{
		return io_ptr;
	}
};

template<std::integral ch_type>
class basic_io_file:public basic_io_io_observer<ch_type>
{
public:
	using typename basic_io_io_observer<ch_type>::char_type;
	using typename basic_io_io_observer<ch_type>::native_handle_type;
	constexpr basic_io_file()=default;
	constexpr basic_io_file(basic_io_io_observer<ch_type>) noexcept=delete;
	constexpr basic_io_file& operator=(basic_io_io_observer<ch_type>) noexcept=delete;
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	basic_io_file(basic_io_file const& other):basic_io_io_observer<ch_type>{other.io_ptr->clone()}{}
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	basic_io_file& operator=(basic_io_file const& other)
	{
		auto temp{other.io_ptr->clone()};
		delete this->io_ptr;
		this->io_ptr=temp;
		return *this;
	}
	constexpr basic_io_file(basic_io_file&& other) noexcept:basic_io_io_observer<ch_type>{other.io_ptr}
	{
		other.io_ptr=nullptr;
	}
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	basic_io_file& operator=(basic_io_file&& other) noexcept
	{
		if(__builtin_addressof(other)==this)
			return *this;
		delete this->io_ptr;
		this->io_ptr=other.io_ptr;
		other.io_ptr=nullptr;
		return *this;
	}

	template<typename native_hd>
	requires std::same_as<native_hd,native_handle_type>
	explicit constexpr basic_io_file(native_hd ptr) noexcept:basic_io_io_observer<ch_type>{ptr}{}
	explicit constexpr basic_io_file(decltype(nullptr)) noexcept=delete;

	template<stream smt,typename... Args>
	requires std::constructible_from<smt,Args...>
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	basic_io_file(io_cookie_type_t<smt>,Args&& ...args)
		:basic_io_io_observer<ch_type>{new basic_io_io_derived<char_type,smt>(io_cookie_type<smt>,::fast_io::freestanding::forward<Args>(args)...)}{}
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	~basic_io_file()
	{
		delete this->io_ptr;
	}
};

template<std::integral ch_type,::fast_io::freestanding::contiguous_iterator Iter>
requires (std::same_as<::fast_io::freestanding::iter_value_t<Iter>,ch_type>||std::same_as<ch_type,char>)
constexpr Iter read(basic_io_io_observer<ch_type> iob,Iter begin,Iter end)
{
	if constexpr(std::same_as<::fast_io::freestanding::iter_value_t<Iter>,ch_type>)
	{
		return iob.io_ptr->read_internal(::fast_io::freestanding::to_address(begin),::fast_io::freestanding::to_address(end))-::fast_io::freestanding::to_address(begin)+begin;
	}
	else
	{
		auto b(reinterpret_cast<ch_type*>(::fast_io::freestanding::to_address(begin)));
		return begin+(iob.io_ptr->read_internal(b,
			b+(end-begin)*sizeof(*begin))-b)/sizeof(*begin);
	}
}

template<std::integral ch_type,::fast_io::freestanding::contiguous_iterator Iter>
requires (std::same_as<::fast_io::freestanding::iter_value_t<Iter>,ch_type>||std::same_as<ch_type,char>)
constexpr Iter write(basic_io_io_observer<ch_type> iob,Iter begin,Iter end)
{
	if constexpr(std::same_as<::fast_io::freestanding::iter_value_t<Iter>,ch_type>)
	{
		return iob.io_ptr->write_internal(::fast_io::freestanding::to_address(begin),::fast_io::freestanding::to_address(end))-::fast_io::freestanding::to_address(begin)+begin;
	}
	else
	{
		auto b(reinterpret_cast<ch_type const*>(::fast_io::freestanding::to_address(begin)));
		return begin+(iob.io_ptr->write_internal(b,
			b+(end-begin)*sizeof(*begin))-b)/sizeof(*begin);
	}
}

template<std::integral ch_type>
constexpr std::uintmax_t seek(basic_io_io_observer<ch_type> iob,std::intmax_t offset=0,seekdir s=seekdir::cur)
{
	return iob.io_ptr->seek_internal(offset,s);
}

template<std::integral ch_type>
inline constexpr basic_io_io_observer<ch_type> io_value_handle(basic_io_io_observer<ch_type> iob) noexcept
{
	return iob;
}

using io_io_base = basic_io_io_base<char>;
using io_io_observer = basic_io_io_observer<char>;
using io_file = basic_io_file<char>;

using wio_io_base = basic_io_io_base<wchar_t>;
using wio_io_observer = basic_io_io_observer<wchar_t>;
using wio_file = basic_io_file<wchar_t>;

using u8io_io_base = basic_io_io_base<char8_t>;
using u8io_io_observer = basic_io_io_observer<char8_t>;
using u8io_file = basic_io_file<char8_t>;

using u16io_io_base = basic_io_io_base<char16_t>;
using u16io_io_observer = basic_io_io_observer<char16_t>;
using u16io_file = basic_io_file<char16_t>;

using u32io_io_base = basic_io_io_base<char32_t>;
using u32io_io_observer = basic_io_io_observer<char32_t>;
using u32io_file = basic_io_file<char32_t>;
}