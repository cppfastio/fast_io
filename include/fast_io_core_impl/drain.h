#pragma once

namespace fast_io
{

template<buffer_output_stream source_type>
constexpr basic_io_scatter_t<typename std::remove_cvref_t<source_type>::char_type> print_alias_define(io_alias_t,manip::drainage<source_type&> source)
{
	auto bg{obuffer_begin(source.reference)};
	auto curr{obuffer_curr(source.reference)};
	return {bg,static_cast<std::size_t>(curr-bg)};
}

template<output_stream dest_type,output_stream source_type>
inline constexpr void drain(dest_type&& dest,source_type&& source)
{
	print_freestanding(::std::forward<dest_type>(dest),drainage(source));
}


}