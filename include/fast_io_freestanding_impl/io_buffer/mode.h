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

#if 0

template<typename T,::std::size_t N>
struct basic_io_buffer_pointers_on_stack
{
	using value_type = T;
	using pointer = T*;
	pointer buffer_curr{},buffer_end{};
	value_type buffer[N];
};

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

template<typename char_type>
constexpr ::std::size_t compute_default_buffer_size() noexcept
{
	if constexpr(::std::same_as<char_type,void>)
	{
		return 0;
	}
	else if constexpr(sizeof(char_type)==0)
	{
		return 0;
	}
	else
	{
		constexpr
			::std::size_t buffersize{
#ifdef FAST_IO_BUFFER_SIZE
	(FAST_IO_BUFFER_SIZE<sizeof(char_type))?FAST_IO_BUFFER_SIZE:sizeof(char_type)
#elif SIZE_MAX<=UINT_LEAST16_MAX
	128
#elif SIZE_MAX<=UINT_LEAST32_MAX
	8192
#else
	131072
#endif
			};
		return buffersize/sizeof(char_type);
	}
}

template<typename chtype>
inline constexpr ::std::size_t compute_default_input_output_buf_size(::fast_io::buffer_mode req,::fast_io::buffer_mode m) noexcept
{
	if((m&req)==req)
	{
		return ::fast_io::details::compute_default_buffer_size<chtype>();
	}
	return 0;
}

template<typename chtype>
inline constexpr bool compute_whether_possible_flag_for_buffer_mode(::fast_io::buffer_mode req,::fast_io::buffer_mode m,::std::size_t buffersize) noexcept
{
	if((m&req)==req)
	{
		return buffersize!=0&&::std::integral<chtype>;
	}
	else
	{
		return buffersize==0&&::std::same_as<chtype,void>;
	}
}

}

template<typename char_type>
inline constexpr
	::std::size_t default_buffer_size{::fast_io::details::compute_default_buffer_size<char_type>()};

template<buffer_mode mde,
	typename allocatortype,
	typename input_chtype,
	typename output_chtype,
	::std::size_t input_buf_size=
		::fast_io::details::compute_default_input_output_buf_size<input_chtype>(::fast_io::buffer_mode::in,mde),
	::std::size_t output_buf_size=
		::fast_io::details::compute_default_input_output_buf_size<output_chtype>(::fast_io::buffer_mode::out,mde)>
requires (
	::fast_io::details::compute_whether_possible_flag_for_buffer_mode<input_chtype>(::fast_io::buffer_mode::in,mde,input_buf_size)
	&&
	::fast_io::details::compute_whether_possible_flag_for_buffer_mode<output_chtype>(::fast_io::buffer_mode::out,mde,output_buf_size)
	)
struct basic_io_buffer_traits
{
	using allocator_type = allocatortype;
	using input_char_type = input_chtype;
	using output_char_type = output_chtype;
	static inline constexpr buffer_mode mode = mde;
	static inline constexpr bool input_buffer_on_stack = false;
	static inline constexpr bool output_buffer_on_stack = false;
	static inline constexpr bool input_buffer_size_is_fixed = true;
	static inline constexpr bool output_buffer_size_is_fixed = true;
	static inline constexpr bool allocator_is_object = false;
	static inline constexpr ::std::size_t input_buffer_size = input_buf_size;
	static inline constexpr ::std::size_t output_buffer_size = output_buf_size;
};

}
