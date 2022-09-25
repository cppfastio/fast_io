#pragma once

namespace fast_io
{

namespace details
{

class ossl_evp_guard
{
public:
	EVP_MD_CTX* pmdctx{};
	ossl_evp_guard():pmdctx{noexcept_call(EVP_MD_CTX_new)}
	{
		if(this->pmdctx==nullptr)
		{
			throw_posix_error(EINVAL);
		}
	}
	ossl_evp_guard(ossl_evp_guard const&)=delete;
	ossl_evp_guard& operator=(ossl_evp_guard const&)=delete;
	EVP_MD_CTX* release() noexcept
	{
		auto temp{pmdctx};
		this->pmdctx=nullptr;
		return temp;
	}
	~ossl_evp_guard()
	{
		if(this->pmdctx)
		{
			noexcept_call(EVP_MD_CTX_free,this->pmdctx);
		}
	}
};

inline EVP_MD_CTX* create_ossl_evp_hash_impl(char const* name)
{
	auto md{noexcept_call(EVP_get_digestbyname,name)};
	if(md==nullptr)
	{
		throw_posix_error(EINVAL);
	}
	ossl_evp_guard guard;
	if(!noexcept_call(EVP_DigestInit_ex,guard.pmdctx,md,nullptr))
	{
		throw_posix_error(EINVAL);
	}
	return guard.release();
}

struct ossl_evp_common
{
	inline EVP_MD_CTX* operator()(char const* name) const
	{
		return create_ossl_evp_hash_impl(name);
	}
};

template<constructible_to_os_c_str T>
inline EVP_MD_CTX* create_ossl_evp_hash_impl(T const& t)
{
	return posix_api_common(t,ossl_evp_common{});
}

}

class ossl_evp_hash_file
{
public:
	using native_handle_type = EVP_MD_CTX*;
	static inline constexpr std::size_t evp_max_md_size{EVP_MAX_MD_SIZE};
	native_handle_type pmdctx{};
	std::size_t evp_size{};
	::std::byte digest_buffer[evp_max_md_size];
	constexpr ossl_evp_hash_file() noexcept = default;
	template<constructible_to_os_c_str T>
	explicit ossl_evp_hash_file(T const& s):pmdctx{::fast_io::details::create_ossl_evp_hash_impl(s)}{}
	ossl_evp_hash_file(ossl_evp_hash_file const&)=delete;
	ossl_evp_hash_file& operator=(ossl_evp_hash_file const&)=delete;
	~ossl_evp_hash_file()
	{
		if(this->pmdctx)
		{
			noexcept_call(EVP_MD_CTX_free,this->pmdctx);
		}
	}
	void update(std::byte const* first,std::byte const* last)
	{
		if(!noexcept_call(EVP_DigestUpdate,this->pmdctx,first,static_cast<std::size_t>(last-first)))
		{
			throw_posix_error(EINVAL);
		}
	}
	void reset()
	{
		noexcept_call(EVP_MD_CTX_reset,this->pmdctx);
	}
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return this->pmdctx;
	}
	void do_final()
	{
		int unsigned u{};
		if(!noexcept_call(EVP_DigestFinal_ex,this->pmdctx,reinterpret_cast<char unsigned*>(digest_buffer),__builtin_addressof(u)))
		{
			throw_posix_error(EINVAL);
		}
		evp_size=static_cast<std::size_t>(u);
	}
	constexpr std::size_t runtime_digest_size() const noexcept
	{
		return evp_size;
	}
	constexpr std::byte const* digest_byte_ptr() const noexcept
	{
		return digest_buffer;
	}
};

}
