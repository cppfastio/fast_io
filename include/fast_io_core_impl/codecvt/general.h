#pragma once

namespace fast_io
{

namespace details::codecvt
{
template<encoding_scheme encoding,typename T>
inline constexpr auto general_advance(T* src_first,std::size_t sz) noexcept
{
	if constexpr(encoding_scheme::utf_ebcdic==encoding)
		return utf_ebcdic_advance(src_first,sz);
	else
		return gb18030::gb18030_advance(src_first,sz);
}

template<encoding_scheme encoding,typename T>
inline constexpr std::size_t get_general_invalid_code_units(T* dst) noexcept
{
	if constexpr(encoding==encoding_scheme::utf_ebcdic)
		return get_utf_ebcdic_invalid_code_units(dst);
	else if constexpr(encoding==encoding_scheme::gb18030)
		return gb18030::get_gb18030_invalid_code_units(dst);
	else
	{
		if constexpr(sizeof(T)>=2)
		{
			if constexpr(encoding_is_utf(encoding)&&!is_native_scheme(encoding))
			{
				constexpr T val{byte_swap(static_cast<T>(0xFFFD))};
				*dst=val;
			}
			else
				*dst=static_cast<T>(0xFFFD);
			return 1;
		}
		else
			return get_utf8_invalid_code_units(dst);
	}
}

template<std::integral char_type>
inline constexpr encoding_scheme get_execution_charset_encoding_scheme(encoding_scheme scheme) noexcept
{
	if(scheme!=encoding_scheme::execution_charset)
		return scheme;
	return execution_charset_encoding_scheme<char_type>();
}

template<
encoding_scheme src_encoding=encoding_scheme::execution_charset,
encoding_scheme encoding=encoding_scheme::execution_charset,
std::integral src_char_type,std::integral dest_char_type>
requires (sizeof(src_char_type)<=4 &&sizeof(dest_char_type)<=4)
inline constexpr code_cvt_result<src_char_type,dest_char_type> general_code_cvt(src_char_type const* src_first,src_char_type const* src_last,dest_char_type* __restrict dst) noexcept
{
	if constexpr(src_encoding==encoding_scheme::execution_charset)
	{
		return general_code_cvt<get_execution_charset_encoding_scheme<src_char_type>(src_encoding),
			encoding>(src_first,src_last,dst);
	}
	else if constexpr(encoding==encoding_scheme::execution_charset)
	{
		return general_code_cvt<src_encoding,
			get_execution_charset_encoding_scheme<dest_char_type>(encoding)>(src_first,src_last,dst);
	}
	else if constexpr(sizeof(src_char_type)==sizeof(dest_char_type)&&src_encoding==encoding)
	{
		std::size_t diff{static_cast<std::size_t>(src_last-src_first)};
		non_overlapped_copy_n(src_first,diff,dst);
		return {src_last,dst+diff};
	}
	else if constexpr(sizeof(src_char_type)==sizeof(dest_char_type)&&
		((src_encoding==encoding_scheme::utf_le&&encoding==encoding_scheme::utf_be)||
		(src_encoding==encoding_scheme::utf_be&&encoding==encoding_scheme::utf_le)))
	{
		if constexpr(sizeof(src_char_type)==1)
		{
			return general_code_cvt<encoding_scheme::utf,encoding_scheme::utf>(src_first,src_last,dst);
		}
		else
		{
			for(;src_first!=src_last;++src_first)
				*dst=byte_swap(*src_first);
			return {src_first,dst};
		}
	}
	else if constexpr(sizeof(src_char_type)==4)
	{
		static_assert(src_encoding==encoding_scheme::utf_be||src_encoding==encoding_scheme::utf_le);
		for(;src_first!=src_last;++src_first)
			dst+=get_utf_code_units<encoding>(static_cast<char32_t>(*src_first),dst);
		return {src_last,dst};
	}
	else if constexpr(sizeof(src_char_type)==2)
	{
		static_assert(src_encoding==encoding_scheme::utf_be||src_encoding==encoding_scheme::utf_le);
/*
Referenced from
https://stackoverflow.com/questions/23919515/how-to-convert-from-utf-16-to-utf-32-on-linux-with-std-library
*/
		for(;src_first!=src_last;++src_first)
		{
			char16_t code{static_cast<char16_t>(*src_first)};
			if constexpr(!is_native_scheme(src_encoding))
				code=byte_swap(code);
			if(is_utf16_surrogate(code))[[unlikely]]
			{
				if(is_utf16_high_surrogate(code))
				{
					if(src_first+1==src_last)
					{
						break;
					}
					char16_t code1{static_cast<char16_t>(*++src_first)};
					if constexpr(!is_native_scheme(src_encoding))
						code1=byte_swap(code1);
					if(is_utf16_low_surrogate(code1))
					{
						if constexpr(sizeof(dest_char_type)==4)
						{
							*dst = utf16_surrogate_to_utf32(code,code1);
							++dst;
						}
						else
							dst+=get_utf_code_units<encoding>(utf16_surrogate_to_utf32(code,code1),dst);
						continue;
					}
				}
				if constexpr(sizeof(dest_char_type)==4)
				{
					if constexpr(is_native_scheme(encoding))
						*dst=static_cast<dest_char_type>(0xFFFD);
					else
						*dst=static_cast<dest_char_type>(0xFDFF0000);
					++dst;
				}
				else
				{
					dst+=get_general_invalid_code_units<encoding>(dst);
				}
			}
			else[[likely]]
			{
				if constexpr(sizeof(dest_char_type)==4)
				{
					if constexpr(src_encoding==encoding)
						*dst=static_cast<dest_char_type>(code);
					else
						*dst=byte_swap(static_cast<dest_char_type>(code));
					++dst;
				}
				else
					dst+=get_utf_code_units<encoding>(code,dst);
			}
		}
		return {src_first,dst};
	}
	else
	{
#if (defined(_MSC_VER)&&defined(_M_AMD64)&&!defined(__clang__)) || (defined(__SSE__) && defined(__x86_64__) && __cpp_lib_is_constant_evaluated>=201811L)
		if constexpr(src_encoding!=encoding_scheme::utf_ebcdic&&encoding!=encoding_scheme::utf_ebcdic&&1==sizeof(src_char_type)
		&&(1==sizeof(dest_char_type)||encoding_is_utf(encoding)))
		{
		if (!std::is_constant_evaluated())
		{
		constexpr std::size_t m128i_size{16};
		while(m128i_size < static_cast<std::size_t>(src_last-src_first))
		{
			if (static_cast<char8_t>(*src_first) < 0x80)
			{
				auto [new_src,new_dst]= convert_ascii_with_sse(src_first, dst);
				src_first=new_src;
				dst=new_dst;
			}
			else
			{
				if constexpr(src_encoding==encoding_scheme::gb18030)
				{
					auto [code,adv] = gb18030::gb18030_advance_unchecked(src_first);
					src_first+=adv;
					if constexpr(sizeof(dest_char_type)==4)
					{
						*dst=code;
						++dst;
					}
					else
						dst+=get_utf_code_units<encoding>(code,dst);
				}
				else
				{
					auto [src,code] = advance_with_big_table_unchecked(src_first);
					src_first=src;
					if constexpr(sizeof(dest_char_type)==4)
					{
						*dst=code;
						++dst;
					}
					else
						dst+=get_utf_code_units<encoding>(code,dst);
				}
			}
		}
		}
		}
#endif

		for(;src_first!=src_last;)
			if constexpr(src_encoding==encoding_scheme::utf_ebcdic)
			{
				auto [code,adv]=general_advance<src_encoding>(src_first,src_last-src_first);
				if(adv==static_cast<char8_t>(-1))
					break;
				src_first+=adv;
				if constexpr(sizeof(dest_char_type)==4)
				{
					*dst=code;
					if constexpr(encoding_is_utf(encoding)&&!is_native_scheme(encoding))
						*dst=byte_swap(*dst);
					++dst;
				}
				else
				{
					dst+=get_utf_code_units<encoding>(code,dst);
				}
			}
			else
			{
				if (static_cast<char8_t>(*src_first) < 0x80)
				{
					if constexpr(encoding_scheme::utf_ebcdic==encoding)
						*dst= static_cast<dest_char_type>(bm_i8_to_ebcdic[*src_first]);
					else
						*dst= static_cast<char8_t>(*src_first);
					if constexpr(sizeof(dest_char_type)!=1&&encoding_is_utf(encoding)&&!is_native_scheme(encoding))
						*dst=byte_swap(*dst);
					++src_first;
					++dst;
				}
				else
				{
					if constexpr(src_encoding!=encoding_scheme::utf)
					{
						auto [code,adv]=general_advance<src_encoding>(src_first,src_last-src_first);
						if(adv==static_cast<char8_t>(-1))
							break;
						src_first+=adv;
						if constexpr(sizeof(dest_char_type)==4)
						{
							*dst=code;
							if constexpr(encoding_is_utf(encoding)&&!is_native_scheme(encoding))
								*dst=byte_swap(*dst);
							++dst;
						}
						else
						{
							dst+=get_utf_code_units<encoding>(code,dst);
						}
					}
					else
					{
						auto [failed,src,code] = advance_with_big_table(src_first, src_last);
						if(failed)
							break;
						src_first=src;
						if constexpr(sizeof(dest_char_type)==4)
						{
							*dst=code;
							if constexpr(encoding_is_utf(encoding)&&!is_native_scheme(encoding))
								*dst=byte_swap(*dst);
							++dst;
						}
						else
							dst+=get_utf_code_units<encoding>(code,dst);
					}
				}
			}
		return {src_first,dst};
	}
	
}

template<
encoding_scheme src_encoding=encoding_scheme::execution_charset,
encoding_scheme encoding=encoding_scheme::execution_charset,typename state_type,
std::integral src_char_type,std::integral dest_char_type>
requires (sizeof(src_char_type)<=4 &&sizeof(dest_char_type)<=4)
inline constexpr dest_char_type* general_code_cvt(state_type& __restrict state,src_char_type const* src_first,src_char_type const* src_last,dest_char_type* __restrict dst) noexcept
{
	if constexpr(src_encoding==encoding_scheme::execution_charset)
	{
		return general_code_cvt<get_execution_charset_encoding_scheme<src_char_type>(src_encoding),
			encoding>(state,src_first,src_last,dst);
	}
	else if constexpr(encoding==encoding_scheme::execution_charset)
	{
		return general_code_cvt<src_encoding,
			get_execution_charset_encoding_scheme<dest_char_type>(encoding)>(state,src_first,src_last,dst);
	}
	else if constexpr(sizeof(src_char_type)==4)
	{
		static_assert(src_encoding==encoding_scheme::utf);
		return general_code_cvt<src_encoding,encoding>(src_first,src_last,dst).dst;
	}
	else if constexpr(sizeof(src_char_type)==2)
	{
		static_assert(src_encoding==encoding_scheme::utf);
		if(state.state)
		{
			if(src_first==src_last)
				return dst;
			char16_t low{state.value};
			if constexpr(!is_native_scheme(src_encoding))
				low=byte_swap(low);
			char16_t v{static_cast<char16_t>(*src_first)};
			if constexpr(!is_native_scheme(src_encoding))
				v=byte_swap(v);
			if(is_utf16_low_surrogate(v))
			{
				if constexpr(sizeof(dest_char_type)==4)
				{
					*dst = utf16_surrogate_to_utf32(low,v);
					if constexpr(!is_native_scheme(encoding))
						*dst = byte_swap(*dst);
					++dst;
				}
				else
					dst+=get_utf_code_units<encoding>(utf16_surrogate_to_utf32(low,v),dst);
				++src_first;
			}
			else[[unlikely]]
			{
				if constexpr(sizeof(dest_char_type)==4)
				{
					if constexpr(is_native_scheme(encoding))
						*dst=static_cast<dest_char_type>(0xFFFD);
					else
						*dst=static_cast<dest_char_type>(0xFDFF0000);
					++dst;
				}
				else
					dst+=get_general_invalid_code_units<encoding>(dst);
			}
		}
		auto [new_src,new_dst]=general_code_cvt<src_encoding,encoding>(src_first,src_last,dst);
		if((state.state=(new_src!=src_last)))
			state.value=*new_src;
		return new_dst;
	}
	else if constexpr(sizeof(src_char_type)==1)
	{
		std::size_t const state_size{static_cast<std::size_t>(state.size)};
		if(state_size)
		{
			std::size_t src_diff{static_cast<std::size_t>(src_last-src_first)};
			if(src_diff==0)
				return dst;
			constexpr std::size_t state_bytes{8};
			constexpr std::size_t state_bytesm1{state_bytes-1};
			std::size_t remain_unsolved{state_bytes-state_size};
			char8_t bytes[state_bytes];
			non_overlapped_copy_n(state.bytes,state_bytesm1,bytes);
			if(src_diff<remain_unsolved)
				remain_unsolved=src_diff;
			non_overlapped_copy_n(src_first,remain_unsolved,bytes+state_size);
			std::size_t total_bytes{state_size+remain_unsolved};
			if constexpr(src_encoding==encoding_scheme::utf)
			{
				auto [failed,bytes_src,code] = advance_with_big_table(bytes, bytes+total_bytes);
				if(failed)
				{
					non_overlapped_copy_n(bytes,state_bytesm1,state.bytes);
					state.size=static_cast<char8_t>(total_bytes);
					return dst;
				}
				if constexpr(sizeof(dest_char_type)==4)
				{
					*dst=code;
					if constexpr(!is_native_scheme(encoding))
						*dst=byte_swap(*dst);
					++dst;
				}
				else
					dst+=get_utf_code_units<encoding>(code,dst);
				src_first+=static_cast<std::size_t>(bytes_src-bytes-state_size);
			}
			else
			{
				auto [code,adv]=general_advance<src_encoding>(bytes,total_bytes);
				if(adv==static_cast<char8_t>(-1))
				{
					non_overlapped_copy_n(bytes,state_bytesm1,state.bytes);
					state.size=static_cast<char8_t>(total_bytes);
					return dst;
				}
				if constexpr(sizeof(dest_char_type)==4)
				{
					*dst=code;
					if constexpr(!is_native_scheme(encoding))
						*dst=byte_swap(*dst);
					++dst;
				}
				else
					dst+=get_utf_code_units<encoding>(code,dst);
				src_first+=static_cast<std::size_t>(static_cast<std::size_t>(adv)-state_size);
			}
		}
		auto [new_src,new_dst]=general_code_cvt<src_encoding,encoding>(src_first,src_last,dst);
		std::size_t diff{static_cast<std::size_t>(src_last-new_src)};
		state.size=static_cast<char8_t>(diff);
		non_overlapped_copy_n(new_src,diff,state.bytes);
		return new_dst;
	}
	else
		return dst;
}

template<
encoding_scheme src_encoding=encoding_scheme::execution_charset,
encoding_scheme encoding=encoding_scheme::execution_charset,
std::integral src_char_type,std::integral dest_char_type>
requires (sizeof(src_char_type)<=4 &&sizeof(dest_char_type)<=4)
inline constexpr dest_char_type* general_code_cvt_full(src_char_type const* src_first,src_char_type const* src_last,dest_char_type* __restrict dst) noexcept
{
	if constexpr(src_encoding==encoding_scheme::execution_charset)
	{
		constexpr auto src_scheme = get_execution_charset_encoding_scheme<src_char_type>(src_encoding);
		constexpr auto dst_scheme = get_execution_charset_encoding_scheme<dest_char_type>(encoding);
		return general_code_cvt_full<src_scheme,
			dst_scheme>(src_first,src_last,dst);
	}
	else
	{
		auto [src,new_dst]=general_code_cvt<src_encoding,encoding>(src_first,src_last,dst);
		if(src!=src_last)
		{
			if constexpr(sizeof(dest_char_type)==4)
			{
				if constexpr(is_native_scheme(encoding))
					*new_dst=static_cast<dest_char_type>(0xFFFD);
				else
					*new_dst=static_cast<dest_char_type>(0xFDFF0000);
				++new_dst;
			}
			else
				new_dst+=get_general_invalid_code_units<encoding>(dst);
		}
		return new_dst;
	}
}

template<typename T>
concept type_has_value_type = requires(T)
{
	typename T::value_type;
};

template<typename T>
inline constexpr bool print_alias_test_codecvt_impl() noexcept
{
	if constexpr(alias_printable<T>)
	{
		using alias_type = decltype(print_alias_define(io_alias,*static_cast<T const*>(static_cast<void const*>(nullptr))));
		if constexpr(type_has_value_type<alias_type>)
		{
			using value_type = typename alias_type::value_type;
			return std::same_as<alias_type,basic_io_scatter_t<value_type>>;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

}

namespace manipulators
{

template<
encoding_scheme src_scheme,
encoding_scheme dst_scheme,
std::integral char_type>
struct code_cvt_t
{
	using manip_tag = manip_tag_t;
	basic_io_scatter_t<char_type> reference;
};

template<
encoding_scheme src_scheme=encoding_scheme::execution_charset,
encoding_scheme dst_scheme=encoding_scheme::execution_charset,
typename T>
requires (::fast_io::details::codecvt::print_alias_test_codecvt_impl<T>())
constexpr auto code_cvt(T const& t) noexcept
{
	using value_type = typename decltype(print_alias_define(io_alias,t))::value_type;
	return code_cvt_t<src_scheme,dst_scheme,value_type>{print_alias_define(io_alias,t)};
}

template<
encoding_scheme src_scheme=encoding_scheme::execution_charset,
encoding_scheme dst_scheme=encoding_scheme::execution_charset,
std::integral char_type>
constexpr auto code_cvt(basic_io_scatter_t<char_type> t) noexcept
{
	return code_cvt_t<src_scheme,dst_scheme,char_type>{t};
}

template<
encoding_scheme src_scheme=encoding_scheme::execution_charset,
encoding_scheme dst_scheme=encoding_scheme::execution_charset,
std::integral char_type,std::size_t N>
constexpr auto code_cvt(small_scatter_t<char_type,N> t) noexcept
{
	return code_cvt_t<src_scheme,dst_scheme,char_type>{{t.base,t.len}};
}

template<
encoding_scheme src_scheme=encoding_scheme::execution_charset,
encoding_scheme dst_scheme=encoding_scheme::execution_charset,
::std::integral char_type>
constexpr auto code_cvt_os_c_str(char_type const* cstr) noexcept
{
	return ::fast_io::manipulators::code_cvt<src_scheme,dst_scheme>(::fast_io::manipulators::os_c_str(cstr));
}

template<
encoding_scheme src_scheme=encoding_scheme::execution_charset,
encoding_scheme dst_scheme=encoding_scheme::execution_charset,
::std::integral char_type>
constexpr auto code_cvt_os_c_str(char_type const* cstr,::std::size_t n) noexcept
{
	return ::fast_io::manipulators::code_cvt<src_scheme,dst_scheme>(::fast_io::mnp::os_c_str(cstr,n));
}

template<
encoding_scheme src_scheme=encoding_scheme::execution_charset,
encoding_scheme dst_scheme=encoding_scheme::execution_charset,
std::integral src_char_type,
std::integral dst_char_type>
inline constexpr std::size_t print_reserve_size(
io_reserve_type_t<dst_char_type,code_cvt_t<src_scheme,dst_scheme,src_char_type>>,
code_cvt_t<src_scheme,dst_scheme,src_char_type> v) noexcept
{
	return details::cal_full_reserve_size<sizeof(src_char_type),sizeof(dst_char_type)>(v.reference.len);
}

template<
encoding_scheme src_scheme=encoding_scheme::execution_charset,
encoding_scheme dst_scheme=encoding_scheme::execution_charset,
std::integral src_char_type,
::std::integral char_type>
inline constexpr char_type* print_reserve_define(
io_reserve_type_t<char_type,code_cvt_t<src_scheme,dst_scheme,src_char_type>>,
char_type* iter,
code_cvt_t<src_scheme,dst_scheme,src_char_type> v) noexcept
{
	return details::codecvt::general_code_cvt_full(v.reference.base,v.reference.base+v.reference.len,iter);
}

}

}
