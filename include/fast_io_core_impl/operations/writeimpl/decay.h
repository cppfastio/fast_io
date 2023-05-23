#pragma once

namespace fast_io::operations::decay
{

template<typename outstmtype>
inline constexpr typename outstmtype::input_char_type const* write_some_decay(outstmtype outsm,typename outstmtype::input_char_type const *first,typename outstmtype::input_char_type const *last)
{
	return ::fast_io::details::write_some_impl(outsm,first,last);
}

template<typename outstmtype>
inline constexpr void write_all_decay(outstmtype outsm,
	typename outstmtype::input_char_type const *first,
	typename outstmtype::input_char_type const *last)
{
	::fast_io::details::write_all_impl(outsm,first,last);
}

template<typename outstmtype>
inline constexpr ::std::byte const* write_some_bytes_decay(outstmtype outsm,::std::byte const *first,::std::byte const *last)
{
	return ::fast_io::details::write_some_bytes_impl(outsm,first,last);
}

template<typename outstmtype>
inline constexpr void write_all_bytes_decay(outstmtype outsm,
	::std::byte const *first,::std::byte const *last)
{
	::fast_io::details::write_all_bytes_impl(outsm,first,last);
}

template<typename outstmtype>
inline constexpr io_scatter_status_t scatter_write_some_decay(outstmtype outsm,
	basic_io_scatter_t<typename outstmtype::input_char_type> const *pscatters,
	::std::size_t n)
{
	return ::fast_io::details::scatter_write_some_impl(outsm,pscatters,n);
}

template<typename outstmtype>
inline constexpr io_scatter_status_t scatter_write_some_bytes_decay(outstmtype outsm,
	basic_io_scatter_t<typename outstmtype::input_char_type> const *pscatters,
	::std::size_t n)
{
	return ::fast_io::details::scatter_write_some_bytes_impl(outsm,pscatters,n);
}

template<typename outstmtype>
inline constexpr void scatter_write_all_decay(outstmtype outsm,
	basic_io_scatter_t<typename outstmtype::input_char_type> const *pscatters,
	::std::size_t n)
{
	::fast_io::details::scatter_write_all_impl(outsm,pscatters,n);
}

template<typename outstmtype>
inline constexpr void scatter_write_all_bytes_decay(outstmtype outsm,
	basic_io_scatter_t<typename outstmtype::input_char_type> const *pscatters,
	::std::size_t n)
{
	::fast_io::details::scatter_write_all_bytes_impl(outsm,pscatters,n);
}

template<typename outstmtype>
inline constexpr typename outstmtype::input_char_type const* pwrite_some_decay(outstmtype outsm,typename outstmtype::input_char_type const *first,typename outstmtype::input_char_type const *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pwrite_some_impl(outsm,first,last,off);
}

template<typename outstmtype>
inline constexpr void pwrite_all_decay(outstmtype outsm,
	typename outstmtype::input_char_type const *first,
	typename outstmtype::input_char_type const *last,
	::fast_io::intfpos_t off)
{
	::fast_io::details::pwrite_all_impl(outsm,first,last,off);
}

template<typename outstmtype>
inline constexpr ::std::byte const* pwrite_some_bytes_decay(outstmtype outsm,::std::byte const *first,::std::byte const *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pwrite_some_bytes_impl(outsm,first,last,off);
}

template<typename outstmtype>
inline constexpr void pwrite_all_bytes_decay(outstmtype outsm,
	::std::byte const *first,::std::byte const *last,::fast_io::intfpos_t off)
{
	::fast_io::details::pwrite_all_bytes_impl(outsm,first,last,off);
}

template<typename outstmtype>
inline constexpr io_scatter_status_t scatter_pwrite_some_decay(outstmtype outsm,
	basic_io_scatter_t<typename outstmtype::input_char_type> const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	return ::fast_io::details::scatter_pwrite_some_impl(outsm,pscatters,n,off);
}

template<typename outstmtype>
inline constexpr io_scatter_status_t scatter_pwrite_some_bytes_decay(outstmtype outsm,
	basic_io_scatter_t<typename outstmtype::input_char_type> const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	return ::fast_io::details::scatter_pwrite_some_bytes_impl(outsm,pscatters,n,off);
}

template<typename outstmtype>
inline constexpr void scatter_pwrite_all_decay(outstmtype outsm,
	basic_io_scatter_t<typename outstmtype::input_char_type> const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	::fast_io::details::scatter_pwrite_all_impl(outsm,pscatters,n,off);
}

template<typename outstmtype>
inline constexpr void scatter_pwrite_all_bytes_decay(outstmtype outsm,
	basic_io_scatter_t<typename outstmtype::input_char_type> const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	::fast_io::details::scatter_pwrite_all_bytes_impl(outsm,pscatters,n,off);
}

}
