#pragma once

namespace fast_io
{

template<std::integral ch_type>
class basic_win32_crypt_gen_random_io_observer
{
public:
	using native_handle_type = std::uintptr_t;
	using char_type = ch_type;
	native_handle_type hprov{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return hprov;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto tmp{hprov};
		hprov=0;
		return tmp;
	}
};

template<std::integral char_type>
inline constexpr basic_win32_crypt_gen_random_io_observer<char_type> io_value_handle(basic_win32_crypt_gen_random_io_observer<char_type> giob) noexcept
{
	return giob;
}

template<std::integral char_type>
inline constexpr void require_secure_clear(basic_win32_crypt_gen_random_io_observer<char_type>) noexcept{}

namespace win32::details
{

inline void win32_crypt_gen_random_read_u32(std::uintptr_t hprov,void* ptr,std::size_t sz)
{
	if(!::fast_io::win32::CryptGenRandom(hprov,static_cast<std::uint_least32_t>(sz),reinterpret_cast<char unsigned*>(ptr)))
	{
		throw_win32_error();
	}
}

inline std::size_t win32_crypt_gen_random_read(std::uintptr_t hprov,void* ptr,std::size_t sz)
{
	std::byte *base_ptr{reinterpret_cast<std::byte*>(ptr)};
	std::byte *iter{base_ptr};
	while(sz)
	{
		constexpr std::size_t ul32_max{static_cast<std::size_t>(UINT_LEAST32_MAX)};
		std::size_t mn{sz};
		if(ul32_max<sz)
			mn=ul32_max;
		if(!::fast_io::win32::CryptGenRandom(hprov,static_cast<std::uint_least32_t>(mn),reinterpret_cast<char unsigned*>(iter)))
		{
			if(base_ptr==iter)
				throw_win32_error();
			break;
		}
		sz-=mn;
		iter+=mn;
	}
	return static_cast<std::size_t>(iter-base_ptr);
}

}

template<std::integral char_type,::std::contiguous_iterator Iter>
inline Iter read(basic_win32_crypt_gen_random_io_observer<char_type> hp,Iter bg,Iter ed)
{
	if constexpr(sizeof(std::uint_least32_t)<sizeof(std::size_t))
	{
		auto ret{win32::details::win32_crypt_gen_random_read(hp.hprov,::std::to_address(bg),static_cast<std::size_t>(ed-bg)*sizeof(*bg))};
		return bg+(ret/sizeof(*bg));
	}
	else
	{
		win32::details::win32_crypt_gen_random_read_u32(hp.hprov,::std::to_address(bg),static_cast<std::size_t>(ed-bg)*sizeof(*bg));
		return ed;
	}
}

template<win32_family family,std::integral ch_type>
class basic_win32_family_crypt_gen_random_file:public basic_win32_crypt_gen_random_io_observer<ch_type>
{
public:
	using native_handle_type = std::uintptr_t;
	using char_type = ch_type;
	basic_win32_family_crypt_gen_random_file():basic_win32_crypt_gen_random_io_observer<ch_type>{::fast_io::details::win32::crypt_acquire_context_fallback<family>()}{}
	constexpr basic_win32_family_crypt_gen_random_file(::std::nullptr_t):basic_win32_crypt_gen_random_io_observer<ch_type>{}
	{}
	basic_win32_family_crypt_gen_random_file(basic_win32_family_crypt_gen_random_file const&)=delete;
	basic_win32_family_crypt_gen_random_file& operator=(basic_win32_family_crypt_gen_random_file const&)=delete;
	constexpr basic_win32_family_crypt_gen_random_file(basic_win32_family_crypt_gen_random_file&& __restrict other) noexcept:
		basic_win32_crypt_gen_random_io_observer<ch_type>{other.hprov}
	{
		other.hprov=0;
	}
	basic_win32_family_crypt_gen_random_file& operator=(basic_win32_family_crypt_gen_random_file&& __restrict other) noexcept
	{
		if(this->hprov)[[likely]]
		{
			::fast_io::win32::CryptReleaseContext(this->hprov,0);
		}
		this->hprov=other.hprov;
		other.hprov=0;
		return *this;
	}

	~basic_win32_family_crypt_gen_random_file()
	{
		if(!this->hprov)[[likely]]
		{
			::fast_io::win32::CryptReleaseContext(this->hprov,0);
		}
	}
	void close()
	{
		if(this->hprov)[[likely]]
		{
			if(!::fast_io::win32::CryptReleaseContext(this->hprov,0))
				throw_win32_error();
		}
	}
};

template<std::integral char_type>
using basic_win32_crypt_gen_random_file_9xa = basic_win32_family_crypt_gen_random_file<win32_family::ansi_9x,char_type>;
template<std::integral char_type>
using basic_win32_crypt_gen_random_file_ntw = basic_win32_family_crypt_gen_random_file<win32_family::wide_nt,char_type>;
template<std::integral char_type>
using basic_win32_crypt_gen_random_file = basic_win32_family_crypt_gen_random_file<win32_family::native,char_type>;

}
