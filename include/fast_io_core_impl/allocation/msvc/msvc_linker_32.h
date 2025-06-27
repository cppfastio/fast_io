#pragma once
// clang-format off

#pragma comment(linker, "/alternatename:__imp_?HeapAlloc@win32@fast_io@@YAPAXPAXII@Z=__imp_HeapAlloc")
#pragma comment(linker, "/alternatename:__imp_?HeapFree@win32@fast_io@@YAHPAXI0@Z=__imp_HeapFree")
#pragma comment(linker, "/alternatename:__imp_?GetProcessHeap@win32@fast_io@@YAPAXXZ=__imp_GetProcessHeap")
#pragma comment(linker, "/alternatename:__imp_?HeapReAlloc@win32@fast_io@@YAPAXPAXI0I@Z=__imp_HeapReAlloc")
#pragma comment(linker, "/alternatename:__imp_?HeapSize@win32@fast_io@@YAIPAXIPBX@Z=__imp_HeapSize")
#pragma comment(linker, "/alternatename:__imp_?RtlAllocateHeap@nt@win32@fast_io@@YAPAXPAXII@Z=__imp_RtlAllocateHeap")
#pragma comment(linker, "/alternatename:__imp_?RtlFreeHeap@nt@win32@fast_io@@YAEPAXI0@Z=__imp_RtlFreeHeap")
#pragma comment(linker, "/alternatename:__imp_?RtlGetCurrentPeb@nt@win32@fast_io@@YAPAUpeb@123@XZ=__imp_RtlGetCurrentPeb")
#pragma comment(linker, "/alternatename:__imp_?RtlReAllocateHeap@nt@win32@fast_io@@YAPAXPAXI0I@Z=__imp_RtlReAllocateHeap")
#pragma comment(linker, "/alternatename:__imp_?RtlSizeHeap@nt@win32@fast_io@@YAIPAXI0@Z=__imp_RtlSizeHeap")
#pragma comment(linker, "/alternatename:__imp_?VirtualAlloc@win32@fast_io@@YAPAXPAXIII@Z=__imp_VirtualAlloc")
#pragma comment(linker, "/alternatename:__imp_?VirtualProtect@win32@fast_io@@YAHPAXIIPAI@Z=__imp_VirtualProtect")
#pragma comment(linker, "/alternatename:__imp_?VirtualFree@win32@fast_io@@YAHPAXII@Z=__imp_VirtualFree")
#pragma comment(linker, "/alternatename:__imp_?VirtualQuery@win32@fast_io@@YAHPBXPAUmemory_basic_information@12@I@Z=__imp_VirtualQuery")

#pragma comment(linker, "/alternatename:__imp_?mi_malloc@mimalloc@fast_io@@YAPAXI@Z=mi_malloc")
#pragma comment(linker, "/alternatename:__imp_?mi_free@mimalloc@fast_io@@YAXPAX@Z=mi_free")
#pragma comment(linker, "/alternatename:__imp_?mi_calloc@mimalloc@fast_io@@YAPAXII@Z=mi_calloc")
#pragma comment(linker, "/alternatename:__imp_?mi_realloc@mimalloc@fast_io@@YAPAXPAXI@Z=mi_realloc")
#pragma comment(linker, "/alternatename:__imp_?mi_malloc_aligned@mimalloc@fast_io@@YAPAXII@Z=mi_malloc_aligned")
#pragma comment(linker, "/alternatename:__imp_?mi_ralloc_aligned@mimalloc@fast_io@@YAPAXPAXII@Z=mi_ralloc_aligned")
#pragma comment(linker, "/alternatename:__imp_?mi_calloc_aligned@mimalloc@fast_io@@YAPAXIII@Z=mi_calloc_aligned")
#pragma comment(linker, "/alternatename:__imp_?mi_free_aligned@mimalloc@fast_io@@YAPAXPAXI@Z=mi_free_aligned")
// clang-format on
