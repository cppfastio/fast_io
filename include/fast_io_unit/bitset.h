#pragma once

namespace fast_io
{

struct bitset_print_reserve_proxy
{
	using manip_tag = manip_tag_t;
	
	using native_storage_type =
#if defined(_LIBCPP_VERSION)
	::std::size_t
#else
	unsigned long
#endif
;
	using native_storage_const_pointer
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= native_storage_type const*;
	using reference_type = native_storage_const_pointer;
	native_storage_const_pointer reference{};
	::std::size_t bits{};
};

template<::std::integral char_type>
inline constexpr
	::std::size_t print_reserve_size(io_reserve_type_t<char_type,bitset_print_reserve_proxy>,bitset_print_reserve_proxy p) noexcept
{
	return p.bits;
}

namespace details
{

template<::std::integral char_type>
inline constexpr char_type* pr_vs_iter_impl(char_type* first,::std::size_t bits,
	::std::unsigned_integral auto p) noexcept
{
	auto last{first+bits};
	for(auto i{last};i!=first;)
	{
		*(--i)=::fast_io::char_literal_add<char_type>(p&0x1u);
		p>>=1;
	}
	return last;
}

template<::std::integral char_type>
inline constexpr char_type* prv_rsv_bitset_impl(char_type* iter,typename bitset_print_reserve_proxy::native_storage_const_pointer ptr,::std::size_t bits) noexcept
{
	using nr_type = typename bitset_print_reserve_proxy::native_storage_type;
	constexpr
		::std::size_t bits_per_rep{::std::numeric_limits<nr_type>::digits};
	::std::size_t const counts{bits/bits_per_rep};
	::std::size_t const remain_bits{bits%bits_per_rep};
	auto ptr_end{ptr+counts};
	if(remain_bits)
	{
		iter=pr_vs_iter_impl(iter,remain_bits,*ptr_end);
	}
	for(;ptr!=ptr_end;iter=pr_vs_iter_impl(iter,bits_per_rep,*(--ptr_end)));
	return iter;
}

}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,bitset_print_reserve_proxy>,char_type* iter,bitset_print_reserve_proxy p) noexcept
{
	return ::fast_io::details::prv_rsv_bitset_impl(iter,p.reference,p.bits);
}

template<
	::std::size_t n>
inline bitset_print_reserve_proxy print_alias_define(io_alias_t,
	::std::bitset<n> const& bts) noexcept
{
	return ::fast_io::bitset_print_reserve_proxy{reinterpret_cast<::fast_io::bitset_print_reserve_proxy::native_storage_const_pointer>(__builtin_addressof(bts)),n};
}

}
