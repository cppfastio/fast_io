#pragma once

#include <comdef.h>

namespace fast_io
{
#if 0
namespace details
{

inline void report_com_error(error_reporter& report, auto hresult) requires(std::same_as<TCHAR,char>)
{
	_com_error ce(hresult);
	print_freestanding(report,cstring_view{ce.ErrorMessage()});
}

inline void report_com_error(error_reporter& report, auto hresult) requires(std::same_as<TCHAR,wchar_t>&&sizeof(wchar_t)==2)
{
	_com_error ce(hresult);
	print_freestanding(report,fast_io::code_cvt(std::wstring_view{ce.ErrorMessage()}));
}
}
#endif
class com_error : public std::exception
{
public:
	HRESULT hresult{};
	explicit com_error(HRESULT hr):hresult(hr){}
	constexpr auto code() const noexcept
	{
		return hresult;
	}
#if 0
	void report(error_reporter& report) const override
	{
		details::report_com_error(report,hresult);
	}
#endif
};

}