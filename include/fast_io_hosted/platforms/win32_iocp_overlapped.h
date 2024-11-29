#pragma once

namespace fast_io
{

class iocp_overlapped_base : public fast_io::win32::overlapped
{
public:
	inline constexpr iocp_overlapped_base()
		: fast_io::win32::overlapped{}
	{}
	inline
#if __cpp_constexpr >= 201907L
	constexpr
#endif
		virtual void
		invoke(::std::size_t) noexcept = 0;
	inline
#if __cpp_constexpr >= 201907L
	constexpr
#endif
		virtual ~iocp_overlapped_base() = default;
};

} // namespace fast_io
