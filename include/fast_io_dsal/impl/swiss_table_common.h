#pragma once

namespace fast_io::details
{

/*
 * Control-byte layout follows abseil's raw_hash_set:
 *
 *   ctrl[0 .. cap)                    real bytes (empty/deleted/h2 of full slot)
 *   ctrl[cap]                         sentinel
 *   ctrl[cap+1 .. cap+1+cloned)       clones of ctrl[0 .. cloned)
 *
 * where cloned = swiss_table_cloned_counts == group_counts - 1, so that a
 * group load starting at any probe offset stays in bounds. Capacities are
 * always 2^m - 1 (or 0) so that `cap` itself is the probe mask: probe offsets
 * and clone mirroring are computed with `& cap`.
 */

// Group width is the simd_vector size mask countr accelerates: 16 with
// sse2/wasm simd128, 32 with avx2. Values below 16 mean group probing is
// unsupported and the table falls back to scalar probing.
inline constexpr ::std::size_t swiss_table_group_counts{
	::fast_io::intrinsics::optimal_simd_vector_run_with_cpu_instruction_size_with_mask_countr >= 16u
		? ::fast_io::intrinsics::optimal_simd_vector_run_with_cpu_instruction_size_with_mask_countr
		: 0u};

// Clone bytes after the sentinel; only group probing needs them.
inline constexpr ::std::size_t swiss_table_cloned_counts{
	swiss_table_group_counts != 0 ? swiss_table_group_counts - 1u : 0u};

// Sentinel plus clones: control bytes allocated beyond the cap real ones.
inline constexpr ::std::size_t swiss_table_ctrl_tail_counts{swiss_table_cloned_counts + 1u};

// Probe index grows by a group each step; scalar probing advances by one.
inline constexpr ::std::size_t swiss_table_probe_stride{
	swiss_table_group_counts != 0 ? swiss_table_group_counts : 1u};

inline constexpr ::std::size_t swiss_table_min_capacity{
	swiss_table_group_counts != 0 ? swiss_table_cloned_counts : 3u};

enum class swiss_table_ctrl : ::std::uint_least8_t
{
	empty = 0b10000000,
	deleted = 0b11111110,
	sentinel = 0b11111111
};

inline constexpr ::std::size_t swiss_table_hash_h1(::std::uint_least64_t hash) noexcept
{
	return static_cast<::std::size_t>(hash >> 7u);
}

inline constexpr ::std::uint_least8_t swiss_table_hash_h2(::std::uint_least64_t hash) noexcept
{
	return static_cast<::std::uint_least8_t>(hash & 0x7Fu);
}

inline constexpr bool swiss_table_ctrl_is_full(::std::uint_least8_t ctrl) noexcept
{
	return static_cast<signed char>(ctrl) >= 0;
}

inline constexpr bool swiss_table_ctrl_is_empty(::std::uint_least8_t ctrl) noexcept
{
	return ctrl == static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty);
}

inline constexpr bool swiss_table_ctrl_is_deleted(::std::uint_least8_t ctrl) noexcept
{
	return ctrl == static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted);
}

inline constexpr bool swiss_table_ctrl_is_empty_or_deleted(::std::uint_least8_t ctrl) noexcept
{
	return static_cast<signed char>(ctrl) <
		   static_cast<signed char>(::fast_io::details::swiss_table_ctrl::sentinel);
}

// Width of the group vectors. When group probing is unsupported the group
// is never used, but a valid width keeps discarded if constexpr branches
// instantiable in non-template functions.
inline constexpr ::std::size_t swiss_table_group_vec_counts{
	swiss_table_group_counts != 0 ? swiss_table_group_counts : 16u};

using swiss_table_group_vec_type =
	::fast_io::intrinsics::simd_vector<::std::uint_least8_t, ::fast_io::details::swiss_table_group_vec_counts>;
using swiss_table_group_svec_type =
	::fast_io::intrinsics::simd_vector<signed char, ::fast_io::details::swiss_table_group_vec_counts>;

template <::std::integral T>
inline constexpr ::fast_io::intrinsics::simd_vector<T, ::fast_io::details::swiss_table_group_vec_counts>
swiss_table_group_splat(T v) noexcept
{
	::fast_io::intrinsics::simd_vector<T, ::fast_io::details::swiss_table_group_vec_counts> pat{};
	for (::std::size_t i{}; i != ::fast_io::details::swiss_table_group_vec_counts; ++i)
	{
		pat.value[i] = v;
	}
	return pat;
}

// Lane indices 0 .. group_counts-1, for masking off a lane prefix.
inline constexpr ::fast_io::details::swiss_table_group_vec_type swiss_table_group_ramp() noexcept
{
	::fast_io::details::swiss_table_group_vec_type ramp{};
	for (::std::size_t i{}; i != ::fast_io::details::swiss_table_group_vec_counts; ++i)
	{
		ramp.value[i] = static_cast<::std::uint_least8_t>(i);
	}
	return ramp;
}

struct swiss_table_group
{
	static constexpr ::std::size_t counts{::fast_io::details::swiss_table_group_vec_counts};
	::fast_io::details::swiss_table_group_vec_type ctrlv;

	inline explicit swiss_table_group(::std::uint_least8_t const *pos) noexcept
	{
		ctrlv.load(pos);
	}

	// 0xff lanes where the control byte equals h2
	inline constexpr ::fast_io::details::swiss_table_group_vec_type match(::std::uint_least8_t h2) const noexcept
	{
		return ctrlv == ::fast_io::details::swiss_table_group_splat(h2);
	}

	// 0xff lanes where the control byte is empty
	inline constexpr ::fast_io::details::swiss_table_group_vec_type mask_empty() const noexcept
	{
		return ctrlv == ::fast_io::details::swiss_table_group_splat(
							static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty));
	}

	// 0xff lanes where the control byte is empty or deleted
	inline constexpr ::fast_io::details::swiss_table_group_svec_type mask_empty_or_deleted() const noexcept
	{
		return static_cast<::fast_io::details::swiss_table_group_svec_type>(ctrlv) <
			   ::fast_io::details::swiss_table_group_splat<signed char>(static_cast<signed char>(-1));
	}

	// special (empty/deleted/sentinel) -> empty, full -> deleted.
	// Used by drop-deletes rehash before reinsertion.
	inline constexpr void convert_special_to_empty_and_full_to_deleted(::std::uint_least8_t *dst) const noexcept
	{
		// special lanes are negative as signed char; full lanes are not.
		// res = special ? empty(0x80) : deleted(0xfe) = 0x80 | (~special & 0x7e)
		auto const special{static_cast<::fast_io::details::swiss_table_group_svec_type>(ctrlv) <
						   ::fast_io::details::swiss_table_group_splat<signed char>(static_cast<signed char>(0))};
		auto const res{::fast_io::details::swiss_table_group_splat(
						   static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty)) |
					   (static_cast<::fast_io::details::swiss_table_group_vec_type>(~special) &
						::fast_io::details::swiss_table_group_splat(static_cast<::std::uint_least8_t>(0x7eu)))};
		res.store(dst);
	}
};

// Triangular quadratic probe sequence: offset advances by 0, 16, 48, 80, ...
// (index grows by the group width each step), all mod cap+1 via & cap.
struct swiss_table_probe_seq
{
	::std::size_t cap;
	::std::size_t offset;
	::std::size_t index{};

	inline constexpr ::std::size_t offset_at(::std::size_t i) const noexcept
	{
		return (offset + i) & cap;
	}

	inline constexpr void next() noexcept
	{
		index += ::fast_io::details::swiss_table_probe_stride;
		offset = (offset + index) & cap;
	}
};

// Writes a control byte and maintains the clone region.
inline constexpr void swiss_table_set_ctrl(::std::uint_least8_t *controls, ::std::size_t cap,
										   ::std::size_t i, ::std::uint_least8_t v) noexcept
{
	controls[i] = v;
	constexpr ::std::size_t cloned{::fast_io::details::swiss_table_cloned_counts};
	controls[((i - cloned) & cap) + cloned] = v;
}

struct swiss_table_imp_common
{
	::std::uint_least8_t *controls;
	::std::size_t cap;
	::std::size_t counts;
	::std::size_t leftmost;
	::std::size_t growth_left;
};

template <::std::integral chtype>
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	swiss_table_str_type_erased_imp_common
{
	::std::uint_least8_t *controls;
	::std::size_t cap;
	::std::size_t counts;
	::std::size_t leftmost;
	::std::size_t growth_left;
	void *slots;
};

template <::std::integral chtype>
struct swiss_table_str_imp_common
{
	::std::uint_least8_t *controls;
	::std::size_t cap;
	::std::size_t counts;
	::std::size_t leftmost;
	::std::size_t growth_left;
	::fast_io::details::associative_string<chtype> *slots;
};

struct swiss_table_find_result
{
	::std::size_t pos;
	bool found;
};

template <typename imptype, typename keytype>
inline constexpr ::fast_io::details::swiss_table_find_result swiss_table_find_common(imptype const &imp,
																					 keytype key, ::std::uint_least64_t hash) noexcept
{
	::std::size_t const cap{imp.cap};
	if (!cap)
	{
		return {0u, false};
	}
	auto controls{imp.controls};
	auto slots{imp.slots};
	auto const h2{::fast_io::details::swiss_table_hash_h2(hash)};
	::fast_io::details::swiss_table_probe_seq seq{cap, ::fast_io::details::swiss_table_hash_h1(hash) & cap, {}};
	if constexpr (::fast_io::details::swiss_table_group_counts != 0)
	{
		constexpr ::std::size_t group_counts{::fast_io::details::swiss_table_group_counts};
		for (;;)
		{
#if FAST_IO_HAS_BUILTIN(__builtin_prefetch)
			__builtin_prefetch(slots + seq.offset);
#endif
			::fast_io::details::swiss_table_group const group{controls + seq.offset};
			for (auto match{group.match(h2)};;)
			{
				auto const i{::fast_io::intrinsics::vector_mask_countr_zero(match)};
				if (i == group_counts)
				{
					break;
				}
				auto const pos{seq.offset_at(static_cast<::std::size_t>(i))};
				if (key == slots[pos].key())
				{
					return {pos, true};
				}
				match.value[i] = 0;
			}
			if (auto const emptyidx{::fast_io::intrinsics::vector_mask_countr_zero(group.mask_empty())};
				emptyidx != group_counts)
			{
				return {seq.offset_at(static_cast<::std::size_t>(emptyidx)), false};
			}
			seq.next();
		}
	}
	else
	{
		for (;;)
		{
			auto const offset{seq.offset};
			auto const c{controls[offset]};
			if (::fast_io::details::swiss_table_ctrl_is_empty(c))
			{
				return {offset, false};
			}
			if (c == h2 && key == slots[offset].key())
			{
				return {offset, true};
			}
			seq.next();
		}
	}
}

template <::std::integral chtype, typename T>
inline constexpr ::fast_io::details::swiss_table_find_result swiss_table_find_common_with_str(
	T const &imp,
	chtype const *keybase, ::std::size_t keylen, ::std::uint_least64_t hash) noexcept
{
	::fast_io::basic_string_view<chtype> key(keybase, keylen);
	return ::fast_io::details::swiss_table_find_common(imp, key, hash);
}

template <::std::integral chtype, typename hasher, typename T>
inline constexpr ::fast_io::details::swiss_table_find_result swiss_table_find_common_with_str_hashfunc_with_hasher(
	T const &imp,
	chtype const *keybase, ::std::size_t keylen, hasher hash) noexcept
{
	return ::fast_io::details::swiss_table_find_common_with_str(imp, keybase, keylen,
																hash.do_hash(reinterpret_cast<::std::byte const *>(keybase), reinterpret_cast<::std::byte const *>(keybase + keylen)));
}

// First non-full (empty or deleted) position in the probe sequence of hash.
// Used for insertion and for rehashing.
inline constexpr ::std::size_t swiss_table_find_first_non_full(::std::uint_least8_t const *controls, ::std::size_t cap,
															   ::std::uint_least64_t hash) noexcept
{
	::fast_io::details::swiss_table_probe_seq seq{cap, ::fast_io::details::swiss_table_hash_h1(hash) & cap, {}};
	if constexpr (::fast_io::details::swiss_table_group_counts != 0)
	{
		constexpr ::std::size_t group_counts{::fast_io::details::swiss_table_group_counts};
		for (;;)
		{
			if (auto const i{::fast_io::intrinsics::vector_mask_countr_zero(
					::fast_io::details::swiss_table_group{controls + seq.offset}.mask_empty_or_deleted())};
				i != group_counts)
			{
				return seq.offset_at(static_cast<::std::size_t>(i));
			}
			seq.next();
		}
	}
	else
	{
		for (;;)
		{
			if (::fast_io::details::swiss_table_ctrl_is_empty_or_deleted(controls[seq.offset]))
			{
				return seq.offset;
			}
			seq.next();
		}
	}
}

// Whether the erased slot's probe cluster never overflowed a group boundary.
// If true the slot can be marked empty instead of deleted. References abseil's
// was_never_full.
inline constexpr bool swiss_table_was_never_full(::std::uint_least8_t const *controls, ::std::size_t cap,
												 ::std::size_t index) noexcept
{
	constexpr ::std::size_t group_counts{::fast_io::details::swiss_table_group_counts};
	if constexpr (group_counts == 0)
	{
		// Without group probing there is no group window to test; always mark
		// deleted. Tombstones are reclaimed by the in-place rehash.
		return false;
	}
	else
	{
		if (cap <= group_counts)
		{
			return true;
		}
		auto const empty_after{::fast_io::details::swiss_table_group{controls + index}.mask_empty()};
		auto const empty_after_index{::fast_io::intrinsics::vector_mask_countr_zero(empty_after)};
		if (empty_after_index == group_counts)
		{
			return false;
		}
		auto const index_before{static_cast<::std::size_t>((index - group_counts) & cap)};
		auto const empty_before{::fast_io::details::swiss_table_group{controls + index_before}.mask_empty()};
		// Abseil tests trailing_zeros(empty_after) + leading_zeros(empty_before) <
		// group width, i.e. the run of non-empty control bytes containing the erased
		// slot is shorter than a group. leading_zeros(empty_before) < width -
		// empty_after_index is the same as asking whether empty_before has a set
		// lane at index >= empty_after_index, so mask off the prefix lanes instead
		// of counting from the end.
		auto const suffix{empty_before &
						  (::fast_io::details::swiss_table_group_ramp() >=
						   ::fast_io::details::swiss_table_group_splat(
							   static_cast<::std::uint_least8_t>(empty_after_index)))};
		return ::fast_io::intrinsics::vector_mask_countr_zero(suffix) != group_counts;
	}
}

// Marks slot pos as erased: empty when the slot was never full (reclaimable),
// deleted otherwise. Maintains the clone region and growth_left.
template <typename imptype>
inline constexpr void swiss_table_erase_meta_only(imptype &imp, ::std::size_t pos) noexcept
{
	if (::fast_io::details::swiss_table_was_never_full(imp.controls, imp.cap, pos))
	{
		::fast_io::details::swiss_table_set_ctrl(imp.controls, imp.cap, pos,
												 static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty));
		++imp.growth_left;
	}
	else
	{
		::fast_io::details::swiss_table_set_ctrl(imp.controls, imp.cap, pos,
												 static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted));
	}
}

// Number of insertions a table of capacity cap can hold before needing to
// grow or rehash (abseil CapacityToGrowth): cap-1 for small tables so that a
// probe sequence always finds an empty slot, cap*7/8 for large ones.
inline constexpr ::std::size_t swiss_table_capacity_to_growth(::std::size_t cap) noexcept
{
	if (!cap)
	{
		return 0u;
	}
	if constexpr (::fast_io::details::swiss_table_group_counts == 0)
	{
		// Scalar probing needs at least one empty slot to terminate.
		return cap <= 63u ? cap - 1u : cap - (cap >> 3u);
	}
	else
	{
		constexpr ::std::size_t kmax_capacity_for_load_factor_one{::fast_io::details::swiss_table_group_counts * 4u - 1u};
		if (cap <= kmax_capacity_for_load_factor_one)
		{
			return cap - (cap >= swiss_table_group_counts - 1u);
		}
		return cap - (cap >> 3u);
	}
}

// Releases the fresh control array if the paired slot allocation fails.
template <typename allocator_type>
struct swiss_table_ctrl_alloc_guard
{
	::std::uint_least8_t *controls;
	::std::size_t n;
	inline constexpr ~swiss_table_ctrl_alloc_guard() noexcept
	{
		if (controls != nullptr)
		{
			::fast_io::typed_generic_allocator_adapter<allocator_type, ::std::uint_least8_t>::deallocate_n(controls, n);
		}
	}
};

// Smallest valid capacity whose growth bound can hold `size` elements.
// Ports abseil SizeToCapacity.
template <bool throwing = false>
inline constexpr ::std::size_t swiss_table_size_to_capacity(::std::size_t size) FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
{
	if (!size)
	{
		return 0u;
	}
	constexpr ::std::size_t digits{static_cast<::std::size_t>(::std::numeric_limits<::std::size_t>::digits)};
	// For scalar probing always leave at least one empty slot.
	constexpr ::std::size_t kmax_capacity_for_load_factor_one{
		::fast_io::details::swiss_table_group_counts != 0
			? ::fast_io::details::swiss_table_group_counts * 4u - 1u
			: 63u};
	if (size >= (::std::numeric_limits<::std::size_t>::max() >> 3u))
	{
		::fast_io::containers::details::contract_violation_report<throwing>(::std::errc::value_too_large);
	}
	auto leading_zeros{::std::countl_zero(size + (size >= ::fast_io::details::swiss_table_group_counts / 2u))};
	::std::size_t cap;
	if (size < kmax_capacity_for_load_factor_one)
	{
		cap = (~::std::size_t{}) >> leading_zeros;
	}
	else
	{
		constexpr ::std::size_t klast3bits{::std::size_t{7u} << (digits - 3u)};
		auto const max_size_for_next_capacity{static_cast<::std::size_t>(klast3bits >> leading_zeros)};
		leading_zeros -= static_cast<int>(size > max_size_for_next_capacity);
		cap = (~::std::size_t{}) >> leading_zeros;
	}
	return cap < ::fast_io::details::swiss_table_min_capacity ? ::fast_io::details::swiss_table_min_capacity : cap;
}

template <bool throwing = false>
inline constexpr ::std::size_t str_swiss_table_grow_compute_newcap(::std::size_t oldcap) FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
{
	if (oldcap == 0)
	{
		return ::fast_io::details::swiss_table_min_capacity;
	}
	constexpr ::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
	constexpr ::std::size_t mxdv2{(mx - 1u) >> 1u};
	if (mxdv2 < oldcap)
	{
		::fast_io::containers::details::contract_violation_report<throwing>(::std::errc::value_too_large);
	}
	return (oldcap << 1u) | 1u;
}

template <bool throwing = false>
inline constexpr ::std::size_t str_swiss_table_reserve_compute_newcap(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
{
	return ::fast_io::details::swiss_table_size_to_capacity<throwing>(n);
}

template <bool isprev>
inline constexpr ::std::uint_least8_t const *swiss_table_iterator_common(::std::uint_least8_t const *controlpos) noexcept
{
	if constexpr (isprev || ::fast_io::details::swiss_table_group_counts == 0)
	{
		do
		{
			if constexpr (isprev)
			{
				--controlpos;
			}
			else
			{
				++controlpos;
			}
		} while (::fast_io::details::swiss_table_ctrl_is_empty_or_deleted(*controlpos));
		return controlpos;
	}
	else
	{
		// Skip over runs of empty/deleted control bytes a group at a time.
		// Stops at full slots and at the sentinel, which is not
		// empty-or-deleted and therefore terminates the walk at end().
		++controlpos;
		for (;;)
		{
			auto const skipped{::fast_io::intrinsics::vector_mask_countr_one(
				::fast_io::details::swiss_table_group{controlpos}.mask_empty_or_deleted())};
			controlpos += skipped;
			if (skipped != ::fast_io::details::swiss_table_group_counts)
			{
				return controlpos;
			}
		}
	}
}

// special -> empty, full -> deleted over the whole control array, then
// restores the sentinel and clones. Used by drop-deletes rehash.
inline constexpr void
swiss_table_convert_special_to_empty_and_full_to_deleted(::std::uint_least8_t *controls, ::std::size_t cap) noexcept
{
	if constexpr (::fast_io::details::swiss_table_group_counts != 0)
	{
		// Strides cover cap+1 control bytes including the sentinel.
		for (::std::uint_least8_t *pos{controls}; pos != controls + cap + 1u;
			 pos += ::fast_io::details::swiss_table_group_counts)
		{
			::fast_io::details::swiss_table_group{pos}.convert_special_to_empty_and_full_to_deleted(pos);
		}
	}
	else
	{
		for (::std::uint_least8_t *pos{controls}; pos != controls + cap + 1u; ++pos)
		{
			auto &c{*pos};
			c = ::fast_io::details::swiss_table_ctrl_is_full(c)
					? static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted)
					: static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty);
		}
	}
	controls[cap] = static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::sentinel);
	__builtin_memcpy(controls + cap + 1u, controls, ::fast_io::details::swiss_table_cloned_counts);
}

} // namespace fast_io::details
