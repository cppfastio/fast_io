	.file	"sha512.cc"
	.text
	.p2align 4
	.globl	_Z14sha512_invokerPyPKSt4byteS2_
	.def	_Z14sha512_invokerPyPKSt4byteS2_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14sha512_invokerPyPKSt4byteS2_
_Z14sha512_invokerPyPKSt4byteS2_:
.LFB3252:
	pushp	%r15
	.seh_pushreg	%r15
	push2p	%r13, %r14
	.seh_stackalloc	16
	.seh_savereg	%r14, 8
	.seh_savereg	%r13, 0
	push2p	%rbp, %r12
	.seh_stackalloc	16
	.seh_savereg	%r12, 8
	.seh_savereg	%rbp, 0
	push2p	%rsi, %rdi
	.seh_stackalloc	16
	.seh_savereg	%rdi, 8
	.seh_savereg	%rsi, 0
	pushp	%rbx
	.seh_pushreg	%rbx
	subq	$56, %rsp
	.seh_stackalloc	56
	.seh_endprologue
	movq	%rdx, 136(%rsp)
	movq	%r8, 144(%rsp)
	cmpq	%r8, %rdx
	je	.L1
	movq	8(%rcx), %rax
	movq	(%rcx), %r14
	movq	%rcx, 128(%rsp)
	movq	32(%rcx), %r15
	movq	40(%rcx), %r13
	movq	%rax, (%rsp)
	movq	16(%rcx), %rax
	movq	%rax, 8(%rsp)
	movq	24(%rcx), %rax
	movq	%rax, 16(%rsp)
	movq	48(%rcx), %rax
	movq	%rax, 24(%rsp)
	movq	56(%rcx), %rax
	movq	%rax, 32(%rsp)
	.p2align 4
	.p2align 3
.L4:
	movq	136(%rsp), %rax
	rorq	$14, %r15, %rdx
	movq	24(%rsp), %rbx
	movabsq	$4794697086780616226, %rcx
	movq	(%rsp), %rsi
	movq	8(%rsp), %rdi
	movq	(%rax), %rax
	xorq	%r14, %rsi, %r8
	bswap	%rax
	movq	%rax, %r12
	rorq	$18, %r15, %rax
	xorq	%rax, %rdx
	rolq	$23, %r15, %rax
	xorq	%rax, %rdx
	addq	%r12, 32(%rsp), %rax
	addq	%rcx, %rax
	rolq	$30, %r14, %rcx
	addq	%rax, %rdx
	xorq	%r13, %rbx, %rax
	andq	%r15, %rax
	xorq	%rbx, %rax
	addq	%rax, %rdx
	rorq	$28, %r14, %rax
	addq	%rdx, 16(%rsp), %r11
	xorq	%rcx, %rax
	rolq	$25, %r14, %rcx
	xorq	%rcx, %rax
	xorq	%rsi, %rdi, %rcx
	rorq	$18, %r11, %r9
	andq	%r8, %rcx
	xorq	%rsi, %rcx
	addq	%rcx, %rax
	xorq	%r13, %r15, %rcx
	addq	%rdx, %rax
	movq	136(%rsp), %rdx
	andq	%r11, %rcx
	xorq	%r13, %rcx
	movq	8(%rdx), %r31
	bswap	%r31
	leaq	(%rbx,%r31), %rdx
	movabsq	$8158064640168781261, %rbx
	addq	%rbx, %rdx
	movabsq	$-5349999486874862801, %rbx
	addq	%rdx, %rcx
	rorq	$14, %r11, %rdx
	xorq	%r9, %rdx
	rolq	$23, %r11, %r9
	xorq	%r9, %rdx
	rolq	$30, %rax, %r10
	xorq	%r14, %rax, %r9
	addq	%rdx, %rcx
	rorq	$28, %rax, %rdx
	andq	%r9, %r8
	xorq	%r10, %rdx
	rolq	$25, %rax, %r10
	xorq	%r14, %r8
	leaq	(%rdi,%rcx), %r16
	xorq	%r10, %rdx
	rorq	$18, %r16, %r10
	addq	%r8, %rdx
	xorq	%r15, %r11, %r8
	addq	%rcx, %rdx
	movq	136(%rsp), %rcx
	andq	%r16, %r8
	xorq	%r15, %r8
	rolq	$30, %rdx, %r18
	movq	16(%rcx), %r30
	bswap	%r30
	leaq	(%r30,%r13), %rcx
	addq	%rbx, %rcx
	movq	136(%rsp), %rbx
	addq	%rcx, %r8
	rorq	$14, %r16, %rcx
	xorq	%r10, %rcx
	rolq	$23, %r16, %r10
	movq	64(%rbx), %r23
	movabsq	$-2880145864133508542, %rbx
	xorq	%r10, %rcx
	xorq	%rdx, %rax, %r10
	addq	%rcx, %r8
	rorq	$28, %rdx, %rcx
	andq	%r10, %r9
	bswap	%r23
	leaq	(%rsi,%r8), %r17
	movq	136(%rsp), %rsi
	xorq	%r18, %rcx
	rolq	$25, %rdx, %r18
	xorq	%rax, %r9
	xorq	%r18, %rcx
	movq	24(%rsi), %r29
	addq	%r9, %rcx
	xorq	%r16, %r11, %r9
	rorq	$18, %r17, %r18
	movabsq	$-1606136188198331460, %rsi
	addq	%r8, %rcx
	andq	%r17, %r9
	bswap	%r29
	leaq	(%r29,%r15), %r8
	xorq	%r11, %r9
	xorq	%rcx, %rdx, %r19
	addq	%rsi, %r8
	rolq	$30, %rcx, %r20
	movq	136(%rsp), %rsi
	andq	%r19, %r10
	addq	%r8, %r9
	rorq	$14, %r17, %r8
	xorq	%rdx, %r10
	xorq	%r18, %r8
	rolq	$23, %r17, %r18
	movq	32(%rsi), %r28
	movabsq	$4131703408338449720, %rsi
	xorq	%r18, %r8
	addq	%r8, %r9
	rorq	$28, %rcx, %r8
	bswap	%r28
	xorq	%r20, %r8
	rolq	$25, %rcx, %r20
	leaq	(%r9,%r14), %r18
	xorq	%r20, %r8
	addq	%r10, %r8
	xorq	%r17, %r16, %r10
	addq	%r9, %r8
	andq	%r18, %r10
	leaq	(%r28,%rsi), %r9
	movq	136(%rsp), %rsi
	addq	%r11, %r9
	xorq	%r16, %r10
	rorq	$18, %r18, %r11
	addq	%r9, %r10
	rorq	$14, %r18, %r9
	movq	40(%rsi), %r27
	movabsq	$6480981068601479193, %rsi
	xorq	%r11, %r9
	rolq	$23, %r18, %r11
	xorq	%r11, %r9
	xorq	%r8, %rcx, %r20
	rolq	$30, %r8, %r11
	bswap	%r27
	addq	%r9, %r10
	rorq	$28, %r8, %r9
	andq	%r20, %r19
	xorq	%r11, %r9
	rolq	$25, %r8, %r11
	xorq	%rcx, %r19
	addq	%r10, %rax
	xorq	%r11, %r9
	xorq	%r18, %r17, %r11
	addq	%r19, %r9
	andq	%rax, %r11
	addq	%r10, %r9
	leaq	(%r27,%rsi), %r10
	xorq	%r17, %r11
	movq	136(%rsp), %rsi
	addq	%r16, %r10
	rorq	$18, %rax, %r16
	xorq	%r9, %r8, %r19
	addq	%r10, %r11
	rorq	$14, %rax, %r10
	andq	%r19, %r20
	movq	48(%rsi), %r26
	movabsq	$-7908458776815382629, %rsi
	xorq	%r16, %r10
	rolq	$23, %rax, %r16
	xorq	%r16, %r10
	rolq	$30, %r9, %r16
	bswap	%r26
	addq	%r10, %r11
	rorq	$28, %r9, %r10
	xorq	%r16, %r10
	rolq	$25, %r9, %r16
	addq	%r11, %rdx
	xorq	%r16, %r10
	xorq	%r8, %r20
	xorq	%rax, %r18, %r16
	addq	%r20, %r10
	andq	%rdx, %r16
	addq	%r11, %r10
	leaq	(%r26,%rsi), %r11
	xorq	%r18, %r16
	movq	136(%rsp), %rsi
	addq	%r17, %r11
	rorq	$18, %rdx, %r17
	addq	%r11, %r16
	rorq	$14, %rdx, %r11
	movq	56(%rsi), %r24
	movabsq	$-6116909921290321640, %rsi
	xorq	%r17, %r11
	rolq	$23, %rdx, %r17
	xorq	%r17, %r11
	rolq	$30, %r10, %r20
	xorq	%r10, %r9, %r17
	bswap	%r24
	addq	%r11, %r16
	rorq	$28, %r10, %r11
	andq	%r17, %r19
	xorq	%r20, %r11
	rolq	$25, %r10, %r20
	xorq	%r9, %r19
	addq	%r16, %rcx
	xorq	%r20, %r11
	addq	%r19, %r11
	leaq	(%r24,%rsi), %r19
	addq	%r16, %r11
	xorq	%rdx, %rax, %r16
	addq	%r19, %r18
	rorq	$18, %rcx, %r19
	andq	%rcx, %r16
	xorq	%rax, %r16
	addq	%r18, %r16
	rorq	$14, %rcx, %r18
	xorq	%r19, %r18
	rolq	$23, %rcx, %r19
	xorq	%r19, %r18
	rorq	$28, %r11, %rsi
	addq	%r18, %r16
	rolq	$30, %r11, %r19
	xorq	%r11, %r10, %r18
	xorq	%r19, %rsi
	andq	%r18, %r17
	rolq	$25, %r11, %r19
	addq	%r16, %r8
	xorq	%r10, %r17
	xorq	%r19, %rsi
	addq	%r17, %rsi
	leaq	(%r23,%rbx), %r17
	movq	136(%rsp), %rbx
	addq	%r16, %rsi
	xorq	%rcx, %rdx, %r16
	addq	%r17, %rax
	rorq	$18, %r8, %r17
	andq	%r8, %r16
	rolq	$30, %rsi, %r19
	movq	72(%rbx), %r22
	movabsq	$1334009975649890238, %rbx
	xorq	%rdx, %r16
	addq	%rax, %r16
	rorq	$14, %r8, %rax
	bswap	%r22
	xorq	%r17, %rax
	rolq	$23, %r8, %r17
	xorq	%r17, %rax
	xorq	%rsi, %r11, %r17
	addq	%rax, %r16
	rorq	$28, %rsi, %rax
	xorq	%rax, %r19
	rolq	$25, %rsi, %rax
	addq	%r16, %r9
	xorq	%r19, %rax
	andq	%r17, %r18
	xorq	%r11, %r18
	addq	%r18, %rax
	leaq	(%r22,%rbx), %r18
	movq	136(%rsp), %rbx
	addq	%r16, %rax
	xorq	%r8, %rcx, %r16
	addq	%rdx, %r18
	rorq	$18, %r9, %rdx
	andq	%r9, %r16
	rolq	$30, %rax, %r19
	movq	80(%rbx), %r21
	movabsq	$2608012711638119052, %rbx
	xorq	%rcx, %r16
	addq	%r18, %r16
	rorq	$14, %r9, %r18
	bswap	%r21
	xorq	%r18, %rdx
	rolq	$23, %r9, %r18
	xorq	%r18, %rdx
	xorq	%rax, %rsi, %r18
	addq	%rdx, %r16
	rorq	$28, %rax, %rdx
	andq	%r18, %r17
	xorq	%rdx, %r19
	rolq	$25, %rax, %rdx
	xorq	%rsi, %r17
	addq	%r16, %r10
	xorq	%r19, %rdx
	addq	%r17, %rdx
	leaq	(%r21,%rbx), %r17
	movq	136(%rsp), %rbx
	addq	%r16, %rdx
	xorq	%r9, %r8, %r16
	addq	%rcx, %r17
	andq	%r10, %r16
	movq	88(%rbx), %r20
	movabsq	$6128411473006802146, %rbx
	xorq	%r8, %r16
	addq	%r17, %r16
	rorq	$14, %r10, %r17
	bswap	%r20
	rorq	$18, %r10, %rcx
	rolq	$30, %rdx, %r19
	xorq	%r17, %rcx
	rolq	$23, %r10, %r17
	xorq	%r17, %rcx
	xorq	%rdx, %rax, %r17
	addq	%rcx, %r16
	rorq	$28, %rdx, %rcx
	andq	%r17, %r18
	xorq	%rcx, %r19
	rolq	$25, %rdx, %rcx
	xorq	%rax, %r18
	addq	%r16, %r11
	xorq	%r19, %rcx
	addq	%r18, %rcx
	leaq	(%r20,%rbx), %r18
	movq	136(%rsp), %rbx
	addq	%r16, %rcx
	xorq	%r10, %r9, %r16
	addq	%r8, %r18
	rorq	$18, %r11, %r8
	andq	%r11, %r16
	rolq	$30, %rcx, %r19
	xorq	%r9, %r16
	addq	%r18, %r16
	rorq	$14, %r11, %r18
	xorq	%r18, %r8
	rolq	$23, %r11, %r18
	xorq	%r18, %r8
	xorq	%rcx, %rdx, %r18
	addq	%r8, %r16
	rorq	$28, %rcx, %r8
	addq	%r16, %rsi
	xorq	%r8, %r19
	rolq	$25, %rcx, %r8
	andq	%r18, %r17
	xorq	%rdx, %r17
	xorq	%r19, %r8
	movq	96(%rbx), %r19
	movabsq	$8268148722764581231, %rbx
	addq	%r17, %r8
	addq	%r16, %r8
	xorq	%r11, %r10, %r16
	bswap	%r19
	leaq	(%r19,%rbx), %r17
	andq	%rsi, %r16
	addq	%r9, %r17
	rorq	$18, %rsi, %r9
	movq	136(%rsp), %rbx
	xorq	%r10, %r16
	rolq	$30, %r8, %r25
	addq	%r17, %r16
	rorq	$14, %rsi, %r17
	xorq	%r17, %r9
	rolq	$23, %rsi, %r17
	xorq	%r17, %r9
	xorq	%r8, %rcx, %r17
	addq	%r9, %r16
	rorq	$28, %r8, %r9
	andq	%r17, %r18
	xorq	%r9, %r25
	rolq	$25, %r8, %r9
	xorq	%rcx, %r18
	addq	%r16, %rax
	xorq	%r25, %r9
	addq	%r18, %r9
	movq	104(%rbx), %r18
	movq	%r14, 40(%rsp)
	movabsq	$-9160688886553864527, %rbx
	addq	%r16, %r9
	xorq	%rsi, %r11, %r16
	andq	%rax, %r16
	bswap	%r18
	leaq	(%r18,%rbx), %r25
	movq	136(%rsp), %rbx
	xorq	%r11, %r16
	addq	%r10, %r25
	rorq	$18, %rax, %r10
	addq	%r25, %r16
	rorq	$14, %rax, %r25
	xorq	%r25, %r10
	rolq	$23, %rax, %r25
	xorq	%r25, %r10
	rolq	$30, %r9, %rdi
	xorq	%r9, %r8, %r25
	addq	%r10, %r16
	rorq	$28, %r9, %r10
	andq	%r25, %r17
	xorq	%r10, %rdi
	rolq	$25, %r9, %r10
	xorq	%r8, %r17
	addq	%r16, %rdx
	xorq	%rdi, %r10
	addq	%r17, %r10
	movq	112(%rbx), %r17
	movabsq	$-7215885187991268811, %rbx
	addq	%r16, %r10
	xorq	%rax, %rsi, %r16
	andq	%rdx, %r16
	bswap	%r17
	leaq	(%r17,%rbx), %rdi
	movq	136(%rsp), %rbx
	addq	%r11, %rdi
	xorq	%rsi, %r16
	rorq	$18, %rdx, %r11
	addq	%rdi, %r16
	rorq	$14, %rdx, %rdi
	xorq	%rdi, %r11
	rolq	$23, %rdx, %rdi
	xorq	%rdi, %r11
	xorq	%r10, %r9, %rdi
	addq	%r11, %r16
	addq	%r16, %rcx
	rorq	$28, %r10, %r11
	andq	%rdi, %r25
	rolq	$30, %r10, %rbp
	xorq	%r9, %r25
	xorq	%r11, %rbp
	rolq	$25, %r10, %r11
	xorq	%rbp, %r11
	addq	%r25, %r11
	xorq	%rdx, %rax, %r25
	addq	%r16, %r11
	movq	120(%rbx), %r16
	andq	%rcx, %r25
	movabsq	$-4495734319001033068, %rbx
	xorq	%rax, %r25
	bswap	%r16
	leaq	(%r16,%rbx), %rbp
	rolq	$30, %r11, %rbx
	addq	%rsi, %rbp
	rorq	$18, %rcx, %rsi
	addq	%rbp, %r25
	rorq	$14, %rcx, %rbp
	xorq	%rbp, %rsi
	rolq	$23, %rcx, %rbp
	xorq	%rbp, %rsi
	xorq	%r11, %r10, %rbp
	addq	%rsi, %r25
	rorq	$28, %r11, %rsi
	andq	%rbp, %rdi
	xorq	%rsi, %rbx
	rolq	$25, %r11, %rsi
	xorq	%r10, %rdi
	addq	%r25, %r8
	xorq	%rbx, %rsi
	addq	%rdi, %rsi
	addq	%r25, %rsi
	leaq	_ZN7fast_io7details6sha51210k512scalarE(%rip), %r25
.L3:
	rorq	$8, %r31, %rdi
	rorq	$1, %r31, %rbx
	xorq	%rdi, %rbx
	shrq	$7, %r31, %rdi
	rorq	$19, %r17, %r14
	xorq	%rdi, %rbx
	rolq	$3, %r17, %rdi
	xorq	%r14, %rdi
	shrq	$6, %r17, %r14
	xorq	%r14, %rdi
	rorq	$28, %rsi, %r14
	addq	%rdi, %rbx
	rorq	$18, %r8, %rdi
	addq	%r22, %rbx
	addq	%rbx, %r12
	rorq	$14, %r8, %rbx
	xorq	%rdi, %rbx
	rolq	$23, %r8, %rdi
	xorq	%rdi, %rbx
	xorq	%rdx, %rcx, %rdi
	andq	%r8, %rdi
	xorq	%rdx, %rdi
	addq	%rdi, %rbx
	addq	(%r25), %rbx
	xorq	%r11, %rsi, %rdi
	addq	%r12, %rbx
	addq	%rax, %rbx
	rolq	$30, %rsi, %rax
	xorq	%r14, %rax
	rolq	$25, %rsi, %r14
	addq	%rbx, %r9
	xorq	%r14, %rax
	addq	%rbx, %rax
	andq	%rdi, %rbp
	rorq	$1, %r30, %rbx
	addq	%r21, %r31
	xorq	%r11, %rbp
	addq	8(%r25), %rdx
	addq	%rbp, %rax
	rorq	$8, %r30, %rbp
	xorq	%rbp, %rbx
	shrq	$7, %r30, %rbp
	xorq	%rbp, %rbx
	rolq	$3, %r16, %rbp
	addq	%r31, %rbx
	rorq	$19, %r16, %r31
	xorq	%rbp, %r31
	shrq	$6, %r16, %rbp
	xorq	%rbp, %r31
	xorq	%rcx, %r8, %rbp
	addq	%rbx, %r31
	andq	%r9, %rbp
	rorq	$18, %r9, %rbx
	xorq	%rcx, %rbp
	addq	%r31, %rdx
	addq	%rdx, %rbp
	rorq	$14, %r9, %rdx
	xorq	%rbx, %rdx
	rolq	$23, %r9, %rbx
	xorq	%rbx, %rdx
	xorq	%rsi, %rax, %rbx
	addq	%rdx, %rbp
	rorq	$28, %rax, %rdx
	addq	%rbp, %r10
	rolq	$30, %rax, %r14
	andq	%rbx, %rdi
	addq	%r20, %r30
	xorq	%rdx, %r14
	rolq	$25, %rax, %rdx
	xorq	%rsi, %rdi
	addq	16(%r25), %rcx
	xorq	%r14, %rdx
	addq	%rdi, %rdx
	rorq	$1, %r29, %rdi
	addq	%rbp, %rdx
	rorq	$8, %r29, %rbp
	xorq	%rbp, %rdi
	shrq	$7, %r29, %rbp
	xorq	%rbp, %rdi
	rolq	$3, %r12, %rbp
	addq	%r30, %rdi
	rorq	$19, %r12, %r30
	xorq	%rbp, %r30
	shrq	$6, %r12, %rbp
	xorq	%rbp, %r30
	xorq	%r8, %r9, %rbp
	addq	%rdi, %r30
	andq	%r10, %rbp
	rorq	$18, %r10, %rdi
	xorq	%r8, %rbp
	addq	%r30, %rcx
	addq	%rcx, %rbp
	rorq	$14, %r10, %rcx
	xorq	%rdi, %rcx
	rolq	$23, %r10, %rdi
	xorq	%rdi, %rcx
	addq	%rcx, %rbp
	xorq	%rdx, %rax, %rdi
	rorq	$28, %rdx, %rcx
	addq	%r19, %r29
	rolq	$30, %rdx, %r14
	andq	%rdi, %rbx
	addq	%rbp, %r11
	addq	24(%r25), %r8
	xorq	%rcx, %r14
	rolq	$25, %rdx, %rcx
	xorq	%rax, %rbx
	xorq	%r14, %rcx
	addq	%rbx, %rcx
	rorq	$1, %r28, %rbx
	addq	%rbp, %rcx
	rorq	$8, %r28, %rbp
	xorq	%rbp, %rbx
	shrq	$7, %r28, %rbp
	xorq	%rbp, %rbx
	rolq	$3, %r31, %rbp
	addq	%r29, %rbx
	rorq	$19, %r31, %r29
	xorq	%rbp, %r29
	shrq	$6, %r31, %rbp
	xorq	%rbp, %r29
	xorq	%r10, %r9, %rbp
	addq	%rbx, %r29
	andq	%r11, %rbp
	xorq	%r9, %rbp
	addq	%r29, %r8
	addq	%r8, %rbp
	rorq	$14, %r11, %r8
	rorq	$18, %r11, %rbx
	rolq	$30, %rcx, %r14
	addq	%r18, %r28
	xorq	%rbx, %r8
	rolq	$23, %r11, %rbx
	xorq	%rbx, %r8
	xorq	%rcx, %rdx, %rbx
	addq	%r8, %rbp
	rorq	$28, %rcx, %r8
	andq	%rbx, %rdi
	xorq	%r8, %r14
	rolq	$25, %rcx, %r8
	xorq	%rdx, %rdi
	addq	%rbp, %rsi
	xorq	%r14, %r8
	addq	%rdi, %r8
	rorq	$1, %r27, %rdi
	addq	%rbp, %r8
	rorq	$8, %r27, %rbp
	xorq	%rbp, %rdi
	shrq	$7, %r27, %rbp
	xorq	%rbp, %rdi
	rolq	$3, %r30, %rbp
	addq	%r28, %rdi
	rorq	$19, %r30, %r28
	xorq	%rbp, %r28
	shrq	$6, %r30, %rbp
	xorq	%rbp, %r28
	addq	%rdi, %r28
	xorq	%r11, %r10, %rdi
	andq	%rsi, %rdi
	xorq	%r10, %rdi
	addq	%r9, 32(%r25), %rbp
	rorq	$14, %rsi, %r9
	addq	%r17, %r27
	addq	%r28, %rbp
	rolq	$30, %r8, %r14
	addq	%rdi, %rbp
	rorq	$18, %rsi, %rdi
	xorq	%rdi, %r9
	rolq	$23, %rsi, %rdi
	xorq	%rdi, %r9
	xorq	%r8, %rcx, %rdi
	addq	%r9, %rbp
	rorq	$28, %r8, %r9
	andq	%rdi, %rbx
	xorq	%r9, %r14
	rolq	$25, %r8, %r9
	xorq	%rcx, %rbx
	addq	%rbp, %rax
	xorq	%r14, %r9
	addq	%rbx, %r9
	rorq	$1, %r26, %rbx
	addq	%rbp, %r9
	rorq	$8, %r26, %rbp
	xorq	%rbp, %rbx
	shrq	$7, %r26, %rbp
	xorq	%rbp, %rbx
	rolq	$3, %r29, %rbp
	addq	%r27, %rbx
	rorq	$19, %r29, %r27
	xorq	%rbp, %r27
	shrq	$6, %r29, %rbp
	xorq	%rbp, %r27
	addq	%rbx, %r27
	xorq	%rsi, %r11, %rbx
	addq	%r10, 40(%r25), %rbp
	rorq	$14, %rax, %r10
	andq	%rax, %rbx
	addq	%r27, %rbp
	rolq	$30, %r9, %r14
	addq	%r16, %r26
	xorq	%r11, %rbx
	addq	%rbx, %rbp
	rorq	$18, %rax, %rbx
	xorq	%rbx, %r10
	rolq	$23, %rax, %rbx
	xorq	%rbx, %r10
	xorq	%r9, %r8, %rbx
	addq	%r10, %rbp
	rorq	$28, %r9, %r10
	andq	%rbx, %rdi
	xorq	%r10, %r14
	rolq	$25, %r9, %r10
	xorq	%r8, %rdi
	addq	%rbp, %rdx
	xorq	%r14, %r10
	addq	%rdi, %r10
	rorq	$1, %r24, %rdi
	addq	%rbp, %r10
	rorq	$8, %r24, %rbp
	xorq	%rbp, %rdi
	shrq	$7, %r24, %rbp
	xorq	%rbp, %rdi
	addq	%r26, %rdi
	rorq	$19, %r28, %r26
	rolq	$3, %r28, %rbp
	rolq	$30, %r10, %r14
	xorq	%rbp, %r26
	shrq	$6, %r28, %rbp
	xorq	%rbp, %r26
	addq	%rdi, %r26
	xorq	%rax, %rsi, %rdi
	andq	%rdx, %rdi
	leaq	(%r11,%r26), %rbp
	addq	48(%r25), %rbp
	rorq	$14, %rdx, %r11
	xorq	%rsi, %rdi
	addq	%rdi, %rbp
	rorq	$18, %rdx, %rdi
	xorq	%rdi, %r11
	rolq	$23, %rdx, %rdi
	xorq	%rdi, %r11
	xorq	%r10, %r9, %rdi
	addq	%r11, %rbp
	rorq	$28, %r10, %r11
	andq	%rdi, %rbx
	xorq	%r11, %r14
	rolq	$25, %r10, %r11
	xorq	%r9, %rbx
	addq	%rbp, %rcx
	xorq	%r14, %r11
	addq	%rbx, %r11
	rorq	$1, %r23, %rbx
	addq	%rbp, %r11
	rorq	$8, %r23, %rbp
	xorq	%rbp, %rbx
	shrq	$7, %r23, %rbp
	xorq	%rbp, %rbx
	addq	%rbx, %r24
	rolq	$3, %r27, %rbp
	leaq	(%r24,%r12), %rbx
	rorq	$19, %r27, %r24
	rolq	$30, %r11, %r14
	xorq	%rbp, %r24
	shrq	$6, %r27, %rbp
	xorq	%rbp, %r24
	xorq	%rdx, %rax, %rbp
	addq	%rbx, %r24
	andq	%rcx, %rbp
	addq	%r24, 56(%r25), %rbx
	addq	%rsi, %rbx
	xorq	%rax, %rbp
	rorq	$18, %rcx, %rsi
	addq	%rbx, %rbp
	rorq	$14, %rcx, %rbx
	xorq	%rsi, %rbx
	rolq	$23, %rcx, %rsi
	xorq	%rsi, %rbx
	xorq	%r11, %r10, %rsi
	addq	%rbx, %rbp
	rorq	$28, %r11, %rbx
	andq	%rsi, %rdi
	xorq	%rbx, %r14
	rolq	$25, %r11, %rbx
	xorq	%r10, %rdi
	addq	%rbp, %r8
	xorq	%r14, %rbx
	addq	%rdi, %rbx
	rorq	$1, %r22, %rdi
	addq	%rbp, %rbx
	rorq	$8, %r22, %rbp
	xorq	%rbp, %rdi
	shrq	$7, %r22, %rbp
	xorq	%rbp, %rdi
	rolq	$3, %r26, %rbp
	addq	%rdi, %r23
	rolq	$30, %rbx, %r14
	leaq	(%r23,%r31), %rdi
	rorq	$19, %r26, %r23
	xorq	%rbp, %r23
	shrq	$6, %r26, %rbp
	xorq	%rbp, %r23
	xorq	%rcx, %rdx, %rbp
	addq	%rdi, %r23
	andq	%r8, %rbp
	addq	%r23, 64(%r25), %rdi
	addq	%rdi, %rax
	xorq	%rdx, %rbp
	rorq	$18, %r8, %rdi
	addq	%rax, %rbp
	rorq	$14, %r8, %rax
	xorq	%rdi, %rax
	rolq	$23, %r8, %rdi
	xorq	%rdi, %rax
	xorq	%rbx, %r11, %rdi
	addq	%rax, %rbp
	rorq	$28, %rbx, %rax
	andq	%rdi, %rsi
	xorq	%rax, %r14
	rolq	$25, %rbx, %rax
	xorq	%r11, %rsi
	addq	%rbp, %r9
	xorq	%r14, %rax
	addq	%rsi, %rax
	addq	%rbp, %rax
	rorq	$8, %r21, %rbp
	rorq	$1, %r21, %rsi
	rolq	$30, %rax, %r14
	xorq	%rbp, %rsi
	shrq	$7, %r21, %rbp
	xorq	%rbp, %rsi
	rolq	$3, %r24, %rbp
	addq	%rsi, %r22
	leaq	(%r22,%r30), %rsi
	rorq	$19, %r24, %r22
	xorq	%rbp, %r22
	shrq	$6, %r24, %rbp
	xorq	%rbp, %r22
	xorq	%r8, %rcx, %rbp
	addq	%rsi, %r22
	andq	%r9, %rbp
	addq	%r22, 72(%r25), %rsi
	addq	%rsi, %rdx
	xorq	%rcx, %rbp
	rorq	$18, %r9, %rsi
	addq	%rdx, %rbp
	rorq	$14, %r9, %rdx
	xorq	%rsi, %rdx
	rolq	$23, %r9, %rsi
	xorq	%rsi, %rdx
	xorq	%rax, %rbx, %rsi
	addq	%rdx, %rbp
	rorq	$28, %rax, %rdx
	xorq	%rdx, %r14
	rolq	$25, %rax, %rdx
	addq	%rbp, %r10
	xorq	%r14, %rdx
	andq	%rsi, %rdi
	xorq	%rbx, %rdi
	addq	%rdi, %rdx
	rorq	$1, %r20, %rdi
	addq	%rbp, %rdx
	rorq	$8, %r20, %rbp
	xorq	%rbp, %rdi
	shrq	$7, %r20, %rbp
	xorq	%rbp, %rdi
	rolq	$3, %r23, %rbp
	addq	%rdi, %r21
	rolq	$30, %rdx, %r14
	leaq	(%r21,%r29), %rdi
	rorq	$19, %r23, %r21
	xorq	%rbp, %r21
	shrq	$6, %r23, %rbp
	xorq	%rbp, %r21
	addq	%rdi, %r21
	addq	%r21, 80(%r25), %rbp
	rorq	$18, %r10, %rdi
	addq	%rcx, %rbp
	xorq	%r9, %r8, %rcx
	andq	%r10, %rcx
	xorq	%r8, %rcx
	addq	%rcx, %rbp
	rorq	$14, %r10, %rcx
	xorq	%rdi, %rcx
	rolq	$23, %r10, %rdi
	xorq	%rdi, %rcx
	xorq	%rdx, %rax, %rdi
	addq	%rcx, %rbp
	rorq	$28, %rdx, %rcx
	addq	%rbp, %r11
	xorq	%rcx, %r14
	rolq	$25, %rdx, %rcx
	andq	%rdi, %rsi
	xorq	%rax, %rsi
	xorq	%r14, %rcx
	addq	%rsi, %rcx
	rorq	$1, %r19, %rsi
	addq	%rbp, %rcx
	rorq	$8, %r19, %rbp
	xorq	%rbp, %rsi
	shrq	$7, %r19, %rbp
	xorq	%rbp, %rsi
	rolq	$3, %r22, %rbp
	addq	%rsi, %r20
	leaq	(%r20,%r28), %rsi
	rorq	$19, %r22, %r20
	xorq	%rbp, %r20
	shrq	$6, %r22, %rbp
	xorq	%rbp, %r20
	addq	%rsi, %r20
	addq	%r20, 88(%r25), %rbp
	rorq	$18, %r11, %rsi
	addq	%r8, %rbp
	xorq	%r10, %r9, %r8
	andq	%r11, %r8
	xorq	%r9, %r8
	addq	%r8, %rbp
	rorq	$14, %r11, %r8
	xorq	%rsi, %r8
	rolq	$23, %r11, %rsi
	xorq	%rsi, %r8
	xorq	%rcx, %rdx, %rsi
	addq	%r8, %rbp
	addq	%rbp, %rbx
	rorq	$28, %rcx, %r8
	andq	%rsi, %rdi
	rolq	$30, %rcx, %r14
	xorq	%rdx, %rdi
	xorq	%r8, %r14
	rolq	$25, %rcx, %r8
	xorq	%r14, %r8
	addq	%rdi, %r8
	rorq	$1, %r18, %rdi
	addq	%rbp, %r8
	rorq	$8, %r18, %rbp
	xorq	%rbp, %rdi
	shrq	$7, %r18, %rbp
	xorq	%rbp, %rdi
	rolq	$3, %r21, %rbp
	addq	%rdi, %r19
	leaq	(%r19,%r27), %rdi
	rorq	$19, %r21, %r19
	xorq	%rbp, %r19
	shrq	$6, %r21, %rbp
	xorq	%rbp, %r19
	addq	%rdi, %r19
	addq	%r19, 96(%r25), %rbp
	rorq	$18, %rbx, %rdi
	addq	%r9, %rbp
	xorq	%r11, %r10, %r9
	andq	%rbx, %r9
	xorq	%r10, %r9
	addq	%r9, %rbp
	rorq	$14, %rbx, %r9
	xorq	%rdi, %r9
	rolq	$23, %rbx, %rdi
	xorq	%rdi, %r9
	xorq	%r8, %rcx, %rdi
	rolq	$30, %r8, %r14
	addq	%r9, %rbp
	rorq	$28, %r8, %r9
	andq	%rdi, %rsi
	xorq	%r9, %r14
	rolq	$25, %r8, %r9
	xorq	%rcx, %rsi
	addq	%rbp, %rax
	xorq	%r14, %r9
	addq	%rsi, %r9
	rorq	$1, %r17, %rsi
	addq	%rbp, %r9
	rorq	$8, %r17, %rbp
	xorq	%rbp, %rsi
	shrq	$7, %r17, %rbp
	xorq	%rbp, %rsi
	rolq	$3, %r20, %rbp
	addq	%rsi, %r18
	leaq	(%r18,%r26), %rsi
	rorq	$19, %r20, %r18
	xorq	%rbp, %r18
	shrq	$6, %r20, %rbp
	xorq	%rbp, %r18
	addq	%rsi, %r18
	addq	%r18, 104(%r25), %rbp
	addq	%r10, %rbp
	xorq	%rbx, %r11, %r10
	andq	%rax, %r10
	xorq	%r11, %r10
	addq	%r10, %rbp
	rorq	$14, %rax, %r10
	rorq	$18, %rax, %rsi
	rolq	$30, %r9, %r14
	xorq	%rsi, %r10
	rolq	$23, %rax, %rsi
	xorq	%rsi, %r10
	xorq	%r9, %r8, %rsi
	addq	%r10, %rbp
	rorq	$28, %r9, %r10
	andq	%rsi, %rdi
	xorq	%r10, %r14
	rolq	$25, %r9, %r10
	xorq	%r8, %rdi
	addq	%rbp, %rdx
	xorq	%r14, %r10
	addq	%rdi, %r10
	rorq	$1, %r16, %rdi
	addq	%rbp, %r10
	rorq	$8, %r16, %rbp
	xorq	%rbp, %rdi
	shrq	$7, %r16, %rbp
	xorq	%rbp, %rdi
	rolq	$3, %r19, %rbp
	addq	%rdi, %r17
	leaq	(%r17,%r24), %rdi
	rorq	$19, %r19, %r17
	xorq	%rbp, %r17
	shrq	$6, %r19, %rbp
	xorq	%rbp, %r17
	addq	%rdi, %r17
	addq	%r17, 112(%r25), %rbp
	addq	%r11, %rbp
	xorq	%rax, %rbx, %r11
	andq	%rdx, %r11
	xorq	%rbx, %r11
	rorq	$18, %rdx, %rdi
	addq	%r11, %rbp
	rorq	$14, %rdx, %r11
	xorq	%rdi, %r11
	rolq	$23, %rdx, %rdi
	xorq	%rdi, %r11
	rolq	$30, %r10, %r14
	xorq	%r10, %r9, %rdi
	addq	%r11, %rbp
	rorq	$28, %r10, %r11
	andq	%rdi, %rsi
	xorq	%r11, %r14
	rolq	$25, %r10, %r11
	xorq	%r9, %rsi
	addq	%rbp, %rcx
	xorq	%r14, %r11
	addq	%rsi, %r11
	rorq	$1, %r12, %rsi
	addq	%rbp, %r11
	rorq	$8, %r12, %rbp
	xorq	%rbp, %rsi
	shrq	$7, %r12, %rbp
	xorq	%rbp, %rsi
	rolq	$3, %r18, %rbp
	addq	%rsi, %r16
	leaq	(%r16,%r23), %rsi
	rorq	$19, %r18, %r16
	xorq	%rbp, %r16
	shrq	$6, %r18, %rbp
	xorq	%rbp, %r16
	addq	%rsi, %r16
	xorq	%rdx, %rax, %rsi
	andq	%rcx, %rsi
	addq	%r16, 120(%r25), %rbp
	rolq	$30, %r11, %r14
	subq	$-128, %r25
	xorq	%rax, %rsi
	addq	%rbp, %rbx
	rorq	$18, %rcx, %rbp
	addq	%rsi, %rbx
	rorq	$14, %rcx, %rsi
	xorq	%rbp, %rsi
	rolq	$23, %rcx, %rbp
	xorq	%rbp, %rsi
	xorq	%r11, %r10, %rbp
	addq	%rsi, %rbx
	rorq	$28, %r11, %rsi
	andq	%rbp, %rdi
	xorq	%rsi, %r14
	rolq	$25, %r11, %rsi
	xorq	%r10, %rdi
	addq	%rbx, %r8
	xorq	%r14, %rsi
	addq	%rdi, %rsi
	addq	%rbx, %rsi
	leaq	512+_ZN7fast_io7details6sha51210k512scalarE(%rip), %rbx
	cmpq	%rbx, %r25
	jne	.L3
	movq	40(%rsp), %r14
	addq	%rax, 32(%rsp)
	addq	%r8, %r15
	addq	%rcx, %r13
	subq	$-128, 136(%rsp)
	movq	136(%rsp), %rax
	addq	%r11, (%rsp)
	addq	%rsi, %r14
	addq	%r10, 8(%rsp)
	addq	%r9, 16(%rsp)
	addq	%rdx, 24(%rsp)
	cmpq	%rax, 144(%rsp)
	jne	.L4
	movq	128(%rsp), %r17
	movq	(%rsp), %rax
	movq	%rax, 8(%r17)
	movq	8(%rsp), %rax
	movq	%r14, (%r17)
	movq	%rax, 16(%r17)
	movq	16(%rsp), %rax
	movq	%r15, 32(%r17)
	movq	%rax, 24(%r17)
	movq	24(%rsp), %rax
	movq	%r13, 40(%r17)
	movq	%rax, 48(%r17)
	movq	32(%rsp), %rax
	movq	%rax, 56(%r17)
.L1:
	addq	$56, %rsp
	popp	%rbx
	pop2p	%rdi, %rsi
	pop2p	%r12, %rbp
	pop2p	%r14, %r13
	popp	%r15
	ret
	.seh_endproc
	.globl	_ZN7fast_io7details6sha51210k512scalarE
	.section	.rdata$_ZN7fast_io7details6sha51210k512scalarE,"dr"
	.linkonce same_size
	.align 32
_ZN7fast_io7details6sha51210k512scalarE:
	.quad	-1973867731355612462
	.quad	-1171420211273849373
	.quad	1135362057144423861
	.quad	2597628984639134821
	.quad	3308224258029322869
	.quad	5365058923640841347
	.quad	6679025012923562964
	.quad	8573033837759648693
	.quad	-7476448914759557205
	.quad	-6327057829258317296
	.quad	-5763719355590565569
	.quad	-4658551843659510044
	.quad	-4116276920077217854
	.quad	-3051310485924567259
	.quad	489312712824947311
	.quad	1452737877330783856
	.quad	2861767655752347644
	.quad	3322285676063803686
	.quad	5560940570517711597
	.quad	5996557281743188959
	.quad	7280758554555802590
	.quad	8532644243296465576
	.quad	-9096487096722542874
	.quad	-7894198246740708037
	.quad	-6719396339535248540
	.quad	-6333637450476146687
	.quad	-4446306890439682159
	.quad	-4076793802049405392
	.quad	-3345356375505022440
	.quad	-2983346525034927856
	.quad	-860691631967231958
	.quad	1182934255886127544
	.quad	1847814050463011016
	.quad	2177327727835720531
	.quad	2830643537854262169
	.quad	3796741975233480872
	.quad	4115178125766777443
	.quad	5681478168544905931
	.quad	6601373596472566643
	.quad	7507060721942968483
	.quad	8399075790359081724
	.quad	8693463985226723168
	.quad	-8878714635349349518
	.quad	-8302665154208450068
	.quad	-8016688836872298968
	.quad	-6606660893046293015
	.quad	-4685533653050689259
	.quad	-4147400797238176981
	.quad	-3880063495543823972
	.quad	-3348786107499101689
	.quad	-1523767162380948706
	.quad	-757361751448694408
	.quad	500013540394364858
	.quad	748580250866718886
	.quad	1242879168328830382
	.quad	1977374033974150939
	.quad	2944078676154940804
	.quad	3659926193048069267
	.quad	4368137639120453308
	.quad	4836135668995329356
	.quad	5532061633213252278
	.quad	6448918945643986474
	.quad	6902733635092675308
	.quad	7801388544844847127
	.ident	"GCC: (GNU) 15.0.0 20240606 (experimental)"
