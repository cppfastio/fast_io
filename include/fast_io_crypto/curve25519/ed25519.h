#pragma once

/*
Generic class wrapper over the ed25519 API.
The free functions live in ::fast_io::curve25519::details; this class
exposes the same surface as static members for template code written
generically over signature algorithms.
*/

namespace fast_io
{

class ed25519
{
public:
	using blinding_context = ::fast_io::curve25519::edp_blinding_context;
	using verify_context = ::fast_io::curve25519::ed25519_verify_context;

	static inline constexpr ::std::size_t secret_key_size{32};
	static inline constexpr ::std::size_t public_key_size{32};
	static inline constexpr ::std::size_t private_key_size{64};
	static inline constexpr ::std::size_t signature_size{64};

	static inline constexpr blinding_context const &get_default_blinding() noexcept
	{
		return ::fast_io::curve25519::default_blinding;
	}

	static inline constexpr void create_key_pair_to_ptr(::std::byte *public_key, ::std::byte *private_key, ::std::byte const *secret_key) noexcept
	{
		::fast_io::curve25519::details::ed25519_create_key_pair_to_ptr(public_key, private_key, secret_key);
	}

	static inline constexpr void create_key_pair_with_blinding_to_ptr(::std::byte *public_key, ::std::byte *private_key, ::std::byte const *secret_key, blinding_context &blinding) noexcept
	{
		::fast_io::curve25519::details::ed25519_create_key_pair_with_blinding_to_ptr(public_key, private_key, secret_key, blinding);
	}

	static inline constexpr void sign_message_to_ptr(::std::byte *signature, ::std::byte const *private_key, ::std::byte const *msg, ::std::size_t msg_size) noexcept
	{
		::fast_io::curve25519::details::ed25519_sign_message_to_ptr(signature, private_key, msg, msg_size);
	}

	static inline constexpr void sign_message_with_blinding_to_ptr(::std::byte *signature, ::std::byte const *private_key, ::std::byte const *msg, ::std::size_t msg_size, blinding_context &blinding) noexcept
	{
		::fast_io::curve25519::details::ed25519_sign_message_with_blinding_to_ptr(signature, private_key, msg, msg_size, blinding);
	}

	static inline constexpr void verify_init_to_ptr(verify_context &ctx, ::std::byte const *public_key) noexcept
	{
		::fast_io::curve25519::details::ed25519_verify_init_to_ptr(ctx, public_key);
	}

	static inline constexpr bool verify_check_to_ptr(verify_context const &ctx, ::std::byte const *signature, ::std::byte const *msg, ::std::size_t msg_size) noexcept
	{
		return ::fast_io::curve25519::details::ed25519_verify_check_to_ptr(ctx, signature, msg, msg_size);
	}

	static inline constexpr bool verify_signature_to_ptr(::std::byte const *signature, ::std::byte const *public_key, ::std::byte const *msg, ::std::size_t msg_size) noexcept
	{
		return ::fast_io::curve25519::details::ed25519_verify_signature_to_ptr(signature, public_key, msg, msg_size);
	}

	static inline constexpr blinding_context &blinding_init_to_ptr(blinding_context &ctx, ::std::byte const *seed, ::std::size_t seed_size) noexcept
	{
		return ::fast_io::curve25519::details::ed25519_blinding_init_to_ptr(ctx, seed, seed_size);
	}

	static inline constexpr void create_key_pair(::fast_io::containers::index_span<::std::byte, public_key_size> public_key, ::fast_io::containers::index_span<::std::byte, private_key_size> private_key, ::fast_io::containers::index_span<::std::byte const, secret_key_size> secret_key) noexcept
	{
		::fast_io::curve25519::details::ed25519_create_key_pair(public_key, private_key, secret_key);
	}

	static inline constexpr void create_key_pair_with_blinding(::fast_io::containers::index_span<::std::byte, public_key_size> public_key, ::fast_io::containers::index_span<::std::byte, private_key_size> private_key, ::fast_io::containers::index_span<::std::byte const, secret_key_size> secret_key, blinding_context &blinding) noexcept
	{
		::fast_io::curve25519::details::ed25519_create_key_pair_with_blinding(public_key, private_key, secret_key, blinding);
	}

	static inline constexpr void sign_message(::fast_io::containers::index_span<::std::byte, signature_size> signature, ::fast_io::containers::index_span<::std::byte const, private_key_size> private_key, ::fast_io::containers::span<::std::byte const> msg) noexcept
	{
		::fast_io::curve25519::details::ed25519_sign_message(signature, private_key, msg);
	}

	static inline constexpr void sign_message_with_blinding(::fast_io::containers::index_span<::std::byte, signature_size> signature, ::fast_io::containers::index_span<::std::byte const, private_key_size> private_key, ::fast_io::containers::span<::std::byte const> msg, blinding_context &blinding) noexcept
	{
		::fast_io::curve25519::details::ed25519_sign_message_with_blinding(signature, private_key, msg, blinding);
	}

	static inline constexpr void verify_init(verify_context &ctx, ::fast_io::containers::index_span<::std::byte const, public_key_size> public_key) noexcept
	{
		::fast_io::curve25519::details::ed25519_verify_init(ctx, public_key);
	}

	static inline constexpr bool verify_check(verify_context const &ctx, ::fast_io::containers::index_span<::std::byte const, signature_size> signature, ::fast_io::containers::span<::std::byte const> msg) noexcept
	{
		return ::fast_io::curve25519::details::ed25519_verify_check(ctx, signature, msg);
	}

	static inline constexpr bool verify_signature(::fast_io::containers::index_span<::std::byte const, signature_size> signature, ::fast_io::containers::index_span<::std::byte const, public_key_size> public_key, ::fast_io::containers::span<::std::byte const> msg) noexcept
	{
		return ::fast_io::curve25519::details::ed25519_verify_signature(signature, public_key, msg);
	}

	static inline constexpr blinding_context &blinding_init(blinding_context &ctx, ::fast_io::containers::span<::std::byte const> seed) noexcept
	{
		return ::fast_io::curve25519::details::ed25519_blinding_init(ctx, seed);
	}

	template <typename instmtype>
	static inline blinding_context &blinding_init_from_input_stream(blinding_context &ctx, instmtype &&instm)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	{
		return ::fast_io::curve25519::details::ed25519_blinding_init_from_input_stream(ctx, ::fast_io::freestanding::forward<instmtype>(instm));
	}
};

} // namespace fast_io
