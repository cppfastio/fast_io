#pragma once

namespace fast_io
{

namespace details
{

template<bool uppercase,::std::integral char_type>
inline constexpr char_type* crypto_hash_pr_df_impl(std::byte const* first,std::byte const* last,char_type* iter) noexcept
{
	constexpr auto tb{::fast_io::details::digits_table<char_type,16,uppercase>};
	constexpr std::size_t two{2};
	for(;first!=last;++first)
		iter=non_overlapped_copy_n(tb+(static_cast<std::uint_least8_t>(*first)<<1),two,iter);
	return iter;
}

template<typename T>
inline void update_multiple_blocks(T* __restrict ctx,io_scatter_t const* base,std::size_t n) noexcept
{
	for(std::size_t i{};i!=n;++i)
	{
		io_scatter_t e{base[i]};
		ctx->update(reinterpret_cast<std::byte const*>(e.base),reinterpret_cast<std::byte const*>(e.base)+e.len);
	}
}

template<bool uppercase,::std::integral char_type>
inline constexpr char_type* pr_rsv_uuid(char_type* iter,std::byte const* uuid) noexcept
{
	auto next_it{uuid+4};//4
	iter=crypto_hash_pr_df_impl<uppercase>(uuid,next_it,iter);
	for(std::size_t i{};i!=3;++i)//2*3=6
	{
		*iter=char_literal_v<u8'-',char_type>;
		++iter;
		uuid=next_it;
		next_it+=2;
		iter=crypto_hash_pr_df_impl<uppercase>(uuid,next_it,iter);
	}
	*iter=char_literal_v<u8'-',char_type>;//6
	++iter;
	return crypto_hash_pr_df_impl<uppercase>(next_it,next_it+6,iter);
}

inline constexpr std::size_t hex_encoding_prv_size_cal(std::byte const* first,std::byte const* last) noexcept
{
	std::size_t n{static_cast<std::size_t>(last-first)};
	constexpr std::size_t mxn{std::numeric_limits<std::size_t>::max()/2};
	if(n>mxn)
	{
		::fast_io::fast_terminate();
	}
	return n<<1;
}

}

template<bool uppercase>
struct basic_hex_encode
{
	using manip_tag = manip_tag_t;
	std::byte const* reference{};
	std::byte const* last{};
};

template<std::integral char_type,bool uppercase>
inline constexpr std::size_t print_reserve_size(
	io_reserve_type_t<char_type,::fast_io::basic_hex_encode<uppercase>>,
	::fast_io::basic_hex_encode<uppercase> e) noexcept
{
	return ::fast_io::details::hex_encoding_prv_size_cal(e.reference,e.last);
}

template<std::integral char_type,bool uppercase>
inline constexpr char_type* print_reserve_define(
	io_reserve_type_t<char_type,::fast_io::basic_hex_encode<uppercase>>,
	char_type* iter,
	::fast_io::basic_hex_encode<uppercase> e) noexcept
{
	return ::fast_io::details::crypto_hash_pr_df_impl<uppercase>(e.reference,e.last,iter);
}

template<::std::integral ch_type,typename T>
struct basic_crypto_hash_as_file
{
	using char_type = ch_type;
	using manip_tag = manip_tag_t;
	using native_handle_type = T*;
	T* ptr{};
};

template<::std::integral ch_type,typename T>
inline constexpr basic_crypto_hash_as_file<ch_type,T> io_value_handle(basic_crypto_hash_as_file<ch_type,T> t) noexcept
{
	return t;
}

template<std::integral char_type,typename T>
inline constexpr void require_secure_clear(basic_crypto_hash_as_file<char_type,T>) noexcept{}

template<::std::integral ch_type,typename T,::std::integral char_type>
inline constexpr void write(basic_crypto_hash_as_file<ch_type,T> t,char_type const* first,char_type const* last) noexcept
{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		for(;first!=last;++first)
		{
			auto a{::std::bit_cast<::fast_io::freestanding::array<::std::byte,sizeof(*first)>>(*first)};
			t.ptr->update(a.data(),a.data()+sizeof(*first));
		}
	}
	else
#endif
	{
		t.ptr->update(reinterpret_cast<std::byte const*>(first),
			reinterpret_cast<std::byte const*>(last));
	}
}

template<::std::integral ch_type,typename T>
inline void scatter_write(basic_crypto_hash_as_file<ch_type,T> t,io_scatters_t scatters) noexcept
{
	::fast_io::details::update_multiple_blocks(t.ptr,scatters.base,scatters.len);
}

namespace manipulators
{

template<::std::contiguous_iterator Iter>
requires std::is_trivially_copyable_v<std::iter_value_t<Iter>>
inline constexpr ::fast_io::basic_hex_encode<false> hex_encode(Iter first,Iter last) noexcept
{
	if constexpr(std::same_as<std::iter_value_t<Iter>,std::byte>)
	{
		return {std::to_address(first),std::to_address(last)};
	}
	else
	{
		return {reinterpret_cast<std::byte const*>(std::to_address(first)),
		reinterpret_cast<std::byte const*>(std::to_address(last))};
	}
}

template<::std::contiguous_iterator Iter>
requires std::is_trivially_copyable_v<std::iter_value_t<Iter>>
inline constexpr ::fast_io::basic_hex_encode<true> hex_encode_upper(Iter first,Iter last) noexcept
{
	if constexpr(std::same_as<std::iter_value_t<Iter>,std::byte>)
	{
		return {std::to_address(first),std::to_address(last)};
	}
	else
	{
		return {reinterpret_cast<std::byte const*>(std::to_address(first)),
		reinterpret_cast<std::byte const*>(std::to_address(last))};
	}
}

template<::std::ranges::contiguous_range rg>
requires std::is_trivially_copyable_v<::std::ranges::range_value_t<rg>>
inline constexpr ::fast_io::basic_hex_encode<false> hex_encode(rg&& r) noexcept
{
	return hex_encode(std::to_address(std::ranges::begin(r)),std::to_address(std::ranges::end(r)));
}

template<::std::ranges::contiguous_range rg>
requires std::is_trivially_copyable_v<::std::ranges::range_value_t<rg>>
inline constexpr ::fast_io::basic_hex_encode<true> hex_encode_upper(rg&& r) noexcept
{
	return hex_encode_upper(std::to_address(std::ranges::begin(r)),std::to_address(std::ranges::end(r)));
}

template<std::integral char_type,typename T>
inline constexpr ::fast_io::basic_crypto_hash_as_file<char_type,T> basic_as_file(T& hashctx) noexcept
{
	return {__builtin_addressof(hashctx)};
}

template<typename T>
inline constexpr ::fast_io::basic_crypto_hash_as_file<char,T> as_file(T& hashctx) noexcept
{
	return {__builtin_addressof(hashctx)};
}

template<typename T>
inline constexpr ::fast_io::basic_crypto_hash_as_file<wchar_t,T> was_file(T& hashctx) noexcept
{
	return {__builtin_addressof(hashctx)};
}

template<typename T>
inline constexpr ::fast_io::basic_crypto_hash_as_file<char8_t,T> u8as_file(T& hashctx) noexcept
{
	return {__builtin_addressof(hashctx)};
}

template<typename T>
inline constexpr ::fast_io::basic_crypto_hash_as_file<char16_t,T> u16as_file(T& hashctx) noexcept
{
	return {__builtin_addressof(hashctx)};
}

template<typename T>
inline constexpr ::fast_io::basic_crypto_hash_as_file<char32_t,T> u32as_file(T& hashctx) noexcept
{
	return {__builtin_addressof(hashctx)};
}

}

namespace manipulators
{

template<typename T>
concept crypto_hash_context = requires(T t,std::byte* ptr)
{
	t.update(ptr,ptr);
	t.do_final();
};

template<typename T>
concept compile_time_size_crypto_hash_context = requires(T t)
{
	std::remove_cvref_t<T>::digest_size;
};

template<typename T>
concept runtime_size_crypto_hash_context = requires(T t)
{
	{t.runtime_digest_size()}->::std::convertible_to<std::size_t>;
};

enum class digest_format
{
lower,
upper,
raw_bytes
};

template<digest_format d,typename T>
struct hash_digest_t
{
	using manip_tag = manip_tag_t;
	using reference_type = T;
	reference_type reference;
};

template<crypto_hash_context ctx>
inline constexpr hash_digest_t<digest_format::lower,ctx const&> hash_digest(ctx const& r) noexcept
{
	return {r};
}

template<crypto_hash_context ctx>
inline constexpr hash_digest_t<digest_format::upper,ctx const&> hash_digest_upper(ctx const& r) noexcept
{
	return {r};
}

template<crypto_hash_context ctx>
inline constexpr hash_digest_t<digest_format::raw_bytes,ctx const&> hash_digest_raw_bytes(ctx const& r) noexcept
{
	return {r};
}

template<digest_format d,crypto_hash_context T>
struct hash_compress_t
{
	using manip_tag = manip_tag_t;
	using reference_type = T;
	std::byte const* base{};
	std::size_t len{};
};

template<crypto_hash_context ctx,::std::ranges::contiguous_range T>
requires (::std::is_trivially_copyable_v<::std::ranges::range_value_t<T>>&&!::std::is_array_v<T>)
inline constexpr hash_compress_t<digest_format::lower,ctx> hash_compress(T const& t) noexcept
{
	if constexpr(std::same_as<::std::ranges::range_value_t<T>,std::byte>)
	{
		return {::std::ranges::data(t),::std::ranges::size(t)};
	}
	else
	{
		return {reinterpret_cast<std::byte const*>(::std::ranges::data(t)),static_cast<std::size_t>(::std::ranges::size(t))*sizeof(::std::ranges::range_value_t<T>)};
	}
}

template<crypto_hash_context ctx,::std::ranges::contiguous_range T>
requires (::std::is_trivially_copyable_v<::std::ranges::range_value_t<T>>&&!::std::is_array_v<T>)
inline constexpr hash_compress_t<digest_format::upper,ctx> hash_compress_upper(T const& t) noexcept
{
	if constexpr(std::same_as<::std::ranges::range_value_t<T>,std::byte>)
	{
		return {::std::ranges::data(t),::std::ranges::size(t)};
	}
	else
	{
		return {reinterpret_cast<std::byte const*>(::std::ranges::data(t)),static_cast<std::size_t>(::std::ranges::size(t))*sizeof(T)};
	}
}

template<crypto_hash_context ctx,::std::ranges::contiguous_range T>
requires (::std::is_trivially_copyable_v<::std::ranges::range_value_t<T>>&&!::std::is_array_v<T>)
inline constexpr hash_compress_t<digest_format::raw_bytes,ctx> hash_compress_raw_bytes(T const& t) noexcept
{
	if constexpr(std::same_as<::std::ranges::range_value_t<T>,std::byte>)
	{
		return {::std::ranges::data(t),::std::ranges::size(t)};
	}
	else
	{
		return {reinterpret_cast<std::byte const*>(::std::ranges::data(t)),static_cast<std::size_t>(::std::ranges::size(t))*sizeof(T)};
	}
}

}

namespace details
{
template<typename T>
concept context_digest_to_byte_ptr_runtime_impl = requires(T t,std::byte* ptr)
{
	{t.digest_to_byte_ptr(ptr)}->std::same_as<std::byte*>;
};

template<typename T>
concept context_digest_byte_ptr_impl = requires(T t)
{
	{t.digest_byte_ptr()}->std::same_as<std::byte const*>;
};

template<::fast_io::manipulators::digest_format d,std::size_t digest_size>
requires (static_cast<std::size_t>(d)<static_cast<std::size_t>(3))
inline constexpr std::size_t cal_crypto_hash_resrv_size() noexcept
{
	static_assert(digest_size<=SIZE_MAX/2);
	constexpr std::size_t v{d==::fast_io::manipulators::digest_format::raw_bytes?digest_size:(digest_size<<1u)};
	return v;
}

template<::fast_io::manipulators::digest_format d,std::size_t digest_size>
inline constexpr std::size_t crypto_hash_resrv_size_cache{cal_crypto_hash_resrv_size<d,digest_size>()};

template<::fast_io::manipulators::digest_format d,::std::integral char_type>
inline constexpr char_type* copy_to_hash_df_commom_impl(char_type* iter,std::byte const* buffer,std::size_t digest_size) noexcept
{
	if constexpr(d==::fast_io::manipulators::digest_format::raw_bytes)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if(__builtin_is_constant_evaluated())
#endif
		{
			for(std::size_t i{};i!=digest_size;++i)
			{
				*iter=std::to_integer<char unsigned>(buffer[i]);
				++iter;
			}
			return iter;
		}
		else
#endif
		{
			return ::fast_io::details::non_overlapped_copy_n(reinterpret_cast<char unsigned const*>(buffer),digest_size,iter);
		}
	}
	else
	{
		return ::fast_io::details::crypto_hash_pr_df_impl<d==::fast_io::manipulators::digest_format::upper>(buffer,buffer+digest_size,iter);
	}
}


template<::fast_io::manipulators::digest_format d,typename T,::std::integral char_type>
inline constexpr char_type* prv_srv_hash_df_common_impl(char_type* iter,T const& t) noexcept
{
	if constexpr(::fast_io::manipulators::compile_time_size_crypto_hash_context<T>)
	{
		constexpr std::size_t digest_size{std::remove_cvref_t<T>::digest_size};
		std::byte buffer[digest_size];
		if constexpr(context_digest_to_byte_ptr_runtime_impl<T>)
		{
			std::size_t diff{static_cast<std::size_t>(t.digest_to_byte_ptr(buffer)-buffer)};
			return copy_to_hash_df_commom_impl<d>(iter,buffer,diff);
		}
		else
		{
			t.digest_to_byte_ptr(buffer);
			return copy_to_hash_df_commom_impl<d>(iter,buffer,digest_size);
		}
	}
	else
	{
		std::size_t digest_size{t.runtime_digest_size()};
		if constexpr(::fast_io::details::context_digest_byte_ptr_impl<T>)
		{
			std::byte const* ptr{t.digest_byte_ptr()};
			return copy_to_hash_df_commom_impl<d>(iter,ptr,digest_size);
		}
		else
		{
			::fast_io::details::local_operator_new_array_ptr<::std::byte> bufferf(digest_size);
			std::byte *buffer{bufferf.ptr};
			if constexpr(context_digest_to_byte_ptr_runtime_impl<T>)
			{
				std::size_t diff{static_cast<std::size_t>(t.digest_to_byte_ptr(buffer)-buffer)};
				return copy_to_hash_df_commom_impl<d>(iter,buffer,diff);
			}
			else
			{
				t.digest_to_byte_ptr(buffer);
				return copy_to_hash_df_commom_impl<d>(iter,buffer,digest_size);
			}
		}
	}
}

// this function may possibly be useless because scan_freestanding will correctly deal with context_scannable
template<::fast_io::manipulators::digest_format d,typename T,::std::random_access_iterator Iter>
inline constexpr Iter prv_srv_hash_df_impl(Iter iter,T const& t) noexcept
{
	if constexpr(::std::contiguous_iterator<Iter>&&!::std::is_pointer_v<Iter>)
	{
		return ::fast_io::details::prv_srv_hash_df_impl<d>(::std::to_address(iter),t)-::std::to_address(iter)+iter;
	}
	else
	{
		using char_type = ::std::iter_value_t<Iter>;
		if constexpr(d==::fast_io::manipulators::digest_format::raw_bytes)
		{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
			if consteval
#else
			if(__builtin_is_constant_evaluated())
#endif
			{
				return ::fast_io::details::prv_srv_hash_df_common_impl<d>(iter,t);
			}
			else
#endif
			{
				if constexpr(sizeof(char_type)==1&&::std::is_pointer_v<Iter>)
				{
					if constexpr(context_digest_to_byte_ptr_runtime_impl<T>)
					{
						return t.digest_to_byte_ptr(reinterpret_cast<std::byte*>(iter));
					}
					else
					{
						constexpr std::size_t digest_size{std::remove_cvref_t<T>::digest_size};
						t.digest_to_byte_ptr(reinterpret_cast<std::byte*>(iter));
						return iter+digest_size;
					}
				}
				else
				{
					return ::fast_io::details::prv_srv_hash_df_common_impl<d>(iter,t);
				}
			}
		}
		else
		{
			return ::fast_io::details::prv_srv_hash_df_common_impl<d>(iter,t);
		}
	}
}

template<typename T>
inline constexpr std::byte* cal_hash_internal_impl(std::byte const* base,std::size_t len,std::byte* buffer) noexcept
{
	T t;
	t.update(base,base+len);
	t.do_final();
	return t.digest_to_byte_ptr(buffer);
}

template<typename T>
inline constexpr void cal_hash_internal(std::byte const* base,std::size_t len,std::byte* buffer) noexcept
{
	T t;
	t.update(base,base+len);
	t.do_final();
	t.digest_to_byte_ptr(buffer);
}

template<::fast_io::manipulators::digest_format d,typename T,::std::integral char_type>
inline constexpr char_type* prv_srv_hash_compress_df_impl(char_type* iter,std::byte const* base,std::size_t len) noexcept
{
	constexpr std::size_t digest_size{std::remove_cvref_t<T>::digest_size};
	if constexpr(context_digest_to_byte_ptr_runtime_impl<T>)
	{
		if constexpr(d==::fast_io::manipulators::digest_format::raw_bytes)
		{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
			if consteval
#else
			if(__builtin_is_constant_evaluated())
#endif
			{
				std::byte buffer[digest_size];
				auto ret{cal_hash_internal_impl<T>(base,len,buffer)};
				return ::fast_io::details::copy_to_hash_df_commom_impl<d==::fast_io::manipulators::digest_format::upper>(buffer,iter,static_cast<std::size_t>(ret-buffer));
			}
			else
#endif
			{
				if constexpr(sizeof(char_type)==1)
				{
					return cal_hash_internal_impl<T>(base,len,reinterpret_cast<std::byte*>(iter));
				}
				else
				{
					std::byte buffer[digest_size];
					auto p{cal_hash_internal_impl<T>(base,len,buffer)};
					return ::fast_io::details::copy_to_hash_df_commom_impl<d>(iter,buffer,static_cast<std::size_t>(p-buffer));
				}
			}
		}
		else
		{
			std::byte buffer[digest_size];
			auto p{cal_hash_internal_impl<T>(base,len,buffer)};
			return ::fast_io::details::copy_to_hash_df_commom_impl<d>(iter,buffer,static_cast<std::size_t>(p-buffer));
		}
	}
	else
	{
		if constexpr(d==::fast_io::manipulators::digest_format::raw_bytes)
		{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
			if consteval
#else
			if(__builtin_is_constant_evaluated())
#endif
			{
				std::byte buffer[digest_size];
				cal_hash_internal<T>(base,len,buffer);
				return ::fast_io::details::copy_to_hash_df_commom_impl<d==::fast_io::manipulators::digest_format::upper>(buffer,iter,digest_size);
			}
			else
#endif
			{
				if constexpr(sizeof(char_type)==1)
				{
					cal_hash_internal<T>(base,len,reinterpret_cast<std::byte*>(iter));
					return iter+digest_size;
				}
				else
				{
					std::byte buffer[digest_size];
					cal_hash_internal<T>(base,len,buffer);
					return ::fast_io::details::copy_to_hash_df_commom_impl<d>(iter,buffer,digest_size);
				}
			}
		}
		else
		{
			std::byte buffer[digest_size];
			cal_hash_internal<T>(base,len,buffer);
			return ::fast_io::details::copy_to_hash_df_commom_impl<d>(iter,buffer,digest_size);
		}
	}
}

}

template<std::integral char_type,::fast_io::manipulators::digest_format d,::fast_io::manipulators::compile_time_size_crypto_hash_context T>
requires (static_cast<std::size_t>(d)<static_cast<std::size_t>(3))
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,::fast_io::manipulators::hash_digest_t<d,T const&>>) noexcept
{
	return ::fast_io::details::crypto_hash_resrv_size_cache<d,std::remove_cvref_t<T>::digest_size>;
}

template<std::integral char_type,::fast_io::manipulators::digest_format d,::fast_io::manipulators::runtime_size_crypto_hash_context T>
requires (static_cast<std::size_t>(d)<static_cast<std::size_t>(3))
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,::fast_io::manipulators::hash_digest_t<d,T const&>>,::fast_io::manipulators::hash_digest_t<d,T const&> digest) noexcept
{
	if constexpr(d==::fast_io::manipulators::digest_format::raw_bytes)
	{
		return digest.reference.runtime_digest_size();
	}
	else
	{
		std::size_t dgst_size{digest.reference.runtime_digest_size()};
		constexpr std::size_t half_size{SIZE_MAX/2u};
		if(dgst_size>half_size)
		{
			fast_terminate();
		}
		return dgst_size<<1u;
	}
}

template<::fast_io::manipulators::digest_format d,::fast_io::manipulators::crypto_hash_context T,::std::integral char_type>
inline constexpr char_type* print_reserve_define(
	::fast_io::io_reserve_type_t<char_type,
	::fast_io::manipulators::hash_digest_t<d,T const&>>,
	char_type* iter,::fast_io::manipulators::hash_digest_t<d,T const&> t) noexcept
{
	return ::fast_io::details::prv_srv_hash_df_impl<d>(iter,t.reference);
}


template<std::integral char_type,::fast_io::manipulators::digest_format d,::fast_io::manipulators::crypto_hash_context T>
requires (static_cast<std::size_t>(d)<static_cast<std::size_t>(3))
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,::fast_io::manipulators::hash_compress_t<d,T>>) noexcept
{
	return ::fast_io::details::crypto_hash_resrv_size_cache<d,std::remove_cvref_t<T>::digest_size>;
}
template<::fast_io::manipulators::digest_format d,::fast_io::manipulators::crypto_hash_context T,::std::integral char_type>
inline constexpr char_type* print_reserve_define(
	::fast_io::io_reserve_type_t<char_type,
	::fast_io::manipulators::hash_compress_t<d,T>>,
	char_type* iter,::fast_io::manipulators::hash_compress_t<d,T> t) noexcept
{
	return ::fast_io::details::prv_srv_hash_compress_df_impl<d,T>(iter,t.base,t.len);
}
}
