#pragma once

namespace fast_io
{

/*
https://www.openssl.org/docs/man1.1.0/man3/EVP_PKEY_keygen.html
 #include <openssl/evp.h>

 int EVP_PKEY_keygen_init(EVP_PKEY_CTX *ctx);
 int EVP_PKEY_keygen(EVP_PKEY_CTX *ctx, EVP_PKEY **ppkey);
 int EVP_PKEY_paramgen_init(EVP_PKEY_CTX *ctx);
 int EVP_PKEY_paramgen(EVP_PKEY_CTX *ctx, EVP_PKEY **ppkey);

 typedef int EVP_PKEY_gen_cb(EVP_PKEY_CTX *ctx);

 void EVP_PKEY_CTX_set_cb(EVP_PKEY_CTX *ctx, EVP_PKEY_gen_cb *cb);
 EVP_PKEY_gen_cb *EVP_PKEY_CTX_get_cb(EVP_PKEY_CTX *ctx);

 int EVP_PKEY_CTX_get_keygen_info(EVP_PKEY_CTX *ctx, int idx);

 void EVP_PKEY_CTX_set_app_data(EVP_PKEY_CTX *ctx, void *data);
 void *EVP_PKEY_CTX_get_app_data(EVP_PKEY_CTX *ctx);

 
*/

class evp_pkey_observer
{
public:
	using native_handle_type = EVP_PKEY*;
	native_handle_type key{};
	constexpr auto& native_handle() const noexcept
	{
		return key;
	}
	constexpr auto& native_handle() noexcept
	{
		return key;
	}
	constexpr operator bool() noexcept
	{
		return key;
	}
	constexpr auto release() noexcept
	{
		auto temp{key};
		key=nullptr;
		return temp;
	}
	std::size_t use_count() const noexcept
	{
		return EVP_PKEY_up_ref(key);
	}
};

class evp_pkey:public evp_pkey_observer
{
public:
	using native_handle_type = EVP_PKEY*;
	constexpr evp_pkey()=default;
	constexpr evp_pkey(native_handle_type ctx):evp_pkey_observer{ctx}{}
	evp_pkey(native_interface_t):evp_pkey_observer{EVP_PKEY_new()}
	{
		if(this->native_handle()==nullptr)[[unlikely]]
			throw_openssl_error();
	}
	evp_pkey(evp_pkey const&)=delete;
	evp_pkey& operator=(evp_pkey const&)=delete;
	constexpr evp_pkey(evp_pkey&& __restrict other) noexcept: evp_pkey_observer{other.native_handle()}
	{
		other.native_handle()=nullptr;
	}
	evp_pkey& operator=(evp_pkey&& __restrict other) noexcept
	{
		if(this->native_handle())[[likely]]
			EVP_PKEY_free(this->native_handle());
		this->native_handle()=other.native_handle();
		other.native_handle()=nullptr;
		return *this;
	}
	inline constexpr void reset(native_handle_type newhandle=nullptr) noexcept
	{
		if(this->native_handle())[[likely]]
			EVP_PKEY_free(this->native_handle());
		this->native_handle()=newhandle;
	}

	~evp_pkey()
	{
		if(this->native_handle())[[likely]]
			EVP_PKEY_free(this->native_handle());
	}
};


class evp_pkey_ctx_observer
{
public:
	using native_handle_type = EVP_PKEY_CTX*;
	native_handle_type ctx{};
	constexpr auto& native_handle() const noexcept
	{
		return ctx;
	}
	constexpr auto& native_handle() noexcept
	{
		return ctx;
	}
	constexpr operator bool() noexcept
	{
		return ctx;
	}
	constexpr auto release() noexcept
	{
		auto temp{ctx};
		ctx=nullptr;
		return temp;
	}
};

class evp_pkey_ctx:public evp_pkey_ctx_observer
{
public:
	using native_handle_type = EVP_PKEY_CTX*;
	constexpr evp_pkey_ctx()=default;
	constexpr evp_pkey_ctx(native_handle_type ctx):evp_pkey_ctx_observer{ctx}{}
	evp_pkey_ctx(native_interface_t,EVP_PKEY *pkey, ENGINE *e):evp_pkey_ctx_observer{EVP_PKEY_CTX_new(pkey,e)}
	{
		if(this->native_handle()==nullptr)[[unlikely]]
			throw_openssl_error();
	}
	evp_pkey_ctx(native_interface_t,int id, ENGINE *e):evp_pkey_ctx_observer{EVP_PKEY_CTX_new_id(id,e)}
	{
		if(this->native_handle()==nullptr)[[unlikely]]
			throw_openssl_error();
	}
	evp_pkey_ctx(evp_pkey_ctx const& other):evp_pkey_ctx_observer{EVP_PKEY_CTX_dup(other.native_handle())}
	{
		if(this->native_handle()==nullptr)[[unlikely]]
			throw_openssl_error();
	}
	evp_pkey_ctx& operator=(evp_pkey_ctx const& other)
	{
		auto newp{EVP_PKEY_CTX_dup(other.native_handle())};
		if(newp==nullptr)[[unlikely]]
			throw_openssl_error();
		if(this->native_handle())[[likely]]
			EVP_PKEY_CTX_free(this->native_handle());
		this->native_handle()=newp;
		return *this;
	}
	constexpr evp_pkey_ctx(evp_pkey_ctx&& __restrict other) noexcept: evp_pkey_ctx_observer{other.native_handle()}
	{
		other.native_handle()=nullptr;
	}
	evp_pkey_ctx& operator=(evp_pkey_ctx&& __restrict other) noexcept
	{
		if(this->native_handle())[[likely]]
			EVP_PKEY_CTX_free(this->native_handle());
		this->native_handle()=other.native_handle();
		other.native_handle()=nullptr;
		return *this;
	}
	void reset(native_handle_type newctx=nullptr) noexcept
	{
		if(this->native_handle())[[likely]]
			EVP_PKEY_CTX_free(this->native_handle());
		this->native_handle()=newctx;
	}
	~evp_pkey_ctx()
	{
		if(this->native_handle())[[likely]]
			EVP_PKEY_CTX_free(this->native_handle());
	}
};


inline void keygen_init(evp_pkey_ctx_observer evob)
{
	if(EVP_PKEY_keygen_init(evob.ctx)<=0)
		throw_openssl_error();
}

inline evp_pkey keygen(evp_pkey_ctx_observer evob)
{
	EVP_PKEY* add{};
	if(EVP_PKEY_keygen(evob.ctx,__builtin_addressof(add))<=0)
		throw_openssl_error();
	return evp_pkey(add);
}

struct private_key
{
	evp_pkey_observer x{};
	EVP_CIPHER const * enc{};
	unsigned char *kstr{};
	int klen{};
	pem_password_cb *cb{};
	void *u{};
};

struct public_key
{
	evp_pkey_observer x{};
};

/*
https://linux.die.net/man/3/pem_read_rsa_pubkey
*/


template<output_stream output>
requires (std::same_as<typename std::remove_cvref_t<output>::char_type,char>&&(std::derived_from<std::remove_cvref_t<output>,bio_io_observer>||buffer_output_stream<std::remove_cvref_t<output>>))
inline void print_define(io_reserve_type_t<typename output::char_type,private_key>,output out,private_key const& key)
{
	if constexpr(std::derived_from<std::remove_cvref_t<output>,bio_io_observer>)
	{
		if(!noexcept_call(PEM_write_bio_PrivateKey,out.bio,key.x.key,key.enc,key.kstr,key.klen,key.cb,key.u))
			throw_openssl_error();
	}
	else
	{
		bio_file bf(io_cookie_type<output>,out);
		print_freestanding(bf,key);
	}	
}

template<output_stream output>
requires (std::same_as<typename std::remove_cvref_t<output>::char_type,char>&&(std::derived_from<std::remove_cvref_t<output>,bio_io_observer>||buffer_output_stream<std::remove_cvref_t<output>>))
inline void print_define(io_reserve_type_t<typename output::char_type,public_key>,output out,public_key key)
{
	if constexpr(std::derived_from<std::remove_cvref_t<output>,bio_io_observer>)
	{
		if(!noexcept_call(PEM_write_bio_PUBKEY,out.bio,key.x.key))
			throw_openssl_error();
	}
	else
	{
		bio_file bf(io_cookie,out);
		print_freestanding(bf,key);
	}
}


}
