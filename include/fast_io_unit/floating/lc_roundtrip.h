#pragma once

namespace fast_io::details
{
//let the compiler pick the best calling convention.
template<::std::integral char_type,my_unsigned_integral U>
inline constexpr char_type* lc_print_rsv_fp_dgs_common_decay_impl(char_type const* decimal_point_base,std::size_t decimal_point_len,char_type* iter,U m10,std::uint_least32_t m10len) noexcept
{
	auto res{iter+m10len+decimal_point_len};
	print_reserve_integral_main_impl<10,false,true>(res,m10,m10len);
	*iter=iter[decimal_point_len];
	++iter;
	if(decimal_point_len==1)[[likely]]
		*iter=*decimal_point_base;
	else
		iter=non_overlapped_copy_n(decimal_point_base,decimal_point_len,iter);
	return res;
}

template<::std::integral char_type,my_unsigned_integral U>
inline constexpr char_type* lc_print_rsv_fp_dgs_common_impl(basic_io_scatter_t<char_type> const& decimal_point_ref,char_type* iter,U m10,std::uint_least32_t m10len) noexcept
{
	basic_io_scatter_t<char_type> decimal_point{decimal_point_ref};
	return lc_print_rsv_fp_dgs_common_decay_impl(decimal_point.base,decimal_point.len,iter,m10,m10len);
}

template<::std::integral char_type,my_unsigned_integral U>
inline constexpr char_type* lc_print_rsv_fp_decimal_common_impl(basic_io_scatter_t<char_type> const& decimal_point_ref,char_type* iter,U m10,std::uint_least32_t m10len) noexcept
{
	if(m10len==1)[[unlikely]]
	{
		*iter=::fast_io::char_literal_add<char_type>(m10);
		++iter;
		return iter;
	}
	else
		return lc_print_rsv_fp_dgs_common_impl(decimal_point_ref,iter,m10,m10len);
}
struct lc_fixed_carrier
{
	std::int_least32_t olength;
	std::int_least32_t real_exp;
};

template<typename flt,::std::integral char_type>
inline constexpr char_type* lc_no_grouping_fixed_case1_integer_and_point(char_type* iter,typename iec559_traits<flt>::mantissa_type m10,lc_fixed_carrier carrier,basic_io_scatter_t<char_type> const& decimal_point) noexcept
{
	std::int_least32_t olength{carrier.olength};
	std::int_least32_t real_exp{carrier.real_exp};
	auto eposition(real_exp+1);
	if(olength==eposition)
		print_reserve_integral_main_impl<10,false,true>(iter+=olength,m10,static_cast<std::uint_least32_t>(olength));
	else
	{
		auto tmp{iter};
		auto decimal_point_base{decimal_point.base};
		std::size_t const decimal_point_len{decimal_point.len};
		print_reserve_integral_main_impl<10,false,true>(iter+=olength+decimal_point_len,m10,static_cast<std::uint_least32_t>(olength));
		my_copy_n(tmp+decimal_point_len,static_cast<std::uint_least32_t>(eposition),tmp);
		if(decimal_point_len==1u)[[likely]]
			tmp[eposition]=*decimal_point_base;
		else
		{
			non_overlapped_copy_n(decimal_point_base,decimal_point_len,tmp+eposition);
		}
	}
	return iter;
}

template<typename flt,::std::integral char_type>
inline constexpr char_type* lc_grouping_fixed_case2_all_point(char_type* iter,typename iec559_traits<flt>::mantissa_type m10,lc_fixed_carrier carrier,basic_io_scatter_t<char_type> const& decimal_point) noexcept
{
	*iter=char_literal_v<u8'0',char_type>;
	++iter;
	auto decimal_point_base{decimal_point.base};
	std::size_t const decimal_point_len{decimal_point.len};
	if(decimal_point_len==1u)
	{
		*iter=*decimal_point_base;
		++iter;
	}
	else
	{
		iter=non_overlapped_copy_n(decimal_point_base,decimal_point_len,iter);
	}
	std::int_least32_t olength{carrier.olength};
	std::int_least32_t real_exp{carrier.real_exp};
	iter=fill_zeros_impl(iter,static_cast<std::uint_least32_t>(-real_exp-1));
	print_reserve_integral_main_impl<10,false,true>(iter+=olength,m10,static_cast<std::uint_least32_t>(olength));
	return iter;
}

template<::std::integral char_type>
constexpr char_type* lc_grouping_single_sep_ch_impl(std::size_t const* grouping_base,std::size_t grouping_len,char_type* first,char_type* last,char_type* out_reverse,char_type replacement_ch) noexcept
{
	auto iter{last};
	std::size_t i{};
	for(;i!=grouping_len;++i)
	{
		auto e{grouping_base[i]};
		if(e==0)
			break;
		for(std::size_t j{};j!=e;++j)
		{
			*--out_reverse=*--iter;
			if(iter==first)
				return out_reverse;
		}
		*--out_reverse=replacement_ch;
	}
	if(i!=grouping_len)
	{
		for(;;)
		{
			*--out_reverse=*--iter;
			if(iter==first)
				return out_reverse;
		}
	}
	else
	{
		for(std::size_t e{grouping_base[i-1]};;*--out_reverse=replacement_ch)
		{
			for(std::size_t j{};j!=e;++j)
			{
				*--out_reverse=*--iter;
				if(iter==first)
					return out_reverse;
			}
		}
	}
	return out_reverse;
}

template<::std::random_access_iterator srcIter,::std::random_access_iterator destIter>
inline constexpr destIter grouping_handle_buffer_internal(srcIter first,srcIter last,destIter dest,std::size_t const* grouping_base,std::size_t grouping_len,
::std::iter_value_t<destIter> const* thousands_sep_base,std::size_t thousands_sep_len,::std::iter_value_t<destIter>* buffer_end) noexcept
{
	using char_type = ::std::iter_value_t<srcIter>;
	char_type seperator_ch{char_literal_v<u8',',char_type>};
	bool sep_single_character{thousands_sep_len==1};
	if(sep_single_character)
	{
		seperator_ch=*thousands_sep_base;
	}
	auto buffer_start{lc_grouping_single_sep_ch_impl(grouping_base,grouping_len,first,last,buffer_end,seperator_ch)};
	if(sep_single_character)
		return non_overlapped_copy(buffer_start,buffer_end,dest);
	else
		return grouping_mul_sep_print_sep_impl(thousands_sep_base,thousands_sep_len,buffer_start,buffer_end,dest);
}

template<std::size_t digits,::std::random_access_iterator srcIter,::std::random_access_iterator destIter>
requires (digits<(SIZE_MAX>>1u))
inline constexpr destIter grouping_handle_buffer(srcIter first,srcIter last,destIter dest,std::size_t const* grouping_base,std::size_t grouping_len,
::std::iter_value_t<destIter> const* thousands_sep_base,std::size_t thousands_sep_len) noexcept
{
	using char_type = ::std::iter_value_t<srcIter>;
	constexpr std::size_t allocated_buffer_size{digits<<1u};
	char_type buffer[allocated_buffer_size];
	return grouping_handle_buffer_internal(first,last,dest,grouping_base,grouping_len,thousands_sep_base,thousands_sep_len,buffer+allocated_buffer_size);
}

template<typename flt,::std::random_access_iterator Iter>
inline constexpr Iter lc_grouping_fixed_case1_integer_and_point(Iter iter,typename iec559_traits<flt>::mantissa_type m10,lc_fixed_carrier carrier,basic_io_scatter_t<::std::iter_value_t<Iter>> const& decimal_point,
std::size_t const* grouping_base,std::size_t grouping_len,::std::iter_value_t<Iter> const* thousands_sep_base,std::size_t thousands_sep_len) noexcept
{
	using trait = iec559_traits<flt>;
	constexpr std::size_t m10total_digits{trait::m10digits+trait::e10digits+2};
	using char_type = ::std::iter_value_t<Iter>;
	std::int_least32_t olength{carrier.olength};
	std::int_least32_t real_exp{carrier.real_exp};
	auto eposition(real_exp+1);
	if(olength==eposition)
	{
		char_type buffer1[m10total_digits];
		auto buffer1_end{buffer1+olength};
		print_reserve_integral_main_impl<10,false,true>(buffer1_end,m10,static_cast<std::uint_least32_t>(olength));
		return grouping_handle_buffer<m10total_digits>(buffer1,buffer1_end,iter,grouping_base,grouping_len,thousands_sep_base,thousands_sep_len);
	}
	else
	{
		char_type buffer1[m10total_digits];
		auto buffer1_end{buffer1+olength};
		print_reserve_integral_main_impl<10,false,true>(buffer1_end,m10,static_cast<std::uint_least32_t>(olength));
		iter=grouping_handle_buffer<m10total_digits>(buffer1,buffer1+eposition,iter,grouping_base,grouping_len,thousands_sep_base,thousands_sep_len);
		auto decimal_point_base{decimal_point.base};
		auto decimal_point_len{decimal_point.len};
		if(decimal_point_len==1u)
		{
			*iter=*decimal_point_base;
			++iter;
		}
		else
		{
			iter=non_overlapped_copy_n(decimal_point_base,decimal_point_len,iter);
		}
		iter=non_overlapped_copy(buffer1+eposition,buffer1_end,iter);
	}
	return iter;
}

template<typename flt,::std::integral char_type>
inline constexpr char_type* lc_print_rsv_fp_fixed_decision_impl(basic_lc_all<char_type> const* all,char_type* iter,typename iec559_traits<flt>::mantissa_type m10,std::int_least32_t e10) noexcept
{
	auto const& numeric_ref{all->numeric};
	auto thousands_sep{numeric_ref.thousands_sep};
	auto thousands_sep_base{thousands_sep.base};
	auto thousands_sep_len{thousands_sep.len};
	auto grouping{numeric_ref.grouping};
	auto grouping_base{grouping.base};
	auto grouping_len{grouping.len};
	std::int_least32_t olength(static_cast<std::int_least32_t>(chars_len<10,true>(m10)));	
	std::int_least32_t const real_exp(static_cast<std::int_least32_t>(e10 + olength - 1));
	bool no_grouping_grouping_case{((grouping_len==0u)||(thousands_sep_len==0u))};	//no grouping. for case like "C" or "POSIX"
	if(olength<=real_exp)
	{
		if(no_grouping_grouping_case)
		{
			return fixed_case0_full_integer<flt>(iter,m10,olength,real_exp);
		}
		else
		{
			using trait = iec559_traits<flt>;
			constexpr std::size_t m10total_digits{trait::m10digits+trait::e10max+2};
			char_type buffer1[m10total_digits];
			auto buffer1_end{fixed_case0_full_integer<flt>(buffer1,m10,olength,real_exp)};
			return grouping_handle_buffer<m10total_digits>(buffer1,buffer1_end,iter,grouping_base,grouping_len,thousands_sep_base,thousands_sep_len);
		}
	}
	else if(0<=real_exp&&real_exp<olength)
	{
		if(no_grouping_grouping_case)
		{
			return lc_no_grouping_fixed_case1_integer_and_point<flt>(iter,m10,{olength,real_exp},numeric_ref.decimal_point);
		}
		else
		{
			return lc_grouping_fixed_case1_integer_and_point<flt>(iter,m10,{olength,real_exp},numeric_ref.decimal_point,grouping_base,grouping_len,thousands_sep_base,thousands_sep_len);
		}
	}
	else
	{
		return lc_grouping_fixed_case2_all_point<flt>(iter,m10,{olength,real_exp},numeric_ref.decimal_point);
	}
}

template<
typename flt,
bool uppercase_e,
::fast_io::manipulators::floating_format mt,
::std::integral char_type>
inline constexpr char_type* lc_print_rsv_fp_decision_impl(basic_lc_all<char_type> const* all,char_type* iter,typename iec559_traits<flt>::mantissa_type m10,std::int_least32_t e10) noexcept
{
	if constexpr(mt==::fast_io::manipulators::floating_format::general)
	{
		if(-5<e10&&e10<7)
		{
			return lc_print_rsv_fp_fixed_decision_impl<flt>(all,iter,m10,e10);
		}
		return lc_print_rsv_fp_decision_impl<flt,uppercase_e,::fast_io::manipulators::floating_format::scientific>(all,iter,m10,e10);
	}
	else if constexpr(mt==::fast_io::manipulators::floating_format::scientific)
	{
		if(m10<10u)[[unlikely]]
		{
			*iter=::fast_io::char_literal_add<char_type>(m10);
			++iter;
		}
		else
		{
			std::uint_least32_t olength{static_cast<std::uint_least32_t>(chars_len<10,true>(m10))};
			std::uint_least32_t sz{static_cast<std::uint_least32_t>(olength-1u)};
			e10+=static_cast<std::int_least32_t>(sz);
			iter=lc_print_rsv_fp_dgs_common_impl(all->numeric.decimal_point,iter,m10,olength);
		}
		return print_rsv_fp_e_impl<flt,uppercase_e>(iter,e10);
	}
	else	//decimal
	{
		std::int_least32_t olength{static_cast<std::int_least32_t>(chars_len<10,true>(m10))};
		std::int_least32_t const real_exp{static_cast<std::int_least32_t>(e10 + olength - 1)};
		std::uint_least32_t fixed_length{},this_case{};
		if(olength<=real_exp)
		{
			fixed_length=static_cast<std::uint_least32_t>(real_exp+1);
			this_case=1;
		}
		else if(0<=real_exp&&real_exp<olength)
		{
			fixed_length=static_cast<std::uint_least32_t>(olength+2);
			if(olength==real_exp+1)
				--fixed_length;
			this_case=2;
		}
		else
			fixed_length=static_cast<std::uint_least32_t>(static_cast<std::uint_least32_t>(-real_exp)+static_cast<std::uint_least32_t>(olength)+1u);
		std::uint_least32_t scientific_length{static_cast<std::uint_least32_t>(olength==1?olength+3:olength+5)};
		auto const& numeric_ref{all->numeric};
		if(scientific_length<fixed_length)
		{
			//scientific decision
			iter=lc_print_rsv_fp_decimal_common_impl(numeric_ref.decimal_point,iter,m10,static_cast<std::uint_least32_t>(olength));
			return print_rsv_fp_e_impl<flt,uppercase_e>(iter,real_exp);
		}
		//fixed decision
		auto thousands_sep{numeric_ref.thousands_sep};
		auto thousands_sep_base{thousands_sep.base};
		auto thousands_sep_len{thousands_sep.len};
		auto grouping{numeric_ref.grouping};
		auto grouping_base{grouping.base};
		auto grouping_len{grouping.len};
		bool no_grouping_grouping_case{((grouping_len==0u)||(thousands_sep_len==0u))};	//no grouping. for case like "C" or "POSIX"
		using trait = iec559_traits<flt>;
		constexpr std::size_t m10total_digits{trait::m10digits+trait::e10digits+2};
		switch(this_case)
		{
			case 1:
			{
				if(no_grouping_grouping_case)
				{
					return fixed_case0_full_integer<flt>(iter,m10,olength,real_exp);
				}
				else
				{
					char_type buffer1[m10total_digits];
					auto buffer1_end{fixed_case0_full_integer<flt>(buffer1,m10,olength,real_exp)};
					return grouping_handle_buffer<m10total_digits>(buffer1,buffer1_end,iter,grouping_base,grouping_len,thousands_sep_base,thousands_sep_len);
				}
			}
			case 2:
			{
				if(no_grouping_grouping_case)
				{
					return lc_no_grouping_fixed_case1_integer_and_point<flt>(iter,m10,{olength,real_exp},numeric_ref.decimal_point);
				}
				else
				{
					return lc_grouping_fixed_case1_integer_and_point<flt>(iter,m10,{olength,real_exp},numeric_ref.decimal_point,grouping_base,grouping_len,thousands_sep_base,thousands_sep_len);
				}
			}
			default:
			{
				return lc_grouping_fixed_case2_all_point<flt>(iter,m10,{olength,real_exp},numeric_ref.decimal_point);
			}
		}
	}
}

template<
bool showpos,
bool uppercase,
bool uppercase_e,
::fast_io::manipulators::floating_format mt,
typename flt,::std::integral char_type>
inline constexpr char_type* lc_print_rsvflt_define_impl(basic_lc_all<char_type> const* all,char_type* iter,flt f) noexcept
{
	if constexpr(::fast_io::manipulators::floating_format::fixed==mt&&uppercase_e)
	{
		return lc_print_rsvflt_define_impl<showpos,uppercase,false,mt>(all,iter,f);
	}
	else
	{
		using trait = iec559_traits<flt>;
		using mantissa_type = typename trait::mantissa_type;
		constexpr std::size_t ebits{trait::ebits};
		constexpr mantissa_type exponent_mask{(static_cast<mantissa_type>(1)<<ebits)-1};
		constexpr std::uint_least32_t exponent_mask_u32{static_cast<std::uint_least32_t>(exponent_mask)};
		auto [mantissa,exponent,sign] = get_punned_result(f);
		iter=print_rsv_fp_sign_impl<showpos>(iter,sign);
		if(exponent==exponent_mask_u32)
			return prsv_fp_nan_impl<uppercase>(iter,mantissa);
		if(!mantissa&&!exponent)
		{
			if constexpr(mt!=::fast_io::manipulators::floating_format::scientific)
			{
				*iter=char_literal_v<u8'0',char_type>;
				++iter;
				return iter;
			}
			else
				return prsv_fp_dece0<uppercase>(iter);
		}
		auto [m10,e10] = dragonbox_impl<flt>(mantissa,static_cast<std::int_least32_t>(exponent));
		if constexpr(mt==::fast_io::manipulators::floating_format::fixed)
			return lc_print_rsv_fp_fixed_decision_impl<flt>(all,iter,m10,e10);
		else
			return lc_print_rsv_fp_decision_impl<flt,uppercase_e,mt>(all,iter,m10,e10);
	}
}

}
