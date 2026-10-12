#pragma once

namespace fast_io
{

template <typename T>
class basic_io_buffer_transcode_ref
{
public:
	using io_buffer_type = T;
	using handle_type = typename io_buffer_type::handle_type;
	using traits_type = typename io_buffer_type::traits_type;
	using input_char_type = typename io_buffer_type::input_char_type;
	using output_char_type = typename io_buffer_type::output_char_type;
	using allocator_type = typename io_buffer_type::traits_type::allocator_type;
	using input_allocator_type = allocator_type;
	using output_allocator_type = allocator_type;
	using io_allocator_type = allocator_type;
	using allocator_handle_type = typename io_buffer_type::allocator_handle_type;
	using native_handle_type = io_buffer_type *;
	native_handle_type iobptr{};
};

template <typename handletype, typename iobuffertraits>
	requires((iobuffertraits::mode & buffer_mode::in) == buffer_mode::in &&
			 basic_io_buffer<handletype, iobuffertraits>::allocator_has_status)
inline constexpr typename basic_io_buffer<handletype, iobuffertraits>::allocator_handle_type
input_stream_allocator_handle_define(
	basic_io_buffer_transcode_ref<basic_io_buffer<handletype, iobuffertraits>> iobref) noexcept
{
	return iobref.iobptr->allocator_handle;
}

template <typename handletype, typename iobuffertraits>
	requires((iobuffertraits::mode & buffer_mode::out) == buffer_mode::out &&
			 basic_io_buffer<handletype, iobuffertraits>::allocator_has_status)
inline constexpr typename basic_io_buffer<handletype, iobuffertraits>::allocator_handle_type
output_stream_allocator_handle_define(
	basic_io_buffer_transcode_ref<basic_io_buffer<handletype, iobuffertraits>> iobref) noexcept
{
	return iobref.iobptr->allocator_handle;
}

template <typename handletype, typename iobuffertraits>
	requires((iobuffertraits::mode & buffer_mode::in) == buffer_mode::in &&
			 (iobuffertraits::mode & buffer_mode::out) == buffer_mode::out &&
			 basic_io_buffer<handletype, iobuffertraits>::allocator_has_status)
inline constexpr typename basic_io_buffer<handletype, iobuffertraits>::allocator_handle_type
io_stream_allocator_handle_define(
	basic_io_buffer_transcode_ref<basic_io_buffer<handletype, iobuffertraits>> iobref) noexcept
{
	return iobref.iobptr->allocator_handle;
}

template <typename handletype, typename iobuffertraits>
	requires((iobuffertraits::mode & buffer_mode::out) == buffer_mode::out &&
			 (iobuffertraits::mode & buffer_mode::in) == buffer_mode::in)
inline constexpr basic_io_buffer_transcode_ref<basic_io_buffer<handletype, iobuffertraits>>
io_stream_transcode_deco_filter_ref_define(basic_io_buffer<handletype, iobuffertraits> &&r) noexcept
{
	return {__builtin_addressof(r)};
}

template <typename handletype, typename iobuffertraits>
	requires((iobuffertraits::mode & buffer_mode::out) == buffer_mode::out &&
			 (iobuffertraits::mode & buffer_mode::in) == buffer_mode::in)
inline constexpr auto
io_stream_deco_filter_ref_define(basic_io_buffer_transcode_ref<basic_io_buffer<handletype, iobuffertraits>> r) noexcept
{
	return r;
}

namespace details
{

inline constexpr ::std::size_t compute_transcode_buffer_size(::std::size_t oldbuffersize, ::std::size_t oldchartypesize,
															 ::std::size_t newchartypesize) noexcept
{
	::std::size_t val{oldbuffersize * oldchartypesize / newchartypesize};
	if (val == 0)
	{
		val = 1;
	}
	return val;
}

} // namespace details

template <typename handletype, typename iobuffertraits, typename dectref>
inline constexpr auto
io_stream_transcode_deco_filter_define(basic_io_buffer_transcode_ref<basic_io_buffer<handletype, iobuffertraits>> rf,
									   dectref &&deco)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(io_stream_transcode_deco_filter_ref_define(
		::fast_io::freestanding::move(rf.iobptr->handle))))
{
	using newtype = decltype(io_stream_transcode_deco_filter_define(
		io_stream_transcode_deco_filter_ref_define(::fast_io::freestanding::move(rf.iobptr->handle)), deco));
	return ::fast_io::basic_io_buffer<
		newtype, ::fast_io::basic_io_buffer_traits<
					 iobuffertraits::mode, typename iobuffertraits::allocator_type, typename newtype::input_char_type,
					 typename newtype::output_char_type,
					 ::fast_io::details::compute_transcode_buffer_size(
						 iobuffertraits::input_buffer_size, sizeof(typename newtype::input_char_type),
						 sizeof(typename iobuffertraits::input_char_type)),
					 ::fast_io::details::compute_transcode_buffer_size(
						 iobuffertraits::output_buffer_size, sizeof(typename newtype::output_char_type),
						 sizeof(typename iobuffertraits::input_char_type))>>(
		io_stream_transcode_deco_filter_define(
			io_stream_transcode_deco_filter_ref_define(::fast_io::freestanding::move(rf.iobptr->handle)), deco));
}

template <typename handletype, typename iobuffertraits>
	requires((iobuffertraits::mode & buffer_mode::out) == buffer_mode::out)
inline constexpr basic_io_buffer_transcode_ref<basic_io_buffer<handletype, iobuffertraits>>
output_stream_transcode_deco_filter_ref_define(basic_io_buffer<handletype, iobuffertraits> &&r) noexcept
{
	return {__builtin_addressof(r)};
}

template <typename handletype, typename iobuffertraits>
	requires((iobuffertraits::mode & buffer_mode::out) == buffer_mode::out)
inline constexpr auto output_stream_deco_filter_ref_define(
	basic_io_buffer_transcode_ref<basic_io_buffer<handletype, iobuffertraits>> r) noexcept
{
	return r;
}

template <typename handletype, typename iobuffertraits>
	requires((iobuffertraits::mode & buffer_mode::in) == buffer_mode::in)
inline constexpr basic_io_buffer_transcode_ref<basic_io_buffer<handletype, iobuffertraits>>
input_stream_transcode_deco_filter_ref_define(basic_io_buffer<handletype, iobuffertraits> &&r) noexcept
{
	return {__builtin_addressof(r)};
}

template <typename handletype, typename iobuffertraits>
	requires((iobuffertraits::mode & buffer_mode::in) == buffer_mode::in)
inline constexpr auto input_stream_deco_filter_ref_define(
	basic_io_buffer_transcode_ref<basic_io_buffer<handletype, iobuffertraits>> r) noexcept
{
	return r;
}

} // namespace fast_io
