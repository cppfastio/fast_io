# binfmt + lcblob — design spec (v1, for review)

Binary program representation of format strings, plus the flat
position-independent locale container that carries them.

Two pieces:

1. **binfmt** — a compiled *program* representing what a format string
   would do. Text is parsed **once**, offline (localedef/codegen time).
   fast_io never parses a format string at runtime — the only thing that
   executes is a bounds-checked byte program.
2. **lcblob** — flat locale file where every internal reference is an
   **RVA-style u32 offset from the blob base**. No pointers, no
   relocations: mmap'd read-only and shared byte-identical across all
   processes, embeddable in `.rodata`, loadable on targets with no
   DLL support at all. Replaces the old `.cpp`→DLL codegen — pointer
   data in a DLL relocates per-process and every process gets private
   copies of the pages.

## Security model — structural, not enforced

The hazard in format strings is that data becomes code (`%n` writes,
`format(str, str)` double-format, `*`-driven arg consumption, macro/codegen
injection around "compile-time" formats). binfmt removes the class, not
the instances:

- there is no format-string parser at runtime, period;
- the op set cannot express writes (`%n`), lookups (`{name}`), or
  dynamic specs (`*`, `{:{}}`, `!r`) — no codes exist for them;
- an argref is only an index — **arg content can never become format
  code**;
- blobs are trusted compiled artifacts (same trust level as generated
  tables); the decoder bounds-checks structure only — it does not police
  sizes or counts, since nothing encodable can do anything dangerous.

---

## Part 1 — wire encoding

### Integers and text

- Node stream: **LEB128** (`uleb128`/`sleb128`) — tags, lengths, scalar
  values. Locale data is mostly small numbers and markers; fixed u32
  would spend most of its bytes on zeros.
- The **container** (lcblob) uses LEB128 for every numeric value —
  lengths, counts, ints, sizes. Only **RVA index tables** stay fixed
  u32: they're the random-access mechanism (an array of variable-width
  entries can't be indexed), and you decode exactly one entry per
  lookup anyway.
- Text payloads (literals, names, fill chars) are byte strings in the
  container's declared charset (UTF-8 default).
- Structural codes are numeric enums — never character codes. A compiler
  on an EBCDIC host maps its charset at compile time; the blob is
  charset-independent.
- No alignment requirements anywhere.

### Node framing — lisp-like tagged nodes

Every node is `[uleb128 tag][payload]`:

```
uleb128 tag = (code << 2) | payload_kind

kind 0 none    bare marker                    next node at +tagbytes
kind 1 uleb    [uleb128 value]                unsigned scalar
kind 2 bytes   [uleb128 len][data]            a code, a length, a payload
kind 3 list    [uleb128 count][child nodes]   children counted, not sized
kind 4 sleb    [sleb128 value]                signed scalar
(kind 5-7 reserved)
```

Every node is self-delimiting: the walker reads the tag varint and hops
— it never inspects payload to find the next node. `list` reads a child
count and hops that many child nodes (each still self-delimiting); list
children are scalars/bytes/markers — a list never contains a list, so
skipping needs no recursion. Unknown codes skip cleanly via kind, so
new codes are forward-compatible. Common codes are 1-byte tags; the
encoding spends bytes on data, not on zeros.

### Code space

`code` is unbounded (it lives in the varint tag) — no partition needed.
Current assignments: `0x00`–`0x3F` core codes (1-byte tags),
`0x40`–`0x4F` gettext sub-space (`plural`/`count`/`form`, 2-byte tags).
Future codes just take the next free value.

### Program

A program is a **bare node sequence** — no header, no version, no
grammar marker. The bound comes from wherever the program is embedded:
an lcblob `PROGRAM` slot's `len`, a `constexpr` array's extent. There
is no end-of-program node either — the container's bound is the
terminator. Every program is exactly the same shape as a `chrono` or
`form` payload: nodes until the outer length runs out.

Forward compat is handled by unknown-code skipping (every node is
self-delimiting).

There is **one opcode space**: `literal`/`field`/`pct`/`plural` are just
ops in a stream, not categories of program. Which ops appear depends on
which front-end compiled the text — a strftime slot produces `pct` ops,
a fmt string produces `field` ops, a printf string produces the same
`field` ops (printf is a front-end option, not a format). The
interpreter never asks what grammar made the program; a chrono `field`
carrying `pct` children is just how ops compose.

---

## Part 2 — top-level ops

| code | node | kind | payload |
|------|------|------|---------|
| 1 | `literal` | bytes | already-unescaped text |
| 2 | `field` | list | **the** format field — fmt-spec superset; printf compiles into this too (children below) |
| 3 | `pct` | uleb / list | `%`-directive — `uleb` = conv only (the common case, 2B); `list` = parameterized (children below) |
| 0x40 | `plural` | list | gettext entry — sole root node only; `count` + `form`×N (Part 5). 0x40–0x4F is the gettext sub-space; op code 4 stays free |

`plural` is NOT a mid-stream op — a plural program is exactly one root
`plural` node; it cannot nest inside a program or appear next to other
ops. That's a grammar rule, not an encoding layer: the decoder sees
"program = single plural node", no marker needed. Deliberately not ICU
— no inline select/gender/nested message machinery.

`field` may appear inside a `pct` node sequence (chrono/generic specs
allow nested `{...}`).

`field` takes a call arg and formats it. `pct` indexes locale/time-struct
fields — `%H` is "hour", not an arg. Different domains, different nodes.

The `field` node is deliberately a **superset of fmt's format spec**:
fmt needs fill/align/sign/`#`/`0`/width/prec/`L`/type/chrono — printf
adds `'`-grouping and `I`-outdigits. printf **length modifiers compile
to `ctype`** (`hh`…`wf` → i32…cf128/c8ptr…gbview/ptr) — kept in the blob for
type-erased consumers, ignored by the in-process interpreter where the
real C++ arg type wins.

---

## Part 3 — `field` children (printf ≡ fmt unified)

| code | param | kind | value | printf | fmt |
|------|-------|------|-------|--------|-----|
| 1 | arg | uleb | arg index | `%2$`→1 | `{1}` |
| 2 | fill | bytes | raw fill bytes in container charset; absent → `" "`. Covers multi-byte / fullwidth / grapheme fills; zero transcode | — | `{:*>8}` |
| 3 | align | uleb | 1`<` 2`>` 3`^` | `-`→1 | `<` `>` `^` |
| 4 | sign | uleb | 1`+` 2`-` 3` ` | `+` ` ` | `+` `-` ` ` |
| 5 | flag-alt | none | `#` | `#` | `#` |
| 6 | flag-zero | none | `0` | `0` | `0` |
| 7 | flag-group | none | `'` | `'` | — |
| 8 | flag-locale | none | `L` | — | `L` |
| 9 | flag-outdigits | none | `I` | `I` | — |
| 10 | width | uleb | value | `%5` | `{:5}` |
| 11 | prec | uleb | value (`.` alone → 0) | `%.3` | `{:.3}` |
| 12 | ctype | uleb | enum below — declared C arg type | `%lld`→i64 | — |
| 13 | type | uleb | conv enum below | `%d` | `{:d}` |
| 14 | chrono | bytes | nested pct node sequence | — | `{:%H:%M}` |
| 15 | element | bytes | node seq = element's field params (no `arg`) — range types only | — | `{::^8x}` |
| 16 | flag-upper | none | uppercase digits/prefix/exponent | `E F G A X B` | `{:X}` |

**ctype enum** — the C arg type, declared as `i`/`u`/`f` fixed-width codes
instead of the length-modifier zoo. printf source always emits it;
other front-ends emit it when they know the arg's C type. The compiler
resolves platform meanings at build time
(`l`→i64 on LP64 / i32 on LLP64, `z`→usize/isize, `t`→isize, `L`→ld,
`w64`/`wf64`→i64, `q`→i64). In-process interpreters ignore it — the C++
arg type is authoritative — but it's kept in the blob for type-erased
consumers (DLL boundaries, `void*` args, codegen):

| code | ctype | from | code | ctype | from |
|------|-------|------|------|-------|------|
| 0 | `other` | user-defined / absent | 22 | `cebc` | EBCDIC codepage char |
| 1 | `i32` | `%d` `%hhd` `%hd` | 23 | `c8ptr` | `%s` — `char8_t const*` NUL |
| 2 | `i64` | `%lld` `%jd` `%w64` `%zd` `%td` | 24 | `c16ptr` | `%ls` `%S` — `char16_t const*` |
| 3 | `i128` | ext | 25 | `c32ptr` | `char32_t const*` NUL |
| 4 | `u32` | `%u` `%x` `%hhu` | 26 | `ebcptr` | UTF-EBCDIC string, NUL |
| 5 | `u64` | `%llu` `%ju` `%zu` | 27 | `gbptr` | GB18030 string, NUL |
| 6 | `u128` | ext | 28 | `c8view` | `{char8_t const*, len}` |
| 7 | `f16` | — (binary16) | 29 | `c16view` | `{char16_t const*, len}` |
| 8 | `bf16` | — (bfloat16) | 30 | `c32view` | `{char32_t const*, len}` |
| 9 | `f32` | — | 31 | `ebcview` | UTF-EBCDIC `{ptr, len}` |
| 10 | `f64` | `%f` `%e` `%g` `%a` | 32 | `gbview` | GB18030 `{ptr, len}` |
| 11 | `f80` | `%Lf` (x86 ld) | 33 | `ptr` | `%p` — `void const*` |
| 12 | `f128` | `%Lf` (ppc/arm ld) | 34 | `fptr` | function ptr — not `void*` |
| 13 | `cf16` | — | 35 | `fldptr_i` | Itanium member object ptr — 1 word |
| 14 | `cbf16` | — | 36 | `mthptr_i` | Itanium member fn ptr — 2 words |
| 15 | `cf32` | `float _Complex` | 37 | `fldptr_m1` | MS member object ptr — 1 word |
| 16 | `cf64` | `double _Complex` | 38 | `fldptr_m2` | MS member object ptr — 2 words |
| 17 | `cf80` | x86 `ld _Complex` | 39 | `mthptr_m1` | MS member fn ptr — 1 word |
| 18 | `cf128` | ppc/arm `ld _Complex` | 40 | `mthptr_m2` | MS member fn ptr — 2 words |
| 19 | `c8` | `%c` — `char8_t` | 41 | `mthptr_m3` | MS member fn ptr — 3 words |
| 20 | `c16` | `%lc` `%C` — `char16_t` | 42 | `mthptr_m4` | MS member fn ptr — 4 words |
| 21 | `c32` | `char32_t` | 43 | `error` | `std::error` — `{domain const*, size_t}` |

`other` = 0 covers user-defined types — and is also what an absent
`ctype` param decodes to (a fmt-source field declares no C type).

No `i8`/`i16`/`u8`/`u16` — C varargs promote `hh`/`h` args to `int`.
No `usize`/`isize` — `%z`/`%t`/`%j` resolve to `u64`/`i64` (or `u32`/`i32`
on 32-bit targets) at compile; matching the passed arg is the caller's
responsibility. No complex ints — GNU `_Complex int` and
`std::complex<int>` fall to `other`. `cf*` never appears in
printf-source programs (no complex conversion exists) — it's metadata
for fast_io-source / codegen-produced programs.

Chars are the fixed-width types (`c8`/`c16`/`c32`) plus `cebc` (one
EBCDIC codepage byte) — never `wchar_t` (platform-width, ambiguous), no
`cgb` (a GB18030 char is 1–4 bytes, i.e. already a `gbview`). Strings
split into the two real shapes (`*ptr` NUL-terminated vs `*view`
ptr+len) × three byte charsets: UTF code units, UTF-EBCDIC (`ebc*`),
GB18030 (`gb*`). The encoded forms are byte strings — the tag tells the
consumer how to transcode to the container charset.

Pointers split by *representation*, not convenience: `fptr` is a
function pointer (not `void*` — POSIX-only interconvertible). Member
pointers are **ABI-split** since their layouts differ:

- `fldptr_i` / `mthptr_i` — Itanium: member object ptr is one
  `ptrdiff_t` offset (always 1 word); member fn ptr is exactly 2 words
  `{fn ptr | vtable offset, this-adjust}`
- `fldptr_m1/m2`, `mthptr_m1..m4` — MSVC `/vmg`: sizes vary *per type*
  by inheritance model — member object ptr 1–2 words, member fn ptr
  1–4 words `{code, this-delta, vb-delta, vtordisp}`. The word count is
  encoded in the code so a type-erased consumer knows the arg size
  without a second field. The compiler resolves the count from the
  declared type + `/vm*` model at build.
- `error` — herbceptions `std::error`: a plain 2-word struct
  `{error_domain_singleton const*, size_t opaque code}`. The code's
  meaning lives in the domain's functions; emit is domain-driven.

`chrono` is exclusive with the standard spec params — for a chrono arg
the *entire* spec is the time spec (`{:>20%Y}` = literal `">20"` + `%Y`,
not "align right width 20"). Compile rule: try a standard-spec parse
first; anything it cannot fully consume becomes a pct program — the same
way `formatter<T>::parse` claims the spec only when it recognises it.
A chrono spec may contain `{...}` fields (bounded depth).

The `chrono` payload is a **bare node sequence** — the bytes-len is the
delimiter, same as the top-level program's container bound. No headers
anywhere in the nesting.

Absent `arg` = AUTO (next arg). printf `%2$` is normalized to 0-based
index at compile. fmt `-` align is printf `-`; fmt `-` *sign* is sign=2.

**type enum** (semantic codes — `%i` canonicalizes to `d`; a blob
decompiled to printf always emits `d`):

| code | name | emits | printf | fmt / fast_io |
|------|------|-------|--------|---------------|
| 1 | `d` | signed dec — absorbs `%i` | `%d` `%i` | `{:d}` |
| 2 | `u` | unsigned dec | `%u` | `mnp::udec` |
| 3 | `o` | octal | `%o` | `{:o}` `mnp::oct` |
| 4 | `x` | hex — `#`→`0x`, case→`flag-upper` | `%x` `%X` | `{:x}` `{:X}` `mnp::hex` |
| 5 | `b` | binary | — | `{:b}` `{:B}` `mnp::bin` |
| 6 | `c` | character — `ctype` picks width | `%c` `%lc` `%C` | `{:c}` |
| 7 | `s` | string — `ctype` picks charset/shape | `%s` `%ls` `%S` | `{:s}` |
| 8 | `?` | debug-escaped string | — | `{:?}` |
| 9 | `p` | pointer `0x…` impl-defined | `%p` | `{:p}` `{:P}`→upper |
| 10 | `addr` | `0x` + full-width hex | — | `mnp::addrvw` `pointervw` `itervw` `funcvw` `fieldptrvw` |
| 11 | `mth` | member fn ptr: `0x<w0>` then signed `+wi` per extra word | — | `mnp::methodvw` |
| 12 | `dec` | decimal — shortest of fixed/sci (**fast_io float default**) | — | `mnp::decimal(t)`, fast_io `{}` |
| 13 | `decp` | decimal + precision | — | `mnp::decimal(t,n)` |
| 14 | `fix` | fixed, shortest | — | `mnp::fixed(t)` |
| 15 | `fixp` | fixed, prec frac digits | `%f` `%F` (prec→6) `{:.Nf}` | `mnp::fixed(t,n)` `{:f}`→prec 6 |
| 16 | `sci` | scientific, shortest | — | `mnp::scientific(t)` |
| 17 | `scip` | scientific, prec frac digits | `%e` `%E` (prec→6) `{:.Ne}` | `mnp::scientific(t,n)` `{:e}`→prec 6 |
| 18 | `gen` | general, shortest | — | `mnp::general(t)` |
| 19 | `genp` | general, prec sig digits | `%g` `%G` (prec→6) `{:.Ng}` | `mnp::general(t,n)` `{:g}`→prec 6 |
| 20 | `hexf` | hexfloat, shortest | `%a` `%A` (no prec) | `mnp::hexfloat(t)` `{:a}` |
| 21 | `hexfp` | hexfloat, prec frac hex digits | `%.Na` `%.NA` | `mnp::hexfloat(t,n)` `{:.Na}` |
| 22 | `rng` | range `[e0, e1]` — `, ` sep | — | `{:}` on a range |
| 23 | `rngn` | range naked `e0 e1` — ` ` sep | — | `{:n}` |
| 24 | `rngm` | map `{k0: v0}` — `, `/`: ` | — | `{:m}` |

**Normalization rules:**

- **Case is a flag, not a type** — `%X`/`{:X}`, `%E`, `{:B}`, `{:P}` all
  compile to the lower type + `flag-upper` (matches `mnp::x<upper>`,
  a bool template param, not a second conversion)
- **Float letters fold into fast_io modes** — printf/fmt `e`/`f`/`g`
  absent-prec defaults bake `prec=6` at compile: `%f` ≡ `{:f}` ≡
  `(type fixp)(prec 6)`. `%a` no-prec *is* shortest → `hexf`; `%.3a` →
  `hexfp`. The two algorithms (roundtrip vs precision) are distinct
  codes because fast_io dispatches them as different overloads
  (`roundtrip.h` vs `precision.h`), not a default arg
- **fmt `{}` on a float** → `dec` (fmt's default is also shortest repr)
- **`%lc`/`%ls`/`%C`/`%S` fold** — `type c`/`s` + `ctype c16`/`c16ptr`
  carries the width. No wide-letter codes
- **`fieldptrvw` shares `addr`** — member-object-ptr emit is identical
  (`0x`+full hex); only `methodvw` differs (multiword + signed adjust),
  so it gets `mth`

**`element` (param 15)** — range types only; payload is a node sequence
of the element's field params (same grammar as field children, minus
`arg`). `{::^8x}` → `(type rng)(element <(align ^)(width 8)(type x)>)`.
Outer `width`/`align`/`fill`/`sign` apply to the whole `[…]` output.
Nesting recurses: `vector<vector<int>>` `{:::x}` = `rng` + `element`
containing `rng` + its own `element` — depth-bounded like `chrono`.

Example — `"%08.3f"` and `"{:08.3f}"` compile to the same node:

```
(field (flag-zero) (width 8) (prec 3) (type fixp))
```

---

## Part 4 — `pct` children (%-directives)

`pct` has two payload shapes — the tag's `kind` disambiguates:

- **`uleb` — bare directive** (2 bytes): payload is the conv enum
  directly, no children. Every `%Y`/`%m`/`%H` with no modifiers.
- **`list` — parameterized** : children below; `conv` still required.
  Only used when `pad`/`case`/`modifier`/`colons`/`width`/`prec` exist
  (`%Ec`, `%5Y`, `%::z`, `%Od`).

| code | param | kind | value |
|------|-------|------|-------|
| 20 | conv | uleb | enum below — required (first child in list form) |
| 21 | letter | uleb | raw ASCII conv — generic slots only (name_fmt, postal_fmt, tel_*_fmt); meaning is slot-defined |
| 22 | pad | uleb | bit0 `-` bit1 `_` bit2 `0` |
| 23 | case | uleb | bit0 `^` bit1 `#` |
| 24 | modifier | uleb | 1 `E` era · 2 `O` alternative |
| 25 | colons | uleb | 1–3 (`%:z` `%::z` `%:::z`, conv `z` only) |
| 10 | width | uleb | shared code (`%5Y`) |
| 11 | prec | uleb | fractional-second digits etc. |

`conv` is its **own enum** — pct codes are not ASCII. strftime `%c`
(locale date-time composite) and printf `%c` (a character) are different
semantics → different numbers.

### conv enum

| code | directive | emits |
|------|-----------|-------|
| 1 | `%a` | weekday abbr |
| 2 | `%A` | weekday full |
| 3 | `%b` | month abbr — **`%h` canonicalizes here** |
| 4 | `%B` | month full |
| 5 | `%c` | composite → `d_t_fmt` |
| 6 | `%C` | century (year/100) |
| 7 | `%d` | day of month |
| 8 | `%e` | day of month, space-padded |
| 9 | `%F` | composite ISO date → `%Y-%m-%d` |
| 10 | `%G` | ISO week-numbering year (4-digit) |
| 11 | `%H` | hour 00–23 |
| 12 | `%I` | hour 01–12 |
| 13 | `%j` | day of year |
| 14 | `%k` | hour 0–23 space-pad |
| 15 | `%l` | hour 1–12 space-pad |
| 16 | `%m` | month |
| 17 | `%M` | minute |
| 18 | `%p` | AM/PM |
| 19 | `%P` | am/pm lowercase |
| 20 | `%r` | composite → `t_fmt_ampm` |
| 21 | `%R` | composite → `%H:%M` |
| 22 | `%s` | seconds since epoch |
| 23 | `%S` | second |
| 24 | `%T` | composite → `%H:%M:%S` |
| 25 | `%u` | weekday Mon=1..Sun=7 |
| 26 | `%U` | week number (Sunday) |
| 27 | `%V` | ISO week number |
| 28 | `%w` | weekday Sun=0..Sat=6 |
| 29 | `%W` | week number (Monday) |
| 30 | `%x` | composite → `d_fmt` |
| 31 | `%X` | composite → `t_fmt` |
| 32 | `%Y` | year (full — only form) |
| 33 | `%z` | tz offset ±hhmm; colons param for `±hh:mm` |
| 34 | `%Z` | tz name |
| 35 | `%+` | composite → `date_fmt` |
| 36 | `iso8601` | **fast_io's own iso8601 timestamp** — one code; emitter is `iso8601_timestamp` in `iso/iso8601.h` |
| 37 | `iso8601-utc` | utc variant (emits `Z` / zero offset form) |
| 38 | `fracsec` | fractional seconds; `prec` param = digits |

Composite convs stay codes: the interpreter resolves the referenced
locale slot (itself a blob) with a bounded recursion depth.

### Modifier matrix — `E`/`O` (from real localedata usage)

```
%E:  c C x X Y y
     %Ec → era_d_t_fmt   %Ex → era_d_fmt   %EX → era_t_fmt
     %EC → era name      %EY → era full (name+year "令和5年")
     %Ey → era year (year within era — NOT a 2-digit year; eras
           number years from their own offset: 令和5年 → "5")

%O:  d e H I k l m M p S u U V w W B C
     alternative-digit/alt-form numerics; %Op alt AM/PM;
     %OB/%OC → ab_alt_mon / alt forms (ru_RU etc. use these)
```

`%Ey` compiles to `modifier E` + `conv Y` — the same node as `%EY`, since
year-of-era needs no truncation. `%Oy`/`%y`/`%g`/`%D` get the 4-digit
substitution described below.

### Substituted — 2-digit years become 4-digit

`%y`, `%Oy`, `%g`, `%D` are **not encodable** as two-digit years — the
format cannot represent truncation. The compiler substitutes the 4-digit
equivalent instead of rejecting:

| source | emits | notes |
|--------|-------|-------|
| `%y` | `conv Y` | 2-digit → 4-digit year |
| `%Oy` | `modifier O` + `conv Y` | alt-form 4-digit year |
| `%Ey` | `modifier E` + `conv Y` | identical encoding to `%EY` — era year |
| `%g` | `conv G` | ISO week-year 2-digit → 4-digit |
| `%D` | `pct m`, `"/"`, `pct d`, `"/"`, `pct Y` | `%m/%d/%y` expansion |

This is deliberately lossy: a locale that asked for year-mod-100 gets the
full year. Better a wider year than a wrong one.

Also folded to literal bytes by the compiler (never nodes): `%%`→`%`,
`%n`→`\n`, `%t`→`\t`, `{{`→`{`, `}}`→`}`.

---

## Part 5 — plural (gettext)

A gettext catalog entry is `msgid` / `msgid_plural` / `msgstr[0..N-1]`
plus a plural rule. **The rule does not live in the blob** — it is a
fixed function of `n` per locale, and this format is not a programming
language. The caller evaluates the rule (a native per-locale function,
selected by a rule id stored in the lcblob catalog metadata) and passes
the chosen **form index**; the `plural` node is just an ordered list of
programs:

```
(plural (count <arg-idx>)  (form <prog>)  (form <prog>) ...)

1st child: count — uleb, which call arg holds n
rest:      form×N — bytes nodes; each payload is a bare node sequence
                    (the msgstr[N] program, usually via the printf front-end)
```

Interpretation: the caller reads `count` to know which arg is `n`,
evaluates its own plural rule, passes the chosen index → `form[i]` runs.
`form[i]` is whatever the locale's rule says — `form[0]` is conventionally
the singular but the rule decides: English `n != 1` sends `n=0` to the
plural ("0 apples"), French `n <= 1` sends it to `form[0]`. The blob only
carries the forms; which n maps where is the rule's business.

`count` may index a non-integer arg: "0.5 apples" is legal input. The
native rule works on CLDR operands (`n`, `i`, `v`, `w`, `f`, `e` —
integer part, visible fraction digits, etc.), so "1.0 apple" vs
"1 apple" can select differently. The program only needs the arg index.

The forms are ordinary programs — gettext printf-isms (`%1$s`, `%d`…)
compile through the printf front-end into `field` nodes, so a form can
consume the count arg like any other (`"%d apples"` just prints it).

The lcblob side carries `plural_rule` as an integer slot — an index into
fast_io's own plural-rule table (CLDR rules implemented as native code),
*not* an encoded expression.

| code | node | kind | payload |
|------|------|------|---------|
| 0x40 | `plural` | list | children: `count` + `form`×N |
| 0x41 | `count` | uleb | arg index of `n` |
| 0x42 | `form` | bytes | bare node sequence (a program's body) |

### Example — `"an apple"` / `"%d apples"`

Source text stays source — the *forms* hold compiled node sequences:

```
(plural (count 0)
        (form <nodes: (literal "an apple")>)
        (form <nodes: (field (type d))(literal " apples")>))
```

`form[0]` is literal-only — "an apple" has no field at all; `form[1]`'s
`"%d apples"` compiled through the printf front-end into the same nodes
`"{:d} apples"` would produce — no C syntax survives serialization.

---

## Byte examples

`{0:2147483647}` — 10 bytes total (the whole program is the field node;
the embedder's bound ends it):

```
0B               tag (2<<2)|3   field, list
  02             count = 2 children
  05 00          tag (1<<2)|1   arg  = 0
  29 FF*4 07     tag (10<<2)|1  width = 2147483647  (uleb: 5 bytes)
```

`{0:%Y-%m-%d}` on a chrono arg — 18 bytes: the spec becomes
an embedded pct program via the `chrono` param:

```
0B               tag (2<<2)|3   field, list
  02             count = 2
  05 00          arg = 0
  3A             tag (14<<2)|2  chrono, bytes
    0C             len = 12     → bare nodes, no inner header
      0D 20          pct scalar (3<<2)|1, conv = 32 (%Y)
      06 01 "-"      literal
      0D 10          conv = 16 (%m)
      06 01 "-"      literal
      0D 07          conv = 7  (%d)
```

`"%Y年%m月%d日 %H時%M分%S秒"` (ja_JP `d_t_fmt`) = 43 bytes —
each `%X` directive is 2 bytes, e.g. `%Y` = `0D 20`. A parameterized one
uses the list form — `%Ec` (6 bytes):

```
0F               tag (3<<2)|3   pct, list
  02             count = 2
  61 01          modifier (24<<2)|1 = 1 (E)
  51 05          conv (20<<2)|1 = 5 ('c')
```

Plural, `count=arg0` + `"an apple"` / `"%d apples"` — the caller picks
the index = 35 bytes:

```
83 02            tag (0x40<<2)|3 plural, list
  03             count = 3 children
  85 02 00       tag (0x41<<2)|1 count = 0
  8A 02 0A       tag (0x42<<2)|2 form, len=10
    <nodes: (literal "an apple")>
  8A 02 0D       tag (0x42<<2)|2 form, len=13
    <nodes: (field (type d))(literal " apples")>
```

---

## Part 6 — rejected constructs

No encoding exists; compilers error out:

- `%n`, `%m`, `%[` (printf grammar)
- `*`, `*n$` — runtime-driven width/precision
- `{name}` — named args
- `!r` `!a` `!s` — repr dispatch
- `{:{}}` — nested `{}` inside width/precision
- `,`/`_` digit-group specifiers
- `%y` `%Oy` `%g` `%D` as two-digit years — the compiler substitutes the
  4-digit equivalent (`%Y`/`%OY`/`%G`/`%m/%d/%Y`; `%Ey` shares the `%EY`
  encoding)
- unknown convs; colons on non-`z`; unterminated fields; lone `}`

---

## Part 7 — lcblob container

One file per `<locale>.<codeset>`: `de_DE.UTF-8.bin`, `zh_CN.GB18030.bin`.
The codeset names the **`char` view only** — `char8_t`/`char16_t`/
`char32_t` sections are always present and identical across codesets.
Supported codesets: UTF-8, GB18030, UTF-EBCDIC. `C`/`POSIX` emit once as
`POSIX.UTF-8.bin`.

**The file IS the locale data.** It is a flat, position-independent
image of the `basic_lc_*` C-struct layout — every pointer is an
`lc_rva<T>` (a `u32` file offset) and every `basic_io_scatter_t` is an
`lc_scatter<T>` (`lc_rva` + `u32` unit count). All fields are
little-endian `u32`/`s32`; the layout is all-`u32`, deterministic on
every ABI. Loading is: mmap (private copy-on-write) + check magic and
version + return a `lc_locale const*` into the image. There is no
decoding step and no validation beyond the two header words.

```
lc_locale root (40 bytes, at file offset 0):
  u32 magic            'FCL1' = 0x314C4346
  u32 version          format version (currently 1)
  u32 total            file size
  u32 flags            reserved, 0
  u32 codeset          locale_charset of the char view: 0 UTF-8,
                       1 GB18030, 2 UTF-EBCDIC
  lc_scatter<char8_t> name     canonical "de_DE.GB18030", utf8
  lc_rva<basic_lc_all<char>>     all    charset section — the declared
  lc_rva<basic_lc_all<char8_t>>  u8all    char codeset for char text;
  lc_rva<basic_lc_all<char16_t>> u16all   aliases u8all when UTF-8
  lc_rva<basic_lc_all<char32_t>> u32all

name pool            utf8 name bytes
section image*       per present slot:
                     basic_lc_all<char_type> bytes + payload pool
```

Every `rva` is an absolute file offset resolved with
`lc_get_rva(file_base, field.ref)`; scalars read via `lc_u32(field)` /
`lc_s32`. `rva == 0` means absent. A section image's members point into
its own pool which directly follows the struct — one contiguous
position-independent region.

Field types inside a section:

```
lc_scatter<char_type>              text fields (units = section chars)
lc_scatter<char8_t>                programs (compiled binfmt blobs)
                                   and byte lists (grouping) — len in bytes
lc_scatter<char_type> arr[N]       inline fixed arrays (abday[7], am_pm[2])
lc_scatter<lc_scatter<char_type>>  dynamic string lists — member is
                                   {tbl_rva, count}, tbl = strref[count]
lc_scatter<basic_lc_time_era<char_type>>  era list — {tbl_rva, count},
                                   tbl = basic_lc_time_era[count]
s32                                integer fields
```

`basic_lc_time_era` (48 bytes, in the pool):

```
  s32 direction        +1 | -1
  s32 offset
  s32 start_year
  u32 start_month, start_day
  s32 end_year         i32_MIN = "-*", i32_MAX = "+*"
  u32 end_month, end_day
  lc_scatter<char_type> name
  lc_scatter<char8_t> era_format   compiled binfmt program
```

Categories (members of `basic_lc_all`, master parity): identification,
monetary, numeric, time, messages, paper, telephone, name, address,
measurement, keyboard. `ctype`, `collate` and `xliterate` are not
modeled.

- `messages.yesexpr/noexpr` stay **strings** — they're regex sources for
  matching, not formatting programs
- LEB128 is used ONLY inside compiled `binfmt` program payloads —
  never in the container
- The mapping is private copy-on-write: the image is shared through the
  OS page cache but a process may patch rva fields in its own copy
  without touching the file

## Open items

1. ~~`%y` fallout~~ — resolved: 2-digit year forms compile to their
   4-digit equivalents (`%y`→`%Y`, `%g`→`%G`, `%D`→`%m/%d/%Y`)
2. era `fmt` inside `era_rec` is a pct program — era fmts can themselves
   contain `%E` directives (`%EC%Ey年`); recursion is depth-capped ≤ 8.
3. `field` inside `pct` (chrono nested `{}`) — allowed; depth-capped.
4. iso8601 composite granularity: single `iso8601` code + utc variant +
   `fracsec` — enough, or do we want week-date/ordinal-date codes too?
