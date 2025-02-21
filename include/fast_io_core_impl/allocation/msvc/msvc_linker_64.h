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
#pragma comment(linker, "/alternatename:__imp_?VirtualAlloc@win32@fast_io@@YAPEAXPEAX_KII@Z=__imp_VirtualAlloc")
#pragma comment(linker, "/alternatename:__imp_?VirtualProtect@win32@fast_io@@YAHPEAX_KIPEAI@Z=__imp_VirtualProtect")
#pragma comment(linker, "/alternatename:__imp_?VirtualFree@win32@fast_io@@YAHPEAX_KI@Z=__imp_VirtualFree")
#pragma comment(linker, "/alternatename:__imp_?VirtualQuery@win32@fast_io@@YAHPEBXPEAUmemory_basic_information@12@_K@Z=__imp_VirtualQuery")

#pragma comment(linker, "/alternatename:__imp_?mi_malloc@mimalloc@fast_io@@YAPEAX_K@Z=mi_malloc")
#pragma comment(linker, "/alternatename:__imp_?mi_free@mimalloc@fast_io@@YAXPEAX@Z=mi_free")
#pragma comment(linker, "/alternatename:__imp_?mi_calloc@mimalloc@fast_io@@YAPEAX_K0@Z=mi_calloc")
#pragma comment(linker, "/alternatename:__imp_?mi_realloc@mimalloc@fast_io@@YAPEAXPEAX_K@Z=mi_realloc")
#pragma comment(linker, "/alternatename:__imp_?mi_malloc_aligned@mimalloc@fast_io@@YAPEAX_K0@Z=mi_malloc_aligned")
#pragma comment(linker, "/alternatename:__imp_?mi_ralloc_aligned@mimalloc@fast_io@@YAPEAXPEAX_K1@Z=mi_ralloc_aligned")
#pragma comment(linker, "/alternatename:__imp_?mi_calloc_aligned@mimalloc@fast_io@@YAPEAX_K00@Z=mi_calloc_aligned")
#pragma comment(linker, "/alternatename:__imp_?mi_free_aligned@mimalloc@fast_io@@YAPEAXPEAX_K@Z=mi_free_aligned")

// clang-format on
