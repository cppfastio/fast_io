# fast_io conventions

## TLS module (include/fast_io_crypto/tls/)

- Free functions and public structs only. No OOP encapsulation, no
  private state, no convenience methods. RAII and moves are fine.
- Herbceptions only. `catch throws(::std::error)` is the only handler;
  `catch(...)` is forbidden and must never appear.
- Name qualification: inside `fast_io::tls{::details}` scope bare
  names resolve locally and stay bare; any code outside that scope
  must write `::fast_io::tls::` / `::fast_io::tls::details::` fully.
- Windows-specific code: `#if (defined(_WIN32) && !defined(__WINE__)) ||
  defined(__CYGWIN__)`. wine-gcc defines _WIN32 on a POSIX-hosted
  compiler, so Wine takes POSIX paths; Cygwin uses posix_thread_pool.
- FAST_IO_TLS_FORCE_FAST_IO=1 pins everything to the fast_io userspace
  implementations (defined in defs.h).

## Crypto backend seam

`basic_tls_client` takes a defaulted `crypto` backend supplying seven
ops: sha256/sha384 contexts (transcript+HKDF), x25519_keypair /
x25519_shared_secret, record_open/seal_inner/seal (suite-dispatched
AEAD), cert_sig_verify / cert_cv_verify (issuer/leaf cert passed whole
so providers can import spki_der). Protocol machinery -- record
framing, key schedule, X.509 parse, chain walk, SAN -- always stays in
fast_io.

Default backend resolves: openssl EVP > gnutls > builtin on non-windows;
builtin on windows and under FORCE_FAST_IO.
## Freestanding discipline

This library is freestanding. Do not use C++ hosted standard library
facilities anywhere in include/.

- No `::std::pair`, `::std::tuple`, `::std::unique_ptr`,
  `::std::shared_ptr`, or any standard smart pointer.
- No `new`/`delete` expressions and no `::std::construct_at` --
  use placement new (`::new (ptr) T{...}`) or the fast_io construct
  helpers.
- No `::std::exchange`, `::std::move` where a fast_io utility exists,
  and no standard-library containers at all -- `vector`, `string`,
  `deque`, `list`, `map`, `unordered_map` etc. come from
  `fast_io::containers` (`basic_vector`, `basic_string`, ...), not
  `::std`.
- No `iostream`, `fstream`, `sstream`, `algorithm` heap helpers, or
  any other hosted-only standard header.
## Herbceptions

All error handling uses Herbceptions, not legacy C++ exceptions.

- `throw throws(::std::error{...})` to raise; `try {}` +
  `catch throws(::std::error e) {}` to handle.
- The canonical handler in user-facing code and examples reports the
  error -- it never swallows it silently:

```cpp
try
{
	/* ... */
}
catch throws(::std::error e)
{
	::fast_io::io::perrln(e);
	return 1;
}
```

- When a handler cannot call throws-marked code inside its body, the
  empty handler form is correct: `catch throws(::std::error){}`.
- `catch(...)` is forbidden everywhere -- it is legacy-EH-only, catches
  nothing that can exist under `-fherbceptions`, and lies that an
  unknown C++ exception means a protocol error. NEVER write
  `catch(...)` or `catch (...) {}` in examples either; use the
  `catch throws(::std::error e){perrln(e);}` form above.
- `FAST_IO_HERBCEPTIONS_TRY`/`FAST_IO_HERBCEPTIONS_CATCH`/`FAST_IO_-
  HERBCEPTIONS_CATCH_ALL` expand to the right form per build config;
  `FAST_IO_HERBCEPTIONS_CATCH_ALL` is the only sanctioned catch-all
  and is for C-ABI / thread-entry boundaries only.
- Throwing functions are marked `FAST_IO_HERBCEPTIONS_THROWS`
  (or the explicit `throws` spec); noexcept helpers stay noexcept.
## Detemplatization

Template parameters must pay for themselves. Code inside a template
that does not touch the template parameter is generated per
instantiation -- N template args means N copies of identical machine
code.

```cpp
// BAD: the before/after work is re-emitted for every T
template <typename T>
void foo(T t) noexcept
{
	// big block that never touches t
	t.bar();
	// big block that never touches t
}

// GOOD: hoist the invariant blocks; only the glue stays templated
inline void foo_before() noexcept { /* ... */ }
inline void foo_after() noexcept { /* ... */ }

template <typename T>
void foo(T t) noexcept
{
	foo_before();
	t.bar();
	foo_after();
}
```

- Split functions at the points where the template parameter is
  actually used; the extracted pieces take the client's concrete
  fields, not the template param.
- Prefer extracting to `details::` free functions on the non-templated
  client/observer aggregate over adding more parameters.
- A param used only at the boundary (e.g. `ch_type` on an observer
  whose handle points at a ch_type-free client) means the whole
  operation can drop that parameter -- delegate straight to the
  client-level function.
- Same rule for `crypto` backends: wire-format, transcript-buffer and
  X.509 work is type-independent -- hoist it; only the primitive calls
  belong in the templated layer.
- Lambdas are particularly bad here: every lambda is a unique type,
  so a lambda passed into a templated pump/state machine instantiates
  the entire machine per call site even when the body differs by one
  line. Factor shared lambda bodies into named functions and pass a
  plain function pointer or a small functor -- one instantiation, not
  one per call site.
- This is true even for identical bodies. Two copies of the same
  lambda are two distinct types, hence two instantiations:

```cpp
::std::ranges::sort(vec, [](auto const &a, auto const &b) { return b < a; });
::std::ranges::sort(vec, [](auto const &a, auto const &b) { return b < a; });
/* twice the code -- each lambda is its own closure type */

::std::ranges::sort(vec, ::std::ranges::greater{});
::std::ranges::sort(vec, ::std::ranges::greater{});
/* one instantiation */
```

  Prefer named function objects / function pointers; avoid lambdas
  whenever the call goes through a template.
- Value parameters multiply the same way -- a `template <::std::size_t
  N>` buffer/capacity instantiates once per N even when the body only
  uses N as an array bound. If the function does not fundamentally need
  the constant, take the size as a runtime parameter and let the
  caller's `obj.data()`/`obj.size()` carry it:

```cpp
// BAD: f<4096>, f<8192>, f<65536> are three copies of the same loop
template <::std::size_t N>
void consume(::fast_io::containers::basic_string<char8_t, ...N...> &s);

// GOOD: one copy, size arrives with the argument
void consume(char8_t const *data, ::std::size_t size);
```

  This applies double when the size is a loop bound:

```cpp
// BAD: the loop is re-emitted for every N
template <::std::size_t N>
void clear(::std::byte *buf)
{
	for (::std::size_t i{}; i != N; ++i)
	{
		buf[i] = {};
	}
}

// GOOD: one loop, one copy -- the bound is just a register
inline void clear(::std::byte *buf, ::std::size_t n)
{
	for (::std::size_t i{}; i != n; ++i)
	{
		buf[i] = {};
	}
}
```

## Dedup near-identical code

A big block repeated with small variations is still one copy too many:
extract the common shape into a function and let call sites pass the
difference.

```cpp
// BAD
void foo()
{
	/* somewhat similar code */
}

void bar()
{
	/* somewhat similar code */
}

// GOOD
void baz(/* the differing bits as params */)
{
	/* the common shape */
}

void foo() { baz(...); }
void bar() { baz(...); }
```

- "Somewhat similar" counts -- do not wait for byte-identical copies;
  parameterize the differing constants/branches instead.
- The extracted function is `details::` or file-local; call sites keep
  their own names.

## Debugging internals

Use `__builtin_printf`/`__builtin_write` to debug fast_io's own code --
the library cannot print through itself while it is the thing being
debugged (chicken-and-egg). `perrln`/`debug_perrln` allocate, run the
format machinery, and may themselves be part of the bug; a raw
`__builtin_printf("%p %zu\n", ptr, n)` touches none of it. Strip the
probes before committing.

## Verification

Compile-check changed headers with the full warning set:

```sh
clang++ --config=$HOME/herbcfgs/x86_64-linux-gnu-libcxx.cfg \
	-fherbceptions -std=c++2c -fsyntax-only -Iinclude \
	-Wall -Wextra -Wpedantic -Wmisleading-indentation -Wunused \
	-Wuninitialized -Wshadow -Wconversion your_check.cc
```

- Zero warnings is the bar -- latent warnings in uninstantiated
  templates only surface when something instantiates them, so prefer a
  check TU that forces the touched machinery (explicit
  `template void f<...>(...)` instantiation or a call path through it).
- Format touched files with `clang-format` before committing.

## Commits

Plain `git commit -m "message"` -- no co-author lines, no signature
trailers, no "Generated with" footers.
