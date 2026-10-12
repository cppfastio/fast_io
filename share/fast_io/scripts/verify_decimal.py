#!/usr/bin/env python3
"""Exact-math canonical shortest-decimal verifier for fast_io's floating
conversion module (fast_io_floating.cppm).

Uses fractions.Fraction to compute the true rounding interval of each raw
(mantissa, exponent-field) pair and applies the canonical rule:

    maximize e10 such that some integer sig has lo <= sig * 10**e10 <= hi,
    then take the member nearest to v (ties to even; endpoints included
    only for an even significand).

Input files are produced by a C++ dump tool that prints, per line:
    <hex fields> <m10 hex/dec> <e10>
for each conversion entry point (see tests that drive
to_decimal_binary16/bfloat16/binary32/binary64/binary80/binary128).

Usage:
    python3 verify_decimal.py f16.out      # exhaustive binary16
    python3 verify_decimal.py bf16.out     # exhaustive bfloat16
    python3 verify_decimal.py f80_20k.out  # sampled binary80
    python3 verify_decimal.py f128_10k.out # sampled binary128
"""
import sys
from fractions import Fraction


def canonical(m2, e2, mbits, ebits):
    """exact canonical shortest decimal for raw (mantissa, exponent) fields"""
    bias = (1 << (ebits - 1)) - 1
    if e2 == 0:
        m, e, regular = m2, 1 - bias - mbits, True
    else:
        m = m2 | (1 << mbits)
        e = e2 - bias - mbits
        regular = m2 != 0
    if m == 0:
        return 0, 0
    v = Fraction(m) * Fraction(2) ** e
    if regular:
        lo = v - Fraction(2) ** (e - 1)
        hi = v + Fraction(2) ** (e - 1)
    else:
        lo = v - Fraction(2) ** (e - 2)
        hi = v + Fraction(2) ** (e - 1)
    even = (m & 1) == 0
    k0 = int(e * 0.30103) + 8
    for e10 in range(k0, k0 - 52, -1):
        sc = Fraction(10) ** e10
        cl, ch = lo / sc, hi / sc
        c_lo = -(-cl.numerator // cl.denominator)
        c_hi = ch.numerator // ch.denominator
        if not even:
            if cl.denominator == 1 and cl.numerator == c_lo:
                c_lo += 1
            if ch.denominator == 1 and ch.numerator == c_hi:
                c_hi -= 1
        if c_lo > c_hi:
            continue
        # member exists; take the one nearest to v, tie to even
        x = v / sc
        fl = x.numerator // x.denominator
        rem = x - fl
        nearest = fl + 1 if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (fl & 1)) else fl
        return max(c_lo, min(c_hi, nearest)), e10
    return None


FIELD_BITS = {'f16': (10, 5), 'bf16': (7, 8), 'f32': (23, 8),
              'f64': (52, 11), 'f80': (63, 15), 'f128': (112, 15)}


def main():
    mode = sys.argv[1]
    mbits, ebits = FIELD_BITS[mode]
    fails = tested = 0
    for line in open(sys.argv[2]):
        p = line.split()
        if mode == 'f80':
            m2 = int(p[0].split(':')[0], 16)
            e2 = int(p[0].split(':')[1], 16)
            m10 = (int(p[1].split(':')[0], 16) << 64) | int(p[1].split(':')[1], 16)
            e10 = int(p[2])
        elif mode == 'f128':
            m2 = (int(p[0].split(':')[0], 16) << 64) | int(p[0].split(':')[1], 16)
            e2 = int(p[0].split(':')[2], 16)
            m10 = (int(p[1].split(':')[0], 16) << 64) | int(p[1].split(':')[1], 16)
            e10 = int(p[2])
        else:
            bits = int(p[0], 16)
            m2 = bits & ((1 << mbits) - 1)
            e2 = (bits >> mbits) & ((1 << ebits) - 1)
            m10 = int(p[1])
            e10 = int(p[2])
        tested += 1
        ref = canonical(m2, e2, mbits, ebits)
        if ref != (m10, e10):
            if fails < 15:
                print(f"m2={m2:x} e2={e2:x} ours=({m10},{e10}) ref={ref}")
            fails += 1
    print(f"{mode}: tested={tested} fails={fails}")


if __name__ == '__main__':
    main()
