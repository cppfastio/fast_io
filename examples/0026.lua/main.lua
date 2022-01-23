if _G.bit == nil then
	local bit = dofile("luabit.lua")
	_G.bit=bit
end

local wasi = dofile("wasilib.lua")
--[[replace this with your wasi implementation. Just copy it from wasm2lua]]
local module = dofile("hello_fast_io.lua")

module.imports.wasi_snapshot_preview1=wasi(module.memory)

module.init()
module.exports._start()
