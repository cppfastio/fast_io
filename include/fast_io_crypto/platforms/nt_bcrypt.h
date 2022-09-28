#pragma once

namespace fast_io
{

class nt_bcrypt_hash_file;

namespace details
{
inline void create_bcrypt_common_hash_impl(nt_bcrypt_hash_file& g,char16_t const* name);

template<constructible_to_os_c_str T>
inline void create_nt_bcrypt_impl(nt_bcrypt_hash_file& h,T const& t);

inline void ntbcrypt_update_impl(void* hashHandle,::std::byte const* first,::std::byte const* last);

inline void ntbcrypt_do_final_impl(void* hashhandle,::std::byte* buffer,::std::uint_least32_t bcrypt_size);
}

class nt_bcrypt_hash_file
{
public:
	using native_handle_type = void*;
	native_handle_type phAlgorithm{};
	native_handle_type hashHandle{};
	::std::uint_least32_t bcrypt_size{};
	std::byte* digest_buffer{};
	constexpr nt_bcrypt_hash_file() noexcept = default;
	template<constructible_to_os_c_str T>
	explicit nt_bcrypt_hash_file(T const& s)
	{
		::fast_io::details::create_nt_bcrypt_impl(*this,s);
	}
	nt_bcrypt_hash_file(nt_bcrypt_hash_file const&)=delete;
	nt_bcrypt_hash_file& operator=(nt_bcrypt_hash_file const&)=delete;
	~nt_bcrypt_hash_file()
	{
		if(digest_buffer)
		{
			::fast_io::details::deallocate_with_secure_clear<false>(digest_buffer,bcrypt_size);
		}
		if(this->hashHandle)
		{
			::fast_io::win32::BCryptDestroyHash(this->hashHandle);
		}
		if(this->phAlgorithm)
		{
			::fast_io::win32::BCryptCloseAlgorithmProvider(this->phAlgorithm,0u);
		}
	}
	void update(::std::byte const* first,::std::byte const* last)
	{
		::fast_io::details::ntbcrypt_update_impl(this->hashHandle,first,last);
	}
	void do_final()
	{
		::fast_io::details::ntbcrypt_do_final_impl(this->hashHandle,digest_buffer,bcrypt_size);
	}
	void reset()
	{
		this->do_final();
	}
	constexpr std::size_t runtime_digest_size() const noexcept
	{
		return bcrypt_size;
	}
	constexpr std::byte const* digest_byte_ptr() const noexcept
	{
		return digest_buffer;
	}
};

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
	status=::fast_io::win32::BCryptGetProperty(phalgo,u"HashDigestLength",__builtin_addressof(hash_digest_length),
		sizeof(hash_digest_length),__builtin_addressof(result_length),0u);
	if(status)
	{
		throw_nt_error(status);
	}
	::fast_io::details::local_operator_new_array_ptr<::std::byte> locarr(hash_digest_length);
	void* hash_handle{};
	status=::fast_io::win32::BCryptCreateHash(phalgo,__builtin_addressof(hash_handle),nullptr,0u,nullptr,0u,0x00000020);
	if(status)
	{
		throw_nt_error(status);
	}
	g.phAlgorithm=guard.release();
	g.hashHandle=hash_handle;
	g.bcrypt_size=static_cast<::std::uint_least32_t>(hash_digest_length);
	g.digest_buffer=locarr.ptr;
	locarr.ptr=nullptr;
	locarr.size=0u;
}

struct bcrypt_common
{
	nt_bcrypt_hash_file* fl{};
	inline void operator()(char16_t const* name) const
	{
		using char16_t_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char16_t const*;
		create_bcrypt_common_hash_impl(*(this->fl),reinterpret_cast<char16_t_may_alias_const_ptr>(name));
	}
};

template<constructible_to_os_c_str T>
inline void create_nt_bcrypt_impl(nt_bcrypt_hash_file& h,T const& t)
{
	nt_api_common(t,bcrypt_common{__builtin_addressof(h)});
}

inline void ntbcrypt_update_impl(void* hashHandle,::std::byte const* first,::std::byte const* last)
{
	if constexpr(sizeof(::std::ptrdiff_t)<=sizeof(::std::uint_least32_t))
	{
		::std::uint_least32_t ntstatus{::fast_io::win32::BCryptHashData(hashHandle,first,
			static_cast<::std::uint_least32_t>(static_cast<std::size_t>(last-first)),0u)};
		if(ntstatus)
		{
			throw_nt_error(ntstatus);
		}
	}
	else
	{
		constexpr std::size_t ul32_max{static_cast<std::size_t>(std::numeric_limits<std::uint_least32_t>::max())};
		for(std::size_t sz{static_cast<std::size_t>(last-first)};sz;)
		{
			std::size_t mn{sz};
			if(ul32_max<sz)
			{
				mn=ul32_max;
			}
			::std::uint_least32_t ntstatus{::fast_io::win32::BCryptHashData(hashHandle,first,
				static_cast<::std::uint_least32_t>(mn),0u)};
			if(ntstatus)
			{
				throw_nt_error(ntstatus);
			}
			sz-=mn;
			first+=mn;
		}
	}
}

inline void ntbcrypt_do_final_impl(void* hashhandle,::std::byte* buffer,::std::uint_least32_t bcrypt_size)
{
	::std::uint_least32_t ntstatus{::fast_io::win32::BCryptFinishHash(hashhandle,buffer,bcrypt_size,0u)};
	if(ntstatus)
	{
		throw_nt_error(ntstatus);
	}
}

}

}
