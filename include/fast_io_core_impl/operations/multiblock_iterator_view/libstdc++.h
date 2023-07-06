#pragma once

namespace fast_io
{

namespace details
{

inline constexpr
	::std::size_t glibcxxdequebufbytes
{
#ifdef _GLIBCXX_DEQUE_BUF_SIZE
_GLIBCXX_DEQUE_BUF_SIZE
#else
512
#endif
};

template<typename T>
inline constexpr
	::std::size_t glibcxxdequebufsize
{
sizeof(T) < glibcxxdequebufbytes ? static_cast<::std::size_t>(glibcxxdequebufbytes / sizeof(T)) : static_cast<::std::size_t>(1)
};

}

template<::std::random_access_iterator Iter>
requires requires(Iter iter)
{
	typename Iter::iterator_category;
	typename Iter::value_type;
	typename Iter::_Elt_pointer;
	typename Iter::_Map_pointer;
	typename Iter::_Self;
	{iter._M_cur}->std::same_as<typename Iter::_Elt_pointer&>;
	{iter._M_first}->std::same_as<typename Iter::_Elt_pointer&>;
	{iter._M_last}->std::same_as<typename Iter::_Elt_pointer&>;
	{iter._M_node}->std::same_as<typename Iter::_Map_pointer&>;
}
constexpr auto multiblock_iterator_view_ref_define(Iter iter) noexcept ->
	::fast_io::multiblock_iterator_view<::fast_io::details::glibcxxdequebufsize<typename Iter::value_type>,
	::std::iter_value_t<Iter>>
{
	return {iter._M_node,iter._M_first,iter._M_cur,iter._M_last};
}

}
