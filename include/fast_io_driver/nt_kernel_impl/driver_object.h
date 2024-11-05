#pragma once

namespace fast_io::win32::nt
{
struct driver_object;
struct device_object;

struct list_entry
{
	list_entry *Flink;
	list_entry *Blink;
};

struct single_list_entry
{
	single_list_entry *Next;
};

struct kdevice_queue
{
	::std::int_least16_t Type;
	::std::int_least16_t Size;
	list_entry DeviceListHead;
	::std::size_t Lock;

#if defined(__x86_64__) || defined(_M_AMD64)

	union
	{
		::std::uint_least8_t Busy;

		struct
		{
			::std::int_least64_t Reserved : 8;
			::std::int_least64_t Hint : 56;
		};
	};

#else

	::std::uint_least8_t Busy;

#endif
};

struct kdevice_queue_entry
{
	list_entry DeviceListEntry;
	::std::uint_least32_t SortKey;
	::std::uint_least8_t Inserted;
};

enum class io_allocation_action
{
	KeepObject = 1,
	DeallocateObject,
	DeallocateObjectKeepRegisters
};

struct dispatcher_header
{
	union
	{
		union
		{
			::std::int_least32_t volatile Lock;
			::std::int_least32_t LockNV;
		} DUMMYUNIONNAME;

		struct
		{
			// Events, Semaphores, Gates, etc.
			::std::uint_least8_t Type; // All (accessible via KOBJECT_TYPE)
			::std::uint_least8_t Signalling;
			::std::uint_least8_t Size;
			::std::uint_least8_t Reserved1;
		} DUMMYSTRUCTNAME;

		struct
		{
			// Timer
			::std::uint_least8_t TimerType;

			union
			{
				::std::uint_least8_t TimerControlFlags;

				struct
				{
					::std::uint_least8_t Absolute : 1;
					::std::uint_least8_t Wake : 1;
					::std::uint_least8_t EncodedTolerableDelay : 6;
				} DUMMYSTRUCTNAME;
			};

			::std::uint_least8_t Hand;

			union
			{
				::std::uint_least8_t TimerMiscFlags;

				struct
				{

#if !defined(KENCODED_TIMER_PROCESSOR)

					::std::uint_least8_t Index : 6;

#else

					::std::uint_least8_t Index : 1;
					::std::uint_least8_t Processor : 5;

#endif

					::std::uint_least8_t Inserted : 1;
					::std::uint_least8_t volatile Expired : 1;
				} DUMMYSTRUCTNAME;
			} DUMMYUNIONNAME;
		} DUMMYSTRUCTNAME2;

		struct
		{ // Timer2
			::std::uint_least8_t Timer2Type;

			union
			{
				::std::uint_least8_t Timer2Flags;

				struct
				{
					::std::uint_least8_t Timer2Inserted : 1;
					::std::uint_least8_t Timer2Expiring : 1;
					::std::uint_least8_t Timer2CancelPending : 1;
					::std::uint_least8_t Timer2SetPending : 1;
					::std::uint_least8_t Timer2Running : 1;
					::std::uint_least8_t Timer2Disabled : 1;
					::std::uint_least8_t Timer2ReservedFlags : 2;
				} DUMMYSTRUCTNAME;
			} DUMMYUNIONNAME;

			::std::uint_least8_t Timer2ComponentId;
			::std::uint_least8_t Timer2RelativeId;
		} DUMMYSTRUCTNAME3;

		struct
		{ // Queue
			::std::uint_least8_t QueueType;

			union
			{
				::std::uint_least8_t QueueControlFlags;

				struct
				{
					::std::uint_least8_t Abandoned : 1;
					::std::uint_least8_t DisableIncrement : 1;
					::std::uint_least8_t QueueReservedControlFlags : 6;
				} DUMMYSTRUCTNAME;
			} DUMMYUNIONNAME;

			::std::uint_least8_t QueueSize;
			::std::uint_least8_t QueueReserved;
		} DUMMYSTRUCTNAME4;

		struct
		{ // Thread
			::std::uint_least8_t ThreadType;
			::std::uint_least8_t ThreadReserved;

			union
			{
				::std::uint_least8_t ThreadControlFlags;

				struct
				{
					::std::uint_least8_t CycleProfiling : 1;
					::std::uint_least8_t CounterProfiling : 1;
					::std::uint_least8_t GroupScheduling : 1;
					::std::uint_least8_t AffinitySet : 1;
					::std::uint_least8_t Tagged : 1;
					::std::uint_least8_t EnergyProfiling : 1;
					::std::uint_least8_t SchedulerAssist : 1;

#if !defined(_X86_)

					::std::uint_least8_t ThreadReservedControlFlags : 1;

#else

					::std::uint_least8_t Instrumented : 1;

#endif

				} DUMMYSTRUCTNAME;
			} DUMMYUNIONNAME;

			union
			{
				::std::uint_least8_t DebugActive;

#if !defined(_X86_)

				struct
				{
					::std::uint_least8_t ActiveDR7 : 1;
					::std::uint_least8_t Instrumented : 1;
					::std::uint_least8_t Minimal : 1;
					::std::uint_least8_t Reserved4 : 2;
					::std::uint_least8_t AltSyscall : 1;
					::std::uint_least8_t Emulation : 1;
					::std::uint_least8_t Reserved5 : 1;
				} DUMMYSTRUCTNAME;

#endif

			} DUMMYUNIONNAME2;
		} DUMMYSTRUCTNAME5;

		struct
		{ // Mutant
			::std::uint_least8_t MutantType;
			::std::uint_least8_t MutantSize;
			::std::uint_least8_t DpcActive;
			::std::uint_least8_t MutantReserved;
		} DUMMYSTRUCTNAME6;
	} DUMMYUNIONNAME;

	::std::int_least32_t SignalState; // Object lock
	list_entry WaitListHead;          // Object lock
};

struct kevent
{
	dispatcher_header Header;
};

struct mdl
{
	mdl *Next;
	::std::int_least16_t Size;
	::std::int_least16_t MdlFlags;

	struct eprocess *Process;
	void *MappedSystemVa; /* see creators for field size annotations. */
	void *StartVa;        /* see creators for validity; could be address 0.  */
	::std::uint_least32_t ByteCount;
	::std::uint_least32_t ByteOffset;
};

using pdriver_control = io_allocation_action (*)(struct device_object *DeviceObject, struct irp *Irp, void *MapRegisterBase, void *Context);

using pkdeferred_routine = void (*)(struct kdpc *Dpc, void *DeferredContext, void *SystemArgument1, void *SystemArgument2);

using pdriver_cancel = void (*)(struct device_object *Dpc, struct irp *Irp);

struct kapc
{
	::std::uint_least8_t Type;
	::std::uint_least8_t AllFlags;
	::std::uint_least8_t Size;
	::std::uint_least8_t SpareByte1;
	::std::uint_least32_t SpareLong0;
	struct _KTHREAD *Thread;
	list_entry ApcListEntry;
	void *Reserved[3];
	void *NormalContext;
	void *SystemArgument1;
	void *SystemArgument2;
	char ApcStateIndex;
	char ApcMode;
	::std::uint_least8_t Inserted;
};

struct irp
{
	::std::int_least16_t Type;
	::std::uint_least16_t Size;

	//
	// Define the common fields used to control the IRP.
	//

	//
	// Define a pointer to the Memory Descriptor List (MDL) for this I/O
	// request.  This field is only used if the I/O is "direct I/O".
	//

	mdl *MdlAddress;

	//
	// Flags word - used to remember various flags.
	//

	::std::uint_least32_t Flags;

	//
	// The following union is used for one of three purposes:
	//
	//    1. This IRP is an associated IRP.  The field is a pointer to a master
	//       IRP.
	//
	//    2. This is the master IRP.  The field is the count of the number of
	//       IRPs which must complete (associated IRPs) before the master can
	//       complete.
	//
	//    3. This operation is being buffered and the field is the address of
	//       the system space buffer.
	//

	union
	{
		irp *MasterIrp;
		::std::int_least32_t IrpCount;
		void *SystemBuffer;
	} AssociatedIrp;

	//
	// Thread list entry - allows queuing the IRP to the thread pending I/O
	// request packet list.
	//

	list_entry ThreadListEntry;

	//
	// I/O status - final status of operation.
	//

	io_status_block IoStatus;

	//
	// Requester mode - mode of the original requester of this operation.
	//

	char RequestorMode;

	//
	// Pending returned - TRUE if pending was initially returned as the
	// status for this packet.
	//

	::std::uint_least8_t PendingReturned;

	//
	// Stack state information.
	//

	char StackCount;
	char CurrentLocation;

	//
	// Cancel - packet has been canceled.
	//

	::std::uint_least8_t Cancel;

	//
	// Cancel Irql - Irql at which the cancel spinlock was acquired.
	//

	::std::uint_least8_t CancelIrql;

	//
	// ApcEnvironment - Used to save the APC environment at the time that the
	// packet was initialized.
	//

	char ApcEnvironment;

	//
	// Allocation control flags.
	//

	::std::uint_least8_t AllocationFlags;

	//
	// User parameters.
	//

	union
	{
		io_status_block *UserIosb;

		//
		// Context used when the Irp is managed by IoRing and is used by IoRing.
		// UserIosb is used to cancel an Irp, so sharing space with UserIosb
		// let IoRing cancel an Irp based on its context.
		//

		void *IoRingContext;
	};

	kevent *UserEvent;

	union
	{
		struct
		{
			union
			{
				pio_apc_routine UserApcRoutine;
				void *IssuingProcess;
			};

			union
			{
				void *UserApcContext;

				//
				// IoRing object that rolled this Irp, if any.  The completion
				// is processed through this IoRing object.  UserApcRoutine and
				// UserApcContext is not supported when issuing IOs through an
				// IoRing so we union this with UserApcContext.  We did not use
				// UserApcRoutine because IssuingProcess use the same location
				// and is used when an Irp is queued to FileObject and when the
				// Irp is managed by IoRing it is queued to the FileObject.
				//

				struct _IORING_OBJECT *IoRing;
			};
		} AsynchronousParameters;

		large_integer AllocationSize;
	} Overlay;

	//
	// CancelRoutine - Used to contain the address of a cancel routine supplied
	// by a device driver when the IRP is in a cancelable state.
	//

	pdriver_cancel CancelRoutine;

	//
	// Note that the UserBuffer parameter is outside of the stack so that I/O
	// completion can copy data back into the user's address space without
	// having to know exactly which service was being invoked.  The length
	// of the copy is stored in the second half of the I/O status block. If
	// the UserBuffer field is NULL, then no copy is performed.
	//

	void *UserBuffer;

	//
	// Kernel structures
	//
	// The following section contains kernel structures which the IRP needs
	// in order to place various work information in kernel controller system
	// queues.  Because the size and alignment cannot be controlled, they are
	// placed here at the end so they just hang off and do not affect the
	// alignment of other fields in the IRP.
	//

	union
	{

		struct
		{

			union
			{

				//
				// DeviceQueueEntry - The device queue entry field is used to
				// queue the IRP to the device driver device queue.
				//

				kdevice_queue_entry DeviceQueueEntry;

				struct
				{

					//
					// The following are available to the driver to use in
					// whatever manner is desired, while the driver owns the
					// packet.
					//

					void *DriverContext[4];
				};
			};

			//
			// Thread - pointer to caller's Thread Control Block.
			//

			struct ethread *Thread;

			//
			// Auxiliary buffer - pointer to any auxiliary buffer that is
			// required to pass information to a driver that is not contained
			// in a normal buffer.
			//

			char *AuxiliaryBuffer;

			//
			// The following unnamed structure must be exactly identical
			// to the unnamed structure used in the minipacket header used
			// for completion queue entries.
			//

			struct
			{

				//
				// List entry - used to queue the packet to completion queue, among
				// others.
				//

				list_entry ListEntry;

				union
				{

					//
					// Current stack location - contains a pointer to the current
					// IO_STACK_LOCATION structure in the IRP stack.  This field
					// should never be directly accessed by drivers.  They should
					// use the standard functions.
					//

					struct _IO_STACK_LOCATION *CurrentStackLocation;

					//
					// Minipacket type.
					//

					::std::uint_least32_t PacketType;
				};
			};

			//
			// Original file object - pointer to the original file object
			// that was used to open the file.  This field is owned by the
			// I/O system and should not be used by any other drivers.
			//

			struct file_object *OriginalFileObject;

		} Overlay;

		//
		// APC - This APC control block is used for the special kernel APC as
		// well as for the caller's APC, if one was specified in the original
		// argument list.  If so, then the APC is reused for the normal APC for
		// whatever mode the caller was in and the "special" routine that is
		// invoked before the APC gets control simply deallocates the IRP.
		//

		kapc Apc;

		//
		// CompletionKey - This is the key that is used to distinguish
		// individual I/O operations initiated on a single file handle.
		//

		void *CompletionKey;

	} Tail;
};

struct kdpc
{
	union
	{
		::std::uint_least32_t TargetInfoAsUlong;

		struct
		{
			::std::uint_least8_t Type;
			::std::uint_least8_t Importance;
			::std::uint_least16_t volatile Number;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;

	single_list_entry DpcListEntry;
	::std::size_t ProcessorHistory;
	pkdeferred_routine DeferredRoutine;
	void *DeferredContext;
	void *SystemArgument1;
	void *SystemArgument2;
	void *DpcData;
};

struct wait_context_block
{
	union
	{
		kdevice_queue_entry WaitQueueEntry;

		struct
		{
			list_entry DmaWaitEntry;
			::std::uint_least32_t NumberOfChannels;
			::std::uint_least32_t SyncCallback : 1;
			::std::uint_least32_t DmaContext : 1;
			::std::uint_least32_t ZeroMapRegisters : 1;
			::std::uint_least32_t Reserved : 9;
			::std::uint_least32_t NumberOfRemapPages : 20;
		};
	};

	pdriver_control DeviceRoutine;
	void *DeviceContext;
	::std::uint_least32_t NumberOfMapRegisters;
	void *DeviceObject;
	void *CurrentIrp;
	kdpc *BufferChainingDpc;
};

struct device_object
{
	::std::int_least16_t Type;
	::std::uint_least16_t Size;
	::std::int_least32_t ReferenceCount;
	driver_object *DriverObject;
	device_object *NextDevice;
	device_object *AttachedDevice;
	struct irp *CurrentIrp;
	struct io_timer *Timer;
	::std::uint_least32_t Flags;
	::std::uint_least32_t Characteristics;
	struct vpb *Vpb;
	void *DeviceExtension;
	::std::uint_least32_t DeviceType;
	char StackSize;

	union
	{
		list_entry ListEntry;
		wait_context_block Wcb;
	} Queue;

	::std::uint_least32_t AlignmentRequirement;
	kdevice_queue DeviceQueue;
	kdpc Dpc;
	::std::uint_least32_t ActiveThreadCount;
	void *SecurityDescriptor;
	kevent DeviceLock;
	::std::uint_least16_t SectorSize;
	::std::uint_least16_t Spare1;
	struct devobj_extension *DeviceObjectExtension;
	void *Reserved;
};

struct device_extension
{
	device_object *pDevice;
	::fast_io::win32::nt::unicode_string ustrDeviceName;
	::fast_io::win32::nt::unicode_string ustrSymLinkName;
};

struct driver_object
{
	::std::int_least16_t Type;
	::std::uint_least16_t Size;
	device_object *DeviceObject;
	::std::uint_least32_t Flags;
	void *DriverStart;
	::std::uint_least32_t DriverSize;
	void *DriverSection;
	void *DriverExtension;
	::fast_io::win32::nt::unicode_string DriverName;
	::fast_io::win32::nt::unicode_string *HardwareDatabase;
	void *FastIoDispatch;
	void *DriverInit;
	void *DriverStartIo;
	void *DriverUnload;
	void *MajorFunction[28];
};

} // namespace fast_io::win32::nt
