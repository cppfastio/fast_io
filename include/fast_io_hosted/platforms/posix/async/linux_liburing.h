#pragma once
/*
 * The liburing part of fast_io's io_uring support: kernel ABI
 * definitions, the three syscalls, the mmap'd ring machinery
 * (queue_init/queue_exit, sq/cq helpers) and the sqe/cqe
 * manipulation functions, all operating on io_uring_ring_state
 * directly. No liburing, no libc io_uring helpers. The
 * fast_io-facing observer layer lives in linux_io_uring.h.
 */

#if defined(__linux__)


namespace fast_io::liburing
{

namespace details
{
inline constexpr ::std::uint_least64_t io_uring_setup_syscall_number{
#if defined(__NR_io_uring_setup)
	__NR_io_uring_setup
#elif defined(__alpha__)
	535
#elif defined(__mips__) && defined(__NR_Linux)
	__NR_Linux + 425
#else
	425
#endif
};
inline constexpr ::std::uint_least64_t io_uring_enter_syscall_number{
#if defined(__NR_io_uring_enter)
	__NR_io_uring_enter
#elif defined(__alpha__)
	536
#elif defined(__mips__) && defined(__NR_Linux)
	__NR_Linux + 426
#else
	426
#endif
};
inline constexpr ::std::uint_least64_t io_uring_register_syscall_number{
#if defined(__NR_io_uring_register)
	__NR_io_uring_register
#elif defined(__alpha__)
	537
#elif defined(__mips__) && defined(__NR_Linux)
	__NR_Linux + 427
#else
	427
#endif
};
} // namespace details


/*
 * Kernel UAPI (stable ABI, mirrored from linux/io_uring.h so neither
 * liburing nor kernel headers are required).
 */
struct sqe_uring_cmd
{
	::std::uint_least32_t cmd_op;
	::std::uint_least32_t pad1;
};
struct sqe_sockopt
{
	::std::uint_least32_t level;
	::std::uint_least32_t optname;
};
struct sqe_addrlen
{
	::std::uint_least16_t addr_len;
	::std::uint_least16_t pad3;
};
struct sqe_addr3_and_pad
{
	::std::uint_least64_t addr3;
	::std::uint_least64_t pad2;
};
struct sqe_attr
{
	::std::uint_least64_t attr_ptr;       /* pointer to attribute information */
	::std::uint_least64_t attr_type_mask; /* bit mask of attributes */
};

struct io_uring_sqe
{
	::std::uint_least8_t opcode;  /* type of operation for this sqe */
	::std::uint_least8_t flags;   /* io_uring_sqe_flag */
	::std::uint_least16_t ioprio; /* ioprio for the request */
	::std::int32_t fd;            /* file descriptor to do IO on */
	union
	{
		::std::uint_least64_t off;   /* offset into file */
		::std::uint_least64_t addr2; /* literal pointer / second address */
		sqe_uring_cmd uring_cmd;
	};
	union
	{
		::std::uint_least64_t addr; /* pointer to buffer or iovecs */
		::std::uint_least64_t splice_off_in;
		sqe_sockopt sockopt;
	};
	::std::uint_least32_t len; /* buffer size or number of iovecs */
	union
	{
		::std::uint_least32_t rw_flags;
		::std::uint_least32_t fsync_flags;
		::std::uint_least16_t poll_events;
		::std::uint_least32_t poll32_events;
		::std::uint_least32_t sync_range_flags;
		::std::uint_least32_t msg_flags;
		::std::uint_least32_t timeout_flags;
		::std::uint_least32_t accept_flags;
		::std::uint_least32_t cancel_flags;
		::std::uint_least32_t open_flags;
		::std::uint_least32_t statx_flags;
		::std::uint_least32_t fadvise_advice;
		::std::uint_least32_t splice_flags;
		::std::uint_least32_t rename_flags;
		::std::uint_least32_t unlink_flags;
		::std::uint_least32_t hardlink_flags;
		::std::uint_least32_t xattr_flags;
		::std::uint_least32_t msg_ring_flags;
		::std::uint_least32_t uring_cmd_flags;
		::std::uint_least32_t waitid_flags;
		::std::uint_least32_t futex_flags;
		::std::uint_least32_t install_fd_flags;
		::std::uint_least32_t nop_flags;
		::std::uint_least32_t pipe_flags;
	};
	::std::uint_least64_t user_data; /* data to be passed back at completion time */
	union
	{
		::std::uint_least16_t buf_index; /* index into fixed buffers, if used */
		::std::uint_least16_t buf_group; /* for grouped buffer selection */
	};
	::std::uint_least16_t personality; /* personality to use, if used */
	union
	{
		::std::int32_t splice_fd_in;
		::std::uint_least32_t file_index;
		::std::uint_least32_t optlen;
		sqe_addrlen addrlen;
	};
	union
	{
		sqe_addr3_and_pad addr3_and_pad;
		sqe_attr attr;
		::std::uint_least64_t optval;
		/*
		 * If the ring is initialized with io_uring_setup_sqe128, then
		 * this field is used for arbitrary command data
		 */
		::std::uint_least8_t cmd[16];
	};
};


struct io_uring_cqe
{
	::std::uint_least64_t user_data; /* sqe->user_data value passed back */
	::std::int32_t res;              /* result code for this event */
	::std::uint_least32_t flags;     /* io_uring_cqe_flag */
};


/* Filled with the offsets for mmap(2) by io_uring_setup(2) */
struct io_sqring_offsets
{
	::std::uint_least32_t head;
	::std::uint_least32_t tail;
	::std::uint_least32_t ring_mask;
	::std::uint_least32_t ring_entries;
	::std::uint_least32_t flags;
	::std::uint_least32_t dropped;
	::std::uint_least32_t array;
	::std::uint_least32_t resv1;
	::std::uint_least64_t user_addr;
};

struct io_cqring_offsets
{
	::std::uint_least32_t head;
	::std::uint_least32_t tail;
	::std::uint_least32_t ring_mask;
	::std::uint_least32_t ring_entries;
	::std::uint_least32_t overflow;
	::std::uint_least32_t cqes;
	::std::uint_least32_t flags;
	::std::uint_least32_t resv1;
	::std::uint_least64_t user_addr;
};

/* Passed in for io_uring_setup(2). Copied back with updated info on success */
struct io_uring_params
{
	::std::uint_least32_t sq_entries;
	::std::uint_least32_t cq_entries;
	::std::uint_least32_t flags;
	::std::uint_least32_t sq_thread_cpu;
	::std::uint_least32_t sq_thread_idle;
	::std::uint_least32_t features;
	::std::uint_least32_t wq_fd;
	::std::uint_least32_t resv[3];
	::fast_io::liburing::io_sqring_offsets sq_off;
	::fast_io::liburing::io_cqring_offsets cq_off;
};


/* Argument for io_uring_enter(2) with io_uring_enter_ext_arg */
struct io_uring_getevents_arg
{
	::std::uint_least64_t sigmask;
	::std::uint_least32_t sigmask_sz;
	::std::uint_least32_t min_wait_usec;
	::std::uint_least64_t ts;
};

/* __kernel_timespec equivalent */
struct io_uring_timespec
{
	::std::int_least64_t tv_sec;
	::std::int_least64_t tv_nsec;
};

/* io_uring_setup(2) flags */
enum io_uring_setup_flag : ::std::uint_least32_t
{
	io_uring_setup_iopoll = 1U << 0,              /* io_context is polled */
	io_uring_setup_sqpoll = 1U << 1,              /* SQ poll thread */
	io_uring_setup_sq_aff = 1U << 2,              /* sq_thread_cpu is valid */
	io_uring_setup_cqsize = 1U << 3,              /* app defines CQ size */
	io_uring_setup_clamp = 1U << 4,               /* clamp SQ/CQ ring sizes */
	io_uring_setup_attach_wq = 1U << 5,           /* attach to existing wq */
	io_uring_setup_r_disabled = 1U << 6,          /* start with ring disabled */
	io_uring_setup_submit_all = 1U << 7,          /* continue submit on error */
	io_uring_setup_coop_taskrun = 1U << 8,        /* cooperative task running */
	io_uring_setup_taskrun_flag = 1U << 9,        /* IORING_SQ_TASKRUN notification */
	io_uring_setup_sqe128 = 1U << 10,             /* SQEs are 128 byte */
	io_uring_setup_cqe32 = 1U << 11,              /* CQEs are 32 byte */
	io_uring_setup_single_issuer = 1U << 12,      /* only one task submits */
	io_uring_setup_defer_taskrun = 1U << 13,      /* defer task work to enter */
	io_uring_setup_no_mmap = 1U << 14,            /* no mmap for rings (unsupported) */
	io_uring_setup_registered_fd_only = 1U << 15, /* only registered ring fd */
	io_uring_setup_no_sqarray = 1U << 16,         /* no SQ array indirection */
	io_uring_setup_hybrid_iopoll = 1U << 17,      /* kernel decides to iopoll */
	io_uring_setup_cqe_mixed = 1U << 18,          /* mixed 16/32 byte CQEs */
	io_uring_setup_sqe_mixed = 1U << 19,          /* mixed 64/128 byte SQEs */
	io_uring_setup_sq_rewind = 1U << 20           /* rewind SQ on submit */
};

/* io_uring_enter(2) flags */
enum io_uring_enter_flag : ::std::uint_least32_t
{
	io_uring_enter_getevents = 1U << 0,
	io_uring_enter_sq_wakeup = 1U << 1,
	io_uring_enter_sq_wait = 1U << 2,
	io_uring_enter_ext_arg = 1U << 3,
	io_uring_enter_registered_ring = 1U << 4,
	io_uring_enter_abs_timer = 1U << 5,
	io_uring_enter_ext_arg_reg = 1U << 6,
	io_uring_enter_no_iowait = 1U << 7
};

/* io_uring_params.features flags */
enum io_uring_feat : ::std::uint_least32_t
{
	io_uring_feat_single_mmap = 1U << 0,
	io_uring_feat_nodrop = 1U << 1,
	io_uring_feat_submit_stable = 1U << 2,
	io_uring_feat_rw_cur_pos = 1U << 3,
	io_uring_feat_cur_personality = 1U << 4,
	io_uring_feat_fast_poll = 1U << 5,
	io_uring_feat_poll_32bits = 1U << 6,
	io_uring_feat_sqpoll_nonfixed = 1U << 7,
	io_uring_feat_ext_arg = 1U << 8,
	io_uring_feat_native_workers = 1U << 9,
	io_uring_feat_rsrc_tags = 1U << 10,
	io_uring_feat_cqe_skip = 1U << 11,
	io_uring_feat_linked_file = 1U << 12,
	io_uring_feat_reg_reg_ring = 1U << 13,
	io_uring_feat_recvsend_bundle = 1U << 14,
	io_uring_feat_min_timeout = 1U << 15,
	io_uring_feat_rw_attr = 1U << 16,
	io_uring_feat_no_iowait = 1U << 17
};

/* sq_ring->flags (kernel writes, application reads) */
enum io_uring_sq_flag : ::std::uint_least32_t
{
	io_uring_sq_need_wakeup = 1U << 0, /* needs io_uring_enter wakeup */
	io_uring_sq_cq_overflow = 1U << 1, /* CQ ring is overflown */
	io_uring_sq_taskrun = 1U << 2      /* task should enter the kernel */
};

/* cq_ring->flags */
enum io_uring_cq_flag : ::std::uint_least32_t
{
	io_uring_cq_eventfd_disabled = 1U << 0
};

/* cqe->flags */
enum io_uring_cqe_flag : ::std::uint_least32_t
{
	io_uring_cqe_f_buffer = 1U << 0, /* upper 16 bits are the buffer ID */
	io_uring_cqe_f_more = 1U << 1,   /* parent SQE will generate more CQEs */
	io_uring_cqe_f_sock_nonempty = 1U << 2,
	io_uring_cqe_f_notif = 1U << 3,
	io_uring_cqe_f_buf_more = 1U << 4,
	io_uring_cqe_f_skip = 1U << 5, /* padding CQE in a mixed ring, ignore */
	io_uring_cqe_f_32 = 1U << 15   /* 32-byte CQE in a mixed ring */
};

inline constexpr ::std::uint_least32_t io_uring_cqe_buffer_shift{16};

/* Magic offsets for the application to mmap the data it needs */
enum io_uring_mmap_offset : ::std::uint_least64_t
{
	io_uring_off_sq_ring = 0ULL,
	io_uring_off_cq_ring = 0x8000000ULL,
	io_uring_off_sqes = 0x10000000ULL,
	io_uring_off_pbuf_ring = 0x80000000ULL
};

/* sqe->opcode */
enum io_uring_op : ::std::uint_least8_t
{
	io_uring_op_nop,
	io_uring_op_readv,
	io_uring_op_writev,
	io_uring_op_fsync,
	io_uring_op_read_fixed,
	io_uring_op_write_fixed,
	io_uring_op_poll_add,
	io_uring_op_poll_remove,
	io_uring_op_sync_file_range,
	io_uring_op_sendmsg,
	io_uring_op_recvmsg,
	io_uring_op_timeout,
	io_uring_op_timeout_remove,
	io_uring_op_accept,
	io_uring_op_async_cancel,
	io_uring_op_link_timeout,
	io_uring_op_connect,
	io_uring_op_fallocate,
	io_uring_op_openat,
	io_uring_op_close,
	io_uring_op_files_update,
	io_uring_op_statx,
	io_uring_op_read,
	io_uring_op_write,
	io_uring_op_fadvise,
	io_uring_op_madvise,
	io_uring_op_send,
	io_uring_op_recv,
	io_uring_op_openat2,
	io_uring_op_epoll_ctl,
	io_uring_op_splice,
	io_uring_op_provide_buffers,
	io_uring_op_remove_buffers,
	io_uring_op_tee,
	io_uring_op_shutdown,
	io_uring_op_renameat,
	io_uring_op_unlinkat,
	io_uring_op_mkdirat,
	io_uring_op_symlinkat,
	io_uring_op_linkat,
	io_uring_op_msg_ring,
	io_uring_op_fsetxattr,
	io_uring_op_setxattr,
	io_uring_op_fgetxattr,
	io_uring_op_getxattr,
	io_uring_op_socket,
	io_uring_op_uring_cmd,
	io_uring_op_send_zc,
	io_uring_op_sendmsg_zc,
	io_uring_op_read_multishot,
	io_uring_op_waitid,
	io_uring_op_futex_wait,
	io_uring_op_futex_wake,
	io_uring_op_futex_waitv,
	io_uring_op_fixed_fd_install,
	io_uring_op_ftruncate,
	io_uring_op_bind,
	io_uring_op_listen,
	io_uring_op_recv_zc,
	io_uring_op_epoll_wait,
	io_uring_op_readv_fixed,
	io_uring_op_writev_fixed,
	io_uring_op_pipe,
	io_uring_op_nop128,
	io_uring_op_uring_cmd128,
	io_uring_op_last
};

/* sqe->flags */
enum io_uring_sqe_flag : ::std::uint_least8_t
{
	io_uring_sqe_fixed_file = 1U << 0,      /* use fixed fileset */
	io_uring_sqe_io_drain = 1U << 1,        /* issue after inflight IO */
	io_uring_sqe_io_link = 1U << 2,         /* links next sqe */
	io_uring_sqe_io_hardlink = 1U << 3,     /* like LINK, but stronger */
	io_uring_sqe_async = 1U << 4,           /* always go async */
	io_uring_sqe_buffer_select = 1U << 5,   /* select buffer from sqe->buf_group */
	io_uring_sqe_cqe_skip_success = 1U << 6 /* don't post CQE if success */
};

/* sqe->fsync_flags */
enum io_uring_fsync_flag : ::std::uint_least32_t
{
	io_uring_fsync_datasync = 1U << 0
};

/* sqe->timeout_flags */
enum io_uring_timeout_flag : ::std::uint_least32_t
{
	io_uring_timeout_abs = 1U << 0,
	io_uring_timeout_update = 1U << 1,
	io_uring_timeout_boottime = 1U << 2,
	io_uring_timeout_realtime = 1U << 3,
	io_uring_link_timeout_update = 1U << 4,
	io_uring_timeout_etime_success = 1U << 5,
	io_uring_timeout_multishot = 1U << 6,
	io_uring_timeout_clock_mask = io_uring_timeout_boottime | io_uring_timeout_realtime,
	io_uring_timeout_update_mask = io_uring_timeout_update | io_uring_link_timeout_update
};

/* sqe->cancel_flags */
enum io_uring_cancel_flag : ::std::uint_least32_t
{
	io_uring_async_cancel_all = 1U << 0,
	io_uring_async_cancel_fd = 1U << 1,
	io_uring_async_cancel_any = 1U << 2,
	io_uring_async_cancel_fd_fixed = 1U << 3,
	io_uring_async_cancel_userdata = 1U << 4,
	io_uring_async_cancel_op = 1U << 5
};

/* sqe->len for io_uring_op_poll_add */
enum io_uring_poll_flag : ::std::uint_least32_t
{
	io_uring_poll_add_multi = 1U << 0 /* multishot poll */
};

/* sqe->ioprio for io_uring_op_accept */
enum io_uring_accept_ioprio : ::std::uint_least16_t
{
	io_uring_accept_multishot = 1U << 0
};

/* sqe->msg_flags for io_uring_op_send/recv */
enum io_uring_recvsend_flag : ::std::uint_least32_t
{
	io_uring_recvsend_poll_first = 1U << 0,
	io_uring_recvsend_multishot = 1U << 1,
	io_uring_recvsend_fixed_buf = 1U << 2,
	io_uring_recvsend_bundle = 1U << 4
};

/* io_uring_register(2) opcodes */
enum io_uring_register_op : ::std::uint_least32_t
{
	io_uring_regop_buffers = 0,
	io_uring_regop_unregister_buffers = 1,
	io_uring_regop_files = 2,
	io_uring_regop_unregister_files = 3,
	io_uring_regop_eventfd = 4,
	io_uring_regop_unregister_eventfd = 5,
	io_uring_regop_files_update = 6,
	io_uring_regop_eventfd_async = 7,
	io_uring_regop_probe = 8,
	io_uring_regop_personality = 9,
	io_uring_regop_unregister_personality = 10,
	io_uring_regop_restrictions = 11,
	io_uring_regop_enable_rings = 12,
	io_uring_regop_files2 = 13,
	io_uring_regop_files_update2 = 14,
	io_uring_regop_buffers2 = 15,
	io_uring_regop_buffers_update = 16,
	io_uring_regop_iowq_aff = 17,
	io_uring_regop_unregister_iowq_aff = 18,
	io_uring_regop_iowq_max_workers = 19,
	io_uring_regop_ring_fds = 20,
	io_uring_regop_unregister_ring_fds = 21,
	io_uring_regop_pbuf_ring = 22,
	io_uring_regop_unregister_pbuf_ring = 23,
	io_uring_regop_sync_cancel = 24,
	io_uring_regop_file_alloc_range = 25,
	io_uring_regop_pbuf_status = 26,
	io_uring_regop_napi = 27,
	io_uring_regop_unregister_napi = 28,
	io_uring_regop_clock = 29,
	io_uring_regop_clone_buffers = 30,
	io_uring_regop_send_msg_ring = 31
};

/* Tell io_uring to allocate a direct (fixed) descriptor for ops that can
 * instantiate one (openat/accept/etc.) */
inline constexpr ::std::uint_least32_t io_uring_file_index_alloc{~0U};

/* user_data value used internally for wait timeouts on kernels without
 * io_uring_feat_ext_arg; applications must not use it */
inline constexpr ::std::uint_least64_t io_uring_internal_timeout_user_data{~0ULL};

namespace details
{

inline int io_uring_setup_impl(::std::uint_least32_t entries, io_uring_params *params) noexcept
{
	return ::fast_io::system_call<io_uring_setup_syscall_number, int>(entries, params);
}

inline int io_uring_enter2_impl(::std::uint_least32_t fd, ::std::uint_least32_t to_submit, ::std::uint_least32_t min_complete,
								::std::uint_least32_t flags, void const *arg, ::std::size_t sz) noexcept
{
	return ::fast_io::system_call<io_uring_enter_syscall_number, int>(fd, to_submit, min_complete, flags, arg, sz);
}

inline int io_uring_register_impl(::std::uint_least32_t fd, ::std::uint_least32_t opcode, void const *arg,
								  ::std::uint_least32_t nr_args) noexcept
{
	return ::fast_io::system_call<io_uring_register_syscall_number, int>(fd, opcode, arg, nr_args);
}

/* the ring fields point into kernel-owned mmap'd memory: accesses must
 * go through may_alias pointers, like the rest of fast_io */
using io_uring_u32_may_alias_ptr [[__gnu__::__may_alias__]] = ::std::uint_least32_t *;
using io_uring_sqe_may_alias_ptr [[__gnu__::__may_alias__]] = ::fast_io::liburing::io_uring_sqe *;
using io_uring_cqe_may_alias_ptr [[__gnu__::__may_alias__]] = ::fast_io::liburing::io_uring_cqe *;

/* userspace-side submission queue state (mmap'd, shared with the kernel) */
struct io_uring_sq_state
{
	io_uring_u32_may_alias_ptr khead{};
	io_uring_u32_may_alias_ptr ktail{};
	io_uring_u32_may_alias_ptr kflags{};
	io_uring_u32_may_alias_ptr kdropped{};
	io_uring_u32_may_alias_ptr array{};
	io_uring_sqe_may_alias_ptr sqes{};
	::std::uint_least32_t sqe_head{};
	::std::uint_least32_t sqe_tail{};
	::std::size_t ring_sz{};
	void *ring_ptr{};
	::std::uint_least32_t ring_mask{};
	::std::uint_least32_t ring_entries{};
	::std::size_t sqes_sz{};
};

/* userspace-side completion queue state (mmap'd, shared with the kernel) */
struct io_uring_cq_state
{
	io_uring_u32_may_alias_ptr khead{};
	io_uring_u32_may_alias_ptr ktail{};
	io_uring_u32_may_alias_ptr koverflow{};
	io_uring_cqe_may_alias_ptr cqes{};
	::std::size_t ring_sz{};
	void *ring_ptr{};
	::std::uint_least32_t ring_mask{};
	::std::uint_least32_t ring_entries{};
};

enum io_uring_int_flag : ::std::uint_least32_t
{
	io_uring_int_cq_enter = 1U << 0 /* IOPOLL (without SQPOLL) always needs enter */
};

} // namespace details

struct io_uring_ring_state
{
	::fast_io::liburing::details::io_uring_sq_state sq{};
	::fast_io::liburing::details::io_uring_cq_state cq{};
	int ring_fd{-1};
	::std::uint_least32_t flags{};
	::std::uint_least32_t features{};
	::std::uint_least32_t int_flags{};
};

namespace details
{

/*
 * Memory ordering required by the ring protocol:
 * - SQ tail must be release-stored after the SQEs are written (SQPOLL).
 * - SQ head must be acquire-loaded before overwriting SQEs (SQPOLL).
 * - CQ tail must be acquire-loaded before reading CQEs.
 * - CQ head must be release-stored after the CQEs have been consumed.
 */
inline ::std::uint_least32_t io_uring_load_acquire(::std::uint_least32_t const *p) noexcept
{
	return __atomic_load_n(p, __ATOMIC_ACQUIRE);
}

inline void io_uring_store_release(::std::uint_least32_t *p, ::std::uint_least32_t v) noexcept
{
	__atomic_store_n(p, v, __ATOMIC_RELEASE);
}

inline void io_uring_smp_mb() noexcept
{
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
}

inline ::std::uint_least32_t io_uring_sqe_shift(::fast_io::liburing::io_uring_ring_state const &ring) noexcept
{
	return !!(ring.flags & io_uring_setup_sqe128);
}

inline ::std::uint_least32_t io_uring_cqe_shift(::fast_io::liburing::io_uring_ring_state const &ring) noexcept
{
	return !!(ring.flags & io_uring_setup_cqe32);
}

inline void io_uring_unmap_rings(::fast_io::liburing::io_uring_ring_state &ring) noexcept
{
	if (ring.sq.ring_ptr != nullptr && ring.sq.ring_sz != 0)
	{
		::fast_io::details::sys_munmap_nothrow(ring.sq.ring_ptr, ring.sq.ring_sz);
	}
	if (ring.cq.ring_ptr != nullptr && ring.cq.ring_ptr != ring.sq.ring_ptr && ring.cq.ring_sz != 0)
	{
		::fast_io::details::sys_munmap_nothrow(ring.cq.ring_ptr, ring.cq.ring_sz);
	}
	ring.sq.ring_ptr = nullptr;
	ring.cq.ring_ptr = nullptr;
}

/*
 * Raw syscall mmap for the rings (PROT_READ|PROT_WRITE,
 * MAP_SHARED|MAP_POPULATE). Returns the raw syscall result so the caller
 * can distinguish the errno; check with linux_system_call_fails.
 */
inline ::std::ptrdiff_t io_uring_mmap_impl(::std::size_t len, int fd, ::std::uint_least64_t offset) noexcept
{
	constexpr int prot_read_write{3};          /* PROT_READ | PROT_WRITE */
	constexpr int map_shared_populate{0x8001}; /* MAP_SHARED | MAP_POPULATE */
#if defined(__s390__) || defined(__s390x__)
	// s390's __NR_mmap is the old single-argument entry; pass the kernel's
	// mmap argument block.
	struct s390_mmap_arg_struct
	{
		::std::size_t addr;
		::std::size_t len;
		::std::size_t prot;
		::std::size_t flags;
		::std::size_t fd;
		::std::size_t offset;
	};
	s390_mmap_arg_struct args{0, static_cast<::std::size_t>(len),
							  static_cast<::std::size_t>(prot_read_write),
							  static_cast<::std::size_t>(map_shared_populate),
							  static_cast<::std::size_t>(fd), static_cast<::std::size_t>(offset)};
	return ::fast_io::system_call<__NR_mmap, ::std::ptrdiff_t>(__builtin_addressof(args));
#elif defined(__NR_mmap)
	return ::fast_io::system_call<__NR_mmap, ::std::ptrdiff_t>(nullptr, len, prot_read_write, map_shared_populate, fd,
															   offset);
#elif defined(__NR_mmap2)
	/* mmap2 takes the offset in 4KiB units; the io_uring magic offsets are
	 * all page aligned */
	return ::fast_io::system_call<__NR_mmap2, ::std::ptrdiff_t>(nullptr, len, prot_read_write, map_shared_populate, fd,
																offset >> 12);
#else
	void *p{::fast_io::noexcept_call(::mmap, nullptr, len, prot_read_write, map_shared_populate, fd,
									 static_cast<::std::int_least64_t>(offset))};
	if (p == reinterpret_cast<void *>(-1))
	{
		return -errno;
	}
	return static_cast<::std::ptrdiff_t>(reinterpret_cast<::std::uintptr_t>(p));
#endif
}

} // namespace details

/* RAII guard for one io_uring mmap region: the constructor maps and a
 * failure is a herbception; release() hands the mapping to the ring. */
class io_uring_mmap_guard
{
public:
	void *ptr{};
	::std::size_t size{};
	inline io_uring_mmap_guard(::std::size_t len, int fd, ::std::uint_least64_t offset) throws
	{
		::std::ptrdiff_t p{::fast_io::liburing::details::io_uring_mmap_impl(len, fd, offset)};
		if (::fast_io::linux_system_call_fails(p)) [[unlikely]]
		{
			::fast_io::throw_posix_error(static_cast<int>(-p));
		}
		ptr = reinterpret_cast<void *>(p);
		size = len;
	}
	io_uring_mmap_guard(io_uring_mmap_guard const &) = delete;
	io_uring_mmap_guard &operator=(io_uring_mmap_guard const &) = delete;
	inline ~io_uring_mmap_guard()
	{
		if (ptr != nullptr)
		{
			::fast_io::details::sys_munmap_nothrow(ptr, size);
		}
	}
	inline void *release() noexcept
	{
		auto p{ptr};
		ptr = nullptr;
		return p;
	}
};

namespace details
{

/*
 * io_uring_setup(2) + mmap of the SQ ring, the CQ ring (shared when
 * io_uring_feat_single_mmap) and the SQE array. Ring pointers are then
 * derived from the offsets the kernel reported in io_uring_params.
 * io_uring_setup_no_sqarray is always tried first (like liburing >= 2.2);
 * on kernels that reject it we retry with the SQ array.
 */
inline void io_uring_queue_init_impl(::fast_io::liburing::io_uring_ring_state &ring, ::std::uint_least32_t entries, ::std::uint_least32_t flags)
	throws
{
	io_uring_params params{};
	params.flags = flags;
	if (!(flags & io_uring_setup_no_sqarray))
	{
		params.flags = flags | io_uring_setup_no_sqarray;
	}
	int fd{io_uring_setup_impl(entries, __builtin_addressof(params))};
	if (fd == -EINVAL && !(flags & io_uring_setup_no_sqarray))
	{
		params.flags = flags;
		fd = io_uring_setup_impl(entries, __builtin_addressof(params));
	}
	::fast_io::system_call_throw_error(fd);
	::fast_io::posix_file pf{fd};

	ring.sq.ring_sz = params.sq_off.array + params.sq_entries * sizeof(::std::uint_least32_t);
	::std::size_t cqes_bytes{static_cast<::std::size_t>(params.cq_entries)
							 << (((params.flags & io_uring_setup_cqe32) != 0) ? 5 : 4)};
	ring.cq.ring_sz = params.cq_off.cqes + cqes_bytes;

	if (params.features & io_uring_feat_single_mmap)
	{
		if (ring.cq.ring_sz > ring.sq.ring_sz)
		{
			ring.sq.ring_sz = ring.cq.ring_sz;
		}
		ring.cq.ring_sz = ring.sq.ring_sz;
	}

	io_uring_mmap_guard sq_guard{ring.sq.ring_sz, fd, io_uring_off_sq_ring};
	ring.sq.ring_ptr = sq_guard.release();
	if (params.features & io_uring_feat_single_mmap)
	{
		ring.cq.ring_ptr = ring.sq.ring_ptr;
	}
	else
	{
		io_uring_mmap_guard cq_guard{ring.cq.ring_sz, fd, io_uring_off_cq_ring};
		ring.cq.ring_ptr = cq_guard.release();
	}

	::std::size_t sqes_bytes{static_cast<::std::size_t>(params.sq_entries)
							 << (((params.flags & io_uring_setup_sqe128) != 0) ? 7 : 6)};
	io_uring_mmap_guard sqes_guard{sqes_bytes, fd, io_uring_off_sqes};
	ring.sq.sqes_sz = sqes_bytes;
	ring.sq.sqes = static_cast<io_uring_sqe_may_alias_ptr>(sqes_guard.release());

	::std::byte *sq_ring{reinterpret_cast<::std::byte *>(ring.sq.ring_ptr)};
	::std::byte *cq_ring{reinterpret_cast<::std::byte *>(ring.cq.ring_ptr)};

	ring.sq.khead = reinterpret_cast<io_uring_u32_may_alias_ptr>(sq_ring + params.sq_off.head);
	ring.sq.ktail = reinterpret_cast<io_uring_u32_may_alias_ptr>(sq_ring + params.sq_off.tail);
	ring.sq.kflags = reinterpret_cast<io_uring_u32_may_alias_ptr>(sq_ring + params.sq_off.flags);
	ring.sq.kdropped = reinterpret_cast<io_uring_u32_may_alias_ptr>(sq_ring + params.sq_off.dropped);
	if (!(params.flags & io_uring_setup_no_sqarray))
	{
		ring.sq.array = reinterpret_cast<io_uring_u32_may_alias_ptr>(sq_ring + params.sq_off.array);
	}

	ring.cq.khead = reinterpret_cast<io_uring_u32_may_alias_ptr>(cq_ring + params.cq_off.head);
	ring.cq.ktail = reinterpret_cast<io_uring_u32_may_alias_ptr>(cq_ring + params.cq_off.tail);
	ring.cq.koverflow = reinterpret_cast<io_uring_u32_may_alias_ptr>(cq_ring + params.cq_off.overflow);
	ring.cq.cqes = reinterpret_cast<io_uring_cqe_may_alias_ptr>(cq_ring + params.cq_off.cqes);

	ring.sq.ring_mask = *reinterpret_cast<io_uring_u32_may_alias_ptr>(sq_ring + params.sq_off.ring_mask);
	ring.sq.ring_entries = *reinterpret_cast<io_uring_u32_may_alias_ptr>(sq_ring + params.sq_off.ring_entries);
	ring.cq.ring_mask = *reinterpret_cast<io_uring_u32_may_alias_ptr>(cq_ring + params.cq_off.ring_mask);
	ring.cq.ring_entries = *reinterpret_cast<io_uring_u32_may_alias_ptr>(cq_ring + params.cq_off.ring_entries);

	if (ring.sq.array != nullptr)
	{
		/* map SQ slots directly to SQEs once, like liburing >= 2.0 does */
		for (::std::uint_least32_t index{}; index < ring.sq.ring_entries; ++index)
		{
			ring.sq.array[index] = index;
		}
	}

	ring.features = params.features;
	ring.flags = params.flags;
	ring.ring_fd = pf.release();

	/* IOPOLL without SQPOLL always needs io_uring_enter(2) to reap CQEs */
	if ((ring.flags & (io_uring_setup_iopoll | io_uring_setup_sqpoll)) == io_uring_setup_iopoll)
	{
		ring.int_flags |= io_uring_int_cq_enter;
	}
}

inline void io_uring_queue_exit_impl(::fast_io::liburing::io_uring_ring_state &ring) noexcept
{
	if (ring.sq.sqes != nullptr)
	{
		::fast_io::details::sys_munmap_nothrow(ring.sq.sqes, ring.sq.sqes_sz);
		ring.sq.sqes = nullptr;
	}
	io_uring_unmap_rings(ring);
	if (ring.ring_fd >= 0)
	{
		::fast_io::details::sys_close(ring.ring_fd);
		ring.ring_fd = -1;
	}
}

} // namespace details

namespace details
{

inline ::std::uint_least32_t io_uring_load_sq_head(::fast_io::liburing::io_uring_ring_state const &ring) noexcept
{
	/* without SQPOLL the kernel only advances khead as a hint */
	if (ring.flags & io_uring_setup_sqpoll)
	{
		return io_uring_load_acquire(ring.sq.khead);
	}
	return *ring.sq.khead;
}

inline void io_uring_initialize_sqe(::fast_io::liburing::io_uring_sqe *sqe) noexcept
{
	sqe->flags = 0;
	sqe->ioprio = 0;
	sqe->rw_flags = 0;
	sqe->buf_index = 0;
	sqe->personality = 0;
	sqe->file_index = 0;
	sqe->addr3_and_pad.addr3 = 0;
	sqe->addr3_and_pad.pad2 = 0;
}

inline bool io_uring_sq_ring_needs_enter(::fast_io::liburing::io_uring_ring_state const &ring, ::std::uint_least32_t submit,
										 ::std::uint_least32_t &flags) noexcept
{
	if (submit == 0)
	{
		return false;
	}
	if (!(ring.flags & io_uring_setup_sqpoll))
	{
		return true;
	}
	/* ensure the kernel can see the SQ tail store before we read the flags */
	io_uring_smp_mb();
	if (io_uring_load_acquire(ring.sq.kflags) & io_uring_sq_need_wakeup)
	{
		flags |= io_uring_enter_sq_wakeup;
		return true;
	}
	return false;
}

inline bool io_uring_cq_ring_needs_flush(::fast_io::liburing::io_uring_ring_state const &ring) noexcept
{
	return (io_uring_load_acquire(ring.sq.kflags) & (io_uring_sq_cq_overflow | io_uring_sq_taskrun)) != 0;
}

inline bool io_uring_cq_ring_needs_enter(::fast_io::liburing::io_uring_ring_state const &ring) noexcept
{
	return (ring.int_flags & io_uring_int_cq_enter) || io_uring_cq_ring_needs_flush(ring);
}

inline int io_uring_enter_impl(::fast_io::liburing::io_uring_ring_state const &ring, ::std::uint_least32_t to_submit,
							   ::std::uint_least32_t min_complete, ::std::uint_least32_t flags) noexcept
{
	return io_uring_enter2_impl(static_cast<::std::uint_least32_t>(ring.ring_fd), to_submit, min_complete, flags,
								nullptr, 0);
}

/*
 * Sync internal state with kernel ring state on the SQ side. Returns the
 * number of pending (unsubmitted) SQEs.
 */
inline ::std::uint_least32_t io_uring_flush_sq(::fast_io::liburing::io_uring_ring_state &ring) noexcept
{
	auto &sq{ring.sq};
	::std::uint_least32_t tail{sq.sqe_tail};
	if (ring.flags & io_uring_setup_sq_rewind)
	{
		sq.sqe_tail = 0;
		return tail;
	}
	if (sq.sqe_head != tail)
	{
		sq.sqe_head = tail;
		if (!(ring.flags & io_uring_setup_sqpoll))
		{
			*sq.ktail = tail;
		}
		else
		{
			io_uring_store_release(sq.ktail, tail);
		}
	}
	/* khead is written concurrently by the kernel for SQPOLL */
	return tail - io_uring_load_acquire(sq.khead);
}

/*
 * Advance the CQ head by the number of slots a CQE occupies (2 for
 * 32-byte CQEs in mixed rings, 1 otherwise).
 */
inline void io_uring_cq_advance_cqe(::fast_io::liburing::io_uring_ring_state &ring, ::fast_io::liburing::io_uring_cqe const *cqe) noexcept
{
	io_uring_store_release(ring.cq.khead,
						   *ring.cq.khead + ((cqe->flags & io_uring_cqe_f_32) ? 2U : 1U));
}

/*
 * Whether this CQE is internal and must be consumed without being
 * reported: F_SKIP padding entries in mixed rings, and the internal
 * timeout SQE used by wait timeouts on kernels without
 * io_uring_feat_ext_arg. Errors from the internal timeout are propagated
 * through err.
 */
inline bool io_uring_skip_cqe(::fast_io::liburing::io_uring_ring_state &ring, ::fast_io::liburing::io_uring_cqe *cqe) throws
{
	if (!(cqe->flags & io_uring_cqe_f_skip))
	{
		if (ring.features & io_uring_feat_ext_arg)
		{
			return false;
		}
		if (cqe->user_data != io_uring_internal_timeout_user_data)
		{
			return false;
		}
		if (cqe->res < 0)
		{
			io_uring_cq_advance_cqe(ring, cqe);
			::fast_io::throw_posix_error(static_cast<int>(-cqe->res));
		}
	}
	io_uring_cq_advance_cqe(ring, cqe);
	return true;
}

struct io_uring_peek_cqe_result
{
	::fast_io::liburing::io_uring_cqe *cqe_ptr;
	::std::uint_least32_t available;
};

/* returns the first visible CQE (or nullptr) plus the number of CQEs
 * currently visible, for wait_nr comparisons */
inline ::fast_io::liburing::details::io_uring_peek_cqe_result
io_uring_peek_cqe_impl(::fast_io::liburing::io_uring_ring_state &ring) throws
{
	io_uring_cqe *cqe{};
	::std::uint_least32_t available{};
	::std::uint_least32_t mask{ring.cq.ring_mask};
	::std::uint_least32_t shift{io_uring_cqe_shift(ring)};
	do
	{
		/* acquire ordering pairs with the kernel publishing CQEs */
		::std::uint_least32_t tail{io_uring_load_acquire(ring.cq.ktail)};
		::std::uint_least32_t head{*ring.cq.khead};
		cqe = nullptr;
		available = tail - head;
		if (available == 0)
		{
			break;
		}
		cqe = ring.cq.cqes + ((head & mask) << shift);
		if (!io_uring_skip_cqe(ring, cqe))
		{
			break;
		}
		cqe = nullptr;
	} while (true);
	return {cqe, available};
}

struct io_uring_get_data
{
	::std::uint_least32_t submit;
	::std::uint_least32_t wait_nr;
	::std::uint_least32_t get_flags;
	::std::size_t sz;
	bool has_ts;
	void const *arg;
};

/*
 * Returns the first available CQE, or nullptr when the caller's deadline
 * expired. Kernel errors — including EAGAIN when there is nothing to
 * reap without another kernel entry — are herbceptions.
 */
inline io_uring_cqe *io_uring_get_cqe_impl(::fast_io::liburing::io_uring_ring_state &ring, io_uring_get_data &data) throws
{
	io_uring_cqe *cqe{};
	bool looped{};
	do
	{
		bool need_enter{};
		::std::uint_least32_t flags{};
		auto peek{io_uring_peek_cqe_impl(ring)};
		cqe = peek.cqe_ptr;
		::std::uint_least32_t nr_available{peek.available};
		if (cqe == nullptr && data.wait_nr == 0 && data.submit == 0)
		{
			/*
			 * If we already looped once, we already entered the kernel.
			 * Since there's nothing to submit or wait for, don't keep
			 * retrying.
			 */
			if (looped || !io_uring_cq_ring_needs_enter(ring))
			{
				::fast_io::throw_posix_error(EAGAIN);
			}
			need_enter = true;
		}
		if (data.wait_nr > nr_available || need_enter)
		{
			flags |= io_uring_enter_getevents | data.get_flags;
			need_enter = true;
		}
		if (io_uring_sq_ring_needs_enter(ring, data.submit, flags))
		{
			need_enter = true;
		}
		if (!need_enter)
		{
			break;
		}
		if (looped && data.has_ts)
		{
			break;
		}
		int ret{io_uring_enter2_impl(static_cast<::std::uint_least32_t>(ring.ring_fd), data.submit,
									 data.wait_nr, flags, data.arg, data.sz)};
		if (ret < 0)
		{
			if (ret == -ETIME && data.has_ts)
			{
				return nullptr;
			}
			::fast_io::system_call_throw_error(ret);
		}
		data.submit -= static_cast<::std::uint_least32_t>(ret);
		if (cqe != nullptr)
		{
			break;
		}
		looped = true;
	} while (true);
	return cqe;
}

} // namespace details

/* Number of CQ slots a CQE occupies (2 for 32-byte CQEs in mixed rings) */
inline ::std::uint_least32_t io_uring_cqe_nr(io_uring_cqe const *cqe) noexcept
{
	return (cqe->flags & io_uring_cqe_f_32) ? 2U : 1U;
}

/* user_data helpers */
inline void io_uring_sqe_set_data64(::fast_io::liburing::io_uring_sqe *sqe, ::std::uint_least64_t data) noexcept
{
	sqe->user_data = data;
}

inline void io_uring_sqe_set_data(::fast_io::liburing::io_uring_sqe *sqe, void const *data) noexcept
{
	sqe->user_data = reinterpret_cast<::std::uint_least64_t>(data);
}

inline ::std::uint_least64_t io_uring_cqe_get_data64(io_uring_cqe const *cqe) noexcept
{
	return cqe->user_data;
}

inline void *io_uring_cqe_get_data(io_uring_cqe const *cqe) noexcept
{
	return reinterpret_cast<void *>(static_cast<::std::uintptr_t>(cqe->user_data));
}

inline void io_uring_sqe_set_flags(::fast_io::liburing::io_uring_sqe *sqe, ::std::uint_least32_t flags) noexcept
{
	sqe->flags = static_cast<::std::uint_least8_t>(flags);
}

/* ======================= sqe preparation ======================= */

/*
 * Shared initializer for the rw-shaped opcodes: fd, one address/length
 * pair and a 64-bit offset. All the prep functions below reduce to it,
 * exactly like liburing's io_uring_prep_rw.
 */
inline void io_uring_prep_rw(io_uring_op op, ::fast_io::liburing::io_uring_sqe *sqe, int fd, void const *addr,
							 ::std::uint_least32_t len, ::std::uint_least64_t offset) noexcept
{
	sqe->opcode = static_cast<::std::uint_least8_t>(op);
	sqe->fd = fd;
	sqe->off = offset;
	sqe->addr = reinterpret_cast<::std::uint_least64_t>(addr);
	sqe->len = len;
}

inline void io_uring_prep_nop(::fast_io::liburing::io_uring_sqe *sqe) noexcept
{
	io_uring_prep_rw(io_uring_op_nop, sqe, -1, nullptr, 0, 0);
}

inline void io_uring_prep_readv(::fast_io::liburing::io_uring_sqe *sqe, int fd, io_scatter_t const *iovecs,
								::std::uint_least32_t nr_vecs, ::std::uint_least64_t offset) noexcept
{
	io_uring_prep_rw(io_uring_op_readv, sqe, fd, iovecs, nr_vecs, offset);
}

inline void io_uring_prep_writev(::fast_io::liburing::io_uring_sqe *sqe, int fd, io_scatter_t const *iovecs,
								 ::std::uint_least32_t nr_vecs, ::std::uint_least64_t offset) noexcept
{
	io_uring_prep_rw(io_uring_op_writev, sqe, fd, iovecs, nr_vecs, offset);
}

inline void io_uring_prep_readv2(::fast_io::liburing::io_uring_sqe *sqe, int fd, io_scatter_t const *iovecs,
								 ::std::uint_least32_t nr_vecs, ::std::uint_least64_t offset, ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_readv(sqe, fd, iovecs, nr_vecs, offset);
	sqe->rw_flags = flags;
}

inline void io_uring_prep_writev2(::fast_io::liburing::io_uring_sqe *sqe, int fd, io_scatter_t const *iovecs,
								  ::std::uint_least32_t nr_vecs, ::std::uint_least64_t offset, ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_writev(sqe, fd, iovecs, nr_vecs, offset);
	sqe->rw_flags = flags;
}

inline void io_uring_prep_read(::fast_io::liburing::io_uring_sqe *sqe, int fd, void *buf, ::std::uint_least32_t nbytes,
							   ::std::uint_least64_t offset) noexcept
{
	io_uring_prep_rw(io_uring_op_read, sqe, fd, buf, nbytes, offset);
}

inline void io_uring_prep_write(::fast_io::liburing::io_uring_sqe *sqe, int fd, void const *buf, ::std::uint_least32_t nbytes,
								::std::uint_least64_t offset) noexcept
{
	io_uring_prep_rw(io_uring_op_write, sqe, fd, buf, nbytes, offset);
}

inline void io_uring_prep_read_fixed(::fast_io::liburing::io_uring_sqe *sqe, int fd, void *buf, ::std::uint_least32_t nbytes,
									 ::std::uint_least64_t offset, ::std::uint_least16_t buf_index) noexcept
{
	io_uring_prep_rw(io_uring_op_read_fixed, sqe, fd, buf, nbytes, offset);
	sqe->buf_index = buf_index;
}

inline void io_uring_prep_write_fixed(::fast_io::liburing::io_uring_sqe *sqe, int fd, void const *buf, ::std::uint_least32_t nbytes,
									  ::std::uint_least64_t offset, ::std::uint_least16_t buf_index) noexcept
{
	io_uring_prep_rw(io_uring_op_write_fixed, sqe, fd, buf, nbytes, offset);
	sqe->buf_index = buf_index;
}

inline void io_uring_prep_fsync(::fast_io::liburing::io_uring_sqe *sqe, int fd, ::std::uint_least32_t fsync_flags) noexcept
{
	io_uring_prep_rw(io_uring_op_fsync, sqe, fd, nullptr, 0, 0);
	sqe->fsync_flags = fsync_flags;
}

inline void io_uring_prep_sync_file_range(::fast_io::liburing::io_uring_sqe *sqe, int fd, ::std::uint_least32_t len,
										  ::std::uint_least64_t offset, ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_sync_file_range, sqe, fd, nullptr, len, offset);
	sqe->sync_range_flags = flags;
}

inline void io_uring_prep_recvmsg(::fast_io::liburing::io_uring_sqe *sqe, int fd, void *msg, ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_recvmsg, sqe, fd, msg, 1, 0);
	sqe->msg_flags = flags;
}

inline void io_uring_prep_sendmsg(::fast_io::liburing::io_uring_sqe *sqe, int fd, void const *msg, ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_sendmsg, sqe, fd, msg, 1, 0);
	sqe->msg_flags = flags;
}

inline void io_uring_prep_recv(::fast_io::liburing::io_uring_sqe *sqe, int fd, void *buf, ::std::uint_least32_t len,
							   ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_recv, sqe, fd, buf, len, 0);
	sqe->msg_flags = flags;
}

inline void io_uring_prep_send(::fast_io::liburing::io_uring_sqe *sqe, int fd, void const *buf, ::std::uint_least32_t len,
							   ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_send, sqe, fd, buf, len, 0);
	sqe->msg_flags = flags;
}

inline void io_uring_prep_poll_add(::fast_io::liburing::io_uring_sqe *sqe, int fd, ::std::uint_least32_t poll_mask) noexcept
{
	io_uring_prep_rw(io_uring_op_poll_add, sqe, fd, nullptr, 0, 0);
	if constexpr (::std::endian::native == ::std::endian::big)
	{
		/* poll32_events is word-reversed relative to poll_events on BE */
		poll_mask = (poll_mask >> 16) | (poll_mask << 16);
	}
	sqe->poll32_events = poll_mask;
}

inline void io_uring_prep_poll_multishot(::fast_io::liburing::io_uring_sqe *sqe, int fd, ::std::uint_least32_t poll_mask) noexcept
{
	io_uring_prep_poll_add(sqe, fd, poll_mask);
	sqe->len = io_uring_poll_add_multi;
}

inline void io_uring_prep_poll_remove(::fast_io::liburing::io_uring_sqe *sqe, ::std::uint_least64_t user_data) noexcept
{
	io_uring_prep_rw(io_uring_op_poll_remove, sqe, -1, nullptr, 0, 0);
	sqe->addr = user_data;
}

inline void io_uring_prep_timeout(::fast_io::liburing::io_uring_sqe *sqe, io_uring_timespec const *ts, ::std::uint_least32_t count,
								  ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_timeout, sqe, -1, ts, 1, count);
	sqe->timeout_flags = flags;
}

inline void io_uring_prep_timeout_remove(::fast_io::liburing::io_uring_sqe *sqe, ::std::uint_least64_t user_data,
										 ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_timeout_remove, sqe, -1, nullptr, 0, 0);
	sqe->addr = user_data;
	sqe->timeout_flags = flags;
}

inline void io_uring_prep_timeout_update(::fast_io::liburing::io_uring_sqe *sqe, io_uring_timespec const *ts,
										 ::std::uint_least64_t user_data, ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_timeout_remove, sqe, -1, nullptr, 0,
					 reinterpret_cast<::std::uint_least64_t>(ts));
	sqe->addr = user_data;
	sqe->timeout_flags = flags | io_uring_timeout_update;
}

inline void io_uring_prep_link_timeout(::fast_io::liburing::io_uring_sqe *sqe, io_uring_timespec const *ts,
									   ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_link_timeout, sqe, -1, ts, 1, 0);
	sqe->timeout_flags = flags;
}

inline void io_uring_prep_accept(::fast_io::liburing::io_uring_sqe *sqe, int fd, void *addr, void *addrlen,
								 ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_accept, sqe, fd, addr, 0, reinterpret_cast<::std::uint_least64_t>(addrlen));
	sqe->accept_flags = flags;
}

inline void io_uring_prep_multishot_accept(::fast_io::liburing::io_uring_sqe *sqe, int fd, void *addr, void *addrlen,
										   ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_accept(sqe, fd, addr, addrlen, flags);
	sqe->ioprio |= io_uring_accept_multishot;
}

inline void io_uring_prep_cancel64(::fast_io::liburing::io_uring_sqe *sqe, ::std::uint_least64_t user_data,
								   ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_async_cancel, sqe, -1, nullptr, 0, 0);
	sqe->addr = user_data;
	sqe->cancel_flags = flags;
}

inline void io_uring_prep_cancel_fd(::fast_io::liburing::io_uring_sqe *sqe, int fd, ::std::uint_least32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_async_cancel, sqe, fd, nullptr, 0, 0);
	sqe->cancel_flags = flags | io_uring_async_cancel_fd;
}

inline void io_uring_prep_connect(::fast_io::liburing::io_uring_sqe *sqe, int fd, void const *addr,
								  ::std::uint_least32_t addrlen) noexcept
{
	io_uring_prep_rw(io_uring_op_connect, sqe, fd, addr, 0, addrlen);
}

inline void io_uring_prep_openat(::fast_io::liburing::io_uring_sqe *sqe, int dfd, char const *path, ::std::uint_least32_t flags,
								 ::std::uint_least32_t mode) noexcept
{
	io_uring_prep_rw(io_uring_op_openat, sqe, dfd, path, mode, 0);
	sqe->open_flags = flags;
}

inline void io_uring_prep_close(::fast_io::liburing::io_uring_sqe *sqe, int fd) noexcept
{
	io_uring_prep_rw(io_uring_op_close, sqe, fd, nullptr, 0, 0);
}

inline void io_uring_prep_close_direct(::fast_io::liburing::io_uring_sqe *sqe, ::std::uint_least32_t file_index) noexcept
{
	io_uring_prep_close(sqe, 0);
	/* indexes are encoded as "index + 1"; 0 means "no fixed file" */
	sqe->file_index = file_index + 1;
}

inline void io_uring_prep_splice(::fast_io::liburing::io_uring_sqe *sqe, int fd_in, ::std::int_least64_t off_in, int fd_out,
								 ::std::int_least64_t off_out, ::std::uint_least32_t nbytes,
								 ::std::uint_least32_t splice_flags) noexcept
{
	io_uring_prep_rw(io_uring_op_splice, sqe, fd_out, nullptr, nbytes, static_cast<::std::uint_least64_t>(off_out));
	sqe->splice_off_in = static_cast<::std::uint_least64_t>(off_in);
	sqe->splice_fd_in = fd_in;
	sqe->splice_flags = splice_flags;
}

inline void io_uring_prep_tee(::fast_io::liburing::io_uring_sqe *sqe, int fd_in, int fd_out, ::std::uint_least32_t nbytes,
							  ::std::uint_least32_t splice_flags) noexcept
{
	io_uring_prep_rw(io_uring_op_tee, sqe, fd_out, nullptr, nbytes, 0);
	sqe->splice_off_in = 0;
	sqe->splice_fd_in = fd_in;
	sqe->splice_flags = splice_flags;
}

inline void io_uring_prep_shutdown(::fast_io::liburing::io_uring_sqe *sqe, int fd, ::std::uint_least32_t how) noexcept
{
	io_uring_prep_rw(io_uring_op_shutdown, sqe, fd, nullptr, how, 0);
}

/* request a direct (fixed) descriptor as the result of ops like
 * openat/accept; pass io_uring_file_index_alloc - 1 to let the kernel
 * pick the slot */
inline void io_uring_sqe_set_target_fixed_file(::fast_io::liburing::io_uring_sqe *sqe, ::std::uint_least32_t file_index) noexcept
{
	sqe->file_index = file_index + 1;
}


/* ======================= ring-level API ======================= */


/* ======================= submission queue ======================= */

/*
 * Return an sqe to fill, or nullptr if the SQ is full. Submission only
 * happens when the tail is published by io_uring_submit() (or a wait
 * function that submits on the caller's behalf).
 */
inline io_uring_sqe *io_uring_get_sqe(::fast_io::liburing::io_uring_ring_state &ring) noexcept
{
	auto &sq{ring.sq};
	::std::uint_least32_t head{::fast_io::liburing::details::io_uring_load_sq_head(ring)};
	::std::uint_least32_t tail{sq.sqe_tail};
	if (tail - head >= sq.ring_entries)
	{
		return nullptr;
	}
	io_uring_sqe *sqe{sq.sqes + ((tail & sq.ring_mask) << ::fast_io::liburing::details::io_uring_sqe_shift(ring))};
	sq.sqe_tail = tail + 1;
	::fast_io::liburing::details::io_uring_initialize_sqe(sqe);
	return sqe;
}

/* Returns how many unsubmitted entries are pending in the SQ */
inline ::std::uint_least32_t io_uring_sq_ready(::fast_io::liburing::io_uring_ring_state &ring) noexcept
{
	return ring.sq.sqe_tail - ::fast_io::liburing::details::io_uring_load_sq_head(ring);
}

/* Returns how much space is left in the SQ ring */
inline ::std::uint_least32_t io_uring_sq_space_left(::fast_io::liburing::io_uring_ring_state &ring) noexcept
{
	return ring.sq.ring_entries - io_uring_sq_ready(ring);
}

/*
 * Publish pending SQEs to the kernel. Returns the number submitted.
 * With io_uring_setup_sqpoll this only wakes the SQ thread when needed.
 */
inline ::std::uint_least32_t io_uring_submit(::fast_io::liburing::io_uring_ring_state &ring) throws
{
	::std::uint_least32_t submitted{::fast_io::liburing::details::io_uring_flush_sq(ring)};
	bool cq_enter{::fast_io::liburing::details::io_uring_cq_ring_needs_enter(ring)};
	::std::uint_least32_t flags{};
	if (::fast_io::liburing::details::io_uring_sq_ring_needs_enter(ring, submitted, flags) || cq_enter)
	{
		if (cq_enter)
		{
			flags |= io_uring_enter_getevents;
		}
		int ret{::fast_io::liburing::details::io_uring_enter_impl(ring, submitted, 0, flags)};
		::fast_io::system_call_throw_error(ret);
		return static_cast<::std::uint_least32_t>(ret);
	}
	return submitted;
}

/* Submit pending SQEs and wait for at least wait_nr CQEs */
inline ::std::uint_least32_t io_uring_submit_and_wait(::fast_io::liburing::io_uring_ring_state &ring, ::std::uint_least32_t wait_nr)
	throws
{
	::std::uint_least32_t submitted{::fast_io::liburing::details::io_uring_flush_sq(ring)};
	bool cq_enter{wait_nr != 0 || ::fast_io::liburing::details::io_uring_cq_ring_needs_enter(ring)};
	::std::uint_least32_t flags{};
	if (::fast_io::liburing::details::io_uring_sq_ring_needs_enter(ring, submitted, flags) || cq_enter)
	{
		if (cq_enter)
		{
			flags |= io_uring_enter_getevents;
		}
		int ret{::fast_io::liburing::details::io_uring_enter_impl(ring, submitted, wait_nr, flags)};
		::fast_io::system_call_throw_error(ret);
		return static_cast<::std::uint_least32_t>(ret);
	}
	return submitted;
}

/* Flush pending CQEs the kernel may be holding (IOPOLL/overflow/taskrun) */
inline void io_uring_get_events(::fast_io::liburing::io_uring_ring_state &ring) throws
{
	int ret{::fast_io::liburing::details::io_uring_enter_impl(ring, 0, 0, io_uring_enter_getevents)};
	::fast_io::system_call_throw_error(ret);
}

/*
 * SQPOLL only: wait for space to free up in the SQ ring. No-op for
 * non-SQPOLL rings or when space is already available.
 */
inline void io_uring_sqring_wait(::fast_io::liburing::io_uring_ring_state &ring) throws
{
	if (!(ring.flags & io_uring_setup_sqpoll) || io_uring_sq_space_left(ring) != 0)
	{
		return;
	}
	int ret{::fast_io::liburing::details::io_uring_enter_impl(ring, 0, 0, io_uring_enter_sq_wait)};
	::fast_io::system_call_throw_error(ret);
}

namespace details
{
/*
 * Ensure at least n sqe slots are free BEFORE any is taken. Linked
 * requests (e.g. an op and its io_uring_op_link_timeout partner) must be
 * submitted as adjacent sqes in one flush: a flush between them would
 * leave the kernel holding a link flag with nothing following it, so the
 * reservation loop runs first and the sqes are taken afterwards.
 */
inline void io_uring_reserve_sqes(::fast_io::liburing::io_uring_ring_state &ring, ::std::uint_least32_t n) throws
{
	while (io_uring_sq_space_left(ring) < n)
	{
		if (ring.flags & io_uring_setup_sqpoll)
		{
			int ret{io_uring_enter_impl(ring, 0, 0, io_uring_enter_sq_wait)};
			::fast_io::system_call_throw_error(ret);
		}
		else
		{
			::std::uint_least32_t submitted{io_uring_flush_sq(ring)};
			int ret{io_uring_enter_impl(ring, submitted, 0, 0)};
			::fast_io::system_call_throw_error(ret);
		}
	}
}

inline io_uring_sqe *ensure_io_uring_sqe(::fast_io::liburing::io_uring_ring_state &ring) throws
{
	io_uring_sqe *sqe{io_uring_get_sqe(ring)};
	while (sqe == nullptr)
	{
		/*
		 * SQ is full. Without SQPOLL, submitting publishes the pending
		 * SQEs and the kernel advances the head as it consumes them.
		 * With SQPOLL the kernel thread drains the SQ by itself; wait
		 * for space instead.
		 */
		if (ring.flags & io_uring_setup_sqpoll)
		{
			int ret{io_uring_enter_impl(ring, 0, 0, io_uring_enter_sq_wait)};
			::fast_io::system_call_throw_error(ret);
		}
		else
		{
			::std::uint_least32_t submitted{io_uring_flush_sq(ring)};
			int ret{io_uring_enter_impl(ring, submitted, 0, 0)};
			::fast_io::system_call_throw_error(ret);
		}
		sqe = io_uring_get_sqe(ring);
	}
	return sqe;
}
} // namespace details


/* ======================= completion queue ======================= */

/* Returns how many unconsumed entries are ready in the CQ ring */
inline ::std::uint_least32_t io_uring_cq_ready(::fast_io::liburing::io_uring_ring_state &ring) noexcept
{
	return ::fast_io::liburing::details::io_uring_load_acquire(ring.cq.ktail) - *ring.cq.khead;
}

/*
 * Must be called after the application has consumed nr CQ slots, so the
 * kernel can reuse them.
 */
inline void io_uring_cq_advance(::fast_io::liburing::io_uring_ring_state &ring, ::std::uint_least32_t nr) noexcept
{
	if (nr != 0)
	{
		::fast_io::liburing::details::io_uring_store_release(ring.cq.khead, *ring.cq.khead + nr);
	}
}

/* Mark one CQE as consumed */
inline void io_uring_cqe_seen(::fast_io::liburing::io_uring_ring_state &ring, io_uring_cqe const *cqe) noexcept
{
	if (cqe != nullptr)
	{
		io_uring_cq_advance(ring, io_uring_cqe_nr(cqe));
	}
}

/*
 * Peek at the next completion without entering the kernel unless CQEs
 * might be pending a flush (IOPOLL / overflow / taskrun). Returns nullptr
 * when no completion is currently available.
 */
inline io_uring_cqe *io_uring_peek_cqe(::fast_io::liburing::io_uring_ring_state &ring) throws
{
	io_uring_cqe *cqe{::fast_io::liburing::details::io_uring_peek_cqe_impl(ring).cqe_ptr};
	if (cqe != nullptr)
	{
		return cqe;
	}
	if (!(ring.flags & io_uring_setup_iopoll) &&
		!(::fast_io::liburing::details::io_uring_load_acquire(ring.sq.kflags) & (io_uring_sq_cq_overflow | io_uring_sq_taskrun)))
	{
		return nullptr;
	}
	/* slow path: one kernel round trip with wait_nr = 0; when even that
	 * comes back empty the result is EAGAIN, delivered like any error */
	::fast_io::liburing::details::io_uring_get_data data{.submit = 0, .wait_nr = 0, .get_flags = 0, .sz = 0, .has_ts = false, .arg = nullptr};
	return ::fast_io::liburing::details::io_uring_get_cqe_impl(ring, data);
}

/* Wait for (at least) wait_nr completions; returns the first CQE */
inline io_uring_cqe *io_uring_wait_cqes(::fast_io::liburing::io_uring_ring_state &ring, ::std::uint_least32_t wait_nr)
	throws
{
	::fast_io::liburing::details::io_uring_get_data data{.submit = 0, .wait_nr = wait_nr, .get_flags = 0, .sz = 0, .has_ts = false, .arg = nullptr};
	return ::fast_io::liburing::details::io_uring_get_cqe_impl(ring, data);
}

inline io_uring_cqe *io_uring_wait_cqe(::fast_io::liburing::io_uring_ring_state &ring) throws
{
	return io_uring_wait_cqes(ring, 1);
}

/* Submit pending SQEs, then wait for wait_nr completions */
inline io_uring_cqe *io_uring_submit_and_wait_cqes(::fast_io::liburing::io_uring_ring_state &ring,
												   ::std::uint_least32_t wait_nr) throws
{
	::fast_io::liburing::details::io_uring_get_data data{.submit = ::fast_io::liburing::details::io_uring_flush_sq(ring),
														 .wait_nr = wait_nr,
														 .get_flags = 0,
														 .sz = 0,
														 .has_ts = false,
														 .arg = nullptr};
	return ::fast_io::liburing::details::io_uring_get_cqe_impl(ring, data);
}

/*
 * Wait for a completion with a timeout. On kernels with
 * io_uring_feat_ext_arg the deadline rides in io_uring_enter(2) directly;
 * on older kernels an internal io_uring_op_timeout SQE is queued instead.
 * Returns nullptr when the deadline elapsed.
 */
inline io_uring_cqe *io_uring_wait_cqe_timeout(::fast_io::liburing::io_uring_ring_state &ring,
											   ::fast_io::posix_statx_timestamp64 timestamp) throws
{
	/* the kernel reads the full 64-bit nsec field out of the address we
	 * hand it; convert so the statx layout's 4-byte tail padding can
	 * never leak garbage into it */
	io_uring_timespec ts{static_cast<::std::int64_t>(timestamp.tv_sec),
						 static_cast<::std::int64_t>(timestamp.tv_nsec)};
	io_uring_cqe *cqe{};
	if (ring.features & io_uring_feat_ext_arg)
	{
		io_uring_getevents_arg arg{0, 0, 0, reinterpret_cast<::std::uint_least64_t>(__builtin_addressof(ts))};
		::fast_io::liburing::details::io_uring_get_data data{.submit = 0,
															 .wait_nr = 1,
															 .get_flags = io_uring_enter_ext_arg,
															 .sz = sizeof(arg),
															 .has_ts = true,
															 .arg = __builtin_addressof(arg)};
		cqe = ::fast_io::liburing::details::io_uring_get_cqe_impl(ring, data);
	}
	else
	{
		/* queue an internal timeout SQE that the kernel completes when
		 * either the deadline expires or a CQE is posted */
		io_uring_sqe *sqe{::fast_io::liburing::details::ensure_io_uring_sqe(ring)};
		sqe->opcode = io_uring_op_timeout;
		sqe->fd = -1;
		sqe->addr = reinterpret_cast<::std::uint_least64_t>(__builtin_addressof(ts));
		sqe->len = 1;
		sqe->off = 1;
		sqe->timeout_flags = 0;
		sqe->user_data = io_uring_internal_timeout_user_data;
		::fast_io::liburing::details::io_uring_get_data data{.submit = ::fast_io::liburing::details::io_uring_flush_sq(ring),
															 .wait_nr = 1,
															 .get_flags = 0,
															 .sz = 0,
															 .has_ts = false,
															 .arg = nullptr};
		try
		{
			cqe = ::fast_io::liburing::details::io_uring_get_cqe_impl(ring, data);
		}
		catch throws(::std::error e)
		{
			/* the internal timeout sqe completed: deadline expired */
			if (e != static_cast<::std::errc>(ETIME))
			{
				throw throws;
			}
		}
	}
	return cqe;
}

/*
 * Fill an array of CQE pointers for the currently available completions.
 * Returns the number filled; nothing is consumed — call
 * io_uring_cq_advance afterwards with the total slot count
 * (sum of io_uring_cqe_nr over the returned CQEs).
 */
inline ::std::uint_least32_t io_uring_peek_batch_cqe(::fast_io::liburing::io_uring_ring_state &ring, io_uring_cqe **cqes,
													 ::std::uint_least32_t count) throws
{
	::std::uint_least32_t ready{io_uring_cq_ready(ring)};
	if (ready == 0)
	{
		if (!::fast_io::liburing::details::io_uring_cq_ring_needs_flush(ring))
		{
			return 0;
		}
		io_uring_get_events(ring);
		ready = io_uring_cq_ready(ring);
		if (ready == 0)
		{
			return 0;
		}
	}
	::std::uint_least32_t head{*ring.cq.khead};
	::std::uint_least32_t mask{ring.cq.ring_mask};
	::std::uint_least32_t shift{::fast_io::liburing::details::io_uring_cqe_shift(ring)};
	::std::uint_least32_t nr{};
	::std::uint_least32_t last{head + ready};
	while (head != last && nr < count)
	{
		io_uring_cqe *cqe{ring.cq.cqes + ((head & mask) << shift)};
		if (cqe->flags & io_uring_cqe_f_skip)
		{
			/* a skip entry can only be consumed at the CQ head */
			if (nr != 0)
			{
				break;
			}
			io_uring_cq_advance(ring, 1);
			++head;
			continue;
		}
		head += io_uring_cqe_nr(cqe);
		cqes[nr++] = cqe;
	}
	return nr;
}

/* ======================= io_uring_register(2) ======================= */

inline int io_uring_register(::fast_io::liburing::io_uring_ring_state &ring, ::std::uint_least32_t opcode, void const *arg,
							 ::std::uint_least32_t nr_args) throws
{
	int ret{::fast_io::liburing::details::io_uring_register_impl(static_cast<::std::uint_least32_t>(ring.ring_fd), opcode, arg,
																 nr_args)};
	::fast_io::system_call_throw_error(ret);
	return ret;
}

inline int io_uring_register_buffers(::fast_io::liburing::io_uring_ring_state &ring, io_scatter_t const *iovecs,
									 ::std::uint_least32_t nr) throws
{
	return io_uring_register(ring, io_uring_regop_buffers, iovecs, nr);
}

inline int io_uring_unregister_buffers(::fast_io::liburing::io_uring_ring_state &ring) throws
{
	return io_uring_register(ring, io_uring_regop_unregister_buffers, nullptr, 0);
}

inline int io_uring_register_files(::fast_io::liburing::io_uring_ring_state &ring, int const *files,
								   ::std::uint_least32_t nr) throws
{
	return io_uring_register(ring, io_uring_regop_files, files, nr);
}

inline int io_uring_unregister_files(::fast_io::liburing::io_uring_ring_state &ring) throws
{
	return io_uring_register(ring, io_uring_regop_unregister_files, nullptr, 0);
}

} // namespace fast_io::liburing

#endif
