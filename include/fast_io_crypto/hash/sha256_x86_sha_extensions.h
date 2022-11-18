#pragma once


#if defined(__SSE2__) && (!defined(__SHA__) || !defined(__SSSE3__)) && __has_cpp_attribute(__gnu__::__target__) && !defined(__clang__) && defined(__ELF__) && defined(FAST_IO_RUNTIME_DISPATCH)
[[__gnu__::__target__("ssse3,sha")]]
#elif __has_cpp_attribute(__gnu__::__flatten__)
[[__gnu__::__flatten__]]
#endif
inline void sha256_runtime_routine(std::uint_least32_t* __restrict state,std::byte const* __restrict block,std::byte const* __restrict blocks_last) noexcept
{
	constexpr std::size_t block_size{64};
	using ::fast_io::intrinsics::simd_vector;
	simd_vector<int,4> state0st{static_cast<int>(state[5]),static_cast<int>(state[4]),
				static_cast<int>(state[1]),static_cast<int>(state[0])};
	simd_vector<int,4> state1st{static_cast<int>(state[7]),static_cast<int>(state[6]),
				static_cast<int>(state[3]),static_cast<int>(state[2])};
	simd_vector<int,4> state0=state0st;
	simd_vector<int,4> state1=state1st;
	simd_vector<int,4> msg,msg0,msg1,msg2,msg3;
	constexpr int compiler_magic{
#if defined(__clang__)
4		//clang and gcc disagree with __builtin_ia32_palignr128
#else
32
#endif
	};
	for(;block!=blocks_last;block+=block_size)
	{
		/* Rounds 0-3 */
		msg0.load(block);
		msg0.swap_endian();
		{
		//0xE9B5DBA5B5C0FBCFULL, 0x71374491428A2F98ULL
		constexpr simd_vector<int,4> constant{1116352408,1899447441,-1245643825,-373957723};
		msg=wrap_add(msg0,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);

		/* Rounds 4-7 */
		msg1.load(block+16);
		msg1.swap_endian();
		{
		//0xAB1C5ED5923F82A4ULL, 0x59F111F13956C25BULL
		constexpr simd_vector<int,4> constant{961987163,1508970993,-1841331548,-1424204075};
		msg=wrap_add(msg1,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);
		msg0 = __builtin_ia32_sha256msg1(msg0,msg1);

		/* Rounds 8-11 */
		msg2.load(block+32);
		msg2.swap_endian();
		{
			constexpr simd_vector<int,4> constant{-670586216,310598401,607225278,1426881987};
			msg = wrap_add(msg2,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);
		msg1 = __builtin_ia32_sha256msg1(msg1,msg2);

		/* Rounds 12-15 */
		msg3.load(block+48);
		msg3.swap_endian();
		{
			constexpr simd_vector<int,4> constant{1925078388,-2132889090,-1680079193,-1046744716};
			msg = wrap_add(msg3,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg0.wrap_add_assign(__builtin_ia32_palignr128(
					static_cast<simd_vector<long long,2>>(msg3),
					static_cast<simd_vector<long long,2>>(msg2),compiler_magic));
		msg0=__builtin_ia32_sha256msg2(msg0,msg3);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);	
		msg2 = __builtin_ia32_sha256msg1(msg2,msg3);

		/* Rounds 16-19 */
		{
		constexpr simd_vector<int,4> constant{-459576895,-272742522,264347078,604807628};
		msg = wrap_add(msg0,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);

		msg1.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg0),
					static_cast<simd_vector<long long,2>>(msg3),compiler_magic));

		msg1=__builtin_ia32_sha256msg2(msg1,msg0);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);		
		msg3 = __builtin_ia32_sha256msg1(msg3,msg0);

		/* Rounds 20-23 */
		{
			constexpr simd_vector<int,4> constant{770255983,1249150122,1555081692,1996064986};
			msg = wrap_add(msg1,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg2.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg1),
					static_cast<simd_vector<long long,2>>(msg0),compiler_magic));
		msg2=__builtin_ia32_sha256msg2(msg2,msg1);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);		
		msg0 = __builtin_ia32_sha256msg1(msg0,msg1);

		/* Rounds 24-27 */
		{
			constexpr simd_vector<int,4> constant{-1740746414,-1473132947,-1341970488,-1084653625};
			msg = wrap_add(msg2,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg3.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg2),
					static_cast<simd_vector<long long,2>>(msg1),compiler_magic));
		msg3=__builtin_ia32_sha256msg2(msg3,msg2);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);		
		msg1 = __builtin_ia32_sha256msg1(msg1,msg2);

		/* Rounds 28-31 */
		{
			constexpr simd_vector<int,4> constant{-958395405,-710438585,113926993,338241895};
			msg = wrap_add(msg3,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg0.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg3),
					static_cast<simd_vector<long long,2>>(msg2),compiler_magic));
		msg0=__builtin_ia32_sha256msg2(msg0,msg3);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);		
		msg2 = __builtin_ia32_sha256msg1(msg2,msg3);

		/* Rounds 32-35 */
		{
			constexpr simd_vector<int,4> constant{666307205,773529912,1294757372,1396182291};
			msg = wrap_add(msg0,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg1.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg0),
					static_cast<simd_vector<long long,2>>(msg3),compiler_magic));
		msg1=__builtin_ia32_sha256msg2(msg1,msg0);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);		
		msg3 = __builtin_ia32_sha256msg1(msg3,msg0);

		/* Rounds 36-39 */
		{
			constexpr simd_vector<int,4> constant{1695183700,1986661051,-2117940946,-1838011259};
			msg = wrap_add(msg1,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg2.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg1),
					static_cast<simd_vector<long long,2>>(msg0),compiler_magic));
		msg2 = __builtin_ia32_sha256msg2(msg2,msg1);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);
		msg0 = __builtin_ia32_sha256msg1(msg0,msg1);

		/* Rounds 40-43 */
		{
			constexpr simd_vector<int,4> constant{-1564481375,-1474664885,-1035236496,-949202525};
			msg = wrap_add(msg2,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg3.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg2),
					static_cast<simd_vector<long long,2>>(msg1),compiler_magic));
		msg3 = __builtin_ia32_sha256msg2(msg3,msg2);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);
		msg1 = __builtin_ia32_sha256msg1(msg1,msg2);

		/* Rounds 44-47 */
		{
			constexpr simd_vector<int,4> constant{-778901479,-694614492,-200395387,275423344};
			msg = wrap_add(msg3,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg0.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg3),
					static_cast<simd_vector<long long,2>>(msg2),compiler_magic));
		msg0 = __builtin_ia32_sha256msg2(msg0,msg3);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);
		msg2 = __builtin_ia32_sha256msg1(msg2,msg3);

		/* Rounds 48-51 */
		{
			constexpr simd_vector<int,4> constant{430227734,506948616,659060556,883997877};
			msg = wrap_add(msg0,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg1.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg0),
					static_cast<simd_vector<long long,2>>(msg3),compiler_magic));
		msg1 = __builtin_ia32_sha256msg2(msg1,msg0);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);
		msg3 = __builtin_ia32_sha256msg1(msg3,msg0);

		/* Rounds 52-55 */
		{
			constexpr simd_vector<int,4> constant{958139571,1322822218,1537002063,1747873779};
			msg = wrap_add(msg1,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg2.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg1),
					static_cast<simd_vector<long long,2>>(msg0),compiler_magic));
		msg2 = __builtin_ia32_sha256msg2(msg2,msg1);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);

		/* Rounds 56-59 */
		{
			constexpr simd_vector<int,4> constant{1955562222,2024104815,-2067236844,-1933114872};
			msg = wrap_add(msg2,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg3.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg2),
					static_cast<simd_vector<long long,2>>(msg1),compiler_magic));
		msg3 = __builtin_ia32_sha256msg2(msg3,msg2);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);

		/* Rounds 60-63 */
		{
			constexpr simd_vector<int,4> constant{-1866530822,-1538233109,-1090935817,-965641998};
			msg = wrap_add(msg3,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);


		/* Combine state  */
		state0st.wrap_add_assign(state0);
		state0 = state0st;
		state1st.wrap_add_assign(state1);
		state1 = state1st;
	}
	state[0]=static_cast<std::uint_least32_t>(state0[3]);
	state[1]=static_cast<std::uint_least32_t>(state0[2]);
	state[2]=static_cast<std::uint_least32_t>(state1[3]);
	state[3]=static_cast<std::uint_least32_t>(state1[2]);
	state[4]=static_cast<std::uint_least32_t>(state0[1]);
	state[5]=static_cast<std::uint_least32_t>(state0[0]);
	state[6]=static_cast<std::uint_least32_t>(state1[1]);
	state[7]=static_cast<std::uint_least32_t>(state1[0]);
}
