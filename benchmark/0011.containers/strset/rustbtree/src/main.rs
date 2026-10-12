use rand::rngs::StdRng;
use rand::{Rng, SeedableRng};
use std::collections::BTreeSet;
use std::time::Instant;

fn generate_test_data() -> Vec<String> {
    let mut vec = Vec::with_capacity(1_000_000);
    let mut rng = StdRng::seed_from_u64(0xDEADBEEF); // deterministic seed

    for _ in 0..1_000_000 {
        let len = rng.random_range(25..=30); // replaces Uniform(8, 20)
        let mut s = String::with_capacity(len);

        for _ in 0..len {
            let x = rng.random_range(0..=61); // replaces Uniform(0, 61)
            let ch = match x {
                x if x < 10 => (b'0' + x as u8) as char,
                x if x < 36 => (b'a' + (x - 10) as u8) as char,
                _ => (b'A' + (x - 36) as u8) as char,
            };
            s.push(ch);
        }

        vec.push(s);
    }

    vec
}

fn main() {
    let vec = generate_test_data();
    let start_total = Instant::now();
    println!("str_btree_set test started");

    let mut bset = BTreeSet::new();

    let start_insert = Instant::now();
    for s in &vec {
        bset.insert(s.clone()); // cloning to move into BTreeSet
    }
    println!("insert_key: {:.2?}", start_insert.elapsed());

    let mut count = 0;
    let start_contains = Instant::now();
    for s in &vec {
        if bset.contains(s) {
            count += 1;
        }
    }
    println!("contains: {:.2?}", start_contains.elapsed());
    // 🚀 Forward iteration benchmark
    let mut total_size = 0;
    let start_iter = Instant::now();
    for s in &bset {
        total_size += s.len();
    }
    println!(
        "forward iteration total_size = {}, elapsed: {:.2?}",
        total_size,
        start_iter.elapsed()
    );

    // 🔄 Reverse iteration benchmark
    let mut total_size = 0;
    let start_rev_iter = Instant::now();
    for s in bset.iter().rev() {
        total_size += s.len();
    }
    println!(
        "reverse iteration total_size = {}, elapsed: {:.2?}",
        total_size,
        start_rev_iter.elapsed()
    );

    println!("count = {}", count);
    println!("total time: {:.2?}", start_total.elapsed());
}
