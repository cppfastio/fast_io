#pragma once

namespace fast_io
{

class freestanding_global_allocator
{
public:
static ::fast_io::allocation_least_result allocate_aligned_conditional_zero_at_least_die(::std::size_t alignment, ::std::size_t n, bool zero) noexcept;
static ::fast_io::allocation_least_result allocate_aligned_conditional_zero_at_least_try(::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS;
static void deallocate_aligned_n(void* p, ::std::size_t alignment, ::std::size_t n) noexcept;
};

#if defined(FAST_IO_DISABLE_FREESTANDING_THREAD_LOCAL_ALLOCATOR)
using freestanding_thread_local_allocator = freestanding_global_allocator;
#else
class freestanding_thread_local_allocator
{
public:
static ::fast_io::allocation_least_result allocate_aligned_conditional_zero_at_least_die(::std::size_t alignment, ::std::size_t n, bool zero) noexcept;
static ::fast_io::allocation_least_result allocate_aligned_conditional_zero_at_least_try(::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS;
static void deallocate_aligned_n(void* p, ::std::size_t alignment, ::std::size_t n) noexcept;
};
#endif

} // namespace fast_io
