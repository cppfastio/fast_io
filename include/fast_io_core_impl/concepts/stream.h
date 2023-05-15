#pragma once

namespace fast_io
{

/**
 * A concept that determines if a given type satisfies the requirements of an output stream.
 *
 * A type satisfies the requirements of an output stream if it has a member function for writing data to it
 * and can be used as an argument for output functions. The concept checks if the given type meets these 
 * requirements by using the `has_output_or_io_stream_ref_define` trait defined in the `::fast_io::details` namespace.
 *
 * @tparam stmtype The type to check for output stream requirements.
 *
 * @returns `true` if the given type satisfies the requirements of an output stream, `false` otherwise.
 */
template<typename stmtype>
concept output_stream = ::fast_io::details::has_output_or_io_stream_ref_define<stmtype>;

/**
 * A concept that determines if a given type satisfies the requirements of an input stream.
 *
 * A type satisfies the requirements of an input stream if it has a member function for reading data from it
 * and can be used as an argument for input functions. The concept checks if the given type meets these
 * requirements by using the `has_input_or_io_stream_ref_define` trait defined in the `::fast_io::details` namespace.
 *
 * @tparam stmtype The type to check for input stream requirements.
 *
 * @returns `true` if the given type satisfies the requirements of an input stream, `false` otherwise.
 */
template<typename stmtype>
concept input_stream = ::fast_io::details::has_input_or_io_stream_ref_define<stmtype>;

/**
 * A concept that determines if a given type satisfies the requirements of an input/output stream.
 *
 * A type satisfies the requirements of an input/output stream if it has member functions for both reading and
 * writing data to it, and can be used as an argument for both input and output functions. The concept checks if
 * the given type meets these requirements by using the `has_input_or_io_stream_ref_define`, `has_output_or_io_stream_ref_define`,
 * and `has_io_stream_ref_define` traits defined in the `::fast_io::details` namespace.
 *
 * @tparam stmtype The type to check for input/output stream requirements.
 *
 * @returns `true` if the given type satisfies the requirements of an input/output stream, `false` otherwise.
 */
template<typename stmtype>
concept io_stream = output_stream<stmtype>&&input_stream<stmtype>&&::fast_io::details::has_io_stream_ref_define<stmtype>;


/**
 * A concept that determines if a given type satisfies the requirements of a buffered output stream.
 *
 * A type satisfies the requirements of a buffered output stream if it has member functions for writing data to
 * a buffer and flushing the buffer to the output stream, and can be used as an argument for output functions.
 * The concept checks if the given type meets these requirements by using the `has_output_or_io_stream_ref_define`
 * and `has_obuffer_ops` traits defined in the `::fast_io::details` and `::fast_io::details::streamreflect` namespaces,
 * respectively.
 *
 * @tparam stmtype The type to check for buffered output stream requirements.
 *
 * @returns `true` if the given type satisfies the requirements of a buffered output stream, `false` otherwise.
 */
template<typename stmtype>
concept buffer_output_stream = output_stream<stmtype>&&::fast_io::details::streamreflect::has_obuffer_ops<stmtype>;

/**
 * A concept that determines if a given type satisfies the requirements of a non-line-buffered output stream.
 *
 * A type satisfies the requirements of a non-line-buffered output stream if it has member functions for writing
 * data to a buffer and flushing the buffer to the output stream without any special handling for line buffering,
 * and can be used as an argument for output functions. The concept checks if the given type meets these requirements
 * by using the `buffer_output_stream` concept and the `has_obuffer_is_line_buffering_define` trait defined in the
 * `::fast_io::details::streamreflect` namespace.
 *
 * The reason the `noline_buffer_output_stream` concept exists is because the GNU C library's `stdio`'s `FILE*` has
 * a weird way of describing the output buffer pointer. The current buffer pointer may exceed the end buffer pointer
 * for output due to line buffering. For over 99% of `buffer_output_stream` types, they satisfy the requirements of
 * `noline_buffer_output_stream`, meaning the current buffer pointer cannot surpass the end buffer pointer.
 *
 * @tparam stmtype The type to check for non-line-buffered output stream requirements.
 *
 * @returns `true` if the given type satisfies the requirements of a non-line-buffered output stream, `false` otherwise.
 */
template<typename stmtype>
concept noline_buffer_output_stream = buffer_output_stream<stmtype>&&!::fast_io::details::streamreflect::has_obuffer_is_line_buffering_define<stmtype>;

/**
 * A concept that determines if a given type satisfies the requirements of a buffered input stream.
 *
 * A type satisfies the requirements of a buffered input stream if it has member functions for reading data from
 * a buffer, filling the buffer from the input stream, and resetting the buffer pointer, and can be used as an argument
 * for input functions. The concept checks if the given type meets these requirements by using the `input_stream`
 * concept and the `has_ibuffer_ops` trait defined in the `::fast_io::details::streamreflect` namespace.
 *
 * @tparam stmtype The type to check for buffered input stream requirements.
 *
 * @returns `true` if the given type satisfies the requirements of a buffered input stream, `false` otherwise.
 */
template<typename stmtype>
concept buffer_input_stream = input_stream<stmtype>&&::fast_io::details::streamreflect::has_ibuffer_ops<stmtype>;

/**
 * A concept that determines if a given type satisfies the requirements of a buffered I/O stream.
 *
 * A type satisfies the requirements of a buffered I/O stream if it has member functions for reading and writing
 * data from/to a buffer, filling the buffer from the input stream, resetting the buffer pointer, and flushing the
 * buffer to the output stream, and can be used as an argument for I/O functions. The concept checks if the given type
 * meets these requirements by using the `io_stream`, `buffer_input_stream`, and `buffer_output_stream` concepts.
 *
 * @tparam stmtype The type to check for buffered I/O stream requirements.
 *
 * @returns `true` if the given type satisfies the requirements of a buffered I/O stream, `false` otherwise.
 */
template<typename stmtype>
concept buffer_io_stream = io_stream<stmtype>&&buffer_input_stream<stmtype>&&buffer_output_stream<stmtype>;

/**
 * A concept that determines if a given type satisfies the requirements of a contiguous input stream.
 *
 * A type satisfies the requirements of a contiguous input stream if it has member functions for reading data from
 * a contiguous memory buffer, and can be used as an argument for I/O functions. The concept checks if the given type
 * meets these requirements by using the `buffer_input_stream` concept, and by checking if the input buffer is never
 * underflowed by using the `has_ibuffer_underflow_never_define` trait.
 *
 * @tparam stmtype The type to check for contiguous input stream requirements.
 *
 * @returns `true` if the given type satisfies the requirements of a contiguous input stream, `false` otherwise.
 */
template<typename stmtype>
concept contiguous_input_stream = buffer_input_stream<stmtype>&&::fast_io::details::streamreflect::has_ibuffer_underflow_never_define<stmtype>;

template<typename stmtype>
concept contiguous_output_stream = buffer_output_stream<stmtype>&&::fast_io::details::streamreflect::has_obuffer_overflow_never_define<stmtype>;


/**
 * A concept that determines if a given type satisfies the requirements of a constant size buffer output stream.
 *
 * A type satisfies the requirements of a constant size buffer output stream if it has member functions for writing
 * data to a fixed-size buffer, and can be used as an argument for I/O functions. The concept checks if the given type
 * meets these requirements by using the `buffer_output_stream` concept, and by checking if the output buffer has a
 * fixed size by using the `has_obuffer_constant_size` trait.
 *
 * @tparam stmtype The type to check for constant size buffer output stream requirements.
 *
 * @returns `true` if the given type satisfies the requirements of a constant size buffer output stream, `false`
 * otherwise.
 */
template<typename stmtype>
concept constant_size_buffer_output_stream = buffer_output_stream<stmtype>&&::fast_io::details::streamreflect::has_obuffer_constant_size<stmtype>;


/**
 * A concept that determines if a given type satisfies the requirements of a dynamic output stream.
 *
 * A type satisfies the requirements of a dynamic output stream if it has member functions for writing data to a dynamic
 * buffer that can resize as necessary, and can be used as an argument for I/O functions. The concept checks if the given
 * type meets these requirements by using the `buffer_output_stream` concept, and by checking if the output buffer can
 * resize as necessary by using the `has_obuffer_reserve_define` trait.
 *
 * @tparam stmtype The type to check for dynamic output stream requirements.
 *
 * @returns `true` if the given type satisfies the requirements of a dynamic output stream, `false` otherwise.
 */
template<typename stmtype>
concept dynamic_output_stream = buffer_output_stream<stmtype>&&::fast_io::details::streamreflect::has_obuffer_reserve_define<stmtype>;


/**
 * A concept that determines if a given type satisfies the requirements of a random-access output stream.
 *
 * A type satisfies the requirements of a random-access output stream if it has member functions for writing data to an
 * output buffer, and can seek to a specified position in the output buffer. The concept checks if the given type meets
 * these requirements by using the `output_stream` concept, and by checking if the type has the `has_output_stream_seek`
 * trait.
 *
 * @tparam stmtype The type to check for random-access output stream requirements.
 *
 * @returns `true` if the given type satisfies the requirements of a random-access output stream, `false` otherwise.
 */
template<typename stmtype>
concept random_access_output_stream = output_stream<stmtype>&&::fast_io::details::streamreflect::has_output_stream_seek<stmtype>;

/**
 * A concept that determines if a given type satisfies the requirements of a random-access input stream.
 *
 * A type satisfies the requirements of a random-access input stream if it has member functions for reading data from an
 * input buffer, and can seek to a specified position in the input buffer. The concept checks if the given type meets
 * these requirements by using the `input_stream` concept, and by checking if the type has the `has_input_stream_seek`
 * trait.
 *
 * @tparam stmtype The type to check for random-access input stream requirements.
 *
 * @returns `true` if the given type satisfies the requirements of a random-access input stream, `false` otherwise.
 */
template<typename stmtype>
concept random_access_input_stream = input_stream<stmtype>&&::fast_io::details::streamreflect::has_input_stream_seek<stmtype>;

/**
 * A concept that determines if a given type satisfies the requirements of a random-access I/O stream.
 *
 * A type satisfies the requirements of a random-access I/O stream if it has member functions for reading and writing
 * data to and from an input/output buffer, and can seek to a specified position in the input/output buffer. The concept
 * checks if the given type meets these requirements by using the `io_stream` concept, and by checking if the type has
 * the `has_io_stream_seek` trait.
 *
 * @tparam stmtype The type to check for random-access I/O stream requirements.
 *
 * @returns `true` if the given type satisfies the requirements of a random-access I/O stream, `false` otherwise.
 */
template<typename stmtype>
concept random_access_io_stream = io_stream<stmtype>&&
	random_access_input_stream<stmtype>&&
	random_access_output_stream<stmtype>&&
	::fast_io::details::streamreflect::has_io_stream_seek<stmtype>;

/**
 * @brief A concept for output streams that have a lock mechanism to ensure thread-safety.
 * 
 * A type T satisfies the `mutex_output_stream` concept if it is an output stream and
 * has a lock mechanism for thread-safe operations. This concept is useful when multiple
 * threads may be accessing the same output stream and there is a possibility of data races.
 *
 * @tparam T The type to be checked.
 */
template<typename stmtype>
concept mutex_output_stream = output_stream<stmtype>&&::fast_io::details::streamreflect::has_output_stream_mutex_ref<stmtype>;

/**
 * @brief A concept for input streams that have a lock mechanism to ensure thread-safety.
 * 
 * A type T satisfies the `mutex_input_stream` concept if it is an input stream and
 * has a lock mechanism for thread-safe operations. This concept is useful when multiple
 * threads may be accessing the same input stream and there is a possibility of data races.
 *
 * @tparam T The type to be checked.
 */
template<typename stmtype>
concept mutex_input_stream = input_stream<stmtype>&&::fast_io::details::streamreflect::has_input_stream_mutex_ref<stmtype>;

/**
 * @brief A concept for I/O streams that have a lock mechanism to ensure thread-safety.
 * 
 * A type T satisfies the `mutex_io_stream` concept if it is an I/O stream and
 * has a lock mechanism for thread-safe operations on both input and output. This concept
 * is useful when multiple threads may be accessing the same I/O stream and there is a
 * possibility of data races.
 *
 * @tparam T The type to be checked.
 */
template<typename stmtype>
concept mutex_io_stream = io_stream<stmtype>&&
	mutex_input_stream<stmtype>&&
	mutex_output_stream<stmtype>&&
	::fast_io::details::streamreflect::has_io_stream_mutex_ref<stmtype>;

/**
 * @brief Concept for determining if a stream provides byte-addressed output operations. 
 * This may allow a direct interaction with Unix writev(2) and pwritev(2) syscalls.
 * 
 * @tparam stmtype The type to check for byte output operations.
 */
template<typename stmtype>
concept byte_output_stream =
	output_stream<stmtype>&&
	(::fast_io::details::streamreflect::has_any_of_byte_write_operations<stmtype>
	||(::fast_io::details::streamreflect::has_output_stream_seek_bytes<stmtype>&&
	::fast_io::details::streamreflect::has_any_of_byte_pwrite_operations<stmtype>));

/**
 * Concept for determining if a stream provides byte-addressed input operations.
 * This may allow a direct interaction with UNIX readv(2) preadv(2) syscalls.
 * @tparam stmtype The type to check for byte input operations.
 */
template<typename stmtype>
concept byte_input_stream =
	input_stream<stmtype>&&
	(::fast_io::details::streamreflect::has_any_of_byte_read_operations<stmtype>||
	((::fast_io::details::streamreflect::has_input_stream_seek_bytes<stmtype>&&
	::fast_io::details::streamreflect::has_any_of_byte_pread_operations<stmtype>)));

/**
 * Concept for determining if a stream provides byte-addressed input operations.
 * This may allow a direct interaction with UNIX readv(2) preadv(2) syscalls.
 * @tparam stmtype The type to check for byte input operations.
 */
template<typename stmtype>
concept byte_io_stream =
	io_stream<stmtype>&&
	byte_input_stream<stmtype>&&
	byte_output_stream<stmtype>;

/**
 * @brief Concept for determining if a stream provides byte-addressed output operations with random access capability.
 * This may allow direct interaction with unix writev(2), pwritev(2), syscalls, and seek operations.
 * 
 * @tparam stmtype The type to check for byte output operations with random access capability.
 */
template<typename stmtype>
concept random_access_byte_output_stream = byte_output_stream<stmtype>&&
	::fast_io::details::streamreflect::has_output_stream_seek_bytes<stmtype>;

/**
 * @brief Concept for determining if a stream provides byte-addressed input operations with random access capability.
 * This may allow direct interaction with unix readv(2), preadv(2), syscalls, and seek operations.
 * 
 * @tparam stmtype The type to check for byte input operations with random access capability.
 */
template<typename stmtype>
concept random_access_byte_input_stream = byte_input_stream<stmtype>&&
	::fast_io::details::streamreflect::has_input_stream_seek_bytes<stmtype>;

/**
 * @brief Concept for determining if a stream provides byte-addressed input/output operations with random access capability.
 * This may allow direct interaction with unix readv(2), preadv(2), writev(2), pwritev(2), syscalls, and seek operations.
 * 
 * @tparam stmtype The type to check for byte input/output operations with random access capability.
 */
template<typename stmtype>
concept random_access_byte_io_stream = byte_io_stream<stmtype>&&
	random_access_byte_input_stream<stmtype>&&
	random_access_byte_output_stream<stmtype>&&
	::fast_io::details::streamreflect::has_io_stream_seek_bytes<stmtype>;


/**
 * Concept for a stream that provides status operations for input.
 * This allows users to override the behaviors for scan functions for custom behaviors at compile time.
 * A typical example would be to support fast_io's locale system.
 * @tparam stmtype The type to check for status input operations.
 */
template<typename stmtype>
concept status_input_stream = ::fast_io::details::streamreflect::statusinputstreamdef<stmtype>;

/**
 * Concept for a stream that provides status operations for output.
 * This allows users to override the behaviors for print functions for custom behaviors at compile time.
 * A typical example would be to support fast_io's locale system.
 * @tparam stmtype The type to check for status output operations.
 */
template<typename stmtype>
concept status_output_stream = ::fast_io::details::streamreflect::statusoutputstreamdef<stmtype>;

/**
 * Concept for a stream that provides status operations for both input and output.
 * This allows users to override the behaviors for print and scan functions for custom behaviors at compile time.
 * A typical example would be to support fast_io's locale system.
 * @tparam stmtype The type to check for status input and output operations.
 */
template<typename stmtype>
concept status_io_stream = status_input_stream<stmtype>&&
			status_input_stream<stmtype>&&
			::fast_io::details::streamreflect::statusiostreamdef<stmtype>;


/*
Zero copy needs better design
*/
template<typename stmtype>
concept zero_copy_input_stream =
input_stream<stmtype>&&::fast_io::details::streamreflect::has_zero_copy_in_handle<stmtype>;

template<typename stmtype>
concept zero_copy_output_stream =
output_stream<stmtype>&&::fast_io::details::streamreflect::has_zero_copy_out_handle<stmtype>;

}
