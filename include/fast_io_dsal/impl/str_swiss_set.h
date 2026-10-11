#pragma once

namespace fast_io::details
{

template <::std::integral chtype>
using str_swiss_set_imp_common = ::fast_io::details::swiss_table_str_imp_common<chtype>;

template <::std::integral chtype>
struct str_swiss_set_iterator
{
	using value_type = ::fast_io::containers::basic_cstring_view<chtype>;
	using iterator_tag = ::std::bidirectional_iterator_tag;
	using difference_type = ::std::ptrdiff_t;
	::std::uint_least8_t const *controlpos{};
	::fast_io::details::associative_string<chtype> const *slots{};
	constexpr value_type operator*() const noexcept
	{
		return slots->strvw();
	}
	constexpr str_swiss_set_iterator &operator++() noexcept
	{
		auto oldcontrolpos{this->controlpos};
		auto newcontrolpos{::fast_io::details::swiss_table_iterator_common<false>(oldcontrolpos)};
		this->controlpos = newcontrolpos;
		auto diff{newcontrolpos - oldcontrolpos};
#if __has_cpp_attribute(assume)
		[[assume(0 < diff)]];
#endif
		this->slots += static_cast<::std::size_t>(diff);
		return *this;
	}
	constexpr str_swiss_set_iterator operator++(int) noexcept
	{
		auto temp{*this};
		++*this;
		return temp;
	}
	constexpr str_swiss_set_iterator &operator--() noexcept
	{
		auto oldcontrolpos{this->controlpos};
		auto newcontrolpos{::fast_io::details::swiss_table_iterator_common<true>(oldcontrolpos)};
		this->controlpos = newcontrolpos;
		auto diff{oldcontrolpos - newcontrolpos};
#if __has_cpp_attribute(assume)
		[[assume(0 < diff)]];
#endif
		this->slots -= static_cast<::std::size_t>(diff);
		return *this;
	}
	constexpr str_swiss_set_iterator operator--(int) noexcept
	{
		auto temp{*this};
		--*this;
		return temp;
	}
};

template <::std::integral chtype>
inline constexpr bool operator==(::fast_io::details::str_swiss_set_iterator<chtype> a,
								 ::fast_io::details::str_swiss_set_iterator<chtype> b) noexcept
{
	return a.controlpos == b.controlpos;
}

template <typename allocator_type, typename hasher, ::std::integral chtype>
inline constexpr void str_swiss_set_reserve_to_newcap(
	::fast_io::details::str_swiss_set_imp_common<chtype> &imp, ::std::size_t newcap, hasher hash)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_on_allocation_failure<allocator_type>)
{
	using char_type = chtype;
	using slot_type = ::fast_io::details::associative_string<char_type>;
	using typed_slot_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, slot_type>;
	using typed_ctrl_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, ::std::uint_least8_t>;

	auto oldcontrols{imp.controls};
	auto oldslots{imp.slots};
	auto const oldcap{imp.cap};

	::std::size_t const newctrln{static_cast<::std::size_t>(newcap + ::fast_io::details::swiss_table_ctrl_tail_counts)};
	auto newcontrols{typed_ctrl_allocator_type::allocate(newctrln)};
	::fast_io::details::swiss_table_ctrl_alloc_guard<allocator_type> ctrlguard{newcontrols, newctrln};
	auto newslots{typed_slot_allocator_type::allocate(newcap)};
	ctrlguard.controls = nullptr;

	::fast_io::freestanding::my_memset(newcontrols,
									   static_cast<int>(::fast_io::details::swiss_table_ctrl::empty),
									   static_cast<::std::size_t>(newcap + ::fast_io::details::swiss_table_ctrl_tail_counts));
	newcontrols[newcap] = static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::sentinel);

	imp.controls = newcontrols;
	imp.cap = newcap;
	imp.slots = newslots;

	// Rehash existing entries
	if (oldcap == 0)
	{
		imp.leftmost = newcap;
	}
	else
	{
		::std::size_t leftmost{newcap};
		for (::std::size_t i{}; i != oldcap; ++i)
		{
			if (::fast_io::details::swiss_table_ctrl_is_full(oldcontrols[i]))
			{
				auto const &oldslot{oldslots[i]};
				auto const oldhash{hash.do_hash(reinterpret_cast<::std::byte const *>(oldslot.ptr), reinterpret_cast<::std::byte const *>(oldslot.ptr + oldslot.n))};
				auto pos{::fast_io::details::swiss_table_find_first_non_full(newcontrols, newcap, oldhash)};
				::fast_io::details::swiss_table_set_ctrl(newcontrols, newcap, pos,
														 ::fast_io::details::swiss_table_hash_h2(oldhash));
				newslots[pos] = oldslot;
				if (pos < leftmost)
				{
					leftmost = pos;
				}
			}
		}
		imp.leftmost = leftmost;
		typed_ctrl_allocator_type::deallocate_n(oldcontrols, static_cast<::std::size_t>(oldcap + ::fast_io::details::swiss_table_ctrl_tail_counts));
		typed_slot_allocator_type::deallocate_n(oldslots, oldcap);
	}
	imp.growth_left = ::fast_io::details::swiss_table_capacity_to_growth(newcap) - imp.counts;
}

template <typename allocator_type, typename hasher, ::std::integral chtype>
inline constexpr void str_swiss_set_reserve(
	::fast_io::details::str_swiss_set_imp_common<chtype> &imp, ::std::size_t n, hasher hash)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
{
	if (n <= imp.counts + imp.growth_left)
	{
		return;
	}
	::std::size_t newcap{::fast_io::details::str_swiss_table_reserve_compute_newcap<::fast_io::containers::details::allocator_throws_on_overflow<allocator_type>>(n)};
	if (newcap <= imp.cap)
	{
		return;
	}
	::fast_io::details::str_swiss_set_reserve_to_newcap<allocator_type, hasher, chtype>(imp, newcap, hash);
}

template <typename allocator_type, typename hasher, ::std::integral chtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void str_swiss_set_grow(
	::fast_io::details::str_swiss_set_imp_common<chtype> &imp, hasher hash)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
{
	::fast_io::details::str_swiss_set_reserve_to_newcap<allocator_type, hasher, chtype>(imp,
																						::fast_io::details::str_swiss_table_grow_compute_newcap<::fast_io::containers::details::allocator_throws_on_overflow<allocator_type>>(imp.cap), hash);
}

template <typename allocator_type, ::std::integral chtype>
inline constexpr void str_swiss_set_insert_key_internal(
	::fast_io::details::str_swiss_set_imp_common<chtype> &imp,
	::std::size_t pos, chtype const *keybase, ::std::size_t keylen,
	::std::uint_least64_t hash)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
{
	using char_type = chtype;
	auto const h2{::fast_io::details::swiss_table_hash_h2(hash)};
	// build the key before claiming the slot so a throwing allocation leaves
	// the table untouched
	auto newkey{::fast_io::details::create_associative_string<allocator_type, char_type>(keybase, keylen)};
	if (::fast_io::details::swiss_table_ctrl_is_empty(imp.controls[pos]))
	{
		--imp.growth_left;
	}
	::fast_io::details::swiss_table_set_ctrl(imp.controls, imp.cap, pos, h2);
	imp.slots[pos] = newkey;
	if (pos < imp.leftmost)
	{
		imp.leftmost = pos;
	}
	++imp.counts;
}

// Drops tombstones by rehashing the table in place without resizing.
// References abseil DropDeletesWithoutResize.
template <typename allocator_type, typename hasher, ::std::integral chtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void str_swiss_set_rehash_in_place(
	::fast_io::details::str_swiss_set_imp_common<chtype> &imp, hasher hash) noexcept
{
	auto const controls{imp.controls};
	auto const slots{imp.slots};
	auto const cap{imp.cap};
	// special -> empty, full -> deleted (live slots are now tombstones).
	::fast_io::details::swiss_table_convert_special_to_empty_and_full_to_deleted(controls, cap);

	// Probe distance in steps; scalar probing counts single slots so the
	// same-group test degenerates to target == i.
	auto const probe_index{[cap](::std::size_t pos, ::std::size_t offset) noexcept {
		return ((pos - offset) & cap) / ::fast_io::details::swiss_table_probe_stride;
	}};
	::std::size_t leftmost{cap};
	for (::std::size_t i{}; i != cap; ++i)
	{
		if (!::fast_io::details::swiss_table_ctrl_is_deleted(controls[i]))
		{
			continue;
		}
		for (;;)
		{
			auto const h{hash.do_hash(reinterpret_cast<::std::byte const *>(slots[i].ptr),
									  reinterpret_cast<::std::byte const *>(slots[i].ptr + slots[i].n))};
			auto const offset{::fast_io::details::swiss_table_hash_h1(h) & cap};
			auto const target{::fast_io::details::swiss_table_find_first_non_full(controls, cap, h)};
			if (probe_index(target, offset) == probe_index(i, offset))
			{
				// target is within the same group: the slot stays
				::fast_io::details::swiss_table_set_ctrl(controls, cap, i,
														 ::fast_io::details::swiss_table_hash_h2(h));
				if (i < leftmost)
				{
					leftmost = i;
				}
				break;
			}
			if (::fast_io::details::swiss_table_ctrl_is_empty(controls[target]))
			{
				::fast_io::details::swiss_table_set_ctrl(controls, cap, target,
														 ::fast_io::details::swiss_table_hash_h2(h));
				slots[target] = slots[i];
				::fast_io::details::swiss_table_set_ctrl(controls, cap, i,
														 static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty));
				if (target < leftmost)
				{
					leftmost = target;
				}
				break;
			}
			// swap element at i into its probe position and reprocess i
			::fast_io::details::swiss_table_set_ctrl(controls, cap, target,
													 ::fast_io::details::swiss_table_hash_h2(h));
			{
				auto tmp{slots[i]};
				slots[i] = slots[target];
				slots[target] = tmp;
			}
			if (target < leftmost)
			{
				leftmost = target;
			}
		}
	}
	imp.leftmost = leftmost;
	imp.growth_left = ::fast_io::details::swiss_table_capacity_to_growth(cap) - imp.counts;
}

template <bool needdestroy, typename allocator_type, ::std::integral chtype>
inline constexpr void str_swiss_set_clear_impl(
	::fast_io::details::str_swiss_set_imp_common<chtype> &imp) noexcept
{
	using char_type = chtype;
	using slot_type = ::fast_io::details::associative_string<char_type>;
	using typed_slot_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, slot_type>;
	using typed_ctrl_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, ::std::uint_least8_t>;

	auto const cap{imp.cap};
	if (!cap)
	{
		return;
	}
	auto controls{imp.controls};
	auto slots{imp.slots};
	for (::std::size_t i{}; i != cap; ++i)
	{
		if (::fast_io::details::swiss_table_ctrl_is_full(controls[i]))
		{
			auto si{slots[i]};
			::fast_io::details::deallocate_associative_string<allocator_type, char_type>(si.ptr, si.n);
		}
	}
	if constexpr (needdestroy)
	{
		typed_ctrl_allocator_type::deallocate_n(controls, static_cast<::std::size_t>(cap + ::fast_io::details::swiss_table_ctrl_tail_counts));
		typed_slot_allocator_type::deallocate_n(slots, cap);
		imp = {};
	}
	else
	{
		::fast_io::freestanding::my_memset(controls,
										   static_cast<int>(::fast_io::details::swiss_table_ctrl::empty),
										   static_cast<::std::size_t>(cap + ::fast_io::details::swiss_table_ctrl_tail_counts));
		controls[cap] = static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::sentinel);
		imp.counts = 0u;
		imp.leftmost = cap;
		imp.growth_left = ::fast_io::details::swiss_table_capacity_to_growth(cap);
	}
}

template <typename allocator_type, ::std::integral chtype>
inline constexpr ::std::size_t str_swiss_set_erase_rg(::fast_io::details::str_swiss_set_imp_common<chtype> &imp, ::std::size_t first, ::std::size_t last) noexcept
{
	if (first == last)
	{
		return first;
	}
	::std::size_t i{first};
	::std::size_t counting{};
	for (; i != last; ++counting)
	{
		auto si{imp.slots[i]};
		::fast_io::details::deallocate_associative_string<allocator_type, chtype>(si.ptr, si.n);
		::fast_io::details::swiss_table_erase_meta_only(imp, i);
		i = static_cast<::std::size_t>(::fast_io::details::swiss_table_iterator_common<false>(imp.controls + i) - imp.controls);
	}
	if (!(imp.counts -= counting) || imp.leftmost == first)
	{
		imp.leftmost = i;
	}
	return i;
}

template <bool compute_next, typename allocator_type, ::std::integral chtype>
inline constexpr ::std::conditional_t<compute_next, ::std::size_t, void> str_swiss_set_erase(::fast_io::details::str_swiss_set_imp_common<chtype> &imp, ::std::size_t pos) noexcept
{
	auto si{imp.slots[pos]};
	::fast_io::details::deallocate_associative_string<allocator_type, chtype>(si.ptr, si.n);
	::fast_io::details::swiss_table_erase_meta_only(imp, pos);
	auto controls{imp.controls};
	auto controlspos{controls + pos};
	if constexpr (compute_next)
	{
		::std::size_t next{imp.cap};
		if (--imp.counts)
		{
			next = static_cast<::std::size_t>(::fast_io::details::swiss_table_iterator_common<false>(controlspos) - controls);
			if (imp.leftmost == pos)
			{
				imp.leftmost = next;
			}
		}
		else
		{
			imp.leftmost = next;
		}
		return next;
	}
	else
	{
		if (--imp.counts)
		{
			if (imp.leftmost == pos)
			{
				imp.leftmost = static_cast<::std::size_t>(::fast_io::details::swiss_table_iterator_common<false>(controlspos) - controls);
			}
		}
		else
		{
			imp.leftmost = imp.cap;
		}
	}
}

template <typename allocator_type, typename hasher, ::std::integral chtype>
inline constexpr bool str_swiss_set_erase_key(::fast_io::details::str_swiss_set_imp_common<chtype> &imp, chtype const *str, ::std::size_t strn, hasher hash) noexcept
{
	auto [pos, found] = ::fast_io::details::swiss_table_find_common_with_str_hashfunc_with_hasher<chtype>(
		imp, str, strn, hash);
	if (!found)
	{
		return false;
	}
	::fast_io::details::str_swiss_set_erase<false, allocator_type, chtype>(imp, pos);
	return true;
}

// Destroys every key cloned so far plus the control/slot arrays when a key
// allocation throws partway through str_swiss_set_clone. Disarmed by nulling
// controls.
template <typename allocator_type, ::std::integral chtype>
struct str_swiss_set_clone_guard
{
	::std::uint_least8_t *controls;
	::fast_io::details::associative_string<chtype> *slots;
	::std::size_t cap;
	::std::size_t upto;

	inline constexpr ~str_swiss_set_clone_guard() noexcept
	{
		if (controls == nullptr)
		{
			return;
		}
		for (::std::size_t i{}; i != upto; ++i)
		{
			if (::fast_io::details::swiss_table_ctrl_is_full(controls[i]))
			{
				auto si{slots[i]};
				::fast_io::details::deallocate_associative_string<allocator_type, chtype>(si.ptr, si.n);
			}
		}
		using typed_slot_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, ::fast_io::details::associative_string<chtype>>;
		using typed_ctrl_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, ::std::uint_least8_t>;
		typed_ctrl_allocator_type::deallocate_n(controls, static_cast<::std::size_t>(cap + ::fast_io::details::swiss_table_ctrl_tail_counts));
		typed_slot_allocator_type::deallocate_n(slots, cap);
	}
};

template <typename allocator_type, ::std::integral chtype>
inline constexpr ::fast_io::details::str_swiss_set_imp_common<chtype> str_swiss_set_clone(::fast_io::details::str_swiss_set_imp_common<chtype> const &other)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
{
	using char_type = chtype;
	using slot_type = ::fast_io::details::associative_string<char_type>;
	using typed_slot_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, slot_type>;
	using typed_ctrl_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, ::std::uint_least8_t>;

	auto const cap{other.cap};
	if (!cap)
	{
		return {};
	}
	auto othercontrols{other.controls};
	auto otherslots{other.slots};
	::std::size_t const ctrlsz{static_cast<::std::size_t>(cap + ::fast_io::details::swiss_table_ctrl_tail_counts)};
	auto controls{typed_ctrl_allocator_type::allocate(ctrlsz)};
	::fast_io::details::swiss_table_ctrl_alloc_guard<allocator_type> ctrlguard{controls, ctrlsz};
	auto slots{typed_slot_allocator_type::allocate(cap)};
	ctrlguard.controls = nullptr;

	::fast_io::freestanding::non_overlapped_copy_n(othercontrols, ctrlsz, controls);
	::fast_io::details::str_swiss_set_clone_guard<allocator_type, char_type> guard{controls, slots, cap, {}};
	for (::std::size_t i{}; i != cap; ++i)
	{
		guard.upto = i;
		if (::fast_io::details::swiss_table_ctrl_is_full(othercontrols[i]))
		{
			auto si{otherslots[i]};
			slots[i] = ::fast_io::details::create_associative_string<allocator_type, char_type>(si.ptr, si.n);
		}
	}
	guard.upto = cap;
	guard.controls = nullptr;
	return {controls, cap, other.counts, other.leftmost, other.growth_left, slots};
}


template <::std::integral chtype>
struct str_swiss_set_insert_key_result
{
	::fast_io::details::str_swiss_set_iterator<chtype> position;
	bool inserted;
};

template <typename allocator_type, typename hasher, ::std::integral char_type>
constexpr ::fast_io::details::str_swiss_set_insert_key_result<char_type> str_swiss_set_insert_key_with_hash(::fast_io::details::str_swiss_set_imp_common<char_type> &imp, char_type const *key, ::std::size_t keyn, hasher hash)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
{
	auto hval{hash.do_hash(reinterpret_cast<::std::byte const *>(key), reinterpret_cast<::std::byte const *>(key + keyn))};
	auto const result{::fast_io::details::swiss_table_find_common_with_str<char_type>(
		imp, key, keyn, hval)};
	::std::size_t pos{result.pos};
	if (result.found)
	{
		return {{imp.controls + pos, imp.slots + pos}, false};
	}
	if (!imp.growth_left)
	{
		if (imp.counts < ::fast_io::details::swiss_table_capacity_to_growth(imp.cap))
		{
			// tombstones consume the growth budget: rehash in place
			::fast_io::details::str_swiss_set_rehash_in_place<allocator_type, hasher, char_type>(
				imp, hash);
		}
		else
		{
			::fast_io::details::str_swiss_set_grow<allocator_type, hasher, char_type>(
				imp, hash);
		}
		pos = ::fast_io::details::swiss_table_find_first_non_full(imp.controls, imp.cap, hval);
	}
	else if (::fast_io::details::swiss_table_capacity_to_growth(imp.cap) != imp.growth_left + imp.counts)
	{
		// tombstones exist: reuse the first non-full slot in probe order
		pos = ::fast_io::details::swiss_table_find_first_non_full(imp.controls, imp.cap, hval);
	}
	::fast_io::details::str_swiss_set_insert_key_internal<allocator_type, char_type>(
		imp, pos, key, keyn, hval);
	return {{imp.controls + pos, imp.slots + pos}, true};
}

} // namespace fast_io::details

namespace fast_io
{

namespace containers
{
template <::std::integral chtype, typename Hash, typename Allocator>
class basic_str_swiss_set
{
public:
	using char_type = chtype;
	using key_string_view_type = ::fast_io::containers::basic_string_view<char_type>;
	using key_cstring_view_type = ::fast_io::containers::basic_cstring_view<char_type>;
	using allocator_type = Allocator;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using const_iterator = ::fast_io::details::str_swiss_set_iterator<chtype>;
	using iterator = const_iterator;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
	using reverse_iterator = const_reverse_iterator;
	using insert_result_type = ::fast_io::details::str_swiss_set_insert_key_result<char_type>;
	using hasher = Hash;

	::fast_io::details::str_swiss_set_imp_common<char_type> imp{};
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	hasher hash{};

	constexpr basic_str_swiss_set() noexcept = default;

	explicit constexpr basic_str_swiss_set(::fast_io::freestanding::from_hasher_t, hasher h)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_copy_constructible_v<hasher>)
		: hash(h)
	{}

	constexpr basic_str_swiss_set(basic_str_swiss_set const &other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> ||
									   !::std::is_nothrow_copy_constructible_v<hasher>)
		: imp(::fast_io::details::str_swiss_set_clone<allocator_type, chtype>(other.imp)), hash(other.hash) {};
	constexpr basic_str_swiss_set &operator=(basic_str_swiss_set const &other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> ||
									   !::std::is_nothrow_copy_constructible_v<hasher>)
	{
		if (this != ::std::addressof(other))
		{
			auto tmp = ::fast_io::details::str_swiss_set_clone<allocator_type, chtype>(other.imp);
			this->clear_destroy();
			this->imp = tmp;
			this->hash = other.hash;
		}
		return *this;
	}

private:
	struct construct_range_destroyer
	{
		basic_str_swiss_set *ptr{};
		explicit constexpr construct_range_destroyer(basic_str_swiss_set *p) noexcept
			: ptr{p}
		{}

		construct_range_destroyer(construct_range_destroyer const &) = delete;
		construct_range_destroyer &operator=(construct_range_destroyer const &) = delete;

		constexpr ~construct_range_destroyer() noexcept
		{
			if (ptr)
			{
				ptr->clear_destroy();
			}
		}
	};
	template <::std::ranges::range R>
	constexpr void construct_with_range_common(R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> ||
									   !::std::is_nothrow_constructible_v<key_string_view_type, ::std::ranges::range_reference_t<R>>)
	{
		construct_range_destroyer des(this);
		if constexpr (::std::ranges::sized_range<R>)
		{
			this->reserve(::std::ranges::size(rg));
		}
		for (auto const &e : rg)
		{
			this->insert_key(e);
		}
		des.ptr = nullptr;
	}

public:
	explicit constexpr basic_str_swiss_set(::std::initializer_list<key_string_view_type> ilist)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		this->construct_with_range_common(ilist);
	}

	template <::std::ranges::range R>
	explicit constexpr basic_str_swiss_set(::fast_io::freestanding::from_range_t, R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> ||
									   !::std::is_nothrow_constructible_v<key_string_view_type, ::std::ranges::range_value_t<R>>)
	{
		this->construct_with_range_common(::std::forward<R>(rg));
	}

	explicit constexpr basic_str_swiss_set(::fast_io::from_hasher_t, hasher h, ::std::initializer_list<key_string_view_type> ilist)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> ||
									   !::std::is_nothrow_copy_constructible_v<hasher>)
		: hash(h)
	{
		this->construct_with_range_common(ilist);
	}

	template <::std::ranges::range R>
	explicit constexpr basic_str_swiss_set(::fast_io::from_hasher_t, hasher h, ::fast_io::freestanding::from_range_t, R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> ||
									   !::std::is_nothrow_copy_constructible_v<hasher> ||
									   !::std::is_nothrow_constructible_v<key_string_view_type, ::std::ranges::range_value_t<R>>)
		: hash(h)
	{
		this->construct_with_range_common(::std::forward<R>(rg));
	}

	constexpr basic_str_swiss_set(basic_str_swiss_set &&other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_move_constructible_v<hasher>)
		: imp{other.imp}, hash(::std::move(other.hash))
	{
		other.imp = {};
	}

	constexpr basic_str_swiss_set &operator=(basic_str_swiss_set &&other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_move_assignable_v<hasher>)
	{
		if (this != ::std::addressof(other))
		{
			::fast_io::details::str_swiss_set_clear_impl<true, allocator_type, char_type>(this->imp);
			this->imp = other.imp;
			other.imp = {};
			this->hash = ::std::move(other.hash);
		}
		return *this;
	}
	constexpr hasher hash_function() const
		FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_copy_constructible_v<hasher>)
	{
		return hash;
	}

	constexpr void clear() noexcept
	{
		::fast_io::details::str_swiss_set_clear_impl<false, allocator_type, char_type>(this->imp);
	}

	constexpr void clear_destroy() noexcept
	{
		::fast_io::details::str_swiss_set_clear_impl<true, allocator_type, char_type>(this->imp);
		this->imp = {};
	}

	constexpr ~basic_str_swiss_set() noexcept
	{
		::fast_io::details::str_swiss_set_clear_impl<true, allocator_type, char_type>(this->imp);
	}

	constexpr size_type size() const noexcept
	{
		return this->imp.counts;
	}

	constexpr size_type capacity() const noexcept
	{
		return this->imp.cap;
	}

	static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type val{static_cast<size_type>((::std::numeric_limits<size_type>::max() >> 1u)) / (sizeof(::fast_io::details::associative_string<chtype>) + 1u)};
		return val;
	}

	constexpr insert_result_type insert_key(key_string_view_type key)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		return ::fast_io::details::str_swiss_set_insert_key_with_hash<allocator_type, hasher, char_type>(
			this->imp, key.ptr, key.n, hash);
	}

	template <::std::ranges::range R>
	constexpr void insert_range(R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> ||
									   !::std::is_nothrow_constructible_v<key_string_view_type, ::std::ranges::range_value_t<R>>)
	{
		for (auto const &e : rg)
		{
			this->insert_key(e);
		}
	}

	constexpr bool contains(key_string_view_type key) const noexcept
	{
		return ::fast_io::details::swiss_table_find_common_with_str_hashfunc_with_hasher<char_type>(
				   this->imp, key.ptr, key.n, hash)
			.found;
	}
	constexpr iterator find_key(key_string_view_type key) const noexcept
	{
		auto [pos, found] = ::fast_io::details::swiss_table_find_common_with_str_hashfunc_with_hasher<char_type>(
			this->imp, key.ptr, key.n, hash);
		if (found)
		{
			return {this->imp.controls + pos, this->imp.slots + pos};
		}
		return cend();
	}
	constexpr size_type count(key_string_view_type key) const noexcept
	{
		return this->contains(key);
	}
	constexpr bool is_empty() const noexcept
	{
		return !this->imp.counts;
	}

	constexpr size_type erase_key(key_string_view_type key) noexcept
	{
		return ::fast_io::details::str_swiss_set_erase_key<allocator_type, hasher>(this->imp, key.data(), key.size(), hash);
	}

	constexpr iterator erase(const_iterator iter) noexcept
	{
		auto next{::fast_io::details::str_swiss_set_erase<true, allocator_type, char_type>(this->imp, static_cast<::std::size_t>(iter.controlpos - this->imp.controls))};
		return {this->imp.controls + next, this->imp.slots + next};
	}
	constexpr iterator erase(const_iterator first, const_iterator last) noexcept
	{
		auto controls{this->imp.controls};
		auto next{::fast_io::details::str_swiss_set_erase_rg<allocator_type, char_type>(this->imp, static_cast<::std::size_t>(first.controlpos - controls), static_cast<::std::size_t>(last.controlpos - controls))};
		return {this->imp.controls + next, this->imp.slots + next};
	}
	constexpr const_iterator cbegin() const noexcept
	{
		auto leftmost{this->imp.leftmost};
		return {this->imp.controls + leftmost, this->imp.slots + leftmost};
	}
	constexpr const_iterator cend() const noexcept
	{
		auto cap{this->imp.cap};
		return {this->imp.controls + cap, this->imp.slots + cap};
	}
	constexpr iterator begin() const noexcept
	{
		return this->cbegin();
	}
	constexpr iterator end() const noexcept
	{
		return this->cend();
	}
	constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(cend());
	}
	constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(cbegin());
	}
	constexpr reverse_iterator rbegin() const noexcept
	{
		return this->crbegin();
	}
	constexpr reverse_iterator rend() const noexcept
	{
		return this->crend();
	}

	constexpr void reserve(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		::fast_io::details::str_swiss_set_reserve<allocator_type, hasher, char_type>(this->imp, n, hash);
	}
	constexpr void swap(basic_str_swiss_set &other) FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_swappable_v<hasher>)
	{
		::std::ranges::swap(this->imp, other.imp);
		::std::ranges::swap(this->hash, other.hash);
	}
};

template <::std::integral chtype, typename Hash, typename Allocator>
constexpr void swap(basic_str_swiss_set<chtype, Hash, Allocator> &a, basic_str_swiss_set<chtype, Hash, Allocator> &b) noexcept
{
	a.swap(b);
}

template <::std::integral chtype, typename Hash, typename Allocator>
constexpr bool operator==(basic_str_swiss_set<chtype, Hash, Allocator> const &lhs, basic_str_swiss_set<chtype, Hash, Allocator> const &rhs) noexcept
{
	if (lhs.size() != rhs.size())
	{
		return false;
	}
	return ::std::is_permutation(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

} // namespace containers

} // namespace fast_io
