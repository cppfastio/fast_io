#!/usr/bin/env python3
# Generate da_cache.bin for fast_io.floating module.
#
# Layout (all little-endian):
#   [0x0000, 0x0800)  exponent_shifts: 2048 x u8
#   [0x0800, end)     pow10_cache:     618 x {hi: u64, lo: u64}  (9888 bytes)
#
# pow10_cache[i] is the normalized 128-bit *lower* endpoint of 10^q,
# q = i - 293, i.e. floor(10^q * 2^s) with s chosen so the value's
# top bit is bit 127.
#
# The table is produced with the same compressed-seed construction as the
# reference implementation and then independently re-proved against exact
# bignum arithmetic for every entry.

import struct, sys, os

MASK = (1 << 64) - 1

# ---------------------------------------------------------------------------
# seeds (normalized factors), identical to fast_io::details::da constants
# ---------------------------------------------------------------------------
power10_minor = [
    0x8000000000000000, 0xa000000000000000, 0xc800000000000000, 0xfa00000000000000,
    0x9c40000000000000, 0xc350000000000000, 0xf424000000000000, 0x9896800000000000,
    0xbebc200000000000, 0xee6b280000000000, 0x9502f90000000000, 0xba43b74000000000,
    0xe8d4a51000000000, 0x9184e72a00000000, 0xb5e620f480000000, 0xe35fa931a0000000,
    0x8e1bc9bf04000000, 0xb1a2bc2ec5000000, 0xde0b6b3a76400000, 0x8ac7230489e80000,
    0xad78ebc5ac620000, 0xd8d726b7177a8000, 0x878678326eac9000, 0xa968163f0a57b400,
    0xd3c21bcecceda100, 0x84595161401484a0, 0xa56fa5b99019a5c8, 0xcecb8f27f4200f3a,
]

power10_major = [
    (0xaf8e5410288e1b6f, 0x07ecf0ae5ee44dda),
    (0xb1442798f49ffb4a, 0x99cd11cfdf41779d),
    (0xb2fe3f0b8599ef07, 0x861fa7e6dcb4aa15),
    (0xb4bca50b065abe63, 0x0fed077a756b53aa),
    (0xb67f6455292cbf08, 0x1a3bc84c17b1d543),
    (0xb84687c269ef3bfb, 0x3d5d514f40eea742),
    (0xba121a4650e4ddeb, 0x92f34d62616ce413),
    (0xbbe226efb628afea, 0x890489f70a55368c),
    (0xbdb6b8e905cb600f, 0x5400e987bbc1c921),
    (0xbf8fdb78849a5f96, 0xde98520472bdd034),
    (0xc16d9a0095928a27, 0x75b7053c0f178294),
    (0xc350000000000000, 0x0000000000000000),
    (0xc5371912364ce305, 0x6c28000000000000),
    (0xc722f0ef9d80aad6, 0x424d3ad2b7b97ef6),
    (0xc913936dd571c84c, 0x03bc3a19cd1e38ea),
    (0xcb090c8001ab551c, 0x5cadf5bfd3072cc6),
    (0xcd036837130890a1, 0x36dba887c37a8c10),
    (0xcf02b2c21207ef2e, 0x94f967e45e03f4bc),
    (0xd106f86e69d785c7, 0xe13336d701beba52),
    (0xd31045a8341ca07c, 0x1ede48111209a051),
    (0xd51ea6fa85785631, 0x552a74227f3ea566),
    (0xd732290fbacaf133, 0xa97c177947ad4096),
    (0xd94ad8b1c7380874, 0x18375281ae7822bc),
]

power10_fixups = [
    0x0a4e363f, 0x00001840, 0x00006400, 0x24200040,
    0x00000000, 0x0c000000, 0x82c81380, 0x5e4ce01f,
    0xd730f60f, 0x0000001b, 0x00000000, 0xcdf7fffc,
    0x6e8201d8, 0x40cd3fd1, 0xdb642501, 0x00000d0d,
    0x14042400, 0x53713840, 0x11781db4, 0x00000000,
]

POW10_SIZE = 618
POW10_MIN_EXP = -293          # q = index + POW10_MIN_EXP
EXTRA_SHIFT = 6
B64_EXP_OFFSET = 1075

assert len(power10_minor) == 28
assert len(power10_major) == 23
assert len(power10_fixups) == 20


def umulh(a, b):
    return (a * b) >> 64


def compute_seed(i):
    """Reference compressed-seed construction of entry i."""
    minor = power10_minor[(i + 10) % 28]
    mhi, mlo = power10_major[(i + 10) // 28]
    h1 = umulh(mlo, minor)
    c0 = (mlo * minor) & MASK
    prod = mhi * minor
    plo, phi = prod & MASK, prod >> 64
    c1 = (h1 + plo) & MASK
    c2 = (phi + (1 if c1 < h1 else 0)) & MASK
    if c2 >> 63:
        hi, lo = c2, c1
    else:
        hi = ((c2 << 1) | (c1 >> 63)) & MASK
        lo = ((c1 << 1) | (c0 >> 63)) & MASK
    lo = (lo - ((power10_fixups[i >> 5] >> (i & 31)) & 1)) & MASK
    return hi, lo


def compute_exact(i):
    """Independent exact-arithmetic construction: floor(10^q * 2^s) normalized."""
    q = i + POW10_MIN_EXP
    if q >= 0:
        x_num, x_den = 10 ** q, 1
    else:
        x_num, x_den = 1, 10 ** (-q)
    # choose s so that floor(x_num * 2^s / x_den) has its top bit at 127
    # s = 127 - floor(log2(x))
    # compute floor(log2(x)) exactly from bit lengths
    bl_num, bl_den = x_num.bit_length(), x_den.bit_length()
    # x ~ 2^(bl_num-bl_den-1 .. bl_num-bl_den+1); determine k = floor(log2(x))
    k = bl_num - bl_den
    # x >= 2^k iff x_num >= x_den * 2^k (handle negative k without << on k)
    ge = (x_num >= (x_den << k)) if k >= 0 else ((x_num << (-k)) >= x_den)
    if not ge:
        k -= 1
    # now 2^k <= x < 2^(k+1)
    s = 127 - k
    if s >= 0:
        val = (x_num << s) // x_den
    else:
        val = x_num // (x_den << (-s))
    assert (1 << 127) <= val < (1 << 128), (i, q)
    return val >> 64, val & MASK


# ---------------------------------------------------------------------------
# exponent shift table:  raw exponent r -> compute_exponent_shift(e, d+1) + 6
# where e = r - 1075 (r==0 maps to e = -1074), d = floor(e * log10 2)
# ---------------------------------------------------------------------------
def sar(v, n):
    return v >> n  # python >> is arithmetic on ints


def compute_decimal_exponent(e):
    return (e * 315653) >> 20


def compute_exponent_shift(e, q):
    return e + ((-q * 217707) >> 16) + 1


def build_shift_table():
    data = bytearray(2048)
    for raw in range(2048):
        e = raw - B64_EXP_OFFSET + (1 if raw == 0 else 0)
        d = compute_decimal_exponent(e)
        v = compute_exponent_shift(e, d + 1) + EXTRA_SHIFT
        assert 0 <= v <= 255, (raw, v)
        data[raw] = v
    return data


def build_pow10_table():
    out = bytearray()
    for i in range(POW10_SIZE):
        shi, slo = compute_seed(i)
        ehi, elo = compute_exact(i)
        assert (shi, slo) == (ehi, elo), f"pow10 entry {i} mismatch: seed=({shi:#x},{slo:#x}) exact=({ehi:#x},{elo:#x})"
        assert shi >> 63, f"entry {i} not normalized"
        out += struct.pack("<QQ", shi, slo)
    return out


def main():
    shifts = build_shift_table()
    pow10 = build_pow10_table()
    blob = bytes(shifts) + bytes(pow10)
    assert len(blob) == 2048 + POW10_SIZE * 16
    out = os.path.join(os.path.dirname(os.path.abspath(__file__)), "da_cache.bin")
    with open(out, "wb") as f:
        f.write(blob)
    print(f"wrote {out}: {len(blob)} bytes "
          f"(2048 shift + {POW10_SIZE}x16 pow10), all 618 entries verified vs exact bignum")


if __name__ == "__main__":
    main()
