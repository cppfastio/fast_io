#pragma once

namespace fast_io
{

template<typename handletype,
	typename decoratortp,
	buffer_mode mde,
	typename allocatortp,
	std::size_t bfs>
class basic_io_decorator_filter
{
public:
	using decorator_type = decoratortp;
};

};
