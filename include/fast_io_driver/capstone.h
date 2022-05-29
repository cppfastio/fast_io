#pragma once

#include<capstone/capstone.h>

namespace fast_io::capstone
{

struct cs_error
{
	cs_err e{};
};

inline void throw_cs_error(cs_err e[[maybe_unused]])
{
#ifdef __cpp_exceptions
#if defined(_MSC_VER) && (!defined(_HAS_EXCEPTIONS) || _HAS_EXCEPTIONS == 0)
	fast_terminate();
#else
	throw cs_error{e};
#endif
#else
	fast_terminate();
#endif
}

namespace details
{

inline void cs_option_wrapper_impl(std::size_t csh,cs_opt_type t,std::size_t value)
{
	auto ret{::fast_io::noexcept_call(::cs_option,csh,t,value)};
	if(ret!=CS_ERR_OK)
		throw_cs_error(ret);
}

}

class cs_io_observer
{
public:
	std::size_t csh{};
	void option(cs_opt_type t,std::size_t value) const
	{
		::fast_io::capstone::details::cs_option_wrapper_impl(this->csh,t,value);
	}
};

class cs_file:public cs_io_observer
{
public:
	constexpr cs_file() noexcept = default;
	template<typename T>
	requires std::same_as<T,std::size_t>
	explicit constexpr cs_file(T cs) noexcept:cs_io_observer{cs}{}
	cs_file(cs_file const&)=delete;
	cs_file& operator=(cs_file const&)=delete;
	constexpr cs_file(cs_file&& __restrict other) noexcept: cs_io_observer{other.csh}
	{
		other.csh=0;
	}
	cs_file& operator=(cs_file&& __restrict c) noexcept
	{
		if(this->csh)[[likely]]
			::fast_io::noexcept_call(cs_close,__builtin_addressof(this->csh));
		this->csh=c.csh;
		c.csh=0;
		return *this;
	}
	cs_file(cs_arch arch, cs_mode mode)
	{
		auto ret{::fast_io::noexcept_call(cs_open,arch,mode,__builtin_addressof(this->csh))};
		if(ret!=CS_ERR_OK)
			throw_cs_error(ret);
	}
	~cs_file()
	{
		if(this->csh)[[likely]]
			::fast_io::noexcept_call(cs_close,__builtin_addressof(this->csh));
	}
};

class cs_insn_range
{
public:
	cs_insn* ins{};
	std::size_t count{};

	using value_type = cs_insn;
	using pointer = cs_insn*;
	using const_pointer = cs_insn const*;
	using const_iterator = const_pointer;
	using iterator = pointer;
	using reference = cs_insn&;
	using const_reference = cs_insn const&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;

	constexpr cs_insn_range() noexcept = default;

	explicit cs_insn_range(cs_io_observer csiob,char const* buffer,std::size_t code_size,std::uint_least64_t address,std::size_t c) noexcept
	{
		count=noexcept_call(cs_disasm,csiob.csh,reinterpret_cast<std::uint_least8_t const*>(buffer),code_size,address,c,__builtin_addressof(ins));
	}
	cs_insn_range(cs_insn_range const&)=delete;
	cs_insn_range& operator=(cs_insn_range const&)=delete;
	cs_insn_range(cs_insn_range&& __restrict other) noexcept:ins{other.ins},count{other.count}
	{
		other.ins=nullptr;
		other.count=0;
	}
	cs_insn_range& operator=(cs_insn_range&& __restrict other) noexcept
	{
		if(this->ins)[[likely]]
			::fast_io::noexcept_call(cs_free,this->ins,this->count);
		this->ins=other.ins;
		this->count=other.count;
		other.ins=nullptr;
		other.count=0;
		return *this;
	}
	~cs_insn_range()
	{
		if(this->ins)[[likely]]
			::fast_io::noexcept_call(cs_free,this->ins,this->count);
	}
	constexpr pointer data() const noexcept
	{
		return ins;
	}
	constexpr bool empty() const noexcept
	{
		return !this->count;
	}
	constexpr std::size_t size() const noexcept
	{
		return this->count;
	}
	constexpr const_iterator cbegin() const noexcept
	{
		return ins;
	}
	constexpr const_iterator begin() const noexcept
	{
		return ins;
	}
	constexpr iterator begin() noexcept
	{
		return ins;
	}
	constexpr const_iterator cend() const noexcept
	{
		return ins+this->count;
	}
	constexpr const_iterator end() const noexcept
	{
		return ins+this->count;
	}
	constexpr iterator end() noexcept
	{
		return ins+this->count;
	}
	constexpr std::size_t max_size() const noexcept
	{
		constexpr std::size_t mx_size{SIZE_MAX/sizeof(value_type)};
		return mx_size;
	}
	constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator{ins+this->count};
	}
	constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator{ins+this->count};
	}
	constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator{ins+this->count};
	}
	constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator{ins};
	}
	constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator{ins};
	}
	constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator{ins};
	}
	constexpr const_reference front() const noexcept
	{
		return *ins;
	}
	constexpr reference front() noexcept
	{
		return *ins;
	}
	constexpr const_reference back() const noexcept
	{
		return ins[this->count-1];
	}
	constexpr reference back() noexcept
	{
		return ins[this->count-1];
	}
	inline constexpr reference operator[](size_type size) noexcept
	{
		return ins[size];
	}
	inline constexpr const_reference operator[](size_type size) const noexcept
	{
		return ins[size];
	}
};

}
