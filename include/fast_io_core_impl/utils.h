#pragma once

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC system_header
#endif

namespace fast_io
{

//All compilers have supported bit_cast but most people are stick with GCC 10. We provide emulation currently.
template<typename To,typename From>
requires (sizeof(To)==sizeof(From) && std::is_trivially_copyable_v<To> && std::is_trivial_v<From>)
inline
#if __cpp_lib_bit_cast >= 201806L
constexpr
#endif
 To bit_cast(From const& src) noexcept
{
#if __cpp_lib_bit_cast >= 201806L
	return std::bit_cast<To>(src);
#else
	To dst;
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcpy)
		__builtin_memcpy
#else
		std::memcpy
#endif
#else
		std::memcpy
#endif
	(__builtin_addressof(dst), __builtin_addressof(src), sizeof(To));
	return dst;
#endif
}

template<typename R, typename ...Args>
struct make_noexcept
{};

template<typename R, typename ...Args>
struct make_noexcept<R(Args...)> { using type = R(Args...) noexcept; };

template<typename R, typename ...Args>
struct make_noexcept<R(Args...) noexcept> { using type = R(Args...) noexcept; };

template<typename R, typename ...Args>
using make_noexcept_t = typename make_noexcept<R,Args...>::type;


template<typename F>
requires std::is_function_v<F>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr auto noexcept_cast(F* f) noexcept
{
#if __cpp_lib_bit_cast >= 201806L
	return __builtin_bit_cast(make_noexcept_t<F>*,f);
#else
	return reinterpret_cast<make_noexcept_t<F>*>(f);
#endif
}

template<typename F,typename... Args>
requires std::is_function_v<F>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811
constexpr
#endif
decltype(auto) noexcept_call(F* f,Args&& ...args) noexcept
{
#if __cpp_if_consteval >= 202106L
	if consteval
	{
		return f(::std::forward<Args>(args)...);		//EH unwinding does not matter here
	}
	else
	{
		return noexcept_cast(f)(::std::forward<Args>(args)...);
	}
#else
#if __cpp_lib_is_constant_evaluated >= 201811
	if (std::is_constant_evaluated())
		return f(::std::forward<Args>(args)...);		//EH unwinding does not matter here
	else
#endif
		return noexcept_cast(f)(::std::forward<Args>(args)...);
#endif
}

/*
Since many toolchains do not provide lock_guard. Let's implement it by ourselves based on libstdc++'s lock_guard
https://github.com/gcc-mirror/gcc/blob/53046f072c6e92aa4ba4594c992fe31d89e223ed/libstdc%2B%2B-v3/include/bits/std_mutex.h#L152
*/

template<typename mutx_type>
struct io_lock_guard
{
	using mutex_type = mutx_type;
	mutex_type& device;
	explicit constexpr io_lock_guard(mutex_type& m) noexcept: device(m)
	{ device.lock(); }

	#if __cpp_constexpr >= 201907L
	constexpr
	#endif
	~io_lock_guard() noexcept
	{ device.unlock(); }

	io_lock_guard(io_lock_guard const&) = delete;
	io_lock_guard& operator=(io_lock_guard const&) = delete;
};

template<typename stream_type>
struct io_flush_guard
{
	using handle_type = stream_type;
	handle_type& device;
	explicit constexpr io_flush_guard(handle_type& m) noexcept: device(m)
	{ }

	#if __cpp_constexpr >= 201907L
	constexpr
	#endif
	~io_flush_guard() noexcept
	{
#if (defined(_MSC_VER)&&_HAS_EXCEPTIONS!=0) || (!defined(_MSC_VER)&&__cpp_exceptions)
#if __cpp_exceptions
		try
		{
#endif
#endif
			flush(device);
#if (defined(_MSC_VER)&&_HAS_EXCEPTIONS!=0) || (!defined(_MSC_VER)&&__cpp_exceptions)
#if __cpp_exceptions
		}
		catch(...)
		{
		}
#endif
#endif
	}
	io_flush_guard(io_flush_guard const&) = delete;
	io_flush_guard& operator=(io_flush_guard const&) = delete;
};

namespace details
{


template<typename T>
using my_make_signed_t=
#ifdef __SIZEOF_INT128__
std::conditional_t<
std::same_as<std::remove_cv_t<T>,__int128_t>||std::same_as<std::remove_cv_t<T>,__uint128_t>,
std::conditional_t<std::same_as<std::remove_cv_t<T>,__int128_t>,T,
std::conditional_t<std::same_as<T,__uint128_t volatile const>,__int128_t volatile const,
std::conditional_t<std::same_as<T,__uint128_t const>,__int128_t const,
std::conditional_t<std::same_as<T,__uint128_t volatile>,__int128_t volatile,
__int128_t>>>>,
std::make_signed_t<std::conditional_t<std::same_as<std::remove_cv_t<T>,__int128_t>||std::same_as<std::remove_cv_t<T>,__uint128_t>,int,T>>>;
#else
std::make_signed_t<T>;
#endif
template<typename T>
using my_make_unsigned_t=
#ifdef __SIZEOF_INT128__
std::conditional_t<
std::same_as<std::remove_cv_t<T>,__int128_t>||std::same_as<std::remove_cv_t<T>,__uint128_t>,
std::conditional_t<std::same_as<std::remove_cv_t<T>,__uint128_t>,T,
std::conditional_t<std::same_as<T,__int128_t volatile const>,__uint128_t volatile const,
std::conditional_t<std::same_as<T,__int128_t const>,__uint128_t const,
std::conditional_t<std::same_as<T,__int128_t volatile>,__uint128_t volatile,
__uint128_t>>>>,
std::make_unsigned_t<std::conditional_t<std::same_as<std::remove_cv_t<T>,__int128_t>||std::same_as<std::remove_cv_t<T>,__uint128_t>,int,T>>>;
#else
std::make_unsigned_t<T>;
#endif

template<typename T>
concept my_integral = 
std::integral<T>
#ifdef __SIZEOF_INT128__
||std::same_as<std::remove_cv_t<T>,__uint128_t>||std::same_as<std::remove_cv_t<T>,__int128_t>
#endif
;

template<typename T>
concept my_signed_integral = 
std::signed_integral<T>
#ifdef __SIZEOF_INT128__
||std::same_as<std::remove_cv_t<T>,__int128_t>
#endif
;
template<typename T>
concept my_unsigned_integral = my_integral<T>&&!my_signed_integral<T>;


template<typename T>
concept my_floating_point = 
std::floating_point<T>
#ifdef __SIZEOF_FLOAT16__
||std::same_as<std::remove_cv_t<T>,__float16>
#endif
#ifdef __SIZEOF_FLOAT80__
||std::same_as<std::remove_cv_t<T>,__float80>
#endif
#ifdef __SIZEOF_FLOAT128__
||std::same_as<std::remove_cv_t<T>,__float128>
#endif
;

#ifdef __SIZEOF_INT128__ 
inline constexpr __uint128_t calculate_byteswap_ff(std::size_t v) noexcept
{
	return static_cast<__uint128_t>(0xff)<<(v*8);
}
template<std::size_t v>
inline constexpr __uint128_t uint128_t_bsv{calculate_byteswap_ff(v)};
#endif

template<typename U>
requires (sizeof(U)==1||sizeof(U)==2||sizeof(U)==4||sizeof(U)==8
#ifdef __SIZEOF_INT128__ 
||sizeof(U)==16
#endif
)
inline constexpr U byte_swap_naive_impl(U a) noexcept
{
#ifdef __SIZEOF_INT128__
	if constexpr(sizeof(U)==16)
	{
		return  ((a & uint128_t_bsv<15>) >> 120)|
			((a & uint128_t_bsv<14>) >> 104)|
			((a & uint128_t_bsv<13>) >> 88) |
			((a & uint128_t_bsv<12>) >> 72) |
			((a & uint128_t_bsv<11>) >> 56) |
			((a & uint128_t_bsv<10>) >> 40) |
			((a & uint128_t_bsv<9>) >> 24) |
			((a & uint128_t_bsv<8>) >> 8)  |
			((a & uint128_t_bsv<7>) << 8)  |
			((a & uint128_t_bsv<6>) << 24) |
			((a & uint128_t_bsv<5>) << 40) |
			((a & uint128_t_bsv<4>) << 56) |
			((a & uint128_t_bsv<3>) << 72) |
			((a & uint128_t_bsv<2>) << 88) |
			((a & uint128_t_bsv<1>) << 104)|
			((a & uint128_t_bsv<0>) << 120);
	}
	else
#endif
	if constexpr(sizeof(U)==8)
	{
		return  ((a & 0xff00000000000000ULL) >> 56) |
			((a & 0x00ff000000000000ULL) >> 40) |
			((a & 0x0000ff0000000000ULL) >> 24) |
			((a & 0x000000ff00000000ULL) >> 8)  |
			((a & 0x00000000ff000000ULL) << 8)  |
			((a & 0x0000000000ff0000ULL) << 24) |
			((a & 0x000000000000ff00ULL) << 40) |
			((a & 0x00000000000000ffULL) << 56) ;
	}
	else if constexpr(sizeof(U)==4)
	{
		return  ((a & 0xff000000U) >> 24) |
			((a & 0x00ff0000U) >> 8) |
			((a & 0x0000ff00U) << 8) |
			((a & 0x000000ffU) << 24)  ;
	}
	else if constexpr(sizeof(U)==2)
	{
		return static_cast<U>(static_cast<unsigned>(((a & 0xff00) >> 8) |
			((a & 0x00ff) << 8))) ;
	}
	else
		return a;
}

}

template<details::my_unsigned_integral U>
inline constexpr U byte_swap(U a) noexcept
{
	if constexpr(sizeof(U)==1)
		return a;
	else
	{
#if (defined(__GNUC__) || defined(__clang__))
#ifdef __SIZEOF_INT128__
	if constexpr(sizeof(U)==16)
#if __has_builtin(__builtin_bswap128)
		return __builtin_bswap128(a);
#else
	{
		std::uint_least64_t high(__builtin_bswap64(static_cast<std::uint_least64_t>(a>>64)));
		std::uint_least64_t low(__builtin_bswap64(static_cast<std::uint_least64_t>(a)));
		return (static_cast<__uint128_t>(low)<<64)|static_cast<__uint128_t>(high);
	}
#endif
	else
#endif
	if constexpr(sizeof(U)==8)
		return __builtin_bswap64(a);
	else if constexpr(sizeof(U)==4)
		return __builtin_bswap32(a);
	else
		return __builtin_bswap16(a);
#else

#if __cpp_lib_is_constant_evaluated>=201811L
	if (std::is_constant_evaluated())
	{
		return details::byte_swap_naive_impl(a);
	}
	else
#endif
	{
#if defined(_MSC_VER)
	if constexpr(sizeof(U)==8)
		return _byteswap_uint64(a);
	else if constexpr(sizeof(U)==4)
		return _byteswap_ulong(a);
	else
		return _byteswap_ushort(a);
#else
	return details::byte_swap_naive_impl(a);
#endif
	}
#endif
	}
}

template<details::my_unsigned_integral U>
inline constexpr U big_endian(U u) noexcept
{
	if constexpr(sizeof(U)==1||std::endian::big==std::endian::native)
		return u;
	else if constexpr(std::endian::little==std::endian::native)
		return byte_swap(u);
	else
		return details::byte_swap_naive_impl(u);	//support architectures like PDP11
}

template<details::my_unsigned_integral U>
inline constexpr U little_endian(U u) noexcept
{
	if constexpr(sizeof(U)==1||std::endian::little==std::endian::native)
		return u;
	else if constexpr(std::endian::big==std::endian::native)
		return byte_swap(u);
	else
		return details::byte_swap_naive_impl(u);	//support architectures like PDP11
}

namespace details
{

template<typename range_type>
inline 
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
constexpr
#endif
void compile_time_type_punning_copy_n(range_type const* first,std::size_t bytes,std::byte* out)
{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(__builtin_is_constant_evaluated())
#endif
	{
		if constexpr(std::same_as<range_type,std::byte>)
		{
			for(auto i{first},e{first+bytes};i!=e;++i)
			{
				*out=*i;
				++out;
			}
		}
		else
		{
			auto j{out};
			auto first_end{first+bytes/sizeof(range_type)};
			for(auto i{first};i!=first_end;++i)
			{
				::fast_io::freestanding::array<std::byte,sizeof(range_type)> arr{std::bit_cast<::fast_io::freestanding::array<std::byte,sizeof(range_type)>>(*i)};
				j=non_overlapped_copy_n(arr.data(),arr.size(),j);
			}
		}
	}
	else
#endif
	{
		if(bytes)[[likely]]
			my_memcpy(out,first,bytes);
	}
}

template<::std::input_or_output_iterator output_iter,typename T>
requires (std::is_trivially_copyable_v<T> && sizeof(T)<=sizeof(std::uintmax_t))
inline constexpr output_iter my_fill_n(output_iter first,std::size_t count, T value)
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(__builtin_is_constant_evaluated())
#endif
	{
		::fast_io::freestanding::fill_n(first,count,value);
	}
	else
#endif
	{
	using output_value_type = ::std::iter_value_t<output_iter>;
	if constexpr(::std::contiguous_iterator<output_iter>&&std::is_trivially_copyable_v<output_value_type>&&std::integral<output_value_type>&&sizeof(output_value_type)==1)
	{
		if(count)[[likely]]
			my_memset(::std::to_address(first),static_cast<int>(value),count);
		return first+count;
	}
	else
		::fast_io::freestanding::fill_n(first,count,value);
	}
	return first+count;
}

template<::std::forward_iterator fwd_iter,typename T>
requires (std::is_trivially_copyable_v<T> && sizeof(T)<=sizeof(std::uintmax_t))
inline constexpr void my_fill(fwd_iter first,fwd_iter last,T value)
{
	using fwd_iter_value_type = ::std::iter_value_t<fwd_iter>;
	if constexpr(::std::contiguous_iterator<fwd_iter>&&std::is_trivially_copyable_v<fwd_iter_value_type>&&std::integral<fwd_iter_value_type>&&sizeof(fwd_iter_value_type)==1)
		::fast_io::freestanding::fill_n(first,last-first,value);
	else
		::fast_io::freestanding::fill(first,last,value);
}

template<std::integral char_type,std::size_t n>
requires(n!=0)
inline constexpr std::size_t string_literal_size(char_type const(&)[n])
{
	return n-1;
}

template<std::integral char_type,std::size_t n,::std::random_access_iterator output_iter>
requires(n!=0)
inline constexpr output_iter copy_string_literal(char_type const(&s)[n],output_iter result)
{
	details::non_overlapped_copy_n(s,n-1,result);
	return result+(n-1);
}

template<::std::input_or_output_iterator output_iter>
inline constexpr output_iter copy_scatter(basic_io_scatter_t<::std::iter_value_t<output_iter>> scatter,output_iter result)
{
	return details::non_overlapped_copy_n(scatter.base,scatter.len,result);
}

template<my_integral T>
inline constexpr T compile_time_pow(T base,std::size_t pow) noexcept
{
	T t=1;
	for(std::size_t i{};i!=pow;++i)
		t*=base;
	return t;
}

template<my_integral T,std::size_t pow>
inline constexpr T compile_pow10{::fast_io::details::compile_time_pow<::std::remove_cvref_t<T>>(10,pow)};

template<my_integral T,std::size_t pow>
inline constexpr T compile_pow5{::fast_io::details::compile_time_pow<::std::remove_cvref_t<T>>(5,pow)};

template<my_integral T,std::size_t pow>
inline constexpr T compile_pow2{::fast_io::details::compile_time_pow<::std::remove_cvref_t<T>>(2,pow)};

#if 0
inline constexpr std::uint_least64_t fast_lup_table[]{
      4294967296,  8589934582,  8589934582,  8589934582,  12884901788,
      12884901788, 12884901788, 17179868184, 17179868184, 17179868184,
      21474826480, 21474826480, 21474826480, 21474826480, 25769703776,
      25769703776, 25769703776, 30063771072, 30063771072, 30063771072,
      34349738368, 34349738368, 34349738368, 34349738368, 38554705664,
      38554705664, 38554705664, 41949672960, 41949672960, 41949672960,
      42949672960, 42949672960};

inline constexpr std::uint_least64_t fast_lup_switch(std::uint_least32_t value) noexcept
{
	switch(value)
	{
	case 0:case 1:case 2:case 3:case 4: return 42949672960ULL;
	case 5:case 6:case 7:return 38554705664ULL;
	case 8:case 9:case 10:case 11:return 34349738368ULL;
	case 12:case 13:case 14:return 30063771072ULL;
	case 15:case 16:case 17:return 25769703776ULL;
	case 18:case 19:case 20:case 21:return 21474826480ULL;
	case 22:case 23:case 24:return 17179868184ULL;
	case 25:case 26:case 27:return 12884901788ULL;
	case 28:case 29:case 30:return 8589934582ULL;
	case 31:case 32:return 4294967296ULL;
	default:
#if defined(__GNUC__)||defined(__clang__)
		__builtin_unreachable();
#endif
	}
}
#endif

inline constexpr bool is_wasi_environment{
#if __wasi__
true
#endif
};

using optimal_print_unsigned_type = std::conditional_t<is_wasi_environment,::std::uint_least64_t,
std::conditional_t<(sizeof(std::size_t)<sizeof(unsigned)),unsigned,std::size_t>>;

template<typename T>
inline constexpr bool need_seperate_print{(sizeof(T)>sizeof(optimal_print_unsigned_type))&&(sizeof(T)%sizeof(optimal_print_unsigned_type)==0)&&(sizeof(T)/sizeof(optimal_print_unsigned_type)==2)};

template<std::uint_least32_t base,bool ryu_mode=false,std::size_t mx_size=std::numeric_limits<std::size_t>::max(),my_unsigned_integral U>
constexpr std::size_t chars_len(U value) noexcept
{
#if 0
	if constexpr(base==10&&2<=sizeof(U)&&sizeof(U)<=4&&sizeof(std::size_t)>=8&&!ryu_mode)
	{
		return (static_cast<std::uint_least32_t>(value) + fast_lup_switch(
#if defined(_MSC_VER) && !defined(__clang__)
		std::countl_zero(static_cast<std::uint_least32_t>(value))
#elif __has_builtin(__builtin_ia32_lzcnt_u32)
		__builtin_ia32_lzcnt_u32(static_cast<std::uint_least32_t>(value))
#elif __has_builtin(__builtin_clz)
		__builtin_clz(static_cast<std::uint_least32_t>(value) | 1)
#else
		std::countl_zero(static_cast<std::uint_least32_t>(value))
#endif
		)) >> 32;
	}
	else
#endif
	if constexpr(base==10&&sizeof(U)<=16)
	{
		if constexpr(15<sizeof(U))
		{
			using M = std::remove_cvref_t<U>;
			if constexpr(16<sizeof(M)||!ryu_mode)
			{
			if constexpr(39<=mx_size)
			if(compile_pow10<M,38><=value)
				return 39;
			if constexpr(38<=mx_size)
			if(compile_pow10<M,37><=value)
				return 38;
			if constexpr(37<=mx_size)
			if(compile_pow10<M,36><=value)
				return 37;
			}
			if constexpr(36<=mx_size)
			if(compile_pow10<M,35><=value)
				return 36;
			if constexpr(35<=mx_size)
			if(compile_pow10<M,34><=value)
				return 35;
			if constexpr(34<=mx_size)
			if(compile_pow10<M,33><=value)
				return 34;
			if constexpr(33<=mx_size)
			if(compile_pow10<M,32><=value)
				return 33;
			if constexpr(32<=mx_size)
			if(compile_pow10<M,31><=value)
				return 32;
			if constexpr(31<=mx_size)
			if(compile_pow10<M,30><=value)
				return 31;
			if constexpr(30<=mx_size)
			if(compile_pow10<M,29><=value)
				return 30;
			if constexpr(29<=mx_size)
			if(compile_pow10<M,28><=value)
				return 29;
			if constexpr(28<=mx_size)
			if(compile_pow10<M,27><=value)
				return 28;
			if constexpr(27<=mx_size)
			if(compile_pow10<M,26><=value)
				return 27;
			if constexpr(26<=mx_size)
			if(compile_pow10<M,25><=value)
				return 26;
			if constexpr(25<=mx_size)
			if(compile_pow10<M,24><=value)
				return 25;
			if constexpr(24<=mx_size)
			if(compile_pow10<M,23><=value)
				return 24;
			if constexpr(23<=mx_size)
			if(compile_pow10<M,22><=value)
				return 23;
			if constexpr(22<=mx_size)
			if(compile_pow10<M,21><=value)
				return 22;
			if constexpr(21<=mx_size)
			if(compile_pow10<M,20><=value)
				return 21;
		}
		if constexpr(7<sizeof(U))
		{
			if constexpr(8<sizeof(U)||!ryu_mode)
			{
			if constexpr(20<=mx_size)
			if(10000000000000000000ULL<=value)
				return 20;
			if constexpr(19<=mx_size)
			if(1000000000000000000ULL<=value)
				return 19;
			if constexpr(18<=mx_size)
			if(100000000000000000ULL<=value)
				return 18;
			}
			if constexpr(17<=mx_size)
			if(10000000000000000ULL<=value)
				return 17;
			if constexpr(16<=mx_size)
			if(1000000000000000ULL<=value)
				return 16;
			if constexpr(15<=mx_size)
			if(100000000000000ULL<=value)
				return 15;
			if constexpr(14<=mx_size)
			if(10000000000000ULL<=value)
				return 14;
			if constexpr(13<=mx_size)
			if(1000000000000ULL<=value)
				return 13;
			if constexpr(12<=mx_size)
			if(100000000000ULL<=value)
				return 12;
			if constexpr(11<=mx_size)
			if(10000000000ULL<=value)
				return 11;
		}
		if constexpr(3<sizeof(U))
		{
			if constexpr(4<sizeof(U)||!ryu_mode)
			{
			if constexpr(10<=mx_size)
			if(1000000000U<=value)
				return 10;
			}
			if constexpr(9<=mx_size)
			if(100000000U<=value)
				return 9;
			if constexpr(8<=mx_size)
			if(10000000U<=value)
				return 8;
			if constexpr(7<=mx_size)
			if(1000000U<=value)
				return 7;
			if constexpr(6<=mx_size)
			if(100000U<=value)
				return 6;
		}
		if constexpr(1<sizeof(U))
		{
			if constexpr(5<=mx_size)
			if(10000U<=value)
				return 5;
			if constexpr(4<=mx_size)
			if(1000U<=value)
				return 4;
		}
		if constexpr(3<=mx_size)
		if(100U<=value)
			return 3;
		if constexpr(2<=mx_size)
		if(10U<=value)
			return 2;
		return 1;
	}
	else if constexpr(base==2u||base==4u||base==8u||base==16u||base==32u)
	{
		constexpr std::size_t bits{sizeof(U)*8u};
#if defined(__SIZEOF_INT128__)
		if constexpr(sizeof(U)==sizeof(__uint128_t))
		{
			std::uint_least64_t high{static_cast<std::uint_least64_t>(value>>64u)};
			constexpr std::uint_least64_t zero{};
			std::size_t base2len;
			if(high==zero)
			{
				constexpr std::uint_least64_t one_constant{1};
				std::uint_least64_t low{static_cast<std::uint_least64_t>(value)|one_constant};
				constexpr std::size_t bitsdv2{bits>>1u};
				base2len=bitsdv2-static_cast<std::size_t>(std::countl_zero(low));
			}
			else
			{
				base2len=bits-static_cast<std::size_t>(std::countl_zero(high));
			}
			if constexpr(base==2)
				return base2len;
			else
			{
				constexpr std::size_t one{1};
				constexpr std::size_t log2base{static_cast<std::size_t>(std::countr_zero(base))};
				return (base2len-one)/log2base+one;
			}
		}
		else
#endif
		{
			constexpr U one_constant{1};
			std::size_t const base2len{bits-static_cast<std::size_t>(std::countl_zero(static_cast<U>(value|one_constant)))};
			if constexpr(base==2)
				return base2len;
			else
			{
				constexpr std::size_t one{1};
				constexpr std::size_t log2base{static_cast<std::size_t>(std::countr_zero(base))};
				return (base2len-one)/log2base+one;
			}
		}
	}
	else
	{
		constexpr std::uint_least32_t base2(base  * base);
		constexpr std::uint_least32_t base3(base2 * base);
		constexpr std::uint_least32_t base4(base3 * base);
		for (std::size_t n(1);;n+=4)
		{
			if (value < base)
				return n;
			if (value < base2)
				return n + 1;
			if (value < base3)
				return n + 2;
			if (value < base4)
				return n + 3;
			value /= base4;
		}
	}
}

template<my_integral T>
inline constexpr my_make_unsigned_t<T> cal_int_max() noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
	return static_cast<my_make_unsigned_t<T>>(std::numeric_limits<T>::max());
#else
	my_make_unsigned_t<T> n{};
	--n;
	if constexpr(my_signed_integral<T>)
		n>>=1;
	return n;
#endif
}
template<my_integral T>
inline constexpr T get_int_max() noexcept
{
	constexpr T v{static_cast<T>(cal_int_max<T>())};
	return v;
}
template<my_integral T>
inline constexpr auto get_int_max_unsigned() noexcept
{
	constexpr my_make_unsigned_t<std::remove_cvref_t<T>> v{static_cast<my_make_unsigned_t<std::remove_cvref_t<T>>>(cal_int_max<std::remove_cvref_t<T>>())};
	return v;
}
template<my_integral T,char8_t base = 10>
inline constexpr std::size_t cal_max_int_size() noexcept
{
	std::size_t i{};
	auto n(get_int_max_unsigned<T>());
	for(;n;++i)
		n/=base;
	return i;
}

//static_assert(cal_max_int_size<std::uint_least64_t,10>()==20);
//static_assert(cal_max_int_size<std::uint_least32_t,10>()==10);
template<typename char_type,std::size_t N>
inline constexpr basic_io_scatter_t<char_type> tsc(char_type const (&a)[N]) noexcept
{
	return {a,N-1};
}

template<typename char_type,std::size_t N>
inline constexpr std::size_t cal_array_size(char_type const (&)[N]) noexcept
{
	return N-1;
}

template<std::integral char_type,bool iobuf=false>
inline constexpr std::size_t cal_buffer_size()
{
#ifdef FAST_IO_BUFFER_SIZE
	static_assert(sizeof(char_type)<=FAST_IO_BUFFER_SIZE);
#endif
	if constexpr(iobuf)
	{
		return 
#ifdef FAST_IO_BUFFER_SIZE
		FAST_IO_BUFFER_SIZE
#else
	131072u
#endif
		/sizeof(char_type);
	}
	else
	{
	return 
#ifdef FAST_IO_BUFFER_SIZE
	FAST_IO_BUFFER_SIZE	//avoid BUFSIZ macro since it is a cancer and often set incorrectly
#else
#ifdef FAST_IO_TRANSMIT_ON_STACK
	4096
#else
	131072u
#endif
#endif
	/sizeof(char_type);
	}
}
}

}
