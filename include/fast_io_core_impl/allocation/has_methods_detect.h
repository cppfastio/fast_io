#pragma once

template<typename alloc>
concept has_allocate_impl = requires(::std::size_t n)
{
	{alloc::allocate(n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_allocate_aligned_impl = requires(::std::size_t n)
{
	{alloc::allocate_aligned(n,n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_allocate_zero_impl = requires(::std::size_t n)
{
	{alloc::allocate_zero(n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_allocate_aligned_zero_impl = requires(::std::size_t n)
{
	{alloc::allocate_aligned_zero(n,n)}->::std::same_as<void*>;
};


template<typename alloc>
concept has_reallocate_impl = requires(void* p,::std::size_t n)
{
	{alloc::reallocate(p,n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_reallocate_aligned_impl = requires(void* p,::std::size_t n)
{
	{alloc::reallocate_aligned(p,n,n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_reallocate_zero_impl = requires(void* p,::std::size_t n)
{
	{alloc::reallocate_zero(p,n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_reallocate_aligned_zero_impl = requires(void* p,::std::size_t n)
{
	{alloc::reallocate_aligned_zero(p,n,n)}->::std::same_as<void*>;
};


template<typename alloc>
concept has_reallocate_n_impl = requires(void* p,::std::size_t n)
{
	{alloc::reallocate_n(p,n,n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_reallocate_aligned_n_impl = requires(void* p,::std::size_t n)
{
	{alloc::reallocate_aligned_n(p,n,n,n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_reallocate_zero_n_impl = requires(void* p,::std::size_t n)
{
	{alloc::reallocate_zero_n(p,n,n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_reallocate_aligned_zero_n_impl = requires(void* p,::std::size_t n)
{
	{alloc::reallocate_aligned_zero(p,n,n,n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_deallocate_impl = requires(void *p)
{
	{alloc::deallocate(p)}->::std::same_as<void>;
};

template<typename alloc>
concept has_deallocate_aligned_impl = requires(void *p,::std::size_t n)
{
	{alloc::deallocate_aligned(p,n)}->::std::same_as<void>;
};

template<typename alloc>
concept has_deallocate_n_impl = requires(void *p,::std::size_t n)
{
	{alloc::deallocate_n(p,n)}->::std::same_as<void>;
};

template<typename alloc>
concept has_deallocate_aligned_n_impl = requires(void *p,::std::size_t n)
{
	{alloc::deallocate_aligned_n(p,n,n)}->::std::same_as<void>;
};
