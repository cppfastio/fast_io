#pragma once

namespace fast_io
{
class nt_bcrypt_hash_file;
namespace details
{

struct bcrypt_algo_guard
{
	void* phalgo{};
	explicit constexpr bcrypt_algo_guard() noexcept = default;
	bcrypt_algo_guard(bcrypt_algo_guard const&)=delete;
	bcrypt_algo_guard& operator=(bcrypt_algo_guard const&)=delete;
	~bcrypt_algo_guard()
	{
		if(this->phalgo)
		{
			::fast_io::win32::BCryptCloseAlgorithmProvider(this->phalgo,0u);
		}
	}
	void* release() noexcept
	{
		auto temp{this->phalgo};
		this->phalgo=nullptr;
		return temp;
	}
};

inline void create_bcrypt_common_hash_impl(nt_bcrypt_hash_file& g,char16_t const* name)
{
	void* phalgo{};
	::std::uint_least32_t status{::fast_io::win32::BCryptOpenAlgorithmProvider(__builtin_addressof(phalgo),name,nullptr,0u)};
	if(status)
	{
		throw_nt_error(status);
	}
	bcrypt_algo_guard guard;
	guard.phalgo=phalgo;
	::std::uint_least32_t hash_digest_length{},result_length{};
	status=::fast_io::win32::BCryptGetProperty(phalgo,u"HashDigestLength",__builtin_addressof(hash_digest_length),sizeof(hash_digest_length),__builtin_addressof(result_length),0u);
	if(status)
	{
		throw_nt_error(status);
	}
	::fast_io::details::local_operator_new_array_ptr<std::byte> locarr(hash_digest_length);
	void* hash_handle{};
	status=::fast_io::win32::BCryptCreateHash(phalgo,__builtin_addressof(hash_handle),nullptr,0u,nullptr,0u,0u);
	if(status)
	{
		throw_nt_error(status);
	}	
	g.phAlgorithm=guard.release();
	g.hashHandle=hash_handle;
	g.bcrypt_size=static_cast<::std::size_t>(hash_digest_length);
	g.buffer=locarr.ptr;
	locarr.ptr=nullptr;
	locarr.size=0u;
}

struct bcrypt_common
{
	nt_bcrypt_hash_file* fl{};
	inline void* operator()(char16_t const* name) const
	{
		return create_bcrypt_common_hash_impl(*this->fl,name);
	}
};

template<constructible_to_os_c_str T>
inline void create_nt_bcrypt_impl(nt_bcrypt_hash_file& h,T const& t)
{
	return nt_api_common(t,bcrypt_common{__builtin_addressof(h)});
}

}

class nt_bcrypt_hash_file
{
public:
	using native_handle_type = void*;
	native_handle_type phAlgorithm{};
	native_handle_type hashHandle{};
	std::size_t bcrypt_size{};
	std::byte* buffer{};
	template<constructible_to_os_c_str T>
	explicit nt_bcrypt_hash_file(T const& s):phAlgorithm{::fast_io::details::create_nt_bcrypt_impl(s)}{}
	nt_bcrypt_hash_file(nt_bcrypt_hash_file const&)=delete;
	nt_bcrypt_hash_file& operator=(nt_bcrypt_hash_file const&)=delete;
	~nt_bcrypt_hash_file()
	{
		if(this->phAlgorithm)
		{
			::fast_io::win32::BCryptCloseAlgorithmProvider(this->phAlgorithm,0u);
		}
	}
};

}