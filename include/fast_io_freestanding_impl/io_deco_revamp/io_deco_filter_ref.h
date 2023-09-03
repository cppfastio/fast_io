#pragma once

namespace fast_io
{

template<typename T>
class basic_io_deco_filter_ref
{
public:
	using io_buffer_type = T;
	using handle_type = typename io_buffer_type::handle_type;
	using traits_type = typename io_buffer_type::traits_type;
	using input_char_type = typename traits_type::input_char_type;
	using output_char_type = typename traits_type::output_char_type;
	using decorators_type = typename io_buffer_type::decorators_type;

	io_buffer_type *idoptr{};
};

template<typename handletype,
	typename decorators,
	typename iobuffertraits>
inline constexpr basic_io_deco_filter_ref<basic_io_deco_filter<handletype,decorators,iobuffertraits>>
	io_stream_ref_define(basic_io_deco_filter<handletype,decorators,iobuffertraits> &r) noexcept
{
	return {__builtin_addressof(r.base)};
}

template<typename handletype,
	typename decorators,
	typename iobuffertraits>
inline constexpr basic_io_deco_filter_ref<basic_io_deco_filter<handletype,decorators,iobuffertraits>>
	io_stream_ref_define(basic_io_deco_filter<handletype,decorators,iobuffertraits> &&r) noexcept
{
	return {__builtin_addressof(r.base)};
}

template<typename handletype,
	typename decorators,
	typename iobuffertraits>
inline constexpr basic_io_deco_filter_ref<basic_io_deco_filter<handletype,decorators,iobuffertraits>>
	io_stream_ref_define(basic_io_deco_filter_ref<basic_io_deco_filter<handletype,decorators,iobuffertraits>> r) noexcept
{
	return r;
}

}
