#pragma once

namespace fast_io
{

enum class buffer_mode
{
in=1<<0,
out=1<<1,
tie=1<<2,
io=in|out|tie,
secure_clear=1<<3,
#if 0
line_buffering = (1<<4)|(1<<1),
#endif
};

inline constexpr buffer_mode operator&(buffer_mode x, buffer_mode y) noexcept
{
using utype = typename std::underlying_type<buffer_mode>::type;
return static_cast<buffer_mode>(static_cast<utype>(x) & static_cast<utype>(y));
}

inline constexpr buffer_mode operator|(buffer_mode x, buffer_mode y) noexcept
{
using utype = typename std::underlying_type<buffer_mode>::type;
return static_cast<buffer_mode>(static_cast<utype>(x) | static_cast<utype>(y));
}

inline constexpr buffer_mode operator^(buffer_mode x, buffer_mode y) noexcept
{
using utype = typename std::underlying_type<buffer_mode>::type;
return static_cast<buffer_mode>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

inline constexpr buffer_mode operator~(buffer_mode x) noexcept
{
using utype = typename std::underlying_type<buffer_mode>::type;
return static_cast<buffer_mode>(~static_cast<utype>(x));
}

inline constexpr buffer_mode& operator&=(buffer_mode& x, buffer_mode y) noexcept{return x=x&y;}

inline constexpr buffer_mode& operator|=(buffer_mode& x, buffer_mode y) noexcept{return x=x|y;}

inline constexpr buffer_mode& operator^=(buffer_mode& x, buffer_mode y) noexcept{return x=x^y;}


template<typename char_type>
inline constexpr std::size_t io_default_buffer_size = details::cal_buffer_size<char_type,true>();

struct empty_buffer_pointers
{};

template<typename T>
struct basic_io_buffer_pointers
{
	using value_type = T;
	using pointer = T*;
	pointer buffer_begin{},buffer_curr{},buffer_end{};
};

template<typename T,::std::size_t N>
struct basic_io_buffer_pointers_on_stack
{
	using value_type = T;
	using pointer = T*;
	pointer buffer_curr{},buffer_end{};
	value_type buffer[N];
};

#if 0

template<typename T>
struct basic_io_buffer_pointers_with_cap
{
	using value_type = T;
	using pointer = T*;
	pointer buffer_begin{},buffer_curr{},buffer_end{},buffer_cap{};
};

template<typename T>
struct basic_io_buffer_pointers_only_begin
{
	using value_type = T;
	using pointer = T*;
	pointer buffer_begin{};
};

#endif

template<typename T>
struct basic_io_buffer_pointers_no_curr
{
	using value_type = T;
	using pointer = T*;
	pointer buffer_begin{},buffer_end{};
};

namespace details
{
template<typename handle_type>
inline 
#if __cpp_consteval >= 201811L
consteval
#else
constexpr
#endif
bool constraint_buffer_mode(buffer_mode mode) noexcept
{
	if(((mode&buffer_mode::in)==buffer_mode::in)&&(!::fast_io::input_stream<handle_type>))
		return false;
	if(((mode&buffer_mode::out)==buffer_mode::out)&&(!::fast_io::output_stream<handle_type>))
		return false;
//	if constexpr(secure_clear_requirement_stream<handle_type>)
	if((mode&buffer_mode::secure_clear)!=buffer_mode::secure_clear)
		return false;
	return true;
}

}

template<buffer_mode mde,
	typename allocatortype,
	typename input_chtype,
	::std::size_t input_buf_size,
	typename output_chtype,
	::std::size_t output_buf_size>
requires (input_buf_size!=0&&output_buf_size!=0)
struct basic_io_buffer_traits
{
	using allocator_type = allocatortype;
	static inline constexpr buffer_mode mode = mde;
	static inline constexpr bool input_buffer_on_stack = false;
	static inline constexpr bool output_buffer_on_stack = false;
	static inline constexpr bool input_buffer_size_is_fixed = true;
	static inline constexpr bool output_buffer_size_is_fixed = true;
	static inline constexpr bool allocator_is_object = false;
	static inline constexpr ::std::size_t input_buffer_minimum_size = input_buf_size;
	static inline constexpr ::std::size_t output_buffer_minimum_size = output_buf_size;
};

}
