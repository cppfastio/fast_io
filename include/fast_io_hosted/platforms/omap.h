#pragma once

namespace fast_io
{

template<std::integral ch_type>
class basic_omemory_map
{
public:
	using char_type = ch_type;
	char_type *begin_ptr{},*curr_ptr{},*end_ptr{};
	constexpr basic_omemory_map() = default;
	constexpr basic_omemory_map(native_memory_map_file const& iob,std::size_t offset=0):begin_ptr(reinterpret_cast<char_type*>(iob.address_begin+offset)),curr_ptr(begin_ptr),end_ptr(begin_ptr+iob.size()/sizeof(char_type)){}

	constexpr std::size_t written_bytes() const noexcept
	{
		return static_cast<std::size_t>(curr_ptr-begin_ptr)*sizeof(char_type);
	}
};

namespace details
{

template<std::integral char_type>
inline constexpr void omemory_map_write_impl(basic_omemory_map<char_type>& bomp,char_type const* begin,char_type const* end) noexcept
{
	std::size_t const to_write(end-begin);
	if(static_cast<std::size_t>(bomp.end_ptr-bomp.curr_ptr)<to_write)
		fast_terminate();
	non_overlapped_copy_n(begin,to_write,bomp.curr_ptr);
	bomp.curr_ptr+=to_write;
}
}

template<std::integral char_type,::std::contiguous_iterator Iter>
constexpr void write(basic_omemory_map<char_type>& bomp,Iter begin,Iter end) noexcept
{
	details::omemory_map_write_impl(bomp,::std::to_address(begin),::std::to_address(end));
}

template<std::integral char_type>
constexpr char_type* obuffer_begin(basic_omemory_map<char_type>& bomp) noexcept
{
	return bomp.begin_ptr;
}
template<std::integral char_type>
constexpr char_type* obuffer_curr(basic_omemory_map<char_type>& bomp) noexcept
{
	return bomp.curr_ptr;
}
template<std::integral char_type>
constexpr char_type* obuffer_end(basic_omemory_map<char_type>& bomp) noexcept
{
	return bomp.end_ptr;
}

template<std::integral char_type>
constexpr void obuffer_overflow(basic_omemory_map<char_type>&,char_type) noexcept
{
	fast_terminate();
}

template<std::integral char_type>
constexpr void obuffer_set_curr(basic_omemory_map<char_type>& bomp,char_type* ptr) noexcept
{
	bomp.curr_ptr=ptr;
}

template<std::integral char_type>
constexpr void obuffer_obuffer_overflow_never(basic_omemory_map<char_type>&) noexcept{}

using omemory_map = basic_omemory_map<char>;

template<std::integral ch_type>
class basic_imemory_map
{
public:
	using char_type = ch_type;
	char_type *begin_ptr{},*curr_ptr{},*end_ptr{};
	constexpr basic_imemory_map() = default;
	constexpr basic_imemory_map(native_memory_map_file const& iob,std::size_t offset=0):begin_ptr(reinterpret_cast<char_type*>(iob.address_begin+offset)),curr_ptr(this->begin_ptr),end_ptr(this->begin_ptr+iob.size()/sizeof(char_type)){}
};

template<std::integral char_type,::std::contiguous_iterator Iter>
constexpr Iter read(basic_imemory_map<char_type>& bomp,Iter begin,Iter end) noexcept
{
	std::size_t to_read(end-begin);
	std::size_t const remain_space(bomp.end_ptr-bomp.curr_ptr);
	if(remain_space<to_read)[[unlikely]]
		to_read=remain_space;
	non_overlapped_copy_n(bomp.curr_ptr,to_read,begin);
	bomp.curr_ptr+=to_read;
	return begin+to_read;
}

template<std::integral char_type>
constexpr char_type* ibuffer_begin(basic_imemory_map<char_type>& bomp) noexcept
{
	return bomp.begin_ptr;
}
template<std::integral char_type>
constexpr char_type* ibuffer_curr(basic_imemory_map<char_type>& bomp) noexcept
{
	return bomp.curr_ptr;
}
template<std::integral char_type>
constexpr char_type* ibuffer_end(basic_imemory_map<char_type>& bomp) noexcept
{
	return bomp.end_ptr;
}

template<std::integral char_type>
constexpr bool ibuffer_underflow(basic_imemory_map<char_type>&) noexcept
{
	return false;
}

template<std::integral char_type>
constexpr void ibuffer_set_curr(basic_imemory_map<char_type>& bomp,char_type* ptr) noexcept
{
	bomp.curr_ptr=ptr;
}

template<std::integral char_type>
constexpr void ibuffer_underflow_never(basic_imemory_map<char_type>&) noexcept{}

using imemory_map = basic_imemory_map<char>;

}
