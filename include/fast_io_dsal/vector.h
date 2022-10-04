#pragma once
#undef min
#undef max

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif
#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning( disable : 4464 )
#pragma warning( disable : 4514 )
#pragma warning( disable : 4623 )
#pragma warning( disable : 4626 )
#pragma warning( disable : 4668 )
#pragma warning( disable : 4710 )
#pragma warning( disable : 4820 )
#pragma warning( disable : 5027 )
#pragma warning( disable : 5045 )
#include<cstring>
#endif

#include<version>
#include<type_traits>
#include<concepts>
#include<limits>
#include<cstdint>
#include<cstddef>
#include<new>
#include<initializer_list>
#include<bit>
#include<compare>
#include"../fast_io_core_impl/freestanding/impl.h"
#include"../fast_io_core_impl/terminate.h"
#include"../fast_io_core_impl/allocation/impl.h"

#include"impl/vector.h"

namespace fast_io
{

template<::std::movable T,typename Alloc=::fast_io::native_global_allocator>
using vector = ::fast_io::containers::vector<T,Alloc>;

namespace tlc
{
template<::std::movable T,typename Alloc=::fast_io::native_thread_local_allocator>
using vector = ::fast_io::containers::vector<T,Alloc>;
}

}

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning (pop)
#endif
