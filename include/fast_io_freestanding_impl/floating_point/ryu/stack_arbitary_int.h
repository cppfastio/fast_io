#pragma once

namespace fast_io::details::ryu
{

template<std::size_t buffer_size>
class stack_arbitary_int
{
public:
	::fast_io::freestanding::array<char8_t,buffer_size> digits;
	std::size_t position{};
	template<std::integral char_type>
	inline constexpr void push_back_unchecked(char_type ch) noexcept
	{
		digits[position]=static_cast<char8_t>(static_cast<std::make_unsigned_t<char_type>>(ch));
		++position;
	}
	inline constexpr auto data() noexcept
	{
		return digits.data();
	}
	inline constexpr auto data() const noexcept
	{
		return digits.data();
	}
	inline constexpr std::size_t size() const noexcept
	{
		return position;
	}
	inline constexpr bool full() const noexcept
	{
		return position==digits.size();
	}
	inline constexpr void final_zeros()
	{
		my_fill_n(digits.data()+position,digits.size()-position,static_cast<char8_t>(0));
		position=buffer_size;
	}
	inline constexpr void fillnz(std::size_t count)
	{
		my_fill_n(digits.data()+position,count,static_cast<char8_t>(0));
		position+=count;
	}
};
template<std::size_t buffer_size>
inline constexpr auto begin(stack_arbitary_int<buffer_size> const& val) noexcept
{
	return val.digits.data();
}
template<std::size_t buffer_size>
inline constexpr auto end(stack_arbitary_int<buffer_size> const& val) noexcept
{
	return val.digits.data()+val.position;
}

template<buffer_output_stream output,std::size_t buffer_size>
inline constexpr void print_define(output& out,stack_arbitary_int<buffer_size> const& e)
{
	for(auto const & ch : e)
		put(out,ch+u8'0');
}

template<std::size_t buffer_size>
inline constexpr bool fake_minus_assignment(stack_arbitary_int<buffer_size>& large,stack_arbitary_int<buffer_size> const& small)
{
	auto small_end{small.digits.data()+small.size()};
	auto large_end{large.digits.data()+large.size()};
	bool carry{};
	for(;small.digits.data()+1<small_end;)
	{
		*--large_end-=*--small_end+carry;
		if((carry=(9<*large_end)))
			*large_end+=10;
	}
	if(small.size()==large.size())
	{
		--small_end;
		--large_end;
		auto sum{*small_end+carry};
		if(*large_end<sum)
			return false;
		*large_end-=sum;
	}
	else
	{
		*--large_end-=*--small_end+carry;
		if((carry=(9<*large_end)))
			*large_end+=10;
	}
	return true;
}

}