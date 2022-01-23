#pragma once
case 0: return 0;
#if defined(EACCES)
return 5;
#endif
#if defined(EAGAIN)
return 89;
#endif
#if defined(EBADF)
return 6;
#endif
#if defined(EBADRQC)
return 1;
#endif
#if defined(EBUSY)
return 32;
#endif
#if defined(ECHILD)
return 128;
#endif
#if defined(ECOMM)
return 233;
#endif
#if defined(ECONNREFUSED)
return 1225;
#endif
#if defined(EDEADLOCK)
return 1131;
#endif
#if defined(EEXIST)
return 80;
#endif
#if defined(EFAULT)
return 998;
#endif
#if defined(EFBIG)
return 1450;
#endif
#if defined(EINTR)
return 104;
#endif
#if defined(EINVAL)
return 13;
#endif
#if defined(EIO)
return 23;
#endif
#if defined(ELIBBAD)
return 14001;
#endif
#if defined(EMFILE)
return 4;
#endif
#if defined(EMLINK)
return 1142;
#endif
#if defined(EMSGSIZE)
return 234;
#endif
#if defined(ENAMETOOLONG)
return 206;
#endif
#if defined(ENFILE)
return 113;
#endif
#if defined(ENMFILE)
return 18;
#endif
#if defined(ENODATA)
return 38;
#endif
#if defined(ENODEV)
return 15;
#endif
#if defined(ENOENT)
return 2;
#endif
#if defined(ENOEXEC)
return 191;
#endif
#if defined(ENOLCK)
return 36;
#endif
#if defined(ENOLINK)
return 2250;
#endif
#if defined(ENOMEDIUM)
return 21;
#endif
#if defined(ENOMEM)
return 8;
#endif
#if defined(ENONET)
return 51;
#endif
#if defined(ENOSPC)
return 39;
#endif
#if defined(ENOSYS)
return 50;
#endif
#if defined(ENOTDIR)
return 267;
#endif
#if defined(ENOTEMPTY)
return 145;
#endif
#if defined(ENOTSUP)
return 282;
#endif
#if defined(ENOTUNIQ)
return 52;
#endif
#if defined(ENXIO)
return 1006;
#endif
#if defined(EPERM)
return 82;
#endif
#if defined(EPIPE)
return 109;
#endif
#if defined(EROFS)
return 19;
#endif
#if defined(ESRCH)
return 127;
#endif
#if defined(EXDEV)
return 17;
#endif
