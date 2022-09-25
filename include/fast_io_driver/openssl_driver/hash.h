#pragma once

namespace fast_io::ossl
{

template<typename metainfotype>
struct basic_ossl_hash_context
{
	using meta_info_type = metainfotype;
	using native_handle_type = typename meta_info_type::ctx_type;
	native_handle_type handle;
	static inline constexpr std::size_t digest_size{meta_info_type::digest_length};
	std::byte digest_buffer[digest_size];
	explicit basic_ossl_hash_context() noexcept
	{
		this->reset();
	}
//let contract violation dies. Programming bug should just die. C++ EH is a historical mistake.
	void reset() noexcept
	{
		if(meta_info_type::init_function(__builtin_addressof(handle))==0)[[unlikely]]
			::fast_io::fast_terminate();
	}
	constexpr void digest_to_byte_ptr(std::byte* dest) const noexcept
	{
		::fast_io::freestanding::nonoverlapped_bytes_copy_n(digest_buffer,digest_size,dest);
	}
	void update(std::byte const* first, std::byte const* last) noexcept
	{
		if(meta_info_type::update_function(__builtin_addressof(handle),first,static_cast<std::size_t>(last-first))==0)[[unlikely]]
			::fast_io::fast_terminate();
	}
	void do_final() noexcept
	{
		if(meta_info_type::final_function(reinterpret_cast<char unsigned*>(digest_buffer),__builtin_addressof(handle))==0)[[unlikely]]
			::fast_io::fast_terminate();
	}
};

struct md5_meta_info
{
	using ctx_type = MD5_CTX;
	static inline constexpr std::size_t digest_length{MD5_DIGEST_LENGTH};
	static inline int init_function(ctx_type* c) noexcept
	{
		return ::fast_io::noexcept_call(MD5_Init,c);
	}
	static inline int update_function(ctx_type* c,void const* data,std::size_t len) noexcept
	{
		return ::fast_io::noexcept_call(MD5_Update,c,data,len);
	}
	static inline int final_function(char unsigned* md,ctx_type* c) noexcept
	{
		return ::fast_io::noexcept_call(MD5_Final,md,c);
	}
};

using md5_context
#ifndef FAST_IO_NO_WARNING_DEPRECATED_CRYPTO_ALGOS
[[deprecated("The weaknesses of MD5 have been exploited in the field, most infamously by the Flame malware in 2012. See wikipedia https://en.wikipedia.org/wiki/MD5")]]
#endif
= basic_ossl_hash_context<md5_meta_info>;

struct sha1_meta_info
{
	using ctx_type = SHA_CTX;
	static inline constexpr std::size_t digest_length{SHA_DIGEST_LENGTH};
	static inline int init_function(ctx_type* c) noexcept
	{
		return ::fast_io::noexcept_call(SHA1_Init,c);
	}
	static inline int update_function(ctx_type* c,void const* data,std::size_t len) noexcept
	{
		return ::fast_io::noexcept_call(SHA1_Update,c,data,len);
	}
	static inline int final_function(char unsigned* md,ctx_type* c) noexcept
	{
		return ::fast_io::noexcept_call(SHA1_Final,md,c);
	}
};

using sha1_context
#ifndef FAST_IO_NO_WARNING_DEPRECATED_CRYPTO_ALGOS
[[deprecated("SHA1 is no longer a secure algorithm. See wikipedia https://en.wikipedia.org/wiki/SHA-1")]]
#endif
= basic_ossl_hash_context<sha1_meta_info>;

struct sha224_meta_info
{
	using ctx_type = SHA256_CTX;
	static inline constexpr std::size_t digest_length{SHA224_DIGEST_LENGTH};
	static inline int init_function(ctx_type* c) noexcept
	{
		return ::fast_io::noexcept_call(SHA224_Init,c);
	}
	static inline int update_function(ctx_type* c,void const* data,std::size_t len) noexcept
	{
		return ::fast_io::noexcept_call(SHA224_Update,c,data,len);
	}
	static inline int final_function(char unsigned* md,ctx_type* c) noexcept
	{
		return ::fast_io::noexcept_call(SHA224_Final,md,c);
	}
};

using sha224_context = basic_ossl_hash_context<sha224_meta_info>;

struct sha256_meta_info
{
	using ctx_type = SHA256_CTX;
	static inline constexpr std::size_t digest_length{SHA256_DIGEST_LENGTH};
	static inline int init_function(ctx_type* c) noexcept
	{
		return ::fast_io::noexcept_call(SHA256_Init,c);
	}
	static inline int update_function(ctx_type* c,void const* data,std::size_t len) noexcept
	{
		return ::fast_io::noexcept_call(SHA256_Update,c,data,len);
	}
	static inline int final_function(char unsigned* md,ctx_type* c) noexcept
	{
		return ::fast_io::noexcept_call(SHA256_Final,md,c);
	}
};

using sha256_context = basic_ossl_hash_context<sha256_meta_info>;

struct sha384_meta_info
{
	using ctx_type = SHA512_CTX;
	static inline constexpr std::size_t digest_length{SHA384_DIGEST_LENGTH};
	static inline int init_function(ctx_type* c) noexcept
	{
		return ::fast_io::noexcept_call(SHA384_Init,c);
	}
	static inline int update_function(ctx_type* c,void const* data,std::size_t len) noexcept
	{
		return ::fast_io::noexcept_call(SHA384_Update,c,data,len);
	}
	static inline int final_function(char unsigned* md,ctx_type* c) noexcept
	{
		return ::fast_io::noexcept_call(SHA384_Final,md,c);
	}
};

using sha384_context = basic_ossl_hash_context<sha384_meta_info>;

struct sha512_meta_info
{
	using ctx_type = SHA512_CTX;
	static inline constexpr std::size_t digest_length{SHA512_DIGEST_LENGTH};
	static inline int init_function(ctx_type* c) noexcept
	{
		return ::fast_io::noexcept_call(SHA512_Init,c);
	}
	static inline int update_function(ctx_type* c,void const* data,std::size_t len) noexcept
	{
		return ::fast_io::noexcept_call(SHA512_Update,c,data,len);
	}
	static inline int final_function(char unsigned* md,ctx_type* c) noexcept
	{
		return ::fast_io::noexcept_call(SHA512_Final,md,c);
	}
};

using sha512_context = basic_ossl_hash_context<sha512_meta_info>;

}
