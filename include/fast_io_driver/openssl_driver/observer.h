#pragma once

namespace fast_io::details
{

template<typename T>
class observer
{
public:
	using native_handle_type = T;
	native_handle_type handle{};
	constexpr operator bool() noexcept
	{
		return handle;
	}
	constexpr auto& native_handle() const noexcept
	{
		return handle;
	}
	constexpr auto& native_handle() noexcept
	{
		return handle;
	}
	constexpr void reset(native_handle_type newhandle=nullptr) noexcept
	{
		handle=newhandle;
	}
	constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle=nullptr;
		return temp;
	}
};

}
