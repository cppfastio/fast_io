#pragma once
/* fast_io_crypto/websocket/impl.h — RFC 6455 websocket wire layer.
 * Frame model + streaming header scanner + mask/unmask + encoder, and
 * the HTTP-upgrade handshake helpers (sha1 + base64 for
 * Sec-WebSocket-Accept). Direction policy is the caller's:
 * client-to-server frames MUST be masked, server-to-client MUST NOT —
 * the scanner reports the mask bit; a server checks hdr.mask itself. */

namespace fast_io
{

enum class websocket_opcode : ::std::uint_least8_t
{
	continuation = 0,
	text = 1,
	binary = 2,
	close = 8,
	ping = 9,
	pong = 10
};

struct websocket_frame_header
{
	bool fin{};
	bool mask{};
	::fast_io::websocket_opcode opcode{};
	::std::uint_least64_t payload_length{};
	::std::byte mask_key[4]{};
};

inline constexpr ::std::size_t websocket_accept_key_size{28};

namespace details
{

/* total header bytes given the second wire byte (mask bit + len7) */
inline constexpr ::std::size_t ws_frame_header_size_impl(::std::byte b1) noexcept
{
	::std::size_t n{2};
	switch (static_cast<::std::uint_least8_t>(b1) & 0x7f)
	{
	case 126:
		n += 2;
		break;
	case 127:
		n += 8;
		break;
	}
	if ((static_cast<::std::uint_least8_t>(b1) & 0x80) != 0)
	{
		n += 4;
	}
	return n;
}

/* parse a complete 2..14-byte header; false on a protocol violation:
 * rsv bits set (no extensions negotiated), reserved opcode, a
 * fragmented or oversized control frame, or a 64-bit length with the
 * top bit set */
inline constexpr bool ws_parse_frame_header_impl(::std::byte const *b,
												 ::fast_io::websocket_frame_header &hdr) noexcept
{
	auto const b0{static_cast<::std::uint_least8_t>(b[0])};
	auto const b1{static_cast<::std::uint_least8_t>(b[1])};
	if ((b0 & 0x70) != 0)
	{
		return false;
	}
	auto const op{static_cast<::std::uint_least8_t>(b0 & 0x0f)};
	switch (op)
	{
	case 0:
	case 1:
	case 2:
	case 8:
	case 9:
	case 10:
		break;
	default:
		return false;
	}
	hdr.opcode = static_cast<::fast_io::websocket_opcode>(op);
	hdr.fin = (b0 & 0x80) != 0;
	hdr.mask = (b1 & 0x80) != 0;
	auto const l7{static_cast<::std::uint_least8_t>(b1 & 0x7f)};
	::std::size_t off{2};
	if (l7 == 126)
	{
		hdr.payload_length =
			(static_cast<::std::uint_least64_t>(static_cast<::std::uint_least8_t>(b[2])) << 8) |
			static_cast<::std::uint_least8_t>(b[3]);
		off = 4;
	}
	else if (l7 == 127)
	{
		if ((static_cast<::std::uint_least8_t>(b[2]) & 0x80) != 0)
		{
			return false;
		}
		::std::uint_least64_t v{};
		for (::std::size_t i{}; i != 8; ++i)
		{
			v = (v << 8) | static_cast<::std::uint_least8_t>(b[2 + i]);
		}
		hdr.payload_length = v;
		off = 10;
	}
	else
	{
		hdr.payload_length = l7;
	}
	if (op >= 8 && (!hdr.fin || hdr.payload_length > 125))
	{
		return false;
	}
	if (hdr.mask)
	{
		::fast_io::details::my_memcpy(__builtin_addressof(hdr.mask_key), b + off, 4);
	}
	return true;
}

inline constexpr void ws_apply_mask_impl(::std::byte *data, ::std::size_t len,
										 ::std::byte const key[4]) noexcept
{
	for (::std::size_t i{}; i != len; ++i)
	{
		data[i] = static_cast<::std::byte>(static_cast<::std::uint_least8_t>(data[i]) ^
										   static_cast<::std::uint_least8_t>(key[i & 3]));
	}
}

/* encode a frame header; mask nullptr means unmasked (the server
 * direction). returns bytes written — 2, 4 or 10, +4 when masked */
inline constexpr ::std::size_t
ws_encode_frame_header_impl(::std::byte *out, bool fin, ::fast_io::websocket_opcode op,
							::std::uint_least64_t len, ::std::byte const *mask) noexcept
{
	out[0] = static_cast<::std::byte>((fin ? 0x80 : 0) | static_cast<::std::uint_least8_t>(op));
	::std::size_t off;
	::std::uint_least8_t const maskbit{static_cast<::std::uint_least8_t>(mask != nullptr ? 0x80 : 0)};
	if (len < 126)
	{
		out[1] = static_cast<::std::byte>(maskbit | len);
		off = 2;
	}
	else if (len <= 0xffff)
	{
		out[1] = static_cast<::std::byte>(maskbit | 126);
		out[2] = static_cast<::std::byte>(len >> 8);
		out[3] = static_cast<::std::byte>(len);
		off = 4;
	}
	else
	{
		out[1] = static_cast<::std::byte>(maskbit | 127);
		for (::std::size_t i{}; i != 8; ++i)
		{
			out[2 + i] = static_cast<::std::byte>(len >> (56 - i * 8));
		}
		off = 10;
	}
	if (mask != nullptr)
	{
		::fast_io::details::my_memcpy(out + off, mask, 4);
		off += 4;
	}
	return off;
}

/* base64 encode — returns chars written, ((in_size+2)/3)*4, '=' padding */
inline constexpr ::std::size_t ws_b64_encode_impl(char8_t *out, ::std::byte const *in,
												  ::std::size_t in_size) noexcept
{
	char8_t constexpr alphabet[]{
		u8"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
	auto const *start{out};
	for (::std::size_t i{}; i + 3 <= in_size; i += 3)
	{
		auto const v{(static_cast<::std::uint_least32_t>(static_cast<::std::uint_least8_t>(in[i])) << 16) |
					 (static_cast<::std::uint_least32_t>(static_cast<::std::uint_least8_t>(in[i + 1])) << 8) |
					 static_cast<::std::uint_least8_t>(in[i + 2])};
		out[0] = alphabet[v >> 18];
		out[1] = alphabet[(v >> 12) & 0x3f];
		out[2] = alphabet[(v >> 6) & 0x3f];
		out[3] = alphabet[v & 0x3f];
		out += 4;
	}
	auto const rem{in_size % 3};
	if (rem != 0)
	{
		auto const v{(static_cast<::std::uint_least32_t>(static_cast<::std::uint_least8_t>(in[in_size - rem])) << 16) |
					 (rem == 2 ? static_cast<::std::uint_least32_t>(static_cast<::std::uint_least8_t>(in[in_size - 1])) << 8
							   : 0u)};
		out[0] = alphabet[v >> 18];
		out[1] = alphabet[(v >> 12) & 0x3f];
		out[2] = rem == 2 ? alphabet[(v >> 6) & 0x3f] : static_cast<char8_t>(u8'=');
		out[3] = u8'=';
		out += 4;
	}
	return static_cast<::std::size_t>(out - start);
}

/* "key" + the rfc6455 magic GUID -> sha1 -> base64 into out (always
 * websocket_accept_key_size chars, includes the '=' pad) */
inline void ws_accept_key_impl(char8_t *out, char8_t const *key, ::std::size_t key_len) noexcept
{
	::fast_io::details::basic_md5_sha_context_impl<::fast_io::details::sha1::sha1,
												   ::fast_io::details::sha1::sha1_initializer, 64>
		ctx;
	ctx.update(reinterpret_cast<::std::byte const *>(key),
			   reinterpret_cast<::std::byte const *>(key + key_len));
	char8_t const guid[]{u8"258EAFA5-E914-47DA-95CA-C5AB0DC85B11"};
	ctx.update(reinterpret_cast<::std::byte const *>(guid),
			   reinterpret_cast<::std::byte const *>(guid + sizeof(guid) - 1));
	ctx.do_final();
	::std::byte digest[20];
	ctx.digest_to_byte_ptr(digest);
	ws_b64_encode_impl(out, digest, sizeof(digest));
}

} // namespace details

/* encode an unmasked frame header (server -> client). returns bytes
 * written — 2, 4 or 10 */
inline constexpr ::std::size_t
websocket_encode_frame_header(::std::byte *out, bool fin, ::fast_io::websocket_opcode op,
							  ::std::uint_least64_t len) noexcept
{
	return details::ws_encode_frame_header_impl(out, fin, op, len, nullptr);
}

/* encode a masked frame header (client -> server); the payload must
 * still be xored with the same key via websocket_apply_mask */
inline constexpr ::std::size_t
websocket_encode_frame_header_masked(::std::byte *out, bool fin, ::fast_io::websocket_opcode op,
									 ::std::uint_least64_t len, ::std::byte const mask[4]) noexcept
{
	return details::ws_encode_frame_header_impl(out, fin, op, len, mask);
}

inline constexpr void websocket_apply_mask(::std::byte *data, ::std::size_t len,
										   ::std::byte const key[4]) noexcept
{
	details::ws_apply_mask_impl(data, len, key);
}

/* Sec-WebSocket-Accept value for a client key — writes
 * websocket_accept_key_size (28) chars, no terminator */
inline void websocket_accept_key(char8_t *out, char8_t const *key, ::std::size_t key_len) noexcept
{
	details::ws_accept_key_impl(out, key, key_len);
}

/* base64 encode into out — returns chars written ((in_size+2)/3)*4.
 * a client builds Sec-WebSocket-Key this way over a 16-byte nonce */
inline constexpr ::std::size_t websocket_b64_encode(char8_t *out, ::std::byte const *in,
													::std::size_t in_size) noexcept
{
	return details::ws_b64_encode_impl(out, in, in_size);
}

/* --- streaming header scan manipulator --- */

namespace manipulators
{

struct websocket_frame_header_manip_t
{
	using manip_tag = ::fast_io::manip_tag_t;
	::fast_io::websocket_frame_header *reference;
};

inline constexpr ::fast_io::manipulators::websocket_frame_header_manip_t
websocket_frame_header_get(::fast_io::websocket_frame_header &h) noexcept
{
	return {__builtin_addressof(h)};
}

} // namespace manipulators

namespace details
{

template <::std::integral char_type>
struct websocket_frame_header_scan_state_t
{
	char_type buffer[14];
	::std::uint_least8_t size{};
	::std::uint_least8_t expected{};
};

} // namespace details

template <::std::integral char_type>
inline constexpr io_type_t<details::websocket_frame_header_scan_state_t<char_type>>
	scan_context_type(::fast_io::io_reserve_type_t<
					  char_type, ::fast_io::manipulators::websocket_frame_header_manip_t>) noexcept
{
	return {};
}

template <::std::integral char_type>
inline constexpr parse_result<char_type const *>
scan_context_define(::fast_io::io_reserve_type_t<
						char_type, ::fast_io::manipulators::websocket_frame_header_manip_t>,
					details::websocket_frame_header_scan_state_t<char_type> &sc,
					char_type const *begin, char_type const *end,
					::fast_io::manipulators::websocket_frame_header_manip_t t) noexcept
{
	while (begin != end)
	{
		::std::size_t const want{static_cast<::std::size_t>(sc.expected ? sc.expected : 2) -
								 sc.size};
		::std::size_t const avail{static_cast<::std::size_t>(end - begin)};
		::std::size_t const n{avail < want ? avail : want};
		for (::std::size_t i{}; i != n; ++i)
		{
			sc.buffer[sc.size + i] = begin[i];
		}
		sc.size = static_cast<::std::uint_least8_t>(sc.size + n);
		begin += n;
		if (sc.expected == 0 && sc.size == 2)
		{
			sc.expected = static_cast<::std::uint_least8_t>(details::ws_frame_header_size_impl(
				static_cast<::std::byte>(sc.buffer[1])));
		}
		if (sc.expected != 0 && sc.size == sc.expected)
		{
			::std::byte const *b{reinterpret_cast<::std::byte const *>(sc.buffer)};
			if (!details::ws_parse_frame_header_impl(b, *t.reference))
			{
				return {begin, ::fast_io::freestanding::parse_errc::invalid};
			}
			return {begin, ::fast_io::freestanding::parse_errc::ok};
		}
	}
	return {begin, ::fast_io::freestanding::parse_errc::partial};
}

template <::std::integral char_type>
inline constexpr ::fast_io::freestanding::parse_errc
scan_context_eof_define(::fast_io::io_reserve_type_t<
							char_type, ::fast_io::manipulators::websocket_frame_header_manip_t>,
						details::websocket_frame_header_scan_state_t<char_type> &sc,
						::fast_io::manipulators::websocket_frame_header_manip_t) noexcept
{
	return sc.expected != 0 && sc.size == sc.expected
			   ? ::fast_io::freestanding::parse_errc::ok
			   : ::fast_io::freestanding::parse_errc::end_of_file;
}

} // namespace fast_io
