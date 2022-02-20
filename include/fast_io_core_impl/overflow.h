#pragma once

namespace fast_io
{

template<buffer_output_stream output>
inline constexpr void put(output& out,typename std::remove_cvref_t<output>::char_type ch)
{
	auto curr{obuffer_curr(out)};
	auto end{obuffer_end(out)};
/*
Referenced from glibc
https://github.com/bminor/glibc/blob/21c3f4b5368686ade28d90d8c7d79c4c95c72c1b/libio/bits/types/struct_FILE.h
*/
	if constexpr(noline_buffer_output_stream<output>)
	{
		if(curr==end)[[unlikely]]
		{
			obuffer_overflow(out,ch);
			return;
		}
	}
	else
	{
		if(end<=curr)[[unlikely]]
		{
			obuffer_overflow(out,ch);
			return;
		}
	}
	*curr=ch;
	obuffer_set_curr(out,++curr);

}
}
