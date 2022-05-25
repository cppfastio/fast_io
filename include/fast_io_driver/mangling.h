#pragma once
#if __has_include(<cxxabi.h>)
#include<cxxabi.h>
#endif
#include<cstdlib>

namespace fast_io
{

struct cxa_demangle
{
	char* buffer{};
	std::size_t length{};
	explicit cxa_demangle(char const* mangled_name) noexcept
	{
#if __has_include(<cxxabi.h>)
		int status{};
		buffer=static_cast<char*>(
		abi::__cxa_demangle(mangled_name,
		nullptr,__builtin_addressof(length),
			__builtin_addressof(status)));
		if(buffer!=nullptr)
		{
			length=strlen(buffer);
		}
		else
#endif
		{
			length=strlen(mangled_name);
			std::size_t lengthp1{length+1};
			auto newp{malloc(lengthp1)};
			if(newp==nullptr)
			{
				fast_terminate();
			}
			memcpy(newp,mangled_name,lengthp1);
			buffer=reinterpret_cast<char*>(newp);
		}
	}
	cxa_demangle(cxa_demangle const& other) noexcept:
		buffer(reinterpret_cast<char*>(malloc(other.length+1))),
		length(other.length)
	{
		if(buffer==nullptr)
			fast_terminate();
		::fast_io::details::my_memcpy(buffer,other.buffer,length);
		buffer[length]=0;
	}
	cxa_demangle& operator=(cxa_demangle const& other) noexcept
	{
		auto newp{reinterpret_cast<char*>(malloc(other.length+1))};
		if(newp==nullptr)
			fast_terminate();
		::fast_io::details::my_memcpy(newp,other.buffer,other.length);
		newp[other.length]=0;
		free(buffer);
		buffer=newp;
		length=other.length;
		return *this;
	}
	constexpr cxa_demangle(cxa_demangle&& __restrict other) noexcept:buffer(other.buffer),length(other.length)
	{
		other.buffer=nullptr;
		other.length=0;
	}
	cxa_demangle& operator=(cxa_demangle&& __restrict other) noexcept
	{
		free(buffer);
		buffer=other.buffer;
		length=other.length;
		other.buffer=nullptr;
		other.length=0;
		return *this;
	}
	char const* data() const noexcept
	{
		return buffer;
	}
	char const* begin() const noexcept
	{
		return buffer;
	}
	char const* end() const noexcept
	{
		return buffer+length;
	}
	~cxa_demangle()
	{
		free(buffer);
	}
};

inline constexpr basic_io_scatter_t<char> print_alias_define(io_alias_t,cxa_demangle const& man) noexcept
{
	return {man.buffer,man.length};
}

}
