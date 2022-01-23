	if constexpr(std::same_as<char_type,char>)
	{
switch(errn)
{
case 0:
	return tsc("Success");
#if defined(EDOM)
case EDOM:
	return tsc("Mathematics argument out of domain of function");
#endif
#if defined(ERANGE)
case ERANGE:
	return tsc("Result too large");
#endif
#if defined(EOVERFLOW)
case EOVERFLOW:
	return tsc("Value too large for defined data type");
#endif
#if defined(ENOTSUP)
case ENOTSUP:
	return tsc("Not supported");
#endif
#if defined(ENOSYS)
case ENOSYS:
	return tsc("Function not implemented");
#endif
#if defined(EINTR)
case EINTR:
	return tsc("Interrupted system call");
#endif
#if defined(ENOERR)
case ENOERR:
	return tsc("Error");
#endif
default:
	return tsc("Unknown");
}
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
switch(errn)
{
case 0:
	return tsc(L"Success");
#if defined(EDOM)
case EDOM:
	return tsc(L"Mathematics argument out of domain of function");
#endif
#if defined(ERANGE)
case ERANGE:
	return tsc(L"Result too large");
#endif
#if defined(EOVERFLOW)
case EOVERFLOW:
	return tsc(L"Value too large for defined data type");
#endif
#if defined(ENOTSUP)
case ENOTSUP:
	return tsc(L"Not supported");
#endif
#if defined(ENOSYS)
case ENOSYS:
	return tsc(L"Function not implemented");
#endif
#if defined(EINTR)
case EINTR:
	return tsc(L"Interrupted system call");
#endif
#if defined(ENOERR)
case ENOERR:
	return tsc(L"Error");
#endif
default:
	return tsc(L"Unknown");
}
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
switch(errn)
{
case 0:
	return tsc(u"Success");
#if defined(EDOM)
case EDOM:
	return tsc(u"Mathematics argument out of domain of function");
#endif
#if defined(ERANGE)
case ERANGE:
	return tsc(u"Result too large");
#endif
#if defined(EOVERFLOW)
case EOVERFLOW:
	return tsc(u"Value too large for defined data type");
#endif
#if defined(ENOTSUP)
case ENOTSUP:
	return tsc(u"Not supported");
#endif
#if defined(ENOSYS)
case ENOSYS:
	return tsc(u"Function not implemented");
#endif
#if defined(EINTR)
case EINTR:
	return tsc(u"Interrupted system call");
#endif
#if defined(ENOERR)
case ENOERR:
	return tsc(u"Error");
#endif
default:
	return tsc(u"Unknown");
}
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
switch(errn)
{
case 0:
	return tsc(U"Success");
#if defined(EDOM)
case EDOM:
	return tsc(U"Mathematics argument out of domain of function");
#endif
#if defined(ERANGE)
case ERANGE:
	return tsc(U"Result too large");
#endif
#if defined(EOVERFLOW)
case EOVERFLOW:
	return tsc(U"Value too large for defined data type");
#endif
#if defined(ENOTSUP)
case ENOTSUP:
	return tsc(U"Not supported");
#endif
#if defined(ENOSYS)
case ENOSYS:
	return tsc(U"Function not implemented");
#endif
#if defined(EINTR)
case EINTR:
	return tsc(U"Interrupted system call");
#endif
#if defined(ENOERR)
case ENOERR:
	return tsc(U"Error");
#endif
default:
	return tsc(U"Unknown");
}
	}
	else
	{
switch(errn)
{
case 0:
	return tsc(u8"Success");
#if defined(EDOM)
case EDOM:
	return tsc(u8"Mathematics argument out of domain of function");
#endif
#if defined(ERANGE)
case ERANGE:
	return tsc(u8"Result too large");
#endif
#if defined(EOVERFLOW)
case EOVERFLOW:
	return tsc(u8"Value too large for defined data type");
#endif
#if defined(ENOTSUP)
case ENOTSUP:
	return tsc(u8"Not supported");
#endif
#if defined(ENOSYS)
case ENOSYS:
	return tsc(u8"Function not implemented");
#endif
#if defined(EINTR)
case EINTR:
	return tsc(u8"Interrupted system call");
#endif
#if defined(ENOERR)
case ENOERR:
	return tsc(u8"Error");
#endif
default:
	return tsc(u8"Unknown");
}
	}