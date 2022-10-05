#pragma once
#include"output_normal.h"
#include"output_deco.h"

namespace fast_io
{

namespace details
{

template<typename T,::std::random_access_iterator Iter>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void iobuf_write_unhappy_impl(T& t,Iter first,Iter last)
{
	if constexpr(has_external_decorator_impl<typename T::decorators_type>)
		iobuf_write_unhappy_decay_impl_deco<T::buffer_size>(io_ref(t.handle),
		external_decorator(t.decorators),
		t.obuffer,
		t.obuffer_external,
		first,last);
	else
		iobuf_write_unhappy_decay_impl<T::buffer_size>(io_ref(t.handle),t.obuffer,first,last);
}

}

template<stream handletype,
buffer_mode mde,
typename decorators,
std::size_t bfs,::std::random_access_iterator Iter>
requires (((mde&buffer_mode::out)==buffer_mode::out)&&details::allow_iobuf_punning<typename decorators::internal_type,Iter>)
inline constexpr void write(basic_io_buffer<handletype,mde,decorators,bfs>& bios,Iter first,Iter last)
{
	using iter_char_type = ::std::iter_value_t<Iter>;
	using char_type = typename decorators::internal_type;
	if constexpr(std::same_as<iter_char_type,char_type>)
	{
		if constexpr(::std::contiguous_iterator<Iter>&&!std::is_pointer_v<Iter>)
			write(bios,::std::to_address(first),::std::to_address(last));
		else
		{
			if constexpr((mde&buffer_mode::deco_out_no_internal)==buffer_mode::deco_out_no_internal)
			{
				details::write_with_deco(io_ref(bios.handle),
					external_decorator(bios.decorators),
					first,last,
					bios.obuffer_external,bfs);
			}
			else
			{
				std::size_t diff{static_cast<std::size_t>(last-first)};
				std::size_t remain_space{static_cast<std::size_t>(bios.obuffer.buffer_end-bios.obuffer.buffer_curr)};
				if(remain_space<diff)[[unlikely]]
				{
					details::iobuf_write_unhappy_impl(bios,first,last);
					return;
				}
				bios.obuffer.buffer_curr=details::non_overlapped_copy_n(first,diff,bios.obuffer.buffer_curr);
			}
		}
/*
To do : forward_iterator. Support std::forward_list, std::list, std::set and std::unordered_set
*/
	}
	else
		write(bios,reinterpret_cast<char const*>(::std::to_address(first)),
			reinterpret_cast<char const*>(::std::to_address(last)));
}


template<stream handletype,
buffer_mode mde,
typename decorators,
std::size_t bfs>
requires ((mde&buffer_mode::out)==buffer_mode::out&&
(mde&buffer_mode::deco_out_no_internal)!=buffer_mode::deco_out_no_internal)
inline constexpr auto obuffer_begin(basic_io_buffer<handletype,mde,decorators,bfs>& bios) noexcept
{
	return bios.obuffer.buffer_begin;
}


template<stream handletype,
buffer_mode mde,
typename decorators,
std::size_t bfs>
requires ((mde&buffer_mode::out)==buffer_mode::out&&
(mde&buffer_mode::deco_out_no_internal)!=buffer_mode::deco_out_no_internal)
inline constexpr auto obuffer_curr(basic_io_buffer<handletype,mde,decorators,bfs>& bios) noexcept
{
	return bios.obuffer.buffer_curr;
}

template<stream handletype,
buffer_mode mde,
typename decorators,
std::size_t bfs>
requires ((mde&buffer_mode::out)==buffer_mode::out)
inline constexpr auto obuffer_end(basic_io_buffer<handletype,mde,decorators,bfs>& bios) noexcept
{
	return bios.obuffer.buffer_end;
}

template<stream handletype,
buffer_mode mde,
typename decorators,
std::size_t bfs>
requires ((mde&buffer_mode::out)==buffer_mode::out&&
(mde&buffer_mode::deco_out_no_internal)!=buffer_mode::deco_out_no_internal)
inline constexpr void obuffer_set_curr(basic_io_buffer<handletype,mde,decorators,bfs>& bios,typename basic_io_buffer<handletype,mde,decorators,bfs>::char_type* ptr) noexcept
{
	bios.obuffer.buffer_curr=ptr;
}

template<stream handletype,
buffer_mode mde,
typename decorators,
std::size_t bfs>
requires ((mde&buffer_mode::out)==buffer_mode::out&&
(mde&buffer_mode::deco_out_no_internal)!=buffer_mode::deco_out_no_internal)
inline constexpr void obuffer_overflow(basic_io_buffer<handletype,mde,decorators,bfs>& bios,
	typename basic_io_buffer<handletype,mde,decorators,bfs>::char_type ch)
{
	if constexpr(details::has_external_decorator_impl<decorators>)
		details::iobuf_overflow_impl_deco(io_ref(bios.handle),external_decorator(bios.decorators),bios.obuffer,bios.obuffer_external,ch,bfs);
	else
		details::iobuf_overflow_impl(io_ref(bios.handle),bios.obuffer,ch,bfs);
}

template<zero_copy_output_stream handletype,
buffer_mode mde,
typename decorators,
std::size_t bfs>
requires ((mde&buffer_mode::out)==buffer_mode::out&&!details::has_external_decorator_impl<decorators>)
inline constexpr decltype(auto) zero_copy_out_handle(basic_io_buffer<handletype,mde,decorators,bfs>& bios)
{
	return zero_copy_out_handle(bios.handle);
}

namespace details
{

template<typename T>
concept has_file_lock_type_impl = requires(T t)
{
	file_lock(io_ref(t));
};

}

template<typename handletype,
buffer_mode mde,
typename decorators,
std::size_t bfs>
struct basic_io_buffer_file_lock
{
	basic_io_buffer<handletype,mde,decorators,bfs>* ptr{};
	template<typename RequestType>
	constexpr void lock(RequestType& req)
	{
		flush(*ptr);
		if constexpr(::fast_io::details::has_file_lock_type_impl<handletype>)
		{
			file_lock(io_ref(ptr->handle)).lock(req);
		}
	}
	template<typename RequestType>
	constexpr void unlock(RequestType& req) noexcept
	{
#if (defined(_MSC_VER)&&_HAS_EXCEPTIONS!=0) || (!defined(_MSC_VER)&&__cpp_exceptions)
#if __cpp_exceptions
		try
		{
#endif
#endif
			flush(*ptr);
#if (defined(_MSC_VER)&&_HAS_EXCEPTIONS!=0) || (!defined(_MSC_VER)&&__cpp_exceptions)
#if __cpp_exceptions
		}
		catch(...)
		{
		}
#endif
#endif
		if constexpr(::fast_io::details::has_file_lock_type_impl<handletype>)
		{
			file_lock(io_ref(ptr->handle)).unlock(req);
		}
	}
	template<typename RequestType>
	constexpr bool try_lock(RequestType& req)
	{
#if (defined(_MSC_VER)&&_HAS_EXCEPTIONS!=0) || (!defined(_MSC_VER)&&__cpp_exceptions)
#if __cpp_exceptions
		try
		{
#endif
#endif
			flush(*ptr);
#if (defined(_MSC_VER)&&_HAS_EXCEPTIONS!=0) || (!defined(_MSC_VER)&&__cpp_exceptions)
#if __cpp_exceptions
		}
		catch(...)
		{
			return false;
		}
#endif
#endif
		if constexpr(::fast_io::details::has_file_lock_type_impl<handletype>)
		{
			return file_lock(io_ref(ptr->handle)).try_lock(req);
		}
		else
		{
			return true;
		}
	}
};

template<stream handletype,
buffer_mode mde,
typename decorators,
std::size_t bfs>
inline constexpr decltype(auto) file_lock(basic_io_buffer<handletype,mde,decorators,bfs>& bios)
{
	return basic_io_buffer_file_lock<handletype,mde,decorators,bfs>{__builtin_addressof(bios)};
}

}