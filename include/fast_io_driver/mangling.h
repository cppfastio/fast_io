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
	char const* c_str() const noexcept
	{
		if(buffer==nullptr)
			return "";
		return buffer;
	}
	~cxa_demangle()
	{
		free(buffer);
	}
};

}