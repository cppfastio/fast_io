#pragma once

namespace fast_io::curve25519
{

inline constexpr void addition_u256_discard(::fast_io::curve25519::field_number &z,
											::fast_io::curve25519::field_number const &x,
											::fast_io::curve25519::field_number const &y) noexcept
{
	bool carry FAST_IO_INDETERMINATE;
	if constexpr (::std::same_as<::fast_io::curve25519::field_number::value_type, ::std::uint_least32_t>)
	{
		z.front_unchecked() = ::fast_io::intrinsics::addc(x.front_unchecked(), y.front_unchecked(), false, carry);
		z.index_unchecked(1) = ::fast_io::intrinsics::addc(x.index_unchecked(1), y.index_unchecked(1), carry, carry);
		z.index_unchecked(2) = ::fast_io::intrinsics::addc(x.index_unchecked(2), y.index_unchecked(2), carry, carry);
		z.index_unchecked(3) = ::fast_io::intrinsics::addc(x.index_unchecked(3), y.index_unchecked(3), carry, carry);
		z.index_unchecked(4) = ::fast_io::intrinsics::addc(x.index_unchecked(4), y.index_unchecked(4), carry, carry);
		z.index_unchecked(5) = ::fast_io::intrinsics::addc(x.index_unchecked(5), y.index_unchecked(5), carry, carry);
		z.index_unchecked(6) = ::fast_io::intrinsics::addc(x.index_unchecked(6), y.index_unchecked(6), carry, carry);
		z.back_unchecked() = ::fast_io::intrinsics::addc(x.back_unchecked(), y.back_unchecked(), carry, carry);
	}
	else
	{
		z.front_unchecked() = ::fast_io::intrinsics::addc(x.front_unchecked(), y.front_unchecked(), false, carry);
		z.index_unchecked(1) = ::fast_io::intrinsics::addc(x.index_unchecked(1), y.index_unchecked(1), carry, carry);
		z.index_unchecked(2) = ::fast_io::intrinsics::addc(x.index_unchecked(2), y.index_unchecked(2), carry, carry);
		z.back_unchecked() = ::fast_io::intrinsics::addc(x.back_unchecked(), y.back_unchecked(), carry, carry);
	}
}

inline constexpr ::fast_io::curve25519::field_number::value_type addition_u256(::fast_io::curve25519::field_number &z,
														::fast_io::curve25519::field_number const &x,
														::fast_io::curve25519::field_number const &y) noexcept
{
	using unsigned_type = ::fast_io::curve25519::field_number::value_type;
	bool carry FAST_IO_INDETERMINATE;
	if constexpr (::std::same_as<unsigned_type, ::std::uint_least32_t>)
	{
		z.front_unchecked() = ::fast_io::intrinsics::addc(x.front_unchecked(), y.front_unchecked(), false, carry);
		z.index_unchecked(1) = ::fast_io::intrinsics::addc(x.index_unchecked(1), y.index_unchecked(1), carry, carry);
		z.index_unchecked(2) = ::fast_io::intrinsics::addc(x.index_unchecked(2), y.index_unchecked(2), carry, carry);
		z.index_unchecked(3) = ::fast_io::intrinsics::addc(x.index_unchecked(3), y.index_unchecked(3), carry, carry);
		z.index_unchecked(4) = ::fast_io::intrinsics::addc(x.index_unchecked(4), y.index_unchecked(4), carry, carry);
		z.index_unchecked(5) = ::fast_io::intrinsics::addc(x.index_unchecked(5), y.index_unchecked(5), carry, carry);
		z.index_unchecked(6) = ::fast_io::intrinsics::addc(x.index_unchecked(6), y.index_unchecked(6), carry, carry);
		z.back_unchecked() = ::fast_io::intrinsics::addc(x.back_unchecked(), y.back_unchecked(), carry, carry);
	}
	else
	{
		z.front_unchecked() = ::fast_io::intrinsics::addc(x.front_unchecked(), y.front_unchecked(), false, carry);
		z.index_unchecked(1) = ::fast_io::intrinsics::addc(x.index_unchecked(1), y.index_unchecked(1), carry, carry);
		z.index_unchecked(2) = ::fast_io::intrinsics::addc(x.index_unchecked(2), y.index_unchecked(2), carry, carry);
		z.back_unchecked() = ::fast_io::intrinsics::addc(x.back_unchecked(), y.back_unchecked(), carry, carry);
	}
	constexpr unsigned_type zero{};
	unsigned_type res{::fast_io::intrinsics::addc(zero, zero, carry, carry)};
	return res;
}

inline constexpr void field_number_addition(::fast_io::curve25519::field_number &f,
											::fast_io::curve25519::field_number const &x,
											::fast_io::curve25519::field_number const &y) noexcept
{
	using unsigned_type = ::fast_io::curve25519::field_number::value_type;
	constexpr unsigned_type zero{};
	if constexpr (::std::same_as<unsigned_type, ::std::uint_least32_t>)
	{
		unsigned_type f0 FAST_IO_INDETERMINATE, f1 FAST_IO_INDETERMINATE, f2 FAST_IO_INDETERMINATE, f3 FAST_IO_INDETERMINATE;
		unsigned_type f4 FAST_IO_INDETERMINATE, f5 FAST_IO_INDETERMINATE, f6 FAST_IO_INDETERMINATE, f7 FAST_IO_INDETERMINATE;
		bool carry FAST_IO_INDETERMINATE;
		f0 = ::fast_io::intrinsics::addc(x.front_unchecked(), y.front_unchecked(), false, carry);
		f1 = ::fast_io::intrinsics::addc(x.index_unchecked(1), y.index_unchecked(1), carry, carry);
		f2 = ::fast_io::intrinsics::addc(x.index_unchecked(2), y.index_unchecked(2), carry, carry);
		f3 = ::fast_io::intrinsics::addc(x.index_unchecked(3), y.index_unchecked(3), carry, carry);
		f4 = ::fast_io::intrinsics::addc(x.index_unchecked(4), y.index_unchecked(4), carry, carry);
		f5 = ::fast_io::intrinsics::addc(x.index_unchecked(5), y.index_unchecked(5), carry, carry);
		f6 = ::fast_io::intrinsics::addc(x.index_unchecked(6), y.index_unchecked(6), carry, carry);
		f7 = ::fast_io::intrinsics::addc(x.back_unchecked(), y.back_unchecked(), carry, carry);
		unsigned_type v{::fast_io::intrinsics::subc(zero, zero, carry, carry)};
		v &= static_cast<unsigned_type>(38);
		f0 = ::fast_io::intrinsics::addc(f0, v, false, carry);
		f1 = ::fast_io::intrinsics::addc(f1, zero, carry, carry);
		f2 = ::fast_io::intrinsics::addc(f2, zero, carry, carry);
		f3 = ::fast_io::intrinsics::addc(f3, zero, carry, carry);
		f4 = ::fast_io::intrinsics::addc(f4, zero, carry, carry);
		f5 = ::fast_io::intrinsics::addc(f5, zero, carry, carry);
		f6 = ::fast_io::intrinsics::addc(f6, zero, carry, carry);
		f7 = ::fast_io::intrinsics::addc(f7, zero, carry, carry);
		v = ::fast_io::intrinsics::subc(v, v, carry, carry);
		v &= static_cast<unsigned_type>(38);
		f.front_unchecked() = ::fast_io::intrinsics::addc(f0, v, false, carry);
		f.index_unchecked(1) = ::fast_io::intrinsics::addc(f1, zero, carry, carry);
		f.index_unchecked(2) = ::fast_io::intrinsics::addc(f2, zero, carry, carry);
		f.index_unchecked(3) = ::fast_io::intrinsics::addc(f3, zero, carry, carry);
		f.index_unchecked(4) = ::fast_io::intrinsics::addc(f4, zero, carry, carry);
		f.index_unchecked(5) = ::fast_io::intrinsics::addc(f5, zero, carry, carry);
		f.index_unchecked(6) = ::fast_io::intrinsics::addc(f6, zero, carry, carry);
		f.back_unchecked() = ::fast_io::intrinsics::addc(f7, zero, carry, carry);
	}
	else
	{
		unsigned_type f0 FAST_IO_INDETERMINATE, f1 FAST_IO_INDETERMINATE, f2 FAST_IO_INDETERMINATE, f3 FAST_IO_INDETERMINATE;
		bool carry FAST_IO_INDETERMINATE;
		f0 = ::fast_io::intrinsics::addc(x.front_unchecked(), y.front_unchecked(), false, carry);
		f1 = ::fast_io::intrinsics::addc(x.index_unchecked(1), y.index_unchecked(1), carry, carry);
		f2 = ::fast_io::intrinsics::addc(x.index_unchecked(2), y.index_unchecked(2), carry, carry);
		f3 = ::fast_io::intrinsics::addc(x.back_unchecked(), y.back_unchecked(), carry, carry);
		unsigned_type v{::fast_io::intrinsics::subc(zero, zero, carry, carry)};
		v &= static_cast<unsigned_type>(38);
		f0 = ::fast_io::intrinsics::addc(f0, v, false, carry);
		f1 = ::fast_io::intrinsics::addc(f1, zero, carry, carry);
		f2 = ::fast_io::intrinsics::addc(f2, zero, carry, carry);
		f3 = ::fast_io::intrinsics::addc(f3, zero, carry, carry);
		v = ::fast_io::intrinsics::subc(v, v, carry, carry);
		v &= static_cast<unsigned_type>(38);
		f.front_unchecked() = ::fast_io::intrinsics::addc(f0, v, false, carry);
		f.index_unchecked(1) = ::fast_io::intrinsics::addc(f1, zero, carry, carry);
		f.index_unchecked(2) = ::fast_io::intrinsics::addc(f2, zero, carry, carry);
		f.back_unchecked() = ::fast_io::intrinsics::addc(f3, zero, carry, carry);
	}
}
/*
f=x-y
*/

inline constexpr ::fast_io::curve25519::field_number::value_type subtraction_u256(::fast_io::curve25519::field_number &f,
														   ::fast_io::curve25519::field_number const &x,
														   ::fast_io::curve25519::field_number const &y) noexcept
{
	using unsigned_type = ::fast_io::curve25519::field_number::value_type;
	bool borrow FAST_IO_INDETERMINATE;
	if constexpr (::std::same_as<unsigned_type, ::std::uint_least32_t>)
	{
		f.front_unchecked() = ::fast_io::intrinsics::subc(x.front_unchecked(), y.front_unchecked(), false, borrow);
		f.index_unchecked(1) = ::fast_io::intrinsics::subc(x.index_unchecked(1), y.index_unchecked(1), borrow, borrow);
		f.index_unchecked(2) = ::fast_io::intrinsics::subc(x.index_unchecked(2), y.index_unchecked(2), borrow, borrow);
		f.index_unchecked(3) = ::fast_io::intrinsics::subc(x.index_unchecked(3), y.index_unchecked(3), borrow, borrow);
		f.index_unchecked(4) = ::fast_io::intrinsics::subc(x.index_unchecked(4), y.index_unchecked(4), borrow, borrow);
		f.index_unchecked(5) = ::fast_io::intrinsics::subc(x.index_unchecked(5), y.index_unchecked(5), borrow, borrow);
		f.index_unchecked(6) = ::fast_io::intrinsics::subc(x.index_unchecked(6), y.index_unchecked(6), borrow, borrow);
		f.back_unchecked() = ::fast_io::intrinsics::subc(x.back_unchecked(), y.back_unchecked(), borrow, borrow);
	}
	else
	{
		f.front_unchecked() = ::fast_io::intrinsics::subc(x.front_unchecked(), y.front_unchecked(), false, borrow);
		f.index_unchecked(1) = ::fast_io::intrinsics::subc(x.index_unchecked(1), y.index_unchecked(1), borrow, borrow);
		f.index_unchecked(2) = ::fast_io::intrinsics::subc(x.index_unchecked(2), y.index_unchecked(2), borrow, borrow);
		f.back_unchecked() = ::fast_io::intrinsics::subc(x.back_unchecked(), y.back_unchecked(), borrow, borrow);
	}
	constexpr unsigned_type zero{};
	unsigned_type res{::fast_io::intrinsics::subc(zero, zero, borrow, borrow)};
	return res;
}
/*
f=(x-y) mod (2^255-19)
*/
inline constexpr void field_number_subtraction(::fast_io::curve25519::field_number &f,
											   ::fast_io::curve25519::field_number const &x,
											   ::fast_io::curve25519::field_number const &y) noexcept
{
	using unsigned_type = ::fast_io::curve25519::field_number::value_type;
	constexpr unsigned_type zero{};
	if constexpr (::std::same_as<unsigned_type, ::std::uint_least32_t>)
	{
		unsigned_type f0 FAST_IO_INDETERMINATE, f1 FAST_IO_INDETERMINATE, f2 FAST_IO_INDETERMINATE, f3 FAST_IO_INDETERMINATE;
		unsigned_type f4 FAST_IO_INDETERMINATE, f5 FAST_IO_INDETERMINATE, f6 FAST_IO_INDETERMINATE, f7 FAST_IO_INDETERMINATE;
		bool borrow FAST_IO_INDETERMINATE;
		f0 = ::fast_io::intrinsics::subc(x.front_unchecked(), y.front_unchecked(), false, borrow);
		f1 = ::fast_io::intrinsics::subc(x.index_unchecked(1), y.index_unchecked(1), borrow, borrow);
		f2 = ::fast_io::intrinsics::subc(x.index_unchecked(2), y.index_unchecked(2), borrow, borrow);
		f3 = ::fast_io::intrinsics::subc(x.index_unchecked(3), y.index_unchecked(3), borrow, borrow);
		f4 = ::fast_io::intrinsics::subc(x.index_unchecked(4), y.index_unchecked(4), borrow, borrow);
		f5 = ::fast_io::intrinsics::subc(x.index_unchecked(5), y.index_unchecked(5), borrow, borrow);
		f6 = ::fast_io::intrinsics::subc(x.index_unchecked(6), y.index_unchecked(6), borrow, borrow);
		f7 = ::fast_io::intrinsics::subc(x.back_unchecked(), y.back_unchecked(), borrow, borrow);
		unsigned_type v{::fast_io::intrinsics::subc(zero, zero, borrow, borrow)};
		v &= static_cast<unsigned_type>(38);
		f0 = ::fast_io::intrinsics::subc(f0, v, false, borrow);
		f1 = ::fast_io::intrinsics::subc(f1, zero, borrow, borrow);
		f2 = ::fast_io::intrinsics::subc(f2, zero, borrow, borrow);
		f3 = ::fast_io::intrinsics::subc(f3, zero, borrow, borrow);
		f4 = ::fast_io::intrinsics::subc(f4, zero, borrow, borrow);
		f5 = ::fast_io::intrinsics::subc(f5, zero, borrow, borrow);
		f6 = ::fast_io::intrinsics::subc(f6, zero, borrow, borrow);
		f7 = ::fast_io::intrinsics::subc(f7, zero, borrow, borrow);
		v = ::fast_io::intrinsics::subc(v, v, borrow, borrow);
		v &= static_cast<unsigned_type>(38);
		f.front_unchecked() = ::fast_io::intrinsics::subc(f0, v, false, borrow);
		f.index_unchecked(1) = ::fast_io::intrinsics::subc(f1, zero, borrow, borrow);
		f.index_unchecked(2) = ::fast_io::intrinsics::subc(f2, zero, borrow, borrow);
		f.index_unchecked(3) = ::fast_io::intrinsics::subc(f3, zero, borrow, borrow);
		f.index_unchecked(4) = ::fast_io::intrinsics::subc(f4, zero, borrow, borrow);
		f.index_unchecked(5) = ::fast_io::intrinsics::subc(f5, zero, borrow, borrow);
		f.index_unchecked(6) = ::fast_io::intrinsics::subc(f6, zero, borrow, borrow);
		f.back_unchecked() = ::fast_io::intrinsics::subc(f7, zero, borrow, borrow);
	}
	else
	{
		unsigned_type f0 FAST_IO_INDETERMINATE, f1 FAST_IO_INDETERMINATE, f2 FAST_IO_INDETERMINATE, f3 FAST_IO_INDETERMINATE;
		bool borrow FAST_IO_INDETERMINATE;
		f0 = ::fast_io::intrinsics::subc(x.front_unchecked(), y.front_unchecked(), false, borrow);
		f1 = ::fast_io::intrinsics::subc(x.index_unchecked(1), y.index_unchecked(1), borrow, borrow);
		f2 = ::fast_io::intrinsics::subc(x.index_unchecked(2), y.index_unchecked(2), borrow, borrow);
		f3 = ::fast_io::intrinsics::subc(x.back_unchecked(), y.back_unchecked(), borrow, borrow);
		unsigned_type v{::fast_io::intrinsics::subc(zero, zero, borrow, borrow)};
		v &= static_cast<unsigned_type>(38);
		f0 = ::fast_io::intrinsics::subc(f0, v, false, borrow);
		f1 = ::fast_io::intrinsics::subc(f1, zero, borrow, borrow);
		f2 = ::fast_io::intrinsics::subc(f2, zero, borrow, borrow);
		f3 = ::fast_io::intrinsics::subc(f3, zero, borrow, borrow);
		v = ::fast_io::intrinsics::subc(v, v, borrow, borrow);
		v &= static_cast<unsigned_type>(38);
		f.front_unchecked() = ::fast_io::intrinsics::subc(f0, v, false, borrow);
		f.index_unchecked(1) = ::fast_io::intrinsics::subc(f1, zero, borrow, borrow);
		f.index_unchecked(2) = ::fast_io::intrinsics::subc(f2, zero, borrow, borrow);
		f.back_unchecked() = ::fast_io::intrinsics::subc(f3, zero, borrow, borrow);
	}
}

/*
dest[offset..offset+N] += x*b (dest is a 512-bit accumulator absorbing a partial row)
*/
template <std::size_t offset, bool first = false>
inline constexpr void multiply_single(::fast_io::curve25519::field_number::value_type *dest, ::fast_io::curve25519::field_number const &x, ::fast_io::curve25519::field_number::value_type b) noexcept
{
	using unsigned_type = ::fast_io::curve25519::field_number::value_type;
	constexpr unsigned_type zero{};
	if constexpr (::std::same_as<unsigned_type, ::std::uint_least32_t>)
	{
		unsigned_type h0 FAST_IO_INDETERMINATE;
		unsigned_type t0{::fast_io::intrinsics::umul(x.front_unchecked(), b, h0)};
		unsigned_type h1 FAST_IO_INDETERMINATE;
		unsigned_type t1{::fast_io::intrinsics::umul(x.index_unchecked(1), b, h1)};
		unsigned_type h2 FAST_IO_INDETERMINATE;
		unsigned_type t2{::fast_io::intrinsics::umul(x.index_unchecked(2), b, h2)};
		unsigned_type h3 FAST_IO_INDETERMINATE;
		unsigned_type t3{::fast_io::intrinsics::umul(x.index_unchecked(3), b, h3)};
		unsigned_type h4 FAST_IO_INDETERMINATE;
		unsigned_type t4{::fast_io::intrinsics::umul(x.index_unchecked(4), b, h4)};
		unsigned_type h5 FAST_IO_INDETERMINATE;
		unsigned_type t5{::fast_io::intrinsics::umul(x.index_unchecked(5), b, h5)};
		unsigned_type h6 FAST_IO_INDETERMINATE;
		unsigned_type t6{::fast_io::intrinsics::umul(x.index_unchecked(6), b, h6)};
		unsigned_type h7 FAST_IO_INDETERMINATE;
		unsigned_type t7{::fast_io::intrinsics::umul(x.back_unchecked(), b, h7)};
		bool carry FAST_IO_INDETERMINATE;
		if constexpr (first)
		{
			dest[offset] = t0;
			dest[offset + 1] = ::fast_io::intrinsics::addc(t1, h0, false, carry);
			dest[offset + 2] = ::fast_io::intrinsics::addc(t2, h1, carry, carry);
			dest[offset + 3] = ::fast_io::intrinsics::addc(t3, h2, carry, carry);
			dest[offset + 4] = ::fast_io::intrinsics::addc(t4, h3, carry, carry);
			dest[offset + 5] = ::fast_io::intrinsics::addc(t5, h4, carry, carry);
			dest[offset + 6] = ::fast_io::intrinsics::addc(t6, h5, carry, carry);
			dest[offset + 7] = ::fast_io::intrinsics::addc(t7, h6, carry, carry);
			dest[offset + 8] = ::fast_io::intrinsics::addc(zero, h7, carry, carry);
		}
		else
		{
			t1 = ::fast_io::intrinsics::addc(t1, h0, false, carry);
			t2 = ::fast_io::intrinsics::addc(t2, h1, carry, carry);
			t3 = ::fast_io::intrinsics::addc(t3, h2, carry, carry);
			t4 = ::fast_io::intrinsics::addc(t4, h3, carry, carry);
			t5 = ::fast_io::intrinsics::addc(t5, h4, carry, carry);
			t6 = ::fast_io::intrinsics::addc(t6, h5, carry, carry);
			t7 = ::fast_io::intrinsics::addc(t7, h6, carry, carry);
			h7 = ::fast_io::intrinsics::addc(zero, h7, carry, carry);

			dest[offset] = ::fast_io::intrinsics::addc(t0, dest[offset], false, carry);
			dest[offset + 1] = ::fast_io::intrinsics::addc(t1, dest[offset + 1], carry, carry);
			dest[offset + 2] = ::fast_io::intrinsics::addc(t2, dest[offset + 2], carry, carry);
			dest[offset + 3] = ::fast_io::intrinsics::addc(t3, dest[offset + 3], carry, carry);
			dest[offset + 4] = ::fast_io::intrinsics::addc(t4, dest[offset + 4], carry, carry);
			dest[offset + 5] = ::fast_io::intrinsics::addc(t5, dest[offset + 5], carry, carry);
			dest[offset + 6] = ::fast_io::intrinsics::addc(t6, dest[offset + 6], carry, carry);
			dest[offset + 7] = ::fast_io::intrinsics::addc(t7, dest[offset + 7], carry, carry);
			dest[offset + 8] = ::fast_io::intrinsics::addc(zero, h7, carry, carry);
		}
	}
	else
	{
		unsigned_type h0 FAST_IO_INDETERMINATE;
		unsigned_type t0{::fast_io::intrinsics::umul(x.front_unchecked(), b, h0)};
		unsigned_type h1 FAST_IO_INDETERMINATE;
		unsigned_type t1{::fast_io::intrinsics::umul(x.index_unchecked(1), b, h1)};
		unsigned_type h2 FAST_IO_INDETERMINATE;
		unsigned_type t2{::fast_io::intrinsics::umul(x.index_unchecked(2), b, h2)};
		unsigned_type h3 FAST_IO_INDETERMINATE;
		unsigned_type t3{::fast_io::intrinsics::umul(x.back_unchecked(), b, h3)};
		bool carry FAST_IO_INDETERMINATE;
		if constexpr (first)
		{
			dest[offset] = t0;
			dest[offset + 1] = ::fast_io::intrinsics::addc(t1, h0, false, carry);
			dest[offset + 2] = ::fast_io::intrinsics::addc(t2, h1, carry, carry);
			dest[offset + 3] = ::fast_io::intrinsics::addc(t3, h2, carry, carry);
			dest[offset + 4] = ::fast_io::intrinsics::addc(zero, h3, carry, carry);
		}
		else
		{
			t1 = ::fast_io::intrinsics::addc(t1, h0, false, carry);
			t2 = ::fast_io::intrinsics::addc(t2, h1, carry, carry);
			t3 = ::fast_io::intrinsics::addc(t3, h2, carry, carry);
			h3 = ::fast_io::intrinsics::addc(zero, h3, carry, carry);

			dest[offset] = ::fast_io::intrinsics::addc(t0, dest[offset], false, carry);
			dest[offset + 1] = ::fast_io::intrinsics::addc(t1, dest[offset + 1], carry, carry);
			dest[offset + 2] = ::fast_io::intrinsics::addc(t2, dest[offset + 2], carry, carry);
			dest[offset + 3] = ::fast_io::intrinsics::addc(t3, dest[offset + 3], carry, carry);
			dest[offset + 4] = ::fast_io::intrinsics::addc(zero, h3, carry, carry);
		}
	}
}

/*
z = y + constant*x, then fold the carry with *38. Constant-time.
*/
inline constexpr void multiplication_add_reduce(::fast_io::curve25519::field_number &z, ::fast_io::curve25519::field_number const &y, ::fast_io::curve25519::field_number::value_type constant, ::fast_io::curve25519::field_number const &x) noexcept
{
	using unsigned_type = ::fast_io::curve25519::field_number::value_type;
	constexpr unsigned_type zero{};
	if constexpr (::std::same_as<unsigned_type, ::std::uint_least32_t>)
	{
		unsigned_type h0 FAST_IO_INDETERMINATE;
		unsigned_type t0{::fast_io::intrinsics::umul(x.front_unchecked(), constant, h0)};
		unsigned_type h1 FAST_IO_INDETERMINATE;
		unsigned_type t1{::fast_io::intrinsics::umul(x.index_unchecked(1), constant, h1)};
		unsigned_type h2 FAST_IO_INDETERMINATE;
		unsigned_type t2{::fast_io::intrinsics::umul(x.index_unchecked(2), constant, h2)};
		unsigned_type h3 FAST_IO_INDETERMINATE;
		unsigned_type t3{::fast_io::intrinsics::umul(x.index_unchecked(3), constant, h3)};
		unsigned_type h4 FAST_IO_INDETERMINATE;
		unsigned_type t4{::fast_io::intrinsics::umul(x.index_unchecked(4), constant, h4)};
		unsigned_type h5 FAST_IO_INDETERMINATE;
		unsigned_type t5{::fast_io::intrinsics::umul(x.index_unchecked(5), constant, h5)};
		unsigned_type h6 FAST_IO_INDETERMINATE;
		unsigned_type t6{::fast_io::intrinsics::umul(x.index_unchecked(6), constant, h6)};
		unsigned_type h7 FAST_IO_INDETERMINATE;
		unsigned_type t7{::fast_io::intrinsics::umul(x.back_unchecked(), constant, h7)};
		bool carry FAST_IO_INDETERMINATE;
		t0 = ::fast_io::intrinsics::addc(t0, y.front_unchecked(), false, carry);
		t1 = ::fast_io::intrinsics::addc(t1, y.index_unchecked(1), carry, carry);
		t2 = ::fast_io::intrinsics::addc(t2, y.index_unchecked(2), carry, carry);
		t3 = ::fast_io::intrinsics::addc(t3, y.index_unchecked(3), carry, carry);
		t4 = ::fast_io::intrinsics::addc(t4, y.index_unchecked(4), carry, carry);
		t5 = ::fast_io::intrinsics::addc(t5, y.index_unchecked(5), carry, carry);
		t6 = ::fast_io::intrinsics::addc(t6, y.index_unchecked(6), carry, carry);
		t7 = ::fast_io::intrinsics::addc(t7, y.back_unchecked(), carry, carry);
		h7 = ::fast_io::intrinsics::addc(h7, zero, carry, carry);

		t1 = ::fast_io::intrinsics::addc(t1, h0, false, carry);
		t2 = ::fast_io::intrinsics::addc(t2, h1, carry, carry);
		t3 = ::fast_io::intrinsics::addc(t3, h2, carry, carry);
		t4 = ::fast_io::intrinsics::addc(t4, h3, carry, carry);
		t5 = ::fast_io::intrinsics::addc(t5, h4, carry, carry);
		t6 = ::fast_io::intrinsics::addc(t6, h5, carry, carry);
		t7 = ::fast_io::intrinsics::addc(t7, h6, carry, carry);
		h7 = ::fast_io::intrinsics::addc(h7, zero, carry, carry);

		constexpr unsigned_type constant38{38};
		unsigned_type v FAST_IO_INDETERMINATE;
		unsigned_type const tf{::fast_io::intrinsics::umul(h7, constant38, v)};
		t0 = ::fast_io::intrinsics::addc(t0, tf, false, carry);
		t1 = ::fast_io::intrinsics::addc(t1, v, carry, carry);
		t2 = ::fast_io::intrinsics::addc(t2, zero, carry, carry);
		t3 = ::fast_io::intrinsics::addc(t3, zero, carry, carry);
		t4 = ::fast_io::intrinsics::addc(t4, zero, carry, carry);
		t5 = ::fast_io::intrinsics::addc(t5, zero, carry, carry);
		t6 = ::fast_io::intrinsics::addc(t6, zero, carry, carry);
		t7 = ::fast_io::intrinsics::addc(t7, zero, carry, carry);

		v = ::fast_io::intrinsics::subc(zero, zero, carry, carry);
		v &= constant38;
		z.front_unchecked() = ::fast_io::intrinsics::addc(t0, v, false, carry);
		z.index_unchecked(1) = ::fast_io::intrinsics::addc(t1, zero, carry, carry);
		z.index_unchecked(2) = ::fast_io::intrinsics::addc(t2, zero, carry, carry);
		z.index_unchecked(3) = ::fast_io::intrinsics::addc(t3, zero, carry, carry);
		z.index_unchecked(4) = ::fast_io::intrinsics::addc(t4, zero, carry, carry);
		z.index_unchecked(5) = ::fast_io::intrinsics::addc(t5, zero, carry, carry);
		z.index_unchecked(6) = ::fast_io::intrinsics::addc(t6, zero, carry, carry);
		z.back_unchecked() = ::fast_io::intrinsics::addc(t7, zero, carry, carry);
	}
	else
	{
		unsigned_type h0 FAST_IO_INDETERMINATE;
		unsigned_type t0{::fast_io::intrinsics::umul(x.front_unchecked(), constant, h0)};
		unsigned_type h1 FAST_IO_INDETERMINATE;
		unsigned_type t1{::fast_io::intrinsics::umul(x.index_unchecked(1), constant, h1)};
		unsigned_type h2 FAST_IO_INDETERMINATE;
		unsigned_type t2{::fast_io::intrinsics::umul(x.index_unchecked(2), constant, h2)};
		unsigned_type h3 FAST_IO_INDETERMINATE;
		unsigned_type t3{::fast_io::intrinsics::umul(x.back_unchecked(), constant, h3)};

		bool carry FAST_IO_INDETERMINATE;
		t0 = ::fast_io::intrinsics::addc(t0, y.front_unchecked(), false, carry);
		t1 = ::fast_io::intrinsics::addc(t1, y.index_unchecked(1), carry, carry);
		t2 = ::fast_io::intrinsics::addc(t2, y.index_unchecked(2), carry, carry);
		t3 = ::fast_io::intrinsics::addc(t3, y.back_unchecked(), carry, carry);
		h3 = ::fast_io::intrinsics::addc(h3, zero, carry, carry);


		t1 = ::fast_io::intrinsics::addc(t1, h0, false, carry);
		t2 = ::fast_io::intrinsics::addc(t2, h1, carry, carry);
		t3 = ::fast_io::intrinsics::addc(t3, h2, carry, carry);
		h3 = ::fast_io::intrinsics::addc(h3, zero, carry, carry);

		constexpr unsigned_type constant38{38};

		unsigned_type v FAST_IO_INDETERMINATE;
		unsigned_type t4{::fast_io::intrinsics::umul(h3, constant38, v)};
		t0 = ::fast_io::intrinsics::addc(t0, t4, false, carry);
		t1 = ::fast_io::intrinsics::addc(t1, v, carry, carry);
		t2 = ::fast_io::intrinsics::addc(t2, zero, carry, carry);
		t3 = ::fast_io::intrinsics::addc(t3, zero, carry, carry);


		t4 = ::fast_io::intrinsics::subc(zero, zero, carry, carry);
		t4 &= constant38;
		z.front_unchecked() = ::fast_io::intrinsics::addc(t0, t4, false, carry);
		z.index_unchecked(1) = ::fast_io::intrinsics::addc(t1, zero, carry, carry);
		z.index_unchecked(2) = ::fast_io::intrinsics::addc(t2, zero, carry, carry);
		z.back_unchecked() = ::fast_io::intrinsics::addc(t3, zero, carry, carry);
	}
}

/*
wide-input variant: z = t[0..N-1] + constant*t[N..2N-1]
*/
inline constexpr void multiplication_add_reduce_hi(::fast_io::curve25519::field_number &z, ::fast_io::curve25519::field_number::value_type const *t, ::fast_io::curve25519::field_number::value_type constant) noexcept
{
	using unsigned_type = ::fast_io::curve25519::field_number::value_type;
	constexpr unsigned_type zero{};
	if constexpr (::std::same_as<unsigned_type, ::std::uint_least32_t>)
	{
		unsigned_type h0 FAST_IO_INDETERMINATE;
		unsigned_type t0{::fast_io::intrinsics::umul(t[8 + 0], constant, h0)};
		unsigned_type h1 FAST_IO_INDETERMINATE;
		unsigned_type t1{::fast_io::intrinsics::umul(t[8 + 1], constant, h1)};
		unsigned_type h2 FAST_IO_INDETERMINATE;
		unsigned_type t2{::fast_io::intrinsics::umul(t[8 + 2], constant, h2)};
		unsigned_type h3 FAST_IO_INDETERMINATE;
		unsigned_type t3{::fast_io::intrinsics::umul(t[8 + 3], constant, h3)};
		unsigned_type h4 FAST_IO_INDETERMINATE;
		unsigned_type t4{::fast_io::intrinsics::umul(t[8 + 4], constant, h4)};
		unsigned_type h5 FAST_IO_INDETERMINATE;
		unsigned_type t5{::fast_io::intrinsics::umul(t[8 + 5], constant, h5)};
		unsigned_type h6 FAST_IO_INDETERMINATE;
		unsigned_type t6{::fast_io::intrinsics::umul(t[8 + 6], constant, h6)};
		unsigned_type h7 FAST_IO_INDETERMINATE;
		unsigned_type t7{::fast_io::intrinsics::umul(t[8 + 7], constant, h7)};
		bool carry FAST_IO_INDETERMINATE;
		t0 = ::fast_io::intrinsics::addc(t0, t[0], false, carry);
		t1 = ::fast_io::intrinsics::addc(t1, t[1], carry, carry);
		t2 = ::fast_io::intrinsics::addc(t2, t[2], carry, carry);
		t3 = ::fast_io::intrinsics::addc(t3, t[3], carry, carry);
		t4 = ::fast_io::intrinsics::addc(t4, t[4], carry, carry);
		t5 = ::fast_io::intrinsics::addc(t5, t[5], carry, carry);
		t6 = ::fast_io::intrinsics::addc(t6, t[6], carry, carry);
		t7 = ::fast_io::intrinsics::addc(t7, t[7], carry, carry);
		h7 = ::fast_io::intrinsics::addc(h7, zero, carry, carry);

		t1 = ::fast_io::intrinsics::addc(t1, h0, false, carry);
		t2 = ::fast_io::intrinsics::addc(t2, h1, carry, carry);
		t3 = ::fast_io::intrinsics::addc(t3, h2, carry, carry);
		t4 = ::fast_io::intrinsics::addc(t4, h3, carry, carry);
		t5 = ::fast_io::intrinsics::addc(t5, h4, carry, carry);
		t6 = ::fast_io::intrinsics::addc(t6, h5, carry, carry);
		t7 = ::fast_io::intrinsics::addc(t7, h6, carry, carry);
		h7 = ::fast_io::intrinsics::addc(h7, zero, carry, carry);

		constexpr unsigned_type constant38{38};
		unsigned_type v FAST_IO_INDETERMINATE;
		unsigned_type const tf{::fast_io::intrinsics::umul(h7, constant38, v)};
		t0 = ::fast_io::intrinsics::addc(t0, tf, false, carry);
		t1 = ::fast_io::intrinsics::addc(t1, v, carry, carry);
		t2 = ::fast_io::intrinsics::addc(t2, zero, carry, carry);
		t3 = ::fast_io::intrinsics::addc(t3, zero, carry, carry);
		t4 = ::fast_io::intrinsics::addc(t4, zero, carry, carry);
		t5 = ::fast_io::intrinsics::addc(t5, zero, carry, carry);
		t6 = ::fast_io::intrinsics::addc(t6, zero, carry, carry);
		t7 = ::fast_io::intrinsics::addc(t7, zero, carry, carry);

		v = ::fast_io::intrinsics::subc(zero, zero, carry, carry);
		v &= constant38;
		z.front_unchecked() = ::fast_io::intrinsics::addc(t0, v, false, carry);
		z.index_unchecked(1) = ::fast_io::intrinsics::addc(t1, zero, carry, carry);
		z.index_unchecked(2) = ::fast_io::intrinsics::addc(t2, zero, carry, carry);
		z.index_unchecked(3) = ::fast_io::intrinsics::addc(t3, zero, carry, carry);
		z.index_unchecked(4) = ::fast_io::intrinsics::addc(t4, zero, carry, carry);
		z.index_unchecked(5) = ::fast_io::intrinsics::addc(t5, zero, carry, carry);
		z.index_unchecked(6) = ::fast_io::intrinsics::addc(t6, zero, carry, carry);
		z.index_unchecked(7) = ::fast_io::intrinsics::addc(t7, zero, carry, carry);
	}
	else
	{
		unsigned_type h0 FAST_IO_INDETERMINATE;
		unsigned_type t0{::fast_io::intrinsics::umul(t[4], constant, h0)};
		unsigned_type h1 FAST_IO_INDETERMINATE;
		unsigned_type t1{::fast_io::intrinsics::umul(t[5], constant, h1)};
		unsigned_type h2 FAST_IO_INDETERMINATE;
		unsigned_type t2{::fast_io::intrinsics::umul(t[6], constant, h2)};
		unsigned_type h3 FAST_IO_INDETERMINATE;
		unsigned_type t3{::fast_io::intrinsics::umul(t[7], constant, h3)};

		bool carry FAST_IO_INDETERMINATE;
		t0 = ::fast_io::intrinsics::addc(t0, t[0], false, carry);
		t1 = ::fast_io::intrinsics::addc(t1, t[1], carry, carry);
		t2 = ::fast_io::intrinsics::addc(t2, t[2], carry, carry);
		t3 = ::fast_io::intrinsics::addc(t3, t[3], carry, carry);
		h3 = ::fast_io::intrinsics::addc(h3, zero, carry, carry);


		t1 = ::fast_io::intrinsics::addc(t1, h0, false, carry);
		t2 = ::fast_io::intrinsics::addc(t2, h1, carry, carry);
		t3 = ::fast_io::intrinsics::addc(t3, h2, carry, carry);
		h3 = ::fast_io::intrinsics::addc(h3, zero, carry, carry);

		constexpr unsigned_type constant38{38};

		unsigned_type v FAST_IO_INDETERMINATE;
		unsigned_type t4{::fast_io::intrinsics::umul(h3, constant38, v)};
		t0 = ::fast_io::intrinsics::addc(t0, t4, false, carry);
		t1 = ::fast_io::intrinsics::addc(t1, v, carry, carry);
		t2 = ::fast_io::intrinsics::addc(t2, zero, carry, carry);
		t3 = ::fast_io::intrinsics::addc(t3, zero, carry, carry);


		t4 = ::fast_io::intrinsics::subc(zero, zero, carry, carry);
		t4 &= constant38;
		z.front_unchecked() = ::fast_io::intrinsics::addc(t0, t4, false, carry);
		z.index_unchecked(1) = ::fast_io::intrinsics::addc(t1, zero, carry, carry);
		z.index_unchecked(2) = ::fast_io::intrinsics::addc(t2, zero, carry, carry);
		z.back_unchecked() = ::fast_io::intrinsics::addc(t3, zero, carry, carry);
	}
}

inline constexpr void reduce_final(::fast_io::curve25519::field_number &f, ::fast_io::curve25519::field_number::value_type const *x) noexcept
{
	constexpr ::fast_io::curve25519::field_number::value_type constant{38};
	multiplication_add_reduce_hi(f, x, constant);
}


inline constexpr void field_number_multiplication(::fast_io::curve25519::field_number &r, ::fast_io::curve25519::field_number const &x, ::fast_io::curve25519::field_number const &y) noexcept
{
	::fast_io::curve25519::field_number::value_type t FAST_IO_INDETERMINATE[::fast_io::curve25519::field_number::array_size * 2];
	if constexpr (::std::same_as<::fast_io::curve25519::field_number::value_type, ::std::uint_least32_t>)
	{
		multiply_single<0, true>(t, x, y.index_unchecked(0));
		multiply_single<1>(t, x, y.index_unchecked(1));
		multiply_single<2>(t, x, y.index_unchecked(2));
		multiply_single<3>(t, x, y.index_unchecked(3));
		multiply_single<4>(t, x, y.index_unchecked(4));
		multiply_single<5>(t, x, y.index_unchecked(5));
		multiply_single<6>(t, x, y.index_unchecked(6));
		multiply_single<7>(t, x, y.index_unchecked(7));
	}
	else
	{
		multiply_single<0, true>(t, x, y.front_unchecked());
		multiply_single<1>(t, x, y.index_unchecked(1));
		multiply_single<2>(t, x, y.index_unchecked(2));
		multiply_single<3>(t, x, y.back_unchecked());
	}
	reduce_final(r, t);
}

inline constexpr ::fast_io::curve25519::field_number::value_type muladd_w0(::fast_io::curve25519::field_number::value_type add_value, ::fast_io::curve25519::field_number::value_type mul_value, ::fast_io::curve25519::field_number::value_type &high) noexcept
{
	constexpr ::fast_io::curve25519::field_number::value_type constant{38};
	constexpr ::fast_io::curve25519::field_number::value_type zero{};
	::fast_io::curve25519::field_number::value_type low{::fast_io::intrinsics::umul(mul_value, constant, high)};
	bool carry FAST_IO_INDETERMINATE;
	low = ::fast_io::intrinsics::addc(add_value, low, false, carry);
	high = ::fast_io::intrinsics::addc(zero, high, carry, carry);
	return low;
}

inline constexpr ::fast_io::curve25519::field_number::value_type muladd_w1(::fast_io::curve25519::field_number::value_type add_value, ::fast_io::curve25519::field_number::value_type mul_value, ::fast_io::curve25519::field_number::value_type &high, ::fast_io::curve25519::field_number::value_type last_high) noexcept
{
	constexpr ::fast_io::curve25519::field_number::value_type constant{38};
	constexpr ::fast_io::curve25519::field_number::value_type zero{};
	::fast_io::curve25519::field_number::value_type low{::fast_io::intrinsics::umul(mul_value, constant, high)};
	bool carry FAST_IO_INDETERMINATE;
	low = ::fast_io::intrinsics::addc(last_high, low, false, carry);
	high = ::fast_io::intrinsics::addc(zero, high, carry, carry);
	low = ::fast_io::intrinsics::addc(add_value, low, false, carry);
	high = ::fast_io::intrinsics::addc(zero, high, carry, carry);
	return low;
}

inline constexpr void field_number_square(::fast_io::curve25519::field_number &r, ::fast_io::curve25519::field_number const &x) noexcept
{
	if constexpr (::std::same_as<::fast_io::curve25519::field_number::value_type, ::std::uint_least32_t>)
	{
		using unsigned_type = ::fast_io::curve25519::field_number::value_type;
		constexpr unsigned_type zero{};
		constexpr ::std::size_t n{::fast_io::curve25519::field_number::array_size};

		/*
		x^2 = sum_i x_i^2 * B^(2i) + 2 * sum_{i<j} x_i*x_j * B^(i+j).
		Row i computes x_i*x_j for j>i; each product's hi half lands one
		column right and merges into the same carry chain.  The chain is
		doubled, then the diagonal squares are chained in, and
		reduce_final folds the top half with the *38 trick.
		*/
		if constexpr(false)
		{
			unsigned_type t[n << 1]{};
			unsigned_type lows FAST_IO_INDETERMINATE[n + 1];
			unsigned_type highs FAST_IO_INDETERMINATE[n];
			unsigned_type dlo FAST_IO_INDETERMINATE;
			unsigned_type dhi FAST_IO_INDETERMINATE;
			bool carry{};
			for (::std::size_t i{1}; i != n; ++i)
			{
				for (::std::size_t j{}; j != i; ++j)
				{
					lows[j] = ::fast_io::intrinsics::umul(x.index_unchecked(i), x.index_unchecked(j), highs[j]);
				}
				for (::std::size_t j{1}; j != i; ++j)
				{
					lows[j] = ::fast_io::intrinsics::addc(lows[j], highs[j - 1], carry, carry);
				}
				lows[i] = ::fast_io::intrinsics::addc(highs[i - 1], zero, carry, carry);
				lows[i + 1] = ::fast_io::intrinsics::addc(zero, zero, carry, carry);
				for (::std::size_t k{}; k != i + 2; ++k)
				{
					t[i + k] = ::fast_io::intrinsics::addc(lows[k], t[i + k], carry, carry);
				}
				for (::std::size_t k{(i << 1) + 2}; k != (n << 1); ++k)
				{
					t[k] = ::fast_io::intrinsics::addc(t[k], zero, carry, carry);
				}
			}
			for (::std::size_t k{}; k != (n << 1); ++k)
			{
				t[k] = ::fast_io::intrinsics::addc(t[k], t[k], carry, carry);
			}
			for (::std::size_t i{}; i != n; ++i)
			{
				dlo = ::fast_io::intrinsics::umul(x.index_unchecked(i), x.index_unchecked(i), dhi);
				t[i << 1] = ::fast_io::intrinsics::addc(dlo, t[i << 1], carry, carry);
				t[(i << 1) + 1] = ::fast_io::intrinsics::addc(dhi, t[(i << 1) + 1], carry, carry);
			}
			reduce_final(r, t);
		}
		else
		{
		unsigned_type t FAST_IO_INDETERMINATE[n << 1];
		bool carry FAST_IO_INDETERMINATE;
		{
		unsigned_type const b0{x.front_unchecked()};
		unsigned_type h01 FAST_IO_INDETERMINATE;
		unsigned_type const p01{::fast_io::intrinsics::umul(x.index_unchecked(1), b0, h01)};
		unsigned_type h02 FAST_IO_INDETERMINATE;
		unsigned_type const p02{::fast_io::intrinsics::umul(x.index_unchecked(2), b0, h02)};
		unsigned_type h03 FAST_IO_INDETERMINATE;
		unsigned_type const p03{::fast_io::intrinsics::umul(x.index_unchecked(3), b0, h03)};
		unsigned_type h04 FAST_IO_INDETERMINATE;
		unsigned_type const p04{::fast_io::intrinsics::umul(x.index_unchecked(4), b0, h04)};
		unsigned_type h05 FAST_IO_INDETERMINATE;
		unsigned_type const p05{::fast_io::intrinsics::umul(x.index_unchecked(5), b0, h05)};
		unsigned_type h06 FAST_IO_INDETERMINATE;
		unsigned_type const p06{::fast_io::intrinsics::umul(x.index_unchecked(6), b0, h06)};
		unsigned_type h07 FAST_IO_INDETERMINATE;
		unsigned_type const p07{::fast_io::intrinsics::umul(x.index_unchecked(7), b0, h07)};
		unsigned_type m02{::fast_io::intrinsics::addc(p02, h01, false, carry)};
		unsigned_type m03{::fast_io::intrinsics::addc(p03, h02, carry, carry)};
		unsigned_type m04{::fast_io::intrinsics::addc(p04, h03, carry, carry)};
		unsigned_type m05{::fast_io::intrinsics::addc(p05, h04, carry, carry)};
		unsigned_type m06{::fast_io::intrinsics::addc(p06, h05, carry, carry)};
		unsigned_type m07{::fast_io::intrinsics::addc(p07, h06, carry, carry)};
		unsigned_type m08{::fast_io::intrinsics::addc(h07, zero, carry, carry)};
		t[1] = p01;
		t[2] = ::fast_io::intrinsics::addc(m02, zero, false, carry);
		t[3] = ::fast_io::intrinsics::addc(m03, zero, carry, carry);
		t[4] = ::fast_io::intrinsics::addc(m04, zero, carry, carry);
		t[5] = ::fast_io::intrinsics::addc(m05, zero, carry, carry);
		t[6] = ::fast_io::intrinsics::addc(m06, zero, carry, carry);
		t[7] = ::fast_io::intrinsics::addc(m07, zero, carry, carry);
		t[8] = ::fast_io::intrinsics::addc(m08, zero, carry, carry);
		t[9] = ::fast_io::intrinsics::addc(zero, zero, carry, carry);
		t[10] = ::fast_io::intrinsics::addc(zero, zero, carry, carry);
		t[11] = ::fast_io::intrinsics::addc(zero, zero, carry, carry);
		t[12] = ::fast_io::intrinsics::addc(zero, zero, carry, carry);
		t[13] = ::fast_io::intrinsics::addc(zero, zero, carry, carry);
		t[14] = ::fast_io::intrinsics::addc(zero, zero, carry, carry);
		t[15] = ::fast_io::intrinsics::addc(zero, zero, carry, carry);
		}
		{
		unsigned_type const b1{x.index_unchecked(1)};
		unsigned_type h12 FAST_IO_INDETERMINATE;
		unsigned_type const p12{::fast_io::intrinsics::umul(x.index_unchecked(2), b1, h12)};
		unsigned_type h13 FAST_IO_INDETERMINATE;
		unsigned_type const p13{::fast_io::intrinsics::umul(x.index_unchecked(3), b1, h13)};
		unsigned_type h14 FAST_IO_INDETERMINATE;
		unsigned_type const p14{::fast_io::intrinsics::umul(x.index_unchecked(4), b1, h14)};
		unsigned_type h15 FAST_IO_INDETERMINATE;
		unsigned_type const p15{::fast_io::intrinsics::umul(x.index_unchecked(5), b1, h15)};
		unsigned_type h16 FAST_IO_INDETERMINATE;
		unsigned_type const p16{::fast_io::intrinsics::umul(x.index_unchecked(6), b1, h16)};
		unsigned_type h17 FAST_IO_INDETERMINATE;
		unsigned_type const p17{::fast_io::intrinsics::umul(x.index_unchecked(7), b1, h17)};
		unsigned_type m12{p12};
		unsigned_type m13{::fast_io::intrinsics::addc(p13, h12, false, carry)};
		unsigned_type m14{::fast_io::intrinsics::addc(p14, h13, carry, carry)};
		unsigned_type m15{::fast_io::intrinsics::addc(p15, h14, carry, carry)};
		unsigned_type m16{::fast_io::intrinsics::addc(p16, h15, carry, carry)};
		unsigned_type m17{::fast_io::intrinsics::addc(p17, h16, carry, carry)};
		unsigned_type m18{::fast_io::intrinsics::addc(h17, zero, carry, carry)};
		t[3] = ::fast_io::intrinsics::addc(t[3], m12, false, carry);
		t[4] = ::fast_io::intrinsics::addc(t[4], m13, carry, carry);
		t[5] = ::fast_io::intrinsics::addc(t[5], m14, carry, carry);
		t[6] = ::fast_io::intrinsics::addc(t[6], m15, carry, carry);
		t[7] = ::fast_io::intrinsics::addc(t[7], m16, carry, carry);
		t[8] = ::fast_io::intrinsics::addc(t[8], m17, carry, carry);
		t[9] = ::fast_io::intrinsics::addc(t[9], m18, carry, carry);
		t[10] = ::fast_io::intrinsics::addc(t[10], zero, carry, carry);
		t[11] = ::fast_io::intrinsics::addc(t[11], zero, carry, carry);
		t[12] = ::fast_io::intrinsics::addc(t[12], zero, carry, carry);
		t[13] = ::fast_io::intrinsics::addc(t[13], zero, carry, carry);
		t[14] = ::fast_io::intrinsics::addc(t[14], zero, carry, carry);
		t[15] = ::fast_io::intrinsics::addc(t[15], zero, carry, carry);
		}
		{
		unsigned_type const b2{x.index_unchecked(2)};
		unsigned_type h23 FAST_IO_INDETERMINATE;
		unsigned_type const p23{::fast_io::intrinsics::umul(x.index_unchecked(3), b2, h23)};
		unsigned_type h24 FAST_IO_INDETERMINATE;
		unsigned_type const p24{::fast_io::intrinsics::umul(x.index_unchecked(4), b2, h24)};
		unsigned_type h25 FAST_IO_INDETERMINATE;
		unsigned_type const p25{::fast_io::intrinsics::umul(x.index_unchecked(5), b2, h25)};
		unsigned_type h26 FAST_IO_INDETERMINATE;
		unsigned_type const p26{::fast_io::intrinsics::umul(x.index_unchecked(6), b2, h26)};
		unsigned_type h27 FAST_IO_INDETERMINATE;
		unsigned_type const p27{::fast_io::intrinsics::umul(x.index_unchecked(7), b2, h27)};
		unsigned_type m23{p23};
		unsigned_type m24{::fast_io::intrinsics::addc(p24, h23, false, carry)};
		unsigned_type m25{::fast_io::intrinsics::addc(p25, h24, carry, carry)};
		unsigned_type m26{::fast_io::intrinsics::addc(p26, h25, carry, carry)};
		unsigned_type m27{::fast_io::intrinsics::addc(p27, h26, carry, carry)};
		unsigned_type m28{::fast_io::intrinsics::addc(h27, zero, carry, carry)};
		t[5] = ::fast_io::intrinsics::addc(t[5], m23, false, carry);
		t[6] = ::fast_io::intrinsics::addc(t[6], m24, carry, carry);
		t[7] = ::fast_io::intrinsics::addc(t[7], m25, carry, carry);
		t[8] = ::fast_io::intrinsics::addc(t[8], m26, carry, carry);
		t[9] = ::fast_io::intrinsics::addc(t[9], m27, carry, carry);
		t[10] = ::fast_io::intrinsics::addc(t[10], m28, carry, carry);
		t[11] = ::fast_io::intrinsics::addc(t[11], zero, carry, carry);
		t[12] = ::fast_io::intrinsics::addc(t[12], zero, carry, carry);
		t[13] = ::fast_io::intrinsics::addc(t[13], zero, carry, carry);
		t[14] = ::fast_io::intrinsics::addc(t[14], zero, carry, carry);
		t[15] = ::fast_io::intrinsics::addc(t[15], zero, carry, carry);
		}
		{
		unsigned_type const b3{x.index_unchecked(3)};
		unsigned_type h34 FAST_IO_INDETERMINATE;
		unsigned_type const p34{::fast_io::intrinsics::umul(x.index_unchecked(4), b3, h34)};
		unsigned_type h35 FAST_IO_INDETERMINATE;
		unsigned_type const p35{::fast_io::intrinsics::umul(x.index_unchecked(5), b3, h35)};
		unsigned_type h36 FAST_IO_INDETERMINATE;
		unsigned_type const p36{::fast_io::intrinsics::umul(x.index_unchecked(6), b3, h36)};
		unsigned_type h37 FAST_IO_INDETERMINATE;
		unsigned_type const p37{::fast_io::intrinsics::umul(x.index_unchecked(7), b3, h37)};
		unsigned_type m34{p34};
		unsigned_type m35{::fast_io::intrinsics::addc(p35, h34, false, carry)};
		unsigned_type m36{::fast_io::intrinsics::addc(p36, h35, carry, carry)};
		unsigned_type m37{::fast_io::intrinsics::addc(p37, h36, carry, carry)};
		unsigned_type m38{::fast_io::intrinsics::addc(h37, zero, carry, carry)};
		t[7] = ::fast_io::intrinsics::addc(t[7], m34, false, carry);
		t[8] = ::fast_io::intrinsics::addc(t[8], m35, carry, carry);
		t[9] = ::fast_io::intrinsics::addc(t[9], m36, carry, carry);
		t[10] = ::fast_io::intrinsics::addc(t[10], m37, carry, carry);
		t[11] = ::fast_io::intrinsics::addc(t[11], m38, carry, carry);
		t[12] = ::fast_io::intrinsics::addc(t[12], zero, carry, carry);
		t[13] = ::fast_io::intrinsics::addc(t[13], zero, carry, carry);
		t[14] = ::fast_io::intrinsics::addc(t[14], zero, carry, carry);
		t[15] = ::fast_io::intrinsics::addc(t[15], zero, carry, carry);
		}
		{
		unsigned_type const b4{x.index_unchecked(4)};
		unsigned_type h45 FAST_IO_INDETERMINATE;
		unsigned_type const p45{::fast_io::intrinsics::umul(x.index_unchecked(5), b4, h45)};
		unsigned_type h46 FAST_IO_INDETERMINATE;
		unsigned_type const p46{::fast_io::intrinsics::umul(x.index_unchecked(6), b4, h46)};
		unsigned_type h47 FAST_IO_INDETERMINATE;
		unsigned_type const p47{::fast_io::intrinsics::umul(x.index_unchecked(7), b4, h47)};
		unsigned_type m45{p45};
		unsigned_type m46{::fast_io::intrinsics::addc(p46, h45, false, carry)};
		unsigned_type m47{::fast_io::intrinsics::addc(p47, h46, carry, carry)};
		unsigned_type m48{::fast_io::intrinsics::addc(h47, zero, carry, carry)};
		t[9] = ::fast_io::intrinsics::addc(t[9], m45, false, carry);
		t[10] = ::fast_io::intrinsics::addc(t[10], m46, carry, carry);
		t[11] = ::fast_io::intrinsics::addc(t[11], m47, carry, carry);
		t[12] = ::fast_io::intrinsics::addc(t[12], m48, carry, carry);
		t[13] = ::fast_io::intrinsics::addc(t[13], zero, carry, carry);
		t[14] = ::fast_io::intrinsics::addc(t[14], zero, carry, carry);
		t[15] = ::fast_io::intrinsics::addc(t[15], zero, carry, carry);
		}
		{
		unsigned_type const b5{x.index_unchecked(5)};
		unsigned_type h56 FAST_IO_INDETERMINATE;
		unsigned_type const p56{::fast_io::intrinsics::umul(x.index_unchecked(6), b5, h56)};
		unsigned_type h57 FAST_IO_INDETERMINATE;
		unsigned_type const p57{::fast_io::intrinsics::umul(x.index_unchecked(7), b5, h57)};
		unsigned_type m56{p56};
		unsigned_type m57{::fast_io::intrinsics::addc(p57, h56, false, carry)};
		unsigned_type m58{::fast_io::intrinsics::addc(h57, zero, carry, carry)};
		t[11] = ::fast_io::intrinsics::addc(t[11], m56, false, carry);
		t[12] = ::fast_io::intrinsics::addc(t[12], m57, carry, carry);
		t[13] = ::fast_io::intrinsics::addc(t[13], m58, carry, carry);
		t[14] = ::fast_io::intrinsics::addc(t[14], zero, carry, carry);
		t[15] = ::fast_io::intrinsics::addc(t[15], zero, carry, carry);
		}
		{
		unsigned_type const b6{x.index_unchecked(6)};
		unsigned_type h67 FAST_IO_INDETERMINATE;
		unsigned_type const p67{::fast_io::intrinsics::umul(x.index_unchecked(7), b6, h67)};
		unsigned_type m67{p67};
		unsigned_type m68{::fast_io::intrinsics::addc(h67, zero, false, carry)};
		t[13] = ::fast_io::intrinsics::addc(t[13], m67, false, carry);
		t[14] = ::fast_io::intrinsics::addc(t[14], m68, carry, carry);
		t[15] = ::fast_io::intrinsics::addc(t[15], zero, carry, carry);
		}
		{
		t[1] = ::fast_io::intrinsics::addc(t[1], t[1], false, carry);
		t[2] = ::fast_io::intrinsics::addc(t[2], t[2], carry, carry);
		t[3] = ::fast_io::intrinsics::addc(t[3], t[3], carry, carry);
		t[4] = ::fast_io::intrinsics::addc(t[4], t[4], carry, carry);
		t[5] = ::fast_io::intrinsics::addc(t[5], t[5], carry, carry);
		t[6] = ::fast_io::intrinsics::addc(t[6], t[6], carry, carry);
		t[7] = ::fast_io::intrinsics::addc(t[7], t[7], carry, carry);
		t[8] = ::fast_io::intrinsics::addc(t[8], t[8], carry, carry);
		t[9] = ::fast_io::intrinsics::addc(t[9], t[9], carry, carry);
		t[10] = ::fast_io::intrinsics::addc(t[10], t[10], carry, carry);
		t[11] = ::fast_io::intrinsics::addc(t[11], t[11], carry, carry);
		t[12] = ::fast_io::intrinsics::addc(t[12], t[12], carry, carry);
		t[13] = ::fast_io::intrinsics::addc(t[13], t[13], carry, carry);
		t[14] = ::fast_io::intrinsics::addc(t[14], t[14], carry, carry);
		t[15] = ::fast_io::intrinsics::addc(t[15], t[15], carry, carry);
		}
		{
		unsigned_type h00 FAST_IO_INDETERMINATE;
		t[0] = ::fast_io::intrinsics::umul(x.index_unchecked(0), x.index_unchecked(0), h00);
		t[1] = ::fast_io::intrinsics::addc(t[1], h00, false, carry);
		unsigned_type h11 FAST_IO_INDETERMINATE;
		t[2] = ::fast_io::intrinsics::addc(t[2], ::fast_io::intrinsics::umul(x.index_unchecked(1), x.index_unchecked(1), h11), carry, carry);
		t[3] = ::fast_io::intrinsics::addc(t[3], h11, carry, carry);
		unsigned_type h22 FAST_IO_INDETERMINATE;
		t[4] = ::fast_io::intrinsics::addc(t[4], ::fast_io::intrinsics::umul(x.index_unchecked(2), x.index_unchecked(2), h22), carry, carry);
		t[5] = ::fast_io::intrinsics::addc(t[5], h22, carry, carry);
		unsigned_type h33 FAST_IO_INDETERMINATE;
		t[6] = ::fast_io::intrinsics::addc(t[6], ::fast_io::intrinsics::umul(x.index_unchecked(3), x.index_unchecked(3), h33), carry, carry);
		t[7] = ::fast_io::intrinsics::addc(t[7], h33, carry, carry);
		unsigned_type h44 FAST_IO_INDETERMINATE;
		t[8] = ::fast_io::intrinsics::addc(t[8], ::fast_io::intrinsics::umul(x.index_unchecked(4), x.index_unchecked(4), h44), carry, carry);
		t[9] = ::fast_io::intrinsics::addc(t[9], h44, carry, carry);
		unsigned_type h55 FAST_IO_INDETERMINATE;
		t[10] = ::fast_io::intrinsics::addc(t[10], ::fast_io::intrinsics::umul(x.index_unchecked(5), x.index_unchecked(5), h55), carry, carry);
		t[11] = ::fast_io::intrinsics::addc(t[11], h55, carry, carry);
		unsigned_type h66 FAST_IO_INDETERMINATE;
		t[12] = ::fast_io::intrinsics::addc(t[12], ::fast_io::intrinsics::umul(x.index_unchecked(6), x.index_unchecked(6), h66), carry, carry);
		t[13] = ::fast_io::intrinsics::addc(t[13], h66, carry, carry);
		unsigned_type h77 FAST_IO_INDETERMINATE;
		t[14] = ::fast_io::intrinsics::addc(t[14], ::fast_io::intrinsics::umul(x.index_unchecked(7), x.index_unchecked(7), h77), carry, carry);
		t[15] = ::fast_io::intrinsics::addc(t[15], h77, carry, carry);
		}
		reduce_final(r, t);
		}
	}
	else
	{
	using unsigned_type = ::fast_io::curve25519::field_number::value_type;
	constexpr unsigned_type zero{};
	constexpr unsigned_type constant{38};

	unsigned_type x0{x.front_unchecked()}, x1{x.index_unchecked(1)}, x2{x.index_unchecked(2)}, x3{x.back_unchecked()};

	unsigned_type a2;
	unsigned_type a1{::fast_io::intrinsics::umul(x0, x1, a2)};

	unsigned_type b0;
	unsigned_type a3{::fast_io::intrinsics::umul(x0, x3, b0)};

	unsigned_type b2;
	unsigned_type b1{::fast_io::intrinsics::umul(x2, x3, b2)};

	unsigned_type b3_x0x2;
	unsigned_type a0{::fast_io::intrinsics::umul(x0, x2, b3_x0x2)};

	unsigned_type c1;
	unsigned_type c0{::fast_io::intrinsics::umul(x1, x3, c1)};

	unsigned_type c0_x1x2;
	unsigned_type b3{::fast_io::intrinsics::umul(x1, x2, c0_x1x2)};

	bool carry;
	b3 = ::fast_io::intrinsics::addc(b3_x0x2, b3, false, carry);
	c0 = ::fast_io::intrinsics::addc(c0, c0_x1x2, carry, carry);
	c1 = ::fast_io::intrinsics::addc(zero, c1, carry, carry);

	a2 = ::fast_io::intrinsics::addc(a0, a2, false, carry);
	a3 = ::fast_io::intrinsics::addc(b3, a3, carry, carry);
	b0 = ::fast_io::intrinsics::addc(c0, b0, carry, carry);
	b1 = ::fast_io::intrinsics::addc(c1, b1, carry, carry);
	b2 = ::fast_io::intrinsics::addc(zero, b2, carry, carry);

	// Multiply by 2 (the rcl-through-carry idiom)
	a1 = ::fast_io::intrinsics::addc(a1, a1, false, carry);
	a2 = ::fast_io::intrinsics::addc(a2, a2, carry, carry);
	a3 = ::fast_io::intrinsics::addc(a3, a3, carry, carry);
	b0 = ::fast_io::intrinsics::addc(b0, b0, carry, carry);
	b1 = ::fast_io::intrinsics::addc(b1, b1, carry, carry);
	b2 = ::fast_io::intrinsics::addc(b2, b2, carry, carry);
	b3 = ::fast_io::intrinsics::addc(zero, zero, carry, carry);

	unsigned_type y1;
	x0 = ::fast_io::intrinsics::umul(x0, x0, y1);

	unsigned_type y3;
	x1 = ::fast_io::intrinsics::umul(x1, x1, y3);

	unsigned_type y5;
	x2 = ::fast_io::intrinsics::umul(x2, x2, y5);

	unsigned_type y7;
	x3 = ::fast_io::intrinsics::umul(x3, x3, y7);

	y1 = ::fast_io::intrinsics::addc(a1, y1, false, carry);
	x1 = ::fast_io::intrinsics::addc(a2, x1, carry, carry);
	y3 = ::fast_io::intrinsics::addc(a3, y3, carry, carry);
	x2 = ::fast_io::intrinsics::addc(b0, x2, carry, carry);
	y5 = ::fast_io::intrinsics::addc(b1, y5, carry, carry);
	x3 = ::fast_io::intrinsics::addc(b2, x3, carry, carry);
	y7 = ::fast_io::intrinsics::addc(b3, y7, carry, carry);

	unsigned_type high;
	a0 = muladd_w0(x0, x2, high);
	a1 = muladd_w1(y1, y5, high, high);
	a2 = muladd_w1(x1, x3, high, high);
	a3 = muladd_w1(y3, y7, high, high);

	unsigned_type low{::fast_io::intrinsics::umul(constant, high, high)};

	a0 = ::fast_io::intrinsics::addc(low, a0, false, carry);
	a1 = ::fast_io::intrinsics::addc(high, a1, carry, carry);
	a2 = ::fast_io::intrinsics::addc(zero, a2, carry, carry);
	a3 = ::fast_io::intrinsics::addc(zero, a3, carry, carry);
	low = ::fast_io::intrinsics::subc(low, low, carry, carry);

	low &= constant;
	r.front_unchecked() = ::fast_io::intrinsics::addc(low, a0, false, carry);
	r.index_unchecked(1) = ::fast_io::intrinsics::addc(zero, a1, carry, carry);
	r.index_unchecked(2) = ::fast_io::intrinsics::addc(zero, a2, carry, carry);
	r.back_unchecked() = ::fast_io::intrinsics::addc(zero, a3, carry, carry);
	}
}

/*
Reference from mehdi
The curve used is y2 = x^3 + 486662x^2 + x, a Montgomery curve, over
the prime field defined by the prime number 2^255 - 19, and it uses the
base point x = 9.

Protocol uses compressed elliptic point (only X coordinates), so it
allows for efficient use of the Montgomery ladder for ECDH, using only
XZ coordinates.

The curve is birationally equivalent to Ed25519 (Twisted Edwards curve).

b = 256
p = 2**255 - 19
l = 2**252 + 27742317777372353535851937790883648493
*/

/*
Using Euler theorem to calculate the z^(-2) of an element over a finite field
2^255 - 21
*/

inline constexpr void field_number_pow_minus2(::fast_io::curve25519::field_number &r, ::fast_io::curve25519::field_number const &z) noexcept
{
	::fast_io::curve25519::field_number t0 FAST_IO_INDETERMINATE, t1 FAST_IO_INDETERMINATE, z2 FAST_IO_INDETERMINATE, z9 FAST_IO_INDETERMINATE, z11 FAST_IO_INDETERMINATE;
	::fast_io::curve25519::field_number z2_5_0 FAST_IO_INDETERMINATE, z2_10_0 FAST_IO_INDETERMINATE, z2_20_0 FAST_IO_INDETERMINATE, z2_50_0 FAST_IO_INDETERMINATE, z2_100_0 FAST_IO_INDETERMINATE;

	/*
	Montgomery ladder
	*/

	/* 2 */ field_number_square(z2, z);
	/* 4 */ field_number_square(t1, z2);
	/* 8 */ field_number_square(t0, t1);
	/* 9 */ field_number_multiplication(z9, t0, z);
	/* 11 */ field_number_multiplication(z11, z9, z2);
	/* 22 */ field_number_square(t0, z11);
	/* 2^5 - 2^0 = 31 */ field_number_multiplication(z2_5_0, t0, z9);

	/* 2^6 - 2^1 */ field_number_square(t0, z2_5_0);
	/* 2^7 - 2^2 */ field_number_square(t1, t0);
	/* 2^8 - 2^3 */ field_number_square(t0, t1);
	/* 2^9 - 2^4 */ field_number_square(t1, t0);
	/* 2^10 - 2^5 */ field_number_square(t0, t1);
	/* 2^10 - 2^0 */ field_number_multiplication(z2_10_0, t0, z2_5_0);

	/* 2^11 - 2^1 */ field_number_square(t0, z2_10_0);
	/* 2^12 - 2^2 */ field_number_square(t1, t0);
	/* 2^20 - 2^10 */ for (std::uint_fast8_t i{}; i != 4; ++i)
	{
		field_number_square(t0, t1);
		field_number_square(t1, t0);
	}
	/* 2^20 - 2^0 */ field_number_multiplication(z2_20_0, t1, z2_10_0);

	/* 2^21 - 2^1 */ field_number_square(t0, z2_20_0);
	/* 2^22 - 2^2 */ field_number_square(t1, t0);

	/* 2^40 - 2^20 */ for (std::uint_fast8_t i{}; i != 9; ++i)
	{
		field_number_square(t0, t1);
		field_number_square(t1, t0);
	}
	/* 2^40 - 2^0 */ field_number_multiplication(t0, t1, z2_20_0);

	/* 2^41 - 2^1 */ field_number_square(t1, t0);
	/* 2^42 - 2^2 */ field_number_square(t0, t1);
	/* 2^50 - 2^10 */ for (std::uint_fast8_t i{}; i != 4; ++i)
	{
		field_number_square(t1, t0);
		field_number_square(t0, t1);
	}
	/* 2^50 - 2^0 */ field_number_multiplication(z2_50_0, t0, z2_10_0);

	/* 2^51 - 2^1 */ field_number_square(t0, z2_50_0);
	/* 2^52 - 2^2 */ field_number_square(t1, t0);
	/* 2^100 - 2^50 */ for (std::uint_fast8_t i{}; i != 24; ++i)
	{
		field_number_square(t0, t1);
		field_number_square(t1, t0);
	}
	/* 2^100 - 2^0 */ field_number_multiplication(z2_100_0, t1, z2_50_0);

	/* 2^101 - 2^1 */ field_number_square(t1, z2_100_0);
	/* 2^102 - 2^2 */ field_number_square(t0, t1);
	/* 2^200 - 2^100 */ for (std::uint_fast8_t i{}; i != 49; ++i)
	{
		field_number_square(t1, t0);
		field_number_square(t0, t1);
	}
	/* 2^200 - 2^0 */ field_number_multiplication(t1, t0, z2_100_0);

	/* 2^201 - 2^1 */ field_number_square(t0, t1);
	/* 2^202 - 2^2 */ field_number_square(t1, t0);
	/* 2^250 - 2^50 */ for (std::uint_fast8_t i{}; i != 24; ++i)
	{
		field_number_square(t0, t1);
		field_number_square(t1, t0);
	}
	/* 2^250 - 2^0 */ field_number_multiplication(t0, t1, z2_50_0);

	/* 2^251 - 2^1 */ field_number_square(t1, t0);
	/* 2^252 - 2^2 */ field_number_square(t0, t1);
	/* 2^253 - 2^3 */ field_number_square(t1, t0);
	/* 2^254 - 2^4 */ field_number_square(t0, t1);
	/* 2^255 - 2^5 */ field_number_square(t1, t0);
	/* 2^255 - 21 */ field_number_multiplication(r, t1, z11);
}

/*
Using Euler theorem to calculate the inverse of an element over a finite field.
2^255 - 20
*/


inline constexpr ::fast_io::curve25519::field_number pow_minus2(::fast_io::curve25519::field_number const &z) noexcept
{
	::fast_io::curve25519::field_number number FAST_IO_INDETERMINATE;
	field_number_pow_minus2(number, z);
	return number;
}

inline constexpr void field_number_reduce_to_25519(::fast_io::curve25519::field_number &x) noexcept
{
	using unsigned_type = ::fast_io::curve25519::field_number::value_type;
	if constexpr (::std::same_as<unsigned_type, ::std::uint_least32_t>)
	{
		constexpr unsigned_type c1{~unsigned_type{}};
		constexpr unsigned_type c2_constant{c1 - 18};
		constexpr unsigned_type c1r1_constant{c1 >> 1};
		unsigned_type c2{c2_constant};
		unsigned_type c1r1{c1r1_constant};
		unsigned_type t{};
		unsigned_type r0 FAST_IO_INDETERMINATE, r1 FAST_IO_INDETERMINATE, r2 FAST_IO_INDETERMINATE, r3 FAST_IO_INDETERMINATE;
		unsigned_type r4 FAST_IO_INDETERMINATE, r5 FAST_IO_INDETERMINATE, r6 FAST_IO_INDETERMINATE, r7 FAST_IO_INDETERMINATE;
		bool carry FAST_IO_INDETERMINATE;
		r0 = ::fast_io::intrinsics::subc(x.front_unchecked(), c2, false, carry);
		r1 = ::fast_io::intrinsics::subc(x.index_unchecked(1), c1, carry, carry);
		r2 = ::fast_io::intrinsics::subc(x.index_unchecked(2), c1, carry, carry);
		r3 = ::fast_io::intrinsics::subc(x.index_unchecked(3), c1, carry, carry);
		r4 = ::fast_io::intrinsics::subc(x.index_unchecked(4), c1, carry, carry);
		r5 = ::fast_io::intrinsics::subc(x.index_unchecked(5), c1, carry, carry);
		r6 = ::fast_io::intrinsics::subc(x.index_unchecked(6), c1, carry, carry);
		r7 = ::fast_io::intrinsics::subc(x.back_unchecked(), c1r1, carry, carry);
		t = ::fast_io::intrinsics::subc(t, t, carry, carry);
		// undo if carry is true
		c1r1 &= t;
		c2 &= t;

		r0 = ::fast_io::intrinsics::addc(r0, c2, false, carry);
		r1 = ::fast_io::intrinsics::addc(r1, t, carry, carry);
		r2 = ::fast_io::intrinsics::addc(r2, t, carry, carry);
		r3 = ::fast_io::intrinsics::addc(r3, t, carry, carry);
		r4 = ::fast_io::intrinsics::addc(r4, t, carry, carry);
		r5 = ::fast_io::intrinsics::addc(r5, t, carry, carry);
		r6 = ::fast_io::intrinsics::addc(r6, t, carry, carry);
		r7 = ::fast_io::intrinsics::addc(r7, c1r1, carry, carry);

		c1r1 = c1r1_constant;
		c2 = c2_constant;

		// we need to do this 2nd time.
		r0 = ::fast_io::intrinsics::subc(r0, c2, false, carry);
		r1 = ::fast_io::intrinsics::subc(r1, c1, carry, carry);
		r2 = ::fast_io::intrinsics::subc(r2, c1, carry, carry);
		r3 = ::fast_io::intrinsics::subc(r3, c1, carry, carry);
		r4 = ::fast_io::intrinsics::subc(r4, c1, carry, carry);
		r5 = ::fast_io::intrinsics::subc(r5, c1, carry, carry);
		r6 = ::fast_io::intrinsics::subc(r6, c1, carry, carry);
		r7 = ::fast_io::intrinsics::subc(r7, c1r1, carry, carry);
		// undo if carry is true
		t = ::fast_io::intrinsics::subc(t, t, carry, carry);
		c1r1 &= t;
		c2 &= t;
		x.front_unchecked() = ::fast_io::intrinsics::addc(r0, c2, false, carry);
		x.index_unchecked(1) = ::fast_io::intrinsics::addc(r1, t, carry, carry);
		x.index_unchecked(2) = ::fast_io::intrinsics::addc(r2, t, carry, carry);
		x.index_unchecked(3) = ::fast_io::intrinsics::addc(r3, t, carry, carry);
		x.index_unchecked(4) = ::fast_io::intrinsics::addc(r4, t, carry, carry);
		x.index_unchecked(5) = ::fast_io::intrinsics::addc(r5, t, carry, carry);
		x.index_unchecked(6) = ::fast_io::intrinsics::addc(r6, t, carry, carry);
		x.back_unchecked() = ::fast_io::intrinsics::addc(r7, c1r1, carry, carry);
		return;
	}
	else
	{
	constexpr unsigned_type c1{~unsigned_type{}};
	constexpr unsigned_type c2_constant{c1 - 18};
	constexpr unsigned_type c1r1_constant{c1 >> 1};
	unsigned_type c2{c2_constant};
	unsigned_type c1r1{c1r1_constant};
	unsigned_type t{};
	unsigned_type r0, r1, r2, r3;
	bool carry FAST_IO_INDETERMINATE;
	r0 = ::fast_io::intrinsics::subc(x.front_unchecked(), c2, false, carry);
	r1 = ::fast_io::intrinsics::subc(x.index_unchecked(1), c1, carry, carry);
	r2 = ::fast_io::intrinsics::subc(x.index_unchecked(2), c1, carry, carry);
	r3 = ::fast_io::intrinsics::subc(x.back_unchecked(), c1r1, carry, carry);
	t = ::fast_io::intrinsics::subc(t, t, carry, carry);
	// undo if carry is true
	c1r1 &= t;
	c2 &= t;

	r0 = ::fast_io::intrinsics::addc(r0, c2, false, carry);
	r1 = ::fast_io::intrinsics::addc(r1, t, carry, carry);
	r2 = ::fast_io::intrinsics::addc(r2, t, carry, carry);
	r3 = ::fast_io::intrinsics::addc(r3, c1r1, carry, carry);

	c1r1 = c1r1_constant;
	c2 = c2_constant;

	// we need to do this 2nd time.
	r0 = ::fast_io::intrinsics::subc(r0, c2, false, carry);
	r1 = ::fast_io::intrinsics::subc(r1, c1, carry, carry);
	r2 = ::fast_io::intrinsics::subc(r2, c1, carry, carry);
	r3 = ::fast_io::intrinsics::subc(r3, c1r1, carry, carry);
	// undo if carry is true
	t = ::fast_io::intrinsics::subc(t, t, carry, carry);
	c1r1 &= t;
	c2 &= t;
	x.front_unchecked() = ::fast_io::intrinsics::addc(r0, c2, false, carry);
	x.index_unchecked(1) = ::fast_io::intrinsics::addc(r1, t, carry, carry);
	x.index_unchecked(2) = ::fast_io::intrinsics::addc(r2, t, carry, carry);
	x.back_unchecked() = ::fast_io::intrinsics::addc(r3, c1r1, carry, carry);
	}
}

inline constexpr void field_number_inverse(::fast_io::curve25519::field_number &r, ::fast_io::curve25519::field_number const &z) noexcept
{
	field_number_pow_minus2(r, z);
	field_number_reduce_to_25519(r);
}

inline constexpr void field_multiplication_mod(::fast_io::curve25519::field_number &z, ::fast_io::curve25519::field_number const &x, ::fast_io::curve25519::field_number const &y) noexcept
{
	field_number_multiplication(z, x, y);
	field_number_reduce_to_25519(z);
}

inline constexpr ::fast_io::curve25519::field_number inverse(::fast_io::curve25519::field_number const &z) noexcept
{
	::fast_io::curve25519::field_number number FAST_IO_INDETERMINATE;
	field_number_inverse(number, z);
	return number;
}

inline constexpr ::fast_io::curve25519::field_number operator*(::fast_io::curve25519::field_number const &x, ::fast_io::curve25519::field_number const &y) noexcept
{
	::fast_io::curve25519::field_number f FAST_IO_INDETERMINATE;
	field_multiplication_mod(f, x, y);
	return f;
}

struct xz_point
{
	::fast_io::curve25519::field_number x, z;
};

inline constexpr ::fast_io::containers::array<std::byte, 32> montgomery_curve_base_point_x{std::byte{9}};

/* Y = X + X */
inline constexpr void montgomery_curve_point_double(xz_point &y, xz_point const &x) noexcept
{
	::fast_io::curve25519::field_number a FAST_IO_INDETERMINATE, b FAST_IO_INDETERMINATE;
	/*  x2 = (x+z)^2 * (x-z)^2 */
	/*  z2 = ((x+z)^2 - (x-z)^2)*((x+z)^2 + ((A-2)/4)((x+z)^2 - (x-z)^2)) */
	field_number_addition(a, x.x, x.z);
	field_number_subtraction(b, x.x, x.z);
	field_number_square(a, a);
	field_number_square(b, b);
	field_number_multiplication(y.x, a, b);
	field_number_subtraction(b, a, b);

	constexpr ::fast_io::curve25519::field_number::value_type constant{121665};
	multiplication_add_reduce(a, a, constant, b);
	field_number_multiplication(y.z, a, b);
}

inline constexpr void montgomery_curve_mont(xz_point &p, xz_point &q, ::fast_io::curve25519::field_number const &base) noexcept
{
	::fast_io::curve25519::field_number a FAST_IO_INDETERMINATE, b FAST_IO_INDETERMINATE, c FAST_IO_INDETERMINATE, d FAST_IO_INDETERMINATE, e FAST_IO_INDETERMINATE;
	field_number_subtraction(a, p.x, p.z);
	field_number_addition(b, p.x, p.z);
	field_number_subtraction(c, q.x, q.z);
	field_number_addition(d, q.x, q.z);
	field_number_multiplication(a, a, d);
	field_number_multiplication(b, b, c);
	field_number_addition(e, a, b);
	field_number_subtraction(b, a, b);
	field_number_square(p.x, e);
	field_number_square(a, b);
	field_number_multiplication(p.z, a, base);

	field_number_square(a, d);
	field_number_square(b, c);

	field_number_multiplication(q.x, a, b);
	field_number_subtraction(b, a, b);

	constexpr ::fast_io::curve25519::field_number::value_type constant{121665};
	multiplication_add_reduce(a, a, constant, b);
	field_number_multiplication(q.z, a, b);
}

inline constexpr void montgomery_curve_point_multiplication(std::byte *public_key, std::byte const *base_point, std::byte const *secret_key, ::fast_io::curve25519::field_number const &zr) noexcept
{
	constexpr std::size_t total_len{32};
	::fast_io::containers::array<std::byte, 32> sk FAST_IO_INDETERMINATE;
	::fast_io::freestanding::nonoverlapped_bytes_copy_n(secret_key, total_len, sk.data());
	::fast_io::curve25519::field_number x FAST_IO_INDETERMINATE;
	::fast_io::freestanding::type_punning_from_bytes(base_point, x);
	xz_point p FAST_IO_INDETERMINATE, q FAST_IO_INDETERMINATE;
	::fast_io::containers::array<xz_point *, 2> pp FAST_IO_INDETERMINATE;
	::fast_io::containers::array<xz_point *, 2> qp FAST_IO_INDETERMINATE;
	for (std::size_t j{total_len}; j--;)
	{
		char unsigned k{static_cast<char unsigned>(sk.index_unchecked(j))};
		for (std::size_t i{}; i < 8; ++i)
		{
			if (k & 0x80)
			{
				addition_u256_discard(p.z, x, zr);
				field_number_multiplication(p.x, x, p.z);
				montgomery_curve_point_double(q, p);
				pp.front_unchecked() = __builtin_addressof(q);
				pp.back_unchecked() = __builtin_addressof(p);
				qp.front_unchecked() = __builtin_addressof(p);
				qp.back_unchecked() = __builtin_addressof(q);
				auto mont{[&](char unsigned n) noexcept -> void {
					int unsigned chosen{static_cast<int unsigned>((k >> n) & 1)};
					montgomery_curve_mont(*pp.index_unchecked(chosen), *qp.index_unchecked(chosen), x);
				}};
				for (; ++i < 8; mont(7))
				{
					k <<= 1;
				}
				for (; j--;)
				{
					k = static_cast<char unsigned>(sk.index_unchecked(j));
					for (char unsigned m{8}; m--; mont(m))
						;
				}
				field_number_inverse(q.z, p.z);
				field_multiplication_mod(x, p.x, q.z);

				::fast_io::freestanding::type_punning_to_bytes(x, public_key);
				return;
			}
			k <<= 1;
		}
	}
	::fast_io::none_secure_clear(public_key, total_len);
}

inline constexpr void montgomery_curve_point_multiplication(std::byte *public_key, std::byte const *base_point, std::byte const *secret_key) noexcept
{
	montgomery_curve_point_multiplication(public_key, base_point, secret_key, ::fast_io::curve25519::custom_blindings::zr);
}

namespace details
{

inline constexpr void x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32> x) noexcept
{
	x.front_unchecked() = std::byte{static_cast<char8_t>(0xf8 & static_cast<char8_t>(x.front_unchecked()))};
	x.back_unchecked() = std::byte{static_cast<char8_t>((static_cast<char8_t>(x.back_unchecked()) | 0x40) & 0x7f)};
}

} // namespace details

} // namespace fast_io::curve25519

namespace fast_io::diffie_hellman::details
{

inline constexpr std::size_t key_size{32};

inline constexpr ::fast_io::containers::array<std::byte, 32> base_point{std::byte{9}};

inline constexpr void calculate_public_key_to_ptr(std::byte *pk, std::byte *sk) noexcept
{
	::fast_io::curve25519::details::x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, sk});
	::fast_io::curve25519::montgomery_curve_point_multiplication(pk, base_point.data(), sk);
}

inline constexpr void calculate_public_key_to_ptr_with_zr(std::byte *pk, std::byte *sk, ::fast_io::curve25519::field_number const &zr) noexcept
{
	::fast_io::curve25519::details::x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, sk});
	::fast_io::curve25519::montgomery_curve_point_multiplication(pk, base_point.data(), sk, zr);
}

inline constexpr void create_shared_key_to_ptr(std::byte *shared, std::byte const *pk, std::byte *sk) noexcept
{
	::fast_io::curve25519::details::x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, sk});
	::fast_io::curve25519::montgomery_curve_point_multiplication(shared, pk, sk);
}

inline constexpr void create_shared_key_to_ptr_with_zr(std::byte *shared, std::byte const *pk, std::byte *sk, ::fast_io::curve25519::field_number const &zr) noexcept
{
	::fast_io::curve25519::details::x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, sk});
	::fast_io::curve25519::montgomery_curve_point_multiplication(shared, pk, sk, zr);
}

inline constexpr void calculate_public_key(::fast_io::containers::index_span<std::byte, 32> public_key, ::fast_io::containers::index_span<std::byte, 32> secret_key) noexcept
{
	::fast_io::diffie_hellman::details::calculate_public_key_to_ptr(public_key.data(), secret_key.data());
}

inline constexpr void calculate_public_key_with_zr(::fast_io::containers::index_span<std::byte, 32> public_key, ::fast_io::containers::index_span<std::byte, 32> secret_key, ::fast_io::curve25519::field_number const &zr) noexcept
{
	::fast_io::diffie_hellman::details::calculate_public_key_to_ptr_with_zr(public_key.data(), secret_key.data(), zr);
}

inline constexpr void create_shared_key(::fast_io::containers::index_span<std::byte, 32> shared_key, ::fast_io::containers::index_span<std::byte const, 32> public_key, ::fast_io::containers::index_span<std::byte, 32> secret_key) noexcept
{
	::fast_io::diffie_hellman::details::create_shared_key_to_ptr(shared_key.data(), public_key.data(), secret_key.data());
}

inline constexpr void create_shared_key_with_zr(::fast_io::containers::index_span<std::byte, 32> shared_key, ::fast_io::containers::index_span<std::byte const, 32> public_key, ::fast_io::containers::index_span<std::byte, 32> secret_key, ::fast_io::curve25519::field_number const &zr) noexcept
{
	::fast_io::diffie_hellman::details::create_shared_key_to_ptr_with_zr(shared_key.data(), public_key.data(), secret_key.data(), zr);
}

} // namespace fast_io::diffie_hellman::details
