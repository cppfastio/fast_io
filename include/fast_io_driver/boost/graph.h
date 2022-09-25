#pragma once

namespace fast_io
{

#ifdef BOOST_GRAPH_DETAIL_EDGE_HPP

template<std::integral char_type,::fast_io::buffer_output_stream bos,typename directed,typename vertex>
inline constexpr void print_define(io_reserve_type_t<char_type,::boost::detail::edge_desc_impl<directed,vertex>>,
	bos b,
	::boost::detail::edge_desc_impl<directed,vertex> const& e)
{
	print_freestanding(b,fast_io::mnp::chvw(char_literal_v<u8'(',char_type>),e.m_source,
		fast_io::mnp::chvw(char_literal_v<u8',',char_type>),e.m_target,
		fast_io::mnp::chvw(char_literal_v<u8')',char_type>));
}

#endif
}
