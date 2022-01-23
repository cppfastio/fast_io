#pragma once

namespace fast_io
{
namespace details
{

template<bool control,typename T,char8_t value>
struct compile_time_floating_value;

template<char8_t decimal_point>
struct compile_time_floating_value<false,char8_t,decimal_point>
{};

template<char8_t decm>
inline constexpr compile_time_floating_value<false,char8_t,decm> compile_time_floating_v{};

template<std::integral ch_type>
struct compile_time_floating_value<true,ch_type,0>		//if you see ~ which means it is a bug.
{
	ch_type decimal_point{};
};

template<::fast_io::freestanding::contiguous_iterator Iter,my_unsigned_integral U>
inline constexpr std::size_t fp_output_unsigned(Iter iter,U i)
{
	if(std::is_constant_evaluated())
	{
		return optimize_size::output_unsigned(iter,static_cast<std::remove_cvref_t<U>>(i));
	}
	else
	{
		namespace algo_decision = 
#ifdef __OPTIMIZE_SIZE__
			optimize_size;
#elif defined(FAST_IO_OPTIMIZE_TIME)
			jiaendu::fp;
#else
			twodigits::fp;
#endif
		return algo_decision::output_unsigned(iter,static_cast<std::remove_cvref_t<U>>(i));
	}
}

template<::fast_io::freestanding::contiguous_iterator Iter,my_unsigned_integral U>
inline constexpr void fp_output_unsigned_with_len(Iter iter,U i,std::size_t len) noexcept
{
	if(std::is_constant_evaluated())
	{
		optimize_size::with_length::output_unsigned(iter,static_cast<std::remove_cvref_t<U>>(i),len);
	}
	else
	{
#ifdef __OPTIMIZE_SIZE__
		optimize_size::with_length::output_unsigned(iter,static_cast<std::remove_cvref_t<U>>(i),len);
#elif defined(FAST_IO_OPTIMIZE_TIME)
		jiaendu::fp::output_unsigned(iter,static_cast<std::remove_cvref_t<U>>(i));
#else
		return twodigits::with_length::output_unsigned(iter,static_cast<std::remove_cvref_t<U>>(i),len);
#endif
	}
}
/*To remove*/
template<bool control,typename T,char8_t static_decimal_point,my_unsigned_integral U,::fast_io::freestanding::contiguous_iterator Iter>
inline constexpr std::size_t fp_output_unsigned_point([[maybe_unused]]compile_time_floating_value<control,T,static_decimal_point> decm,U value,Iter str) noexcept
{
	if(value >= 10)[[likely]]
	{
		if(std::is_constant_evaluated())
		{
			std::size_t ret(optimize_size::output_unsigned(::fast_io::freestanding::to_address(str)+1,value));
			*str=str[1];
			if constexpr(control)
				str[1]=decm.decimal_point;
			else
				str[1]=static_decimal_point;
			return ret+1;
		}
		else
		{

			namespace algo_decision = 
#ifdef __OPTIMIZE_SIZE__
				optimize_size;
#elif defined(FAST_IO_OPTIMIZE_TIME)
				jiaendu::fp;
#else
				twodigits::fp;
#endif
			std::size_t ret(algo_decision::output_unsigned(::fast_io::freestanding::to_address(str)+1,value));
			*str=str[1];
			if constexpr(control)
				str[1]=decm.decimal_point;
			else
				str[1]=static_decimal_point;
			return ret+1;
		}
	}
	else
	{
		*str = static_cast<char8_t>(value)+u8'0';
		return 1;
	}
}



template<char8_t decimal_point,::fast_io::freestanding::random_access_iterator Iter,my_unsigned_integral U>
inline constexpr std::size_t fp_output_unsigned_point_no_dcm(U value,Iter str) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	if(value >= 10)[[likely]]
	{
		if(std::is_constant_evaluated())
		{
			std::size_t ret(optimize_size::output_unsigned(str+1,value));
			*str=str[1];
			if constexpr(decimal_point==u8'.')
			{
				if constexpr(std::same_as<char_type,char>)
					str[1]='.';
				else if constexpr(std::same_as<char_type,wchar_t>)
					str[1]=L'.';
				else
					str[1]=u8'.';
			}
			else if constexpr(decimal_point==u8',')
			{
				if constexpr(std::same_as<char_type,char>)
					str[1]=',';
				else if constexpr(std::same_as<char_type,wchar_t>)
					str[1]=L',';
				else
					str[1]=u8',';
			}
			return ret+1;
		}
		else
		{

			namespace algo_decision = 
#ifdef __OPTIMIZE_SIZE__
				optimize_size;
#elif defined(FAST_IO_OPTIMIZE_TIME)
				jiaendu::fp;
#else
				twodigits::fp;
#endif
			std::size_t ret(algo_decision::output_unsigned(str+1,value));
			*str=str[1];
			if constexpr(decimal_point==u8'.')
			{
				if constexpr(std::same_as<char_type,char>)
					str[1]='.';
				else if constexpr(std::same_as<char_type,wchar_t>)
					str[1]=L'.';
				else
					str[1]=u8'.';
			}
			else if constexpr(decimal_point==u8',')
			{
				if constexpr(std::same_as<char_type,char>)
					str[1]=',';
				else if constexpr(std::same_as<char_type,wchar_t>)
					str[1]=L',';
				else
					str[1]=u8',';
			}
			return ret+1;
		}
	}
	else
	{
		if constexpr(std::same_as<char_type,char>)
			*str = static_cast<char_type>(value+'0');
		else if constexpr(std::same_as<char_type,wchar_t>)
			*str = static_cast<char_type>(value+L'0');
		else
			*str = static_cast<char_type>(value+u8'0');
		return 1;
	}
}


namespace transparent
{
template<char8_t start=0,::fast_io::freestanding::random_access_iterator Iter,my_unsigned_integral U>
requires (start==0)
inline constexpr std::size_t output_unsigned(Iter str,U value)
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	std::size_t len{chars_len<10,true>(value)};
	str+=len-1;
	for(std::size_t i{};i!=len;++i)
	{
		U const temp(value/10);
		char_type const res(value%10);
		*str=res;
		value=temp;
		--str;
	}
	return len;
}
}

template<::fast_io::freestanding::contiguous_iterator Iter,my_unsigned_integral U>
inline constexpr std::size_t fp_output_unsigned_trans(Iter iter,U i)
{
	if(std::is_constant_evaluated())
	{
		return transparent::output_unsigned<0>(iter,static_cast<std::remove_cvref_t<U>>(i));
	}
	else
	{

		namespace algo_decision = 
#ifdef __OPTIMIZE_SIZE__
			transparent;
#elif defined(FAST_IO_OPTIMIZE_TIME)
			jiaendu::fp;
#else
			twodigits::fp;
#endif
		return algo_decision::output_unsigned<0>(iter,static_cast<std::remove_cvref_t<U>>(i));
	}
}

template<::fast_io::freestanding::contiguous_iterator Iter,my_unsigned_integral U>
inline constexpr void fp_output_two_digits(Iter iter,U i)
{
#ifdef __OPTIMIZE_SIZE__
	std::uint32_t u(i/10);
	std::uint32_t v(i%10);
	*iter=u;
	iter[1]=v;
#elif defined(FAST_IO_OPTIMIZE_TIME)
	my_copy_n(jiaendu::static_tables<::fast_io::freestanding::iter_value_t<Iter>>::table2[i].data(),2,iter);
#else
	constexpr auto tb(get_shared_inline_constexpr_base_table<::fast_io::freestanding::iter_value_t<Iter>,10,false>().data());
	my_copy_n(tb[i].data(),2,iter);
#endif
}

}

}