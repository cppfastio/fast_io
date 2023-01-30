#pragma once

namespace fast_io
{

template<stream handletype,buffer_mode mde,typename decorators_type,std::size_t bfs>
inline constexpr void flush(basic_io_buffer<handletype,mde,decorators_type,bfs>& bios)
{
	if constexpr((mde&buffer_mode::out)==buffer_mode::out)
	{
		if constexpr(details::has_external_decorator_impl<decorators_type>)
			details::iobuf_output_flush_impl_deco<basic_io_buffer<handletype,mde,decorators_type,bfs>::need_secure_clear>(io_ref(bios.handle),external_decorator(bios.decorators),bios.obuffer,bfs);
		else
			details::iobuf_output_flush_impl(io_ref(bios.handle),bios.obuffer);
	}
	if constexpr((mde&buffer_mode::in)==buffer_mode::in)
		bios.ibuffer.buffer_end=bios.ibuffer.buffer_curr=bios.ibuffer.buffer_begin;
}

template<stream handletype,buffer_mode mde,typename decorators_type,std::size_t bfs>
requires ((mde&buffer_mode::secure_clear)==buffer_mode::secure_clear)
inline constexpr void require_secure_clear(basic_io_buffer<handletype,mde,decorators_type,bfs>&){}

template<stream handletype,buffer_mode mde,typename decorators_type,std::size_t bfs>
inline constexpr std::size_t obuffer_constant_size(fast_io::io_reserve_type_t<typename basic_io_buffer<handletype,mde,decorators_type,bfs>::char_type,basic_io_buffer<handletype,mde,decorators_type,bfs>>) noexcept
{
	return bfs;
}

template<stream handletype,buffer_mode mde,typename decorators_type,std::size_t bfs>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void obuffer_constant_flush_prepare(basic_io_buffer<handletype,mde,decorators_type,bfs>& bios) noexcept
{
	if constexpr((mde&buffer_mode::out)==buffer_mode::out)
	{
		if constexpr(details::has_external_decorator_impl<decorators_type>)
			details::iobuf_output_constant_flush_prepare_impl_deco<basic_io_buffer<handletype,mde,decorators_type,bfs>::need_secure_clear>(io_ref(bios.handle),external_decorator(bios.decorators),bios.obuffer,bfs);
		else
			details::iobuf_output_constant_flush_prepare_impl(io_ref(bios.handle),bios.obuffer,bfs);
	}
	if constexpr((mde&buffer_mode::in)==buffer_mode::in)
		bios.ibuffer.buffer_end=bios.ibuffer.buffer_curr=bios.ibuffer.buffer_begin;
}

}