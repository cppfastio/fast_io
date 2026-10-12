#pragma once

/*
basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> --
a non-owning view of a basic_tls_client, the TLS counterpart of
basic_posix_family_io_observer / basic_win32_family_socket_io_observer.
Copies share the underlying client state (secrets included), matching
io_observer semantics for every other family. The stream customization
points are the _bytes operations -- TLS carries an octet stream.
*/

namespace fast_io::tls
{

template <::std::integral ch_type, typename allocator_type = ::fast_io::native_global_allocator,
		  typename socket_observer_type = ::fast_io::native_socket_io_observer,
		  typename crypto = tls_default_crypto>
struct basic_tls_io_observer
{
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = basic_tls_client<allocator_type, socket_observer_type, crypto> *;
	native_handle_type handle{};

	inline constexpr native_handle_type native_handle() const noexcept
	{
		return handle;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return handle != nullptr;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle = nullptr;
		return temp;
	}
};

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto>
inline constexpr bool operator==(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> a,
								 basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> b) noexcept
{
	return a.handle == b.handle;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto>
inline constexpr auto operator<=>(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> a,
								  basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> b) noexcept
{
	return a.handle <=> b.handle;
}
#endif

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto>
inline constexpr basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto>
io_stream_ref_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> other) noexcept
{
	return other;
}

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto>
inline constexpr basic_tls_io_observer<char, allocator_type, socket_observer_type, crypto>
io_bytes_stream_ref_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> other) noexcept
{
	return {other.handle};
}

/*
The two record-layer primitives: everything else (write_all, scatters,
positional forms) is synthesized by the generic operations layer.
Before a handshake runs the stream is the plain transport.
*/
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::byte *read_some_bytes_underflow_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
													 ::std::byte *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	return first + details::tls_client_read_some(tob.handle, first, count);
}

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::byte const *write_some_bytes_overflow_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
														   ::std::byte const *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	return first + details::tls_client_write_some(tob.handle, first, count);
}

/* fully-configured handshake: caller supplies roots, checks, offload */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto>
inline void handshake_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
							 tls_client_config cfg) FAST_IO_HERBCEPTIONS_THROWS
{
	details::tls_client_handshake(tob.handle, __builtin_addressof(cfg));
}

/* graceful close_notify on an established session */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_close_notify(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob) noexcept
{
	details::tls_client_send_close_notify(tob.handle);
}

#if defined(__linux__)
/*
fd-level zero-copy transmit: splice() on the input side and sendfile()
on the output side ride the kTLS socket directly -- only while the
client's offloaded_ is set; userspace-mode streams get the generic
bounce-buffer path from operations-level transmit.
*/
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto>
inline constexpr ::fast_io::posix_transmit_entry
input_transmit_handle_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob) noexcept
	requires requires { tob.handle->sock_.fd; }
{
	return ::fast_io::posix_transmit_entry{tob.handle->sock_.fd};
}

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto>
inline constexpr ::fast_io::posix_transmit_entry
output_transmit_handle_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob) noexcept
	requires requires { tob.handle->sock_.fd; }
{
	return ::fast_io::posix_transmit_entry{tob.handle->sock_.fd};
}
#endif

} // namespace fast_io::tls

namespace fast_io::operations::decay::defines
{

template <typename streamtype, typename argtype>
concept has_handshake_define = requires(streamtype stm, argtype arg) {
	handshake_define(stm, arg);
};

} // namespace fast_io::operations::decay::defines

namespace fast_io::operations::decay
{

/*
 * handshake_decay: runs the TLS 1.3 handshake synchronously on the
 * stream's observer. arg is either tls::tls_client_config (full
 * control) or a u8cstring_view hostname (system trust bundle, chain +
 * SAN checks on -- defined in roots.h).
 */
template <typename streamtype, typename argtype>
inline void handshake_decay(streamtype stm, argtype arg) FAST_IO_HERBCEPTIONS_THROWS
	requires(::fast_io::operations::decay::defines::has_handshake_define<streamtype, argtype>)
{
	handshake_define(stm, arg);
}

} // namespace fast_io::operations::decay

namespace fast_io::operations
{

/*
 * handshake: TLS 1.3 client handshake on a TLS stream. instm decays to
 * its io_stream_ref, so basic_tls, its observers and buffered TLS
 * streams all arrive as basic_tls_io_observer.
 */
template <typename streamtype, typename argtype>
inline void handshake(streamtype &&stm, argtype arg) FAST_IO_HERBCEPTIONS_THROWS
	requires(::fast_io::operations::decay::defines::has_handshake_define<
			 decltype(::fast_io::operations::io_stream_ref(stm)), argtype>)
{
	::fast_io::operations::decay::handshake_decay(::fast_io::operations::io_stream_ref(stm), arg);
}

} // namespace fast_io::operations
