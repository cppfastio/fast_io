template<typename T>
inline constexpr T create_simd_vector_with_single_value(typename T::value_type value) noexcept
{
	::fast_io::freestanding::array<typename T::value_type,T::size()> arr;
	for(auto& e : arr)
	{
		e=value;
	}
	return __builtin_bit_cast(T,arr);
}

inline constexpr ::fast_io::intrinsics::simd_vector<char unsigned,16> create_simd16_vector(char unsigned value) noexcept
{
	return create_simd_vector_with_single_value<::fast_io::intrinsics::simd_vector<char unsigned,16>>(value);
}

template<std::integral char_type>
inline constexpr char_type const* skip_hexdigits(char_type const* first,char_type const* last) noexcept
{
	using ::fast_io::intrinsics::simd_vector;
	constexpr char8_t zero_constant{static_cast<char8_t>(char_literal_v<u8'0',char_type>)};

	constexpr char8_t A_constant{static_cast<char8_t>(char_literal_v<u8'A',char_type>)};

	constexpr char8_t a_constant{static_cast<char8_t>(char_literal_v<u8'a',char_type>)};

	constexpr simd_vector<char unsigned,16> zerovec{create_simd16_vector(zero_constant)};
	constexpr simd_vector<char unsigned,16> Avec{create_simd16_vector(A_constant)};
	constexpr simd_vector<char unsigned,16> avec{create_simd16_vector(a_constant)};

	constexpr simd_vector<char unsigned,16> tenvec{create_simd16_vector(10)};
	constexpr simd_vector<char unsigned,16> sixvec{create_simd16_vector(6)};
	
	for(simd_vector<char unsigned,16> data16;16<=last-first;first+=16)
	{
		data16.load(first);
		simd_vector<char unsigned,16> datamzero{data16-zerovec};
		simd_vector<char unsigned,16> datamA{data16-Avec};
		simd_vector<char unsigned,16> datama{data16-avec};
		simd_vector<char unsigned,16> datain09{datamzero<tenvec};
		simd_vector<char unsigned,16> datainAE{datamA<sixvec};
		simd_vector<char unsigned,16> datainae{datama<sixvec};

		simd_vector<char unsigned,16> maskvec{datain09+datainAE+datainae};
		std::uint_least16_t v{static_cast<std::uint_least16_t>(__builtin_ia32_pmovmskb128((::fast_io::intrinsics::x86_64_v16qi)maskvec.value))};		
		int digits{std::countr_one(v)};
		if(digits!=16)
		{
			return first+digits;
		}
	}
	return skip_digits<16>(first,last);
}

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__hot__)
[[__gnu__::__hot__]]
#endif
inline parse_result<char_type const*> sto_hex_simd16_impl(char_type const* buffer,char_type const* buffer_end,std::uint_least64_t &res) noexcept
{
	constexpr char8_t zero_constant{static_cast<char8_t>(char_literal_v<u8'0',char_type>)};

	constexpr char8_t A_constant{static_cast<char8_t>(char_literal_v<u8'A',char_type>)};

	constexpr char8_t a_constant{static_cast<char8_t>(char_literal_v<u8'a',char_type>)};
	
	
	using ::fast_io::intrinsics::simd_vector;

	simd_vector<char unsigned,16> data16;
	data16.load(buffer);

	constexpr simd_vector<char unsigned,16> zerovec{create_simd16_vector(zero_constant)};
	constexpr simd_vector<char unsigned,16> Avec{create_simd16_vector(A_constant)};
	constexpr simd_vector<char unsigned,16> avec{create_simd16_vector(a_constant)};
	

	constexpr simd_vector<char unsigned,16> tenvec{create_simd16_vector(10)};
	constexpr simd_vector<char unsigned,16> sixvec{create_simd16_vector(6)};
	
	simd_vector<char unsigned,16> datamzero{data16-zerovec};
	simd_vector<char unsigned,16> datamA{data16-Avec};
	simd_vector<char unsigned,16> datama{data16-avec};
	simd_vector<char unsigned,16> datain09{datamzero<tenvec};
	simd_vector<char unsigned,16> datainAE{datamA<sixvec};
	simd_vector<char unsigned,16> datainae{datama<sixvec};
	simd_vector<char unsigned,16> resdata{(datamzero&datain09)+(datamA&datainAE)+(datama&datainae)};

	simd_vector<char unsigned,16> maskvec{datain09+datainAE+datainae};
	std::uint_least16_t v{static_cast<std::uint_least16_t>(__builtin_ia32_pmovmskb128((::fast_io::intrinsics::x86_64_v16qi)maskvec.value))};
	
	int digits{std::countr_one(v)};
#if 0
	if(digits==0)
	{
		return {buffer,parse_code::invalid};
	}
#endif
#if __has_cpp_attribute(assume)
	[[assume(0<digits&&digits<16)]];
#endif
	simd_vector<char unsigned,16> shiftingvec;
	shiftingvec.load(sse_shift_table+digits);
	simd_vector<char unsigned,16> shifted{__builtin_shuffle(maskvec.value,shiftingvec.value)};

	simd_vector<std::uint_least16_t,8> v1;
	{
		constexpr simd_vector<std::uint_least16_t,8> temp{4,4,4,4,4,4,4,4};
		auto temp1{__builtin_convertvector(shifted.value,typename simd_vector<std::uint_least16_t,16>::vec_type)};
		v1=__builtin_shufflevector(temp1,temp1,0,2,4,6,8,10,12,14);
		simd_vector<std::uint_least16_t,8> position2{__builtin_bit_cast(simd_vector<std::uint_least16_t,8>,__builtin_shufflevector(temp1,temp1,1,3,5,7,9,11,13,15))};
		position2<<=temp;
		v1+=position2;
	}

	simd_vector<std::uint_least32_t,4> v2;
	{
		constexpr simd_vector<std::uint_least32_t,4> temp{8,8,8,8};
		auto temp1{__builtin_convertvector(v1.value,typename simd_vector<std::uint_least32_t,8>::vec_type)};

		v2=__builtin_shufflevector(temp1,temp1,0,2,4,6);
		simd_vector<std::uint_least32_t,4> position2{__builtin_shufflevector(temp1,temp1,1,3,5,7)};

		position2<<=temp;
		v2+=position2;
	}

	simd_vector<std::uint_least64_t,2> v3;
	{
		constexpr simd_vector<std::uint_least64_t,2> temp{16,16};
		auto temp1{__builtin_convertvector(v2.value,typename simd_vector<std::uint_least64_t,4>::vec_type)};

		v3=__builtin_bit_cast(simd_vector<std::uint_least64_t,2>,__builtin_shufflevector(temp1,temp1,0,2));
		simd_vector<std::uint_least64_t,2> position2{__builtin_bit_cast(simd_vector<std::uint_least64_t,2>,__builtin_shufflevector(temp1,temp1,1,3))};
		position2<<=temp;
		v3+=position2;
	}

	std::uint_least64_t result{(v3.front()<<32u)+v3.back()};
	if(digits==16)[[unlikely]]
	{
		
		auto first{buffer+16};
		auto none_digits_it{skip_hexdigits(first,buffer_end)};
		if(first!=none_digits_it)
		{
			return {none_digits_it,parse_code::overflow};
		}
	}
	res=result;
	return {buffer+digits,parse_code::ok};
}

