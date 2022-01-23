#pragma once

namespace fast_io::details
{
/*
This is an extremely naive vector merely for filesystem stack iterative. No std::vector because it uses allocators and throws bad_alloc and length_error which are nonsense.
*/

inline constexpr std::size_t naive_vector_initial_size(16);

template<typename T>
requires (sizeof(T)<=128)
class naive_vector
{
public:
	using value_type = T;
	T* beg_ptr{};
	T* end_ptr{};
	T* cap_ptr{};
	explicit
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	naive_vector() noexcept:beg_ptr(allocate_iobuf_space<T>(naive_vector_initial_size)),end_ptr(beg_ptr),cap_ptr(beg_ptr+naive_vector_initial_size){}

	naive_vector(naive_vector const&)=delete;
	naive_vector& operator=(naive_vector const&)=delete;

	constexpr naive_vector(naive_vector<T>&& other) noexcept:beg_ptr(other.beg_ptr),end_ptr(other.end_ptr),cap_ptr(other.cap_ptr)
	{
		other.cap_ptr=other.end_ptr=other.beg_ptr=nullptr;
	}

	constexpr void clear() noexcept
	{
		for(auto i{beg_ptr};i!=end_ptr;++i)
			i->~T();
		end_ptr=beg_ptr;
	}

private:
	void destroy() noexcept
	{
		if(beg_ptr)[[likely]]
		{
			clear();
			deallocate_iobuf_space<false>(beg_ptr,static_cast<std::size_t>(cap_ptr-beg_ptr));
		}
	}
public:

	constexpr naive_vector& operator=(naive_vector<T>&& other) noexcept
	{
		if(this==__builtin_addressof(other))[[unlikely]]
			return *this;
		destroy();
		beg_ptr=other.beg_ptr;
		end_ptr=other.end_ptr;
		cap_ptr=other.cap_ptr;
		other.cap_ptr=other.end_ptr=other.beg_ptr=nullptr;
		return *this;
	}
	constexpr bool empty() const noexcept
	{
		return beg_ptr==end_ptr;
	}
	constexpr std::size_t size() const noexcept
	{
		return static_cast<std::size_t>(end_ptr-beg_ptr);
	}
	constexpr std::size_t capacity() const noexcept
	{
		return static_cast<std::size_t>(cap_ptr-beg_ptr);
	}

	constexpr T& operator[](std::size_t pos) noexcept
	{
		return beg_ptr[pos];
	}

	constexpr T const& operator[](std::size_t pos) const noexcept
	{
		return beg_ptr[pos];
	}

	constexpr T& back() noexcept
	{
		return end_ptr[-1];
	}
	constexpr T& front() noexcept
	{
		return *beg_ptr;
	}
	constexpr T const& back() const noexcept
	{
		return end_ptr[-1];
	}
	constexpr T const& front() const noexcept
	{
		return *beg_ptr;
	}

	constexpr T* data() noexcept
	{
		return beg_ptr;
	}
	constexpr T const* data() const noexcept
	{
		return beg_ptr;
	}

	constexpr T* begin() noexcept
	{
		return beg_ptr;
	}
	constexpr T const* begin() const noexcept
	{
		return beg_ptr;
	}
	constexpr T const* cbegin() const noexcept
	{
		return beg_ptr;
	}
	constexpr T* end() noexcept
	{
		return end_ptr;
	}
	constexpr T const* end() const noexcept
	{
		return end_ptr;
	}

	constexpr T const* cend() const noexcept
	{
		return end_ptr;
	}
	constexpr void pop_back() noexcept
	{
		(--end_ptr)->~T();
	}
	template<typename... Args>
	requires std::constructible_from<T,Args...>
	constexpr void emplace_back_unchecked(Args&& ...args)
	{
		::new (end_ptr) T(::fast_io::freestanding::forward<Args>(args)...);
		++end_ptr;
	}
	void grow_twice() noexcept
	{
		constexpr std::size_t mx_available_size{SIZE_MAX/sizeof(T)/2};
		std::size_t cap{static_cast<size_t>(cap_ptr-beg_ptr)};
		if(cap>mx_available_size)[[unlikely]]
			fast_terminate();
		std::size_t new_cap{cap};
		if(!new_cap)[[unlikely]]
			new_cap=1;
		new_cap<<=1;
		auto newptr{allocate_iobuf_space<T>(new_cap)};
		auto newi{newptr};
		for(auto oldi{beg_ptr};oldi!=end_ptr;++oldi)
		{
			::new (newi) T(::fast_io::freestanding::move(*oldi));
			oldi->~T();
			++newi;
		}
		deallocate_iobuf_space<false>(beg_ptr,static_cast<std::size_t>(cap_ptr-beg_ptr));
		beg_ptr=newptr;
		end_ptr=newi;
		cap_ptr=newptr+new_cap;
	}
	template<typename... Args>
	requires std::constructible_from<T,Args...>
	void emplace_back(Args&& ...args)
	{
		if(end_ptr==cap_ptr)[[unlikely]]
			grow_twice();
		::new (end_ptr) T(::fast_io::freestanding::forward<Args>(args)...);
		++end_ptr;
	}
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	~naive_vector()
	{
		destroy();
	}
};

}