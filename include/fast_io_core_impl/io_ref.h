#pragma once

namespace fast_io
{

template<stream stm>
requires (!value_based_stream<stm>)
struct io_reference_wrapper
{
	using char_type = typename stm::char_type;
	using native_handle_type = stm*;
	stm* ptr{};
	inline constexpr void lock() requires(mutex_stream<stm>)
	{
		ptr->lock();
	}
	inline constexpr void unlock() requires(mutex_stream<stm>)
	{
		ptr->unlock();
	}
	inline constexpr decltype(auto) unlocked_handle() requires(mutex_stream<stm>)
	{
		return ptr->unlocked_handle();
	}
	inline constexpr stm* native_handle() const noexcept
	{
		return ptr;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return ptr;
	}
	inline constexpr stm* release() noexcept
	{
		auto temp{ptr};
		ptr=nullptr;
		return temp;
	}
};

namespace details
{
template<typename strm>
concept require_io_value_handle_impl = requires(strm& stm)
{
	io_value_handle(stm);
};

}

template<typename strm>
inline constexpr auto io_ref(strm& stm) noexcept
{
	if constexpr(details::require_io_value_handle_impl<strm>)
		return io_value_handle(stm);
	else
		return io_reference_wrapper<std::remove_cvref_t<strm>>{__builtin_addressof(stm)};
}

template<stream stm>
constexpr io_reference_wrapper<stm> io_value_handle(io_reference_wrapper<stm> wrap) noexcept
{
	return wrap;
}

template<output_stream output,::std::input_or_output_iterator Iter>
constexpr decltype(auto) write(io_reference_wrapper<output> out,Iter first,Iter last)
{
	return write(*out.ptr,first,last);
}

template<buffer_output_stream output>
constexpr void obuffer_overflow(io_reference_wrapper<output> out,typename std::remove_cvref_t<output>::char_type ch)
{
	obuffer_overflow(*out.ptr,ch);
}

template<buffer_output_stream output>
constexpr decltype(auto) obuffer_begin(io_reference_wrapper<output> out)
{
	return obuffer_begin(*out.ptr);
}

template<buffer_output_stream output>
constexpr decltype(auto) obuffer_curr(io_reference_wrapper<output> out)
{
	return obuffer_curr(*out.ptr);
}

template<buffer_output_stream output>
constexpr decltype(auto) obuffer_end(io_reference_wrapper<output> out)
{
	return obuffer_end(*out.ptr);
}

template<zero_copy_output_stream output>
constexpr decltype(auto) zero_copy_out_handle(io_reference_wrapper<output> out)
{
	return zero_copy_out_handle(*out.ptr);
}

template<buffer_output_stream output>
constexpr void obuffer_set_curr(io_reference_wrapper<output> out,std::remove_cvref_t<decltype(obuffer_curr(*out.ptr))> ptr)
{
	obuffer_set_curr(*out.ptr,ptr);
}

template<dynamic_output_stream output>
constexpr void oreserve(io_reference_wrapper<output> out,std::size_t n)
{
	oreserve(*out.ptr,n);
}

template<dynamic_output_stream output>
constexpr void oshrink_to_fit(io_reference_wrapper<output> out)
{
	oshrink_to_fit(*out.ptr);
}

template<scatter_output_stream output>
constexpr decltype(auto) scatter_write(io_reference_wrapper<output> out,io_scatters_t sp)
{
	return scatter_write(*out.ptr,sp);
}

template<buffer_output_stream output>
requires requires(output outp)
{
	obuffer_initialize(outp);
}
constexpr void obuffer_initialize(io_reference_wrapper<output> out)
{
	obuffer_initialize(*out.ptr);
}

template<input_stream input,::std::input_or_output_iterator Iter>
constexpr Iter read(io_reference_wrapper<input> in,Iter first,Iter last)
{
	return read(*in.ptr,first,last);
}
template<buffer_input_stream input>
constexpr bool ibuffer_underflow(io_reference_wrapper<input> in)
{
	return ibuffer_underflow(*in.ptr);
}

template<capacity_available_buffer_input_stream input>
constexpr decltype(auto) ibuffer_cap(io_reference_wrapper<input> in)
{
	return ibuffer_cap(*in.ptr);
}

template<buffer_input_stream input>
constexpr decltype(auto) ibuffer_begin(io_reference_wrapper<input> in)
{
	return ibuffer_begin(*in.ptr);
}

template<buffer_input_stream input>
constexpr decltype(auto) ibuffer_curr(io_reference_wrapper<input> in)
{
	return ibuffer_curr(*in.ptr);
}
template<buffer_input_stream input>
constexpr decltype(auto) ibuffer_end(io_reference_wrapper<input> in)
{
	return ibuffer_end(*in.ptr);
}

template<buffer_input_stream input>
constexpr void ibuffer_set_curr(io_reference_wrapper<input> in,std::remove_cvref_t<decltype(ibuffer_curr(*in.ptr))> ptr)
{
	ibuffer_set_curr(*in.ptr,ptr);
}

template<zero_copy_input_stream input>
constexpr decltype(auto) zero_copy_in_handle(io_reference_wrapper<input> in)
{
	return zero_copy_in_handle(*in.ptr);
}

template<scatter_input_stream input>
constexpr decltype(auto) scatter_read(io_reference_wrapper<input> in,io_scatters_t sp)
{
	return scatter_read(*in.ptr,sp);
}

template<buffer_input_stream input>
requires requires(input inp)
{
	ibuffer_initialize(inp);
}
constexpr void ibuffer_initialize(io_reference_wrapper<input> in)
{
	ibuffer_initialize(*in.ptr);
}

template<random_access_stream racs>
constexpr std::uintmax_t seek(io_reference_wrapper<racs> rac,std::intmax_t pos=0,seekdir sdir=seekdir::cur)
{
	return seek(*rac.ptr,pos,sdir);
}

template<secure_clear_requirement_stream scrs>
constexpr decltype(auto) require_secure_clear(io_reference_wrapper<scrs> sc)
{
	return require_secure_clear(*sc.ptr);
}

template<contiguous_input_stream cis>
constexpr void ibuffer_underflow_never(io_reference_wrapper<cis>){}

template<contiguous_output_stream cos>
constexpr void obuffer_overflow_never(io_reference_wrapper<cos>){}

template<flush_output_stream output>
constexpr void flush(io_reference_wrapper<output> co)
{
	flush(*co.ptr);
}

template<constant_buffer_output_stream output>
inline constexpr std::size_t obuffer_constant_size(
		io_reserve_type_t<typename io_reference_wrapper<output>::char_type,
		io_reference_wrapper<output>>) noexcept
{
	return obuffer_constant_size(io_reserve_type<typename output::char_type,output>);
}

template<constant_buffer_output_stream output>
inline constexpr void obuffer_constant_flush_prepare(io_reference_wrapper<output> co) noexcept
{
	obuffer_constant_flush_prepare(*co.ptr);
}

}