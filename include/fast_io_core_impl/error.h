#pragma once

/*
Legacy-EH shim payload only: when the library is built without
herbceptions but with C++ exceptions enabled, errc-family throw
functions raise this plain {domain, code} struct instead of
terminating. No comparisons, no printing, no domain registry --
consumers read the two fields and that is all. It exists only in
that build mode; -fno-exceptions builds terminate and herbceptions
builds use the throws channel, so neither may name this type.
*/
#if !defined(__HERBCEPTIONS__) && defined(FAST_IO_CPP_EXCEPTIONS)
namespace fast_io
{

struct error
{
	using value_type = ::std::size_t;
	value_type domain{};
	value_type code{};
};

} // namespace fast_io
#endif
