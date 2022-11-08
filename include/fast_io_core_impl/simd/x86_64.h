#pragma once

namespace fast_io::intrinsics
{
#if __has_cpp_attribute(__gnu__::__vector_size__)
using x86_64_m128 [[__gnu__::__vector_size__(16),__gnu__::__may_alias__]] = float;
using x86_64_m128d [[__gnu__::__vector_size__(16),__gnu__::__may_alias__]] = double;
using x86_64_m128i [[__gnu__::__vector_size__ (16),__gnu__::__may_alias__]] = long long;
using x86_64_v16qi [[__gnu__::__vector_size__ (16)]] = char;
using x86_64_v16qs [[__gnu__::__vector_size__ (16)]] = char signed;
using x86_64_v16qu [[__gnu__::__vector_size__ (16)]] = char unsigned;
using x86_64_v8hi [[__gnu__::__vector_size__ (16)]] = short;
using x86_64_v8hu [[__gnu__::__vector_size__ (16)]] = short unsigned;
using x86_64_v4si [[__gnu__::__vector_size__ (16)]] = int;
using x86_64_v4su [[__gnu__::__vector_size__ (16)]] = int unsigned;

using x86_64_m256 [[__gnu__::__vector_size__(32),__gnu__::__may_alias__]]  = float;
using x86_64_m256i [[__gnu__::__vector_size__ (32),__gnu__::__may_alias__]] = long long;
using x86_64_m256d [[__gnu__::__vector_size__(32),__gnu__::__may_alias__]] = double;
using x86_64_m512 [[__gnu__::__vector_size__(64),__gnu__::__may_alias__]]  = float;
using x86_64_m512i [[__gnu__::__vector_size__ (64),__gnu__::__may_alias__]] = long long;
using x86_64_m512d [[__gnu__::__vector_size__(64),__gnu__::__may_alias__]] = double;
#elif (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
using x86_64_m128 = __m128;
using x86_64_m128i = __m128i;
using x86_64_m128d = __m128d;
using x86_64_m256 = __m256;
using x86_64_m256i = __m256i;
using x86_64_m256d = __m256d;
using x86_64_m512 = __m512;
using x86_64_m512i = __m512i;
using x86_64_m512d = __m512d;

#endif
}
