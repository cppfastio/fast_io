#pragma once
namespace fast_io
{

namespace operations::defines
{
template <typename handletype>
concept available_add_ibuf = requires {
	typename decltype(::fast_io::operations::input_stream_ref(*static_cast<handletype *>(nullptr)))::input_char_type;
};

template <typename handletype>
concept available_add_obuf = requires {
	typename decltype(::fast_io::operations::output_stream_ref(*static_cast<handletype *>(nullptr)))::output_char_type;
};

template <typename handletype>
concept available_add_iobuf = available_add_ibuf<handletype> || available_add_obuf<handletype>;

} // namespace operations::defines
} // namespace fast_io
