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
	using input_buffer_type = std::conditional_t<(traits_type::mode&buffer_mode::in)==buffer_mode::in,
		basic_io_buffer_pointers<input_char_type>,empty_buffer_pointers>;
	using output_buffer_type = std::conditional_t<(traits_type::mode&buffer_mode::out)==buffer_mode::out,
		basic_io_buffer_pointers<output_char_type>,empty_buffer_pointers>;

	using decorators_type = typename io_buffer_type::decorators_type;

	io_buffer_type *idoptr{};
};

template<typename handletype,
	typename iobuffertraits,
	typename decorators>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out)
inline constexpr basic_io_deco_filter_ref<basic_io_deco_filter<handletype,iobuffertraits,decorators>> output_stream_ref_define(
	basic_io_deco_filter<handletype,iobuffertraits,decorators> &r) noexcept
{
	return {__builtin_addressof(r)};
}

template<typename handletype,
	typename iobuffertraits,
	typename decorators>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out)
inline constexpr basic_io_deco_filter_ref<basic_io_deco_filter<handletype,iobuffertraits,decorators>> output_stream_ref_define(
	basic_io_deco_filter<handletype,iobuffertraits,decorators> &&r) noexcept
{
	return {__builtin_addressof(r)};
}

template<typename handletype,
	typename iobuffertraits,
	typename decorators>
requires ((iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr basic_io_deco_filter_ref<basic_io_deco_filter<handletype,iobuffertraits,decorators>> input_stream_ref_define(
	basic_io_deco_filter<handletype,iobuffertraits,decorators> &r) noexcept
{
	return {__builtin_addressof(r)};
}

template<typename handletype,
	typename iobuffertraits,
	typename decorators>
requires ((iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr basic_io_deco_filter_ref<basic_io_deco_filter<handletype,iobuffertraits,decorators>> input_stream_ref_define(
	basic_io_deco_filter<handletype,iobuffertraits,decorators> &&r) noexcept
{
	return {__builtin_addressof(r)};
}

template<typename handletype,
	typename iobuffertraits,
	typename decorators>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out&&(iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr basic_io_deco_filter_ref<basic_io_deco_filter<handletype,iobuffertraits,decorators>>
	io_stream_ref_define(basic_io_deco_filter<handletype,iobuffertraits,decorators> &r) noexcept
{
	return {__builtin_addressof(r)};
}

template<typename handletype,
	typename iobuffertraits,
	typename decorators>
requires ((iobuffertraits::mode&buffer_mode::out)==buffer_mode::out&&(iobuffertraits::mode&buffer_mode::in)==buffer_mode::in)
inline constexpr basic_io_deco_filter_ref<basic_io_deco_filter<handletype,iobuffertraits,decorators>>
	io_stream_ref_define(basic_io_deco_filter<handletype,iobuffertraits,decorators> &&r) noexcept
{
	return {__builtin_addressof(r)};
}

}
