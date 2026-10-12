#pragma once

/*
HMAC over a basic_md5_sha_context_impl-style context (rfc2104).

key longer than block_size is hashed first; the key block is then XORed
with ipad (0x36) / opad (0x5c) around the message hash.
*/

namespace fast_io
{

template <typename hash_context>
class hmac_context
{
public:
	static inline constexpr ::std::size_t digest_size{hash_context::digest_size};
	static inline constexpr ::std::size_t block_size{hash_context::block_size};

private:
	hash_context inner_{};
	::std::byte outer_pad_[block_size]{};

public:
	inline constexpr hmac_context() noexcept = default;

	inline constexpr hmac_context(::std::byte const *key, ::std::size_t key_size) noexcept
	{
		this->init(key, key_size);
	}

	inline constexpr void init(::std::byte const *key, ::std::size_t key_size) noexcept
	{
		::std::byte k[block_size]{};
		if (block_size < key_size)
		{
			hash_context h{};
			h.update(key, key + key_size);
			h.do_final();
			h.digest_to_byte_ptr(k);
		}
		else
		{
			::fast_io::details::non_overlapped_copy_n(key, key_size, k);
		}
		::std::byte ipad[block_size];
		for (::std::size_t i{}; i != block_size; ++i)
		{
			ipad[i] = k[i] ^ ::std::byte{0x36};
			outer_pad_[i] = k[i] ^ ::std::byte{0x5c};
		}
		inner_.reset();
		inner_.update(ipad, ipad + block_size);
	}

	inline constexpr void update(::std::byte const *first, ::std::byte const *last) noexcept
	{
		inner_.update(first, last);
	}

	inline constexpr void do_final() noexcept
	{
		inner_.do_final();
	}

	inline constexpr void digest_to_byte_ptr(::std::byte *ptr) noexcept
	{
		::std::byte inner_digest[digest_size];
		inner_.digest_to_byte_ptr(inner_digest);
		hash_context outer{};
		outer.update(outer_pad_, outer_pad_ + block_size);
		outer.update(inner_digest, inner_digest + digest_size);
		outer.do_final();
		outer.digest_to_byte_ptr(ptr);
	}

	/* one-shot HMAC(key, msg_parts...) */
	inline constexpr void digest_to_byte_ptr(::std::byte *ptr, ::std::byte const *msg, ::std::size_t msg_size) noexcept
	{
		this->update(msg, msg + msg_size);
		this->do_final();
		this->digest_to_byte_ptr(ptr);
	}
};

template <typename hash_context>
inline constexpr void hmac_once_to_ptr(::std::byte *digest,
									   ::std::byte const *key, ::std::size_t key_size,
									   ::std::byte const *msg, ::std::size_t msg_size) noexcept
{
	hmac_context<hash_context> ctx{key, key_size};
	ctx.digest_to_byte_ptr(digest, msg, msg_size);
}

} // namespace fast_io
