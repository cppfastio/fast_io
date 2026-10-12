#!/usr/bin/env python3
"""Fix string-literal misuse in fast_io print-family calls.

For every call to print/perr/debug_print/panic/debug_perr (and their *ln
variants) in the given source trees:

  * runs of adjacent string-literal arguments are merged into a single
    argument ("a", "b" -> "a" "b"),
  * a trailing string literal on an *ln function is folded into the non-ln
    variant (println("x") -> print("x\n")).

These are the two patterns the FAST_IO_WARN_CONSECUTIVE_LITERALS and
FAST_IO_WARN_TRAILING_LITERAL diagnostics (io.h) flag.

Caveats: detection is textual — arguments that expand to literals via the
preprocessor (e.g. __clang_version__), arguments of character-array type that
are not written as literals (e.g. an `auto const&` lambda parameter bound to
a literal), and literal runs separated by preprocessor directives must still
be fixed by hand. Verify afterwards by compiling with
-DFAST_IO_WARN_CONSECUTIVE_LITERALS=1 -DFAST_IO_WARN_TRAILING_LITERAL=1.

Usage: fix_literal_args.py [--apply] [paths...]
Without --apply it prints the edits it would make.
"""

import re
import glob
import os
import sys

LNMAP = {
	'println': 'print',
	'perrln': 'perr',
	'debug_println': 'debug_print',
	'panicln': 'panic',
	'debug_perrln': 'debug_perr',
}
FNAMES = set(LNMAP) | {'print', 'perr', 'debug_print', 'panic', 'debug_perr'}


def skip_ws_comments(t, i, n):
	while i < n:
		if t[i].isspace():
			i += 1
		elif t[i:i + 2] == '//':
			e = t.find('\n', i)
			i = n if e < 0 else e
		elif t[i:i + 2] == '/*':
			e = t.find('*/', i)
			i = n if e < 0 else e + 2
		else:
			break
	return i


def scan_str(t, i):
	n = len(t)
	i += 1
	while i < n:
		if t[i] == '\\':
			i += 2
			continue
		if t[i] == '"':
			return i + 1
		i += 1
	return n


def scan_charlit(t, i):
	n = len(t)
	i += 1
	while i < n:
		if t[i] == '\\':
			i += 2
			continue
		if t[i] == "'":
			return i + 1
		i += 1
	return n


def scan_raw(t, i):
	j = t.find('(', i)
	if j < 0:
		return i + 1
	delim = t[i + 2:j]
	e = t.find(')' + delim + '"', j)
	return len(t) if e < 0 else e + len(delim) + 2


def skip_token(t, i, n):
	c = t[i]
	if c == '"':
		return scan_str(t, i)
	if c == "'":
		return scan_charlit(t, i)
	if c == 'R' and t[i + 1:i + 2] == '"':
		return scan_raw(t, i)
	return i


def find_close(t, p):
	depth = 0
	i = p
	n = len(t)
	while i < n:
		i = skip_ws_comments(t, i, n)
		if i >= n:
			break
		r = skip_token(t, i, n)
		if r != i:
			i = r
			continue
		c = t[i]
		if c in '([{':
			depth += 1
		elif c in ')]}':
			depth -= 1
			if depth == 0:
				return i
		i += 1
	return -1


def split_args(t, a, b):
	"""Split t[a:b] on top-level commas; returns list of arg spans."""
	parts = []
	i = a
	depth = 0
	seg = a
	n = b
	while i < n:
		i = skip_ws_comments(t, i, n)
		if i >= n:
			break
		r = skip_token(t, i, n)
		if r != i:
			i = r
			continue
		c = t[i]
		if c in '([{':
			depth += 1
		elif c in ')]}':
			depth -= 1
		elif c == ',' and depth == 0:
			parts.append((seg, i))
			i += 1
			seg = i
			continue
		i += 1
	parts.append((seg, n))
	return parts


def literal_spans(s):
	"""Return list of (start, end, is_raw) spans for every string literal in
	s, or None if s contains anything besides literals/ws/comments."""
	spans = []
	i = 0
	n = len(s)
	while True:
		i = skip_ws_comments(s, i, n)
		if i >= n:
			break
		m = re.match(r'(u8|u|U|L)?(R)?"', s[i:])
		if not m:
			return None
		if m.group(2):
			j = s.find('(', i + m.end())
			if j < 0:
				return None
			delim = s[i + m.end():j]
			e = s.find(')' + delim + '"', j)
			if e < 0:
				return None
			end = e + len(delim) + 2
			spans.append((i, end, True))
			i = end
		else:
			end = scan_str(s, i + m.end() - 1)
			spans.append((i, end, False))
			i = end
	return spans if spans else None


def iter_calls(text):
	"""Yield (name_start, name, paren_pos) for name( calls outside
	strings/comments, excluding .name( and ->name( member access."""
	i = 0
	n = len(text)
	while i < n:
		i = skip_ws_comments(text, i, n)
		if i >= n:
			break
		r = skip_token(text, i, n)
		if r != i:
			i = r
			continue
		c = text[i]
		if c.isalpha() or c == '_':
			j = i + 1
			while j < n and (text[j].isalnum() or text[j] == '_'):
				j += 1
			name = text[i:j]
			p = skip_ws_comments(text, j, n)
			if p < n and text[p] == '(' and name in FNAMES:
				k = i - 1
				while k >= 0 and text[k] in ' \t':
					k -= 1
				member = (k >= 0 and text[k] == '.') or \
					(k >= 1 and text[k - 1:k + 1] == '->')
				if not member:
					yield (i, name, p)
			i = j
			continue
		i += 1


def fold_newline(argtext):
	"""Append \n to the trailing literal: inside the closing quote for
	ordinary literals, or as an adjacent "\n" for raw literals."""
	spans = literal_spans(argtext)
	s, e, is_raw = spans[-1]
	if is_raw:
		return argtext[:e] + ' "\\n"' + argtext[e:]
	return argtext[:e - 1] + '\\n' + argtext[e - 1:]


def process_file(full):
	text = open(full).read()
	edits = []
	for ns, fname, p in iter_calls(text):
		close = find_close(text, p)
		if close < 0:
			continue
		# preprocessor directives inside the argument list cannot be
		# rewritten safely (they must stay at line start) -- skip the call
		if re.search(r'(?m)^\s*#', text[p:close]):
			continue
		args = split_args(text, p + 1, close)
		argtxt = [text[s:e] for s, e in args]
		if len(argtxt) == 1 and not argtxt[0].strip():
			continue
		lit = [literal_spans(a) is not None for a in argtxt]
		# merge adjacent literal args into a single adjacent-concat literal
		out = []
		i = 0
		changed = False
		while i < len(argtxt):
			cur = argtxt[i]
			if lit[i] and i + 1 < len(argtxt) and lit[i + 1]:
				run = [cur]
				j = i + 1
				while j < len(argtxt) and lit[j]:
					run.append(argtxt[j])
					j += 1
				out.append(' '.join(x.strip() for x in run))
				i = j
				changed = True
			else:
				out.append(cur)
				i += 1
		nfname = fname
		if fname in LNMAP and lit[-1]:
			out[-1] = fold_newline(out[-1])
			nfname = LNMAP[fname]
			changed = True
		if changed:
			edits.append((ns, close + 1, nfname + '(' +
						  ', '.join(x.strip() for x in out) + ')'))
	return edits


def main():
	apply = '--apply' in sys.argv
	paths = [p for p in sys.argv[1:] if p != '--apply']
	if not paths:
		root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
		paths = [os.path.join(root, 'tests'), os.path.join(root, 'examples')]
	files = []
	for p in paths:
		if os.path.isdir(p):
			files += glob.glob(os.path.join(p, '**', '*.cc'), recursive=True)
		else:
			files.append(p)
	total = 0
	nfiles = 0
	for f in sorted(files):
		edits = process_file(f)
		if not edits:
			continue
		total += len(edits)
		nfiles += 1
		if apply:
			text = open(f).read()
			for s, e, ntxt in sorted(edits, reverse=True):
				text = text[:s] + ntxt + text[e:]
			open(f, 'w').write(text)
		else:
			text = open(f).read()
			print('====', f)
			for s, e, ntxt in edits:
				print('  -', text[s:e].replace('\n', ' ')[:110])
				print('  +', ntxt[:110])
	print(total, 'call sites in', nfiles, 'files' +
		  ('' if apply else ' (dry run, pass --apply to write)'))


if __name__ == '__main__':
	main()
