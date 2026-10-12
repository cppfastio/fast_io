# curve25519 benchmark results

x86_64, `rdtscp` cycles, min of 25 batches x 300 iters (`bench_common.h`).

| op | fast_io | openssl | cryptopp | dalek | mehdi |
|---|---|---|---|---|---|
| ed25519 keypair | **37k** | 132k | 51k | 52-61k | 44k |
| ed25519 sign | **40k** | 132k | 55k | 56k | 47k |
| ed25519 verify | 172k | 361k | 150k | **124k** | 205k |
| x25519 pubkey | 131k | 127k | 127k | 50-60k | 164k |
| x25519 pub (fast) | **37k** | — | — | 50k | 44k |
| x25519 shared | 131k | **113k** | 127k | 167-202k | 155k |

i686 (mingw-w64 clang, wine), u32 limbs:

| op | fast_io u32 |
|---|---|
| ed25519 keypair | ~157k |
| ed25519 sign | ~168k |
| ed25519 verify | ~745k |
| x25519 pubkey | ~555k |
| x25519 pub (fast) | ~154k |
| x25519 shared | ~555k |

## What each implementation actually is

- **openssl**: `EVP` ed25519 (portable ge25519) + `x25519-x86_64` asm — 5x51 redundant limbs, `mulx`/`adcx`/`adox` dual carry chains.
- **cryptopp**: wraps Andrew Moon's `ed25519-donna`/`curve25519-donna` (`donna_64.cpp`), 5x51 C. Same ladder for pubkey and shared — no fixed-base path.
- **dalek**: curve25519-dalek 4.1.3. Default build auto-selects the SIMD backend (AVX2/IFMA) — hence verify 124k vs 185k serial. x25519 pubkey uses `mul_base_clamped` (fixed-base Edwards table, same trick as our "fast"); `diffie_hellman` uses the real variable-base Montgomery ladder.
- **mehdi**: Mehdi Sotoodeh's reference — C orchestration + amd64 asm field primitives (`USE_ASM_LIB`).
- **fast_io**: 4x64 saturated limbs, `addc`/`subc`/`umul` intrinsics, fully unrolled carry chains. u32 (8x32) variant hand-unrolled for 32-bit targets.

## pubkey vs pub (fast)

- `calculate_public_key_to_ptr`: generic XZ Montgomery ladder — works for any base point, required for shared-secret DH. ~254 `montgomery_curve_mont` steps.
- `calculate_public_key_fast_to_ptr`: fixed-base Edwards comb over the precomputed fold tables (scalar blinded: `(sk+bl)*B + bp = sk*B`), then Edwards y -> Montgomery u = (1+y)/(1-y). ~3.5x faster; base-point only.

Dalek has the same split (its pubkey uses the fixed-base table, hence 50k); cryptopp and openssl do not.

## Where we win / lose

- We have the fastest scalar field arithmetic: keypair/sign/x25519 all beat every portable implementation, including mehdi's own asm (155k vs our 131k on shared).
- **verify**: donna (150k) and dalek-SIMD (124k) beat us on *algorithm* — precomputed-table multi-scalar `ge25519` mult vs our plain bit-scan in `verify.h`. Not a field-arithmetic problem.
- **x25519 shared**: only openssl beats us (113k) — hand-tuned 5x51 `mulx`/`adcx`/`adox` asm. Representation + ISA, not code quality.
- The residual ~3% vs donna on shared (131 vs 127k) is 5x51's independent `u128` accumulator chains vs our serial `adc` chains, plus donna hardcoding the 3 trailing always-zero clamped bits as pure doublings.

## Compiler notes: carry-flag handling (to be reported to LLVM)

Both GCC and Clang have optimization issues around carry chains:

- `addc`/`subc` lower to `adc`/`sbb` only inside a basic block. **Across loop iterations / BB boundaries the carry flag is materialized to a byte register** (`setb` + `movb` + `addb`) and reconstructed — each "addc" costs 3-4 uops instead of one `adcl`. LLVM has no flag liveness modeling in IR, so SelectionDAG/scheduling cannot keep EFLAGS alive across the back-edge.
- Measured on i686: the loop-based `field_number_square` (~205 insns, carry materialized per iteration) ran **~1.5x slower** end-to-end than the hand-unrolled variant (398 straight-line insns, pure `mull`/`adcl` chains). The loop version is retained in `field.h` under `if constexpr(false)` for comparison.
- Same reason all u32 limbs paths are hand-unrolled: constant-trip loops would be unrolled anyway by LLVM at -O3, but spelling them out keeps the carry in CF deterministically across compilers (including MSVC, which does not unroll them).
- Dead-carry cleanup matters: `addc(x, y, false, carry)` frees the flag dependency so chain heads emit `add` instead of `adc`; a shared `bool carry` with `FAST_IO_INDETERMINATE` avoids the redundant init LLVM otherwise emits per block.
