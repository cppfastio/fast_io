#pragma once

namespace fast_io::operations::decay::defines
{

template <typename T>
concept output_stream_operations_nothrow = requires() {
	typename T::output_char_type;
	requires ::std::integral<typename T::output_char_type>;
	typename T::output_nothrow_tag;
	requires ::std::same_as<typename T::output_nothrow_tag, ::fast_io::io_nothrow_tag>;
};

} // namespace fast_io::operations::decay::defines

namespace fast_io::operations::defines
{

template <typename T>
concept output_stream_operations_nothrow = ::fast_io::operations::decay::defines::output_stream_operations_nothrow<decltype(::fast_io::operations::output_stream_ref(::std::declval<T>()))>;

}
