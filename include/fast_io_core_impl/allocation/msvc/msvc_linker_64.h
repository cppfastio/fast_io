#pragma once
// clang-format off

#pragma comment(linker, "/alternatename:__imp_?HeapAlloc@win32@fast_io@@YAPEAXPEAXI_K@Z=__imp_HeapAlloc")
#pragma comment(linker, "/alternatename:__imp_?HeapFree@win32@fast_io@@YAHPEAXI0@Z=__imp_HeapFree")
#pragma comment(linker, "/alternatename:__imp_?GetProcessHeap@win32@fast_io@@YAPEAXXZ=__imp_GetProcessHeap")
#pragma comment(linker, "/alternatename:__imp_?HeapReAlloc@win32@fast_io@@YAPEAXPEAXI0_K@Z=__imp_HeapReAlloc")
#pragma comment(linker, "/alternatename:__imp_?HeapSize@win32@fast_io@@YA_KPEAXIPEBX@Z=__imp_HeapSize")
#pragma comment(linker, "/alternatename:__imp_?RtlAllocateHeap@nt@win32@fast_io@@YAPEAXPEAXI_K@Z=__imp_RtlAllocateHeap")
#pragma comment(linker, "/alternatename:__imp_?RtlFreeHeap@nt@win32@fast_io@@YAEPEAXI0@Z=__imp_RtlFreeHeap")
#pragma comment(linker, "/alternatename:__imp_?RtlGetCurrentPeb@nt@win32@fast_io@@YAPEAUpeb@123@XZ=__imp_RtlGetCurrentPeb")
#pragma comment(linker, "/alternatename:__imp_?RtlReAllocateHeap@nt@win32@fast_io@@YAPEAXPEAXI0_K@Z=__imp_RtlReAllocateHeap")
#pragma comment(linker, "/alternatename:__imp_?RtlSizeHeap@nt@win32@fast_io@@YA_KPEAXI0@Z=__imp_RtlSizeHeap")

// clang-format on
