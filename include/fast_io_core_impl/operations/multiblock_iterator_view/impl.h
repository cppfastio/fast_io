#pragma once

namespace fast_io
{

template<::std::size_t blocksize,typename T>
struct multiblock_iterator_view
{
	static inline constexpr
		::std::size_t block_size{blocksize};
	using value_type = T;
	using pointer = value_type*;
	using controller_pointer = pointer*;
	controller_pointer controller_ptr;
	pointer block_begin_ptr,block_curr_ptr,block_end_ptr;
};
}


#if defined(__GLIBCXX__)

#include"libstdc++.h"

#endif

namespace fast_io
{

template<typename Iter>
concept multiblock_view_iterator = ::std::random_access_iterator<Iter>&&requires(Iter iter)
{
	multiblock_iterator_view_ref_define(iter);
};

}

