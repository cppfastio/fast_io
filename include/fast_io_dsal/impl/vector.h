#pragma once
namespace fast_io
{

namespace containers
{

namespace details
{

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
vector_model
{
	char8_t* begin_ptr;
	char8_t* curr_ptr;
	char8_t* end_ptr;
};

inline void swap(vector_model& lhs, vector_model& rhs)
{
	auto tmp = lhs;
	lhs = rhs;
	rhs = tmp;
}

template<typename allocator>
inline void grow_to_size_common_impl(vector_model* m,::std::size_t newcap) noexcept
{
	auto begin_ptr{m->begin_ptr};

	::std::size_t const old_size{static_cast<::std::size_t>(m->curr_ptr-begin_ptr)};

	if constexpr(allocator::has_reallocate)
	{
		begin_ptr=reinterpret_cast<char8_t*>(allocator::reallocate(begin_ptr,newcap));
	}
	else
	{
		auto end_ptr{m->end_ptr};
		::std::size_t const old_cap{static_cast<::std::size_t>(end_ptr-begin_ptr)};
		begin_ptr=reinterpret_cast<char8_t*>(allocator::reallocate_n(begin_ptr,old_cap,newcap));
	}
	m->begin_ptr=begin_ptr;
	m->curr_ptr=begin_ptr+old_size;
	m->end_ptr=begin_ptr+newcap;
}

template <typename allocator>
inline void zero_init_grow_to_size_common_impl(vector_model* m, ::std::size_t newcap)
{
	auto begin_ptr{ m->begin_ptr };
	::std::size_t const old_size{ static_cast<::std::size_t>(m->curr_ptr - begin_ptr) };
	if constexpr (allocator::has_reallocate_zero)
	{
		begin_ptr = reinterpret_cast<char8_t*>(allocator::reallocate_zero(begin_ptr, newcap));
	}
	else
	{
		auto end_ptr{ m->end_ptr };
		::std::size_t const old_cap{ static_cast<::std::size_t>(end_ptr - begin_ptr) };
		begin_ptr = reinterpret_cast<char8_t*>(allocator::reallocate_zero_n(begin_ptr, old_cap, newcap));
	}
	m->begin_ptr = begin_ptr;
	m->curr_ptr = begin_ptr + old_size;
	m->end_ptr = begin_ptr + newcap;
}

template<typename allocator>
inline void grow_to_size_common_aligned_impl(vector_model* m,::std::size_t alignment,::std::size_t newcap) noexcept
{
	auto begin_ptr{m->begin_ptr};
	::std::size_t const old_size{static_cast<::std::size_t>(m->curr_ptr-begin_ptr)};
	if constexpr(allocator::has_reallocate_aligned)
	{
		begin_ptr=reinterpret_cast<char8_t*>(allocator::reallocate_aligned(begin_ptr,alignment,newcap));
	}
	else
	{
		auto end_ptr{m->end_ptr};
		::std::size_t const oldcap{static_cast<::std::size_t>(end_ptr-begin_ptr)};
		begin_ptr=reinterpret_cast<char8_t*>(allocator::reallocate_aligned_n(begin_ptr,oldcap,alignment,newcap));
	}
	m->begin_ptr=begin_ptr;
	m->curr_ptr=begin_ptr+old_size;
	m->end_ptr=begin_ptr+newcap;
}

template <typename allocator>
inline void zero_init_grow_to_size_aligned_impl(vector_model* m, ::std::size_t alignment, ::std::size_t newcap)
{
	auto begin_ptr{ m->begin_ptr };
	::std::size_t const old_size{ static_cast<::std::size_t>(m->curr_ptr - begin_ptr) };
	if constexpr (allocator::has_reallocate_aligned)
	{
		begin_ptr = reinterpret_cast<char8_t*>(allocator::reallocate_aligned_zero(begin_ptr, alignment, newcap));
	}
	else
	{
		auto end_ptr{ m->end_ptr };
		::std::size_t const oldcap{ static_cast<::std::size_t>(end_ptr - begin_ptr) };
		begin_ptr = reinterpret_cast<char8_t*>(allocator::reallocate_aligned_zero_n(begin_ptr, oldcap, alignment, newcap));
	}
	m->begin_ptr = begin_ptr;
	m->curr_ptr = begin_ptr + old_size;
	m->end_ptr = begin_ptr + newcap;
}

template<::std::size_t size,bool trivial>
inline constexpr ::std::size_t cal_grow_twice_size(::std::size_t cap) noexcept
{
	constexpr 
		::std::size_t mx_value2{::std::numeric_limits<std::size_t>::max()/size};
	constexpr
		::std::size_t mx_value{trivial?mx_value2*size:mx_value2};
	constexpr 
		::std::size_t mx_half_value{mx_value>>1u};
	if(cap==mx_value)
	{
		::fast_io::fast_terminate();
	}
	else if(cap>mx_half_value)
	{
		if constexpr(trivial)
		{
			return mx_value;
		}
		else
		{
			return 1;
		}
	}
	else if(cap==0)
	{
		return size;
	}
	return static_cast<std::size_t>(cap<<1);
}

template<typename allocator,::std::size_t size>
inline constexpr void grow_twice_common_impl(vector_model* m) noexcept
{
	auto begin_ptr{m->begin_ptr};
	auto end_ptr{m->end_ptr};
	grow_to_size_common_impl<allocator>(m,
		cal_grow_twice_size<size,true>(static_cast<::std::size_t>(end_ptr-begin_ptr)));
}

template<typename allocator,::std::size_t size>
inline constexpr void grow_twice_common_aligned_impl(vector_model* m,::std::size_t alignment) noexcept
{
	auto begin_ptr{m->begin_ptr};
	auto end_ptr{m->end_ptr};
	grow_to_size_common_aligned_impl<allocator>(m,
		alignment,
		cal_grow_twice_size<size,true>(static_cast<::std::size_t>(end_ptr-begin_ptr)));
}

template<typename T>
struct vector_internal
{
	T* begin_ptr{};
	T* curr_ptr{};
	T* end_ptr{};
};

}

template<::std::movable T,typename allocator>
class
#if __has_cpp_attribute(clang::trivial_abi)
[[clang::trivial_abi]]
#endif
vector
{
public:
	using allocator_type = allocator;
	using value_type = T;
private:
	using typed_allocator_type = typed_generic_allocator_adapter<allocator_type,value_type>;
public:
	using pointer = value_type*;
	using const_pointer = value_type const*;

	using reference = value_type&;
	using const_reference = value_type const&;

	using iterator = value_type*;
	using const_iterator = value_type const*;

	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	::fast_io::containers::details::vector_internal<T> imp;

	constexpr iterator begin() noexcept
	{
		return imp.begin_ptr;
	}
	constexpr iterator end() noexcept
	{
		return imp.curr_ptr;
	}
	constexpr const_iterator begin() const noexcept
	{
		return imp.begin_ptr;
	}
	constexpr const_iterator end() const noexcept
	{
		return imp.curr_ptr;
	}
	constexpr const_iterator cbegin() const noexcept
	{
		return imp.begin_ptr;
	}
	constexpr const_iterator cend() const noexcept
	{
		return imp.curr_ptr;
	}

	constexpr pointer data() noexcept
	{
		return imp.begin_ptr;
	}
	constexpr const_pointer data() const noexcept
	{
		return imp.begin_ptr;
	}
	constexpr bool is_empty() const noexcept
	{
		return imp.begin_ptr == imp.curr_ptr;
	}

	constexpr bool empty() const noexcept
	{
		return imp.begin_ptr == imp.curr_ptr;
	}
	constexpr void clear() noexcept
	{
		if constexpr(!::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
		{
			for(auto old_i{imp.begin_ptr},old_e{imp.curr_ptr};old_i!=old_e;++old_i)
			{
				old_i->~value_type();
			}
		}
		imp.curr_ptr = imp.begin_ptr;
	}
	constexpr size_type size() const noexcept
	{
		return static_cast<size_type>(imp.curr_ptr-imp.begin_ptr);
	}
	constexpr size_type capacity() const noexcept
	{
		return static_cast<size_type>(imp.end_ptr-imp.begin_ptr);
	}
	static inline constexpr size_type max_size() noexcept
	{
		constexpr
			size_type mx{::std::numeric_limits<size_type>::max()/sizeof(value_type)};
		return mx;
	}
	explicit constexpr vector() noexcept = default;

	constexpr void swap(vector& other) noexcept
	{
		using ::std::swap;
		swap(*reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)), *reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(other.imp)));
	}

private:
	void destroy() noexcept
	{
		clear();
		if constexpr(!typed_allocator_type::has_deallocate)
		{
			typed_allocator_type::deallocate(imp.begin_ptr);
		}
		else
		{
			typed_allocator_type::deallocate_n(imp.begin_ptr,
				static_cast<::std::size_t>(imp.end_ptr-imp.begin_ptr));
		}
	}
	struct run_destroy
	{
		vector* thisvec{};
		constexpr run_destroy() noexcept=default;
		explicit constexpr run_destroy(vector* p) noexcept:thisvec(p){}
		run_destroy(run_destroy const&)=delete;
		run_destroy& operator=(run_destroy const&)=delete;
		constexpr ~run_destroy()
		{
			if(thisvec)
			{
				thisvec->destroy();
			}
		}
	};
public:

	explicit constexpr vector(size_type n) noexcept(::fast_io::freestanding::is_zero_default_constructible_v<value_type>||noexcept(value_type()))
	{
		if constexpr(::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			imp.begin_ptr=typed_allocator_type::allocate_zero(n);
			imp.end_ptr=imp.curr_ptr=imp.begin_ptr+n;
		}
		else
		{
			auto e{this->imp.end_ptr=(this->imp.curr_ptr=this->imp.begin_ptr=
				typed_allocator_type::allocate(n))+n};
			run_destroy des(this);
			for(;this->imp.curr_ptr!=e;++this->imp.curr_ptr)
			{
				new (this->imp.curr_ptr) value_type;
			}
			des.thisvec=nullptr;
		}
	}

	constexpr vector(vector const& vec) requires(::std::copyable<value_type>)
	{
		std::size_t const vecsize{static_cast<std::size_t>(vec.imp.curr_ptr-vec.imp.begin_ptr)};
		if(vecsize==0)
		{
			return;
		}
		imp.begin_ptr=typed_allocator_type::allocate(vecsize);
		if constexpr(::std::is_trivially_copyable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if !consteval
#else
		if(!__builtin_is_constant_evaluated())
#endif
#endif
		{
			std::size_t const n{vecsize*sizeof(value_type)};
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcpy)
			__builtin_memcpy(imp.begin_ptr,vec.imp.begin_ptr,n);
#else
			std::memcpy(imp.begin_ptr,vec.imp.begin_ptr,n);
#endif
#else
			std::memcpy(imp.begin_ptr,vec.imp.begin_ptr,n);
#endif
			imp.end_ptr=imp.curr_ptr=imp.begin_ptr+vecsize;
			return;
		}
		}
		run_destroy des(this);
		this->imp.curr_ptr=this->imp.begin_ptr;
		this->imp.end_ptr=this->imp.begin_ptr+vecsize;
		for(auto i{vec.imp.begin_ptr};i!=vec.imp.curr_ptr;++i)
		{
			new (this->imp.curr_ptr) value_type(*i);
			++this->imp.curr_ptr;
		}
		des.thisvec=nullptr;
	}
	constexpr vector(vector const& vec) = delete;
	constexpr vector& operator=(vector const& vec) requires(::std::copyable<value_type>)
	{
		vector newvec(vec);
		this->operator=(::fast_io::freestanding::move(newvec));
		return *this;
	}
	constexpr vector& operator=(vector const& vec) = delete;
	constexpr vector(vector&& vec) noexcept:imp(vec.imp)
	{
		vec.imp={};
	}
	constexpr vector& operator=(vector&& vec) noexcept
	{
		this->destroy();
		this->imp=vec.imp;
		vec.imp={};
		return *this;
	}
	constexpr ~vector()
	{
		destroy();
	}

	template<typename... Args>
	requires std::constructible_from<value_type,Args...>
	constexpr reference emplace_back_unchecked(Args&& ...args) noexcept(noexcept(value_type(::fast_io::freestanding::forward<Args>(args)...)))
	{
		auto p{::new (imp.curr_ptr) value_type(::fast_io::freestanding::forward<Args>(args)...)};
		++imp.curr_ptr;
		return *p;
	}

private:
	template <bool copy = true>
	inline constexpr void grow_to_size_impl(size_type newcap) noexcept
	{
		if constexpr(::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if(!__builtin_is_constant_evaluated())
#endif
#endif
			{
				constexpr
					::std::size_t mxv{max_size()};
				if constexpr(1<sizeof(value_type))
				{
					if(mxv<newcap)
					{
						::fast_io::fast_terminate();
					}
				}
				newcap*=sizeof(value_type);
				if constexpr(alignof(value_type)<=allocator_type::default_alignment)
				{
					::fast_io::containers::details::grow_to_size_common_impl<allocator_type>(
						reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
						newcap);
				}
				else
				{
					::fast_io::containers::details::grow_to_size_common_aligned_impl<allocator_type>(
						reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
						alignof(value_type),newcap);
				}
				return;
			}
		}
		std::size_t cap;
		if constexpr(!typed_allocator_type::has_deallocate)
		{
			cap = static_cast<size_type>(imp.end_ptr-imp.begin_ptr);
		}
		auto new_begin_ptr=typed_allocator_type::allocate(newcap);
		auto new_i{new_begin_ptr};
		for(auto old_i{imp.begin_ptr},old_e{imp.curr_ptr};old_i!=old_e;++old_i)
		{
			if constexpr (copy)
				new (new_i) value_type(::fast_io::freestanding::move(*old_i));
			old_i->~value_type();
			++new_i;
		}
		if constexpr(typed_allocator_type::has_deallocate)
		{
			typed_allocator_type::deallocate(imp.begin_ptr);
		}
		else
		{
			typed_allocator_type::deallocate_n(imp.begin_ptr,cap);
		}
		imp.begin_ptr=new_begin_ptr;
		imp.curr_ptr=new_i;
		imp.end_ptr=new_begin_ptr+newcap;
	}
	inline constexpr void grow_twice_impl() noexcept
	{
		if constexpr(::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if(!__builtin_is_constant_evaluated())
#endif
#endif
			{
				if constexpr(alignof(value_type)<=allocator_type::default_alignment)
				{
					::fast_io::containers::details::grow_twice_common_impl<allocator_type,sizeof(value_type)>(
						reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)));
				}
				else
				{
					::fast_io::containers::details::grow_twice_common_aligned_impl<allocator_type,sizeof(value_type)>(
						reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
						alignof(value_type));
				}
				return;
			}
		}
		std::size_t const cap{static_cast<size_type>(imp.end_ptr-imp.begin_ptr)};
		grow_to_size_impl(::fast_io::containers::details::cal_grow_twice_size<sizeof(value_type),false>(cap));
	}
	template <bool copy = true>
	inline constexpr void shrink_to_smaller_size_impl(size_type newcap)
	{
		if constexpr (::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
		{
			return grow_to_size_impl(newcap);
		}
		std::size_t cap;
		if constexpr (!typed_allocator_type::has_deallocate)
		{
			cap = static_cast<size_type>(imp.end_ptr - imp.begin_ptr);
		}
		auto new_begin_ptr = typed_allocator_type::allocate(newcap);
		auto old_e{ imp.begin_ptr + newcap };
		for (auto new_i{ new_begin_ptr }, old_i{ imp.begin_ptr }; old_i != old_e; ++old_i, ++new_i)
		{
			if constexpr (copy)
				new (new_i) value_type(::fast_io::freestanding::move(*old_i));
			old_i->~value_type();
		}
		for (; old_e </*in case old_e has been greater then curr_ptr*/ imp.curr_ptr; ++old_e)
		{
			old_e->~value_type();
		}
		if constexpr (typed_allocator_type::has_deallocate)
		{
			typed_allocator_type::deallocate(imp.begin_ptr);
		}
		else
		{
			typed_allocator_type::deallocate_n(imp.begin_ptr, cap);
		}
		imp.begin_ptr = new_begin_ptr;
		imp.curr_ptr = new_begin_ptr + newcap;
		imp.end_ptr = imp.curr_ptr;
	}
public:
	constexpr void reserve(size_type n) noexcept
	{
		if(n<=static_cast<std::size_t>(imp.end_ptr-imp.begin_ptr))
		{
			return;
		}
		grow_to_size_impl(n);
	}

	constexpr void assign(size_type n, value_type const& value) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		if (n > static_cast<std::size_t>(imp.end_ptr - imp.begin_ptr))
			grow_to_size_impl<false>(n);
		else
		{
			auto tmp_ptr = imp.begin_ptr + n;
			if constexpr (!::fast_io::freestanding::is_trivially_relocatable<value_type>)
			{
				for (auto ptr{tmp_ptr}; ptr != imp.curr_ptr; ++ptr)
				{
					ptr->~value_type();
				}
			}
			imp.curr_ptr = tmp_ptr;
		}
		for (auto ptr{ imp.begin_ptr }; ptr != imp.curr_ptr; ++ptr)
			new (ptr) value_type(value);
	}
	template <typename InputIt>
	constexpr void assign(InputIt first, InputIt last)
	{

	}
	constexpr void assign(::std::initializer_list<T> ilist) noexcept
	{
	}
	constexpr void resize(size_type n) noexcept(::std::is_nothrow_default_constructible_v<value_type>)
	{
		if constexpr (::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
		{
			if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
			{
				constexpr ::std::size_t mxv{ max_size() };
				if constexpr (1 < sizeof(value_type))
				{
					if (mxv < n)
					{
						::fast_io::fast_terminate();
					}
				}
				n *= sizeof(value_type);
				if constexpr (alignof(value_type) <= allocator_type::default_alignment)
				{
					::fast_io::containers::details::zero_init_grow_to_size_common_impl<allocator_type>(
						reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
						n);
				}
				else
				{
					::fast_io::containers::details::zero_init_grow_to_size_aligned_impl<allocator_type>(
						reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
						alignof(value_type), n);
				}
				return;
			}
			// else (that is, not zero-initialized)
			auto const old_size = size();
			grow_to_size_impl(n);
			if (n > old_size)
			{
				for (auto ptr = imp.begin_ptr + old_size; ptr != imp.end_ptr; ++ptr)
				{
					new(ptr) value_type();
				}
			}
			return;
		}
		// else (that is, not trivally relocatable)
		if (n < static_cast<std::size_t>(imp.curr_ptr - imp.begin_ptr))
		{
			shrink_to_smaller_size_impl(n);
		}
		else if (n <= static_cast<std::size_t>(imp.end_ptr - imp.begin_ptr))
		{
			for (auto new_e{ imp.begin_ptr + n }; imp.curr_ptr != new_e; ++imp.curr_ptr)
			{
				new (imp.curr_ptr) value_type();
			}
		}
		else // n > capacity()
		{
			grow_to_size_impl(n);
			for (; imp.curr_ptr != imp.end_ptr; ++imp.curr_ptr)
			{
				new (imp.curr_ptr) value_type();
			}
		}
	}
	constexpr void resize(size_type n, value_type const& value) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
		requires(::std::copy_constructible<value_type>)
	{
		if constexpr (::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
		{
			auto const old_size = static_cast<std::size_t>(imp.curr_ptr - imp.begin_ptr);
			grow_to_size_impl(n);
			if (n > old_size)
			{
				for (auto ptr = imp.begin_ptr + old_size; ptr != imp.end_ptr; ++ptr)
				{
					new(ptr) value_type(value);
				}
			}
			return;
		}
		// else
		if (n < size())
		{
			shrink_to_smaller_size_impl(n);
		}
		else if (n <= capacity())
		{
			for (auto new_e{imp.begin_ptr + n}; imp.curr_ptr != new_e; ++imp.curr_ptr)
			{
				new (imp.curr_ptr) value_type(value);
			}
		}
		else // n > capacity()
		{
			grow_to_size_impl(n);
			for (; imp.curr_ptr != imp.end_ptr; ++imp.curr_ptr)
			{
				new (imp.curr_ptr) value_type(value);
			}
		}
	}
	constexpr iterator erase_unchecked(const_iterator pos) noexcept(::std::is_nothrow_destructible_v<value_type> && ::std::is_nothrow_copy_assignable_v<value_type>)
	{
		auto cur_pos{ const_cast<iterator>(pos) };
		cur_pos->~value_type();
		for (auto ptr{ cur_pos + 1 }; ptr != imp.end_ptr; ++ptr)
		{
			*(ptr - 1) = ::fast_io::freestanding::move(*ptr);
		}
		return cur_pos;
	}
	constexpr iterator erase(const_iterator pos) noexcept(noexcept(erase_unchecked(pos)))
	{
		// if (pos >= imp.end_ptr) ::fast_io::fast_terminate();
		return erase_unchecked(pos);
	}
	//constexpr iterator erase(const_iterator first, const_iterator last) TODO
	constexpr void shrink_to_fit() noexcept
	{
		if(imp.curr_ptr==imp.end_ptr)
		{
			return;
		}
		grow_to_size_impl(static_cast<std::size_t>(imp.curr_ptr-imp.begin_ptr));
	}
	constexpr void pop_back() noexcept
	{
		(--imp.curr_ptr)->~value_type();
	}
	constexpr const_reference operator[](size_type pos) const noexcept
	{
		return imp.begin_ptr[pos];
	}
	constexpr reference operator[](size_type pos) noexcept
	{
		return imp.begin_ptr[pos];
	}
	constexpr const_reference front() const noexcept
	{
		return *imp.begin_ptr;
	}
	constexpr reference front() noexcept
	{
		return *imp.begin_ptr;
	}
	constexpr void push_back(T const& value) noexcept(noexcept(this->emplace_back(value)))
	{
		this->emplace_back(value);
	}
	constexpr void push_back(T&& value) noexcept(noexcept(this->emplace_back(::fast_io::freestanding::move(value))))
	{
		this->emplace_back(::fast_io::freestanding::move(value));
	}
	constexpr void push_back_unchecked(T const& value) noexcept(noexcept(this->emplace_back_unchecked(value)))
	{
		this->emplace_back_unchecked(value);
	}
	constexpr void push_back_unchecked(T&& value) noexcept(noexcept(this->emplace_back_unchecked(::fast_io::freestanding::move(value))))
	{
		this->emplace_back_unchecked(::fast_io::freestanding::move(value));
	}
	constexpr const_reference back() const noexcept
	{
		return imp.curr_ptr[-1];
	}
	constexpr reference back() noexcept
	{
		return imp.curr_ptr[-1];
	}
	template<typename... Args>
	requires std::constructible_from<value_type,Args...>
	constexpr reference emplace_back(Args&& ...args) noexcept(noexcept(value_type(::fast_io::freestanding::forward<Args>(args)...)))
	{
		if(imp.curr_ptr==imp.end_ptr)
#if __has_cpp_attribute(unlikely)
		[[unlikely]]
#endif
		{
			grow_twice_impl();
		}
		auto p{new (imp.curr_ptr) value_type(::fast_io::freestanding::forward<Args>(args)...)};
		++imp.curr_ptr;
		return *p;
	}
};

}

namespace freestanding
{

template<typename T,typename Alloc>
struct is_trivially_relocatable<::fast_io::containers::vector<T,Alloc>>
{
	inline static constexpr bool value = true;
};

template<typename T,typename Alloc>
struct is_zero_default_constructible<::fast_io::containers::vector<T,Alloc>>
{
	inline static constexpr bool value = true;
};

}
}
