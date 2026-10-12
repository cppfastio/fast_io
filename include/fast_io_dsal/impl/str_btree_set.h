#pragma once

namespace fast_io::containers
{

namespace details
{
struct btree_imp
{
	void *root;
	void *leftmost;
	void *rightmost;
};

template <::std::integral chtype, ::std::size_t keys_number>
struct str_btree_set_node
{
	using char_type = chtype;
	::std::size_t size;                                                      // Number of keys currently stored
	bool leaf;                                                               // Indicates whether the node is a leaf
	::fast_io::details::associative_string<char_type> keys[keys_number];     // Array of keys
	str_btree_set_node<char_type, keys_number> *childrens[keys_number + 1u]; // Array of child pointers
	str_btree_set_node<char_type, keys_number> *parent;                      // Pointer to parent node
	::std::size_t parent_pos;                                                // Position in Parent
};

template <::std::size_t keys_number>
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	str_btree_set_common
{
	::std::size_t size;
	bool leaf;
	::fast_io::io_scatter_t keys[keys_number];
	void *childrens[keys_number + 1u];
	void *parent;
	::std::size_t parent_pos;
};

struct find_btree_node_insert_position_result
{
	::std::size_t pos{};
	bool found{};
};

template <typename nodetype>
inline constexpr ::fast_io::containers::details::find_btree_node_insert_position_result find_str_btree_node_insert_position(nodetype *node,
																															typename nodetype::char_type const *keystrptr, ::std::size_t keystrn) noexcept
{
	using char_type = typename nodetype::char_type;
	auto *b{node->keys}, *i{b}, *e{b + node->size};

	::fast_io::containers::basic_string_view<char_type> newkey(keystrptr, keystrn);

	bool found{};
	for (; i != e; ++i)
	{
		auto cmpres{newkey <=> i->strvw()};
		if (cmpres <= 0)
		{
			found = (cmpres == 0);
			break;
		}
	}
	return {static_cast<::std::size_t>(i - b), found};
}


struct find_btree_insert_result
{
	void *node{};
	::std::size_t pos{};
};

template <typename iteratortype>
struct equal_range_result_t
{
	iteratortype first;
	iteratortype second;
};

template <typename nodetype>
inline constexpr ::fast_io::containers::details::find_btree_insert_result str_btree_find(nodetype *node, typename nodetype::char_type const *keystrptr, ::std::size_t keystrn) noexcept
{
	if (node == nullptr)
	{
		return {};
	}
	// **Find the correct position for insertion**
	for (;;)
	{
		auto [postemp, found] = ::fast_io::containers::details::find_str_btree_node_insert_position(node, keystrptr, keystrn);
		// **If the key already exists, return false (no duplicate keys)**
		if (found)
		{
			return {node, postemp};
		}
		// **If the node is a leaf**
		if (node->leaf)
		{
			break;
		}
		node = node->childrens[postemp];
	}
	return {};
}

template <typename nodetype>
inline constexpr bool str_btree_contains(nodetype *node, typename nodetype::char_type const *keystrptr, ::std::size_t keystrn) noexcept
{
	if (node == nullptr)
	{
		return false;
	}
	// **Find the correct position for insertion**
	for (;;)
	{
		auto [postemp, found] = ::fast_io::containers::details::find_str_btree_node_insert_position(node, keystrptr, keystrn);
		// **If the key already exists, return false (no duplicate keys)**
		if (found)
		{
			return true;
		}
		// **If the node is a leaf**
		if (node->leaf)
		{
			break;
		}
		node = node->childrens[postemp];
	}
	return false;
}

template <typename nodetype>
inline constexpr ::fast_io::containers::details::find_btree_insert_result str_btree_lower_bound(nodetype *node, typename nodetype::char_type const *keystrptr, ::std::size_t keystrn) noexcept
{
	::fast_io::containers::details::find_btree_insert_result res{};
	for (; node != nullptr;)
	{
		auto [pos, found] = ::fast_io::containers::details::find_str_btree_node_insert_position(node, keystrptr, keystrn);
		if (found)
		{
			return {node, pos};
		}
		if (pos < node->size)
		{
			res = {node, pos};
		}
		if (node->leaf)
		{
			break;
		}
		node = node->childrens[pos];
	}
	return res;
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr bool str_btree_insert_key_cold(nodetype *node, ::std::size_t pos,
												typename nodetype::char_type const *tempkeystrptr, ::std::size_t tempkeystrn,
												::fast_io::containers::details::btree_imp &imp)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
{
	using char_type = typename nodetype::char_type;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;


	auto keys{node->keys};
	auto keysit{keys + pos};
	auto keysed{keys + keys_number};

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
	//	auto rightchildchildrens{rightchild->childrens};
	char_type const *movekeystrptr{};
	::std::size_t movekeystrn{};
	auto midptr{keys + keys_number_half};
	auto poskeys_number_halfcmp{pos <=> keys_number_half};

	if (poskeys_number_halfcmp < 0)
	{
		auto &keystrptrkeysnumber{midptr[-1]};
		movekeystrptr = keystrptrkeysnumber.ptr;
		movekeystrn = keystrptrkeysnumber.n;

		::fast_io::details::non_overlapped_copy_n(midptr, keys_number_half, rightchildkeys);
		::fast_io::freestanding::overlapped_copy(keys + pos, keys + keys_number_half, keys + pos + 1);

		keys[pos] = {tempkeystrptr, tempkeystrn};
	}
	else if (poskeys_number_halfcmp == 0)
	{
		::fast_io::details::non_overlapped_copy_n(midptr, keys_number_half, rightchildkeys);
		movekeystrptr = tempkeystrptr;
		movekeystrn = tempkeystrn;
	}
	else
	{
		auto &keystrptrkeysnumber{*midptr};
		movekeystrptr = keystrptrkeysnumber.ptr;
		movekeystrn = keystrptrkeysnumber.n;
		auto it{::fast_io::details::non_overlapped_copy(midptr + 1, keysit, rightchildkeys)};
		*it = {tempkeystrptr, tempkeystrn};
		++it;
		::fast_io::details::non_overlapped_copy(keysit, keysed, it);
	}
	::std::size_t child_pos{node->parent_pos};
	for (auto j{node->parent}; j; j = j->parent)
	{
		auto jkeys{j->keys};
		auto jchildrens{j->childrens};
		auto jn{j->size};

		// If parent node has space, insert the promoted key and return
		if (jn != keys_number)
		{
			// Shift keys and children to make room for the promoted key and new child
			::fast_io::freestanding::overlapped_copy(jkeys + child_pos, jkeys + jn, jkeys + child_pos + 1);
			auto &jkeyschildpos{jkeys[child_pos]};
			jkeyschildpos.ptr = movekeystrptr;
			jkeyschildpos.n = movekeystrn;
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

		auto new_right_keys{new_right->keys};
		auto new_right_childrens{new_right->childrens};
		auto child_poskeys_number_halfcmp{child_pos <=> keys_number_half};
		// CASE 1: promoted child is in the left half (before the mid key)
		if (child_poskeys_number_halfcmp < 0)
		{

			auto &jmidkey = jmidptr[-1];
			auto tmpptr{jmidkey.ptr};
			auto tmpn{jmidkey.n};

			::fast_io::details::non_overlapped_copy_n(jmidptr, keys_number_half, new_right_keys);
			::fast_io::freestanding::overlapped_copy(jkeys + child_pos, jmidptr, jkeys + child_pos + 1);
			auto &jkeyschildpos{jkeys[child_pos]};
			jkeyschildpos.ptr = movekeystrptr;
			jkeyschildpos.n = movekeystrn;
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
			*new_right_childrens = rightchild;
			::fast_io::details::non_overlapped_copy_n(jchildrens + keys_number_half_p1, keys_number_half, new_right_childrens + 1);
		}
		else
		{
			auto jkeysit{jkeys + child_pos};
			auto jkeysed{jkeys + keys_number};
			auto it{::fast_io::details::non_overlapped_copy(jmidptr + 1, jkeysit, new_right_keys)};
			it->ptr = movekeystrptr;
			it->n = movekeystrn;
			++it;
			::fast_io::details::non_overlapped_copy(jkeysit, jkeysed, it);

			auto &jkeystrptrkeysnumber{*jmidptr};
			movekeystrptr = jkeystrptrkeysnumber.ptr;
			movekeystrn = jkeystrptrkeysnumber.n;

			auto jchildrensit{jchildrens + child_pos + 1};
			auto jchildrensed{jchildrens + keys_number + 1};
			auto kit{::fast_io::details::non_overlapped_copy(jchildrens + keys_number_half_p1, jchildrensit, new_right_childrens)};
			*kit = rightchild;
			++kit;
			::fast_io::details::non_overlapped_copy(jchildrensit, jchildrensed, kit);
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

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr bool str_btree_insert_key_with_root(::fast_io::containers::details::btree_imp &imp,
													 typename nodetype::char_type const *keystrptr, ::std::size_t keystrn)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
{
	using char_type = typename nodetype::char_type;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;

	auto node{static_cast<::fast_io::containers::details::str_btree_set_node<char_type, keys_number> *>(imp.root)};

	// **If the tree is empty, allocate a new root**
	if (node == nullptr)
	{
		node = typed_allocator_type::allocate(1);
		node->size = 1;
		node->leaf = true;
		node->parent = nullptr;
		node->parent_pos = 0;
		*(node->keys) = ::fast_io::details::create_associative_string<allocator_type, char_type>(keystrptr, keystrn);
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
		auto keysit{keys + pos};
		::fast_io::freestanding::overlapped_copy(keysit, keys + n, keysit + 1);
		*keysit = tempkey;
		++node->size;
		return true;
	}
	return ::fast_io::containers::details::str_btree_insert_key_cold<allocator_type, keys_number>(node, pos, tempkey.ptr, tempkey.n, imp);
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr bool str_btree_insert_key_leaf_at(nodetype *node, ::std::size_t pos,
												   typename nodetype::char_type const *keystrptr, ::std::size_t keystrn,
												   ::fast_io::containers::details::btree_imp &imp)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
{
	using char_type = typename nodetype::char_type;
	auto tempkey{::fast_io::details::create_associative_string<allocator_type, char_type>(keystrptr, keystrn)};
	auto n{node->size};
	if (n != keys_number)
	{
		auto keys{node->keys};
		auto keysit{keys + pos};
		::fast_io::freestanding::overlapped_copy(keysit, keys + n, keysit + 1);
		*keysit = tempkey;
		++node->size;
		return true;
	}
	return ::fast_io::containers::details::str_btree_insert_key_cold<allocator_type, keys_number>(node, pos, tempkey.ptr, tempkey.n, imp);
}

template <::std::size_t keys_number>
inline constexpr void fix_child_links(::fast_io::containers::details::str_btree_set_common<keys_number> *parent_node) noexcept
{
	using nodeptr = decltype(parent_node);
	if (parent_node == nullptr || parent_node->leaf)
	{
		return;
	}
	for (::std::size_t i{}; i <= parent_node->size; ++i)
	{
		auto child{static_cast<nodeptr>(parent_node->childrens[i])};
		child->parent = parent_node;
		child->parent_pos = i;
	}
}

template <typename allocator_type, ::std::size_t keys_number>
inline constexpr void str_btree_erase_underflow(::fast_io::containers::details::btree_imp &imp,
												::fast_io::containers::details::str_btree_set_common<keys_number> *node) noexcept
{
	using nodeptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= ::fast_io::containers::details::str_btree_set_common<keys_number> *;
	using typed_allocator_type =
		::fast_io::typed_generic_allocator_adapter<allocator_type,
												   ::fast_io::containers::details::str_btree_set_common<keys_number>>;

	constexpr ::std::size_t min_keys{(keys_number >> 1u) - 1u};

	auto current = node;
	while (current && current->size < min_keys)
	{
		auto parent = static_cast<nodeptr>(current->parent);

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

				// Shift right sibling's keys left
				::fast_io::freestanding::overlapped_copy(
					right->keys + 1, right->keys + right->size, right->keys);
				--right->size;
				::fast_io::containers::details::fix_child_links<keys_number>(current);
				::fast_io::containers::details::fix_child_links<keys_number>(right);
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
				++current->size;

				// Update parent key to left sibling's rightmost
				parent->keys[parpos - 1] = left->keys[left_size_before - 1];
				--left->size;
				::fast_io::containers::details::fix_child_links<keys_number>(current);
				::fast_io::containers::details::fix_child_links<keys_number>(left);
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

			// Append right sibling's keys
			::fast_io::freestanding::non_overlapped_copy(
				right->keys, right->keys + right_size_before, current->keys + current_size_before + 1);
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
			typed_allocator_type::deallocate_n(right, 1);

			// Shift parent's keys and children
			::fast_io::freestanding::overlapped_copy(
				parent->keys + parpos + 1, parent->keys + parent_size, parent->keys + parpos);
			::fast_io::freestanding::overlapped_copy(
				parent->childrens + parpos + 2, parent->childrens + parent_size + 1,
				parent->childrens + parpos + 1);
			--parent->size;
			::fast_io::containers::details::fix_child_links<keys_number>(parent);

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

			// Append current's keys
			::fast_io::freestanding::non_overlapped_copy(
				current->keys, current->keys + current_size_before, left->keys + left_size_before + 1);
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
			typed_allocator_type::deallocate_n(current, 1);

			// Shift parent's keys and children
			::fast_io::freestanding::overlapped_copy(
				parent->keys + parpos, parent->keys + parent_size, parent->keys + parpos - 1);
			::fast_io::freestanding::overlapped_copy(
				parent->childrens + parpos + 1, parent->childrens + parent_size + 1,
				parent->childrens + parpos);
			--parent->size;
			::fast_io::containers::details::fix_child_links<keys_number>(parent);

			current = parent; // move up
		}
	}
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr bool str_btree_erase(::fast_io::containers::details::btree_imp &imp,
									  typename nodetype::char_type const *keystrptr,
									  ::std::size_t keystrn) noexcept
{
	using char_type = typename nodetype::char_type;
	using nodeptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= ::fast_io::containers::details::str_btree_set_common<keys_number> *;
	using typed_allocator_type =
		::fast_io::typed_generic_allocator_adapter<allocator_type,
												   ::fast_io::containers::details::str_btree_set_common<keys_number>>;

	constexpr std::size_t min_keys{(keys_number >> 1u) - 1u};

	auto root_node{static_cast<::fast_io::containers::details::str_btree_set_node<char_type, keys_number> *>(imp.root)};
	auto [found_ptr, found_pos]{::fast_io::containers::details::str_btree_find(root_node, keystrptr, keystrn)};
	if (found_ptr == nullptr)
	{
		return false;
	}

	nodeptr target_node{static_cast<nodeptr>(found_ptr)};
	auto target_keys{target_node->keys};
	auto target_slot{target_keys + found_pos};
	nodeptr target_parent{static_cast<nodeptr>(target_node->parent)};

	// Case 1: deletion from a leaf node
	if (target_node->leaf)
	{
		// Free payload memory owned by the leaf
		::fast_io::details::deallocate_associative_string<allocator_type, char_type>(target_slot->base, target_slot->len);

		// Physically remove the key by shifting left
		::fast_io::freestanding::overlapped_copy(target_slot + 1, target_keys + target_node->size, target_slot);
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
			nodeptr right_sibling{static_cast<nodeptr>(target_parent->childrens[parent_pos + 1])};
			if (right_sibling->size > min_keys)
			{
				target_keys[target_node->size] = target_parent->keys[parent_pos];
				++target_node->size;
				target_parent->keys[parent_pos] = right_sibling->keys[0];
				::fast_io::freestanding::overlapped_copy(
					right_sibling->keys + 1, right_sibling->keys + right_sibling->size, right_sibling->keys);
				--right_sibling->size;
				return true;
			}
		}

		// Borrow from left sibling
		if (parent_pos != 0)
		{
			nodeptr left_sibling{static_cast<nodeptr>(target_parent->childrens[parent_pos - 1])};
			if (left_sibling->size > min_keys)
			{
				::fast_io::freestanding::overlapped_copy(target_keys, target_keys + target_node->size, target_keys + 1);
				target_keys[0] = target_parent->keys[parent_pos - 1];
				++target_node->size;
				target_parent->keys[parent_pos - 1] = left_sibling->keys[left_sibling->size - 1];
				--left_sibling->size;
				return true;
			}
		}

		// Merge with sibling (prefer right if possible)
		bool merge_with_right{parent_pos != parent_size};
		nodeptr merged_target{};
		nodeptr merged_victim{};
		std::size_t parent_key_index{};
		if (merge_with_right)
		{
			merged_target = target_node;
			merged_victim = static_cast<nodeptr>(target_parent->childrens[parent_pos + 1]);
			parent_key_index = parent_pos;
			merged_target->keys[merged_target->size] = target_parent->keys[parent_key_index];
			::fast_io::freestanding::non_overlapped_copy(
				merged_victim->keys, merged_victim->keys + merged_victim->size,
				merged_target->keys + merged_target->size + 1);
			merged_target->size += merged_victim->size + 1;
		}
		else
		{
			merged_target = static_cast<nodeptr>(target_parent->childrens[parent_pos - 1]);
			merged_victim = target_node;
			parent_key_index = parent_pos - 1;
			merged_target->keys[merged_target->size] = target_parent->keys[parent_key_index];
			::fast_io::freestanding::non_overlapped_copy(
				target_keys, target_keys + target_node->size,
				merged_target->keys + merged_target->size + 1);
			merged_target->size += target_node->size + 1;
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
		::fast_io::freestanding::overlapped_copy(
			target_parent->childrens + parent_key_index + 2, target_parent->childrens + parent_size + 1,
			target_parent->childrens + parent_key_index + 1);
		--target_parent->size;
		for (std::size_t i{parent_key_index + 1}; i <= target_parent->size; ++i)
		{
			auto ch{static_cast<nodeptr>(target_parent->childrens[i])};
			if (ch)
			{
				ch->parent_pos = i;
			}
		}

		str_btree_erase_underflow<allocator_type, keys_number>(imp, target_parent);
		return true;
	}

	// Case 2: deletion from an internal node
	nodeptr internal_node{target_node};
	std::size_t internal_key_index{found_pos};
	nodeptr left_child{static_cast<nodeptr>(internal_node->childrens[internal_key_index])};
	nodeptr right_child{static_cast<nodeptr>(internal_node->childrens[internal_key_index + 1])};

	// Predecessor: replace internal key with rightmost key from left subtree
	if (min_keys < left_child->size)
	{
		nodeptr descend{left_child};
		while (!descend->leaf)
		{
			descend = static_cast<nodeptr>(descend->childrens[descend->size]); // rightmost child
		}

		// Free payload of the original internal key being erased
		::fast_io::details::deallocate_associative_string<allocator_type, char_type>(
			internal_node->keys[internal_key_index].base,
			internal_node->keys[internal_key_index].len);

		// Move predecessor into internal slot (ownership transfer)
		auto &pred_key{descend->keys[descend->size - 1]};
		internal_node->keys[internal_key_index] = pred_key;

		// Remove predecessor from leaf WITHOUT deallocating (ownership moved)
		::fast_io::freestanding::overlapped_copy(
			descend->keys + descend->size, descend->keys + descend->size, descend->keys + (descend->size - 1));
		--descend->size;

		// Repair upward if leaf underflows
		if (descend->parent && descend->size < min_keys)
		{
			str_btree_erase_underflow<allocator_type, keys_number>(imp, static_cast<nodeptr>(descend->parent));
		}
		return true;
	}


	// Successor
	if (right_child->size > min_keys)
	{
		nodeptr descend{right_child};
		while (!descend->leaf)
		{
			descend = static_cast<nodeptr>(*descend->childrens);
		}

		// Free payload of the original internal key being erased (fixes leak)
		::fast_io::details::deallocate_associative_string<allocator_type, char_type>(
			internal_node->keys[internal_key_index].base,
			internal_node->keys[internal_key_index].len);

		auto &succ_key{*descend->keys};
		internal_node->keys[internal_key_index] = succ_key;
		::fast_io::freestanding::overlapped_copy(descend->keys + 1, descend->keys + descend->size, descend->keys);
		--descend->size;
		if (descend->parent && descend->size < min_keys)
		{
			str_btree_erase_underflow<allocator_type, keys_number>(imp, static_cast<nodeptr>(descend->parent));
		}
		return true;
	}

	// Merge left child + internal key + right child into left child, then delete the key in the merged subtree.
	std::size_t left_size_before{left_child->size};
	std::size_t right_size_before{right_child->size};
	left_child->keys[left_size_before] = internal_node->keys[internal_key_index];
	::fast_io::freestanding::non_overlapped_copy(
		right_child->keys, right_child->keys + right_size_before,
		left_child->keys + left_size_before + 1);
	if (!left_child->leaf)
	{
		::fast_io::freestanding::non_overlapped_copy(
			right_child->childrens, right_child->childrens + right_size_before + 1,
			left_child->childrens + left_size_before + 1);
		for (std::size_t i{}; i <= right_size_before; ++i)
		{
			auto ch{static_cast<nodeptr>(right_child->childrens[i])};
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
	typed_allocator_type::deallocate_n(right_child, 1);

	std::size_t internal_size_before{internal_node->size};
	::fast_io::freestanding::overlapped_copy(
		internal_node->keys + internal_key_index + 1,
		internal_node->keys + internal_size_before,
		internal_node->keys + internal_key_index);
	::fast_io::freestanding::overlapped_copy(
		internal_node->childrens + internal_key_index + 2,
		internal_node->childrens + internal_size_before + 1,
		internal_node->childrens + internal_key_index + 1);
	--internal_node->size;
	for (std::size_t i{internal_key_index + 1}; i <= internal_node->size; ++i)
	{
		auto ch{static_cast<nodeptr>(internal_node->childrens[i])};
		if (ch)
		{
			ch->parent_pos = i;
		}
	}

	// Repair potential underflow upward from the internal node
	str_btree_erase_underflow<allocator_type, keys_number>(imp, internal_node);
	return str_btree_erase<allocator_type, keys_number, nodetype>(imp, keystrptr, keystrn);
}

/* Common structure used for str_btree_set iterator.
Holds the current node pointer, position within node, and optional 'last' for reversed iteration fallback. */
struct str_btree_set_iterator_common
{
	void const *ptr{};  // Pointer to current node
	std::size_t pos{};  // Position within current node
	void const *last{}; // Pointer to last node (used for -- from end)
};

/* Advances the iterator to the next key in str_btree_set.
Traverses leaf nodes linearly, and climbs up the parent chain when leaf ends.
For internal nodes, descends leftmost into the right child. */
template <::std::size_t keys_number>
inline constexpr void str_btree_set_next_node(str_btree_set_iterator_common &c) noexcept
{
	auto ptrv{static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(c.ptr)};

	if (ptrv->leaf)
	{
		// Move to next key inside leaf
		if (++c.pos == ptrv->size)
		{
			// Reached end of leaf, climb parent chain to find next sibling
			auto parent{ptrv->parent};
			auto parent_pos{static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(ptrv)->parent_pos};
			while (parent)
			{
				auto parentn{static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(parent)->size};
				if (parent_pos != parentn)
				{
					break; // Found next available key in parent
				}
				parent_pos = static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(parent)->parent_pos;
				parent = static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(parent)->parent;
			}
			c.ptr = parent;
			c.pos = parent_pos;
		}
	}
	else
	{
		// Internal node: descend into leftmost key of right child
		auto nextptr{ptrv->childrens[c.pos + 1]};
		while (!static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(nextptr)->leaf)
		{
			nextptr = *(static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(nextptr)->childrens);
		}
		c.ptr = nextptr;
		c.pos = 0;
	}
}

/* Moves the iterator to the previous key in str_btree_set.
From null (end), restores last node. In leaf, moves left or climbs parent.
For internal node, descends rightmost into the left child. */
template <::std::size_t keys_number>
inline constexpr void str_btree_set_prev_node(str_btree_set_iterator_common &c) noexcept
{
	if (c.ptr == nullptr)
	{
		// Iterator was at end(); fallback to last node if known
		c.ptr = c.last;
		// c.ptr must not be nullptr or it is on empty container which is undefined behavior
		c.pos = static_cast<::std::size_t>(static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(c.ptr)->size - 1u);
		return;
	}

	auto ptrv{static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(c.ptr)};

	if (ptrv->leaf)
	{
		if (!c.pos)
		{
			// Start of leaf, need to go up to parent and find left sibling
			auto parent{ptrv->parent};
			auto parent_pos{static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(ptrv)->parent_pos};
			while (parent)
			{
				if (parent_pos)
				{
					break; // Found non-zero position in parent
				}
				parent_pos = static_cast<::std::size_t>(static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(parent)->parent_pos);
				parent = static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(parent)->parent;
			}
			c.ptr = parent;
			c.pos = 0;
			if (parent)
			{
				c.pos = static_cast<::std::size_t>(parent_pos - 1u);
			}
			return;
		}
		--c.pos; // Move left inside leaf
	}
	else
	{
		// Internal node: descend into rightmost key of left child
		auto prevptr{ptrv->childrens[c.pos]};
		while (!static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(prevptr)->leaf)
		{
			auto e{static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(prevptr)};
			prevptr = e->childrens[e->size];
		}
		c.ptr = prevptr;
		c.pos = static_cast<::std::size_t>(static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(c.ptr)->size - 1u);
	}
}

template <::std::integral chtype, ::std::size_t keys_number>
class str_btree_set_iterator
{
public:
	using value_type = ::fast_io::containers::basic_cstring_view<chtype>;
	using iterator_tag = ::std::bidirectional_iterator_tag;
	using difference_type = ::std::ptrdiff_t;
	::fast_io::containers::details::str_btree_set_iterator_common node;

	constexpr str_btree_set_iterator &operator++() noexcept
	{
		::fast_io::containers::details::str_btree_set_next_node<keys_number>(this->node);
		return *this;
	}
	constexpr str_btree_set_iterator operator++(int) noexcept
	{
		auto tmp{*this};
		++*this;
		return tmp;
	}
	constexpr str_btree_set_iterator &operator--() noexcept
	{
		::fast_io::containers::details::str_btree_set_prev_node<keys_number>(this->node);
		return *this;
	}
	constexpr str_btree_set_iterator operator--(int) noexcept
	{
		auto tmp{*this};
		--*this;
		return tmp;
	}
	constexpr value_type operator*() const noexcept
	{
		return static_cast<::fast_io::containers::details::str_btree_set_node<chtype, keys_number> const *>(node.ptr)->keys[node.pos].strvw();
	}
};

template <::std::integral chtype, ::std::size_t keys_number>
inline constexpr bool operator==(::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number> const &a,
								 ::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number> const &b) noexcept
{
	return a.node.ptr == b.node.ptr && a.node.pos == b.node.pos;
}

template <::std::integral chtype, ::std::size_t keys_number>
inline constexpr bool operator!=(::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number> const &a,
								 ::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number> const &b) noexcept
{
	return !operator==(a, b);
}

} // namespace details

template <::std::integral chtype, typename Allocator, ::std::size_t keys_number = 16>
	requires((keys_number != 0) && ((keys_number & 1) == 0))
class basic_str_btree_set
{
	using node_type = ::fast_io::containers::details::str_btree_set_node<chtype, keys_number>;
	using untyped_allocator_type = Allocator;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<untyped_allocator_type, node_type>;

public:
	using char_type = chtype;
	using string_view_type = ::fast_io::containers::basic_string_view<char_type>;
	using cstring_view_type = ::fast_io::containers::basic_cstring_view<char_type>;
	using allocator_type = Allocator;
	using const_iterator = ::fast_io::containers::details::str_btree_set_iterator<char_type, keys_number>;
	using iterator = const_iterator;
	using equal_range_result_t = ::fast_io::containers::details::equal_range_result_t<const_iterator>;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
	using reverse_iterator = const_reverse_iterator;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;

	::fast_io::containers::details::btree_imp imp{};

	constexpr basic_str_btree_set() noexcept = default;

	constexpr basic_str_btree_set(basic_str_btree_set const &other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		this->insert_range(other);
	}
	constexpr basic_str_btree_set &operator=(basic_str_btree_set const &other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		if (this != ::std::addressof(other))
		{
			this->clear();
			this->insert_range(other);
		}
		return *this;
	}

	explicit constexpr basic_str_btree_set(::std::initializer_list<string_view_type> ilist)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		this->insert_range(ilist);
	}

	template <::std::ranges::range R>
	explicit constexpr basic_str_btree_set(::fast_io::freestanding::from_range_t, R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_constructible_v<string_view_type, ::std::ranges::range_value_t<R>>)
	{
		this->insert_range(::std::forward<R>(rg));
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
		return ::fast_io::containers::details::str_btree_erase<allocator_type, keys_number, node_type>(this->imp, key.ptr, key.n);
	}
	constexpr bool insert_key(string_view_type key)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		return ::fast_io::containers::details::str_btree_insert_key_with_root<allocator_type, keys_number, node_type>(this->imp, key.ptr, key.n);
	}
	constexpr bool insert_key_hint(const_iterator hint, string_view_type key)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		if (this->imp.root == nullptr)
		{
			return this->insert_key(key);
		}
		if (hint.node.ptr == nullptr)
		{
			// hint == end(): fast path for appending keys in order
			auto rm{static_cast<node_type *>(this->imp.rightmost)};
			if (rm != nullptr && rm->size != 0 && rm->keys[rm->size - 1u].strvw() < key)
			{
				return ::fast_io::containers::details::str_btree_insert_key_leaf_at<allocator_type, keys_number>(
					rm, rm->size, key.ptr, key.n, this->imp);
			}
			return this->insert_key(key);
		}
		auto node{static_cast<node_type *>(const_cast<void *>(hint.node.ptr))};
		if (!node->leaf)
		{
			return this->insert_key(key);
		}
		auto const pos{hint.node.pos};
		if (!(key < node->keys[pos].strvw()))
		{
			return this->insert_key(key);
		}
		if (pos != 0)
		{
			if (!(node->keys[pos - 1u].strvw() < key))
			{
				return this->insert_key(key);
			}
		}
		else if (node != this->imp.leftmost)
		{
			return this->insert_key(key);
		}
		return ::fast_io::containers::details::str_btree_insert_key_leaf_at<allocator_type, keys_number>(node, pos, key.ptr, key.n, this->imp);
	}
	template <::std::ranges::range R>
	constexpr void insert_range(R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_constructible_v<string_view_type, ::std::ranges::range_value_t<R>>)
	{
		for (auto const &e : rg)
		{
			this->insert_key_hint(this->cend(), e);
		}
	}
	constexpr iterator erase(const_iterator iter) noexcept
	{
		auto succ{iter};
		++succ;
		if (succ == this->cend())
		{
			this->erase_key(*iter);
			return this->end();
		}
		cstring_view_type const nextkey{*succ};
		this->erase_key(*iter);
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
		cstring_view_type const lastkey{*last};
		for (;;)
		{
			if (*first == lastkey)
			{
				return first;
			}
			first = this->erase(first);
		}
	}
	constexpr void swap(basic_str_btree_set &other) noexcept
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

	constexpr basic_str_btree_set(basic_str_btree_set &&other) noexcept : imp(other.imp)
	{
		other.imp = {};
	}
	constexpr basic_str_btree_set &operator=(basic_str_btree_set &&other) noexcept
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
	constexpr cstring_view_type front() const
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_on_violations<allocator_type>)
	{
		if (this->imp.leftmost == nullptr) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<::fast_io::containers::details::allocator_throws_on_violations<allocator_type>>(::std::errc::invalid_argument);
		}
		return this->front_unchecked();
	}
	constexpr cstring_view_type back() const
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_on_violations<allocator_type>)
	{
		if (this->imp.rightmost == nullptr) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<::fast_io::containers::details::allocator_throws_on_violations<allocator_type>>(::std::errc::invalid_argument);
		}
		return this->back_unchecked();
	}
	constexpr cstring_view_type front_unchecked() const noexcept
	{
		return static_cast<node_type const *>(this->imp.leftmost)->keys->strvw();
	}
	constexpr cstring_view_type back_unchecked() const noexcept
	{
		auto &e{*static_cast<node_type const *>(this->imp.rightmost)};
		return e.keys[e.size - 1u].strvw();
	}
	constexpr ~basic_str_btree_set() noexcept
	{
		clear_node(this->imp.root);
	}
};

template <::std::integral chtype, typename Allocator, ::std::size_t keys_number>
constexpr void swap(::fast_io::containers::basic_str_btree_set<chtype, Allocator, keys_number> &a,
					::fast_io::containers::basic_str_btree_set<chtype, Allocator, keys_number> &b) noexcept
{
	a.swap(b);
}

template <::std::integral chtype, typename Allocator, ::std::size_t keys_number>
constexpr bool operator==(::fast_io::containers::basic_str_btree_set<chtype, Allocator, keys_number> const &lhs,
						  ::fast_io::containers::basic_str_btree_set<chtype, Allocator, keys_number> const &rhs) noexcept
{
	return ::std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

#if __cpp_impl_three_way_comparison >= 201907L
template <::std::integral chtype, typename Allocator, ::std::size_t keys_number>
constexpr auto operator<=>(::fast_io::containers::basic_str_btree_set<chtype, Allocator, keys_number> const &lhs,
						   ::fast_io::containers::basic_str_btree_set<chtype, Allocator, keys_number> const &rhs) noexcept
{
	return ::std::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}
#endif

} // namespace fast_io::containers
