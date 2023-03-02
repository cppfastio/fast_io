#pragma once

namespace fast_io
{

namespace details
{
template<std::integral char_type,manipulators::scalar_flags flags,typename T>
inline constexpr std::size_t lc_print_reserve_size_int_cal(basic_lc_all<char_type> const* __restrict all) noexcept
{
	constexpr std::size_t static_size{print_integer_reserved_size_cache<flags.base,flags.showbase,flags.showpos,std::remove_cv_t<T>>};
	constexpr std::size_t static_sizem1{static_size-1};
	return static_size+static_sizem1*all->numeric.thousands_sep.len;
}

template<std::integral char_type,std::size_t base,bool uppercase,my_unsigned_integral T>
inline constexpr char_type to_char_single_digit(T t) noexcept
{
	if constexpr(base<=10)
	{
		return ::fast_io::char_literal_add<char_type>(t);
	}
	else
	{
		constexpr T ten{static_cast<T>(10u)};
		if(t<ten)
			return ::fast_io::char_literal_add<char_type>(t);
		else
		{
			if constexpr(is_ebcdic<char_type>)
			{
				if constexpr(base<=19)
				{
					if constexpr(uppercase)
						return static_cast<char_type>((0xC1-10)+t);
					else
						return static_cast<char_type>((0x81-10)+t);
				}
				else if constexpr(base<=28)
				{
					if(t<19u)
					{
						if constexpr(uppercase)
							return static_cast<char_type>((0xC1-10u)+t);
						else
							return static_cast<char_type>((0x81-10u)+t);
					}
					else
					{
						if constexpr(uppercase)
							return static_cast<char_type>((0xD1-19u)+t);
						else
							return static_cast<char_type>((0x91-19u)+t);
					}
				}
				else
				{
					if(t<19u)
					{
						if constexpr(uppercase)
							return static_cast<char_type>((0xC1-10u)+t);
						else
							return static_cast<char_type>((0x81-10u)+t);
					}
					else if(t<28u)
					{
						if constexpr(uppercase)
							return static_cast<char_type>((0xD1-19u)+t);
						else
							return static_cast<char_type>((0x91-19u)+t);
					}
					else
					{
						if constexpr(uppercase)
							return static_cast<char_type>((0xE2-28u)+t);
						else
							return static_cast<char_type>((0xA2-28u)+t);
					}
				}
			}
			else
			{
				return ::fast_io::char_literal_add<char_type,(uppercase?u8'A':u8'a')>(t-ten);
			}
		}
	}
}

template<bool full,std::size_t base,bool uppercase,::std::integral char_type,my_unsigned_integral T>
constexpr char_type* lc_grouping_single_sep_ch_impl(basic_io_scatter_t<std::size_t> const& grouping, char_type* iter,T t, char_type replacement_ch) noexcept
{
	std::size_t const* grouping_base{grouping.base};
	std::size_t const grouping_len{grouping.len};
	std::size_t i{};
	constexpr T c0{static_cast<T>(base)};
	constexpr std::size_t full_len{cal_max_int_size<T,base>()};
	std::size_t full_i{};
	for(;i!=grouping_len;++i)
	{
		auto e{grouping_base[i]};
		if(e==0)
			break;
		for(std::size_t j{};j!=e;++j)
		{
			*--iter=to_char_single_digit<char_type,base,uppercase>(static_cast<T>(t%c0));
			t/=c0;
			if constexpr(full)
			{
				if((++full_i)==full_len)
					return iter;
			}
			else
			{
				if(t==0)
					return iter;
			}
		}
		*--iter=replacement_ch;
	}
	if(i!=grouping_len)
	{
		for(;;)
		{
			*--iter=to_char_single_digit<char_type,base,uppercase>(static_cast<T>(t%c0));
			t/=c0;
			if constexpr(full)
			{
				if((++full_i)==full_len)
					return iter;
			}
			else
			{
				if(t==0)
					return iter;
			}
		}
	}
	else
	{
		for(std::size_t e{grouping_base[i-1]};;*--iter=replacement_ch)
		{
			for(std::size_t j{};j!=e;++j)
			{
				*--iter=to_char_single_digit<char_type,base,uppercase>(static_cast<T>(t%c0));
				t/=c0;
				if constexpr(full)
				{
					if((++full_i)==full_len)
						return iter;
				}
				else
				{
					if(t==0)
						return iter;
				}
			}
		}
	}
}


template<std::integral char_type>
constexpr char_type* grouping_mul_sep_print_sep_impl(char_type const* thousands_sep_base,std::size_t thousands_sep_len,
	char_type const* first,char_type const* last,
	char_type* outit) noexcept
{
	for(;first!=last;++first)
	{
		if(*first==char_literal_v<u8',',char_type>)
		{
			outit=non_overlapped_copy_n(thousands_sep_base,thousands_sep_len,outit);
		}
		else
		{
			*outit=*first;
			++outit;
		}
	}
	return outit;
}

template<bool full,std::size_t base,bool uppercase,::std::integral char_type,my_unsigned_integral T>
constexpr char_type* grouping_mul_sep_impl(basic_lc_all<char_type> const* __restrict all, char_type* iter,T t) noexcept
{
	constexpr std::size_t array_len{cal_max_int_size<T,base>()*2u-1u};
	char_type array[array_len];
	auto const ed{array+array_len};
	auto thousands_sep{all->numeric.thousands_sep};
	char_type replacement_ch{char_literal_v<u8',',char_type>};
	bool single_character{thousands_sep.len==1};
	if(single_character)
	{
		replacement_ch=*thousands_sep.base;
	}
	auto first{lc_grouping_single_sep_ch_impl<full,base,uppercase>(all->numeric.grouping,ed,t,replacement_ch)};
	if(single_character)
	{
		return non_overlapped_copy(first,ed,iter);
	}
	else
	{
		return grouping_mul_sep_print_sep_impl(thousands_sep.base,thousands_sep.len,first,ed,iter);
	}
}

template<std::size_t base,bool uppercase,::std::integral char_type,my_unsigned_integral T>
requires (sizeof(T)>1)
constexpr void lc_print_unsigned_with_3_seperator_len(char_type seperator_ch, char_type* iter,T value,std::size_t size) noexcept
{
	constexpr auto table(digits_table<char_type,base,uppercase>);
	constexpr std::uint_least32_t cpow1{static_cast<std::uint_least32_t>(base)};
	constexpr std::uint_least32_t cpow2{static_cast<std::uint_least32_t>(cpow1*cpow1)};
	constexpr std::uint_least32_t cpow3{static_cast<std::uint_least32_t>(cpow2*cpow1)};
	for(;3u<size;*--iter=seperator_ch)
	{
		T low3digits{static_cast<T>(value%cpow3)};
		value/=cpow3;
		T low2digits{static_cast<T>(low3digits%cpow2)};
		T highdigit{static_cast<T>(low3digits/cpow2)};
		non_overlapped_copy_n(table+(low2digits<<1),2u,iter-=2u);
		*--iter=to_char_single_digit<char_type,base,uppercase>(highdigit);
		size-=3u;
	}
	if(size==3)
	{
		T low2digits{static_cast<T>(value%cpow2)};
		T highdigit{static_cast<T>(value/cpow2)};
		non_overlapped_copy_n(table+(low2digits<<1),2u,iter-=2u);
		*--iter=to_char_single_digit<char_type,base,uppercase>(highdigit);
	}
	else if(size==2)
		non_overlapped_copy_n(table+(value<<1),2u,iter-=2u);
	else
	{
		*--iter=to_char_single_digit<char_type,base,uppercase>(value);
	}
}

template<bool full,std::size_t base,bool uppercase,::std::integral char_type,my_unsigned_integral int_type>
inline constexpr char_type* print_lc_grouping_3_path_impl(char_type seperator, char_type* iter,int_type t) noexcept
{
	if constexpr(full)
	{
		constexpr std::size_t size{cal_max_int_size<int_type,base>()};
		constexpr std::size_t offset_size{size+(size-1)/3};
		if constexpr(sizeof(int_type)<=sizeof(unsigned))
			lc_print_unsigned_with_3_seperator_len<base,uppercase>(seperator,iter+=offset_size,static_cast<unsigned>(t),size);
		else
			lc_print_unsigned_with_3_seperator_len<base,uppercase>(seperator,iter+=offset_size,t,size);
	}
	else
	{
		std::size_t const size{chars_len<base>(t)};
		std::size_t const offset_size{size+(size-1u)/3u};
		if constexpr(sizeof(int_type)<=sizeof(unsigned))
			lc_print_unsigned_with_3_seperator_len<base,uppercase>(seperator,iter+=offset_size,static_cast<unsigned>(t),size);
		else
			lc_print_unsigned_with_3_seperator_len<base,uppercase>(seperator,iter+=offset_size,t,size);
	}
	return iter;
}

template<bool full,std::size_t base,bool uppercase,::std::integral char_type,my_unsigned_integral intg>
inline constexpr char_type* lc_print_reserve_integral_withfull_main_impl(basic_lc_all<char_type> const* __restrict all, char_type* first,intg t)
{
	if((all->numeric.grouping.len==0)|(all->numeric.thousands_sep.len==0))
		return print_reserve_integral_withfull_main_impl<full,base,uppercase>(first,t);
	else if((all->numeric.grouping.len==1&&*all->numeric.grouping.base==3)&(all->numeric.thousands_sep.len==1))
		return print_lc_grouping_3_path_impl<full,base,uppercase>(*(all->numeric.thousands_sep.base),first,t);
	else
		return grouping_mul_sep_impl<full,base,uppercase>(all,first,t);
}

template<std::size_t base,
	bool showbase=false,
	bool uppercase_showbase=false,
	bool showpos=false,
	bool uppercase=false,
	bool full=false,
	typename int_type,::std::integral char_type>
inline constexpr char_type* lc_print_reserve_integral_define(basic_lc_all<char_type> const* __restrict all, char_type* first,int_type t) noexcept
{
	if constexpr(base<=10&&uppercase)
	{
		return print_reserve_integral_define<base,showbase,uppercase_showbase,showpos,false,full>(first,t);//prevent duplications
	}
	else
	{
		static_assert((2<=base)&&(base<=36));
		using unsigned_type = ::fast_io::details::my_make_unsigned_t<int_type>;
		unsigned_type u{static_cast<unsigned_type>(t)};
		if constexpr(showpos)
		{
			if constexpr(::fast_io::details::my_unsigned_integral<int_type>)
			{
				*first=char_literal_v<u8'+',char_type>;
			}
			else
			{
				if(t<0)
				{
					*first=char_literal_v<u8'-',char_type>;
					constexpr unsigned_type zero{};
					u=zero-u;
				}
				else
				{
					*first=char_literal_v<u8'+',char_type>;
				}
			}
			++first;
		}
		else
		{
			if constexpr(::fast_io::details::my_signed_integral<int_type>)
			{
				if(t<0)
				{
					*first=char_literal_v<u8'-',char_type>;
					++first;
					constexpr unsigned_type zero{};
					u=zero-u;
				}
			}
		}
		if constexpr(showbase&&(base!=10))
			first=print_reserve_show_base_impl<base,uppercase_showbase>(first);
		return lc_print_reserve_integral_withfull_main_impl<full,base,uppercase>(all,first,u);
	}
}


template<std::integral char_type>
inline constexpr std::size_t print_reserve_size_grouping_timestamp_impl(basic_lc_all<char_type> const* __restrict all)
{
	constexpr std::size_t static_size{print_reserve_size(io_reserve_type<char_type,std::int_least64_t>)};
	constexpr std::size_t static_sizem1{static_size-1};
	return static_size+static_sizem1*all->numeric.thousands_sep.len+all->numeric.decimal_point.len+std::numeric_limits<std::uint_least64_t>::digits10;
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define_grouping_timestamp_impl(basic_lc_all<char_type> const* __restrict all, char_type* iter,unix_timestamp timestamp)
{
	iter=lc_print_reserve_integral_define<10>(all,iter,timestamp.seconds);
	if(timestamp.subseconds)
	{
		if(all->numeric.decimal_point.len==1)
		{
			*iter=all->numeric.decimal_point.base[0];
			++iter;
		}
		else
			iter=non_overlapped_copy_n(all->numeric.decimal_point.base,all->numeric.decimal_point.len,iter);
		iter=output_iso8601_subseconds_main(iter,timestamp.subseconds);
	}
	return iter;
}

}
template<std::integral char_type,::fast_io::manipulators::scalar_flags flags,typename T>
requires ((details::my_integral<T>||std::same_as<std::remove_cv_t<T>,std::byte>)&&!flags.alphabet&&!std::same_as<std::remove_cv_t<T>,bool>)
inline constexpr std::size_t print_reserve_size(basic_lc_all<char_type> const* __restrict all,manipulators::scalar_manip_t<flags,T>) noexcept
{
	if constexpr(std::same_as<std::remove_cv_t<T>,std::byte>)
		return ::fast_io::details::lc_print_reserve_size_int_cal<char_type,flags,char8_t>(all);
	else
		return ::fast_io::details::lc_print_reserve_size_int_cal<char_type,flags,std::remove_cv_t<T>>(all);
}

template<::std::integral char_type,::fast_io::manipulators::scalar_flags flags,typename T>
requires ((details::my_integral<T>||std::same_as<std::remove_cv_t<T>,std::byte>)&&!flags.alphabet&&!std::same_as<std::remove_cv_t<T>,bool>)
inline constexpr char_type* print_reserve_define(basic_lc_all<char_type> const* __restrict all, char_type* iter,manipulators::scalar_manip_t<flags,T> t) noexcept
{
	if constexpr(std::same_as<std::remove_cv_t<T>,std::byte>)
		return details::lc_print_reserve_integral_define<flags.base,flags.showbase,flags.uppercase_showbase,flags.showpos,flags.uppercase,flags.full>(iter,static_cast<char8_t>(t.reference));
	else
		return details::lc_print_reserve_integral_define<flags.base,flags.showbase,flags.uppercase_showbase,flags.showpos,flags.uppercase,flags.full>(all,iter,t.reference);
}

template<std::integral char_type,std::int_least64_t off_to_epoch>
inline constexpr std::size_t print_reserve_size(basic_lc_all<char_type> const* __restrict all,basic_timestamp<off_to_epoch>) noexcept
{
	return details::print_reserve_size_grouping_timestamp_impl(all);
}

template<::std::integral char_type,std::int_least64_t off_to_epoch>
inline constexpr char_type* print_reserve_define(basic_lc_all<char_type> const* __restrict all, char_type* iter,basic_timestamp<off_to_epoch> ts) noexcept
{
	if constexpr(off_to_epoch==0)
		return details::print_reserve_define_grouping_timestamp_impl(all,iter,ts);
	else
		return details::print_reserve_define_grouping_timestamp_impl(all,iter,{ts.seconds,ts.subseconds});
}

template<std::integral char_type,::fast_io::manipulators::scalar_flags flags,typename T>
requires ((details::my_integral<T>||std::same_as<std::remove_cv_t<T>,std::byte>)&&!flags.alphabet&&!std::same_as<std::remove_cv_t<T>,bool>&&(flags.showpos||!details::my_unsigned_integral<T>))
inline constexpr std::size_t print_define_internal_shift(basic_lc_all<char_type> const* __restrict,manipulators::scalar_manip_t<flags,T> t) noexcept
{
	if constexpr(flags.showpos)
	{
		return 1;
	}
	else
	{
		return t.reference<0;
	}
}

template<std::integral char_type,::fast_io::manipulators::scalar_flags flags,std::int_least64_t off_to_epoch>
inline constexpr std::size_t print_define_internal_shift(basic_lc_all<char_type> const* __restrict,basic_timestamp<off_to_epoch> t) noexcept
{
	return t.seconds<0;
}


}
