#pragma once

namespace fast_io
{

struct rw_type_tag_t
{
explicit inline constexpr rw_type_tag_t() noexcept = default;
};

inline constexpr rw_type_tag_t rw_type_tag{};

namespace details
{

template<typename F>
inline constexpr char const* write_some_contiguous_common_charptr_impl(F outstm,char const* first,char const* last)
{
	using char_type = typename F::char_type;
	if constexpr(requires()
	{
		write_some_define(outstm,first,last);
	})
	{
		return write_some_define(outstm,first,last);
	}
	else if constexpr(requires()
	{
		write_some_bytes_define(outstm,first,last);
	})
	{
		return write_some_bytes_define(outstm,first,last);
	}
}

template<typename F,typename char_type>
inline constexpr char_type* write_some_contiguous_common_chtypeptr_impl(F outstm,char_type* first,char_type* last)
{
	using char_type = typename F::char_type;
	if constexpr(requires()
	{
		write_some_define(outstm,first,last);
	})
	{
		return write_some_define(outstm,first,last);
	}
	else if constexpr(requires()
	{
		
	})
	{
		
	}
}

}

template<typename F,::std::forward_iterator Iter>
inline constexpr Iter write_some(F&& foo,Iter first, Iter last)
{
	if constexpr(::std::contiguous_iterator<Iter>)
	{
		
	}
	else
	{
		if constexpr(requires()
		{
			write_some_define(io_ref(foo),first,last);
		})
		{
			return write_some_define(io_ref(foo),first,last);
		}
		else if constexpr(requires()
		{
			
		})
		{
		}
		else if constexpr(requires()
		{
			write_all_define(io_ref(foo),first,last);
		})	
		{
			write_all_define(io_ref(foo),first,last);
			return last;
		}
		else if constexpr(requires(::std::byte const* ptr)
		{
			write_some_bytes_define(io_ref(foo),ptr,ptr);
		})
		{
			return write_some_bytes_define(io_ref(foo),first,last);
		}
		else if constexpr(requires(::std::byte const* ptr)
		{
			write_all_bytes_define(io_ref(foo),ptr,ptr);
		})
		{
			return write_all_bytes_define(io_ref(foo),first,last);
		}
	}
//	io_ref()
}
/*
template<typename Iter>
//requires 
inline constexpr void write_all(Iter first,Iter last)
{
	
}
*/

}
