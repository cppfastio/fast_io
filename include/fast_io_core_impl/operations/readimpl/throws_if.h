#pragma once

namespace fast_io::operations::decay::defines
{

template <typename T>
concept input_stream_operations_nothrow = requires() {
	typename T::input_char_type;
	requires ::std::integral<typename T::input_char_type>;
	typename T::input_nothrow_tag;
	requires ::std::same_as<typename T::input_nothrow_tag, ::fast_io::io_nothrow_tag>;
};

} // namespace fast_io::operations::decay::defines

namespace fast_io::operations::defines
{

template <typename T>
concept input_stream_operations_nothrow = ::fast_io::operations::decay::defines::input_stream_operations_nothrow<decltype(::fast_io::operations::input_stream_ref(::std::declval<T>()))>;

}
