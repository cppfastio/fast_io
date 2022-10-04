#pragma once

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif

namespace fast_io
{

struct empty_decorator
{
};

template<std::integral ch_type,
typename internaltype=empty_decorator,
typename externaltype=empty_decorator>
struct basic_decorators
{
	using internal_type = ch_type;
	using internal_decorator_type=internaltype;
	using external_decorator_type=externaltype;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	internal_decorator_type internal_decorator;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	external_decorator_type external_decorator;
};

template<std::integral char_type,typename internaltype,typename externaltype>
requires (!std::same_as<internaltype,empty_decorator>)
inline constexpr auto internal_decorator(basic_decorators<char_type,internaltype,externaltype>& decos) noexcept
{
	return io_deco_ref(decos.internal_decorator);
}

template<std::integral char_type,typename internaltype,typename externaltype>
requires (!std::same_as<externaltype,empty_decorator>)
inline constexpr auto external_decorator(basic_decorators<char_type,internaltype,externaltype>& decos) noexcept
{
	return io_deco_ref(decos.external_decorator);
}

namespace details
{

template<typename T,typename decot,::std::random_access_iterator Iter>
inline constexpr void write_with_deco(T t,decot deco,Iter first,Iter last,
	basic_io_buffer_pointers_no_curr<typename T::char_type>& external_buffer,
	std::size_t buffer_size)
{
	using external_char_type = typename T::char_type;
	using decot_no_cvref_t = std::remove_cvref_t<decot>;
#if 0
	if constexpr(requires()
	{
		deco_reserve_size(io_reserve_type<external_char_type,decot_no_cvref_t>,deco,first,last)
	})
	{
		for(;first!=last;)
		{
			std::size_t this_round{buffer_size};
			std::size_t diff{static_cast<std::size_t>(last-first)};
			if(diff<this_round)
				this_round=diff;
			std::size_t size{deco_reserve_size(io_reserve_type<external_char_type,decot_no_cvref_t>,deco,first,last)};
			std::size_t current_buffer_size{external_buffer.buffer_end-external_buffer.buffer_begin};
			if(current_buffer_size<size)
			{
				auto current_ptr{allocate_iobuf_space<external_char_type>(size)};
				deallocate_iobuf_space(external_buffer.buffer_begin,current_buffer_size);
			}
		}
	}
	else
#endif
	{
		if(external_buffer.buffer_begin==nullptr)
		{
			std::size_t size{deco_reserve_size(io_reserve_type<external_char_type,decot_no_cvref_t>,deco,buffer_size)};
			external_buffer.buffer_begin=allocate_iobuf_space<external_char_type>(size);
			external_buffer.buffer_end=external_buffer.buffer_begin+size;
		}
		for(auto buffer_begin{external_buffer.buffer_begin};first!=last;)
		{
			std::size_t this_round{buffer_size};
			std::size_t diff{static_cast<std::size_t>(last-first)};
			if(diff<this_round)
				this_round=diff;
			write(t,buffer_begin,deco_reserve_define(io_reserve_type<external_char_type,decot_no_cvref_t>,deco,first,first+this_round,buffer_begin));
			first+=this_round;
		}
	}
}

template<typename decorators_type>
concept has_internal_decorator_impl = requires(decorators_type&& decos)
{
	internal_decorator(decos);
};

template<typename decorators_type>
concept has_external_decorator_impl = requires(decorators_type&& decos)
{
	external_decorator(decos);
};


template<typename handle_type,typename... Args>
concept iobuffer_reopenable_impl =  requires(handle_type handle,Args&& ...args)
{
	handle.reopen(::std::forward<Args>(args)...);
};

}

template<stream handletype,
buffer_mode mde=buffer_mode::io|buffer_mode::secure_clear|buffer_mode::construct_decorator,
typename decoratorstypr=
basic_decorators<typename handletype::char_type>,
std::size_t bfs = io_default_buffer_size<typename decoratorstypr::internal_type>>
requires (details::constraint_buffer_mode<handletype>(mde))
class basic_io_buffer
{
public:
	using handle_type = handletype;
	using decorators_type = decoratorstypr;
	using char_type = typename decorators_type::internal_type;
	using external_char_type = typename handle_type::char_type;
	using pointer = char_type*;
	using const_pointer = char_type const*;
	inline static constexpr buffer_mode mode = mde;
	inline static constexpr std::size_t buffer_size = bfs;
	inline static constexpr bool need_secure_clear = (mode&buffer_mode::secure_clear)==buffer_mode::secure_clear;
	inline static constexpr bool has_ibuffer=(mode&buffer_mode::in)==buffer_mode::in;
	inline static constexpr bool has_obuffer=(mode&buffer_mode::out)==buffer_mode::out;
	inline static constexpr bool has_internal_decorator = details::has_internal_decorator_impl<decorators_type>;
	inline static constexpr bool has_external_decorator = details::has_external_decorator_impl<decorators_type>;

	using ibuffer_type = std::conditional_t<has_ibuffer,
	std::conditional_t<has_internal_decorator,
		basic_io_buffer_pointers_with_cap<char_type>,basic_io_buffer_pointers<char_type>>,
		empty_buffer_pointers>;

	using obuffer_type =std::conditional_t<has_obuffer&&
		(mode&buffer_mode::deco_out_no_internal)!=buffer_mode::deco_out_no_internal,
		basic_io_buffer_pointers<char_type>,empty_buffer_pointers>;

	using ibuffer_external_type = std::conditional_t<has_ibuffer&&details::has_internal_decorator_impl<decorators_type>,
	basic_io_buffer_pointers_only_begin<external_char_type>,
	empty_buffer_pointers>;

	using obuffer_external_type = std::conditional_t<(
	has_obuffer&&
	details::has_external_decorator_impl<decorators_type>),
	basic_io_buffer_pointers_no_curr<external_char_type>,
	empty_buffer_pointers>;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	ibuffer_type ibuffer;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	obuffer_type obuffer;

#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	ibuffer_external_type ibuffer_external;

#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	obuffer_external_type obuffer_external;

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
	decorators_type decorators;
private:
	constexpr void close_throw_impl()
	{
		if constexpr((mode&buffer_mode::out)==buffer_mode::out&&
			(mode&buffer_mode::deco_out_no_internal)!=buffer_mode::deco_out_no_internal)
		{
		if(obuffer.buffer_begin!=obuffer.buffer_curr)
		{
			if constexpr(details::has_external_decorator_impl<decorators_type>)
			{
				details::write_with_deco(io_ref(handle),
				external_decorator(decorators),
				obuffer.buffer_begin,obuffer.buffer_curr,
				obuffer_external,bfs);
			}
			else
			{
				write(io_ref(handle),obuffer.buffer_begin,obuffer.buffer_curr);
			}
		}
		}
	}
	constexpr void close_impl() noexcept
	{
		if constexpr((mode&buffer_mode::out)==buffer_mode::out)
		{
#if (defined(_MSC_VER)&&_HAS_EXCEPTIONS!=0) || (!defined(_MSC_VER)&&__cpp_exceptions)
#if __cpp_exceptions
			try
			{
#endif
#endif
				close_throw_impl();
#if (defined(_MSC_VER)&&_HAS_EXCEPTIONS!=0) || (!defined(_MSC_VER)&&__cpp_exceptions)
#if __cpp_exceptions
			}
			catch(...)
			{
			}
#endif
#endif
		}
	}
	constexpr void cleanup_impl() noexcept
	{
		if constexpr((mode&buffer_mode::out)==buffer_mode::out)
		{
			if constexpr((mode&buffer_mode::deco_out_no_internal)!=buffer_mode::deco_out_no_internal)
			{
			if(obuffer.buffer_begin)
				details::deallocate_iobuf_space<need_secure_clear,char_type>(obuffer.buffer_begin,buffer_size);
			}
			if constexpr(details::has_external_decorator_impl<decorators_type>)
			{
				if(obuffer_external.buffer_begin)
					details::deallocate_iobuf_space<need_secure_clear,external_char_type>(
						obuffer_external.buffer_begin,
						obuffer_external.buffer_end-obuffer_external.buffer_begin);
			}
		}
		if constexpr((mode&buffer_mode::in)==buffer_mode::in)
		{
			if(ibuffer.buffer_begin)
			{
				if constexpr(details::has_internal_decorator_impl<decorators_type>)
				{
					std::size_t real_buffer_cap{static_cast<std::size_t>(ibuffer.buffer_cap-ibuffer.buffer_begin)};
					details::deallocate_iobuf_space<need_secure_clear,char_type>(ibuffer.buffer_begin,real_buffer_cap);
				}
				else
				{
					details::deallocate_iobuf_space<need_secure_clear,char_type>(ibuffer.buffer_begin,buffer_size);
				}
			}
			if constexpr(details::has_internal_decorator_impl<decorators_type>)
			{
				if(ibuffer_external.buffer_begin)
					details::deallocate_iobuf_space<need_secure_clear,external_char_type>(
						ibuffer_external.buffer_begin,
						buffer_size);
			}
		}
	}
public:

	constexpr basic_io_buffer()=default;
	template<typename... Args>
	requires (((mode&buffer_mode::construct_decorator)!=buffer_mode::construct_decorator)&&std::constructible_from<handle_type,Args...>)
	explicit constexpr basic_io_buffer(Args&& ...args):handle(::std::forward<Args>(args)...){}

	template<typename... Args>
	requires (((mode&buffer_mode::construct_decorator)==buffer_mode::construct_decorator)
	&&std::constructible_from<handle_type,Args...>)
	explicit constexpr basic_io_buffer(decorators_type&& decos,Args&& ...args):handle(::std::forward<Args>(args)...),
		decorators(::std::move(decos)){}

	constexpr basic_io_buffer(basic_io_buffer const& other) requires std::copyable<handle_type>:handle(other.handle),decorators(other.decorators){}
	constexpr basic_io_buffer(basic_io_buffer const&)=delete;
	constexpr basic_io_buffer& operator=(basic_io_buffer const& other) requires std::copyable<handle_type>
	{
		close_throw_impl();
		if constexpr((mode&buffer_mode::in)==buffer_mode::in)
			ibuffer.buffer_curr=ibuffer.buffer_end;
		if constexpr((mode&buffer_mode::out)==buffer_mode::out&&
			(mode&buffer_mode::deco_out_no_internal)!=buffer_mode::deco_out_no_internal)
			obuffer.buffer_curr=obuffer.buffer_begin;
		handle=other.handle;
		decorators=other.decorators;
		return *this;
	}
	constexpr basic_io_buffer& operator=(basic_io_buffer const&)=delete;
	template<typename... Args>
	requires (std::movable<handle_type>&&std::constructible_from<handle_type,Args...>)||(details::iobuffer_reopenable_impl<handle_type,Args...>)
	constexpr void reopen(Args&& ...args)
	{
		close_impl();
		if constexpr((mode&buffer_mode::in)==buffer_mode::in)
			ibuffer.buffer_curr=ibuffer.buffer_end;
		if constexpr((mode&buffer_mode::out)==buffer_mode::out)
			obuffer.buffer_curr=obuffer.buffer_begin;
		if constexpr(details::iobuffer_reopenable_impl<handle_type,Args...>)
			handle.reopen(::std::forward<Args>(args)...);
		else
			handle=handle_type(::std::forward<Args>(args)...);
	}
	constexpr void close() requires requires()
	{
		handle.close();
	}
	{
		close_throw_impl();
		if constexpr((mode&buffer_mode::in)==buffer_mode::in)
			ibuffer.buffer_curr=ibuffer.buffer_end;
		if constexpr((mode&buffer_mode::out)==buffer_mode::out&&
			(mode&buffer_mode::deco_out_no_internal)!=buffer_mode::deco_out_no_internal)
			obuffer.buffer_curr=obuffer.buffer_begin;
		handle.close();
	}
	constexpr basic_io_buffer(basic_io_buffer&& other) noexcept requires(std::movable<handle_type>):
		ibuffer(other.ibuffer),obuffer(other.obuffer),
		ibuffer_external(other.ibuffer_external),obuffer_external(other.obuffer_external),
		handle(::std::move(other.handle)),decorators(::std::move(other.decorators))
	{
		other.ibuffer={};
		other.obuffer={};
		other.ibuffer_external={};
		other.obuffer_external={};
	}
	constexpr basic_io_buffer(basic_io_buffer&&) noexcept=delete;
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	basic_io_buffer& operator=(basic_io_buffer&& __restrict other) noexcept requires(std::movable<handle_type>)
	{
		if constexpr((mode&buffer_mode::out)==buffer_mode::out)
			close_impl();
		cleanup_impl();
		ibuffer=other.ibuffer;
		other.ibuffer={};
		obuffer=other.obuffer;
		other.obuffer={};
		ibuffer_external=other.ibuffer_external;
		other.ibuffer_external={};
		obuffer_external=other.obuffer_external;
		other.obuffer_external={};
		handle=::std::move(other.handle);
		decorators=::std::move(other.decorators);
		return *this;
	}
	constexpr basic_io_buffer& operator=(basic_io_buffer&& __restrict)=delete;
	constexpr void swap(basic_io_buffer&& other) noexcept requires std::swappable<handle_type>
	{
		std::ranges::swap(ibuffer,other.ibuffer);
		std::ranges::swap(obuffer,other.obuffer);
		std::ranges::swap(ibuffer_external,other.ibuffer_external);
		std::ranges::swap(obuffer_external,other.obuffer_external);
		std::ranges::swap(handle,other.handle);
		std::ranges::swap(decorators,other.decorators);
	}
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	~basic_io_buffer()
	{
		if constexpr((mode&buffer_mode::out)==buffer_mode::out)
			close_impl();
		cleanup_impl();
	}
};

}