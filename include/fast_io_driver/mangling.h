#pragma once

#include<cxxabi.h>

namespace fast_io
{

struct cxa_demangle
{
	char* buffer{};
	std::size_t length{};
	explicit cxa_demangle(char const* mangled_name)
	{
		int status{};
		buffer=static_cast<char*>(
		abi::__cxa_demangle(mangled_name,
		nullptr,&length,&status));
		if(buffer==nullptr)
		{
			int errn{EINVAL};
			if(status==-1)
				errn=ENOMEM;
			throw_posix_error(errn);
		}
		length=strlen(buffer);
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
		::fast_io::details::my_memcpy(newp,other.buffer,other.length);
		newp[other.length]=0;
		free(buffer);
		buffer=newp;
		length=other.length;
		return *this;
	}
	constexpr cxa_demangle(cxa_demangle&& other) noexcept:buffer(other.buffer),length(other.length)
	{
		other.buffer=nullptr;
		other.length=0;
	}
	cxa_demangle& operator=(cxa_demangle&& other) noexcept
	{
		if(__builtin_addressof(other)==this)
			return *this;
		free(buffer);
		buffer=other.buffer;
		length=other.length;
		other.buffer=nullptr;
		other.length=0;
		return *this;
	}
	explicit constexpr operator ::fast_io::freestanding::string_view() noexcept
	{
		return ::fast_io::freestanding::string_view(buffer,length);
	}
	~cxa_demangle()
	{
		free(buffer);
	}
};

inline constexpr basic_io_scatter_t<char> print_alias_define(io_alias_t,cxa_demangle const& als) noexcept
{
	return {als.buffer,als.length};
}

}