# fast_io

fast_io is a C++20 input/output library that provides [exceptional speed](readme.md#Benchmarks) and is designed to replace the commonly used `<iostream>` and `<cstdio>` libraries. It is a header-only library and is licensed under the [MIT license](license.txt), making it easy to include in any project. However, it requires a C++20 compiler that supports concepts.

*The original repository for fast_io has been archived due to git bloat over time, but older commits can still be found there.*
*old commits were [here](https://bitbucket.org/ejsvifq_mabmip/fast_io_archive-2022-01-23).*

## Examples
### Print to `stdout`
```cpp
#include <fast_io.h>

int main() {
	print("Hello, fast_io world!\n");
}
```
### Memory map file
```cpp
#include <fast_io.h>

int main() {
	fast_io::native_file_loader file_data("text.txt");
	// file_data satisfies std::ranges::contiguous_range
}
```
### Other examples

For other up-to-date examples, please look in the [`examples/`](examples/) folder.

Deprecated examples are [here](https://bitbucket.org/ejsvifq_mabmip/fast_io_deprecated)
but they might not work anymore.

This i18n repo stores the i18n source files.

[https://bitbucket.org/ejsvifq_mabmip/fast_io_i18n](https://bitbucket.org/ejsvifq_mabmip/fast_io_i18n)

## F.A.Q.
### What does "fast" mean in fast_io?

The term ```fast``` in ```fast_io``` does not necessarily imply that it is the fastest input/output library available (as it would have been named as ```fastest_io``` instead). Instead, the term refers to the fact that ```fast_io``` is notably faster than ```<iostream>``` and ```<cstdio>``` libraries in all cases.

### What are the supported platforms, compilers...?
See [support.md](support.md) for a list of supported things and platform specific usage notes.

### Other questions?
You can ask questions on the [fast_io discord server](https://discord.gg/wYfh8kk)
or QQ group: `801441303`.

## Documentation
See either https://ewindy.gitee.io/fast_io_rst/index.html or https://gitee.com/qabeowjbtkwb/fast_io/wikis.

## Features
- As close to system call as possible.
- Unicode support (UTF-8, UTF-16, UTF-32) + codecvt for GB18030 and UTF-EBCDIC (no libc correctly deals with it)
- RAII for C `FILE*`, POSIX `fd` and win32/NT `HANDLE`
- Interop with `<cstdio>` and `<iostream>` 
- No easily misused stuff like `std::endl`
- Static I/O manipulators instead of format strings.
- Optional locale.
- Stateless I/O manipulation.
- Consistent error handling; when available, exceptions as the **only** error reporting mechanism (no `std::error_code`, `std::system_error` or useless bounds checking)
- Freestanding mode.
- Address Sanitizer special code for protecting memory safety issues.
- Dynamic Instrumentation Support

- Supports POSIX iconv. You can use fast_io for your coding conversion.
- Binary serialization for trivially copyable types and standard containers
- Plays well with C++ containers (e.g. `std::vector<fast_io::obuf_file>` is valid)
- Basic/Lua/Python/etc format (print, scan). No support for C and C++ since they are security hazards.
- Provide APIs to expose the internal implementation of `FILE*` and C++ streams.

- Native Handle Interface
- Extremely easy to support custom devices
- C and C++ style compile time open mode parsing.
- No `traits_type` and `EOF`
- Dynamic Type Support
- Multi Process
- Memory mapping
- debugging IO (optionally with GUI)
- Round-trip floating point algorithm
- Hash algorithms support: intrinsic SHA-1, intrinsic HMAC-SHA1, intrinsic SHA-256, intrinsic HMAC-SHA256, SHA-512, HMAC-SHA512 as well as non crypto hash algorithms, such as Jenkins Hash.
- zlib compression/decompression
- Filesystem
- OpenSSL BIO, Qt QFile, MFC CFile support

### Post C++20
  1. Module support
  2. Coroutine support for async IO
  3. Improve and refactor code once [Zero-overhead deterministic exceptions](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0709r0.pdf) are added to the standard

## Benchmarks

1. I/O 10M integers

Goal: Print out ten million integers from 0 to 10M to file. Then reopen that file to scan back.

All benchmarks are in benchmarks/0000.10m_size_t/unit.

Notice: I modified libstdc++'s BUFSIZ 1048576 due to BUFSIZ is too small (512 bytes) for MinGW-W64, or it performs horribly.


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
