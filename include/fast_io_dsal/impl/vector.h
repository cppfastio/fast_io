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

template<typename allocator>
inline void grow_to_size_common_impl(vector_model* m,::std::size_t newcap) noexcept
{
	auto begin_ptr{m->begin_ptr};
#if 0
	auto end_ptr{m->end_ptr};
	::std::size_t const cap{static_cast<::std::size_t>(end_ptr-begin_ptr)};
#endif
	::std::size_t const old_size{static_cast<::std::size_t>(m->curr_ptr-begin_ptr)};
	m->begin_ptr=begin_ptr=reinterpret_cast<char8_t*>(allocator::reallocate(begin_ptr,newcap));
	m->curr_ptr=begin_ptr+old_size;
	m->end_ptr=begin_ptr+newcap;
}

template<typename allocator>
inline void grow_to_size_common_aligned_impl(vector_model* m,::std::size_t alignment,::std::size_t newcap) noexcept
{
	auto begin_ptr{m->begin_ptr};
#if 0
	auto end_ptr{m->end_ptr};
	::std::size_t const cap{static_cast<::std::size_t>(end_ptr-begin_ptr)};
#endif
	::std::size_t const old_size{static_cast<::std::size_t>(m->curr_ptr-begin_ptr)};
	m->begin_ptr=begin_ptr=reinterpret_cast<char8_t*>(allocator::reallocate_aligned(begin_ptr,alignment,newcap));
	m->curr_ptr=begin_ptr+old_size;
	m->end_ptr=begin_ptr+newcap;
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

private:
	void destroy() noexcept
	{
		clear();
#if 0
#endif
		typed_allocator_type::deallocate(imp.begin_ptr);
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

	explicit constexpr vector(size_type n) noexcept(::std::is_scalar_v<value_type>)
	{
		if constexpr(::std::is_scalar_v<value_type>)
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
		vec.imp=nullptr;
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
#if 0
		std::size_t const cap{static_cast<size_type>(imp.end_ptr-imp.begin_ptr)};
#endif
		auto new_begin_ptr=typed_allocator_type::allocate(newcap);
		auto new_i{new_begin_ptr};
		for(auto old_i{imp.begin_ptr},old_e{imp.curr_ptr};old_i!=old_e;++old_i)
		{
			new (new_i) value_type(::fast_io::freestanding::move(*old_i));
			old_i->~value_type();
			++new_i;
		}
#if 0
		typed_allocator_type::deallocate(imp.begin_ptr,cap);
#else
		typed_allocator_type::deallocate(imp.begin_ptr);
#endif
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
public:
	constexpr void reserve(size_type n) noexcept
	{
		if(n<=static_cast<std::size_t>(imp.end_ptr-imp.begin_ptr))
		{
			return;
		}
		grow_to_size_impl(n);
	}


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
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#endif
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

}
