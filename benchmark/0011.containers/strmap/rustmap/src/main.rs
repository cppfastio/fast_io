use rand::rngs::StdRng;
use rand::{Rng, SeedableRng};
use std::collections::HashMap;
use std::time::Instant;

fn generate_test_data() -> Vec<String> {
    let mut vec = Vec::with_capacity(1_000_000);
    let mut rng = StdRng::seed_from_u64(0xDEADBEEF); // deterministic seed

    for _ in 0..1_000_000 {
        let len = rng.random_range(25..=30);
        let mut s = String::with_capacity(len);

        for _ in 0..len {
            let x = rng.random_range(0..=61);
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
    println!("rust HashMap test started");

    let mut bmap: HashMap<String, usize> = HashMap::new();
    let start_insert = Instant::now();
    for (i, s) in vec.iter().enumerate() {
        bmap.insert(s.clone(), i);
    }
    println!("insert: {:.2?}", start_insert.elapsed());

    let mut count = 0;
    let start_contains = Instant::now();
    for s in &vec {
        if bmap.contains_key(s) {
            count += 1;
        }
    }
    println!("contains: {:.2?}", start_contains.elapsed());

    let mut total_size = 0;
    let mut total_mapped: usize = 0;
    let start_iter = Instant::now();
    for (s, v) in &bmap {
        total_size += s.len();
        total_mapped += v;
    }
    println!(
        "iteration total_size = {}, total_mapped = {}, elapsed: {:.2?}",
        total_size,
        total_mapped,
        start_iter.elapsed()
    );

    // HashMap has no reverse iteration: loop again like the C++ benchmark
    let mut total_size = 0;
    let mut total_mapped: usize = 0;
    let start_rev_iter = Instant::now();
    for (s, v) in &bmap {
        total_size += s.len();
        total_mapped += v;
    }
    println!(
        "reverse iteration total_size = {}, total_mapped = {}, elapsed: {:.2?}",
        total_size,
        total_mapped,
        start_rev_iter.elapsed()
    );

    println!("count = {}", count);
    println!("total time: {:.2?}", start_total.elapsed());
}
