#pragma once

namespace fast_io::details
{

struct pesudo_uint_least128_little_endian_t
{
	std::uint_least64_t low{},high{};
};

struct pesudo_uint_least128_big_endian_t
{
	std::uint_least64_t high{},low{};
};

using pesudo_uint_least128_t=std::conditional_t<::std::endian::native==::std::endian::big,pesudo_uint_least128_big_endian_t,pesudo_uint_least128_little_endian_t>;

template<typename T,std::size_t counterbits>
requires (counterbits==64||counterbits==128)
struct md5_sha_common_impl
{
	static inline constexpr std::size_t block_size{T::block_size};
	static inline constexpr std::endian hash_endian{T::hash_endian};
	using counter_type = std::conditional_t<counterbits==64,::std::uint_least64_t,
#if __SIZEOF_INT128__
	__uint128_t
#else
	::fast_io::details::pesudo_uint_least128_t
#endif
	>;
	T hasher;
	counter_type counter;
	std::size_t buffer_offset;
	std::byte buffer[block_size];
	constexpr void update_impl(std::byte const* first,std::size_t blocks_bytes) noexcept
	{
		this->hasher.update_blocks(first,first+blocks_bytes);
		if constexpr(std::same_as<counter_type,::fast_io::details::pesudo_uint_least128_t>)
		{
			static_assert(sizeof(std::size_t)<=sizeof(std::uint_least64_t));
			std::uint_least64_t const blocks_bytes_u64{static_cast<std::uint_least64_t>(blocks_bytes)};
			using namespace ::fast_io::details::intrinsics;
			constexpr std::uint_least64_t zero{};
			add_carry(add_carry(false,counter.low,blocks_bytes_u64,counter.low),counter.high,zero,counter.high);
		}
		else
		{
			counter+=static_cast<counter_type>(blocks_bytes);
		}
	}
	constexpr void update_cold_impl(std::byte const* first,std::size_t diff) noexcept
	{
		std::size_t const buffer_space{static_cast<std::size_t>(buffer_offset)};
		std::size_t const buffer_remain_space{static_cast<std::size_t>(block_size-buffer_offset)};
		if(buffer_remain_space!=block_size)
		{
			::fast_io::details::non_overlapped_copy_n(first,buffer_remain_space,buffer+buffer_space);
			diff-=buffer_remain_space;
			first+=buffer_remain_space;
			this->update_impl(buffer,block_size);
		}
		std::size_t const blocks_bytes{(diff/block_size)*block_size};
		this->update_impl(first,blocks_bytes);
		first+=blocks_bytes;
		diff-=blocks_bytes;
		::fast_io::details::non_overlapped_copy_n(first,diff,buffer);
		buffer_offset=diff;
	}
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
	constexpr
#endif
	void append_sentinal(std::size_t final_block_offset) noexcept
	{
		counter_type ct{counter};
		std::uint_least64_t const val{static_cast<std::uint_least64_t>(final_block_offset)};
		if constexpr(std::same_as<counter_type,::fast_io::details::pesudo_uint_least128_t>)
		{
			using namespace ::fast_io::details::intrinsics;
			constexpr std::uint_least64_t zero{};
			add_carry(add_carry(false,ct.low,val,ct.low),ct.high,zero,ct.high);
			add_carry(add_carry(false,ct.low,ct.low,ct.low),ct.high,ct.high,ct.high);//*2
			add_carry(add_carry(false,ct.low,ct.low,ct.low),ct.high,ct.high,ct.high);//*4
			add_carry(add_carry(false,ct.low,ct.low,ct.low),ct.high,ct.high,ct.high);//*8
		}
		else
		{
			ct+=val;
			ct*=8u;
		}
		if constexpr(std::endian::native!=hash_endian)
		{
			if constexpr(std::same_as<counter_type,::fast_io::details::pesudo_uint_least128_t>)
			{
				auto t{::fast_io::byte_swap(ct.low)};
				ct.low=::fast_io::byte_swap(ct.high);
				ct.high=t;
			}
			else
			{
				ct=::fast_io::byte_swap(ct);
			}
		}
		constexpr std::size_t start_pos{block_size-sizeof(counter_type)};
		::fast_io::freestanding::type_punning_to_bytes(ct,buffer+start_pos);
		this->hasher.update_blocks(buffer,buffer+block_size);
	}
	inline constexpr void update(std::byte const* block_first,std::byte const* block_last) noexcept
	{
		std::size_t const diff{static_cast<std::size_t>(block_last-block_first)};
		std::size_t const buffer_remain_space{static_cast<std::size_t>(block_size-buffer_offset)};
		if(diff<buffer_remain_space)[[likely]]
		{
#if defined(__has_builtin)
#if __has_builtin(__builtin_unreachable)
			if(diff>=block_size)
				__builtin_unreachable();
#endif
#endif
			::fast_io::details::non_overlapped_copy_n(block_first,diff,buffer+buffer_offset);
			buffer_offset+=diff;
		}
		else
		{
			this->update_cold_impl(block_first,diff);
		}
	}

#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
	constexpr
#endif
	void do_final() noexcept
	{
		constexpr std::size_t counter_type_size{sizeof(counter)};
		constexpr std::size_t sz{block_size-counter_type_size};
		std::size_t const buffer_offs{this->buffer_offset};
		std::size_t buffer_off{buffer_offs};
		buffer[buffer_off]=std::byte{0x80};
		++buffer_off;
		if(buffer_off<=sz)
		{
			std::size_t const to_fill{static_cast<std::size_t>(sz-buffer_off)};
#if defined(__has_builtin)
#if __has_builtin(__builtin_unreachable)
			if(to_fill>sz)
				__builtin_unreachable();
#endif
#endif
			::fast_io::none_secure_clear(this->buffer+buffer_off,to_fill);
			this->append_sentinal(buffer_offs);
			return;
		}
		std::size_t const to_fill{static_cast<std::size_t>(block_size-buffer_off)};
#if defined(__has_builtin)
#if __has_builtin(__builtin_unreachable)
		if(to_fill>counter_type_size)
			__builtin_unreachable();
#endif
#endif
		::fast_io::none_secure_clear(this->buffer+buffer_off,to_fill);
		this->hasher.update_blocks(this->buffer,this->buffer+block_size);
		::fast_io::none_secure_clear(this->buffer,sz);
		this->append_sentinal(buffer_offs);
	}
};

template<typename T,typename initializer,std::size_t counterbits>
class basic_md5_sha_context_impl
{
	md5_sha_common_impl<T,counterbits> hasher;
public:
	explicit constexpr basic_md5_sha_context_impl() noexcept
	{
		this->reset();
	}
	static inline constexpr std::size_t digest_size{initializer::digest_size};
	constexpr void update(std::byte const* block_first,std::byte const* block_last) noexcept
	{
		hasher.update(block_first,block_last);
	}
	constexpr void reset() noexcept
	{
		hasher.hasher=initializer::initialize_value;
		hasher.counter={};
		hasher.buffer_offset=0;
	}
	constexpr void do_final() noexcept
	{
		hasher.do_final();
	}
	constexpr void digest_to_byte_ptr(std::byte* digest) const noexcept
	{
		initializer::digest_to_byte_ptr(hasher.hasher.state,digest);
	}
};

template<std::endian end,std::unsigned_integral U>
inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
void hash_digest_to_byte_ptr_common_impl(U const* digest,std::size_t n,std::byte* ptr) noexcept
{
	constexpr std::size_t usz{sizeof(U)};
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(__builtin_is_constant_evaluated())
#endif
	{
		for(std::size_t i{};i!=n;++i)
		{
			U v{digest[i]};
			if constexpr(::std::endian::native==end)
			{
				v=::fast_io::byte_swap(v);
			}
			::fast_io::freestanding::array<::std::byte,usz> va{::fast_io::bit_cast<::fast_io::freestanding::array<::std::byte,usz>>(digest[i])};
			ptr=::fast_io::freestanding::nonoverlapped_bytes_copy_n(va.data(),usz,ptr);
		}
	}
	else
#endif
	{
		if constexpr(::std::endian::native==end)
		{
			::fast_io::details::my_memcpy(ptr,digest,n*sizeof(U));
		}
		else
		{
			for(std::size_t i{};i!=n;++i)
			{
				auto v{::fast_io::byte_swap(digest[i])};
				::fast_io::details::my_memcpy(ptr,__builtin_addressof(v),usz);
				ptr+=usz;
			}
		}
	}
}


#if (!defined(_MSC_VER) || defined(__clang__)) && (defined(__SSE4_2__) || defined(__wasm_simd128__))

template<std::endian end,std::unsigned_integral U>
inline constexpr void hash_digest_to_byte_ptr_simd16_impl(U const* digest,std::size_t n,std::byte* ptr) noexcept
{
	constexpr std::size_t usz{sizeof(U)};
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(__builtin_is_constant_evaluated())
#endif
	{
		hash_digest_to_byte_ptr_common_impl(digest,n,ptr);
	}
	else
#endif
	{
		if constexpr(::std::endian::native==end)
		{
			::fast_io::details::my_memcpy(ptr,digest,n*sizeof(U));
		}
		else
		{
			constexpr std::size_t sixteen{16u};
			constexpr std::size_t factor{sixteen/usz};
			static_assert(sixteen%usz==0&&usz!=sixteen);
			::fast_io::intrinsics::simd_vector<U,factor> s;
			std::byte const* i{reinterpret_cast<std::byte const*>(digest)};
			std::byte const* e{reinterpret_cast<std::byte const*>(digest+n)};
			for(;i!=e;i+=sixteen)
			{
				s.load(i);
				s.swap_endian();
				s.store(ptr);
				ptr+=sixteen;
			}
		}
	}
}
#endif

template<std::unsigned_integral digest_value_type,std::size_t digest_size,std::endian end>
inline constexpr void hash_digest_to_byte_ptr_common(digest_value_type const* digest,std::byte* ptr) noexcept
{
	constexpr std::size_t sz{digest_size/sizeof(digest_value_type)};
	static_assert(sz!=0);
#if (!defined(_MSC_VER) || defined(__clang__)) && (defined(__SSE4_2__) || defined(__wasm_simd128__))
	if constexpr(sz%16u==0)
	{
		hash_digest_to_byte_ptr_simd16_impl<end>(digest,sz,ptr);
	}
	else
#endif
	{
		hash_digest_to_byte_ptr_common_impl<end>(digest,sz,ptr);
	}
	constexpr std::size_t remainder{digest_size%sizeof(digest_value_type)};
	if constexpr(remainder!=0)
	{
		constexpr std::size_t szmul{sz*sizeof(digest_value_type)};
		ptr+=szmul;
		digest_value_type v{digest[sz]};
		if constexpr(end!=std::endian::native)
		{
			v=::fast_io::byte_swap(v);
		}
		::fast_io::freestanding::type_punning_to_bytes_n<remainder>(v,ptr);
	}
}

}
