#pragma once
case 0: return 0;
#if defined(EACCES)
case EACCES: return 5;
#endif
#if defined(EAGAIN)
case EAGAIN: return 89;
#endif
#if defined(EBADF)
case EBADF: return 6;
#endif
#if defined(EBADRQC)
case EBADRQC: return 1;
#endif
#if defined(EBUSY)
case EBUSY: return 32;
#endif
#if defined(ECHILD)
case ECHILD: return 128;
#endif
#if defined(ECOMM)
case ECOMM: return 233;
#endif
#if defined(ECONNREFUSED)
case ECONNREFUSED: return 1225;
#endif
#if defined(EDEADLOCK)
case EDEADLOCK: return 1131;
#endif
#if defined(EEXIST)
case EEXIST: return 80;
#endif
#if defined(EFAULT)
case EFAULT: return 998;
#endif
#if defined(EFBIG)
case EFBIG: return 1450;
#endif
#if defined(EINTR)
case EINTR: return 104;
#endif
#if defined(EINVAL)
case EINVAL: return 13;
#endif
#if defined(EIO)
case EIO: return 23;
#endif
#if defined(ELIBBAD)
case ELIBBAD: return 14001;
#endif
#if defined(EMFILE)
case EMFILE: return 4;
#endif
#if defined(EMLINK)
case EMLINK: return 1142;
#endif
#if defined(EMSGSIZE)
case EMSGSIZE: return 234;
#endif
#if defined(ENAMETOOLONG)
case ENAMETOOLONG: return 206;
#endif
#if defined(ENFILE)
case ENFILE: return 113;
#endif
#if defined(ENMFILE)
case ENMFILE: return 18;
#endif
#if defined(ENODATA)
case ENODATA: return 38;
#endif
#if defined(ENODEV)
case ENODEV: return 15;
#endif
#if defined(ENOENT)
case ENOENT: return 2;
#endif
#if defined(ENOEXEC)
case ENOEXEC: return 191;
#endif
#if defined(ENOLCK)
case ENOLCK: return 36;
#endif
#if defined(ENOLINK)
case ENOLINK: return 2250;
#endif
#if defined(ENOMEDIUM)
case ENOMEDIUM: return 21;
#endif
#if defined(ENOMEM)
case ENOMEM: return 8;
#endif
#if defined(ENONET)
case ENONET: return 51;
#endif
#if defined(ENOSPC)
case ENOSPC: return 39;
#endif
#if defined(ENOSYS)
case ENOSYS: return 50;
#endif
#if defined(ENOTDIR)
case ENOTDIR: return 267;
#endif
#if defined(ENOTEMPTY)
case ENOTEMPTY: return 145;
#endif
#if defined(ENOTSUP)
case ENOTSUP: return 282;
#endif
#if defined(ENOTUNIQ)
case ENOTUNIQ: return 52;
#endif
#if defined(ENXIO)
case ENXIO: return 1006;
#endif
#if defined(EPERM)
case EPERM: return 82;
#endif
#if defined(EPIPE)
case EPIPE: return 109;
#endif
#if defined(EROFS)
case EROFS: return 19;
#endif
#if defined(ESRCH)
case ESRCH: return 127;
#endif
#if defined(EXDEV)
case EXDEV: return 17;
#endif
