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

inline constexpr void swap(vector_model& lhs, vector_model& rhs) noexcept
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
inline void zero_init_grow_to_size_common_impl(vector_model* m, ::std::size_t newcap) noexcept
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
inline void zero_init_grow_to_size_aligned_impl(vector_model* m, ::std::size_t alignment, ::std::size_t newcap) noexcept
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
		::std::size_t mx_value2{::std::numeric_limits<::std::size_t>::max()/size};
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

inline constexpr void add_zero_towards(vector_model* m, char8_t* end) noexcept
{
	if (end <= m->curr_ptr)
	{
		m->curr_ptr = end;
	}
	else
	{
		::fast_io::freestanding::my_memset(m->curr_ptr, 0, static_cast<std::size_t>(end - m->curr_ptr));
		m->curr_ptr = end;
	}
}

template <typename allocator>
inline constexpr void check_size_and_assign(vector_model* m, char8_t const* begin, char8_t const* end) noexcept
{
	auto const newcap{ static_cast<::std::size_t>(end - begin) };
	if (static_cast<::std::size_t>(m->end_ptr - m->begin_ptr) < newcap)
	{
		grow_to_size_common_impl<allocator>(m, newcap);
	}
	m->curr_ptr = ::fast_io::details::non_overlapped_copy_n(begin, newcap, m->begin_ptr);
}

template <typename allocator>
inline constexpr void check_size_and_construct(vector_model* m, char8_t const* begin, char8_t const* end) noexcept
{
	auto const new_size{ static_cast<::std::size_t>(end - begin) };
	m->begin_ptr = reinterpret_cast<char8_t*>(allocator::allocate(new_size));
	m->curr_ptr = m->end_ptr = m->begin_ptr + new_size;
	::fast_io::freestanding::non_overlapped_copy_n(begin, new_size, m->begin_ptr);
}

template <typename allocator>
inline constexpr void check_size_and_assign_align(vector_model* m, ::std::size_t alignment, char8_t const* begin, char8_t const* end) noexcept
{
	auto const newcap{ static_cast<::std::size_t>(m->end_ptr - m->begin_ptr) };
	if ( newcap < static_cast<std::size_t>(end - begin))
	{
		grow_to_size_common_aligned_impl<allocator>(m, alignment, newcap);
	}
	m->curr_ptr = ::fast_io::details::non_overlapped_copy_n(begin, newcap, m->begin_ptr);
}

template <typename allocator>
inline constexpr void check_size_and_construct_align(vector_model* m, ::std::size_t alignment, char8_t const* begin, char8_t const* end) noexcept
{
	auto const new_size{ static_cast<::std::size_t>(end - begin) };
	m->begin_ptr = allocator::allocate_aligned(alignment, new_size);
	m->curr_ptr = m->end_ptr = m->begin_ptr + new_size;
	::fast_io::freestanding::non_overlapped_copy_n(begin, new_size, m->begin_ptr);
}

inline void erase_impl(vector_model* m, char8_t* first, char8_t* last) noexcept
{
	if (last == m->curr_ptr) // no need to memmove
	{
		m->curr_ptr = first;
	}
	else
	{
		std::size_t const length{static_cast<std::size_t>(m->curr_ptr - last)};
		::fast_io::freestanding::my_memmove(first, last, length);
		m->curr_ptr -= last - first;
	}
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

	[[nodiscard]] constexpr iterator begin() noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] constexpr iterator end() noexcept
	{
		return imp.curr_ptr;
	}
	[[nodiscard]] constexpr const_iterator begin() const noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] constexpr const_iterator end() const noexcept
	{
		return imp.curr_ptr;
	}
	[[nodiscard]] constexpr const_iterator cbegin() const noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] constexpr const_iterator cend() const noexcept
	{
		return imp.curr_ptr;
	}

	[[nodiscard]] constexpr pointer data() noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] constexpr const_pointer data() const noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] constexpr bool is_empty() const noexcept
	{
		return imp.begin_ptr == imp.curr_ptr;
	}

	[[nodiscard]] constexpr bool empty() const noexcept
	{
		return imp.begin_ptr == imp.curr_ptr;
	}
	constexpr void clear() noexcept
	{
		if constexpr(!::std::is_trivially_destructible_v<value_type>)
		{
			for(auto old_i{imp.begin_ptr},old_e{imp.curr_ptr};old_i!=old_e;++old_i)
			{
				old_i->~value_type();
			}
		}
		imp.curr_ptr = imp.begin_ptr;
	}
	[[nodiscard]] constexpr size_type size() const noexcept
	{
		return static_cast<size_type>(imp.curr_ptr-imp.begin_ptr);
	}
	[[nodiscard]] constexpr size_type capacity() const noexcept
	{
		return static_cast<size_type>(imp.end_ptr-imp.begin_ptr);
	}
	[[nodiscard]] static inline constexpr size_type max_size() noexcept
	{
		constexpr
			size_type mx{::std::numeric_limits<size_type>::max()/sizeof(value_type)};
		return mx;
	}
	constexpr vector() noexcept = default;

	constexpr void swap(vector& other) noexcept
	{
		using ::std::swap;
		swap(*reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)), *reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(other.imp)));
	}

private:
	void destroy() noexcept
	{
		clear();
		if constexpr(typed_allocator_type::has_deallocate)
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
	struct partial_destroy
	{
		iterator begin{};
		vector* thisvec{};
		constexpr partial_destroy() noexcept = default;
		explicit constexpr partial_destroy(iterator b, vector* p) noexcept :begin(b), thisvec(p) {}
		partial_destroy(partial_destroy const&) = delete;
		partial_destroy& operator=(partial_destroy const&) = delete;
		constexpr ~partial_destroy()
		{
			if (!begin || !thisvec) return;
			for (; begin != thisvec->imp.curr_ptr; ++begin)
				begin->~value_type();
			thisvec->imp.curr_ptr = begin;
		}
	};
public:

	explicit constexpr vector(size_type n,::fast_io::for_overwrite_t) noexcept(::std::is_trivially_constructible_v<value_type> || ::std::is_nothrow_default_constructible_v<value_type>)
	{
		if constexpr (::std::is_trivially_constructible_v<value_type>)
		{
			imp.begin_ptr = typed_allocator_type::allocate(n);
			imp.end_ptr = imp.curr_ptr = imp.begin_ptr + n;
		}
		else
		{
			auto begin_ptr{ typed_allocator_type::allocate(n) };
			if constexpr (::std::is_nothrow_default_constructible_v<value_type>)
			{
				auto e = imp.end_ptr = imp.curr_ptr = (imp.begin_ptr = begin_ptr) + n;
				for (auto p{ begin_ptr }; p != e; ++p)
				{
					new (p) value_type;
				}
			}
			else
			{
				imp.curr_ptr = imp.begin_ptr = begin_ptr;
				auto e = imp.end_ptr = imp.begin_ptr + n;
				run_destroy des(this);
				for (; imp.curr_ptr != e; ++imp.curr_ptr)
				{
					new (imp.curr_ptr) value_type;
				}
				des.thisvec = nullptr;
			}
		}
	}

	explicit constexpr vector(size_type n) noexcept(::fast_io::freestanding::is_zero_default_constructible_v<value_type> || ::std::is_nothrow_default_constructible_v<value_type>)
	{
		if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			imp.begin_ptr = typed_allocator_type::allocate_zero(n);
			imp.end_ptr = imp.curr_ptr = imp.begin_ptr + n;
		}
		else
		{
			auto begin_ptr{ typed_allocator_type::allocate(n) };
			if constexpr (::std::is_nothrow_default_constructible_v<value_type>)
			{
				auto e = imp.end_ptr = imp.curr_ptr = (imp.begin_ptr = begin_ptr) + n;
				for (auto p{ begin_ptr }; p != e; ++p)
				{
					new (p) value_type();
				}
			}
			else
			{
				imp.curr_ptr = imp.begin_ptr = begin_ptr;
				auto e = imp.end_ptr = imp.begin_ptr + n;
				run_destroy des(this);
				for (; imp.curr_ptr != e; ++imp.curr_ptr)
				{
					new (imp.curr_ptr) value_type();
				}
				des.thisvec = nullptr;
			}
		}
	}
	constexpr vector(size_type n, value_type const& value) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		auto begin_ptr{typed_allocator_type::allocate(n)};
		if constexpr(::std::is_nothrow_copy_constructible_v<value_type>)
		{
			auto e = imp.end_ptr = imp.curr_ptr = (imp.begin_ptr=begin_ptr) + n;
			for (auto p{begin_ptr}; p != e; ++p)
			{
				new (p) value_type(value);
			}
		}
		else
		{
			imp.curr_ptr = imp.begin_ptr = begin_ptr;
			auto e = imp.end_ptr = imp.begin_ptr + n;
			run_destroy des(this);
			for (; imp.curr_ptr != e; ++imp.curr_ptr)
			{
				new (imp.curr_ptr) value_type(value);
			}
			des.thisvec = nullptr;
		}
	}
	template <::std::input_iterator InputIt>
	constexpr vector(InputIt first, InputIt last) noexcept
		requires(::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
	{
		if constexpr (::std::contiguous_iterator<InputIt>)
		{
			using char8_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char8_t const*;
			if constexpr (alignof(value_type) <= allocator_type::default_alignment)
			{
				::fast_io::containers::details::check_size_and_construct<allocator_type>(
					reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
					reinterpret_cast<char8_const_ptr>(::std::to_address(first)),
					reinterpret_cast<char8_const_ptr>(::std::to_address(last)));
			}
			else
			{
				::fast_io::containers::details::check_size_and_construct_align<allocator_type>(
					reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
					alignof(value_type),
					reinterpret_cast<char8_const_ptr>(::std::to_address(first)),
					reinterpret_cast<char8_const_ptr>(::std::to_address(last)));
			}
		}
		else
		{
			auto const size{ static_cast<std::size_t>(last - first) };
			new (this) vector(size);
			assign_common_impl(first, last);
		}
	}
	template <::std::input_iterator InputIt>
	constexpr vector(InputIt first, InputIt last) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
		requires(!::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
	{
		auto const size{ static_cast<std::size_t>(last - first) };
		imp.curr_ptr = imp.begin_ptr = typed_allocator_type::allocator(size);
		auto e = imp.end_ptr = imp.begin_ptr + size;
		run_destroy des{ this };
		assign_common_impl(first, last);
		des.thisvec = nullptr;
	}
	constexpr vector(::std::initializer_list<T> ilist) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		if constexpr (::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
		{
			using char8_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char8_t const*;
			if constexpr (alignof(value_type) <= allocator_type::default_alignment)
			{
				::fast_io::containers::details::check_size_and_construct<allocator_type>(
					reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
					reinterpret_cast<char8_const_ptr>(ilist.begin()),
					reinterpret_cast<char8_const_ptr>(ilist.end()));
			}
			else
			{
				::fast_io::containers::details::check_size_and_construct_align<allocator_type>(
					reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
					alignof(value_type),
					reinterpret_cast<char8_const_ptr>(ilist.begin()),
					reinterpret_cast<char8_const_ptr>(ilist.end()));
			}
		}
		else
		{
			auto const size{ ilist.size() };
			imp.curr_ptr = imp.begin_ptr = typed_allocator_type::allocate(size);
			imp.end_ptr = imp.begin_ptr + size;
			run_destroy des{ this };
			assign_common_impl(ilist.begin(), ilist.end());
			des.thisvec = nullptr;
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
			::fast_io::freestanding::my_memcpy(imp.begin_ptr, vec.imp.begin_ptr, n);
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
		this->operator=(::std::move(newvec));
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
	constexpr reference emplace_back_unchecked(Args&& ...args) noexcept(noexcept(value_type(::std::forward<Args>(args)...)))
	{
		auto p{::new (imp.curr_ptr) value_type(::std::forward<Args>(args)...)};
		++imp.curr_ptr;
		return *p;
	}

private:
	inline constexpr void grow_to_size_impl(size_type newcap) noexcept
	{
		// assert(newcap >= size())
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
			new (new_i) value_type(::std::move(*old_i));
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
	inline constexpr void grow_to_size_nearest_impl(size_type leastcap) noexcept
	{
		constexpr 
			::std::size_t mx_value2{(::std::numeric_limits<::std::size_t>::max()/sizeof(value_type))};
		constexpr 
			::std::size_t mx_value{mx_value2>>1};
		std::size_t cap{static_cast<size_type>(imp.end_ptr-imp.begin_ptr)};
		if(mx_value<cap)
		{
			cap=mx_value2;
		}
		else
		{
			cap<<=1;
		}
		if(cap<leastcap)
		{
			cap=leastcap;
		}
		if constexpr(1<sizeof(value_type))
		{
			if(mx_value2<cap)
			{
				::fast_io::fast_terminate();
			}
		}
		grow_to_size_impl(cap);
	}
	template <typename InputIt>
		requires(::std::input_iterator<InputIt>)
	inline constexpr void assign_common_impl(InputIt first, InputIt last) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		auto ptr{ imp.begin_ptr };
		for (; first != last; ++first, ++ptr)
		{
			new (ptr) value_type(*first);
		}
		imp.curr_ptr = ptr;
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

	constexpr void assign(size_type n, value_type const& value) noexcept
		requires(::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
	{
		if (n > static_cast<std::size_t>(imp.end_ptr - imp.begin_ptr))
			grow_to_size_impl(n);
		::fast_io::freestanding::fill_n(imp.begin_ptr, n, value);
		imp.curr_ptr = imp.begin_ptr + n;
	}
	constexpr void assign(size_type n, value_type const& value) noexcept(::std::is_nothrow_copy_constructible_v<value_type>) // weak exception guarantee
		requires(!::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
	{
		clear();
		if (n > static_cast<std::size_t>(imp.end_ptr - imp.begin_ptr))
			grow_to_size_impl(n);
		run_destroy des{ this };
		for (auto end{imp.begin_ptr + n}; imp.curr_ptr != end; ++imp.curr_ptr)
		{
			new (imp.curr_ptr) value_type(value);
		}
		des.thisvec = nullptr;
	}
	template <::std::input_iterator InputIt>
	constexpr void assign(InputIt first, InputIt last) noexcept
		requires(::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
	{
		if constexpr (::std::contiguous_iterator<InputIt>)
		{
			using char8_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char8_t*;
			if constexpr (alignof(value_type) <= allocator_type::default_alignment)
				::fast_io::containers::details::check_size_and_assign<allocator_type>(
					reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
					reinterpret_cast<char8_ptr>(::std::to_address(first)),
					reinterpret_cast<char8_ptr>(::std::to_address(last)));
			else
				::fast_io::containers::details::check_size_and_assign_align<allocator_type>(
					reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
					alignof(value_type),
					reinterpret_cast<char8_ptr>(::std::to_address(first)),
					reinterpret_cast<char8_ptr>(::std::to_address(last)));
		}
		else
		{
			auto const size{ static_cast<std::size_t>(last - first) };
			if (size > static_cast<std::size_t>(imp.end_ptr - imp.begin_ptr))
				grow_to_size_impl(size);
			assign_common_impl(first, last);
		}
	}
	template <::std::input_iterator InputIt>
	constexpr void assign(InputIt first, InputIt last) noexcept(::std::is_nothrow_copy_constructible_v<value_type>) // weak exception guarantee
		requires(!::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
	{
		clear();
		auto const size{ static_cast<std::size_t>(last - first) };
		if (size > static_cast<std::size_t>(imp.end_ptr - imp.begin_ptr))
			grow_to_size_impl(size);
		run_destroy des{ this };
		if constexpr (::std::contiguous_iterator<InputIt>)
			assign_common_impl(::std::to_address(first),
				::std::to_address(last));
		else
			assign_common_impl(first, last);
		des.thisvec = nullptr;
	}
	constexpr void assign(::std::initializer_list<T> ilist) noexcept
		requires(::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
	{
		using char8_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t*;
		if constexpr (alignof(value_type) <= allocator_type::default_alignment)
			::fast_io::containers::details::check_size_and_assign<allocator_type>(
				reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
				reinterpret_cast<char8_ptr>(ilist.begin()),
				reinterpret_cast<char8_ptr>(ilist.end()));
		else
			::fast_io::containers::details::check_size_and_assign_align<allocator_type>(
				reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
				alignof(value_type),
				reinterpret_cast<char8_ptr>(ilist.begin()),
				reinterpret_cast<char8_ptr>(ilist.end()));
	}
	constexpr void assign(::std::initializer_list<T> ilist) noexcept(::std::is_nothrow_copy_constructible_v<value_type>) // weak exception guarantee
		requires(!::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
	{
		clear();
		auto const size{ ilist.size() };
		if (size > static_cast<std::size_t>(imp.end_ptr - imp.begin_ptr))
			grow_to_size_impl(size);
		run_destroy des{ this };
		assign_common_impl(ilist.begin(), ilist.end());
		des.thisvec = nullptr;
	}
	constexpr void resize(size_type n) noexcept(::std::is_nothrow_default_constructible_v<value_type>)
		requires(::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
	{
		if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			if (n <= static_cast<std::size_t>(imp.end_ptr - imp.begin_ptr))
			{
				using char8_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= char8_t*;
				::fast_io::containers::details::add_zero_towards(
					reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
					reinterpret_cast<char8_ptr>(imp.begin_ptr + n));
				return;
			}
			// else (n > capacity())
			constexpr ::std::size_t mxv{ max_size() };
			if constexpr (1 < sizeof(value_type))
			{
				if (mxv < n)
				{
					::fast_io::fast_terminate();
				}
			}
			if constexpr (alignof(value_type) <= allocator_type::default_alignment)
			{
				::fast_io::containers::details::zero_init_grow_to_size_common_impl<allocator_type>(
					reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
					n * sizeof(value_type));
			}
			else
			{
				::fast_io::containers::details::zero_init_grow_to_size_aligned_impl<allocator_type>(
					reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
					alignof(value_type), n * sizeof(value_type));
			}
			imp.curr_ptr = imp.begin_ptr + n;
			return;
		}
		// else (that is, not zero-initialized)
		auto new_e{ imp.begin_ptr + n };
		if (new_e <= imp.curr_ptr)
		{
			imp.curr_ptr = new_e;
			return;
		}
		else if (new_e <= imp.end_ptr)
		{
			for (; imp.curr_ptr != new_e; ++imp.curr_ptr)
				new (imp.curr_ptr) value_type();
			return;
		}
		// else (n > capacity())
		grow_to_size_impl(n);
		for (; imp.curr_ptr != imp.end_ptr; ++imp.curr_ptr)
		{
			new(imp.curr_ptr) value_type();
		}
		return;
	}
	constexpr void resize(size_type n, value_type const& value) noexcept
		requires(::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
	{
		auto new_e{ imp.begin_ptr + n };
		if (new_e <= imp.curr_ptr)
		{
			imp.curr_ptr = new_e;
			return;
		}
		else if (new_e > imp.end_ptr)
		{
			grow_to_size_impl(n);
			// fallthrough
		}
		for (; imp.curr_ptr != imp.end_ptr; ++imp.curr_ptr)
		{
			new(imp.curr_ptr) value_type(value);
		}
		return;
	}
	constexpr void resize(size_type n, value_type const& value = value_type{}) noexcept(::std::is_nothrow_copy_constructible_v<value_type>) // weak exception guarantee
		requires(!::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
	{
		auto new_e{ imp.begin_ptr + n };
		if (new_e <= imp.curr_ptr)
		{
			for (auto ptr{ new_e }; ptr != imp.curr_ptr; ++ptr)
				ptr->~value_type();
			imp.curr_ptr = new_e;
			return;
		}
		if (new_e > imp.end_ptr)
		{
			grow_to_size_impl(n);
			new_e = imp.begin_ptr + n;
			// fallthrough
		}
		partial_destroy des{ imp.curr_ptr, this };
		for (; imp.curr_ptr != new_e; ++imp.curr_ptr)
		{
			new (imp.curr_ptr) value_type(value);
		}
		des.begin = nullptr;
		des.thisvec = nullptr;
	}
	constexpr iterator erase(const_iterator pos) noexcept
	{
		auto mut_pos{ const_cast<iterator>(pos) };
		if constexpr (::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				using char8_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= char8_t*;
				erase_impl(reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
					reinterpret_cast<char8_ptr>(mut_pos),
					reinterpret_cast<char8_ptr>(mut_pos + 1));
				return mut_pos;
			}
		}
		// else
		mut_pos->~value_type();
		for (auto ptr{ mut_pos + 1 }; ptr != imp.curr_ptr; ++ptr)
		{
			*(ptr - 1) = ::std::move(*ptr);
		}
		--imp.curr_ptr;
		return mut_pos;
	}
	constexpr iterator erase(const_iterator first, const_iterator last) noexcept
	{
		if (first == last) return imp.end_ptr;
		auto mut_first = const_cast<iterator>(first);
		auto mut_last = const_cast<iterator>(last);
		if constexpr (::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				using char8_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= char8_t*;
				erase_impl(reinterpret_cast<::fast_io::containers::details::vector_model*>(__builtin_addressof(imp)),
					reinterpret_cast<char8_ptr>(mut_first),
					reinterpret_cast<char8_ptr>(mut_last));
				return mut_first;
			}
		}
		// else
		for (auto ptr{ mut_first }; ptr != mut_last; ++ptr)
		{
			ptr->~value_type();
		}
		auto to_ptr{ mut_first };
		for (auto from_ptr{ mut_last }; from_ptr != imp.curr_ptr; ++to_ptr, ++from_ptr)
		{
			*to_ptr = ::std::move(*from_ptr);
		}
		imp.curr_ptr = to_ptr;
		return mut_first;
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
	[[nodiscard]] constexpr const_reference operator[](size_type pos) const noexcept
	{
		return imp.begin_ptr[pos];
	}
	[[nodiscard]] constexpr reference operator[](size_type pos) noexcept
	{
		return imp.begin_ptr[pos];
	}
	[[nodiscard]] constexpr const_reference front() const noexcept
	{
		return *imp.begin_ptr;
	}
	[[nodiscard]] constexpr reference front() noexcept
	{
		return *imp.begin_ptr;
	}
	constexpr void push_back(T const& value) noexcept(noexcept(this->emplace_back(value)))
	{
		this->emplace_back(value);
	}
	constexpr void push_back(T&& value) noexcept(noexcept(this->emplace_back(::std::move(value))))
	{
		this->emplace_back(::std::move(value));
	}
	constexpr void push_back_unchecked(T const& value) noexcept(noexcept(this->emplace_back_unchecked(value)))
	{
		this->emplace_back_unchecked(value);
	}
	constexpr void push_back_unchecked(T&& value) noexcept(noexcept(this->emplace_back_unchecked(::std::move(value))))
	{
		this->emplace_back_unchecked(::std::move(value));
	}

private:

	template<::std::input_or_output_iterator Iter>
	constexpr void append_impl(Iter first, Iter last) noexcept(noexcept(this->push_back(*this)))
	{
		if constexpr(::std::forward_iterator<Iter>)
		{
			auto dis{::std::distance(first,last)};
			::std::size_t const remain_space{static_cast<size_type>(imp.end_ptr-imp.curr_ptr)};
			auto sum{remain_space + static_cast<::std::size_t>(dis)};
			::std::size_t const capcty{static_cast<size_type>(imp.end_ptr-imp.begin_ptr)};
			if(capcty < sum)
			{
				if constexpr(SIZE_MAX<::std::numeric_limits<decltype(sum)>::max())
				{
					if(SIZE_MAX<sum)
					{
						::fast_io::fast_terminate();
					}
				}
				this->grow_to_size_nearest_impl(sum);
			}
			this->imp.curr_ptr=::fast_io::freestanding::uninitialized_copy(first,last,this->imp.curr_ptr);
		}
		else
		{
			for(;first!=last;++first)
			{
				this->push_back(*first);
			}
		}
	}

public:

	template<::std::ranges::input_range R>
	constexpr void append_range(R && rg) noexcept(noexcept(this->append_impl(::std::ranges::begin(rg),::std::ranges::end(rg))))
	{
		this->append_impl(::std::ranges::begin(rg),::std::ranges::end(rg));
	}

	[[nodiscard]] constexpr const_reference back() const noexcept
	{
		return imp.curr_ptr[-1];
	}
	[[nodiscard]] constexpr reference back() noexcept
	{
		return imp.curr_ptr[-1];
	}
	template<typename... Args>
	requires std::constructible_from<value_type,Args...>
	constexpr reference emplace_back(Args&& ...args) noexcept(noexcept(value_type(::std::forward<Args>(args)...)))
	{
		if(imp.curr_ptr==imp.end_ptr)
#if __has_cpp_attribute(unlikely)
		[[unlikely]]
#endif
		{
			grow_twice_impl();
		}
		auto p{new (imp.curr_ptr) value_type(::std::forward<Args>(args)...)};
		++imp.curr_ptr;
		return *p;
	}

};

template<typename T, typename allocator>
constexpr auto operator<=>(vector<T, allocator>& lhs, vector<T, allocator>& rhs) noexcept
{
	using ordering_category_t = decltype(T{} <=> T{});
	// copied from cppreference: lexicographical_compare
	auto first1 = lhs.imp.begin_ptr, last1 = lhs.imp.curr_ptr,
		first2 = rhs.imp.begin_ptr, last2 = rhs.imp.curr_ptr;
	for (; (first1 != last1) && (first2 != last2); ++first1, (void) ++first2) {
		if (auto result = *first1 <=> *first2; result != 0)
			return result;
	}
	if (first1 == last1)
		return ordering_category_t(first2 <=> last2);
	return ordering_category_t(::std::strong_ordering::greater);
}

template<typename T, typename allocator>
constexpr bool operator==(vector<T, allocator>& lhs, vector<T, allocator>& rhs) noexcept
{
	return (lhs <=> rhs) == 0;
}

template <typename T, typename allocator>
constexpr void swap(vector<T, allocator>& lhs, vector<T, allocator>& rhs) noexcept
{
	lhs.swap(rhs);
}

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
