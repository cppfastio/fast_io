#pragma once

#if defined(_MSC_VER) && !defined(_KERNEL_MODE) && !defined(_WIN32_WINDOWS)
#pragma comment(lib, "ntdll.lib")
#endif

#include <cstdint>
#include "common.h"

namespace fast_io::win32::nt
{
struct peb_ldr_data;
struct rtl_user_process_parameters;
using pps_post_process_init_routine = void(FAST_IO_STDCALL *)(void) noexcept;

struct peb
{
	char unsigned InheritedAddressSpace;
	char unsigned ReadImageFileExecOptions;
	char unsigned BeingDebugged;
	char unsigned SpareBool;
	void *Mutant;
	void *ImageBaseAddress;
	peb_ldr_data *Ldr;
	rtl_user_process_parameters *ProcessParameters; // PRTL_USER_PROCESS_PARAMETERS
	void *SubSystemData;
	void *ProcessHeap;
	void *FastPebLock;
	void *FastPebLockRoutine;
	void *FastPebUnlockRoutine;
	::std::uint_least32_t Reserved6;
	void *Reserved7;
	::std::uint_least32_t Reserved8;
	::std::uint_least32_t AtlThunkSListPtr32;
	void *Reserved9[45];
	char unsigned Reserved10[96];
	pps_post_process_init_routine PostProcessInitRoutine;
	char unsigned Reserved11[128];
	void *Reserved12[1];
	::std::uint_least32_t SessionId;
};

struct nt_tib
{
	struct exception_registration_record *ExceptionList;
	void *StackBase;
	void *StackLimit;
	void *SubSystemTib;
#if defined(_MSC_EXTENSIONS)
	union
	{
		void *FiberData;
		::std::uint_least32_t Version;
	};
#else
	void *FiberData;
#endif
	void *ArbitraryUserPointer;
	nt_tib *Self;
};

struct guid
{
	::std::uint_least32_t Data1;
	::std::uint_least16_t Data2;
	::std::uint_least16_t Data3;
	::std::uint_least8_t Data4[8];
};

struct list_entry
{
	list_entry *Flink;
	list_entry *Blink;
};

struct activation_context_stack
{
	struct rtl_activation_context_stack_frame *ActiveFrame;
	list_entry FrameListCache;
	::std::uint_least32_t Flags;
	::std::uint_least32_t NextCookieSequenceNumber;
	::std::uint_least32_t StackId;
};

struct processor_number
{
	::std::uint_least16_t Group;
	::std::uint_least8_t Number;
	::std::uint_least8_t Reserved;
};

struct gdi_teb_batch
{
	::std::uint_least32_t Offset;
	::std::size_t HDC;
	::std::uint_least32_t Buffer[310];
};

// NOLINTBEGIN(*-optin.performance.Padding)
struct teb
{
	nt_tib NtTib;

	void *EnvironmentPointer;
	client_id ClientId;
	void *ActiveRpcHandle;
	void *ThreadLocalStoragePointer;
	peb *ProcessEnvironmentBlock;

	::std::uint_least32_t LastErrorValue;
	::std::uint_least32_t CountOfOwnedCriticalSections;
	void *CsrClientThread;
	void *Win32ThreadInfo;
	::std::uint_least32_t User32Reserved[26];
	::std::uint_least32_t UserReserved[5];
	void *WOW32Reserved;
	::std::uint_least32_t CurrentLocale;
	::std::uint_least32_t FpSoftwareStatusRegister;
	void *ReservedForDebuggerInstrumentation[16];
#ifdef _WIN64
	void *SystemReserved1[30];
#else
	void *SystemReserved1[26];
#endif
	char PlaceholderCompatibilityMode;
	char PlaceholderReserved[11];
	::std::uint_least32_t ProxiedProcessId;
	activation_context_stack ActivationStack;

	::std::uint_least8_t WorkingOnBehalfTicket[8];
	::std::uint_least32_t ExceptionCode;

	activation_context_stack *ActivationContextStackPointer;
	::std::size_t InstrumentationCallbackSp;
	::std::size_t InstrumentationCallbackPreviousPc;
	::std::size_t InstrumentationCallbackPreviousSp;
#ifdef _WIN64
	::std::uint_least32_t TxFsContext;
#endif
	::std::uint_least8_t InstrumentationCallbackDisabled;
#ifndef _WIN64
	::std::uint_least8_t SpareBytes[23];
	::std::uint_least32_t TxFsContext;
#endif
	gdi_teb_batch GdiTebBatch;
	client_id RealClientId;
	void *GdiCachedProcessHandle;
	::std::uint_least32_t GdiClientPID;
	::std::uint_least32_t GdiClientTID;
	void *GdiThreadLocalInfo;
	::std::size_t Win32ClientInfo[62];
	void *glDispatchTable[233];
	::std::size_t glReserved1[29];
	void *glReserved2;
	void *glSectionInfo;
	void *glSection;
	void *glTable;
	void *glCurrentRC;
	void *glContext;

	::std::uint_least32_t LastStatusValue;
	unicode_string StaticUnicodeString;
	char16_t StaticUnicodeBuffer[261];

	void *DeallocationStack;
	void *TlsSlots[64];
	list_entry TlsLinks;

	void *Vdm;
	void *ReservedForNtRpc;
	void *DbgSsReserved[2];

	::std::uint_least32_t HardErrorMode;
#ifdef _WIN64
	void *Instrumentation[11];
#else
	void *Instrumentation[9];
#endif
	guid ActivityId;

	void *SubProcessTag;
	void *PerflibData;
	void *EtwTraceData;
	void *WinSockData;
	::std::uint_least32_t GdiBatchCount;

	union
	{
		processor_number CurrentIdealProcessor;
		::std::uint_least32_t IdealProcessorValue;
		struct
		{
			::std::uint_least8_t ReservedPad0;
			::std::uint_least8_t ReservedPad1;
			::std::uint_least8_t ReservedPad2;
			::std::uint_least8_t IdealProcessor;
		} s1;
	} u1;

	::std::uint_least32_t GuaranteedStackBytes;
	void *ReservedForPerf;
	void *ReservedForOle;
	::std::uint_least32_t WaitingOnLoaderLock;
	void *SavedPriorityState;
	::std::size_t ReservedForCodeCoverage;
	void *ThreadPoolData;
	void **TlsExpansionSlots;
#ifdef _WIN64
	void *DeallocationBStore;
	void *BStoreLimit;
#endif
	::std::uint_least32_t MuiGeneration;
	::std::uint_least32_t IsImpersonating;
	void *NlsCache;
	void *pShimData;
	::std::uint_least16_t HeapVirtualAffinity;
	::std::uint_least16_t LowFragHeapDataSlot;
	void *CurrentTransactionHandle;
	struct teb_active_frame *ActiveFrame;
	void *FlsData;

	void *PreferredLanguages;
	void *UserPrefLanguages;
	void *MergedPrefLanguages;
	::std::uint_least32_t MuiImpersonation;

	union
	{
		::std::uint_least16_t CrossTebFlags;
		::std::uint_least16_t SpareCrossTebBits : 16;
	} u2;
	union
	{
		::std::uint_least16_t SameTebFlags;
		struct
		{
			::std::uint_least16_t SafeThunkCall : 1;
			::std::uint_least16_t InDebugPrint : 1;
			::std::uint_least16_t HasFiberData : 1;
			::std::uint_least16_t SkipThreadAttach : 1;
			::std::uint_least16_t WerInShipAssertCode : 1;
			::std::uint_least16_t RanProcessInit : 1;
			::std::uint_least16_t ClonedThread : 1;
			::std::uint_least16_t SuppressDebugMsg : 1;
			::std::uint_least16_t DisableUserStackWalk : 1;
			::std::uint_least16_t RtlExceptionAttached : 1;
			::std::uint_least16_t InitialThread : 1;
			::std::uint_least16_t SessionAware : 1;
			::std::uint_least16_t LoadOwner : 1;
			::std::uint_least16_t LoaderWorker : 1;
			::std::uint_least16_t SkipLoaderInit : 1;
			::std::uint_least16_t SpareSameTebBits : 1;
		} s2;
	} u3;

	void *TxnScopeEnterCallback;
	void *TxnScopeExitCallback;
	void *TxnScopeContext;
	::std::uint_least32_t LockCount;
	::std::int_least32_t WowTebOffset;
	void *ResourceRetValue;
	void *ReservedForWdf;
	::std::uint_least64_t ReservedForCrt;
	guid EffectiveContainerId;
	::std::uint_least64_t LastSleepCounter; // Win11
	::std::uint_least32_t SpinCallCount;
	::std::uint_least64_t ExtendedFeatureDisableMask;
};
// NOLINTEND(*-optin.performance.Padding)

FAST_IO_DLLIMPORT FAST_IO_GNU_MALLOC void *FAST_IO_WINSTDCALL RtlAllocateHeap(void *, ::std::uint_least32_t, ::std::size_t) noexcept FAST_IO_WINSTDCALL_RENAME(RtlAllocateHeap, 12);
FAST_IO_DLLIMPORT char unsigned FAST_IO_WINSTDCALL RtlFreeHeap(void *, ::std::uint_least32_t, void *) noexcept FAST_IO_WINSTDCALL_RENAME(RtlFreeHeap, 12);
FAST_IO_DLLIMPORT peb *FAST_IO_WINSTDCALL RtlGetCurrentPeb() noexcept FAST_IO_WINSTDCALL_RENAME(RtlGetCurrentPeb, 0);
FAST_IO_DLLIMPORT void *FAST_IO_WINSTDCALL RtlReAllocateHeap(void *, ::std::uint_least32_t, void *, ::std::size_t) noexcept FAST_IO_WINSTDCALL_RENAME(RtlReAllocateHeap, 16);
FAST_IO_DLLIMPORT ::std::size_t FAST_IO_WINSTDCALL RtlSizeHeap(void *, ::std::uint_least32_t, void *) noexcept FAST_IO_WINSTDCALL_RENAME(RtlSizeHeap, 12);

#if (defined(__GNUC__) || defined(__clang__)) && \
	(defined(__aarch64__) || defined(__arm64ec__))
register ::fast_io::win32::nt::teb *fast_io_nt_current_teb __asm__("x18");
#endif

FAST_IO_GNU_ALWAYS_INLINE_ARTIFICIAL_CONST inline peb *nt_get_current_peb() noexcept
{
#if (defined(__GNUC__) || defined(__clang__))
#if defined(__aarch64__) || defined(__arm64ec__)
	return ::fast_io::win32::nt::fast_io_nt_current_teb->ProcessEnvironmentBlock;
#elif defined(__i386__) || defined(__x86_64__)
	if constexpr (sizeof(::std::size_t) == sizeof(::std::uint_least64_t))
	{
		peb *ppeb;
		__asm__("{movq\t%%gs:0x60, %0|mov\t%0, %%gs:[0x60]}" : "=r"(ppeb));
		return ppeb;
	}
	else if constexpr (sizeof(::std::size_t) == sizeof(::std::uint_least32_t))
	{
		peb *ppeb;
		__asm__("{movl\t%%fs:0x30, %0|mov\t%0, %%fs:[0x30]}" : "=r"(ppeb));
		return ppeb;
	}
	else
	{
		return ::fast_io::win32::nt::RtlGetCurrentPeb();
	}
#else
	if constexpr (sizeof(::std::size_t) == sizeof(::std::uint_least32_t))
	{
		teb *pteb;
		__asm__("MRC p15, 0, %0, c13, c0, 2" : "=r"(pteb));
		return pteb->ProcessEnvironmentBlock;
	}
	else
	{
		return ::fast_io::win32::nt::RtlGetCurrentPeb();
	}
#endif
#elif defined(_MSC_VER)
#if defined(_M_ARM64) || defined(_M_ARM64EC)
	return reinterpret_cast<::fast_io::win32::nt::teb *>(::fast_io::intrinsics::msvc::arm::__getReg(18))->ProcessEnvironmentBlock;
#elif defined(_M_AMD64)
	return reinterpret_cast<::fast_io::win32::nt::peb *>(::fast_io::intrinsics::msvc::x86::__readgsqword(0x60));
#elif defined(_M_IX86)
	return reinterpret_cast<::fast_io::win32::nt::peb *>(::fast_io::intrinsics::msvc::x86::__readfsdword(0x30));
#else
	return reinterpret_cast<::fast_io::win32::nt::teb *>(::fast_io::intrinsics::msvc::arm::_MoveFromCoprocessor(15, 0, 13, 0, 2))->ProcessEnvironmentBlock;
#endif
#else
	return ::fast_io::win32::nt::RtlGetCurrentPeb();
#endif
}

FAST_IO_GNU_ALWAYS_INLINE_ARTIFICIAL_CONST inline teb *nt_current_teb() noexcept
{
#if (defined(__GNUC__) || defined(__clang__))
#if defined(__aarch64__) || defined(__arm64ec__)
	return ::fast_io::win32::nt::fast_io_nt_current_teb;
#elif defined(__i386__) || defined(__x86_64__)
	if constexpr (sizeof(::std::size_t) == sizeof(::std::uint_least64_t))
	{
		teb *pteb;
		__asm__("{movq\t%%gs:0x30, %0|mov\t%0, %%gs:[0x30]}" : "=r"(pteb));
		return pteb;
	}
	else if constexpr (sizeof(::std::size_t) == sizeof(::std::uint_least32_t))
	{
		teb *pteb;
		__asm__("{movl\t%%fs:0x18, %0|mov\t%0, %%fs:[0x18]}" : "=r"(pteb));
		return pteb;
	}
	else
	{
		::fast_io::fast_terminate(); // Unsupported architecture
	}
#else
	if constexpr (sizeof(::std::size_t) == sizeof(::std::uint_least32_t))
	{
		teb *pteb;
		__asm__("MRC p15, 0, %0, c13, c0, 2" : "=r"(pteb));
		return pteb;
	}
	else
	{
		::fast_io::fast_terminate(); // Unsupported architecture
	}
#endif
#elif defined(_MSC_VER)
#if defined(_M_ARM64) || defined(_M_ARM64EC)
	return reinterpret_cast<::fast_io::win32::nt::teb *>(::fast_io::intrinsics::msvc::arm::__getReg(18));
#elif defined(_M_AMD64)
	return reinterpret_cast<::fast_io::win32::nt::teb *>(::fast_io::intrinsics::msvc::x86::__readgsqword(0x30));
#elif defined(_M_IX86)
	return reinterpret_cast<::fast_io::win32::nt::teb *>(::fast_io::intrinsics::msvc::x86::__readfsdword(0x18));
#else
	return reinterpret_cast<::fast_io::win32::nt::teb *>(::fast_io::intrinsics::msvc::arm::_MoveFromCoprocessor(15, 0, 13, 0, 2));
#endif
#else
	::fast_io::fast_terminate(); // Unsupported compiler
#endif
}

FAST_IO_GNU_ALWAYS_INLINE_ARTIFICIAL_CONST inline void *rtl_get_process_heap() noexcept
{
	return ::fast_io::win32::nt::nt_get_current_peb()->ProcessHeap;
}

} // namespace fast_io::win32::nt
