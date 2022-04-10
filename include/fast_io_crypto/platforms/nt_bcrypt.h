#pragma once

namespace fast_io
{

namespace details
{

inline void* create_bcrypt_common_hash_impl(char16_t const* name)
{
	void* phalgo{};
	::std::uint_least32_t status{::fast_io::win32::BCryptOpenAlgorithmProvider(__builtin_addressof(phalgo),name,nullptr,0u)};
	if(!status)
	{
		throw_nt_error(status);
	}
	return phalgo;
}

struct bcrypt_common
{
	inline void* operator()(char16_t const* name) const
	{
		return create_bcrypt_common_hash_impl(name);
	}
};

template<constructible_to_os_c_str T>
inline void* create_nt_bcrypt_impl(T const& t)
{
	return nt_api_common(t,bcrypt_common{});
}

}

class nt_bcrypt_hash_file
{
public:
	using native_handle_type = void*;
	native_handle_type phAlgorithm{};
	std::byte buffer[];
	template<constructible_to_os_c_str T>
	explicit nt_bcrypt_hash_file(T const& s):phAlgorithm{::fast_io::details::create_nt_bcrypt_impl(s)}{}
	nt_bcrypt_hash_file(nt_bcrypt_hash_file const&)=delete;
	nt_bcrypt_hash_file& operator=(nt_bcrypt_hash_file const&)=delete;
	~nt_bcrypt_hash_file()
	{
		::fast_io::win32::BCryptCloseAlgorithmProvider(this->phAlgorithm,0u);
	}
};

}