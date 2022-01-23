	if constexpr(std::same_as<char_type,char>)
	{
switch(errn)
{
case 0:
	return tsc("Success");
#if defined (EPERM) && (!defined (EACCES) || (EPERM != EACCES))
case EPERM:
	return tsc("Not owner");
#endif
#ifdef ENOENT
case ENOENT:
	return tsc("No such file or directory");
#endif
#ifdef ESRCH
case ESRCH:
	return tsc("No such process");
#endif
#ifdef EINTR
case EINTR:
	return tsc("Interrupted system call");
#endif
#ifdef EIO
case EIO:
	return tsc("I/O error");
#endif
/* go32 defines ENXIO as ENODEV */
#if defined (ENXIO) && (!defined (ENODEV) || (ENXIO != ENODEV))
case ENXIO:
	return tsc("No such device or address");
#endif
#ifdef E2BIG
case E2BIG:
	return tsc("Arg list too long");
#endif
#ifdef ENOEXEC
case ENOEXEC:
	return tsc("Exec format error");
#endif
#ifdef EALREADY
case EALREADY:
	return tsc("Socket already connected");
#endif
#ifdef EBADF
case EBADF:
	return tsc("Bad file number");
#endif
#ifdef ECHILD
case ECHILD:
	return tsc("No children");
#endif
#ifdef EDESTADDRREQ
case EDESTADDRREQ:
	return tsc("Destination address required");
#endif
#ifdef EAGAIN
case EAGAIN:
	return tsc("No more processes");
#endif
#ifdef ENOMEM
case ENOMEM:
	return tsc("Not enough space");
#endif
#ifdef EACCES
case EACCES:
	return tsc("Permission denied");
#endif
#ifdef EFAULT
case EFAULT:
	return tsc("Bad address");
#endif
#ifdef ENOTBLK
case ENOTBLK:
	return tsc("Block device required");
#endif
#ifdef EBUSY
case EBUSY:
	return tsc("Device or resource busy");
#endif
#ifdef EEXIST
case EEXIST:
	return tsc("File exists");
#endif
#ifdef EXDEV
case EXDEV:
	return tsc("Cross-device link");
#endif
#ifdef ENODEV
case ENODEV:
	return tsc("No such device");
#endif
#ifdef ENOTDIR
case ENOTDIR:
	return tsc("Not a directory");
#endif
#ifdef EHOSTDOWN
case EHOSTDOWN:
	return tsc("Host is down");
#endif
#ifdef EINPROGRESS
case EINPROGRESS:
	return tsc("Connection already in progress");
#endif
#ifdef EISDIR
case EISDIR:
	return tsc("Is a directory");
#endif
#ifdef EINVAL
case EINVAL:
	return tsc("Invalid argument");
#endif
#ifdef ENETDOWN
case ENETDOWN:
	return tsc("Network interface is not configured");
#endif
#ifdef ENETRESET
case ENETRESET:
	return tsc("Connection aborted by network");
#endif
#ifdef ENFILE
case ENFILE:
	return tsc("Too many open files in system");
#endif
#ifdef EMFILE
case EMFILE:
	return tsc("File descriptor value too large");
#endif
#ifdef ENOTTY
case ENOTTY:
	return tsc("Not a character device");
#endif
#ifdef ETXTBSY
case ETXTBSY:
	return tsc("Text file busy");
#endif
#ifdef EFBIG
case EFBIG:
	return tsc("File too large");
#endif
#ifdef EHOSTUNREACH
case EHOSTUNREACH:
	return tsc("Host is unreachable");
#endif
#ifdef ENOSPC
case ENOSPC:
	return tsc("No space left on device");
#endif
#ifdef ENOTSUP
case ENOTSUP:
	return tsc("Not supported");
#endif
#ifdef ESPIPE
case ESPIPE:
	return tsc("Illegal seek");
#endif
#ifdef EROFS
case EROFS:
	return tsc("Read-only file system");
#endif
#ifdef EMLINK
case EMLINK:
	return tsc("Too many links");
#endif
#ifdef EPIPE
case EPIPE:
	return tsc("Broken pipe");
#endif
#ifdef EDOM
case EDOM:
	return tsc("Mathematics argument out of domain of function");
#endif
#ifdef ERANGE
case ERANGE:
	return tsc("Result too large");
#endif
#ifdef ENOMSG
case ENOMSG:
	return tsc("No message of desired type");
#endif
#ifdef EIDRM
case EIDRM:
	return tsc("Identifier removed");
#endif
#ifdef EILSEQ
case EILSEQ:
	return tsc("Illegal byte sequence");
#endif
#ifdef EDEADLK
case EDEADLK:
	return tsc("Deadlock");
#endif
#ifdef ENETUNREACH
case  ENETUNREACH:
	return tsc("Network is unreachable");
#endif
#ifdef ENOLCK
case ENOLCK:
	return tsc("No lock");
#endif
#ifdef ENOSTR
case ENOSTR:
	return tsc("Not a stream");
#endif
#ifdef ETIME
case ETIME:
	return tsc("Stream ioctl timeout");
#endif
#ifdef ENOSR
case ENOSR:
	return tsc("No stream resources");
#endif
#ifdef ENONET
case ENONET:
	return tsc("Machine is not on the network");
#endif
#ifdef ENOPKG
case ENOPKG:
	return tsc("No package");
#endif
#ifdef EREMOTE
case EREMOTE:
	return tsc("Resource is remote");
#endif
#ifdef ENOLINK
case ENOLINK:
	return tsc("Virtual circuit is gone");
#endif
#ifdef EADV
case EADV:
	return tsc("Advertise error");
#endif
#ifdef ESRMNT
case ESRMNT:
	return tsc("Srmount error");
#endif
#ifdef ECOMM
case ECOMM:
	return tsc("Communication error");
#endif
#ifdef EPROTO
case EPROTO:
	return tsc("Protocol error");
#endif
#ifdef EPROTONOSUPPORT
case EPROTONOSUPPORT:
	return tsc("Unknown protocol");
#endif
#ifdef EMULTIHOP
case EMULTIHOP:
	return tsc("Multihop attempted");
#endif
#ifdef EBADMSG
case EBADMSG:
	return tsc("Bad message");
#endif
#ifdef ELIBACC
case ELIBACC:
	return tsc("Cannot access a needed shared library");
#endif
#ifdef ELIBBAD
case ELIBBAD:
	return tsc("Accessing a corrupted shared library");
#endif
#ifdef ELIBSCN
case ELIBSCN:
	return tsc(".lib section in a.out corrupted");
#endif
#ifdef ELIBMAX
case ELIBMAX:
	return tsc("Attempting to link in more shared libraries than system limit");
#endif
#ifdef ELIBEXEC
case ELIBEXEC:
	return tsc("Cannot exec a shared library directly");
#endif
#ifdef ENOSYS
case ENOSYS:
	return tsc("Function not implemented");
#endif
#ifdef ENMFILE
case ENMFILE:
	return tsc("No more files");
#endif
#ifdef ENOTEMPTY
case ENOTEMPTY:
	return tsc("Directory not empty");
#endif
#ifdef ENAMETOOLONG
case ENAMETOOLONG:
	return tsc("File or path name too long");
#endif
#ifdef ELOOP
case ELOOP:
	return tsc("Too many symbolic links");
#endif
#ifdef ENOBUFS
case ENOBUFS:
	return tsc("No buffer space available");
#endif
#ifdef ENODATA
case ENODATA:
	return tsc("No data");
#endif
#ifdef EAFNOSUPPORT
case EAFNOSUPPORT:
	return tsc("Address family not supported by protocol family");
#endif
#ifdef EPROTOTYPE
case EPROTOTYPE:
	return tsc("Protocol wrong type for socket");
#endif
#ifdef ENOTSOCK
case ENOTSOCK:
	return tsc("Socket operation on non-socket");
#endif
#ifdef ENOPROTOOPT
case ENOPROTOOPT:
	return tsc("Protocol not available");
#endif
#ifdef ESHUTDOWN
case ESHUTDOWN:
	return tsc("Can't send after socket shutdown");
#endif
#ifdef ECONNREFUSED
case ECONNREFUSED:
	return tsc("Connection refused");
#endif
#ifdef ECONNRESET
case ECONNRESET:
	return tsc("Connection reset by peer");
#endif
#ifdef EADDRINUSE
case EADDRINUSE:
	return tsc("Address already in use");
#endif
#ifdef EADDRNOTAVAIL
case EADDRNOTAVAIL:
	return tsc("Address not available");
#endif
#ifdef ECONNABORTED
case ECONNABORTED:
	return tsc("Software caused connection abort");
#endif
#if (defined(EWOULDBLOCK) && (!defined (EAGAIN) || (EWOULDBLOCK != EAGAIN)))
case EWOULDBLOCK:
	return tsc("Operation would block");
#endif
#ifdef ENOTCONN
case ENOTCONN:
	return tsc("Socket is not connected");
#endif
#ifdef ESOCKTNOSUPPORT
case ESOCKTNOSUPPORT:
	return tsc("Socket type not supported");
#endif
#ifdef EISCONN
case EISCONN:
	return tsc("Socket is already connected");
#endif
#ifdef ECANCELED
case ECANCELED:
	return tsc("Operation canceled");
#endif
#ifdef ENOTRECOVERABLE
case ENOTRECOVERABLE:
	return tsc("State not recoverable");
#endif
#ifdef EOWNERDEAD
case EOWNERDEAD:
	return tsc("Previous owner died");
#endif
#ifdef ESTRPIPE
case ESTRPIPE:
	return tsc("Streams pipe error");
#endif
#if defined(EOPNOTSUPP) && (!defined(ENOTSUP) || (ENOTSUP != EOPNOTSUPP))
case EOPNOTSUPP:
	return tsc("Operation not supported on socket");
#endif
#ifdef EOVERFLOW
case EOVERFLOW:
	return tsc("Value too large for defined data type");
#endif
#ifdef EMSGSIZE
case EMSGSIZE:
	return tsc("Message too long");
#endif
#ifdef ETIMEDOUT
case ETIMEDOUT:
	return tsc("Connection timed out");
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
#if defined (EPERM) && (!defined (EACCES) || (EPERM != EACCES))
case EPERM:
	return tsc(L"Not owner");
#endif
#ifdef ENOENT
case ENOENT:
	return tsc(L"No such file or directory");
#endif
#ifdef ESRCH
case ESRCH:
	return tsc(L"No such process");
#endif
#ifdef EINTR
case EINTR:
	return tsc(L"Interrupted system call");
#endif
#ifdef EIO
case EIO:
	return tsc(L"I/O error");
#endif
/* go32 defines ENXIO as ENODEV */
#if defined (ENXIO) && (!defined (ENODEV) || (ENXIO != ENODEV))
case ENXIO:
	return tsc(L"No such device or address");
#endif
#ifdef E2BIG
case E2BIG:
	return tsc(L"Arg list too long");
#endif
#ifdef ENOEXEC
case ENOEXEC:
	return tsc(L"Exec format error");
#endif
#ifdef EALREADY
case EALREADY:
	return tsc(L"Socket already connected");
#endif
#ifdef EBADF
case EBADF:
	return tsc(L"Bad file number");
#endif
#ifdef ECHILD
case ECHILD:
	return tsc(L"No children");
#endif
#ifdef EDESTADDRREQ
case EDESTADDRREQ:
	return tsc(L"Destination address required");
#endif
#ifdef EAGAIN
case EAGAIN:
	return tsc(L"No more processes");
#endif
#ifdef ENOMEM
case ENOMEM:
	return tsc(L"Not enough space");
#endif
#ifdef EACCES
case EACCES:
	return tsc(L"Permission denied");
#endif
#ifdef EFAULT
case EFAULT:
	return tsc(L"Bad address");
#endif
#ifdef ENOTBLK
case ENOTBLK:
	return tsc(L"Block device required");
#endif
#ifdef EBUSY
case EBUSY:
	return tsc(L"Device or resource busy");
#endif
#ifdef EEXIST
case EEXIST:
	return tsc(L"File exists");
#endif
#ifdef EXDEV
case EXDEV:
	return tsc(L"Cross-device link");
#endif
#ifdef ENODEV
case ENODEV:
	return tsc(L"No such device");
#endif
#ifdef ENOTDIR
case ENOTDIR:
	return tsc(L"Not a directory");
#endif
#ifdef EHOSTDOWN
case EHOSTDOWN:
	return tsc(L"Host is down");
#endif
#ifdef EINPROGRESS
case EINPROGRESS:
	return tsc(L"Connection already in progress");
#endif
#ifdef EISDIR
case EISDIR:
	return tsc(L"Is a directory");
#endif
#ifdef EINVAL
case EINVAL:
	return tsc(L"Invalid argument");
#endif
#ifdef ENETDOWN
case ENETDOWN:
	return tsc(L"Network interface is not configured");
#endif
#ifdef ENETRESET
case ENETRESET:
	return tsc(L"Connection aborted by network");
#endif
#ifdef ENFILE
case ENFILE:
	return tsc(L"Too many open files in system");
#endif
#ifdef EMFILE
case EMFILE:
	return tsc(L"File descriptor value too large");
#endif
#ifdef ENOTTY
case ENOTTY:
	return tsc(L"Not a character device");
#endif
#ifdef ETXTBSY
case ETXTBSY:
	return tsc(L"Text file busy");
#endif
#ifdef EFBIG
case EFBIG:
	return tsc(L"File too large");
#endif
#ifdef EHOSTUNREACH
case EHOSTUNREACH:
	return tsc(L"Host is unreachable");
#endif
#ifdef ENOSPC
case ENOSPC:
	return tsc(L"No space left on device");
#endif
#ifdef ENOTSUP
case ENOTSUP:
	return tsc(L"Not supported");
#endif
#ifdef ESPIPE
case ESPIPE:
	return tsc(L"Illegal seek");
#endif
#ifdef EROFS
case EROFS:
	return tsc(L"Read-only file system");
#endif
#ifdef EMLINK
case EMLINK:
	return tsc(L"Too many links");
#endif
#ifdef EPIPE
case EPIPE:
	return tsc(L"Broken pipe");
#endif
#ifdef EDOM
case EDOM:
	return tsc(L"Mathematics argument out of domain of function");
#endif
#ifdef ERANGE
case ERANGE:
	return tsc(L"Result too large");
#endif
#ifdef ENOMSG
case ENOMSG:
	return tsc(L"No message of desired type");
#endif
#ifdef EIDRM
case EIDRM:
	return tsc(L"Identifier removed");
#endif
#ifdef EILSEQ
case EILSEQ:
	return tsc(L"Illegal byte sequence");
#endif
#ifdef EDEADLK
case EDEADLK:
	return tsc(L"Deadlock");
#endif
#ifdef ENETUNREACH
case  ENETUNREACH:
	return tsc(L"Network is unreachable");
#endif
#ifdef ENOLCK
case ENOLCK:
	return tsc(L"No lock");
#endif
#ifdef ENOSTR
case ENOSTR:
	return tsc(L"Not a stream");
#endif
#ifdef ETIME
case ETIME:
	return tsc(L"Stream ioctl timeout");
#endif
#ifdef ENOSR
case ENOSR:
	return tsc(L"No stream resources");
#endif
#ifdef ENONET
case ENONET:
	return tsc(L"Machine is not on the network");
#endif
#ifdef ENOPKG
case ENOPKG:
	return tsc(L"No package");
#endif
#ifdef EREMOTE
case EREMOTE:
	return tsc(L"Resource is remote");
#endif
#ifdef ENOLINK
case ENOLINK:
	return tsc(L"Virtual circuit is gone");
#endif
#ifdef EADV
case EADV:
	return tsc(L"Advertise error");
#endif
#ifdef ESRMNT
case ESRMNT:
	return tsc(L"Srmount error");
#endif
#ifdef ECOMM
case ECOMM:
	return tsc(L"Communication error");
#endif
#ifdef EPROTO
case EPROTO:
	return tsc(L"Protocol error");
#endif
#ifdef EPROTONOSUPPORT
case EPROTONOSUPPORT:
	return tsc(L"Unknown protocol");
#endif
#ifdef EMULTIHOP
case EMULTIHOP:
	return tsc(L"Multihop attempted");
#endif
#ifdef EBADMSG
case EBADMSG:
	return tsc(L"Bad message");
#endif
#ifdef ELIBACC
case ELIBACC:
	return tsc(L"Cannot access a needed shared library");
#endif
#ifdef ELIBBAD
case ELIBBAD:
	return tsc(L"Accessing a corrupted shared library");
#endif
#ifdef ELIBSCN
case ELIBSCN:
	return tsc(L".lib section in a.out corrupted");
#endif
#ifdef ELIBMAX
case ELIBMAX:
	return tsc(L"Attempting to link in more shared libraries than system limit");
#endif
#ifdef ELIBEXEC
case ELIBEXEC:
	return tsc(L"Cannot exec a shared library directly");
#endif
#ifdef ENOSYS
case ENOSYS:
	return tsc(L"Function not implemented");
#endif
#ifdef ENMFILE
case ENMFILE:
	return tsc(L"No more files");
#endif
#ifdef ENOTEMPTY
case ENOTEMPTY:
	return tsc(L"Directory not empty");
#endif
#ifdef ENAMETOOLONG
case ENAMETOOLONG:
	return tsc(L"File or path name too long");
#endif
#ifdef ELOOP
case ELOOP:
	return tsc(L"Too many symbolic links");
#endif
#ifdef ENOBUFS
case ENOBUFS:
	return tsc(L"No buffer space available");
#endif
#ifdef ENODATA
case ENODATA:
	return tsc(L"No data");
#endif
#ifdef EAFNOSUPPORT
case EAFNOSUPPORT:
	return tsc(L"Address family not supported by protocol family");
#endif
#ifdef EPROTOTYPE
case EPROTOTYPE:
	return tsc(L"Protocol wrong type for socket");
#endif
#ifdef ENOTSOCK
case ENOTSOCK:
	return tsc(L"Socket operation on non-socket");
#endif
#ifdef ENOPROTOOPT
case ENOPROTOOPT:
	return tsc(L"Protocol not available");
#endif
#ifdef ESHUTDOWN
case ESHUTDOWN:
	return tsc(L"Can't send after socket shutdown");
#endif
#ifdef ECONNREFUSED
case ECONNREFUSED:
	return tsc(L"Connection refused");
#endif
#ifdef ECONNRESET
case ECONNRESET:
	return tsc(L"Connection reset by peer");
#endif
#ifdef EADDRINUSE
case EADDRINUSE:
	return tsc(L"Address already in use");
#endif
#ifdef EADDRNOTAVAIL
case EADDRNOTAVAIL:
	return tsc(L"Address not available");
#endif
#ifdef ECONNABORTED
case ECONNABORTED:
	return tsc(L"Software caused connection abort");
#endif
#if (defined(EWOULDBLOCK) && (!defined (EAGAIN) || (EWOULDBLOCK != EAGAIN)))
case EWOULDBLOCK:
	return tsc(L"Operation would block");
#endif
#ifdef ENOTCONN
case ENOTCONN:
	return tsc(L"Socket is not connected");
#endif
#ifdef ESOCKTNOSUPPORT
case ESOCKTNOSUPPORT:
	return tsc(L"Socket type not supported");
#endif
#ifdef EISCONN
case EISCONN:
	return tsc(L"Socket is already connected");
#endif
#ifdef ECANCELED
case ECANCELED:
	return tsc(L"Operation canceled");
#endif
#ifdef ENOTRECOVERABLE
case ENOTRECOVERABLE:
	return tsc(L"State not recoverable");
#endif
#ifdef EOWNERDEAD
case EOWNERDEAD:
	return tsc(L"Previous owner died");
#endif
#ifdef ESTRPIPE
case ESTRPIPE:
	return tsc(L"Streams pipe error");
#endif
#if defined(EOPNOTSUPP) && (!defined(ENOTSUP) || (ENOTSUP != EOPNOTSUPP))
case EOPNOTSUPP:
	return tsc(L"Operation not supported on socket");
#endif
#ifdef EOVERFLOW
case EOVERFLOW:
	return tsc(L"Value too large for defined data type");
#endif
#ifdef EMSGSIZE
case EMSGSIZE:
	return tsc(L"Message too long");
#endif
#ifdef ETIMEDOUT
case ETIMEDOUT:
	return tsc(L"Connection timed out");
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
#if defined (EPERM) && (!defined (EACCES) || (EPERM != EACCES))
case EPERM:
	return tsc(u"Not owner");
#endif
#ifdef ENOENT
case ENOENT:
	return tsc(u"No such file or directory");
#endif
#ifdef ESRCH
case ESRCH:
	return tsc(u"No such process");
#endif
#ifdef EINTR
case EINTR:
	return tsc(u"Interrupted system call");
#endif
#ifdef EIO
case EIO:
	return tsc(u"I/O error");
#endif
/* go32 defines ENXIO as ENODEV */
#if defined (ENXIO) && (!defined (ENODEV) || (ENXIO != ENODEV))
case ENXIO:
	return tsc(u"No such device or address");
#endif
#ifdef E2BIG
case E2BIG:
	return tsc(u"Arg list too long");
#endif
#ifdef ENOEXEC
case ENOEXEC:
	return tsc(u"Exec format error");
#endif
#ifdef EALREADY
case EALREADY:
	return tsc(u"Socket already connected");
#endif
#ifdef EBADF
case EBADF:
	return tsc(u"Bad file number");
#endif
#ifdef ECHILD
case ECHILD:
	return tsc(u"No children");
#endif
#ifdef EDESTADDRREQ
case EDESTADDRREQ:
	return tsc(u"Destination address required");
#endif
#ifdef EAGAIN
case EAGAIN:
	return tsc(u"No more processes");
#endif
#ifdef ENOMEM
case ENOMEM:
	return tsc(u"Not enough space");
#endif
#ifdef EACCES
case EACCES:
	return tsc(u"Permission denied");
#endif
#ifdef EFAULT
case EFAULT:
	return tsc(u"Bad address");
#endif
#ifdef ENOTBLK
case ENOTBLK:
	return tsc(u"Block device required");
#endif
#ifdef EBUSY
case EBUSY:
	return tsc(u"Device or resource busy");
#endif
#ifdef EEXIST
case EEXIST:
	return tsc(u"File exists");
#endif
#ifdef EXDEV
case EXDEV:
	return tsc(u"Cross-device link");
#endif
#ifdef ENODEV
case ENODEV:
	return tsc(u"No such device");
#endif
#ifdef ENOTDIR
case ENOTDIR:
	return tsc(u"Not a directory");
#endif
#ifdef EHOSTDOWN
case EHOSTDOWN:
	return tsc(u"Host is down");
#endif
#ifdef EINPROGRESS
case EINPROGRESS:
	return tsc(u"Connection already in progress");
#endif
#ifdef EISDIR
case EISDIR:
	return tsc(u"Is a directory");
#endif
#ifdef EINVAL
case EINVAL:
	return tsc(u"Invalid argument");
#endif
#ifdef ENETDOWN
case ENETDOWN:
	return tsc(u"Network interface is not configured");
#endif
#ifdef ENETRESET
case ENETRESET:
	return tsc(u"Connection aborted by network");
#endif
#ifdef ENFILE
case ENFILE:
	return tsc(u"Too many open files in system");
#endif
#ifdef EMFILE
case EMFILE:
	return tsc(u"File descriptor value too large");
#endif
#ifdef ENOTTY
case ENOTTY:
	return tsc(u"Not a character device");
#endif
#ifdef ETXTBSY
case ETXTBSY:
	return tsc(u"Text file busy");
#endif
#ifdef EFBIG
case EFBIG:
	return tsc(u"File too large");
#endif
#ifdef EHOSTUNREACH
case EHOSTUNREACH:
	return tsc(u"Host is unreachable");
#endif
#ifdef ENOSPC
case ENOSPC:
	return tsc(u"No space left on device");
#endif
#ifdef ENOTSUP
case ENOTSUP:
	return tsc(u"Not supported");
#endif
#ifdef ESPIPE
case ESPIPE:
	return tsc(u"Illegal seek");
#endif
#ifdef EROFS
case EROFS:
	return tsc(u"Read-only file system");
#endif
#ifdef EMLINK
case EMLINK:
	return tsc(u"Too many links");
#endif
#ifdef EPIPE
case EPIPE:
	return tsc(u"Broken pipe");
#endif
#ifdef EDOM
case EDOM:
	return tsc(u"Mathematics argument out of domain of function");
#endif
#ifdef ERANGE
case ERANGE:
	return tsc(u"Result too large");
#endif
#ifdef ENOMSG
case ENOMSG:
	return tsc(u"No message of desired type");
#endif
#ifdef EIDRM
case EIDRM:
	return tsc(u"Identifier removed");
#endif
#ifdef EILSEQ
case EILSEQ:
	return tsc(u"Illegal byte sequence");
#endif
#ifdef EDEADLK
case EDEADLK:
	return tsc(u"Deadlock");
#endif
#ifdef ENETUNREACH
case  ENETUNREACH:
	return tsc(u"Network is unreachable");
#endif
#ifdef ENOLCK
case ENOLCK:
	return tsc(u"No lock");
#endif
#ifdef ENOSTR
case ENOSTR:
	return tsc(u"Not a stream");
#endif
#ifdef ETIME
case ETIME:
	return tsc(u"Stream ioctl timeout");
#endif
#ifdef ENOSR
case ENOSR:
	return tsc(u"No stream resources");
#endif
#ifdef ENONET
case ENONET:
	return tsc(u"Machine is not on the network");
#endif
#ifdef ENOPKG
case ENOPKG:
	return tsc(u"No package");
#endif
#ifdef EREMOTE
case EREMOTE:
	return tsc(u"Resource is remote");
#endif
#ifdef ENOLINK
case ENOLINK:
	return tsc(u"Virtual circuit is gone");
#endif
#ifdef EADV
case EADV:
	return tsc(u"Advertise error");
#endif
#ifdef ESRMNT
case ESRMNT:
	return tsc(u"Srmount error");
#endif
#ifdef ECOMM
case ECOMM:
	return tsc(u"Communication error");
#endif
#ifdef EPROTO
case EPROTO:
	return tsc(u"Protocol error");
#endif
#ifdef EPROTONOSUPPORT
case EPROTONOSUPPORT:
	return tsc(u"Unknown protocol");
#endif
#ifdef EMULTIHOP
case EMULTIHOP:
	return tsc(u"Multihop attempted");
#endif
#ifdef EBADMSG
case EBADMSG:
	return tsc(u"Bad message");
#endif
#ifdef ELIBACC
case ELIBACC:
	return tsc(u"Cannot access a needed shared library");
#endif
#ifdef ELIBBAD
case ELIBBAD:
	return tsc(u"Accessing a corrupted shared library");
#endif
#ifdef ELIBSCN
case ELIBSCN:
	return tsc(u".lib section in a.out corrupted");
#endif
#ifdef ELIBMAX
case ELIBMAX:
	return tsc(u"Attempting to link in more shared libraries than system limit");
#endif
#ifdef ELIBEXEC
case ELIBEXEC:
	return tsc(u"Cannot exec a shared library directly");
#endif
#ifdef ENOSYS
case ENOSYS:
	return tsc(u"Function not implemented");
#endif
#ifdef ENMFILE
case ENMFILE:
	return tsc(u"No more files");
#endif
#ifdef ENOTEMPTY
case ENOTEMPTY:
	return tsc(u"Directory not empty");
#endif
#ifdef ENAMETOOLONG
case ENAMETOOLONG:
	return tsc(u"File or path name too long");
#endif
#ifdef ELOOP
case ELOOP:
	return tsc(u"Too many symbolic links");
#endif
#ifdef ENOBUFS
case ENOBUFS:
	return tsc(u"No buffer space available");
#endif
#ifdef ENODATA
case ENODATA:
	return tsc(u"No data");
#endif
#ifdef EAFNOSUPPORT
case EAFNOSUPPORT:
	return tsc(u"Address family not supported by protocol family");
#endif
#ifdef EPROTOTYPE
case EPROTOTYPE:
	return tsc(u"Protocol wrong type for socket");
#endif
#ifdef ENOTSOCK
case ENOTSOCK:
	return tsc(u"Socket operation on non-socket");
#endif
#ifdef ENOPROTOOPT
case ENOPROTOOPT:
	return tsc(u"Protocol not available");
#endif
#ifdef ESHUTDOWN
case ESHUTDOWN:
	return tsc(u"Can't send after socket shutdown");
#endif
#ifdef ECONNREFUSED
case ECONNREFUSED:
	return tsc(u"Connection refused");
#endif
#ifdef ECONNRESET
case ECONNRESET:
	return tsc(u"Connection reset by peer");
#endif
#ifdef EADDRINUSE
case EADDRINUSE:
	return tsc(u"Address already in use");
#endif
#ifdef EADDRNOTAVAIL
case EADDRNOTAVAIL:
	return tsc(u"Address not available");
#endif
#ifdef ECONNABORTED
case ECONNABORTED:
	return tsc(u"Software caused connection abort");
#endif
#if (defined(EWOULDBLOCK) && (!defined (EAGAIN) || (EWOULDBLOCK != EAGAIN)))
case EWOULDBLOCK:
	return tsc(u"Operation would block");
#endif
#ifdef ENOTCONN
case ENOTCONN:
	return tsc(u"Socket is not connected");
#endif
#ifdef ESOCKTNOSUPPORT
case ESOCKTNOSUPPORT:
	return tsc(u"Socket type not supported");
#endif
#ifdef EISCONN
case EISCONN:
	return tsc(u"Socket is already connected");
#endif
#ifdef ECANCELED
case ECANCELED:
	return tsc(u"Operation canceled");
#endif
#ifdef ENOTRECOVERABLE
case ENOTRECOVERABLE:
	return tsc(u"State not recoverable");
#endif
#ifdef EOWNERDEAD
case EOWNERDEAD:
	return tsc(u"Previous owner died");
#endif
#ifdef ESTRPIPE
case ESTRPIPE:
	return tsc(u"Streams pipe error");
#endif
#if defined(EOPNOTSUPP) && (!defined(ENOTSUP) || (ENOTSUP != EOPNOTSUPP))
case EOPNOTSUPP:
	return tsc(u"Operation not supported on socket");
#endif
#ifdef EOVERFLOW
case EOVERFLOW:
	return tsc(u"Value too large for defined data type");
#endif
#ifdef EMSGSIZE
case EMSGSIZE:
	return tsc(u"Message too long");
#endif
#ifdef ETIMEDOUT
case ETIMEDOUT:
	return tsc(u"Connection timed out");
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
#if defined (EPERM) && (!defined (EACCES) || (EPERM != EACCES))
case EPERM:
	return tsc(U"Not owner");
#endif
#ifdef ENOENT
case ENOENT:
	return tsc(U"No such file or directory");
#endif
#ifdef ESRCH
case ESRCH:
	return tsc(U"No such process");
#endif
#ifdef EINTR
case EINTR:
	return tsc(U"Interrupted system call");
#endif
#ifdef EIO
case EIO:
	return tsc(U"I/O error");
#endif
/* go32 defines ENXIO as ENODEV */
#if defined (ENXIO) && (!defined (ENODEV) || (ENXIO != ENODEV))
case ENXIO:
	return tsc(U"No such device or address");
#endif
#ifdef E2BIG
case E2BIG:
	return tsc(U"Arg list too long");
#endif
#ifdef ENOEXEC
case ENOEXEC:
	return tsc(U"Exec format error");
#endif
#ifdef EALREADY
case EALREADY:
	return tsc(U"Socket already connected");
#endif
#ifdef EBADF
case EBADF:
	return tsc(U"Bad file number");
#endif
#ifdef ECHILD
case ECHILD:
	return tsc(U"No children");
#endif
#ifdef EDESTADDRREQ
case EDESTADDRREQ:
	return tsc(U"Destination address required");
#endif
#ifdef EAGAIN
case EAGAIN:
	return tsc(U"No more processes");
#endif
#ifdef ENOMEM
case ENOMEM:
	return tsc(U"Not enough space");
#endif
#ifdef EACCES
case EACCES:
	return tsc(U"Permission denied");
#endif
#ifdef EFAULT
case EFAULT:
	return tsc(U"Bad address");
#endif
#ifdef ENOTBLK
case ENOTBLK:
	return tsc(U"Block device required");
#endif
#ifdef EBUSY
case EBUSY:
	return tsc(U"Device or resource busy");
#endif
#ifdef EEXIST
case EEXIST:
	return tsc(U"File exists");
#endif
#ifdef EXDEV
case EXDEV:
	return tsc(U"Cross-device link");
#endif
#ifdef ENODEV
case ENODEV:
	return tsc(U"No such device");
#endif
#ifdef ENOTDIR
case ENOTDIR:
	return tsc(U"Not a directory");
#endif
#ifdef EHOSTDOWN
case EHOSTDOWN:
	return tsc(U"Host is down");
#endif
#ifdef EINPROGRESS
case EINPROGRESS:
	return tsc(U"Connection already in progress");
#endif
#ifdef EISDIR
case EISDIR:
	return tsc(U"Is a directory");
#endif
#ifdef EINVAL
case EINVAL:
	return tsc(U"Invalid argument");
#endif
#ifdef ENETDOWN
case ENETDOWN:
	return tsc(U"Network interface is not configured");
#endif
#ifdef ENETRESET
case ENETRESET:
	return tsc(U"Connection aborted by network");
#endif
#ifdef ENFILE
case ENFILE:
	return tsc(U"Too many open files in system");
#endif
#ifdef EMFILE
case EMFILE:
	return tsc(U"File descriptor value too large");
#endif
#ifdef ENOTTY
case ENOTTY:
	return tsc(U"Not a character device");
#endif
#ifdef ETXTBSY
case ETXTBSY:
	return tsc(U"Text file busy");
#endif
#ifdef EFBIG
case EFBIG:
	return tsc(U"File too large");
#endif
#ifdef EHOSTUNREACH
case EHOSTUNREACH:
	return tsc(U"Host is unreachable");
#endif
#ifdef ENOSPC
case ENOSPC:
	return tsc(U"No space left on device");
#endif
#ifdef ENOTSUP
case ENOTSUP:
	return tsc(U"Not supported");
#endif
#ifdef ESPIPE
case ESPIPE:
	return tsc(U"Illegal seek");
#endif
#ifdef EROFS
case EROFS:
	return tsc(U"Read-only file system");
#endif
#ifdef EMLINK
case EMLINK:
	return tsc(U"Too many links");
#endif
#ifdef EPIPE
case EPIPE:
	return tsc(U"Broken pipe");
#endif
#ifdef EDOM
case EDOM:
	return tsc(U"Mathematics argument out of domain of function");
#endif
#ifdef ERANGE
case ERANGE:
	return tsc(U"Result too large");
#endif
#ifdef ENOMSG
case ENOMSG:
	return tsc(U"No message of desired type");
#endif
#ifdef EIDRM
case EIDRM:
	return tsc(U"Identifier removed");
#endif
#ifdef EILSEQ
case EILSEQ:
	return tsc(U"Illegal byte sequence");
#endif
#ifdef EDEADLK
case EDEADLK:
	return tsc(U"Deadlock");
#endif
#ifdef ENETUNREACH
case  ENETUNREACH:
	return tsc(U"Network is unreachable");
#endif
#ifdef ENOLCK
case ENOLCK:
	return tsc(U"No lock");
#endif
#ifdef ENOSTR
case ENOSTR:
	return tsc(U"Not a stream");
#endif
#ifdef ETIME
case ETIME:
	return tsc(U"Stream ioctl timeout");
#endif
#ifdef ENOSR
case ENOSR:
	return tsc(U"No stream resources");
#endif
#ifdef ENONET
case ENONET:
	return tsc(U"Machine is not on the network");
#endif
#ifdef ENOPKG
case ENOPKG:
	return tsc(U"No package");
#endif
#ifdef EREMOTE
case EREMOTE:
	return tsc(U"Resource is remote");
#endif
#ifdef ENOLINK
case ENOLINK:
	return tsc(U"Virtual circuit is gone");
#endif
#ifdef EADV
case EADV:
	return tsc(U"Advertise error");
#endif
#ifdef ESRMNT
case ESRMNT:
	return tsc(U"Srmount error");
#endif
#ifdef ECOMM
case ECOMM:
	return tsc(U"Communication error");
#endif
#ifdef EPROTO
case EPROTO:
	return tsc(U"Protocol error");
#endif
#ifdef EPROTONOSUPPORT
case EPROTONOSUPPORT:
	return tsc(U"Unknown protocol");
#endif
#ifdef EMULTIHOP
case EMULTIHOP:
	return tsc(U"Multihop attempted");
#endif
#ifdef EBADMSG
case EBADMSG:
	return tsc(U"Bad message");
#endif
#ifdef ELIBACC
case ELIBACC:
	return tsc(U"Cannot access a needed shared library");
#endif
#ifdef ELIBBAD
case ELIBBAD:
	return tsc(U"Accessing a corrupted shared library");
#endif
#ifdef ELIBSCN
case ELIBSCN:
	return tsc(U".lib section in a.out corrupted");
#endif
#ifdef ELIBMAX
case ELIBMAX:
	return tsc(U"Attempting to link in more shared libraries than system limit");
#endif
#ifdef ELIBEXEC
case ELIBEXEC:
	return tsc(U"Cannot exec a shared library directly");
#endif
#ifdef ENOSYS
case ENOSYS:
	return tsc(U"Function not implemented");
#endif
#ifdef ENMFILE
case ENMFILE:
	return tsc(U"No more files");
#endif
#ifdef ENOTEMPTY
case ENOTEMPTY:
	return tsc(U"Directory not empty");
#endif
#ifdef ENAMETOOLONG
case ENAMETOOLONG:
	return tsc(U"File or path name too long");
#endif
#ifdef ELOOP
case ELOOP:
	return tsc(U"Too many symbolic links");
#endif
#ifdef ENOBUFS
case ENOBUFS:
	return tsc(U"No buffer space available");
#endif
#ifdef ENODATA
case ENODATA:
	return tsc(U"No data");
#endif
#ifdef EAFNOSUPPORT
case EAFNOSUPPORT:
	return tsc(U"Address family not supported by protocol family");
#endif
#ifdef EPROTOTYPE
case EPROTOTYPE:
	return tsc(U"Protocol wrong type for socket");
#endif
#ifdef ENOTSOCK
case ENOTSOCK:
	return tsc(U"Socket operation on non-socket");
#endif
#ifdef ENOPROTOOPT
case ENOPROTOOPT:
	return tsc(U"Protocol not available");
#endif
#ifdef ESHUTDOWN
case ESHUTDOWN:
	return tsc(U"Can't send after socket shutdown");
#endif
#ifdef ECONNREFUSED
case ECONNREFUSED:
	return tsc(U"Connection refused");
#endif
#ifdef ECONNRESET
case ECONNRESET:
	return tsc(U"Connection reset by peer");
#endif
#ifdef EADDRINUSE
case EADDRINUSE:
	return tsc(U"Address already in use");
#endif
#ifdef EADDRNOTAVAIL
case EADDRNOTAVAIL:
	return tsc(U"Address not available");
#endif
#ifdef ECONNABORTED
case ECONNABORTED:
	return tsc(U"Software caused connection abort");
#endif
#if (defined(EWOULDBLOCK) && (!defined (EAGAIN) || (EWOULDBLOCK != EAGAIN)))
case EWOULDBLOCK:
	return tsc(U"Operation would block");
#endif
#ifdef ENOTCONN
case ENOTCONN:
	return tsc(U"Socket is not connected");
#endif
#ifdef ESOCKTNOSUPPORT
case ESOCKTNOSUPPORT:
	return tsc(U"Socket type not supported");
#endif
#ifdef EISCONN
case EISCONN:
	return tsc(U"Socket is already connected");
#endif
#ifdef ECANCELED
case ECANCELED:
	return tsc(U"Operation canceled");
#endif
#ifdef ENOTRECOVERABLE
case ENOTRECOVERABLE:
	return tsc(U"State not recoverable");
#endif
#ifdef EOWNERDEAD
case EOWNERDEAD:
	return tsc(U"Previous owner died");
#endif
#ifdef ESTRPIPE
case ESTRPIPE:
	return tsc(U"Streams pipe error");
#endif
#if defined(EOPNOTSUPP) && (!defined(ENOTSUP) || (ENOTSUP != EOPNOTSUPP))
case EOPNOTSUPP:
	return tsc(U"Operation not supported on socket");
#endif
#ifdef EOVERFLOW
case EOVERFLOW:
	return tsc(U"Value too large for defined data type");
#endif
#ifdef EMSGSIZE
case EMSGSIZE:
	return tsc(U"Message too long");
#endif
#ifdef ETIMEDOUT
case ETIMEDOUT:
	return tsc(U"Connection timed out");
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
#if defined (EPERM) && (!defined (EACCES) || (EPERM != EACCES))
case EPERM:
	return tsc(u8"Not owner");
#endif
#ifdef ENOENT
case ENOENT:
	return tsc(u8"No such file or directory");
#endif
#ifdef ESRCH
case ESRCH:
	return tsc(u8"No such process");
#endif
#ifdef EINTR
case EINTR:
	return tsc(u8"Interrupted system call");
#endif
#ifdef EIO
case EIO:
	return tsc(u8"I/O error");
#endif
/* go32 defines ENXIO as ENODEV */
#if defined (ENXIO) && (!defined (ENODEV) || (ENXIO != ENODEV))
case ENXIO:
	return tsc(u8"No such device or address");
#endif
#ifdef E2BIG
case E2BIG:
	return tsc(u8"Arg list too long");
#endif
#ifdef ENOEXEC
case ENOEXEC:
	return tsc(u8"Exec format error");
#endif
#ifdef EALREADY
case EALREADY:
	return tsc(u8"Socket already connected");
#endif
#ifdef EBADF
case EBADF:
	return tsc(u8"Bad file number");
#endif
#ifdef ECHILD
case ECHILD:
	return tsc(u8"No children");
#endif
#ifdef EDESTADDRREQ
case EDESTADDRREQ:
	return tsc(u8"Destination address required");
#endif
#ifdef EAGAIN
case EAGAIN:
	return tsc(u8"No more processes");
#endif
#ifdef ENOMEM
case ENOMEM:
	return tsc(u8"Not enough space");
#endif
#ifdef EACCES
case EACCES:
	return tsc(u8"Permission denied");
#endif
#ifdef EFAULT
case EFAULT:
	return tsc(u8"Bad address");
#endif
#ifdef ENOTBLK
case ENOTBLK:
	return tsc(u8"Block device required");
#endif
#ifdef EBUSY
case EBUSY:
	return tsc(u8"Device or resource busy");
#endif
#ifdef EEXIST
case EEXIST:
	return tsc(u8"File exists");
#endif
#ifdef EXDEV
case EXDEV:
	return tsc(u8"Cross-device link");
#endif
#ifdef ENODEV
case ENODEV:
	return tsc(u8"No such device");
#endif
#ifdef ENOTDIR
case ENOTDIR:
	return tsc(u8"Not a directory");
#endif
#ifdef EHOSTDOWN
case EHOSTDOWN:
	return tsc(u8"Host is down");
#endif
#ifdef EINPROGRESS
case EINPROGRESS:
	return tsc(u8"Connection already in progress");
#endif
#ifdef EISDIR
case EISDIR:
	return tsc(u8"Is a directory");
#endif
#ifdef EINVAL
case EINVAL:
	return tsc(u8"Invalid argument");
#endif
#ifdef ENETDOWN
case ENETDOWN:
	return tsc(u8"Network interface is not configured");
#endif
#ifdef ENETRESET
case ENETRESET:
	return tsc(u8"Connection aborted by network");
#endif
#ifdef ENFILE
case ENFILE:
	return tsc(u8"Too many open files in system");
#endif
#ifdef EMFILE
case EMFILE:
	return tsc(u8"File descriptor value too large");
#endif
#ifdef ENOTTY
case ENOTTY:
	return tsc(u8"Not a character device");
#endif
#ifdef ETXTBSY
case ETXTBSY:
	return tsc(u8"Text file busy");
#endif
#ifdef EFBIG
case EFBIG:
	return tsc(u8"File too large");
#endif
#ifdef EHOSTUNREACH
case EHOSTUNREACH:
	return tsc(u8"Host is unreachable");
#endif
#ifdef ENOSPC
case ENOSPC:
	return tsc(u8"No space left on device");
#endif
#ifdef ENOTSUP
case ENOTSUP:
	return tsc(u8"Not supported");
#endif
#ifdef ESPIPE
case ESPIPE:
	return tsc(u8"Illegal seek");
#endif
#ifdef EROFS
case EROFS:
	return tsc(u8"Read-only file system");
#endif
#ifdef EMLINK
case EMLINK:
	return tsc(u8"Too many links");
#endif
#ifdef EPIPE
case EPIPE:
	return tsc(u8"Broken pipe");
#endif
#ifdef EDOM
case EDOM:
	return tsc(u8"Mathematics argument out of domain of function");
#endif
#ifdef ERANGE
case ERANGE:
	return tsc(u8"Result too large");
#endif
#ifdef ENOMSG
case ENOMSG:
	return tsc(u8"No message of desired type");
#endif
#ifdef EIDRM
case EIDRM:
	return tsc(u8"Identifier removed");
#endif
#ifdef EILSEQ
case EILSEQ:
	return tsc(u8"Illegal byte sequence");
#endif
#ifdef EDEADLK
case EDEADLK:
	return tsc(u8"Deadlock");
#endif
#ifdef ENETUNREACH
case  ENETUNREACH:
	return tsc(u8"Network is unreachable");
#endif
#ifdef ENOLCK
case ENOLCK:
	return tsc(u8"No lock");
#endif
#ifdef ENOSTR
case ENOSTR:
	return tsc(u8"Not a stream");
#endif
#ifdef ETIME
case ETIME:
	return tsc(u8"Stream ioctl timeout");
#endif
#ifdef ENOSR
case ENOSR:
	return tsc(u8"No stream resources");
#endif
#ifdef ENONET
case ENONET:
	return tsc(u8"Machine is not on the network");
#endif
#ifdef ENOPKG
case ENOPKG:
	return tsc(u8"No package");
#endif
#ifdef EREMOTE
case EREMOTE:
	return tsc(u8"Resource is remote");
#endif
#ifdef ENOLINK
case ENOLINK:
	return tsc(u8"Virtual circuit is gone");
#endif
#ifdef EADV
case EADV:
	return tsc(u8"Advertise error");
#endif
#ifdef ESRMNT
case ESRMNT:
	return tsc(u8"Srmount error");
#endif
#ifdef ECOMM
case ECOMM:
	return tsc(u8"Communication error");
#endif
#ifdef EPROTO
case EPROTO:
	return tsc(u8"Protocol error");
#endif
#ifdef EPROTONOSUPPORT
case EPROTONOSUPPORT:
	return tsc(u8"Unknown protocol");
#endif
#ifdef EMULTIHOP
case EMULTIHOP:
	return tsc(u8"Multihop attempted");
#endif
#ifdef EBADMSG
case EBADMSG:
	return tsc(u8"Bad message");
#endif
#ifdef ELIBACC
case ELIBACC:
	return tsc(u8"Cannot access a needed shared library");
#endif
#ifdef ELIBBAD
case ELIBBAD:
	return tsc(u8"Accessing a corrupted shared library");
#endif
#ifdef ELIBSCN
case ELIBSCN:
	return tsc(u8".lib section in a.out corrupted");
#endif
#ifdef ELIBMAX
case ELIBMAX:
	return tsc(u8"Attempting to link in more shared libraries than system limit");
#endif
#ifdef ELIBEXEC
case ELIBEXEC:
	return tsc(u8"Cannot exec a shared library directly");
#endif
#ifdef ENOSYS
case ENOSYS:
	return tsc(u8"Function not implemented");
#endif
#ifdef ENMFILE
case ENMFILE:
	return tsc(u8"No more files");
#endif
#ifdef ENOTEMPTY
case ENOTEMPTY:
	return tsc(u8"Directory not empty");
#endif
#ifdef ENAMETOOLONG
case ENAMETOOLONG:
	return tsc(u8"File or path name too long");
#endif
#ifdef ELOOP
case ELOOP:
	return tsc(u8"Too many symbolic links");
#endif
#ifdef ENOBUFS
case ENOBUFS:
	return tsc(u8"No buffer space available");
#endif
#ifdef ENODATA
case ENODATA:
	return tsc(u8"No data");
#endif
#ifdef EAFNOSUPPORT
case EAFNOSUPPORT:
	return tsc(u8"Address family not supported by protocol family");
#endif
#ifdef EPROTOTYPE
case EPROTOTYPE:
	return tsc(u8"Protocol wrong type for socket");
#endif
#ifdef ENOTSOCK
case ENOTSOCK:
	return tsc(u8"Socket operation on non-socket");
#endif
#ifdef ENOPROTOOPT
case ENOPROTOOPT:
	return tsc(u8"Protocol not available");
#endif
#ifdef ESHUTDOWN
case ESHUTDOWN:
	return tsc(u8"Can't send after socket shutdown");
#endif
#ifdef ECONNREFUSED
case ECONNREFUSED:
	return tsc(u8"Connection refused");
#endif
#ifdef ECONNRESET
case ECONNRESET:
	return tsc(u8"Connection reset by peer");
#endif
#ifdef EADDRINUSE
case EADDRINUSE:
	return tsc(u8"Address already in use");
#endif
#ifdef EADDRNOTAVAIL
case EADDRNOTAVAIL:
	return tsc(u8"Address not available");
#endif
#ifdef ECONNABORTED
case ECONNABORTED:
	return tsc(u8"Software caused connection abort");
#endif
#if (defined(EWOULDBLOCK) && (!defined (EAGAIN) || (EWOULDBLOCK != EAGAIN)))
case EWOULDBLOCK:
	return tsc(u8"Operation would block");
#endif
#ifdef ENOTCONN
case ENOTCONN:
	return tsc(u8"Socket is not connected");
#endif
#ifdef ESOCKTNOSUPPORT
case ESOCKTNOSUPPORT:
	return tsc(u8"Socket type not supported");
#endif
#ifdef EISCONN
case EISCONN:
	return tsc(u8"Socket is already connected");
#endif
#ifdef ECANCELED
case ECANCELED:
	return tsc(u8"Operation canceled");
#endif
#ifdef ENOTRECOVERABLE
case ENOTRECOVERABLE:
	return tsc(u8"State not recoverable");
#endif
#ifdef EOWNERDEAD
case EOWNERDEAD:
	return tsc(u8"Previous owner died");
#endif
#ifdef ESTRPIPE
case ESTRPIPE:
	return tsc(u8"Streams pipe error");
#endif
#if defined(EOPNOTSUPP) && (!defined(ENOTSUP) || (ENOTSUP != EOPNOTSUPP))
case EOPNOTSUPP:
	return tsc(u8"Operation not supported on socket");
#endif
#ifdef EOVERFLOW
case EOVERFLOW:
	return tsc(u8"Value too large for defined data type");
#endif
#ifdef EMSGSIZE
case EMSGSIZE:
	return tsc(u8"Message too long");
#endif
#ifdef ETIMEDOUT
case ETIMEDOUT:
	return tsc(u8"Connection timed out");
#endif
default:
	return tsc(u8"Unknown");
}
	}