#pragma once

namespace fast_io::curve25519
{

struct field_number
{
	/*
	Wide limbs (4x64) on targets with a native 64-bit multiply: x86-64,
	aarch64, wasm (wasm32 i64 is a real instruction; the JIT lowers it to
	hardware u64 ops). Narrow limbs (8x32) on true 32-bit targets (i386,
	arm32, riscv32, ...): u64 ops are synthesized there anyway, so we do
	the split ourselves and control the carry schedule.
	*/

	using value_type =
	::std::conditional_t<
#if (defined(__i686__) || defined(_M_IX86))
	true
#else
	false
#endif
	, ::std::uint_least32_t, ::std::uint_least64_t>;

	static inline constexpr ::std::size_t array_size{4u*sizeof(::std::uint_least64_t)/sizeof(value_type)};

	using size_type = ::std::size_t;
	static inline constexpr size_type array_size_bytes{array_size * sizeof(value_type)};
	value_type content[array_size];

	static inline constexpr bool is_empty() noexcept
	{
		return false;
	}
	static inline constexpr size_type size() noexcept
	{
		return array_size;
	}
	static inline constexpr size_type size_bytes() noexcept
	{
		return array_size_bytes;
	}
	static inline constexpr size_type max_size() noexcept
	{
		return array_size;
	}
	static inline constexpr size_type max_size_bytes() noexcept
	{
		return array_size_bytes;
	}
	inline constexpr value_type const &front() const noexcept
	{
		return *content;
	}
	inline constexpr value_type &front() noexcept
	{
		return *content;
	}
	inline constexpr value_type const &back() const noexcept
	{
		constexpr size_type nm1{array_size - 1u};
		return content[nm1];
	}
	inline constexpr value_type &back() noexcept
	{
		constexpr size_type nm1{array_size - 1u};
		return content[nm1];
	}
	inline constexpr value_type const &front_unchecked() const noexcept
	{
		return *content;
	}
	inline constexpr value_type &front_unchecked() noexcept
	{
		return *content;
	}
	inline constexpr value_type const &back_unchecked() const noexcept
	{
		constexpr size_type nm1{array_size - 1u};
		return content[nm1];
	}
	inline constexpr value_type &back_unchecked() noexcept
	{
		constexpr size_type nm1{array_size - 1u};
		return content[nm1];
	}
	inline constexpr value_type const &operator[](size_type pos) const noexcept
	{
		if (array_size <= pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return content[pos];
	}
	inline constexpr value_type &operator[](size_type pos) noexcept
	{
		if (array_size <= pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return content[pos];
	}
	inline constexpr value_type const &index_unchecked(size_type pos) const noexcept
	{
		return content[pos];
	}
	inline constexpr value_type &index_unchecked(size_type pos) noexcept
	{
		return content[pos];
	}
	inline constexpr value_type const *data() const noexcept
	{
		return content;
	}
	inline constexpr value_type *data() noexcept
	{
		return content;
	}
};

static_assert(::std::is_trivially_copyable_v<field_number>);

/*
Build a field_number from four u64 limbs so that table initializers can
stay in u64 form regardless of limb width.
*/
inline constexpr field_number field_number_from_u64(::std::uint_least64_t a0, ::std::uint_least64_t a1,
													::std::uint_least64_t a2, ::std::uint_least64_t a3) noexcept
{
	field_number r FAST_IO_INDETERMINATE;
	if constexpr (::std::same_as<field_number::value_type, ::std::uint_least32_t>)
	{
		r.content[0] = static_cast<::std::uint_least32_t>(a0);
		r.content[1] = static_cast<::std::uint_least32_t>(a0 >> 32);
		r.content[2] = static_cast<::std::uint_least32_t>(a1);
		r.content[3] = static_cast<::std::uint_least32_t>(a1 >> 32);
		r.content[4] = static_cast<::std::uint_least32_t>(a2);
		r.content[5] = static_cast<::std::uint_least32_t>(a2 >> 32);
		r.content[6] = static_cast<::std::uint_least32_t>(a3);
		r.content[7] = static_cast<::std::uint_least32_t>(a3 >> 32);
	}
	else
	{
		r.content[0] = a0;
		r.content[1] = a1;
		r.content[2] = a2;
		r.content[3] = a3;
	}
	return r;
}

} // namespace fast_io::curve25519
