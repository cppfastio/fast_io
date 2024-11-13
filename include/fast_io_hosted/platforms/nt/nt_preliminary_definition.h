#pragma once

namespace fast_io::win32::nt
{
struct peb_ldr_data;
struct rtl_user_process_parameters;
using pps_post_process_init_routine = void(
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__stdcall
#elif (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__attribute__((__stdcall__))
#endif
		*)(void) noexcept;

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

struct unicode_string
{
	::std::uint_least16_t Length;
	::std::uint_least16_t MaximumLength;
	char16_t *Buffer;
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

struct client_id
{
	void *UniqueProcess;
	void *UniqueThread;
};

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

}
