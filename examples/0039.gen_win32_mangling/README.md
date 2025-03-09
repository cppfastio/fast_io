# win32mangling
Generate [manglings table](../../include/fast_io_hosted/platforms/win32) automatically, which is a part of msvc-support.

## Usage
1. First, chdir to the directory of this file

2.
```
clang++ ./prebuild.cc -o prebuild -std=c++2c -I ../../include -O2 -lntdll -fuse-ld=lld -flto=thin
./prebuild $WINDOWS_MSVC_SYSROOT
ninja -C build
```

if you are in developer environment for vs, you should not pass `$WINDOWS_MSVC_SYSROOT`.
