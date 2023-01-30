#pragma once

#if (defined(_WIN32) || defined(__CYGWIN__)) && !defined(__WINE__)
#include"win32_heapalloc.h"
#include"nt_rtlheapalloc.h"
#if defined(_MSC_VER) && !defined(__clang__)
#include"msvc/impl.h"
#endif
#endif
#if ((__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && !defined(_LIBCPP_FREESTANDING)) || defined(FAST_IO_ENABLE_HOSTED_FEATURES))
#include"c_malloc.h"
#endif

#if (defined(__linux__) && defined(__KERNEL__)) || defined(FAST_IO_USE_LINUX_KERNEL_ALLOCATOR)
#include"linux_kernel.h"
#endif

#if (defined(FAST_IO_ENABLE_MIMALLOC) || defined(FAST_IO_USE_MIMALLOC)) && (!defined(_MSC_VER) || defined(__clang__))
#include"mimalloc_driver.h"
#endif

#include"custom.h"
#include"adapters.h"

namespace fast_io
{

using native_global_allocator =
generic_allocator_adapter<
#if defined(FAST_IO_USE_CUSTOM_GLOBAL_ALLOCATOR)
custom_global_allocator
#elif defined(FAST_IO_USE_MIMALLOC) && (!defined(_MSC_VER) || defined(__clang__))
mimalloc_allocator
#elif (defined(__linux__) && defined(__KERNEL__)) || defined(FAST_IO_USE_LINUX_KERNEL_ALLOCATOR)
linux_kmalloc_allocator
#elif ((__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && !defined(_LIBCPP_FREESTANDING)) || defined(FAST_IO_ENABLE_HOSTED_FEATURES))
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(FAST_IO_USE_C_MALLOC)
win32_heapalloc_allocator
#else
c_malloc_allocator
#endif
#else
custom_global_allocator
#endif
>
;

template<typename T>
using native_typed_global_allocator = typed_generic_allocator_adapter<native_global_allocator,T>;

using native_thread_local_allocator =
generic_allocator_adapter<
#if defined(FAST_IO_USE_CUSTOM_THREAD_LOCAL_ALLOCATOR)
custom_thread_local_allocator
#else
native_global_allocator
#endif
>
;

template<typename T>
using native_typed_thread_local_allocator = typed_generic_allocator_adapter<native_thread_local_allocator,T>;

}
