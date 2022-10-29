#pragma once

namespace fast_io
{

enum class parse_code:char unsigned
{
ok = 0,
end_of_file = 1,
partial = 2,
invalid = 3,
overflow = 4
};


template<typename Iter>
struct parse_result
{
	using iterator = Iter;
	iterator iter;
	parse_code code;
};

}
