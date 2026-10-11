#pragma once

namespace fast_io::containers
{

namespace details
{

/*
str_ranked_btree_set_node keeps the same member order as str_btree_set_node
and appends subtree_size at the tail: the number of elements stored in the
subtree rooted at this node (its own keys plus all descendants). The shared
prefix layout lets every key-only btree helper (find/contains/lower_bound,
iterator traversal, fix_child_links) alias ranked nodes through
str_btree_set_common.
*/
template <::std::integral chtype, ::std::size_t keys_number>
struct str_ranked_btree_set_node
{
	using char_type = chtype;
	::std::size_t size;
	bool leaf;
	::fast_io::details::associative_string<char_type> keys[keys_number];
	str_ranked_btree_set_node<char_type, keys_number> *childrens[keys_number + 1u];
	str_ranked_btree_set_node<char_type, keys_number> *parent;
	::std::size_t parent_pos;
	::std::size_t subtree_size;
};

/* subtree_size = own key count + sum of children subtree sizes. */
template <typename nodetype>
inline constexpr void str_ranked_btree_recompute_subtree_size(nodetype *node) noexcept
{
	auto s{node->size};
	if (!node->leaf)
	{
		auto const ed{node->size + 1u};
		for (::std::size_t i{}; i != ed; ++i)
		{
			s += node->childrens[i]->subtree_size;
		}
	}
	node->subtree_size = s;
}

/*
Post-order pass computing subtree_size for the whole tree. Used once after a
bulk load whose inserts deliberately skipped subtree_size maintenance.
*/
template <typename nodetype>
inline constexpr void str_ranked_btree_recompute_tree(nodetype *node) noexcept
{
	if (node == nullptr)
	{
		return;
	}
	if (!node->leaf)
	{
		auto const ed{node->size + 1u};
		for (::std::size_t i{}; i != ed; ++i)
		{
			str_ranked_btree_recompute_tree(node->childrens[i]);
		}
	}
	str_ranked_btree_recompute_subtree_size(node);
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr bool str_ranked_btree_set_insert_key_cold(nodetype *node, ::std::size_t pos,
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
	node->subtree_size = rightchild->subtree_size = keys_number_half;

	auto rightchildkeys{rightchild->keys};
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
		str_ranked_btree_recompute_subtree_size(j);
		str_ranked_btree_recompute_subtree_size(new_right);
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
	new_root->subtree_size = node->subtree_size + rightchild->subtree_size + 1u;
	imp.root = new_root;

	return true;
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr bool str_ranked_btree_set_insert_key_with_root(::fast_io::containers::details::btree_imp &imp,
																typename nodetype::char_type const *keystrptr, ::std::size_t keystrn)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
{
	using char_type = typename nodetype::char_type;
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
		node->subtree_size = 1;
		*(node->keys) = ::fast_io::details::create_associative_string<allocator_type, char_type>(keystrptr, keystrn);
		imp.rightmost = imp.leftmost = imp.root = node;
		return true;
	}

	::std::size_t pos;
	// **Find the correct position for insertion; every node on the path gains
	// one element, so bump subtree_size while descending**
	for (;;)
	{
		auto [postemp, found] = ::fast_io::containers::details::find_str_btree_node_insert_position(node, keystrptr, keystrn);
		pos = postemp;
		// **If the key already exists, roll back the subtree_size increments
		// made on the descent and return false (no duplicate keys)**
		if (found)
		{
			for (auto p{node->parent}; p; p = p->parent)
			{
				--p->subtree_size;
			}
			return false;
		}
		++node->subtree_size;
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
	return ::fast_io::containers::details::str_ranked_btree_set_insert_key_cold<allocator_type, keys_number>(node, pos, tempkey.ptr, tempkey.n, imp);
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr void str_ranked_btree_set_erase_underflow(::fast_io::containers::details::btree_imp &imp,
														   nodetype *node) noexcept
{
	using nodeptr = nodetype *;
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
					auto newroot{current->childrens[0]};
					imp.root = newroot;
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
			auto right = parent->childrens[parpos + 1];
			if (right->size > min_keys)
			{
				auto const current_size_before{current->size};
				// Move parent key down into current
				current->keys[current_size_before] = parent->keys[parpos];
				if (!current->leaf)
				{
					// Move right sibling's leftmost child into current's new last child slot
					current->childrens[current_size_before + 1] = right->childrens[0];
					auto moved_child{right->childrens[0]};
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
				::fast_io::containers::details::fix_child_links<keys_number>(
					reinterpret_cast<::fast_io::containers::details::str_btree_set_common<keys_number> *>(current));
				::fast_io::containers::details::fix_child_links<keys_number>(
					reinterpret_cast<::fast_io::containers::details::str_btree_set_common<keys_number> *>(right));
				str_ranked_btree_recompute_subtree_size(current);
				str_ranked_btree_recompute_subtree_size(right);
				return; // fixed
			}
		}

		// Try borrow from left sibling
		if (parpos != 0)
		{
			auto left = parent->childrens[parpos - 1];
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
					auto moved_child{left->childrens[left_size_before]};
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
				::fast_io::containers::details::fix_child_links<keys_number>(
					reinterpret_cast<::fast_io::containers::details::str_btree_set_common<keys_number> *>(current));
				::fast_io::containers::details::fix_child_links<keys_number>(
					reinterpret_cast<::fast_io::containers::details::str_btree_set_common<keys_number> *>(left));
				str_ranked_btree_recompute_subtree_size(current);
				str_ranked_btree_recompute_subtree_size(left);
				return; // fixed
			}
		}

		// Merge: prefer right sibling
		if (parpos != parent_size)
		{
			auto right = parent->childrens[parpos + 1];
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
					auto child{right->childrens[i]};
					if (child)
					{
						child->parent = current;
						child->parent_pos = current_size_before + 1 + i;
					}
				}
			}
			current->size += right_size_before + 1;
			current->subtree_size += right->subtree_size + 1u;

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
			::fast_io::containers::details::fix_child_links<keys_number>(
				reinterpret_cast<::fast_io::containers::details::str_btree_set_common<keys_number> *>(parent));

			current = parent; // move up
		}
		else
		{
			// Merge with left sibling
			auto left = parent->childrens[parpos - 1];
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
					auto child{current->childrens[i]};
					if (child)
					{
						child->parent = left;
						child->parent_pos = left_size_before + 1 + i;
					}
				}
			}
			left->size += current_size_before + 1;
			left->subtree_size += current->subtree_size + 1u;

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
			::fast_io::containers::details::fix_child_links<keys_number>(
				reinterpret_cast<::fast_io::containers::details::str_btree_set_common<keys_number> *>(parent));

			current = parent; // move up
		}
	}
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr bool str_ranked_btree_set_erase(::fast_io::containers::details::btree_imp &imp,
												 typename nodetype::char_type const *keystrptr,
												 ::std::size_t keystrn) noexcept
{
	using char_type = typename nodetype::char_type;
	using nodeptr = nodetype *;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;

	constexpr std::size_t min_keys{(keys_number >> 1u) - 1u};

	auto root_node{static_cast<nodetype *>(imp.root)};
	auto [found_ptr, found_pos]{::fast_io::containers::details::str_btree_find(root_node, keystrptr, keystrn)};
	if (found_ptr == nullptr)
	{
		return false;
	}

	nodeptr target_node{static_cast<nodeptr>(found_ptr)};
	auto target_keys{target_node->keys};
	auto target_slot{target_keys + found_pos};
	nodeptr target_parent{target_node->parent};

	// Case 1: deletion from a leaf node
	if (target_node->leaf)
	{
		// every ancestor of the target loses one element
		for (auto n{target_node}; n; n = n->parent)
		{
			--n->subtree_size;
		}
		// Free payload memory owned by the leaf
		::fast_io::details::deallocate_associative_string<allocator_type, char_type>(target_slot->ptr, target_slot->n);

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
			nodeptr right_sibling{target_parent->childrens[parent_pos + 1]};
			if (right_sibling->size > min_keys)
			{
				target_keys[target_node->size] = target_parent->keys[parent_pos];
				++target_node->size;
				target_parent->keys[parent_pos] = right_sibling->keys[0];
				::fast_io::freestanding::overlapped_copy(
					right_sibling->keys + 1, right_sibling->keys + right_sibling->size, right_sibling->keys);
				--right_sibling->size;
				str_ranked_btree_recompute_subtree_size(target_node);
				str_ranked_btree_recompute_subtree_size(right_sibling);
				return true;
			}
		}

		// Borrow from left sibling
		if (parent_pos != 0)
		{
			nodeptr left_sibling{target_parent->childrens[parent_pos - 1]};
			if (left_sibling->size > min_keys)
			{
				::fast_io::freestanding::overlapped_copy(target_keys, target_keys + target_node->size, target_keys + 1);
				target_keys[0] = target_parent->keys[parent_pos - 1];
				++target_node->size;
				target_parent->keys[parent_pos - 1] = left_sibling->keys[left_sibling->size - 1];
				--left_sibling->size;
				str_ranked_btree_recompute_subtree_size(target_node);
				str_ranked_btree_recompute_subtree_size(left_sibling);
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
			merged_victim = target_parent->childrens[parent_pos + 1];
			parent_key_index = parent_pos;
			merged_target->keys[merged_target->size] = target_parent->keys[parent_key_index];
			::fast_io::freestanding::non_overlapped_copy(
				merged_victim->keys, merged_victim->keys + merged_victim->size,
				merged_target->keys + merged_target->size + 1);
			merged_target->size += merged_victim->size + 1;
		}
		else
		{
			merged_target = target_parent->childrens[parent_pos - 1];
			merged_victim = target_node;
			parent_key_index = parent_pos - 1;
			merged_target->keys[merged_target->size] = target_parent->keys[parent_key_index];
			::fast_io::freestanding::non_overlapped_copy(
				target_keys, target_keys + target_node->size,
				merged_target->keys + merged_target->size + 1);
			merged_target->size += target_node->size + 1;
		}
		merged_target->subtree_size += merged_victim->subtree_size + 1u;

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
			auto ch{target_parent->childrens[i]};
			if (ch)
			{
				ch->parent_pos = i;
			}
		}

		str_ranked_btree_set_erase_underflow<allocator_type, keys_number>(imp, target_parent);
		return true;
	}

	// Case 2: deletion from an internal node
	nodeptr internal_node{target_node};
	std::size_t internal_key_index{found_pos};
	nodeptr left_child{internal_node->childrens[internal_key_index]};
	nodeptr right_child{internal_node->childrens[internal_key_index + 1]};

	// Predecessor: replace internal key with rightmost key from left subtree
	if (min_keys < left_child->size)
	{
		// every ancestor of the erased element loses one; the moved predecessor
		// also leaves every node on the descend path
		for (auto n{internal_node}; n; n = n->parent)
		{
			--n->subtree_size;
		}
		nodeptr descend{left_child};
		for (;;)
		{
			--descend->subtree_size;
			if (descend->leaf)
			{
				break;
			}
			descend = descend->childrens[descend->size]; // rightmost child
		}

		// Free payload of the original internal key being erased
		::fast_io::details::deallocate_associative_string<allocator_type, char_type>(
			internal_node->keys[internal_key_index].ptr,
			internal_node->keys[internal_key_index].n);

		// Move predecessor into internal slot (ownership transfer)
		auto &pred_key{descend->keys[descend->size - 1]};
		internal_node->keys[internal_key_index] = pred_key;

		// Remove predecessor from leaf WITHOUT deallocating (ownership moved)
		--descend->size;

		// Repair upward if leaf underflows
		if (descend->parent && descend->size < min_keys)
		{
			str_ranked_btree_set_erase_underflow<allocator_type, keys_number>(imp, descend->parent);
		}
		return true;
	}

	// Successor
	if (right_child->size > min_keys)
	{
		for (auto n{internal_node}; n; n = n->parent)
		{
			--n->subtree_size;
		}
		nodeptr descend{right_child};
		for (;;)
		{
			--descend->subtree_size;
			if (descend->leaf)
			{
				break;
			}
			descend = *descend->childrens;
		}

		// Free payload of the original internal key being erased (fixes leak)
		::fast_io::details::deallocate_associative_string<allocator_type, char_type>(
			internal_node->keys[internal_key_index].ptr,
			internal_node->keys[internal_key_index].n);

		auto &succ_key{*descend->keys};
		internal_node->keys[internal_key_index] = succ_key;
		::fast_io::freestanding::overlapped_copy(descend->keys + 1, descend->keys + descend->size, descend->keys);
		--descend->size;
		if (descend->parent && descend->size < min_keys)
		{
			str_ranked_btree_set_erase_underflow<allocator_type, keys_number>(imp, descend->parent);
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
			auto ch{right_child->childrens[i]};
			if (ch)
			{
				ch->parent = left_child;
				ch->parent_pos = left_size_before + 1 + i;
			}
		}
	}
	left_child->size = left_size_before + right_size_before + 1;
	left_child->subtree_size += right_child->subtree_size + 1u;
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
		auto ch{internal_node->childrens[i]};
		if (ch)
		{
			ch->parent_pos = i;
		}
	}

	// Repair potential underflow upward from the internal node
	str_ranked_btree_set_erase_underflow<allocator_type, keys_number>(imp, internal_node);
	// no subtree_size walk here: the recursive erase decrements the path itself
	return str_ranked_btree_set_erase<allocator_type, keys_number, nodetype>(imp, keystrptr, keystrn);
}

/*
Bulk-load insert used only by constructors and copy assignment: each element
goes through the pseudo-hint path (append at the rightmost leaf when the key
is in order) without maintaining subtree_size; a single post-order pass at
the end computes every node's subtree_size.
*/
template <typename allocator_type, ::std::size_t keys_number, typename nodetype,
		  ::std::input_or_output_iterator Iter, ::std::sentinel_for<Iter> Sen>
inline constexpr void str_ranked_btree_set_insert_range_bulk(
	::fast_io::containers::details::btree_imp &imp, Iter first, Sen last)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
{
	using char_type = typename nodetype::char_type;
	for (; first != last; ++first)
	{
		auto const e{*first};
		auto rm{static_cast<nodetype *>(imp.rightmost)};
		if (rm != nullptr && rm->size != 0 && rm->keys[rm->size - 1u].strvw() < e)
		{
			::fast_io::containers::details::str_btree_insert_key_leaf_at<allocator_type, keys_number, nodetype>(
				rm, rm->size, e.ptr, e.n, imp);
		}
		else
		{
			::fast_io::containers::details::str_ranked_btree_set_insert_key_with_root<allocator_type, keys_number, nodetype>(
				imp, e.ptr, e.n);
		}
	}
	str_ranked_btree_recompute_tree(static_cast<nodetype *>(imp.root));
}

/*
Returns {node, pos} of the element at in-order index n. Caller guarantees
n < subtree_size(root).
*/
template <typename nodetype>
inline constexpr ::fast_io::containers::details::find_btree_insert_result str_ranked_btree_nth_element(nodetype *node, ::std::size_t n) noexcept
{
	for (;;)
	{
		if (node->leaf)
		{
			return {node, n};
		}
		auto const sz{node->size};
		::std::size_t i{};
		for (; i != sz; ++i)
		{
			auto const cs{node->childrens[i]->subtree_size};
			if (n < cs)
			{
				break;
			}
			if (n == cs)
			{
				return {node, i};
			}
			n -= cs + 1u;
		}
		node = node->childrens[i];
	}
}

} // namespace details

template <::std::integral chtype, typename Allocator, ::std::size_t keys_number = 16>
	requires((keys_number != 0) && ((keys_number & 1) == 0))
class basic_str_ranked_btree_set
{
	using node_type = ::fast_io::containers::details::str_ranked_btree_set_node<chtype, keys_number>;
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

	constexpr basic_str_ranked_btree_set() noexcept = default;

	constexpr basic_str_ranked_btree_set(basic_str_ranked_btree_set const &other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		::fast_io::containers::details::str_ranked_btree_set_insert_range_bulk<allocator_type, keys_number, node_type>(
			this->imp, other.cbegin(), other.cend());
	}
	constexpr basic_str_ranked_btree_set &operator=(basic_str_ranked_btree_set const &other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		if (this != ::std::addressof(other))
		{
			this->clear();
			::fast_io::containers::details::str_ranked_btree_set_insert_range_bulk<allocator_type, keys_number, node_type>(
				this->imp, other.cbegin(), other.cend());
		}
		return *this;
	}

	explicit constexpr basic_str_ranked_btree_set(::std::initializer_list<string_view_type> ilist)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		::fast_io::containers::details::str_ranked_btree_set_insert_range_bulk<allocator_type, keys_number, node_type>(
			this->imp, ilist.begin(), ilist.end());
	}

	template <::std::ranges::range R>
	explicit constexpr basic_str_ranked_btree_set(::fast_io::freestanding::from_range_t, R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_constructible_v<string_view_type, ::std::ranges::range_value_t<R>>)
	{
		::fast_io::containers::details::str_ranked_btree_set_insert_range_bulk<allocator_type, keys_number, node_type>(
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
	constexpr size_type size() const noexcept
	{
		auto root{static_cast<node_type *>(this->imp.root)};
		return root == nullptr ? size_type{} : root->subtree_size;
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
	constexpr const_iterator nth_element(size_type n) const
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_on_violations<allocator_type>)
	{
		if (this->size() <= n) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<::fast_io::containers::details::allocator_throws_on_violations<allocator_type>>(::std::errc::invalid_argument);
		}
		return this->nth_element_unchecked(n);
	}
	constexpr const_iterator nth_element_unchecked(size_type n) const noexcept
	{
		auto [ptr, pos] = ::fast_io::containers::details::str_ranked_btree_nth_element(
			static_cast<node_type *>(this->imp.root), n);
		return {ptr, pos, this->imp.rightmost};
	}
	constexpr bool is_empty() const noexcept
	{
		return this->imp.root == nullptr;
	}
	constexpr size_type erase_key(string_view_type key) noexcept
	{
		return ::fast_io::containers::details::str_ranked_btree_set_erase<allocator_type, keys_number, node_type>(this->imp, key.ptr, key.n);
	}
	constexpr bool insert_key(string_view_type key)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
	{
		return ::fast_io::containers::details::str_ranked_btree_set_insert_key_with_root<allocator_type, keys_number, node_type>(this->imp, key.ptr, key.n);
	}
	template <::std::ranges::range R>
	constexpr void insert_range(R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type> || !::std::is_nothrow_constructible_v<string_view_type, ::std::ranges::range_value_t<R>>)
	{
		for (auto const &e : rg)
		{
			this->insert_key(e);
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
	constexpr void swap(basic_str_ranked_btree_set &other) noexcept
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

	constexpr basic_str_ranked_btree_set(basic_str_ranked_btree_set &&other) noexcept : imp(other.imp)
	{
		other.imp = {};
	}
	constexpr basic_str_ranked_btree_set &operator=(basic_str_ranked_btree_set &&other) noexcept
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
	constexpr ~basic_str_ranked_btree_set() noexcept
	{
		clear_node(this->imp.root);
	}
};

template <::std::integral chtype, typename Allocator, ::std::size_t keys_number>
constexpr void swap(::fast_io::containers::basic_str_ranked_btree_set<chtype, Allocator, keys_number> &a,
					::fast_io::containers::basic_str_ranked_btree_set<chtype, Allocator, keys_number> &b) noexcept
{
	a.swap(b);
}

template <::std::integral chtype, typename Allocator, ::std::size_t keys_number>
constexpr bool operator==(::fast_io::containers::basic_str_ranked_btree_set<chtype, Allocator, keys_number> const &lhs,
						  ::fast_io::containers::basic_str_ranked_btree_set<chtype, Allocator, keys_number> const &rhs) noexcept
{
	return ::std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

#if __cpp_impl_three_way_comparison >= 201907L
template <::std::integral chtype, typename Allocator, ::std::size_t keys_number>
constexpr auto operator<=>(::fast_io::containers::basic_str_ranked_btree_set<chtype, Allocator, keys_number> const &lhs,
						   ::fast_io::containers::basic_str_ranked_btree_set<chtype, Allocator, keys_number> const &rhs) noexcept
{
	return ::std::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}
#endif

} // namespace fast_io::containers
