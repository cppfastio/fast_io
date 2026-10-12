#pragma once

namespace fast_io
{

template <typename Iter>
struct parse_result
{
	using iterator = Iter;
	iterator iter;
	::fast_io::freestanding::parse_errc code;
};

template <typename Iter>
struct context_print_result
{
	using iterator = Iter;
	iterator iter;
	bool done;
};

} // namespace fast_io
