#pragma once

namespace fast_io::details::jiaendu::fp
{

//this is only use for floating point formatting
template<char8_t start=u8'0',::fast_io::freestanding::contiguous_iterator Iter,my_unsigned_integral U>
requires (sizeof(U)==4||sizeof(U)==8||sizeof(U)==16)
inline std::size_t output_unsigned(Iter str,U value)
{
	using ch_type = std::remove_cvref_t<decltype(*str)>;
	constexpr std::size_t bytes4{4*sizeof(ch_type)};
	if constexpr(sizeof(U)==16)
	{
		if(value>=10000000000000000000ULL)
		{
			auto v2{value/10000000000000000000ULL};
			std::uint64_t remain{static_cast<std::uint64_t>(value-v2*10000000000000000000ULL)};
			auto offset{output_unsigned(str,static_cast<std::uint64_t>(v2))};
			output_unsigned_partial(str+offset,remain);
			return offset+19;
		}
		else
			return fast_io::details::jiaendu::output_unsigned(str,static_cast<std::uint64_t>(value));
	}
	else if constexpr(sizeof(U)==8)
	{
		if (value>=10000000000000000ULL)
		{
			std::uint64_t v4{value/10000000000000000ULL};
			std::uint64_t remains3{value-v4*10000000000000000ULL};
			std::uint64_t temp3(remains3/1000000000000ULL);
			std::uint64_t remains2
			{remains3-temp3*1000000000000ULL};
			remains3=temp3;
			std::uint64_t temp2{remains2};
			std::uint64_t remains0{remains2/10000};
			std::uint64_t remains1{remains2/100000000};
			remains2 = remains1;
			remains1 = remains0 - remains1*10000;
			remains0 = temp2 - remains0*10000;
			*str=static_cast<char8_t>(v4)+start;		
			::fast_io::details::my_memcpy(str + 1,static_tables<ch_type,start>::table4[remains3].data(),bytes4);
			::fast_io::details::my_memcpy(str + 5,static_tables<ch_type,start>::table4[remains2].data(),bytes4);
			::fast_io::details::my_memcpy(str + 9,static_tables<ch_type,start>::table4[remains1].data(),bytes4);
			::fast_io::details::my_memcpy(str + 13,static_tables<ch_type,start>::table4[remains0].data(),bytes4);
			return 17;
		}
		else if (value>=1000000000000ULL)
		{
			std::uint64_t v3{value/1000000000000ULL};
			std::uint64_t remains2
			{value-v3*1000000000000ULL};
			std::uint64_t temp2{remains2};
			std::uint64_t remains0{remains2/10000};
			std::uint64_t remains1{remains2/100000000};
			remains2 = remains1;
			remains1 = remains0 - remains1*10000;
			remains0 = temp2 - remains0*10000;
			if (v3 >= 1000)
			{
				::fast_io::details::my_memcpy(str,static_tables<ch_type,start>::table4[v3].data(),bytes4);
				::fast_io::details::my_memcpy(str += 4,static_tables<ch_type,start>::table4[remains2].data(),bytes4);
				::fast_io::details::my_memcpy(str += 4,static_tables<ch_type,start>::table4[remains1].data(),bytes4);
				::fast_io::details::my_memcpy(str += 4,static_tables<ch_type,start>::table4[remains0].data(),bytes4);
				return 16;
			}
			else
			{
				auto wstr(str);
				::fast_io::details::my_memcpy(wstr,static_tables<ch_type,start>::table5[v3].data(),bytes4);
				wstr += static_offset<>::offset[v3];
				::fast_io::details::my_memcpy(wstr,static_tables<ch_type,start>::table4[remains2].data(),bytes4);
				::fast_io::details::my_memcpy(wstr += 4,static_tables<ch_type,start>::table4[remains1].data(),bytes4);
				::fast_io::details::my_memcpy(wstr += 4,static_tables<ch_type,start>::table4[remains0].data(),bytes4);
				wstr += 4;
				return static_cast<std::size_t>(wstr-str);
			}
		}
		else if (value >= 100000000)
		{
			std::uint64_t remains0{value/10000};// value/1,0000
			std::uint64_t remains1{value/100000000};// value/1,0000,0000
			auto v2(remains1);
			remains1 = remains0 - remains1*10000;
			remains0 = value - remains0*10000;
			if (v2 >= 1000)
			{
				::fast_io::details::my_memcpy(str,static_tables<ch_type,start>::table4[v2].data(),bytes4);
				::fast_io::details::my_memcpy(str += 4,static_tables<ch_type,start>::table4[remains1].data(),bytes4);
				::fast_io::details::my_memcpy(str += 4,static_tables<ch_type,start>::table4[remains0].data(),bytes4);
				return 12;
			}
			else
			{
				auto wstr(str);
				::fast_io::details::my_memcpy(wstr,static_tables<ch_type,start>::table5[v2].data(),bytes4);
				wstr += static_offset<>::offset[v2];
				::fast_io::details::my_memcpy(wstr,static_tables<ch_type,start>::table4[remains1].data(),bytes4);
				::fast_io::details::my_memcpy(wstr += 4,static_tables<ch_type,start>::table4[remains0].data(),bytes4);
				wstr += 4;
				return static_cast<std::size_t>(wstr-str);
			}
		}
		else if (value >= 10000)
		{
			std::uint64_t const v2{
				(static_cast<std::uint64_t>(value) *
				static_cast<std::uint64_t>(3518437209)) >> 45};
			std::uint64_t const remains0{value - v2 * 10000};
			if (v2 >= 1000)
			{
				::fast_io::details::my_memcpy(str,static_tables<ch_type,start>::table4[v2].data(),bytes4);
				str += 4;
				::fast_io::details::my_memcpy(str,static_tables<ch_type,start>::table4[remains0].data(),bytes4);
				return 8;
			}
			else
			{
				auto wstr(str);
				::fast_io::details::my_memcpy(wstr,static_tables<ch_type,start>::table5[v2].data(),bytes4);
				wstr += static_offset<>::offset[v2];
				::fast_io::details::my_memcpy(wstr,static_tables<ch_type,start>::table4[remains0].data(),bytes4);
				wstr += 4;
				return static_cast<std::size_t>(wstr - str);
			}
		}
		else
		{
			if (value >= 1000)
			{
				::fast_io::details::my_memcpy(str,static_tables<ch_type,start>::table4[value].data(),bytes4);
				return 4;
			}
			else if (value >= 100)
			{
				::fast_io::details::my_memcpy(str,static_tables<ch_type,start>::table3[value].data(),3*sizeof(ch_type));
				return 3;
			}
			else if (value >= 10)
			{
				::fast_io::details::my_memcpy(str,static_tables<ch_type,start>::table2[value].data(),2*sizeof(ch_type));
				return 2;
			}
			else
			{
				*str = static_cast<ch_type>(static_cast<char8_t>(value)+start);
				return 1;
			}
		}
	}
	else
	{
		if (value >= 100000000)
		{
#if (_WIN64 || __x86_64__ || __ppc64__)
			std::uint64_t remains0{(static_cast<std::uint64_t>(value) *
			static_cast<std::uint64_t>(3518437209)) >> 45};
			std::uint64_t remains1{static_cast<std::uint64_t>(value) * 
			static_cast<std::uint64_t>((2882303762)) >> 58};
#else
			std::uint32_t remains0{value/10000};
			std::uint32_t remains1{value/100000000};
#endif
			auto v2(remains1);
			remains1 = remains0 - remains1*10000;
			remains0 = value - remains0*10000;
			*str = static_cast<char8_t>(v2)+start;
			::fast_io::details::my_memcpy(str + 1,static_tables<ch_type,start>::table4[remains1].data(),bytes4);
			::fast_io::details::my_memcpy(str + 5,static_tables<ch_type,start>::table4[remains0].data(),bytes4);
			return 9;
		}
		else if (value >= 10000)
		{
#if (_WIN64 || __x86_64__ || __ppc64__)
			std::uint64_t const v2{
				(static_cast<std::uint64_t>(value) *
				static_cast<std::uint64_t>(3518437209)) >> 45};
			std::uint64_t const remains0{value - v2 * 10000};
#else
			std::uint32_t const v2{value/10000};
			std::uint32_t const remains0{value - v2 * 10000};
#endif
			if (v2 >= 1000)
			{
				::fast_io::details::my_memcpy(str,static_tables<ch_type,start>::table4[v2].data(),bytes4);
				str += 4;
				::fast_io::details::my_memcpy(str,static_tables<ch_type,start>::table4[remains0].data(),bytes4);
				return 8;
			}
			else
			{
				auto wstr(str);
				::fast_io::details::my_memcpy(wstr,static_tables<ch_type,start>::table5[v2].data(),bytes4);
				wstr += static_offset<>::offset[v2];
				::fast_io::details::my_memcpy(wstr,static_tables<ch_type,start>::table4[remains0].data(),bytes4);
				wstr += 4;
				return static_cast<std::size_t>(wstr - str);
			}
		}
		else
		{
			if (value >= 1000)
			{
				::fast_io::details::my_memcpy(str,static_tables<ch_type,start>::table4[value].data(),bytes4);
				return 4;
			}
			else if (value >= 100)
			{
				::fast_io::details::my_memcpy(str,static_tables<ch_type,start>::table3[value].data(),3*sizeof(ch_type));
				return 3;
			}
			else if (value >= 10)
			{
				::fast_io::details::my_memcpy(str,static_tables<ch_type,start>::table2[value].data(),2*sizeof(ch_type));
				return 2;
			}
			else
			{
				*str = static_cast<char8_t>(value)+start;
				return 1;
			}
		}
	}
	
}

}