#pragma once

/*
Algorithm: JEAIII
Author: jeaiii
*/

namespace fast_io::details::jeaiii
{

template<std::integral char_type>
inline constexpr void jeaiii_w(char_type* iter,std::uint_fast32_t u) noexcept
{
	constexpr
		auto const *digitstb{digits_table<char_type,10,false>};
	constexpr
		::std::size_t tocopybytes{sizeof(char_type)*2u};
	::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+(u<<1),tocopybytes);
}

template<std::size_t n,std::integral char_type>
inline constexpr std::uint_least64_t jeaiii_a(char_type* iter,std::uint_least32_t u) noexcept
{
	constexpr std::uint_least64_t one{1};
	constexpr std::uint_least64_t v{n/5*n*53/16};
	constexpr std::uint_least64_t constant{(one<<(32+v))/
		::fast_io::details::compile_pow10<std::uint_least64_t,n>+1+n/6-n/8};
	std::uint_least64_t t{constant*u};
	t>>=v;
	constexpr std::uint_least64_t add_factor{n/6*4};
	if constexpr(add_factor!=0u)
		t+=add_factor;
	jeaiii_w(iter,static_cast<std::uint_fast32_t>(t>>32u));
	return t;
}

template<std::size_t n,std::integral char_type>
inline constexpr void jeaiii_s(char_type* iter,std::uint_least64_t t) noexcept
{
	constexpr std::uint_least64_t ten{10u};
	iter[n]=::fast_io::char_literal_add<char_type>((ten*static_cast<std::uint_least32_t>(t))>>32u);
}

template<std::size_t n,bool last=false,std::integral char_type>
inline constexpr auto jeaiii_d(char_type* iter,std::uint_least64_t t) noexcept
{
	constexpr std::uint_least64_t hundred{100u};
	jeaiii_w(iter+n,static_cast<std::uint_least32_t>((t=hundred*static_cast<std::uint_least32_t>(t))>>32));
	if constexpr(!last)
		return t;
}


template<std::size_t n,std::integral char_type>
inline constexpr void jeaiii_c(char_type* iter,std::uint_least32_t u) noexcept
{
	if constexpr(n==0)
	{
		*iter=::fast_io::char_literal_add<char_type>(u);
	}
	else if constexpr(n==1)
	{
		jeaiii_w(iter,u);
	}
	else if constexpr(n==2)
	{
		jeaiii_s<2>(iter,jeaiii_a<1>(iter,u));
	}
	else if constexpr(n==3)
	{
		jeaiii_d<2,true>(iter,jeaiii_a<2>(iter,u));
	}
	else if constexpr(n==4)
	{
		jeaiii_s<4>(iter,jeaiii_d<2>(iter,jeaiii_a<3>(iter,u)));
	}
	else if constexpr(n==5)
	{
		jeaiii_d<4,true>(iter,jeaiii_d<2>(iter,jeaiii_a<4>(iter,u)));
	}
	else if constexpr(n==6)
	{
		jeaiii_s<6>(iter,jeaiii_d<4>(iter,jeaiii_d<2>(iter,jeaiii_a<5>(iter,u))));
	}
	else if constexpr(n==7)
	{
		jeaiii_d<6,true>(iter,jeaiii_d<4>(iter,jeaiii_d<2>(iter,jeaiii_a<6>(iter,u))));
	}
	else if constexpr(n==8)
	{
		std::uint_least64_t t{jeaiii_a<7>(iter,u)};
		t=jeaiii_d<2>(iter,t);
		t=jeaiii_d<4>(iter,t);
		t=jeaiii_d<6>(iter,t);
		jeaiii_s<8>(iter,t);
	}
	else if constexpr(n==9)
	{
		std::uint_least64_t t{jeaiii_a<8>(iter,u)};
		t=jeaiii_d<2>(iter,t);
		t=jeaiii_d<4>(iter,t);
		t=jeaiii_d<6>(iter,t);
		jeaiii_d<8,true>(iter,t);
	}
}

template<std::size_t n,std::integral char_type>
inline constexpr char_type* jeaiii_f(char_type* iter,std::uint_least32_t u) noexcept
{
	constexpr std::size_t np1{n+1};
	jeaiii_c<n>(iter,u);
	return iter+np1;
}

template<std::size_t left,std::size_t right,std::integral char_type>
inline constexpr char_type* jeaiii_tree(char_type* iter,std::uint_least32_t u) noexcept
{
	static_assert(left<=right);
/*
binary search tree
*/
#if 0
	if constexpr(left==0&&right==9)
	{
		if(u<::fast_io::details::compile_pow10<std::uint_least32_t,2>)
		{
			if(u<::fast_io::details::compile_pow10<std::uint_least32_t,1>)
			{
				*iter=::fast_io::char_literal_add<char_type>(u);
				return iter+1;
			}
			else
			{
				return jeaiii_f<1>(iter,u);
			}
		}
		else if(u<::fast_io::details::compile_pow10<std::uint_least32_t,6>)
		{
			if(u<::fast_io::details::compile_pow10<std::uint_least32_t,4>)
			{
				if(u<::fast_io::details::compile_pow10<std::uint_least32_t,3>)
				{
					return jeaiii_f<2>(iter,u);
				}
				else
				{
					return jeaiii_f<3>(iter,u);
				}
			}
			else if(u<::fast_io::details::compile_pow10<std::uint_least32_t,5>)
			{
				return jeaiii_f<4>(iter,u);
			}
			else
			{
				return jeaiii_f<5>(iter,u);
			}
		}
		else if(u<::fast_io::details::compile_pow10<std::uint_least32_t,8>)
		{
			if(u<::fast_io::details::compile_pow10<std::uint_least32_t,7>)
			{
				return jeaiii_f<6>(iter,u);
			}
			else
			{
				return jeaiii_f<7>(iter,u);
			}
		}
		else
		{
			if(u<::fast_io::details::compile_pow10<std::uint_least32_t,9>)
			{
				return jeaiii_f<8>(iter,u);
			}
			else
			{
				return jeaiii_f<9>(iter,u);
			}
		}
	}
	else
#endif
	{
		if constexpr(left==right)
		{
			return jeaiii_f<right>(iter,u);
		}
		else if constexpr(left+1==right)
		{
			if(u<(::fast_io::details::compile_pow10<std::uint_least32_t,right>))
			{
				return jeaiii_f<left>(iter,u);
			}
			else
			{
				return jeaiii_f<right>(iter,u);
			}
		}
		else if constexpr(left+2==right)
		{
			if(u<(::fast_io::details::compile_pow10<std::uint_least32_t,left+1>))
			{
				return jeaiii_f<left>(iter,u);
			}
			else
			{
				return jeaiii_tree<left+1,right>(iter,u);
			}
		}
		else if constexpr(left==0)
		{
			if(u<100u)
			{
				return jeaiii_tree<0,1>(iter,u);
			}
			else
			{
				return jeaiii_tree<2,right>(iter,u);
			}
		}
		else
		{
			constexpr std::size_t middle{(left+right)/2};
			if(u<(::fast_io::details::compile_pow10<std::uint_least32_t,middle+1>))
			{
				return jeaiii_tree<left,middle>(iter,u);
			}
			else
			{
				return jeaiii_tree<middle+1,right>(iter,u);
			}
		}
	}
}

template<bool ryu_mode=false,bool recursive=false,std::integral char_type,::fast_io::details::my_unsigned_integral U>
inline constexpr char_type* jeaiii_main(char_type* iter,U n) noexcept
{
	if constexpr(sizeof(U)>sizeof(std::uint_least64_t)&&sizeof(U)==16)//__uint128_t
	{
		if(static_cast<std::uint_least64_t>(n>>64u)==0)
		{
			return jeaiii_main(iter,static_cast<std::uint_least64_t>(n));
		}
		constexpr std::uint_least64_t divisor{10000000000000000000ull};
		U a{n/divisor};
		std::uint_least64_t u{static_cast<std::uint_least64_t>(n%divisor)};
		std::uint_least64_t alow{static_cast<std::uint_least64_t>(a)};
		if constexpr(ryu_mode)
		{
			iter=jeaiii_main(iter,static_cast<std::uint_least64_t>(alow));
		}
		else
		{
			if(a!=static_cast<U>(alow))
			{
				std::uint_least32_t v{static_cast<std::uint_least32_t>(a/divisor)};
				std::uint_least64_t m{static_cast<std::uint_least64_t>(a%divisor)};
				jeaiii_c<0>(iter,v);
				++iter;
				alow=m;
				iter=jeaiii_main<false,true,char_type>(iter,static_cast<std::uint_least64_t>(alow));
			}
			else
			{
				iter=jeaiii_main(iter,static_cast<std::uint_least64_t>(alow));
			}
		}
		return jeaiii_main<false,true,char_type>(iter,static_cast<std::uint_least64_t>(u));
	}
	else if constexpr(sizeof(U)==sizeof(std::uint_least64_t))
	{
		constexpr std::uint_least32_t divisor{1000000000u};
		if constexpr(recursive)
		{
			std::uint_least64_t high{n/divisor};
			std::uint_least32_t low{static_cast<std::uint_least32_t>(n%divisor)};
			std::uint_least32_t high_first{static_cast<std::uint_least32_t>(high/divisor)};
			std::uint_least32_t high_low{static_cast<std::uint_least32_t>(high%divisor)};
			jeaiii_c<0>(iter,high_first);
			++iter;
			iter=jeaiii_f<8>(jeaiii_f<8>(iter,high_low),low);
		}
		else
		{
			if(static_cast<std::uint_least32_t>(n>>32u)==0)
			{
				return jeaiii_tree<0,9>(iter,static_cast<std::uint_least32_t>(n));
			}
			std::uint_least64_t a{n/divisor};
			std::uint_least32_t u{static_cast<std::uint_least32_t>(n%divisor)};
			std::uint_least32_t alow{static_cast<std::uint_least32_t>(a)};
			if constexpr(ryu_mode)
			{
				iter=jeaiii_tree<0,7>(iter,alow);
			}
			else
			{
				if(a!=static_cast<std::uint_least64_t>(alow))
				{
					std::uint_least32_t v{static_cast<std::uint_least32_t>(a/divisor)};
					alow=static_cast<std::uint_least32_t>(a%divisor);
					if(v<10u)
					{
						jeaiii_c<0>(iter,v);
						++iter;
					}
					else
					{
						jeaiii_w(iter,v);
						iter+=2;
					}
					iter=jeaiii_f<8>(iter,alow);
				}
				else
				{
					iter=jeaiii_tree<0,9>(iter,alow);
				}
			}
			iter=jeaiii_f<8>(iter,u);
		}
		return iter;
	}
	else
	{
		static_assert(!recursive);
		if constexpr(ryu_mode)
		{
			return jeaiii_tree<0,8>(iter,n);
		}
		else
		{
			return jeaiii_tree<0,9>(iter,n);
		}
	}
}

template<std::size_t n,std::integral char_type>
inline constexpr void jeaiii_hash(char_type* iter,std::uint_least32_t u,std::uint_least32_t len) noexcept
{
	if constexpr(n==7)
	{
		switch(len)
		{
			case 1:
			{
				jeaiii_c<0>(iter,u);
				return;
			}
			case 2:
			{
				jeaiii_c<1>(iter,u);
				return;
			}
			case 3:
			{
				jeaiii_c<2>(iter,u);
				return;
			}
			case 4:
			{
				jeaiii_c<3>(iter,u);
				return;
			}
			case 5:
			{
				jeaiii_c<4>(iter,u);
				return;
			}
			case 6:
			{
				jeaiii_c<5>(iter,u);
				return;
			}
			case 7:
			{
				jeaiii_c<6>(iter,u);
				return;
			}
			default:
			{
				jeaiii_c<7>(iter,u);
				return;
			}
		}
	}
	else if constexpr(n==8)
	{
		switch(len)
		{
			case 1:
			{
				jeaiii_c<0>(iter,u);
				return;
			}
			case 2:
			{
				jeaiii_c<1>(iter,u);
				return;
			}
			case 3:
			{
				jeaiii_c<2>(iter,u);
				return;
			}
			case 4:
			{
				jeaiii_c<3>(iter,u);
				return;
			}
			case 5:
			{
				jeaiii_c<4>(iter,u);
				return;
			}
			case 6:
			{
				jeaiii_c<5>(iter,u);
				return;
			}
			case 7:
			{
				jeaiii_c<6>(iter,u);
				return;
			}
			case 8:
			{
				jeaiii_c<7>(iter,u);
				return;
			}
			default:
			{
				jeaiii_c<8>(iter,u);
				return;
			}
		}
	}
	else if constexpr(n==9)
	{
		switch(len)
		{
			case 1:
			{
				jeaiii_c<0>(iter,u);
				return;
			}
			case 2:
			{
				jeaiii_c<1>(iter,u);
				return;
			}
			case 3:
			{
				jeaiii_c<2>(iter,u);
				return;
			}
			case 4:
			{
				jeaiii_c<3>(iter,u);
				return;
			}
			case 5:
			{
				jeaiii_c<4>(iter,u);
				return;
			}
			case 6:
			{
				jeaiii_c<5>(iter,u);
				return;
			}
			case 7:
			{
				jeaiii_c<6>(iter,u);
				return;
			}
			case 8:
			{
				jeaiii_c<7>(iter,u);
				return;
			}
			case 9:
			{
				jeaiii_c<8>(iter,u);
				return;
			}
			default:
			{
				jeaiii_c<9>(iter,u);
				return;
			}
		}
	}
	else
	{
		static_assert(n==SIZE_MAX,"no supported");
	}
}

template<bool ryu_mode=false,bool recursive=false,std::integral char_type,::fast_io::details::my_unsigned_integral U>
inline constexpr void jeaiii_main_len(char_type* iter,U n,std::uint_least32_t len) noexcept
{
	if constexpr(sizeof(U)>sizeof(std::uint_least64_t)&&sizeof(U)==16)//__uint128_t
	{
		if(static_cast<std::uint_least64_t>(n>>64u)==0)
		{
			return jeaiii_main_len(iter,static_cast<std::uint_least64_t>(n),len);
		}
		constexpr std::uint_least32_t full_length{19u};
		constexpr std::uint_least64_t divisor{10000000000000000000ull};
		U a{n/divisor};
		std::uint_least64_t u{static_cast<std::uint_least64_t>(n%divisor)};
		std::uint_least64_t alow{static_cast<std::uint_least64_t>(a)};
		if constexpr(ryu_mode)
		{
			iter=jeaiii_main_len(iter,static_cast<std::uint_least64_t>(alow),len-full_length);
		}
		else
		{
			if(a!=static_cast<U>(alow))
			{
				std::uint_least32_t v{static_cast<std::uint_least32_t>(a/divisor)};
				std::uint_least64_t m{static_cast<std::uint_least64_t>(a%divisor)};
				jeaiii_c<0>(iter,v);
				++iter;
				alow=m;
				jeaiii_main_len<false,true>(iter,static_cast<std::uint_least64_t>(alow),full_length);
				iter+=full_length;
			}
			else
			{
				std::uint_least32_t len_sub{len-full_length};
				jeaiii_main_len(iter,static_cast<std::uint_least64_t>(alow),len_sub);
				iter+=len_sub;
			}
		}
		jeaiii_main_len<false,true>(iter,static_cast<std::uint_least64_t>(u),full_length);
	}
	else if constexpr(sizeof(U)==sizeof(std::uint_least64_t))
	{
		constexpr std::uint_least32_t full_length{9u};
		constexpr std::uint_least32_t divisor{1000000000u};
		if constexpr(recursive)
		{
			std::uint_least64_t high{n/divisor};
			std::uint_least32_t low{static_cast<std::uint_least32_t>(n%divisor)};
			std::uint_least32_t high_first{static_cast<std::uint_least32_t>(high/divisor)};
			std::uint_least32_t high_low{static_cast<std::uint_least32_t>(high%divisor)};
			jeaiii_c<0>(iter,high_first);
			++iter;
			iter=jeaiii_f<8>(jeaiii_f<8>(iter,high_low),low);
		}
		else
		{
			if(static_cast<std::uint_least32_t>(n>>32u)==0)
			{
				return jeaiii_hash<9>(iter,static_cast<std::uint_least32_t>(n),len);
			}
			std::uint_least64_t a{n/divisor};
			std::uint_least32_t u{static_cast<std::uint_least32_t>(n%divisor)};
			std::uint_least32_t alow{static_cast<std::uint_least32_t>(a)};
			if constexpr(ryu_mode)
			{
				std::uint_least32_t len_sub{len-full_length};
				jeaiii_hash<7>(iter,alow,len_sub);
				iter+=len_sub;
			}
			else
			{
				if(a!=static_cast<std::uint_least64_t>(alow))
				{
					std::uint_least32_t v{static_cast<std::uint_least32_t>(a/divisor)};
					alow=static_cast<std::uint_least32_t>(a%divisor);
					if(v<10u)
					{
						jeaiii_c<0>(iter,v);
						++iter;
					}
					else
					{
						jeaiii_w(iter,v);
						iter+=2;
					}
					jeaiii_f<8>(iter,alow);
					iter+=full_length;
				}
				else
				{
					std::uint_least32_t len_sub{len-full_length};
					jeaiii_hash<9>(iter,alow,len_sub);
					iter+=len_sub;
				}
			}
			jeaiii_f<8>(iter,u);
		}
	}
	else
	{
		static_assert(!recursive);
		if constexpr(ryu_mode)
		{
			jeaiii_hash<8>(iter,n,len);
		}
		else
		{
			jeaiii_hash<9>(iter,n,len);
		}
	}
}

}
