/*
Benchmark: ed25519-dalek / x25519-dalek.

Same methodology as benchmark/bench.cc: rdtscp + lfence, warmup 200,
25 batches x N iters, report the minimum cycles/iter.
*/

use ed25519_dalek::{Signature, Signer, SigningKey, Verifier};
use std::hint::black_box;
use x25519_dalek::{PublicKey, StaticSecret};

#[inline]
fn rdtscp() -> u64 {
    unsafe {
        let lo: u32;
        let hi: u32;
        core::arch::asm!("rdtscp", out("eax") lo, out("edx") hi, out("ecx") _, options(nomem, nostack));
        (hi as u64) << 32 | lo as u64
    }
}

#[inline]
fn bench_start() -> u64 {
    unsafe {
        core::arch::asm!("lfence", options(nomem, nostack));
    }
    rdtscp()
}

#[inline]
fn bench_end() -> u64 {
    let t = rdtscp();
    unsafe {
        core::arch::asm!("lfence", options(nomem, nostack));
    }
    t
}

fn bench<F: FnMut() -> R, R>(mut f: F, iters: usize, batches: usize) -> u64 {
    for _ in 0..200 {
        black_box(f());
    }
    let mut best = u64::MAX;
    for _ in 0..batches {
        let t0 = bench_start();
        for _ in 0..iters {
            black_box(f());
        }
        let t1 = bench_end();
        best = best.min((t1 - t0) / iters as u64);
    }
    best
}

fn hex2bytes<const N: usize>(hex: &str) -> [u8; N] {
    let mut out = [0u8; N];
    let hv = |c: u8| -> u8 {
        match c {
            b'0'..=b'9' => c - b'0',
            _ => (c | b' ') - b'a' + 10,
        }
    };
    let b = hex.as_bytes();
    for i in 0..N {
        out[i] = hv(b[2 * i]) << 4 | hv(b[2 * i + 1]);
    }
    out
}

fn main() {
    /* RFC 8032 TEST 1 — same seed/msg as bench.cc */
    let sk_bytes: [u8; 32] =
        hex2bytes("9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60");
    let msg = [0u8; 64];

    let signing_key = SigningKey::from_bytes(&sk_bytes);
    let verifying_key = signing_key.verifying_key();
    let sig: Signature = signing_key.sign(&msg);

    /* sanity */
    assert!(verifying_key.verify(&msg, &sig).is_ok());
    assert!(verifying_key.verify_strict(&msg, &sig).is_ok());
    println!("sanity: dalek verify + verify_strict ok\n");

    const N: usize = 300;
    const B: usize = 25;

    let c = bench(|| SigningKey::from_bytes(black_box(&sk_bytes)), N, B);
    println!("ed25519 keypair   dalek: {} cycles", c);

    let c = bench(|| black_box(signing_key.sign(black_box(&msg))), N, B);
    println!("ed25519 sign      dalek: {} cycles", c);

    let c = bench(
        || black_box(verifying_key.verify(black_box(&msg), black_box(&sig))),
        N,
        B,
    );
    println!("ed25519 verify    dalek: {} cycles", c);

    let c = bench(
        || black_box(verifying_key.verify_strict(black_box(&msg), black_box(&sig))),
        N,
        B,
    );
    println!("ed25519 verify_strict dalek: {} cycles", c);

    /* X25519 — RFC 7748 alice */
    let dhsk: [u8; 32] =
        hex2bytes("77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a");
    let secret = StaticSecret::from(dhsk);
    let public = PublicKey::from(&secret);

    let c = bench(
        || {
            let s = StaticSecret::from(black_box(dhsk));
            black_box(PublicKey::from(&s))
        },
        N,
        B,
    );
    println!("x25519 pubkey     dalek: {} cycles", c);

    let c = bench(
        || black_box(black_box(&secret).diffie_hellman(black_box(&public))),
        N,
        B,
    );
    println!("x25519 shared     dalek: {} cycles", c);
}
