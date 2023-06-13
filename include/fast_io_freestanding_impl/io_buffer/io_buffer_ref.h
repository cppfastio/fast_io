#pragma once

namespace fast_io
{

template<typename T>
class basic_io_buffer_ref
{
public:
	using io_buffer_type = T;
	using handle_type = typename io_buffer_type::handle_type;
	using traits_type = typename io_buffer_type::traits_type;
	using input_char_type = typename io_buffer_type::input_char_type;
	using output_char_type = typename io_buffer_type::output_char_type;
	using allocator_type = typename io_buffer_type::traits_type::allocator_type;
	using native_handle_type = io_buffer_type*;
	native_handle_type iobptr{};
};

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out&&(iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>>
	io_stream_ref_define(basic_io_buffer<handletype,iobuffertraits> &&r) noexcept
{
	return {__builtin_addressof(r)};
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out&&(iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>>
	io_stream_ref_define(basic_io_buffer<handletype,iobuffertraits> &r) noexcept
{
	return {__builtin_addressof(r)};
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out&&(iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>>
	io_stream_ref_define(basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>> r) noexcept
{
	return r;
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out&&(iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr decltype(auto)
	io_stream_deco_filter_ref_define(basic_io_buffer<handletype,iobuffertraits> &r) noexcept
{
	return io_stream_deco_filter_ref_define(r.handle);
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out&&(iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr decltype(auto)
	io_stream_deco_filter_ref_define(basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>> r) noexcept
{
	return io_stream_deco_filter_ref_define(r.iobptr->handle);
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out)
inline constexpr basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>> output_stream_ref_define(basic_io_buffer<handletype,iobuffertraits> &&r) noexcept
{
	return {__builtin_addressof(r)};
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out)
inline constexpr basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>> output_stream_ref_define(basic_io_buffer<handletype,iobuffertraits> &r) noexcept
{
	return {__builtin_addressof(r)};
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out)
inline constexpr basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>>
	output_stream_ref_define(basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>> r) noexcept
{
	return r;
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out)
inline constexpr decltype(auto)
	output_stream_deco_filter_ref_define(basic_io_buffer<handletype,iobuffertraits> &r) noexcept
{
	return output_stream_deco_filter_ref_define(r.handle);
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out)
inline constexpr decltype(auto)
	output_stream_deco_filter_ref_define(basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>> r) noexcept
{
	return output_stream_deco_filter_ref_define(r.iobptr->handle);
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>> input_stream_ref_define(
	basic_io_buffer<handletype,iobuffertraits> &&r) noexcept
{
	return {__builtin_addressof(r)};
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>> input_stream_ref_define(
	basic_io_buffer<handletype,iobuffertraits> &r) noexcept
{
	return {__builtin_addressof(r)};
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>>
	input_stream_ref_define(basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>> r) noexcept
{
	return r;
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr decltype(auto)
	input_stream_deco_filter_ref_define(basic_io_buffer<handletype,iobuffertraits> &r) noexcept
{
	return input_stream_deco_filter_ref_define(r.handle);
}

template<typename handletype,
	typename iobuffertraits>
requires ((iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr decltype(auto)
	input_stream_deco_filter_ref_define(basic_io_buffer_ref<basic_io_buffer<handletype,iobuffertraits>> r) noexcept
{
	return input_stream_deco_filter_ref_define(r.iobptr->handle);
}

}
