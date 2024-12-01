# fast_io

fast_io represents a cutting-edge input/output (I/O) library for C++20, designed to achieve [exceptional speed](readme.md#Benchmarks). Its primary purpose is to supersede the widely used `<iostream>` and `<cstdio>` libraries. This header-only library is distributed under the permissive [MIT license](license.txt), simplifying integration into any project. However, its utilization mandates a C++20 compiler with support for concepts.

*Although the original fast_io repository has been archived due to accumulated git data, historical commits remain accessible.*
*Past commits were stored [here](https://bitbucket.org/ejsvifq_mabmip/fast_io_archive-2022-01-23).*

## Illustrations
### Output to `cstdout`
```cpp
#include <fast_io.h>

int main() 
{
	::fast_io::io::print("Hello, fast_io world!\n");
	::fast_io::io::print(::fast_io::c_stdout(), "Equivalent to the function above\n");
}
```
### Output to `native stdout` 
* on non-windows systems (fd): (int)1
* on win32 (handle): GetStdHandle(STD_OUTPUT_HANDLE)
* on nt (handle): RtlGetCurrentPeb()->ProcessParameters->StandardOutput
```cpp
#include <fast_io.h>

int main() 
{
	::fast_io::io::print(::fast_io::out(), "Hello, fast_io world!\n");z
}
```
### Memory-Mapped File
* Platforms that do not support memory mapping native_file_loader will read files into allocated space instead
```cpp
#include <fast_io.h>

int main() 
{
	::fast_io::native_file_loader file_data{"text.txt"};
	// file_data satisfies ::std::ranges::contiguous_range
}
```
### Scan from `cstdin`
```cpp
#include <fast_io.h>

int main() 
{
	int i;
	::fast_io::io::scan(i);
	// Equivalent to the function above
	::fast_io::io::scan(::fast_io::c_stdin(), i);
}
```
### Scan from `native stdin` 
* Note: scanning will read the data in native stdin into the buffer and clear some data in native stdin
* on non-windows systems (fd): (int)0
* on win32 (handle): GetStdHandle(STD_INPUT_HANDLE)
* on nt (handle): RtlGetCurrentPeb()->ProcessParameters->StandardInput
```cpp
#include <fast_io.h>

int main() 
{
	::fast_io::basic_ibuf<::fast_io::native_io_observer> ibf_in{::fast_io::in()};
	int i;
	::fast_io::io::scan(ibf_in, i);
}
```
### Additional Samples

For up-to-date examples, refer to the [`examples/`](examples/) directories.

Outdated examples are retained [here](https://bitbucket.org/ejsvifq_mabmip/fast_io_deprecated)
though they might no longer be functional.

The i18n repository houses the i18n source files.

[https://bitbucket.org/ejsvifq_mabmip/fast_io_i18n](https://bitbucket.org/ejsvifq_mabmip/fast_io_i18n)

## F.A.Q.
### What does "fast" mean in fast_io?

The term ```fast``` within ```fast_io``` does not strictly denote the absolute fastest I/O library available (which might have prompted the name ```fastest_io```). Instead, it signifies that ```fast_io``` consistently surpasses the performance of both the ```<iostream>``` and ```<cstdio>``` libraries.

### What are the supported platforms, compilers...?

For a comprehensive list of supported elements and platform-specific usage instructions, consult [support.md](support.md).

### Have further questions?
You can ask questions on the [fast_io discord server](https://discord.gg/wYfh8kk)
or the QQ group: `801441303`.

## Documentation
Refer to either https://ewindy.gitee.io/fast_io_rst/index.html or https://gitee.com/qabeowjbtkwb/fast_io/wikis.

## Features
- Utilizing system calls directly, bypassing intermediary layers for enhanced efficiency.
- Embraces Unicode (UTF-8, UTF-16, UTF-32) with codecvt for GB18030 and UTF-EBCDIC (as these aren't properly addressed by libcs).
- Implements RAII for C `FILE*`, POSIX `fd`, and win32/NT `HANDLE`.
- Facilitates interoperation with `<cstdio>` and `<iostream>`.
- Minimizes misuse potential, unlike `std::endl`.
- Employs static I/O manipulators in lieu of format strings.
- Offers optional locale support.
- Implements stateless I/O manipulation.
- Maintains consistent error handling, favoring `exceptions` as the sole error reporting mechanism (excluding `std::error_code`, `std::system_error`, and redundant bounds checking).
- Accommodates freestanding mode.
- Incorporates Address Sanitizer code for memory safety.
- Provides support for Dynamic Instrumentation.
- Supports POSIX iconv for transcoding.
- Provides binary serialization for trivially copyable types and standard containers.
- Integrates seamlessly with C++ containers (e.g., `std::vector<fast_io::obuf_file>` is valid).
- Supplies basic/Lua/Python/etc. formatting (print, scan), omitting C and C++ due to security concerns.
- Native Handle Interface
- Exposes APIs for the internal implementation of `FILE*` and C++ streams.
- Abstains from using traits_type and EOF.
- Encompasses Dynamic Type Support
- Facilitates Multi-Process functionality
- Supports memory mapping
- Features debugging I/O (with optional GUI)
- Employs a round-trip floating-point algorithm
- Supports hash algorithms: intrinsic SHA-1, intrinsic HMAC-SHA1, intrinsic SHA-256, intrinsic HMAC-SHA256, SHA-512, HMAC-SHA512, as well as non-crypto hash algorithms like Jenkins Hash.
- Provides zlib compression/decompression
- Offers Filesystem capabilities
- OpenSSL BIO, Qt QFile, MFC CFile support

### Post C++20
  1. Module support
  2. Coroutine support for asynchronous I/O
  3. Code improvement and refactoring following the integration of [Zero-overhead deterministic exceptions](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0709r0.pdf) into the standard

## Benchmarks

1. I/O for 10 Million Integers

Objective: Output ten million integers from 0 to 10M to a file and subsequently read them back from the same file.

All benchmark details are located in benchmarks/0000.10m_size_t/unit.

Note: I adjusted libstdc++'s BUFSIZ to 1048576 due to MinGW-W64's poor performance with the default BUFSIZ of 512 bytes.
(Update: GCC has rectified this by further increasing the size to 4096)


| Platform                       |        Windows          |MinGW-W64 GCC 11.0.0   |   MSVCRT + libstdc++                                 |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
|                                                                                                                                         |

| Method                         |       Output time       |      Input time       |   Comment                                            |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
| stdio.h(fprintf/fscanf)        |      2.412987s          |   5.607791s           |                                                      |
| fstream                        |      0.462012s          |   1.192s              |                                                      |
| fstream with rdbuf.sputc trick |      0.33895s           |   1.170173s           |                                                      |
| fast_io::i/obuf_file           |      0.04903s           |   0.080996s           |                                                      |
| fast_io::i/obuf_file_mutex     |      0.146064s          |   0.113155s           | thread safe                                          |
| c_locale_i/obuf_file ("C")     |      0.065988s          |   0.086012s           | imbued with locale, locale "C"                       |
| c_locale_i/obuf_file local     |      0.153995s          |   :x:                 | imbued with locale, locale ""                        |
| fmt::format_int+obuf_file      |      0.122999s          |   :x:                 |                                                      |
| fmt::format_int+ofstream       |      0.209055s          |   :x:                 |                                                      |
| fmt::format+ofstream           |      0.548s             |   :x:                 | fmt makes things slower                              |
| fmt::print                     |      0.663996s          |   :x:                 | fmt makes things slower                              |
| std::to_chars+obuf_file        |      0.12s              |   :x:                 |                                                      |
| std::to_chars+ofstream         |      0.192s             |   :x:                 |                                                      |
| fast_io::c_file_unlocked       |      0.098999s          |   0.126003s           | I hacked MSVCRT's FILE* implementation               |
| fast_io::c_file                |      0.298988s          |   0.318001s           | Thread Safe. I hacked MSVCRT's FILE* implementation |
| fast_io::filebuf_file          |      0.048999s          |   0.081s              | I hacked libstdc++'s streambuf/filebuf implementation|
| fast_io::iobuf_utf8_file_char16|      0.124s       |   0.112001s             | UTF-16=>UTF-8 with SSE|
| fast_io::iobuf_utf8_file_char32|      0.110999s       |   0.111011s             | UTF-32=>UTF-8 with SSE|
| std::wofstream             |      2.64s       |   3.843735s             | wofstream with std::locale codecvt. Extremely slow tbh.|
| fast_io::wfilebuf_io_observer  |      2.415692s       |   2.497704s         | wofstream with std::locale codecvt. This proves fstream can never get fixed.|
| Rust language  |      0.483s       | :x:            |  RUST IS SLOW. Also Rust does not deal with locale. Think how bad it is.  |
| Rust itoa library 0.4.6 | > 0.165s | :x: | I ignored the `\n` part for it to ensure no bias. |


Rust language is 10x slower than fast\_io. + binary bloat and itoa library is still extremely slow and usable for me. It is at least 3x slower than fast\_io.


Run the same test on MSVC 19.26.28805.


| Platform                       |       Windows           |  MSVC 19.26.28805     |  Install fmtlib wastes time of my life               |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
|                                                                                                                                         |

| Method                         |       Output time       |      Input time       |   Comment                                            |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
| stdio.h(fprintf/fscanf)        |      1.5353597s         |   1.4157233s          |                                                      |
| fstream                        |      3.6350262s         |   3.8420339s          |                                                      |
| fstream with rdbuf.sputc trick |      3.3735902s         |   3.8145566s          |                                                      |
| fast_io::i/obuf_file           |      0.0631433s         |   0.1030554s          |                                                      |
| fast_io::i/obuf_file_mutex     |      0.2190659s         |   0.2485886s          | thread safe                                          |
| std::to_chars+obuf_file        |      0.1641641s         |   :x:                 |                                                      |
| std::to_chars+ofstream         |      0.5461922s         |   :x:                 |                                                      |
| fast_io::c_file_unlocked       |      0.1102575s         |   0.2399757s          | I hacked Universal CRT's FILE* implementation        |
| fast_io::c_file                |      0.2034755s         |   0.2621148s          | Thread Safe. I hacked UCRT's FILE* implementation    |
| fast_io::filebuf_file          |      0.126661s          |   0.2378803s          | I hacked MSVC STL's streambuf/filebuf implementation |

Run the same test on GCC 11. glibc + libstdc++


| Platform                       |       Linux             |  GCC 11.0.0           |         glibc + libstdc++                            |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
|                                                                                                                                         |

| Method                         |       Output time       |      Input time       |   Comment                                            |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
| stdio.h(fprintf/fscanf)        |      0.532792935s       |   0.591907111s        |                                                      |
| fstream with rdbuf.sputc trick |      0.318896068s       |   0.429406415s        |                                                      |
| fast_io::i/obuf_file           |      0.050300857s       |   0.065372395s        |                                                      |
| fast_io::i/obuf_file_mutex     |      0.05290654s        |   0.083040518s        | thread safe                                          |
| c_locale_i/obuf_file ("C")     |      0.051939052s       |   0.065820056s        | imbued with locale, locale "C"                       |
| c_locale_i/obuf_file local     |      0.162406082s       |   :x:                 | imbued with locale, locale ""                        |
| std::to_chars+obuf_file        |      0.115453587s       |   :x:                 |                                                      |
| fmt::format_int+obuf_file      |      0.1183587s         |   :x:                 |                                                      |
| fmt::format_int+ofstream       |      0.195914384s       |   :x:                 |                                                      |
| fmt::format+ofstream           |      0.633590975s       |   :x:                 | fmt makes things slower                              |
| fmt::print                     |      0.495270371s       |   :x:                 | fmt makes things slower                              |
| boost::iostreams               |      0.400906063s       |   0.444717051s        | Using boost iostreams does not make your code faster |
| fast_io::c_file_unlocked       |      0.060076723s       |   0.073299716s        | I hacked glibc's FILE* implementation                |
| fast_io::c_file                |      0.092490191s       |   0.104545535s        | Thread Safe. I hacked glibc's FILE* implementation   |
| fast_io::filebuf_file          |      0.052251608s       |   0.06655806s         | I hacked libstdc++'s streambuf/filebuf implementation|

You can see fast_io can also boost the performance of existing facilities for 10x! Yes, it can even improve FILE* and fstream's performance for 10x depending on platforms since I use concepts to abstract them all. fmtlib actually slows down I/O performance.

2. Output 10M double in round-trip mode with Ryu algorithm

We only perform this test for MSVC since only msvc's charconv implements it. Yes. fast_io defeats msvc's charconv for over 20% for running the same algorithm.

All benchmarks are in benchmarks/0001.10m_double/charconv.

Run the same test on MSVC 19.26.28805.


| Platform                       |       Windows           |  MSVC 19.26.28805     |                  |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
|                                                                                                                                         |

| Method                         |       Output time       |                           Comment                                            |
|--------------------------------|-------------------------|------------------------------------------------------------------------------|
| i/obuf_file                    |      0.4653818s         |                                                                              |
| charconv + obuf_file           |      0.6011s            |                                                                              |

3. Raw I/O Performance

All benchmarks are in benchmarks/0014.file_io/file_io.

Output 100000000x "Hello World\n"

Notice: I modified libstdc++'s std::filebuf's BUFSIZ to 1048576 due to BUFSIZ is too small (512 bytes) for MinGW-W64 or it performs horribly.



| Platform                       |        Windows          |MinGW-W64 GCC 11.0.0   |   MSVCRT + libstdc++                                 |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
|                                                                                                                                         |

| Method                         |       Output time       |                           Comment                                            |
|--------------------------------|-------------------------|------------------------------------------------------------------------------|
| fwrite                         |      2.524001s          |                                                                              |
| fstream                        |      1.013001s          |                                                                              |
| fast_io::obuf_file             |      0.437998s          |                                                                              |
| fast_io::obuf_file_mutex       |      1.371s             |Thread safe                                                                   |
| fast_io::c_file_unlocked       |      1.164997s          |I hacked MSVCRT's FILE* implementation                                        |
| fast_io::c_file                |      3.337945s          |Thread Safe. I hacked MSVCRT's FILE* implementation. Need further optimization|
| fast_io::filebuf_file          |      0.467001s          |I hacked libstdc++'s std::filebuf implementation                              |


| Platform                       |        Linux            |          GCC 11.0.0   |     glibc + libstdc++                                |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
|                                                                                                                                         |

| Method                         |       Output time       |                           Comment                                            |
|--------------------------------|-------------------------|------------------------------------------------------------------------------|
| fwrite                         |      1.457288317s       |                                                                              |
| fstream                        |      1.249783346s       |                                                                              |
| fast_io::obuf_file             |      0.494827134s       |                                                                              |
| fast_io::obuf_file_mutex       |      0.497138826s       |Thread safe                                                                   |
| fast_io::c_file_unlocked       |      0.687976666s       |I hacked glibc's FILE* implementation                                         |
| fast_io::c_file                |      0.910792697s       |Thread Safe. I hacked glibc's FILE* implementation                            |
| fast_io::filebuf_file          |      0.526955039s       |I hacked libstdc++'s std::filebuf implementation                              |


| Platform                       |        Windows          |  MSVC 19.26.28805     |      UCRT + MSVC STL                                 |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
|                                                                                                                                         |

| Method                         |       Output time       |                           Comment                                            |
|--------------------------------|-------------------------|------------------------------------------------------------------------------|
| fwrite                         |      3.3139122s         |                                                                              |
| fstream                        |      1.7184119s         |                                                                              |
| fast_io::obuf_file             |      0.7996034s         |                                                                              |
| fast_io::obuf_file_mutex       |      2.2949221s         |Thread safe. It looks like std::mutex is horribly slow for MSVC STL.          |
| fast_io::c_file_unlocked       |      1.2103924s         |I hacked UCRT's FILE* implementation                                          |
| fast_io::c_file                |      2.3604295s         |Thread Safe. I hacked UCRT's FILE* implementation                             |
| fast_io::filebuf_file          |      1.2805368s         |I hacked MSVC STL's std::filebuf implementation                               |


4. Binary Size
Just use the benchmark in benchmarks/0014.file_io/file_io.
Dude, you should avoid stream as plague tbh. It is not healthy.


| Platform                       |        Windows          |MinGW-W64 GCC 11.0.0   |   MSVCRT + libstdc++ + static compile                |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
|                                                                                                                                         |

| Method                         |      Binary Size        |                           Comment                                            |
|--------------------------------|-------------------------|------------------------------------------------------------------------------|
| fstream                        |      925KB              |Use fstream is not good for your health since std::locale bloats your binary. |
| fast_io::obuf_file             |      155KB              |                                                                              |
| fast_io::c_file_unlocked       |      157KB              |I hacked MSVCRT's FILE* implementation                                        |
| fast_io::c_file                |      157KB              |Thread Safe. I hacked MSVCRT's FILE* implementation                           |
| fast_io::filebuf_file          |      933KB              |I hacked libstdc++'s std::filebuf implementation. C++ stream sucks            |


5. Code Convert

Generate 100000000 🚄 emojis by using the program in benchmarks/0020.utf/fill_nc.cc

Benchmarks are in examples/0043.iconv
Universal iconv. (UTF-8 to GB18030 as an example)
iconv test:
| Platform                       |        Windows          |MinGW-W64 GCC 11.0.0   |   MSVCRT + libstdc++                                 |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
|                                                                                                                                         |

| Method                         |       Elapsed time      |                           Comment                                            |
|--------------------------------|-------------------------|------------------------------------------------------------------------------|
| iconv command                  |      1.529s             |                                                                              |
| universal.cc                   |      1.293s             |  use POSIX libiconv                                                        |


UTF8->UTF16LE

Benchmarks are in examples/0022.utf

iconv test:
| Platform                       |        Windows          |MinGW-W64 GCC 11.0.0   |   MSVCRT + libstdc++                                 |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
|                                                                                                                                         |

| Method                         |       Elapsed time      |                           Comment                                            |
|--------------------------------|-------------------------|------------------------------------------------------------------------------|
| iconv command                  |      0.967s             |  GNU iconv. No BOM which sucks                                              |
| utf8_file_to_utf16_file.cc     |      0.498s             |  I use the SSE algorithms provided by the utf-utils project.                |

UTF8->UTF32LE

Benchmarks are in examples/0022.utf

iconv test:
| Platform                       |        Windows          |MinGW-W64 GCC 11.0.0   |   MSVCRT + libstdc++                                 |
|--------------------------------|-------------------------|-----------------------|------------------------------------------------------|
|                                                                                                                                         |

| Method                         |       Elapsed time      |                           Comment                                            |
|--------------------------------|-------------------------|------------------------------------------------------------------------------|
| iconv command                  |      0.844s             |  GNU iconv. No BOM which sucks                                           |
| utf8_file_to_utf32_file.cc     |      0.442s             |  I use the SSE algorithms provided by the utf-utils project.                   |

## Credits

The creation and development of this project were made possible thanks to the valuable contributions of various open-source projects. While the code was not copied directly from these projects, I used them as references and re-implemented them to suit the specific purposes of this library. In some cases, integration issues arose that required modifications to the original code. I am grateful to these projects and their developers for their commitment to making their code open and accessible to the wider community.

|Project          | Url
|-----------------|-----------------------------------------
|Grisu-Exact      |https://github.com/jk-jeon/Grisu-Exact
|Ryu              |https://github.com/ulfjack/ryu
|SHA-Intrinsics   |https://github.com/noloader/SHA-Intrinsics
|SHA1             |https://github.com/vog/sha1
|UTF-utils        |https://github.com/BobSteagall/utf_utils
|jenkins-hash-java|https://github.com/vkandy/jenkins-hash-java
|md5              |https://github.com/JieweiWei/md5
|ReactOS          |https://github.com/reactos/reactos
|dirent_h         |https://github.com/win32ports/dirent_h
|GNU C library    |https://www.gnu.org/software/libc/
|GNU Newlib       |https://sourceware.org/newlib/
|Dragonbox        |https://github.com/jk-jeon/dragonbox
|JEAIII           |https://github.com/jeaiii/itoa
|Crypto++         |https://github.com/weidai11/cryptopp
|MyItoA           |https://gitee.com/xjkp2283572185/MyStd
