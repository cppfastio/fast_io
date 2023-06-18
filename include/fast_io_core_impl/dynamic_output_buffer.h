#pragma once

namespace fast_io
{

template<std::integral char_type,::std::size_t buffersize,typename allocatortype>
class basic_generic_dynamic_output_buffer
{
public:
	using output_char_type = char_type;
	using allocator_type = allocatortype;
	static inline constexpr ::std::size_t buffer_size{buffersize};
	char_type *begin_ptr{buffer},*curr_ptr{buffer},*end_ptr{buffer+buffer_size};
	char_type buffer[buffersize];
	constexpr basic_generic_dynamic_output_buffer() noexcept = default;

	basic_generic_dynamic_output_buffer(basic_generic_dynamic_output_buffer const&)=delete;
	basic_generic_dynamic_output_buffer& operator=(basic_generic_dynamic_output_buffer const&)=delete;

	constexpr ~basic_generic_dynamic_output_buffer()
	{
		if(begin_ptr&&begin_ptr!=buffer)
		{
			using typed_allocator = typed_generic_allocator_adapter<allocator_type,char_type>;
			if constexpr(typed_allocator::has_deallocate)
				typed_allocator::deallocate(begin_ptr);
			else
				typed_allocator::deallocate_n(begin_ptr,buffer_size);
		}
	}
};

template<std::integral char_type,typename allocatortype = ::fast_io::native_thread_local_allocator>
using basic_dynamic_output_buffer = basic_generic_dynamic_output_buffer<char_type,256,allocatortype>;

template<typename T>
class basic_dynamic_output_buffer_ref
{
public:
	using dynamic_output_buffer_type = T;
	using output_char_type = typename dynamic_output_buffer_type::output_char_type;
	using native_handle_type = dynamic_output_buffer_type*;
	native_handle_type dob_ptr{};
};

template<std::integral char_type,::std::size_t buffersize,typename allocatortype>
inline constexpr basic_dynamic_output_buffer_ref<basic_generic_dynamic_output_buffer<char_type,buffersize,allocatortype>> output_stream_ref_define(basic_generic_dynamic_output_buffer<char_type,buffersize,allocatortype>& diob) noexcept
{
	return {__builtin_addressof(diob)};
}

namespace details
{

template<std::integral char_type,::std::size_t buffersize,typename allocatortype>
inline constexpr void write_all_overflow_define_impl(
		basic_generic_dynamic_output_buffer<char_type,buffersize,allocatortype>& bob,
		char_type const *first,char_type const *last)
{
	using typed_allocator = typed_generic_allocator_adapter<allocatortype,char_type>;
	::std::size_t bfsz{static_cast<::std::size_t>(bob.end_ptr-bob.begin_ptr)};
	::std::size_t rlsz{static_cast<::std::size_t>(bob.curr_ptr-bob.begin_ptr)};
	::std::size_t diff{static_cast<::std::size_t>(last-first)};
	::std::size_t to_allocate{bfsz+diff};
	::std::size_t twicebfsz;
	constexpr
		::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
	constexpr
		::std::size_t mxdv2{mx>>1u};
	if(bfsz>mxdv2)
	{
		twicebfsz=mx;
	}
	else
	{
		twicebfsz=bfsz;
	}
	if(to_allocate<twicebfsz)
	{
		to_allocate=twicebfsz;
	}
	char_type *pbuffer;
	if(bob.begin_ptr!=bob.buffer)
	{
		pbuffer=typed_allocator::reallocate_n(bob.buffer,to_allocate);
	}
	else
	{
		pbuffer=typed_allocator::allocate(to_allocate);
		::fast_io::details::non_overlapped_copy_n(bob.begin_ptr,to_allocate,pbuffer);
	}
	bob.begin_ptr=pbuffer;
	bob.end_ptr=pbuffer+to_allocate;
	auto curit{pbuffer+rlsz};
	bob.curr_ptr=::fast_io::details::non_overlapped_copy(first,last,curit);
}

template<std::integral char_type,::std::size_t buffersize,typename allocatortype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void grow_twice_define_impl(
		basic_generic_dynamic_output_buffer<char_type,buffersize,allocatortype>& bob)
{
	using typed_allocator = typed_generic_allocator_adapter<allocatortype,char_type>;
	::std::size_t bfsz{static_cast<::std::size_t>(bob.end_ptr-bob.begin_ptr)};
	constexpr
		::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
	constexpr
		::std::size_t mxdv2{mx>>1u};
	::std::size_t twicebfsz;
	if(bfsz>mxdv2)
	{
		twicebfsz=mx;
	}
	else
	{
		twicebfsz=bfsz<<1u;
	}
	char_type *pbuffer{typed_allocator::reallocate_n(bob.buffer,twicebfsz)};
	bob.begin_ptr=pbuffer;
	bob.curr_ptr=pbuffer+bfsz;
	bob.end_ptr=pbuffer+twicebfsz;
}

}

template<std::integral char_type,::std::size_t buffersize,typename allocatortype>
inline constexpr void write_all_overflow_define(basic_dynamic_output_buffer_ref<basic_generic_dynamic_output_buffer<char_type,buffersize,allocatortype>> bdobr,
						char_type const *first,char_type const *last)
{
	::fast_io::details::write_all_overflow_define_impl(*bdobr.dob_ptr,first,last);
}

template<std::integral char_type,::std::size_t buffersize,typename allocatortype>
inline constexpr char_type* obuffer_begin(basic_dynamic_output_buffer_ref<basic_generic_dynamic_output_buffer<char_type,buffersize,allocatortype>> bdobr) noexcept
{
	return bdobr.dob_ptr->begin_ptr;
}

template<std::integral char_type,::std::size_t buffersize,typename allocatortype>
inline constexpr char_type* obuffer_curr(basic_dynamic_output_buffer_ref<basic_generic_dynamic_output_buffer<char_type,buffersize,allocatortype>> bdobr) noexcept
{
	return bdobr.dob_ptr->curr_ptr;
}

template<std::integral char_type,::std::size_t buffersize,typename allocatortype>
inline constexpr char_type* obuffer_end(basic_dynamic_output_buffer_ref<basic_generic_dynamic_output_buffer<char_type,buffersize,allocatortype>> bdobr) noexcept
{
	return bdobr.dob_ptr->end_ptr;
}

template<std::integral char_type,::std::size_t buffersize,typename allocatortype>
inline constexpr void obuffer_set_curr(basic_dynamic_output_buffer_ref<basic_generic_dynamic_output_buffer<char_type,buffersize,allocatortype>> bdobr,
						char_type *it) noexcept
{
	bdobr.dob_ptr->curr_ptr=it;
}

template<std::integral char_type,::std::size_t buffersize,typename allocatortype>
inline constexpr void obuffer_overflow(basic_dynamic_output_buffer_ref<basic_generic_dynamic_output_buffer<char_type,buffersize,allocatortype>> bdobr,
						char_type ch) noexcept
{
	::fast_io::details::grow_twice_define_impl(*bdobr.dob_ptr);
	*bdobr.dob_ptr->curr_ptr=ch;
	++bdobr.dob_ptr->curr_ptr;
}

}
