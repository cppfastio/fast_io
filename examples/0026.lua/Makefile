SYSROOT = $(HOME)/cross/wasm32-wasi
WASM2LUA = wasm2lua
CXX = clang++
LUAJIT = luajit

hello_fast_io.lua:hello_fast_io
	$(WASM2LUA) hello_fast_io hello_fast_io.lua -m, --minify 3
hello_fast_io:hello_fast_io.cc
	$(CXX) -o hello_fast_io hello_fast_io.cc -Ofast -std=c++2b -s -flto -fuse-ld=lld -I../../include -fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-ident --target=wasm32-wasi --sysroot=$(SYSROOT)
run:main.lua hello_fast_io.lua wasilib.lua
	luajit main.lua
clean:
	rm hello_fast_io hello_fast_io.lua
