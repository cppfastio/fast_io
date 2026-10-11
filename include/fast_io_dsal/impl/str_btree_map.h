#pragma once

namespace fast_io::containers
{

template <::std::integral chtype, typename valtype>
struct basic_str_btree_map_key_mapped_pair
{
	using char_type = chtype;
	using key_type = ::fast_io::containers::basic_cstring_view<char_type>;
	using mapped_type = valtype;
	::fast_io::details::associative_string<chtype> ky;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	mapped_type val;

	constexpr basic_str_btree_map_key_mapped_pair() FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_default_constructible_v<mapped_type>) = default;
	constexpr basic_str_btree_map_key_mapped_pair(basic_str_btree_map_key_mapped_pair const &) FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_copy_constructible_v<mapped_type>) = default;
	constexpr basic_str_btree_map_key_mapped_pair(basic_str_btree_map_key_mapped_pair &&) FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_move_constructible_v<mapped_type>) = default;
	constexpr basic_str_btree_map_key_mapped_pair &operator=(basic_str_btree_map_key_mapped_pair const &) FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_copy_assignable_v<mapped_type>) = default;
	constexpr basic_str_btree_map_key_mapped_pair &operator=(basic_str_btree_map_key_mapped_pair &&) FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_move_assignable_v<mapped_type>) = default;

	constexpr key_type key() const noexcept
	{
		return ky.strvw();
	}
	constexpr mapped_type &mapped() noexcept
	{
		return val;
	}
	constexpr mapped_type const &mapped() const noexcept
	{
		return val;
	}
};

template <::std::integral chtype, typename mappedtype>
struct basic_str_btree_map_key_mapped_initializer_list_pair
{
	using char_type = chtype;
	using key_type = ::fast_io::containers::basic_string_view<char_type>;
	using mapped_type = mappedtype;

	::fast_io::containers::basic_string_view<chtype> ky;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	mappedtype val;
	constexpr key_type key() const noexcept
	{
		return ky;
	}
	constexpr mapped_type &mapped() noexcept
	{
		return val;
	}
	constexpr mapped_type const &mapped() const noexcept
	{
		return val;
	}
};

template <::std::integral chtype, typename mappedtype>
inline constexpr bool operator==(::fast_io::containers::basic_str_btree_map_key_mapped_pair<chtype, mappedtype> const &a,
								 ::fast_io::containers::basic_str_btree_map_key_mapped_pair<chtype, mappedtype> const &b) noexcept
{
	return a.key() == b.key() && a.mapped() == b.mapped();
}

namespace details
{

/*
str_btree_map_node keeps the same member order as str_btree_set_node and puts
the mapped values in a separate array at the tail. The shared prefix layout
lets every key-only btree helper (find/contains/lower_bound/insert position,
iterator traversal, fix_child_links) alias map nodes through
str_btree_set_common, so probing only ever touches the keys array.
Invariant: values[i] holds a live object iff i < size; slots beyond size are
raw storage.
*/
template <::std::integral chtype, typename mappedtype, ::std::size_t keys_number>
struct str_btree_map_node
{
	using char_type = chtype;
	using mapped_type = mappedtype;
	::std::size_t size;
	bool leaf;
	::fast_io::details::associative_string<char_type> keys[keys_number];
	str_btree_map_node *childrens[keys_number + 1u];
	str_btree_map_node *parent;
	::std::size_t parent_pos;
	mappedtype values[keys_number];
};

template <::std::integral chtype, typename mappedtype, ::std::size_t keys_number>
struct str_btree_map_iterator_proxy
{
	using char_type = chtype;
	using mapped_type = mappedtype;
	using node_type = ::fast_io::containers::details::str_btree_map_node<char_type, mapped_type, keys_number>;
	void const *ptr{};
	::std::size_t pos{};
	constexpr ::fast_io::containers::basic_cstring_view<char_type> key() const noexcept
	{
		return static_cast<node_type const *>(this->ptr)->keys[this->pos].strvw();
	}
	constexpr mapped_type &mapped() const noexcept
	{
		return const_cast<node_type *>(static_cast<node_type const *>(this->ptr))->values[this->pos];
	}
	constexpr str_btree_map_iterator_proxy const *operator->() const noexcept
	{
		return this;
	}
};

template <::std::integral chtype, typename mappedtype, ::std::size_t keys_number>
inline constexpr bool operator==(::fast_io::containers::details::str_btree_map_iterator_proxy<chtype, mappedtype, keys_number> const &a,
								 ::fast_io::containers::details::str_btree_map_iterator_proxy<chtype, mappedtype, keys_number> const &b) noexcept
{
	return a.key() == b.key() && a.mapped() == b.mapped();
}

#if __cpp_impl_three_way_comparison >= 201907L
template <::std::integral chtype, typename mappedtype, ::std::size_t keys_number>
inline constexpr auto operator<=>(::fast_io::containers::details::str_btree_map_iterator_proxy<chtype, mappedtype, keys_number> const &a,
								  ::fast_io::containers::details::str_btree_map_iterator_proxy<chtype, mappedtype, keys_number> const &b) noexcept
{
	using order_type = ::std::common_comparison_category_t<
		decltype(::std::declval<::fast_io::containers::basic_cstring_view<chtype>>() <=> ::std::declval<::fast_io::containers::basic_cstring_view<chtype>>()),
		::std::compare_three_way_result_t<mappedtype>>;
	if (auto cmp{a.key() <=> b.key()}; cmp != 0)
	{
		return static_cast<order_type>(cmp);
	}
	return static_cast<order_type>(::std::compare_three_way{}(a.mapped(), b.mapped()));
}
#endif

template <::std::integral chtype, typename mappedtype, ::std::size_t keys_number>
class str_btree_map_iterator
{
public:
	using value_type = ::fast_io::containers::details::str_btree_map_iterator_proxy<chtype, mappedtype, keys_number>;
	using iterator_tag = ::std::bidirectional_iterator_tag;
	using difference_type = ::std::ptrdiff_t;
	::fast_io::containers::details::str_btree_set_iterator_common node;

	constexpr str_btree_map_iterator &operator++() noexcept
	{
		::fast_io::containers::details::str_btree_set_next_node<keys_number>(this->node);
		return *this;
	}
	constexpr str_btree_map_iterator operator++(int) noexcept
	{
		auto tmp{*this};
		++*this;
		return tmp;
	}
	constexpr str_btree_map_iterator &operator--() noexcept
	{
		::fast_io::containers::details::str_btree_set_prev_node<keys_number>(this->node);
		return *this;
	}
	constexpr str_btree_map_iterator operator--(int) noexcept
	{
		auto tmp{*this};
		--*this;
		return tmp;
	}
	constexpr value_type operator*() const noexcept
	{
		return {this->node.ptr, this->node.pos};
	}
	constexpr value_type operator->() const noexcept
	{
		return {this->node.ptr, this->node.pos};
	}
};

template <::std::integral chtype, typename mappedtype, ::std::size_t keys_number>
inline constexpr bool operator==(::fast_io::containers::details::str_btree_map_iterator<chtype, mappedtype, keys_number> const &a,
								 ::fast_io::containers::details::str_btree_map_iterator<chtype, mappedtype, keys_number> const &b) noexcept
{
	return a.node.ptr == b.node.ptr && a.node.pos == b.node.pos;
}

template <::std::integral chtype, typename mappedtype, ::std::size_t keys_number>
inline constexpr bool operator!=(::fast_io::containers::details::str_btree_map_iterator<chtype, mappedtype, keys_number> const &a,
								 ::fast_io::containers::details::str_btree_map_iterator<chtype, mappedtype, keys_number> const &b) noexcept
{
	return !operator==(a, b);
}

/* Move-construct [first, last) into raw storage at out; returns advanced out. */
template <typename T>
inline constexpr T *str_btree_map_uninit_move(T *first, T *last, T *out) noexcept
{
	for (; first != last; ++first, ++out)
	{
		::new (static_cast<void *>(out)) T(::std::move(*first));
	}
	return out;
}

template <typename T>
inline constexpr void str_btree_map_uninit_move_n(T *first, ::std::size_t n, T *out) noexcept
{
	for (; n != 0u; --n, ++first, ++out)
	{
		::new (static_cast<void *>(out)) T(::std::move(*first));
	}
}

/*
Insert m at pos into a live range [0, n); the slot at n must be raw storage.
After the shift, slot pos holds a moved-from object which is destroyed and
reconstructed from m so only constructibility (never assignability) of the
source type is required.
*/
template <typename T, typename M>
inline constexpr void str_btree_map_values_insert_at(T *values, ::std::size_t pos, ::std::size_t n, M &&m)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_constructible_v<T, M &&>)
{
	if (pos == n)
	{
		::new (static_cast<void *>(values + pos)) T(::std::forward<M>(m));
		return;
	}
	::new (static_cast<void *>(values + n)) T(::std::move(values[n - 1u]));
	::std::move_backward(values + pos, values + (n - 1u), values + n);
	::std::destroy_at(values + pos);
	::new (static_cast<void *>(values + pos)) T(::std::forward<M>(m));
}

/* Erase pos from live range [0, n), leaving live range [0, n-1). */
template <typename T>
inline constexpr void str_btree_map_values_erase_at(T *values, ::std::size_t pos, ::std::size_t n) noexcept
{
	::std::move(values + (pos + 1u), values + n, values + pos);
	::std::destroy_at(values + (n - 1u));
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype, typename mappedarg>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr bool str_btree_map_insert_key_cold(nodetype *node, ::std::size_t pos,
													typename nodetype::char_type const *tempkeystrptr, ::std::size_t tempkeystrn,
													mappedarg &&mapped,
													::fast_io::containers::details::btree_imp &imp)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_constructible_v<typename nodetype::mapped_type, mappedarg &&>)
{
	using char_type = typename nodetype::char_type;
	using mapped_type = typename nodetype::mapped_type;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;

	auto keys{node->keys};
	auto values{node->values};
	auto keysit{keys + pos};
	auto keysed{keys + keys_number};
	auto valuesit{values + pos};
	auto valuesed{values + keys_number};

	constexpr ::std::size_t keys_number_half{keys_number >> (1u)};
	constexpr ::std::size_t keys_number_half_p1{keys_number_half + 1u};
	// ** Split now
	auto rightchild{typed_allocator_type::allocate(1)};
	if (node == imp.rightmost)
	{
		imp.rightmost = rightchild;
	}
	node->leaf = rightchild->leaf = true;
	node->size = rightchild->size = keys_number_half;

	auto rightchildkeys{rightchild->keys};
	auto rightchildvalues{rightchild->values};
	char_type const *movekeystrptr{};
	::std::size_t movekeystrn{};
	// the promoted mapped value rides up the parent chain inside raw storage
	alignas(mapped_type)::std::byte movevalbuf[sizeof(mapped_type)];
	auto moveval{reinterpret_cast<mapped_type *>(__builtin_addressof(movevalbuf))};
	auto midptr{keys + keys_number_half};
	auto midvptr{values + keys_number_half};
	auto poskeys_number_halfcmp{pos <=> keys_number_half};

	if (poskeys_number_halfcmp < 0)
	{
		auto &keystrptrkeysnumber{midptr[-1]};
		movekeystrptr = keystrptrkeysnumber.ptr;
		movekeystrn = keystrptrkeysnumber.n;
		::new (static_cast<void *>(moveval)) mapped_type(::std::move(midvptr[-1]));

		::fast_io::details::non_overlapped_copy_n(midptr, keys_number_half, rightchildkeys);
		str_btree_map_uninit_move_n(midvptr, keys_number_half, rightchildvalues);
		::fast_io::freestanding::overlapped_copy(keys + pos, keys + keys_number_half, keys + pos + 1);
		::std::move_backward(values + pos, values + (keys_number_half - 1u), values + keys_number_half);

		keys[pos] = {tempkeystrptr, tempkeystrn};
		::std::destroy_at(values + pos);
		::new (static_cast<void *>(values + pos)) mapped_type(::std::forward<mappedarg>(mapped));
	}
	else if (poskeys_number_halfcmp == 0)
	{
		::fast_io::details::non_overlapped_copy_n(midptr, keys_number_half, rightchildkeys);
		str_btree_map_uninit_move_n(midvptr, keys_number_half, rightchildvalues);
		movekeystrptr = tempkeystrptr;
		movekeystrn = tempkeystrn;
		::new (static_cast<void *>(moveval)) mapped_type(::std::forward<mappedarg>(mapped));
	}
	else
	{
		auto &keystrptrkeysnumber{*midptr};
		movekeystrptr = keystrptrkeysnumber.ptr;
		movekeystrn = keystrptrkeysnumber.n;
		::new (static_cast<void *>(moveval)) mapped_type(::std::move(*midvptr));
		auto it{::fast_io::details::non_overlapped_copy(midptr + 1, keysit, rightchildkeys)};
		auto vit{str_btree_map_uninit_move(midvptr + 1, valuesit, rightchildvalues)};
		*it = {tempkeystrptr, tempkeystrn};
		::new (static_cast<void *>(vit)) mapped_type(::std::forward<mappedarg>(mapped));
		++it;
		++vit;
		::fast_io::details::non_overlapped_copy(keysit, keysed, it);
		str_btree_map_uninit_move(valuesit, valuesed, vit);
	}
	// the left node's tail slots [half, keys_number) are dead now; end their lifetimes
	for (auto v{values + keys_number_half}; v != valuesed; ++v)
	{
		::std::destroy_at(v);
	}
	::std::size_t child_pos{node->parent_pos};
	for (auto j{node->parent}; j; j = j->parent)
	{
		auto jkeys{j->keys};
		auto jvalues{j->values};
		auto jchildrens{j->childrens};
		auto jn{j->size};

		// If parent node has space, insert the promoted key and return
		if (jn != keys_number)
		{
			// Shift keys and children to make room for the promoted key and new child
			::fast_io::freestanding::overlapped_copy(jkeys + child_pos, jkeys + jn, jkeys + child_pos + 1);
			jkeys[child_pos] = {movekeystrptr, movekeystrn};
			str_btree_map_values_insert_at(jvalues, child_pos, jn, ::std::move(*moveval));
			::std::destroy_at(moveval);
			::fast_io::freestanding::overlapped_copy(jchildrens + child_pos + 1, jchildrens + jn + 1, jchildrens + child_pos + 2);
			jchildrens[child_pos + 1] = rightchild;
			rightchild->parent = j;
			rightchild->parent_pos = child_pos + 1;

			for (auto k{jchildrens + (child_pos + 2)}, ked{jchildrens + (jn + 2)}; k != ked; ++k)
			{
				(*k)->parent_pos = static_cast<::std::size_t>(k - jchildrens);
			}
			++j->size;

			return true;
		}
		// Parent is full, must split upward
		auto new_right = typed_allocator_type::allocate(1);
		j->leaf = new_right->leaf = false;
		j->size = new_right->size = keys_number_half;

		auto jmidptr = jkeys + keys_number_half;
		auto jmidvptr = jvalues + keys_number_half;

		auto new_right_keys{new_right->keys};
		auto new_right_values{new_right->values};
		auto new_right_childrens{new_right->childrens};
		auto child_poskeys_number_halfcmp{child_pos <=> keys_number_half};

		// second transport for the pair this node promotes upward
		alignas(mapped_type)::std::byte mvupbuf[sizeof(mapped_type)];
		auto mvup{reinterpret_cast<mapped_type *>(__builtin_addressof(mvupbuf))};

		// CASE 1: promoted child is in the left half (before the mid key)
		if (child_poskeys_number_halfcmp < 0)
		{
			auto &jmidkey = jmidptr[-1];
			auto tmpptr{jmidkey.ptr};
			auto tmpn{jmidkey.n};
			::new (static_cast<void *>(mvup)) mapped_type(::std::move(jmidvptr[-1]));

			::fast_io::details::non_overlapped_copy_n(jmidptr, keys_number_half, new_right_keys);
			str_btree_map_uninit_move_n(jmidvptr, keys_number_half, new_right_values);
			::fast_io::freestanding::overlapped_copy(jkeys + child_pos, jmidptr, jkeys + child_pos + 1);
			jkeys[child_pos] = {movekeystrptr, movekeystrn};
			::std::move_backward(jvalues + child_pos, jmidvptr - 1, jmidvptr);
			::std::destroy_at(jvalues + child_pos);
			::new (static_cast<void *>(jvalues + child_pos)) mapped_type(::std::move(*moveval));
			*moveval = ::std::move(*mvup);
			::std::destroy_at(mvup);
			movekeystrptr = tmpptr;
			movekeystrn = tmpn;

			::fast_io::details::non_overlapped_copy_n(jchildrens + keys_number_half, keys_number_half_p1, new_right_childrens);
			::fast_io::freestanding::overlapped_copy(jchildrens + child_pos + 1, jchildrens + keys_number_half_p1, jchildrens + child_pos + 2);
			jchildrens[child_pos + 1] = rightchild;
			rightchild->parent = j;
			rightchild->parent_pos = child_pos + 1;

			for (::std::size_t i{child_pos + 2}; i != keys_number_half_p1; ++i)
			{
				jchildrens[i]->parent_pos = i;
			}
		}
		else if (child_poskeys_number_halfcmp == 0)
		{
			::fast_io::details::non_overlapped_copy_n(jmidptr, keys_number_half, new_right_keys);
			str_btree_map_uninit_move_n(jmidvptr, keys_number_half, new_right_values);
			// movekeystrptr/moveval already carry the incoming pair upward
			::fast_io::details::non_overlapped_copy_n(jchildrens + keys_number_half_p1, keys_number_half, new_right_childrens + 1);
			*new_right_childrens = rightchild;
		}
		else
		{
			auto &jkeystrptrkeysnumber{*jmidptr};
			auto tmpptr{jkeystrptrkeysnumber.ptr};
			auto tmpn{jkeystrptrkeysnumber.n};
			::new (static_cast<void *>(mvup)) mapped_type(::std::move(*jmidvptr));

			auto jkeysit{jkeys + child_pos};
			auto jkeysed{jkeys + keys_number};
			auto it{::fast_io::details::non_overlapped_copy(jmidptr + 1, jkeysit, new_right_keys)};
			auto vit{str_btree_map_uninit_move(jmidvptr + 1, jvalues + child_pos, new_right_values)};
			*it = {movekeystrptr, movekeystrn};
			::new (static_cast<void *>(vit)) mapped_type(::std::move(*moveval));
			*moveval = ::std::move(*mvup);
			::std::destroy_at(mvup);
			movekeystrptr = tmpptr;
			movekeystrn = tmpn;
			++it;
			++vit;
			::fast_io::details::non_overlapped_copy(jkeysit, jkeysed, it);
			str_btree_map_uninit_move(jvalues + child_pos, jvalues + keys_number, vit);

			auto jchildrensit{jchildrens + child_pos + 1};
			auto jchildrensed{jchildrens + keys_number + 1};
			auto kit{::fast_io::details::non_overlapped_copy(jchildrens + keys_number_half_p1, jchildrensit, new_right_childrens)};
			*kit = rightchild;
			++kit;
			::fast_io::details::non_overlapped_copy(jchildrensit, jchildrensed, kit);
		}
		// j keeps only its left half; end the lifetimes of the moved-out tail slots
		for (auto v{jvalues + keys_number_half}; v != jvalues + keys_number; ++v)
		{
			::std::destroy_at(v);
		}

		for (auto k{new_right_childrens}, ked{new_right_childrens + keys_number_half_p1}; k != ked; ++k)
		{
			auto &kref{**k};
			kref.parent = new_right;
			kref.parent_pos = static_cast<::std::size_t>(k - new_right_childrens);
		}
		rightchild = new_right;
		child_pos = j->parent_pos;
		node = j;
	}
	auto new_root = typed_allocator_type::allocate(1);
	new_root->leaf = false;
	new_root->size = 1;
	new_root->keys->ptr = movekeystrptr;
	new_root->keys->n = movekeystrn;
	::new (static_cast<void *>(new_root->values)) mapped_type(::std::move(*moveval));
	::std::destroy_at(moveval);
	new_root->parent = nullptr;
	new_root->parent_pos = 0;
	*(new_root->childrens) = node;
	new_root->childrens[1] = rightchild;
	node->parent = new_root;
	node->parent_pos = 0;
	rightchild->parent = new_root;
	rightchild->parent_pos = 1;
	imp.root = new_root;

	return true;
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype, typename mappedarg>
inline constexpr bool str_btree_map_insert_key_leaf_at(nodetype *node, ::std::size_t pos,
													   typename nodetype::char_type const *keystrptr, ::std::size_t keystrn,
													   mappedarg &&mapped,
													   ::fast_io::containers::details::btree_imp &imp)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_constructible_v<typename nodetype::mapped_type, mappedarg &&>)
{
	using char_type = typename nodetype::char_type;
	using mapped_type = typename nodetype::mapped_type;
	auto tempkey{::fast_io::details::create_associative_string<allocator_type, char_type>(keystrptr, keystrn)};
	auto n{node->size};
	if (n != keys_number)
	{
		auto keys{node->keys};
		::fast_io::freestanding::overlapped_copy(keys + pos, keys + n, keys + pos + 1);
		keys[pos] = tempkey;
		str_btree_map_values_insert_at(node->values, pos, n, ::std::forward<mappedarg>(mapped));
		++node->size;
		return true;
	}
	return str_btree_map_insert_key_cold<allocator_type, keys_number, nodetype, mappedarg>(node, pos, tempkey.ptr, tempkey.n,
																						   ::std::forward<mappedarg>(mapped), imp);
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype, typename mappedarg>
inline constexpr bool str_btree_map_insert_key_with_root(::fast_io::containers::details::btree_imp &imp,
														 typename nodetype::char_type const *keystrptr, ::std::size_t keystrn,
														 mappedarg &&mapped)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_constructible_v<typename nodetype::mapped_type, mappedarg &&>)
{
	using char_type = typename nodetype::char_type;
	using mapped_type = typename nodetype::mapped_type;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;

	auto node{static_cast<nodetype *>(imp.root)};

	// **If the tree is empty, allocate a new root**
	if (node == nullptr)
	{
		node = typed_allocator_type::allocate(1);
		node->size = 1;
		node->leaf = true;
		node->parent = nullptr;
		node->parent_pos = 0;
		*(node->keys) = ::fast_io::details::create_associative_string<allocator_type, char_type>(keystrptr, keystrn);
		::new (static_cast<void *>(node->values)) mapped_type(::std::forward<mappedarg>(mapped));
		imp.rightmost = imp.leftmost = imp.root = node;
		return true;
	}

	::std::size_t pos;
	// **Find the correct position for insertion**
	for (;;)
	{
		auto [postemp, found] = ::fast_io::containers::details::find_str_btree_node_insert_position(node, keystrptr, keystrn);
		pos = postemp;
		// **If the key already exists, return false (no duplicate keys)**
		if (found)
		{
			return false;
		}
		// **If the node is a leaf**
		if (node->leaf)
		{
			break;
		}
		node = node->childrens[pos];
	}

	auto tempkey = ::fast_io::details::create_associative_string<allocator_type, char_type>(keystrptr, keystrn);
	auto n{node->size};
	// **If there is space, insert the key directly**
	if (n != keys_number)
	{
		auto keys{node->keys};
		::fast_io::freestanding::overlapped_copy(keys + pos, keys + n, keys + pos + 1);
		keys[pos] = tempkey;
		str_btree_map_values_insert_at(node->values, pos, n, ::std::forward<mappedarg>(mapped));
		++node->size;
		return true;
	}
	return str_btree_map_insert_key_cold<allocator_type, keys_number, nodetype, mappedarg>(node, pos, tempkey.ptr, tempkey.n,
																						   ::std::forward<mappedarg>(mapped), imp);
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype, typename mappedarg>
inline constexpr bool str_btree_map_insert_key_or_assign_with_root(::fast_io::containers::details::btree_imp &imp,
																   typename nodetype::char_type const *keystrptr, ::std::size_t keystrn,
																   mappedarg &&mapped)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_constructible_v<typename nodetype::mapped_type, mappedarg &&>)
{
	using char_type = typename nodetype::char_type;
	using mapped_type = typename nodetype::mapped_type;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;

	auto node{static_cast<nodetype *>(imp.root)};

	if (node == nullptr)
	{
		node = typed_allocator_type::allocate(1);
		node->size = 1;
		node->leaf = true;
		node->parent = nullptr;
		node->parent_pos = 0;
		*(node->keys) = ::fast_io::details::create_associative_string<allocator_type, char_type>(keystrptr, keystrn);
		::new (static_cast<void *>(node->values)) mapped_type(::std::forward<mappedarg>(mapped));
		imp.rightmost = imp.leftmost = imp.root = node;
		return true;
	}

	::std::size_t pos;
	for (;;)
	{
		auto [postemp, found] = ::fast_io::containers::details::find_str_btree_node_insert_position(node, keystrptr, keystrn);
		pos = postemp;
		if (found)
		{
			auto slot{node->values + pos};
			::std::destroy_at(slot);
			::new (static_cast<void *>(slot)) mapped_type(::std::forward<mappedarg>(mapped));
			return false;
		}
		if (node->leaf)
		{
			break;
		}
		node = node->childrens[pos];
	}

	auto tempkey = ::fast_io::details::create_associative_string<allocator_type, char_type>(keystrptr, keystrn);
	auto n{node->size};
	if (n != keys_number)
	{
		auto keys{node->keys};
		::fast_io::freestanding::overlapped_copy(keys + pos, keys + n, keys + pos + 1);
		keys[pos] = tempkey;
		str_btree_map_values_insert_at(node->values, pos, n, ::std::forward<mappedarg>(mapped));
		++node->size;
		return true;
	}
	return str_btree_map_insert_key_cold<allocator_type, keys_number, nodetype, mappedarg>(node, pos, tempkey.ptr, tempkey.n,
																						   ::std::forward<mappedarg>(mapped), imp);
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype, typename mappedarg>
inline constexpr bool str_btree_map_insert_key_hint(::fast_io::containers::details::btree_imp &imp,
													void const *hintptr, ::std::size_t hintpos,
													typename nodetype::char_type const *keystrptr, ::std::size_t keystrn,
													mappedarg &&mapped)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_constructible_v<typename nodetype::mapped_type, mappedarg &&>)
{
	using char_type = typename nodetype::char_type;
	::fast_io::containers::basic_string_view<char_type> key{keystrptr, keystrn};
	if (imp.root == nullptr)
	{
		return str_btree_map_insert_key_with_root<allocator_type, keys_number, nodetype, mappedarg>(imp, keystrptr, keystrn,
																									::std::forward<mappedarg>(mapped));
	}
	if (hintptr == nullptr)
	{
		// hint == end(): fast path for appending keys in order
		auto rm{static_cast<nodetype *>(imp.rightmost)};
		if (rm != nullptr && rm->size != 0 && rm->keys[rm->size - 1u].strvw() < key)
		{
			return str_btree_map_insert_key_leaf_at<allocator_type, keys_number, nodetype, mappedarg>(
				rm, rm->size, keystrptr, keystrn, ::std::forward<mappedarg>(mapped), imp);
		}
		return str_btree_map_insert_key_with_root<allocator_type, keys_number, nodetype, mappedarg>(imp, keystrptr, keystrn,
																									::std::forward<mappedarg>(mapped));
	}
	auto node{static_cast<nodetype *>(const_cast<void *>(hintptr))};
	if (!node->leaf)
	{
		return str_btree_map_insert_key_with_root<allocator_type, keys_number, nodetype, mappedarg>(imp, keystrptr, keystrn,
																									::std::forward<mappedarg>(mapped));
	}
	if (!(key < node->keys[hintpos].strvw()))
	{
		return str_btree_map_insert_key_with_root<allocator_type, keys_number, nodetype, mappedarg>(imp, keystrptr, keystrn,
																									::std::forward<mappedarg>(mapped));
	}
	if (hintpos != 0)
	{
		if (!(node->keys[hintpos - 1u].strvw() < key))
		{
			return str_btree_map_insert_key_with_root<allocator_type, keys_number, nodetype, mappedarg>(imp, keystrptr, keystrn,
																										::std::forward<mappedarg>(mapped));
		}
	}
	else if (node != imp.leftmost)
	{
		return str_btree_map_insert_key_with_root<allocator_type, keys_number, nodetype, mappedarg>(imp, keystrptr, keystrn,
																									::std::forward<mappedarg>(mapped));
	}
	return str_btree_map_insert_key_leaf_at<allocator_type, keys_number, nodetype, mappedarg>(node, hintpos, keystrptr, keystrn,
																							  ::std::forward<mappedarg>(mapped), imp);
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr void str_btree_map_erase_underflow(::fast_io::containers::details::btree_imp &imp, nodetype *node) noexcept
{
	using nodeptr = nodetype *;
	using mapped_type = typename nodetype::mapped_type;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;

	constexpr ::std::size_t min_keys{(keys_number >> 1u) - 1u};

	auto current = node;
	while (current && current->size < min_keys)
	{
		auto parent = current->parent;

		// Case 1: current is root
		if (!parent)
		{
			if (current->size == 0)
			{
				// If root has no keys, promote its only child (if any)
				if (current->childrens[0])
				{
					imp.root = current->childrens[0];
					auto newroot = static_cast<nodeptr>(imp.root);
					newroot->parent = nullptr;
					newroot->parent_pos = 0;
				}
				else
				{
					// Tree becomes empty
					imp = {};
				}
				typed_allocator_type::deallocate_n(current, 1);
			}
			break;
		}

		std::size_t parpos = current->parent_pos;
		std::size_t parent_size = parent->size;

		// Try borrow from right sibling
		if (parpos != parent_size)
		{
			auto right = static_cast<nodeptr>(parent->childrens[parpos + 1]);
			if (right->size > min_keys)
			{
				auto const current_size_before{current->size};
				// Move parent key down into current
				current->keys[current_size_before] = parent->keys[parpos];
				::new (static_cast<void *>(current->values + current_size_before)) mapped_type(::std::move(parent->values[parpos]));
				if (!current->leaf)
				{
					// Move right sibling's leftmost child into current's new last child slot
					current->childrens[current_size_before + 1] = right->childrens[0];
					auto moved_child{static_cast<nodeptr>(right->childrens[0])};
					if (moved_child)
					{
						moved_child->parent = current;
						moved_child->parent_pos = current_size_before + 1;
					}
					::fast_io::freestanding::overlapped_copy(
						right->childrens + 1, right->childrens + right->size + 1, right->childrens);
				}
				++current->size;

				// Update parent key to right sibling's leftmost
				parent->keys[parpos] = right->keys[0];
				parent->values[parpos] = ::std::move(right->values[0]);

				// Shift right sibling's keys left
				::fast_io::freestanding::overlapped_copy(
					right->keys + 1, right->keys + right->size, right->keys);
				str_btree_map_values_erase_at(right->values, 0, right->size);
				--right->size;
				::fast_io::containers::details::fix_child_links<keys_number>(
					reinterpret_cast<::fast_io::containers::details::str_btree_set_common<keys_number> *>(current));
				::fast_io::containers::details::fix_child_links<keys_number>(
					reinterpret_cast<::fast_io::containers::details::str_btree_set_common<keys_number> *>(right));
				return; // fixed
			}
		}

		// Try borrow from left sibling
		if (parpos != 0)
		{
			auto left = static_cast<nodeptr>(parent->childrens[parpos - 1]);
			if (left->size > min_keys)
			{
				auto const current_size_before{current->size};
				auto const left_size_before{left->size};
				// Shift current keys right
				::fast_io::freestanding::overlapped_copy(
					current->keys, current->keys + current_size_before, current->keys + 1);
				if (!current->leaf)
				{
					// Shift current children right to make room for the borrowed child
					::fast_io::freestanding::overlapped_copy(
						current->childrens, current->childrens + current_size_before + 1, current->childrens + 1);
					current->childrens[0] = left->childrens[left_size_before];
					auto moved_child{static_cast<nodeptr>(left->childrens[left_size_before])};
					if (moved_child)
					{
						moved_child->parent = current;
						moved_child->parent_pos = 0;
					}
				}

				// Move parent key down
				current->keys[0] = parent->keys[parpos - 1];
				str_btree_map_values_insert_at(current->values, 0, current_size_before,
											   ::std::move(parent->values[parpos - 1]));
				++current->size;

				// Update parent key to left sibling's rightmost
				parent->keys[parpos - 1] = left->keys[left_size_before - 1];
				parent->values[parpos - 1] = ::std::move(left->values[left_size_before - 1]);
				::std::destroy_at(left->values + (left_size_before - 1u));
				--left->size;
				::fast_io::containers::details::fix_child_links<keys_number>(
					reinterpret_cast<::fast_io::containers::details::str_btree_set_common<keys_number> *>(current));
				::fast_io::containers::details::fix_child_links<keys_number>(
					reinterpret_cast<::fast_io::containers::details::str_btree_set_common<keys_number> *>(left));
				return; // fixed
			}
		}

		// Merge: prefer right sibling
		if (parpos != parent_size)
		{
			auto right = static_cast<nodeptr>(parent->childrens[parpos + 1]);
			auto const current_size_before{current->size};
			auto const right_size_before{right->size};

			// Bring down parent key
			current->keys[current_size_before] = parent->keys[parpos];
			::new (static_cast<void *>(current->values + current_size_before)) mapped_type(::std::move(parent->values[parpos]));

			// Append right sibling's keys
			::fast_io::freestanding::non_overlapped_copy(
				right->keys, right->keys + right_size_before, current->keys + current_size_before + 1);
			str_btree_map_uninit_move(right->values, right->values + right_size_before,
									  current->values + current_size_before + 1);
			if (!current->leaf)
			{
				::fast_io::freestanding::non_overlapped_copy(
					right->childrens, right->childrens + right_size_before + 1,
					current->childrens + current_size_before + 1);
				for (::std::size_t i{}; i <= right_size_before; ++i)
				{
					auto child{static_cast<nodeptr>(right->childrens[i])};
					if (child)
					{
						child->parent = current;
						child->parent_pos = current_size_before + 1 + i;
					}
				}
			}
			current->size += right_size_before + 1;

			if (right == imp.rightmost)
			{
				imp.rightmost = current;
			}
			// Free right sibling
			for (auto v{right->values}, ved{right->values + right_size_before}; v != ved; ++v)
			{
				::std::destroy_at(v);
			}
			typed_allocator_type::deallocate_n(right, 1);

			// Shift parent's keys and children
			::fast_io::freestanding::overlapped_copy(
				parent->keys + parpos + 1, parent->keys + parent_size, parent->keys + parpos);
			str_btree_map_values_erase_at(parent->values, parpos, parent_size);
			::fast_io::freestanding::overlapped_copy(
				parent->childrens + parpos + 2, parent->childrens + parent_size + 1,
				parent->childrens + parpos + 1);
			--parent->size;
			::fast_io::containers::details::fix_child_links<keys_number>(
				reinterpret_cast<::fast_io::containers::details::str_btree_set_common<keys_number> *>(parent));

			current = parent; // move up
		}
		else
		{
			// Merge with left sibling
			auto left = static_cast<nodeptr>(parent->childrens[parpos - 1]);
			auto const left_size_before{left->size};
			auto const current_size_before{current->size};

			// Bring down parent key
			left->keys[left_size_before] = parent->keys[parpos - 1];
			::new (static_cast<void *>(left->values + left_size_before)) mapped_type(::std::move(parent->values[parpos - 1]));

			// Append current's keys
			::fast_io::freestanding::non_overlapped_copy(
				current->keys, current->keys + current_size_before, left->keys + left_size_before + 1);
			str_btree_map_uninit_move(current->values, current->values + current_size_before,
									  left->values + left_size_before + 1);
			if (!left->leaf)
			{
				::fast_io::freestanding::non_overlapped_copy(
					current->childrens, current->childrens + current_size_before + 1,
					left->childrens + left_size_before + 1);
				for (::std::size_t i{}; i <= current_size_before; ++i)
				{
					auto child{static_cast<nodeptr>(current->childrens[i])};
					if (child)
					{
						child->parent = left;
						child->parent_pos = left_size_before + 1 + i;
					}
				}
			}
			left->size += current_size_before + 1;

			if (current == imp.rightmost)
			{
				imp.rightmost = left;
			}
			// Free current
			for (auto v{current->values}, ved{current->values + current_size_before}; v != ved; ++v)
			{
				::std::destroy_at(v);
			}
			typed_allocator_type::deallocate_n(current, 1);

			// Shift parent's keys and children
			::fast_io::freestanding::overlapped_copy(
				parent->keys + parpos, parent->keys + parent_size, parent->keys + parpos - 1);
			str_btree_map_values_erase_at(parent->values, parpos - 1, parent_size);
			::fast_io::freestanding::overlapped_copy(
				parent->childrens + parpos + 1, parent->childrens + parent_size + 1,
				parent->childrens + parpos);
			--parent->size;
			::fast_io::containers::details::fix_child_links<keys_number>(
				reinterpret_cast<::fast_io::containers::details::str_btree_set_common<keys_number> *>(parent));

			current = parent; // move up
		}
	}
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr bool str_btree_map_erase(::fast_io::containers::details::btree_imp &imp,
										  typename nodetype::char_type const *keystrptr,
										  ::std::size_t keystrn) noexcept
{
	using char_type = typename nodetype::char_type;
	using mapped_type = typename nodetype::mapped_type;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;

	constexpr std::size_t min_keys{(keys_number >> 1u) - 1u};

	auto root_node{static_cast<nodetype *>(imp.root)};
	auto [found_ptr, found_pos]{::fast_io::containers::details::str_btree_find(root_node, keystrptr, keystrn)};
	if (found_ptr == nullptr)
	{
		return false;
	}

	auto target_node{static_cast<nodetype *>(found_ptr)};
	auto target_keys{target_node->keys};
	auto target_values{target_node->values};
	auto target_parent{target_node->parent};

	// Case 1: deletion from a leaf node
	if (target_node->leaf)
	{
		// Free payload memory owned by the leaf
		::fast_io::details::deallocate_associative_string<allocator_type, char_type>(
			target_keys[found_pos].ptr, target_keys[found_pos].n);

		// Physically remove the key by shifting left
		::fast_io::freestanding::overlapped_copy(target_keys + found_pos + 1, target_keys + target_node->size, target_keys + found_pos);
		str_btree_map_values_erase_at(target_values, found_pos, target_node->size);
		--target_node->size;

		// If root and now empty, free root and reset tree
		if (target_parent == nullptr)
		{
			if (target_node->size == 0)
			{
				typed_allocator_type::deallocate_n(target_node, 1);
				imp = {};
			}
			return true;
		}

		if (target_node->size >= min_keys)
		{
			return true;
		}

		// Leaf underflow: try borrow or merge
		std::size_t parent_pos{target_node->parent_pos};
		std::size_t parent_size{target_parent->size};

		// Borrow from right sibling
		if (parent_pos != parent_size)
		{
			auto right_sibling{target_parent->childrens[parent_pos + 1]};
			if (right_sibling->size > min_keys)
			{
				target_keys[target_node->size] = target_parent->keys[parent_pos];
				::new (static_cast<void *>(target_values + target_node->size)) mapped_type(::std::move(target_parent->values[parent_pos]));
				++target_node->size;
				target_parent->keys[parent_pos] = right_sibling->keys[0];
				target_parent->values[parent_pos] = ::std::move(right_sibling->values[0]);
				::fast_io::freestanding::overlapped_copy(
					right_sibling->keys + 1, right_sibling->keys + right_sibling->size, right_sibling->keys);
				str_btree_map_values_erase_at(right_sibling->values, 0, right_sibling->size);
				--right_sibling->size;
				return true;
			}
		}

		// Borrow from left sibling
		if (parent_pos != 0)
		{
			auto left_sibling{target_parent->childrens[parent_pos - 1]};
			if (left_sibling->size > min_keys)
			{
				::fast_io::freestanding::overlapped_copy(target_keys, target_keys + target_node->size, target_keys + 1);
				target_keys[0] = target_parent->keys[parent_pos - 1];
				str_btree_map_values_insert_at(target_values, 0, target_node->size,
											   ::std::move(target_parent->values[parent_pos - 1]));
				++target_node->size;
				target_parent->keys[parent_pos - 1] = left_sibling->keys[left_sibling->size - 1];
				target_parent->values[parent_pos - 1] = ::std::move(left_sibling->values[left_sibling->size - 1]);
				::std::destroy_at(left_sibling->values + (left_sibling->size - 1u));
				--left_sibling->size;
				return true;
			}
		}

		// Merge with sibling (prefer right if possible)
		bool merge_with_right{parent_pos != parent_size};
		nodetype *merged_target{};
		nodetype *merged_victim{};
		std::size_t parent_key_index{};
		if (merge_with_right)
		{
			merged_target = target_node;
			merged_victim = target_parent->childrens[parent_pos + 1];
			parent_key_index = parent_pos;
			merged_target->keys[merged_target->size] = target_parent->keys[parent_key_index];
			::new (static_cast<void *>(merged_target->values + merged_target->size)) mapped_type(::std::move(target_parent->values[parent_key_index]));
			::fast_io::freestanding::non_overlapped_copy(
				merged_victim->keys, merged_victim->keys + merged_victim->size,
				merged_target->keys + merged_target->size + 1);
			str_btree_map_uninit_move(merged_victim->values, merged_victim->values + merged_victim->size,
									  merged_target->values + merged_target->size + 1);
			merged_target->size += merged_victim->size + 1;
		}
		else
		{
			merged_target = target_parent->childrens[parent_pos - 1];
			merged_victim = target_node;
			parent_key_index = parent_pos - 1;
			merged_target->keys[merged_target->size] = target_parent->keys[parent_key_index];
			::new (static_cast<void *>(merged_target->values + merged_target->size)) mapped_type(::std::move(target_parent->values[parent_key_index]));
			::fast_io::freestanding::non_overlapped_copy(
				target_keys, target_keys + target_node->size,
				merged_target->keys + merged_target->size + 1);
			str_btree_map_uninit_move(target_values, target_values + target_node->size,
									  merged_target->values + merged_target->size + 1);
			merged_target->size += target_node->size + 1;
		}

		// end the lifetimes of the victim's moved-out values before freeing the node
		for (auto v{merged_victim->values}, ved{merged_victim->values + merged_victim->size}; v != ved; ++v)
		{
			::std::destroy_at(v);
		}
		typed_allocator_type::deallocate_n(merged_victim, 1);
		if (merged_victim == imp.rightmost)
		{
			imp.rightmost = merged_target;
		}
		if (merged_victim == imp.leftmost)
		{
			imp.leftmost = merged_target;
		}

		::fast_io::freestanding::overlapped_copy(
			target_parent->keys + parent_key_index + 1, target_parent->keys + parent_size,
			target_parent->keys + parent_key_index);
		str_btree_map_values_erase_at(target_parent->values, parent_key_index, parent_size);
		::fast_io::freestanding::overlapped_copy(
			target_parent->childrens + parent_key_index + 2, target_parent->childrens + parent_size + 1,
			target_parent->childrens + parent_key_index + 1);
		--target_parent->size;
		for (std::size_t i{parent_key_index + 1}; i <= target_parent->size; ++i)
		{
			auto ch{target_parent->childrens[i]};
			if (ch)
			{
				ch->parent_pos = i;
			}
		}

		str_btree_map_erase_underflow<allocator_type, keys_number, nodetype>(imp, target_parent);
		return true;
	}

	// Case 2: deletion from an internal node
	auto internal_node{target_node};
	std::size_t internal_key_index{found_pos};
	auto left_child{internal_node->childrens[internal_key_index]};
	auto right_child{internal_node->childrens[internal_key_index + 1]};

	// Predecessor: replace internal key with rightmost key from left subtree
	if (min_keys < left_child->size)
	{
		auto descend{left_child};
		while (!descend->leaf)
		{
			descend = descend->childrens[descend->size]; // rightmost child
		}

		// Free payload of the original internal key being erased
		::fast_io::details::deallocate_associative_string<allocator_type, char_type>(
			internal_node->keys[internal_key_index].ptr,
			internal_node->keys[internal_key_index].n);

		// Move predecessor into internal slot (ownership transfer)
		auto const descend_size{descend->size};
		internal_node->keys[internal_key_index] = descend->keys[descend_size - 1u];
		internal_node->values[internal_key_index] = ::std::move(descend->values[descend_size - 1u]);

		// Remove predecessor from leaf WITHOUT deallocating (ownership moved)
		::std::destroy_at(descend->values + (descend_size - 1u));
		--descend->size;

		// Repair upward if leaf underflows
		if (descend->parent && descend->size < min_keys)
		{
			str_btree_map_erase_underflow<allocator_type, keys_number, nodetype>(imp, descend->parent);
		}
		return true;
	}

	// Successor
	if (right_child->size > min_keys)
	{
		auto descend{right_child};
		while (!descend->leaf)
		{
			descend = descend->childrens[0];
		}

		// Free payload of the original internal key being erased (fixes leak)
		::fast_io::details::deallocate_associative_string<allocator_type, char_type>(
			internal_node->keys[internal_key_index].ptr,
			internal_node->keys[internal_key_index].n);

		auto const descend_size{descend->size};
		internal_node->keys[internal_key_index] = descend->keys[0];
		internal_node->values[internal_key_index] = ::std::move(descend->values[0]);
		::fast_io::freestanding::overlapped_copy(descend->keys + 1, descend->keys + descend_size, descend->keys);
		str_btree_map_values_erase_at(descend->values, 0, descend_size);
		--descend->size;
		if (descend->parent && descend->size < min_keys)
		{
			str_btree_map_erase_underflow<allocator_type, keys_number, nodetype>(imp, descend->parent);
		}
		return true;
	}

	// Merge left child + internal key + right child into left child, then delete the key in the merged subtree.
	std::size_t left_size_before{left_child->size};
	std::size_t right_size_before{right_child->size};
	left_child->keys[left_size_before] = internal_node->keys[internal_key_index];
	::new (static_cast<void *>(left_child->values + left_size_before)) mapped_type(::std::move(internal_node->values[internal_key_index]));
	::fast_io::freestanding::non_overlapped_copy(
		right_child->keys, right_child->keys + right_size_before,
		left_child->keys + left_size_before + 1);
	str_btree_map_uninit_move(right_child->values, right_child->values + right_size_before,
							  left_child->values + left_size_before + 1);
	if (!left_child->leaf)
	{
		::fast_io::freestanding::non_overlapped_copy(
			right_child->childrens, right_child->childrens + right_size_before + 1,
			left_child->childrens + left_size_before + 1);
		for (std::size_t i{}; i <= right_size_before; ++i)
		{
			auto ch{right_child->childrens[i]};
			if (ch)
			{
				ch->parent = left_child;
				ch->parent_pos = left_size_before + 1 + i;
			}
		}
	}
	left_child->size = left_size_before + right_size_before + 1;
	if (right_child == imp.rightmost)
	{
		imp.rightmost = left_child;
	}
	for (auto v{right_child->values}, ved{right_child->values + right_size_before}; v != ved; ++v)
	{
		::std::destroy_at(v);
	}
	typed_allocator_type::deallocate_n(right_child, 1);

	std::size_t internal_size_before{internal_node->size};
	::fast_io::freestanding::overlapped_copy(
		internal_node->keys + internal_key_index + 1,
		internal_node->keys + internal_size_before,
		internal_node->keys + internal_key_index);
	str_btree_map_values_erase_at(internal_node->values, internal_key_index, internal_size_before);
	::fast_io::freestanding::overlapped_copy(
		internal_node->childrens + internal_key_index + 2,
		internal_node->childrens + internal_size_before + 1,
		internal_node->childrens + internal_key_index + 1);
	--internal_node->size;
	for (std::size_t i{internal_key_index + 1}; i <= internal_node->size; ++i)
	{
		auto ch{internal_node->childrens[i]};
		if (ch)
		{
			ch->parent_pos = i;
		}
	}

	// Repair potential underflow upward from the internal node
	str_btree_map_erase_underflow<allocator_type, keys_number, nodetype>(imp, internal_node);
	return str_btree_map_erase<allocator_type, keys_number, nodetype>(imp, keystrptr, keystrn);
}

template <typename chtype, typename valtype, typename R>
concept str_btree_map_range_has_key_mapped_val = ::std::integral<chtype> &&
												 ::std::ranges::range<R> &&
												 requires(::std::ranges::range_value_t<R> rgval) {
													 { rgval.key() } -> ::std::convertible_to<::fast_io::containers::basic_string_view<chtype>>;
													 { rgval.mapped() } -> ::std::convertible_to<valtype>;
												 };

template <typename chtype, typename valtype, typename R>
concept str_btree_map_range_has_key_mapped_noexcept = str_btree_map_range_has_key_mapped_val<chtype, valtype, R> &&
													  ::std::is_nothrow_constructible_v<valtype, decltype(::std::declval<::std::ranges::range_value_t<R>>().mapped())> &&
													  ::std::is_nothrow_constructible_v<::fast_io::containers::basic_string_view<chtype>, decltype(::std::declval<::std::ranges::range_value_t<R>>().key())>;

template <typename mappedtype>
inline constexpr bool str_btree_map_mapped_nothrow_copy_move{
	::std::is_nothrow_copy_constructible_v<mappedtype> && ::std::is_nothrow_move_constructible_v<mappedtype>};

// The member functions below all live in details as free functions: under
// herbceptions a 'throws'-marked member of a class template must not call
// another 'throws'-marked member through this->, so members only forward to
// these.
template <typename allocator_type, ::std::size_t keys_number, typename nodetype,
		  ::std::input_or_output_iterator Iter, ::std::sentinel_for<Iter> Sen>
inline constexpr void str_btree_map_insert_range_common(
	::fast_io::containers::details::btree_imp &imp, Iter first, Sen last)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_constructible_v<typename nodetype::mapped_type, decltype(::std::declval<Iter>()->mapped())>)
{
	using char_type = typename nodetype::char_type;
	for (; first != last; ++first)
	{
		::fast_io::containers::basic_string_view<char_type> k{first->key()};
		str_btree_map_insert_key_hint<allocator_type, keys_number, nodetype>(imp, nullptr, 0, k.ptr, k.n, first->mapped());
	}
}

} // namespace details

template <::std::integral chtype, typename mappedtype, typename Allocator, ::std::size_t keys_number = 16>
	requires((keys_number != 0) && ((keys_number & 1) == 0) &&
			 ::std::is_nothrow_move_constructible_v<mappedtype> &&
			 ::std::is_nothrow_move_assignable_v<mappedtype>)
class basic_str_btree_map
{
	using node_type = ::fast_io::containers::details::str_btree_map_node<chtype, mappedtype, keys_number>;
	using untyped_allocator_type = Allocator;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<untyped_allocator_type, node_type>;

public:
	using char_type = chtype;
	using mapped_type = mappedtype;
	using string_view_type = ::fast_io::containers::basic_string_view<char_type>;
	using cstring_view_type = ::fast_io::containers::basic_cstring_view<char_type>;
	using allocator_type = Allocator;
	using value_type = ::fast_io::containers::basic_str_btree_map_key_mapped_pair<char_type, mapped_type>;
	using initializer_list_pair_type = ::fast_io::containers::basic_str_btree_map_key_mapped_initializer_list_pair<char_type, mapped_type>;
	using const_iterator = ::fast_io::containers::details::str_btree_map_iterator<char_type, mapped_type, keys_number>;
	using iterator = const_iterator;
	using equal_range_result_t = ::fast_io::containers::details::equal_range_result_t<const_iterator>;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
	using reverse_iterator = const_reverse_iterator;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;

	::fast_io::containers::details::btree_imp imp{};

	constexpr basic_str_btree_map() noexcept = default;

	constexpr basic_str_btree_map(basic_str_btree_map const &other) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::fast_io::containers::details::str_btree_map_mapped_nothrow_copy_move<mapped_type>)
	{
		::fast_io::containers::details::str_btree_map_insert_range_common<allocator_type, keys_number, node_type>(
			this->imp, other.cbegin(), other.cend());
	}
	constexpr basic_str_btree_map &operator=(basic_str_btree_map const &other) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::fast_io::containers::details::str_btree_map_mapped_nothrow_copy_move<mapped_type>)
	{
		if (this != ::std::addressof(other))
		{
			this->clear();
			::fast_io::containers::details::str_btree_map_insert_range_common<allocator_type, keys_number, node_type>(
				this->imp, other.cbegin(), other.cend());
		}
		return *this;
	}

	explicit constexpr basic_str_btree_map(::std::initializer_list<initializer_list_pair_type> ilist) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::fast_io::containers::details::str_btree_map_mapped_nothrow_copy_move<mapped_type>)
	{
		::fast_io::containers::details::str_btree_map_insert_range_common<allocator_type, keys_number, node_type>(
			this->imp, ilist.begin(), ilist.end());
	}

	template <::std::ranges::range R>
		requires(::fast_io::containers::details::str_btree_map_range_has_key_mapped_val<chtype, mappedtype, R>)
	explicit constexpr basic_str_btree_map(::fast_io::freestanding::from_range_t, R &&rg) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::fast_io::containers::details::str_btree_map_range_has_key_mapped_noexcept<chtype, mappedtype, R> || !::fast_io::containers::details::str_btree_map_mapped_nothrow_copy_move<mapped_type>)
	{
		::fast_io::containers::details::str_btree_map_insert_range_common<allocator_type, keys_number, node_type>(
			this->imp, ::std::ranges::begin(rg), ::std::ranges::end(rg));
	}

	constexpr bool contains(string_view_type key) const noexcept
	{
		return ::fast_io::containers::details::str_btree_contains(static_cast<node_type *>(this->imp.root), key.ptr, key.n);
	}
	constexpr size_type count(string_view_type key) const noexcept
	{
		return static_cast<size_type>(this->contains(key));
	}
	constexpr const_iterator find(string_view_type key) const noexcept
	{
		auto [ptr, pos] = ::fast_io::containers::details::str_btree_find(static_cast<node_type *>(this->imp.root), key.ptr, key.n);
		return {ptr, pos, this->imp.rightmost};
	}
	constexpr const_iterator find_key(string_view_type key) const noexcept
	{
		return this->find(key);
	}
	constexpr const_iterator lower_bound(string_view_type key) const noexcept
	{
		auto [ptr, pos] = ::fast_io::containers::details::str_btree_lower_bound(static_cast<node_type *>(this->imp.root), key.ptr, key.n);
		return {ptr, pos, this->imp.rightmost};
	}
	constexpr const_iterator upper_bound(string_view_type key) const noexcept
	{
		auto [ptr, pos] = ::fast_io::containers::details::str_btree_find(static_cast<node_type *>(this->imp.root), key.ptr, key.n);
		if (ptr != nullptr)
		{
			const_iterator it{ptr, pos, this->imp.rightmost};
			++it;
			return it;
		}
		return this->lower_bound(key);
	}
	constexpr equal_range_result_t equal_range(string_view_type key) const noexcept
	{
		return {this->lower_bound(key), this->upper_bound(key)};
	}
	constexpr bool is_empty() const noexcept
	{
		return this->imp.root == nullptr;
	}
	constexpr size_type erase_key(string_view_type key) noexcept
	{
		return ::fast_io::containers::details::str_btree_map_erase<allocator_type, keys_number, node_type>(this->imp, key.ptr, key.n);
	}
	constexpr bool insert_key(string_view_type key, mapped_type const &mapval) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_copy_constructible_v<mapped_type>)
	{
		return ::fast_io::containers::details::str_btree_map_insert_key_with_root<allocator_type, keys_number, node_type>(
			this->imp, key.ptr, key.n, mapval);
	}
	constexpr bool insert_key(string_view_type key, mapped_type &&mapval) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		return ::fast_io::containers::details::str_btree_map_insert_key_with_root<allocator_type, keys_number, node_type>(
			this->imp, key.ptr, key.n, ::std::move(mapval));
	}
	constexpr bool insert_key_or_assign(string_view_type key, mapped_type const &mapval) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_copy_constructible_v<mapped_type>)
	{
		return ::fast_io::containers::details::str_btree_map_insert_key_or_assign_with_root<allocator_type, keys_number, node_type>(
			this->imp, key.ptr, key.n, mapval);
	}
	constexpr bool insert_key_or_assign(string_view_type key, mapped_type &&mapval) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		return ::fast_io::containers::details::str_btree_map_insert_key_or_assign_with_root<allocator_type, keys_number, node_type>(
			this->imp, key.ptr, key.n, ::std::move(mapval));
	}
	template <typename... Args>
		requires ::std::constructible_from<mapped_type, Args...>
	constexpr bool emplace_key(string_view_type key, Args &&...args) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_constructible_v<mapped_type, Args &&...>)
	{
		return ::fast_io::containers::details::str_btree_map_insert_key_with_root<allocator_type, keys_number, node_type>(
			this->imp, key.ptr, key.n, mapped_type(::std::forward<Args>(args)...));
	}
	template <typename... Args>
		requires ::std::constructible_from<mapped_type, Args...>
	constexpr bool emplace_key_or_assign(string_view_type key, Args &&...args) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_constructible_v<mapped_type, Args &&...>)
	{
		return ::fast_io::containers::details::str_btree_map_insert_key_or_assign_with_root<allocator_type, keys_number, node_type>(
			this->imp, key.ptr, key.n, mapped_type(::std::forward<Args>(args)...));
	}
	constexpr bool insert_key_hint(const_iterator hint, string_view_type key, mapped_type const &mapval) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_copy_constructible_v<mapped_type>)
	{
		return ::fast_io::containers::details::str_btree_map_insert_key_hint<allocator_type, keys_number, node_type>(
			this->imp, hint.node.ptr, hint.node.pos, key.ptr, key.n, mapval);
	}
	constexpr bool insert_key_hint(const_iterator hint, string_view_type key, mapped_type &&mapval) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		return ::fast_io::containers::details::str_btree_map_insert_key_hint<allocator_type, keys_number, node_type>(
			this->imp, hint.node.ptr, hint.node.pos, key.ptr, key.n, ::std::move(mapval));
	}
	template <::std::ranges::range R>
		requires(::fast_io::containers::details::str_btree_map_range_has_key_mapped_val<chtype, mappedtype, R>)
	constexpr void insert_range(R &&rg) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::fast_io::containers::details::str_btree_map_range_has_key_mapped_noexcept<chtype, mappedtype, R> || !::fast_io::containers::details::str_btree_map_mapped_nothrow_copy_move<mapped_type>)
	{
		::fast_io::containers::details::str_btree_map_insert_range_common<allocator_type, keys_number, node_type>(
			this->imp, ::std::ranges::begin(rg), ::std::ranges::end(rg));
	}
	constexpr void insert_range(::std::initializer_list<initializer_list_pair_type> ilist) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::fast_io::containers::details::str_btree_map_mapped_nothrow_copy_move<mapped_type>)
	{
		::fast_io::containers::details::str_btree_map_insert_range_common<allocator_type, keys_number, node_type>(
			this->imp, ilist.begin(), ilist.end());
	}
	constexpr iterator erase(const_iterator iter) noexcept
	{
		auto succ{iter};
		++succ;
		if (succ == this->cend())
		{
			this->erase_key(iter->key());
			return this->end();
		}
		cstring_view_type const nextkey{succ->key()};
		this->erase_key(iter->key());
		return this->find_key(nextkey);
	}
	constexpr iterator erase(const_iterator first, const_iterator last) noexcept
	{
		if (first == last)
		{
			return first;
		}
		if (last == this->cend())
		{
			while (first != this->cend())
			{
				first = this->erase(first);
			}
			return this->end();
		}
		cstring_view_type const lastkey{last->key()};
		for (;;)
		{
			if (first->key() == lastkey)
			{
				return first;
			}
			first = this->erase(first);
		}
	}
	constexpr void swap(basic_str_btree_map &other) noexcept
	{
		auto const tmp{this->imp};
		this->imp = other.imp;
		other.imp = tmp;
	}

private:
	static inline constexpr void clear_node(void *nodev) noexcept
	{
		auto node{static_cast<node_type *>(nodev)};
		if (node == nullptr)
		{
			return;
		}
		::std::size_t const n{node->size};
		for (::std::size_t i{}; i != n; ++i)
		{
			if (!node->leaf)
			{
				clear_node(node->childrens[i]);
			}
			auto ki{node->keys[i]};
			::fast_io::typed_generic_allocator_adapter<untyped_allocator_type, char_type>::deallocate_n(const_cast<char_type *>(ki.ptr),
																										static_cast<::std::size_t>(ki.n + 1u));
			::std::destroy_at(node->values + i);
		}
		if (!node->leaf)
		{
			clear_node(node->childrens[n]);
		}
		// **Deallocate the node using the allocator**
		typed_allocator_type::deallocate_n(node, 1);
	}

public:
	constexpr void clear() noexcept
	{
		clear_node(this->imp.root);
		this->imp = {nullptr, nullptr, nullptr};
	}

	constexpr void clear_destroy() noexcept
	{
		this->clear();
	}

	constexpr basic_str_btree_map(basic_str_btree_map &&other) noexcept : imp(other.imp)
	{
		other.imp = {};
	}
	constexpr basic_str_btree_map &operator=(basic_str_btree_map &&other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		this->clear();
		this->imp = other.imp;
		other.imp = {};
		return *this;
	}
	constexpr const_iterator cbegin() const noexcept
	{
		return {this->imp.leftmost, 0, this->imp.rightmost};
	}

	constexpr const_iterator cend() const noexcept
	{
		return {nullptr, 0, this->imp.rightmost};
	}
	constexpr const_iterator begin() const noexcept
	{
		return this->cbegin();
	}
	constexpr const_iterator end() const noexcept
	{
		return this->cend();
	}

	constexpr const_reverse_iterator crbegin() const noexcept
	{
		return reverse_iterator(cend());
	}

	static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type mx{::std::numeric_limits<size_type>::max() / sizeof(node_type)};
		return mx;
	}

	static inline constexpr size_type max_size_bytes() noexcept
	{
		constexpr size_type mx{::std::numeric_limits<size_type>::max() / sizeof(node_type) * sizeof(node_type)};
		return mx;
	}

	constexpr const_reverse_iterator crend() const noexcept
	{
		return reverse_iterator(cbegin());
	}
	constexpr const_reverse_iterator rbegin() const noexcept
	{
		return this->crbegin();
	}
	constexpr const_reverse_iterator rend() const noexcept
	{
		return this->crend();
	}
	constexpr typename const_iterator::value_type front() const
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_on_violations<allocator_type>)
	{
		if (this->imp.leftmost == nullptr) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<::fast_io::containers::details::allocator_throws_on_violations<allocator_type>>(::std::errc::invalid_argument);
		}
		return this->front_unchecked();
	}
	constexpr typename const_iterator::value_type back() const
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_on_violations<allocator_type>)
	{
		if (this->imp.rightmost == nullptr) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<::fast_io::containers::details::allocator_throws_on_violations<allocator_type>>(::std::errc::invalid_argument);
		}
		return this->back_unchecked();
	}
	constexpr typename const_iterator::value_type front_unchecked() const noexcept
	{
		return {this->imp.leftmost, 0};
	}
	constexpr typename const_iterator::value_type back_unchecked() const noexcept
	{
		auto &e{*static_cast<node_type const *>(this->imp.rightmost)};
		return {this->imp.rightmost, ::std::size_t{e.size - 1u}};
	}
	constexpr mapped_type &operator[](string_view_type key) FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_default_constructible_v<mapped_type>)
	{
		auto it{this->lower_bound(key)};
		if (it == this->cend() || key < it->key())
		{
			::fast_io::containers::details::str_btree_map_insert_key_hint<allocator_type, keys_number, node_type>(
				this->imp, it.node.ptr, it.node.pos, key.ptr, key.n, mapped_type{});
			return this->find(key)->mapped();
		}
		return it->mapped();
	}
	constexpr ~basic_str_btree_map() noexcept
	{
		clear_node(this->imp.root);
	}
};

template <::std::integral chtype, typename mappedtype, typename Allocator, ::std::size_t keys_number>
constexpr void swap(::fast_io::containers::basic_str_btree_map<chtype, mappedtype, Allocator, keys_number> &a,
					::fast_io::containers::basic_str_btree_map<chtype, mappedtype, Allocator, keys_number> &b) noexcept
{
	a.swap(b);
}

template <::std::integral chtype, typename mappedtype, typename Allocator, ::std::size_t keys_number>
constexpr bool operator==(::fast_io::containers::basic_str_btree_map<chtype, mappedtype, Allocator, keys_number> const &lhs,
						  ::fast_io::containers::basic_str_btree_map<chtype, mappedtype, Allocator, keys_number> const &rhs) noexcept
{
	return ::std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

#if __cpp_impl_three_way_comparison >= 201907L
template <::std::integral chtype, typename mappedtype, typename Allocator, ::std::size_t keys_number>
constexpr auto operator<=>(::fast_io::containers::basic_str_btree_map<chtype, mappedtype, Allocator, keys_number> const &lhs,
						   ::fast_io::containers::basic_str_btree_map<chtype, mappedtype, Allocator, keys_number> const &rhs) noexcept
{
	return ::std::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}
#endif

} // namespace fast_io::containers
