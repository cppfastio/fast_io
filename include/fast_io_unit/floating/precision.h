#pragma once

namespace fast_io
{
namespace details::precisionfloat
{

template<std::unsigned_integral T>
requires (sizeof(T)==sizeof(std::uint_least16_t)||sizeof(T)==sizeof(std::uint_least32_t)||sizeof(T)==sizeof(std::uint_least64_t))
inline constexpr T factor_mulassign{sizeof(std::uint_least16_t)==sizeof(T)?100u:(sizeof(std::uint_least32_t)==sizeof(T)?10000u:1000000000ULL)};

template<std::unsigned_integral T>
requires (sizeof(T)==sizeof(std::uint_least16_t)||sizeof(T)==sizeof(std::uint_least32_t)||sizeof(T)==sizeof(std::uint_least64_t))
inline constexpr std::uint_least32_t factor_mulassigndigits{sizeof(std::uint_least16_t)==sizeof(T)?2:(sizeof(std::uint_least32_t)==sizeof(T)?4:9)};

template<std::unsigned_integral T>
inline constexpr std::size_t mulassign(T* data,
				T* __restrict temp,
				std::size_t na,
				T const* bdatai,
				std::size_t nb) noexcept
{
	std::size_t nab{na+nb};
	for(std::size_t i{};i!=nab;++i)
	{
		temp[i]=0;
	}
	for(std::size_t i{};i!=na;++i)
	{
		auto ini{data[i]};
		auto tempi{temp+i};
		for(std::size_t j{};j!=nb;++j)
		{
			tempi[j]+=ini*bdatai[j];
		}
	}
	T carry{};
	for(std::size_t i{};i!=nab;++i)
	{
		T outi{temp[i]+carry};
		carry=0;
		if(factor_mulassign<T><=outi)
		{
			T t{outi/factor_mulassign<T>};
			T mi{outi%factor_mulassign<T>};
			carry=t;
			outi=mi;
		}
		data[i]=outi;
	}
	if(carry)
	{
		data[nab]=carry;
		++nab;
	}
	for(;nab&&!data[nab-1];--nab);
	return nab;
}

template<std::unsigned_integral T>
inline constexpr std::size_t sqrassign(T* __restrict data,
				T* __restrict temp,std::size_t n) noexcept
{
	return mulassign(data,temp,n,data,n);
}

template<std::unsigned_integral T>
inline constexpr std::size_t mulv(T* data,std::size_t n,std::uint_least32_t constant) noexcept
{
	T carry{};
	for(std::size_t i{};i!=n;++i)
	{
		T temp{data[i]*constant+carry};
		carry=0;
		if(factor_mulassign<T><=temp)
		{
			T t{temp/factor_mulassign<T>};
			T mi{temp%factor_mulassign<T>};
			carry=t;
			temp=mi;
		}
		data[i]=temp;
	}
	if(carry)
	{
		data[n]=carry;
		++n;
	}
	return n;
}

template<std::unsigned_integral T>
inline constexpr std::size_t pow_full(T* __restrict res,
		T* __restrict temp,
		T* __restrict base,std::uint_least32_t exponent) noexcept
{
	*res=1;
	std::size_t nbase{1};
	std::size_t nres{1};
	for(;exponent;)
	{
		if(exponent&1)
		{
			if(nres==1&&*res==1)
			{
				for(std::size_t i{};i!=nbase;++i)
				{
					res[i]=base[i];
				}
				nres=nbase;
			}
			else
			{
				nres=mulassign(res,temp,nres,base,nbase);
			}
		}
		exponent>>=1;
		nbase=sqrassign(base,temp,nbase);
	}
	return nres;
}

template<std::unsigned_integral T>
inline constexpr std::size_t precisionincrement(T* res,std::size_t n) noexcept
{
	for(std::size_t i{};i!=n;++i)
	{
		if((++res[i])!=factor_mulassign<T>)
		{
			return n;
		}
	}
	return n+1;
}
template<std::size_t left,std::size_t right>
inline constexpr std::uint_least32_t chars_len_precision_tree(std::uint_least32_t u) noexcept
{
	if constexpr(left==right)
	{
		return chars_len_precision_tree<left,right>(u);
	}
	else if constexpr(left+1==right)
	{
		if(u<(::fast_io::details::compile_pow10<std::uint_least32_t,right>))
		{
			return left;
		}
		else
		{
			return right;
		}
	}
	else if constexpr(left+2==right)
	{
		if(u<(::fast_io::details::compile_pow10<std::uint_least32_t,left+1>))
		{
			return left;
		}
		else
		{
			return chars_len_precision_tree<left+1,right>(u);
		}
	}
	else if constexpr(left==0)
	{
		if(u<100u)
		{
			return chars_len_precision_tree<0,1>(u);
		}
		else
		{
			return chars_len_precision_tree<2,right>(u);
		}
	}
	else
	{
		constexpr std::size_t middle{(left+right)/2};
		if(u<(::fast_io::details::compile_pow10<std::uint_least32_t,middle+1>))
		{
			return chars_len_precision_tree<left,middle>(u);
		}
		else
		{
			return chars_len_precision_tree<middle+1,right>(u);
		}
	}
}

inline constexpr std::size_t chars_len_precision(std::size_t u) noexcept
{
	return chars_len_precision_tree<0,factor_mulassigndigits<std::size_t>-1>(static_cast<std::uint_least32_t>(u))+1;
}

template<std::unsigned_integral T>
inline constexpr std::size_t total_length(T const* data,std::size_t resn1) noexcept
{
	if(resn1==0)
	{
		return 0;
	}
	std::size_t ndigits{(resn1-1)*factor_mulassigndigits<std::size_t>};
	ndigits+=chars_len_precision(data[resn1-1]);
	return ndigits;
}

template<std::floating_point flt>
inline constexpr
	::std::uint_least32_t requiredbuffersizevamx{
		((iec559_traits<flt>::e10max+iec559_traits<flt>::m10digits)+100)/factor_mulassigndigits<std::size_t>+1};

template<typename flt>
inline constexpr std::size_t precision_m2_e2_common(std::size_t* res,typename iec559_traits<flt>::mantissa_type m2,std::int_least32_t e2) noexcept
{
	using trait = iec559_traits<flt>;
	using mantissa_type = typename iec559_traits<flt>::mantissa_type;


	constexpr
		::std::uint_least32_t m10digits{iec559_traits<flt>::m10digits};
	constexpr
		::std::uint_least32_t digitsperunsigned{m10digits/factor_mulassigndigits<std::size_t>};
	constexpr
		::std::uint_least32_t digitsmod{m10digits%factor_mulassigndigits<std::size_t>};
	constexpr
		::std::uint_least32_t requiredbuffersize{digitsperunsigned+(digitsmod!=0)};
	::std::size_t tempbuffer[requiredbuffersizevamx<flt>],
		base[requiredbuffersizevamx<flt>];
	::std::size_t resn1{1};
	std::uint_least32_t e2abs{};
	if(e2<0)
	{
		*base=5;
		e2abs=static_cast<std::uint_least32_t>(-e2);
	}
	else
	{
		*base=2;
		e2abs=e2;
	}
	resn1=pow_full(res,tempbuffer,base,e2abs);
	::std::size_t basei{};
	if constexpr(requiredbuffersize==1)
	{
		*base=static_cast<std::size_t>(m2);
		basei=1;
	}
	else
	{
		for(;m2;++basei)
		{
			::std::size_t temp{m2%factor_mulassign<std::size_t>};
			m2/=factor_mulassign<std::size_t>;
			base[basei]=temp;
		}
	}
	return mulassign(res,tempbuffer,resn1,base,basei);
}
#if 0
template<std::unsigned_integral T>
inline constexpr std::uint_fast8_t precisionindex(std::size_t* res,std::size_t resn,T val,std::size_t pos) noexcept
{
	return 0;
}
#endif
template<std::integral char_type,
bool showpos,
bool uppercase,
bool uppercase_e,
bool comma,
::fast_io::manipulators::floating_format mt,
typename flt>
inline constexpr char_type* precision_m2_e2(typename iec559_traits<flt>::mantissa_type m2,std::int_least32_t e2,char_type* iter,std::size_t precision) noexcept
{
	using trait = iec559_traits<flt>;
	using mantissa_type = typename iec559_traits<flt>::mantissa_type;
	constexpr std::size_t mbits{trait::mbits};
	constexpr std::size_t ebits{trait::ebits};
	constexpr std::uint_least32_t bias{(static_cast<std::uint_least32_t>(1<<ebits)>>1)-1};
	constexpr std::int_least32_t exponent_bias{bias+mbits};
	constexpr mantissa_type mflags{static_cast<mantissa_type>(static_cast<mantissa_type>(1)<<mbits)};
	if(e2==0)[[unlikely]]
	{
		constexpr std::int_least32_t e2bias{1-static_cast<std::int_least32_t>(exponent_bias)};
		e2=e2bias;
	}
	else
	{
		e2-=exponent_bias;
		m2|=mflags;
		if(e2==0)
		{
			return iter;
		}
	}
	::std::size_t res[requiredbuffersizevamx<flt>];
	::std::size_t n{precision_m2_e2_common<flt>(res,m2,e2)};

	::std::size_t const length{total_length(res,n)};
	::std::int_least32_t const lengthl32{static_cast<std::int_least32_t>(static_cast<std::uint_least32_t>(length))};
	::std::int_least32_t const lengthpluse2{lengthl32+e2};
	if constexpr(mt==::fast_io::manipulators::floating_format::scientific)
	{
		if(lengthpluse2<0)
		{
			
		}
		else
		{

		}
	}
	else if constexpr(mt==::fast_io::manipulators::floating_format::generic)
	{
		
	}
	else if constexpr(mt==::fast_io::manipulators::floating_format::fixed)
	{
		
	}
	return iter;
}

template<std::integral char_type,
bool showpos=false,
bool uppercase=false,
bool uppercase_e=false,
bool comma=false,
::fast_io::manipulators::floating_format mt=::fast_io::manipulators::floating_format::general,
std::floating_point flttype>
inline constexpr char_type* precision_flt(char_type* iter,flttype f,std::size_t precision) noexcept
{
	auto [mantissa,exponent,sign] = get_punned_result(f);
	iter=print_rsv_fp_sign_impl<showpos>(iter,sign);
	using trait = iec559_traits<flttype>;
	using mantissa_type = typename trait::mantissa_type;
	constexpr std::size_t ebits{trait::ebits};
	constexpr mantissa_type exponent_mask{(static_cast<mantissa_type>(1)<<ebits)-1};
	constexpr std::uint_least32_t exponent_mask_u32{static_cast<std::uint_least32_t>(exponent_mask)};
	if(exponent==exponent_mask_u32)
		return prsv_fp_nan_impl<uppercase>(iter,mantissa!=0u);
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
	return precision_m2_e2<char_type,showpos,uppercase,uppercase_e,comma,mt,flttype>(mantissa,exponent,iter,precision);
}

#if 0
template<typename flt,std::integral char_type>
inline constexpr char_type* precision_m2_e2(typename iec559_traits<flt>::mantissa_type m2,std::int_least32_t e2,char_type* iter) noexcept
{
	constexpr
		::std::uint_least32_t vamx{(iec559_traits<flt>::e10max+iec559_traits<flt>::m10digits)+100};
	constexpr
		::std::uint_least32_t requiredbuffersizevamx{vamx/factor_mulassigndigits<std::size_t>+1};
	::std::size_t res[requiredbuffersizevamx];
}
#endif
}
}