#pragma once

namespace stvl2
{

struct stvl2_struct_iterator
{
	using value_type = stvl2_tag;
	using difference_type = std::ptrdiff_t;
	stvl2_tag* ptr{};
	inline constexpr stvl2_tag* operator->() const noexcept
	{
		return ptr;
	}
	inline constexpr stvl2_tag& operator*() const noexcept
	{
		return *ptr;
	}
	inline constexpr stvl2_struct_iterator& operator++() noexcept
	{
		ptr=ptr->next;
		return *this;
	}
	inline constexpr stvl2_struct_iterator operator++(int) noexcept
	{
		auto tmp{*this};
		++*this;
		return tmp;
	}
};

inline constexpr bool operator==(stvl2_struct_iterator first,stvl2_struct_iterator second) noexcept
{
	return first.ptr==second.ptr;
}

inline constexpr bool operator!=(stvl2_struct_iterator first,stvl2_struct_iterator second) noexcept
{
	return first.ptr!=second.ptr;
}

static_assert(::std::forward_iterator<stvl2_struct_iterator>);

inline constexpr stvl2_struct_iterator begin(stvl2_struct& s) noexcept
{
	return {s.tags_root};
}

inline constexpr stvl2_struct_iterator end(stvl2_struct& s) noexcept
{
	return {nullptr};
}

}