#pragma once
// clang-format off

#pragma comment(linker, "/alternatename:?HeapAlloc@win32@fast_io@@$$hYAPEAXPEAXI_K@Z=#HeapAlloc")
#pragma comment(linker, "/alternatename:?HeapFree@win32@fast_io@@$$hYAHPEAXI0@Z=#HeapFree")
#pragma comment(linker, "/alternatename:?GetProcessHeap@win32@fast_io@@$$hYAPEAXXZ=#GetProcessHeap")
#pragma comment(linker, "/alternatename:?HeapReAlloc@win32@fast_io@@$$hYAPEAXPEAXI0_K@Z=#HeapReAlloc")
#pragma comment(linker, "/alternatename:?HeapSize@win32@fast_io@@$$hYA_KPEAXIPEBX@Z=#HeapSize")
#pragma comment(linker, "/alternatename:?RtlAllocateHeap@nt@win32@fast_io@@$$hYAPEAXPEAXI_K@Z=#RtlAllocateHeap")
#pragma comment(linker, "/alternatename:?RtlFreeHeap@nt@win32@fast_io@@$$hYAEPEAXI0@Z=#RtlFreeHeap")
#pragma comment(linker, "/alternatename:?RtlGetCurrentPeb@nt@win32@fast_io@@$$hYAPEAUpeb@123@XZ=#RtlGetCurrentPeb")
#pragma comment(linker, "/alternatename:?RtlReAllocateHeap@nt@win32@fast_io@@$$hYAPEAXPEAXI0_K@Z=#RtlReAllocateHeap")
#pragma comment(linker, "/alternatename:?RtlSizeHeap@nt@win32@fast_io@@$$hYA_KPEAXI0@Z=#RtlSizeHeap")
#pragma comment(linker, "/alternatename:?VirtualAlloc@win32@fast_io@@$$hYAPEAXPEAX_KII@Z=#VirtualAlloc")
#pragma comment(linker, "/alternatename:?VirtualProtect@win32@fast_io@@$$hYAHPEAX_KIPEAI@Z=#VirtualProtect")
#pragma comment(linker, "/alternatename:?VirtualFree@win32@fast_io@@$$hYAHPEAX_KI@Z=#VirtualFree")
#pragma comment(linker, "/alternatename:?VirtualQuery@win32@fast_io@@$$hYAHPEBXPEAUmemory_basic_information@12@_K@Z=#VirtualQuery")

#pragma comment(linker, "/alternatename:?mi_malloc@mimalloc@fast_io@@$$hYAPEAX_K@Z=mi_malloc")
#pragma comment(linker, "/alternatename:?mi_free@mimalloc@fast_io@@$$hYAXPEAX@Z=mi_free")
#pragma comment(linker, "/alternatename:?mi_calloc@mimalloc@fast_io@@$$hYAPEAX_K0@Z=mi_calloc")
#pragma comment(linker, "/alternatename:?mi_realloc@mimalloc@fast_io@@$$hYAPEAXPEAX_K@Z=mi_realloc")
#pragma comment(linker, "/alternatename:?mi_malloc_aligned@mimalloc@fast_io@@$$hYAPEAX_K0@Z=mi_malloc_aligned")
#pragma comment(linker, "/alternatename:?mi_ralloc_aligned@mimalloc@fast_io@@$$hYAPEAXPEAX_K1@Z=mi_ralloc_aligned")
#pragma comment(linker, "/alternatename:?mi_calloc_aligned@mimalloc@fast_io@@$$hYAPEAX_K00@Z=mi_calloc_aligned")
#pragma comment(linker, "/alternatename:?mi_free_aligned@mimalloc@fast_io@@$$hYAPEAXPEAX_K@Z=mi_free_aligned")

// clang-format on
