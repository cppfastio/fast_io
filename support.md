## Compiler Support
- GCC >= 12
- VS >= 19.29.29917
- Clang >= 15
- Winegcc

## C++ standard library support
- GCC libstdc++
- LLVM libc++
- MSVC STL

## Architecture Support
- alpha
- i686
- x86_64
- arm
- aarch64
- riscv
- riscv64
- powerpc64
- wasm32-wasi
- mips-abi32
- mips-abin32
- mips-abi64
- avr
- ia64
- All other architectures should work, they just aren't tested due to time not being infinite.

## Platform Support
- Windows 10/11
- Windows Server 2019
- Linux
- BSD platforms (Including FreeBSD, NetBSD, OpenBSD)
- Apple Darwin (MacOS)
- Apple IOS
- Microsoft DOS
- FreeDOS
- Barebone (No any operating system.)
- Cygwin/MSYS2
- Android
- Wine
- Wine GCC
- ReactOS
- Windows Runtime
- Nintendo DS (devkitPro)
- Webassembly
- Managarm
- Vinix
- limine Bootloader
- AVR
- Banana
- LemonOS
- LLVM `raw_fd_ostream`
- Windows Kernel Driver
- Linux Kernel Module
- wasm2lua (Compile fast_io to Lua!)
- Any other platform (if you want me to support it, tell me, including your own OS)

## Kernel Driver Support
- Windows NT kernel Driver
- Linux kernel Module

## Legacy Windows Version Support
**You need define `_WIN32_WINNT` and `_WIN32_WINDOWS` version by yourself.**
- Windows 95
- Windows 95 Plus!
- Windows Nashville
- Microsoft Memphis
- Windows 98 FE
- Windows 98 SE
- Windows ME
- Microsoft Neptune
- Windows NT
- Windows Odyssey
- Windows 2000
- Windows Whistler
- Windows XP
- Windows Server 2003
- Windows Longhorn
- Windows Vista
- Windows Server 2008
- Windows Vienna
- Windows 7
- Windows 8
- Windows 8.1
- Windows Home Server 2011
- Windows Server 2012
- Windows Server 2016

## Windows 95 or 95 Plus! Support
You can enable `-O3` but not not `-Ofast`.
You also need to install `msvcrt.dll`.

## Legacy Windows Version Support with DJGPP toolchain
- Windows 1.0
- Windows 2.0
- Windows 3.0
- Windows 3.1
- Microsoft Chicago
- Windows 95
- Windows 95 Plus!
- Microsoft Memphis
- Windows 98 FE
- Windows 98 SE
- Windows ME

## Supported libc FILE* hacks platforms
- glibc
- MSVCRT
- Universal CRT
- Universal CRT with MSVC debugging
- Wine's Emulated MSVCRT
- Wine's Emulated UCRT
- ReactOS Emulated MSVCRT
- ReactOS Emulated UCRT
- BSD libc/Apple Darwin
- MUSL libc
- DJGPP
- Newlib
- Bionic(Android)
- uclibc-ng
- wasi libc
- mlibc (this is a new libc which still contains a lot of bugs currently)
- avrlibc
- picolibc

## Supported C++ standard library `std::streambuf`/`std::filebuf` hacks platforms
- GCC libstdc++
- LLVM libc++
- MSVC STL

## Remote Debugger Support
- WinDbg
- Visual Studio Debugger
- DbgViewer
- Android ADB

## Special Support
- LLVM Address Sanitizer
- Full freestanding support (which means we do not have any headers besides freestanding headers. Tested with GCC's --disable-hosted-libstdcxx without any libc, including newlib)
