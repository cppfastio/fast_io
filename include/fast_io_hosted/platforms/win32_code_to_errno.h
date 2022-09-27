#pragma once
case 0: return 0;
#if defined(EACCES)
case 5:return EACCES;
#endif
#if defined(EAGAIN)
case 89:case 164:case 996:case 997:case 1451:case 1452:case 1453:case 1454:case 1455:case 2401:case 2402:case 2404:return EAGAIN;
#endif
#if defined(EBADF)
case 6:return EBADF;
#endif
#if defined(EBADRQC)
case 1:return EBADRQC;
#endif
#if defined(EBUSY)
case 32:case 33:case 129:case 162:case 170:case 231:case 535:case 1053:case 1460:return EBUSY;
#endif
#if defined(ECHILD)
case 128:return ECHILD;
#endif
#if defined(ECOMM)
case 233:case 536:return ECOMM;
#endif
#if defined(ECONNREFUSED)
case 1225:return ECONNREFUSED;
#endif
#if defined(EDEADLOCK)
case 1131:return EDEADLOCK;
#endif
#if defined(EEXIST)
case 80:case 183:case 1392:return EEXIST;
#endif
#if defined(EFAULT)
case 998:case 1067:return EFAULT;
#endif
#if defined(EFBIG)
case 1450:return EFBIG;
#endif
#if defined(EINTR)
case 104:case 1223:return EINTR;
#endif
#if defined(EINVAL)
case 13:case 25:case 27:case 87:case 131:case 208:case 209:case 210:case 230:case 254:case 255:case 424:case 487:case 1008:case 1332:case 2202:return EINVAL;
#endif
#if defined(EIO)
case 23:case 59:case 110:case 156:case 205:case 1101:case 1102:case 1103:case 1104:case 1106:case 1111:case 1117:case 1129:case 1165:case 1166:case 1393:case 1816:case 8341:return EIO;
#endif
#if defined(ELIBBAD)
case 14001:return ELIBBAD;
#endif
#if defined(EMFILE)
case 4:return EMFILE;
#endif
#if defined(EMLINK)
case 1142:return EMLINK;
#endif
#if defined(EMSGSIZE)
case 234:return EMSGSIZE;
#endif
#if defined(ENAMETOOLONG)
case 206:return ENAMETOOLONG;
#endif
#if defined(ENFILE)
case 113:return ENFILE;
#endif
#if defined(ENMFILE)
case 18:case 259:return ENMFILE;
#endif
#if defined(ENODATA)
case 38:return ENODATA;
#endif
#if defined(ENODEV)
case 15:case 20:case 1200:return ENODEV;
#endif
#if defined(ENOENT)
case 2:case 3:case 53:case 55:case 64:case 67:case 123:case 126:case 161:return ENOENT;
#endif
#if defined(ENOEXEC)
case 191:case 192:case 193:case 197:case 216:return ENOEXEC;
#endif
#if defined(ENOLCK)
case 36:return ENOLCK;
#endif
#if defined(ENOLINK)
case 2250:return ENOLINK;
#endif
#if defined(ENOMEDIUM)
case 21:case 1112:return ENOMEDIUM;
#endif
#if defined(ENOMEM)
case 8:case 14:return ENOMEM;
#endif
#if defined(ENONET)
case 51:return ENONET;
#endif
#if defined(ENOSPC)
case 39:case 112:case 275:case 277:case 1100:return ENOSPC;
#endif
#if defined(ENOSYS)
case 50:case 58:case 120:return ENOSYS;
#endif
#if defined(ENOTDIR)
case 267:return ENOTDIR;
#endif
#if defined(ENOTEMPTY)
case 145:return ENOTEMPTY;
#endif
#if defined(ENOTSUP)
case 282:return ENOTSUP;
#endif
#if defined(ENOTUNIQ)
case 52:return ENOTUNIQ;
#endif
#if defined(ENXIO)
case 1006:return ENXIO;
#endif
#if defined(EPERM)
case 82:case 288:case 1314:return EPERM;
#endif
#if defined(EPIPE)
case 109:case 232:return EPIPE;
#endif
#if defined(EROFS)
case 19:return EROFS;
#endif
#if defined(ESRCH)
case 127:return ESRCH;
#endif
#if defined(EXDEV)
case 17:return EXDEV;
#endif
