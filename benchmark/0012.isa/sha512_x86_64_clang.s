	.text
	.file	"sha512.cc"
	.globl	_Z14sha512_invokerPmPKSt4byteS2_ # -- Begin function _Z14sha512_invokerPmPKSt4byteS2_
	.p2align	4, 0x90
	.type	_Z14sha512_invokerPmPKSt4byteS2_,@function
_Z14sha512_invokerPmPKSt4byteS2_:       # @_Z14sha512_invokerPmPKSt4byteS2_
# %bb.0:
	pushq	%rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$232, %rsp
	movq	%rdx, 104(%rsp)                 # 8-byte Spill
	cmpq	%rdx, %rsi
	je	.LBB0_6
# %bb.1:
	movq	56(%rdi), %rbp
	movq	48(%rdi), %r13
	movq	40(%rdi), %r10
	movq	32(%rdi), %rbx
	movq	24(%rdi), %r11
	movq	16(%rdi), %rcx
	movq	(%rdi), %r15
	movq	%rdi, 96(%rsp)                  # 8-byte Spill
	movq	8(%rdi), %rdx
	.p2align	4, 0x90
.LBB0_2:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_3 Depth 2
	movq	%rcx, 88(%rsp)                  # 8-byte Spill
	movq	%rdx, %r9
	movq	(%rsi), %rax
	bswapq	%rax
	movq	%rax, %rdi
	movq	%rax, -128(%rsp)                # 8-byte Spill
	xorq	%rcx, %r9
	movq	%rbx, %rax
	rolq	$50, %rax
	movq	%rbx, %r14
	movq	8(%rsi), %r8
	movq	%rbx, %rcx
	rolq	$46, %rcx
	xorq	%rax, %rcx
	movq	%rdx, %rbx
	movq	%r14, %rdx
	rolq	$23, %rdx
	xorq	%rcx, %rdx
	movq	%r10, %rax
	xorq	%r13, %rax
	andq	%r14, %rax
	xorq	%r13, %rax
	movq	%rbp, 152(%rsp)                 # 8-byte Spill
	addq	%rbp, %rax
	addq	%rdx, %rax
	addq	%rdi, %rax
	movabsq	$4794697086780616226, %rcx      # imm = 0x428A2F98D728AE22
	addq	%rcx, %rax
	movq	%r15, %rcx
	rolq	$36, %rcx
	movq	%r15, %rdx
	rolq	$30, %rdx
	xorq	%rcx, %rdx
	movq	%r15, %rcx
	rolq	$25, %rcx
	xorq	%rdx, %rcx
	movq	%r15, %r12
	movq	%rbx, 80(%rsp)                  # 8-byte Spill
	xorq	%rbx, %r15
	andq	%r15, %r9
	xorq	%rbx, %r9
	addq	%rcx, %r9
	addq	%rax, %r9
	bswapq	%r8
	movq	%r8, -40(%rsp)                  # 8-byte Spill
	movq	%r11, 120(%rsp)                 # 8-byte Spill
	addq	%r11, %rax
	movq	%rax, %rcx
	rolq	$50, %rcx
	movq	%rax, %rdx
	rolq	$46, %rdx
	xorq	%rcx, %rdx
	movq	%rax, %rcx
	rolq	$23, %rcx
	xorq	%rdx, %rcx
	movq	%r14, %rdx
	xorq	%r10, %rdx
	andq	%rax, %rdx
	xorq	%r10, %rdx
	movq	%rsi, %rbx
	movq	%r13, 144(%rsp)                 # 8-byte Spill
	addq	%r13, %rdx
	addq	%r8, %rdx
	addq	%rcx, %rdx
	movabsq	$8158064640168781261, %rcx      # imm = 0x7137449123EF65CD
	addq	%rcx, %rdx
	movq	%r9, %rcx
	rolq	$36, %rcx
	movq	%r9, %rsi
	rolq	$30, %rsi
	xorq	%rcx, %rsi
	movq	%r9, %rdi
	rolq	$25, %rdi
	xorq	%rsi, %rdi
	movq	%r9, %r11
	xorq	%r12, %r11
	andq	%r11, %r15
	xorq	%r12, %r15
	addq	%rdi, %r15
	addq	%rdx, %r15
	movq	16(%rbx), %rbp
	movq	%rbx, %r13
	bswapq	%rbp
	movq	%rbp, 56(%rsp)                  # 8-byte Spill
	addq	88(%rsp), %rdx                  # 8-byte Folded Reload
	movq	%rdx, %rsi
	rolq	$50, %rsi
	movq	%rdx, %rdi
	rolq	$46, %rdi
	xorq	%rsi, %rdi
	movq	%rdx, %rsi
	rolq	$23, %rsi
	xorq	%rdi, %rsi
	movq	%rax, %rcx
	xorq	%r14, %rcx
	andq	%rdx, %rcx
	xorq	%r14, %rcx
	movq	%r10, 136(%rsp)                 # 8-byte Spill
	addq	%r10, %rbp
	addq	%rbp, %rcx
	addq	%rsi, %rcx
	movabsq	$-5349999486874862801, %rsi     # imm = 0xB5C0FBCFEC4D3B2F
	addq	%rsi, %rcx
	movq	%r15, %rsi
	rolq	$36, %rsi
	movq	%r15, %rbx
	rolq	$30, %rbx
	xorq	%rsi, %rbx
	movq	%r15, %rsi
	rolq	$25, %rsi
	xorq	%rbx, %rsi
	movq	%r15, %r10
	xorq	%r9, %r10
	andq	%r10, %r11
	xorq	%r9, %r11
	addq	%rsi, %r11
	addq	%rcx, %r11
	movq	24(%r13), %rbx
	bswapq	%rbx
	addq	80(%rsp), %rcx                  # 8-byte Folded Reload
	movq	%rcx, %rsi
	rolq	$50, %rsi
	movq	%rcx, %rdi
	rolq	$46, %rdi
	xorq	%rsi, %rdi
	movq	%rcx, %rsi
	rolq	$23, %rsi
	xorq	%rdi, %rsi
	movq	%rdx, %rbp
	xorq	%rax, %rbp
	andq	%rcx, %rbp
	xorq	%rax, %rbp
	movq	%r14, 128(%rsp)                 # 8-byte Spill
	movq	%rbx, -8(%rsp)                  # 8-byte Spill
	addq	%r14, %rbp
	addq	%rbx, %rbp
	addq	%rsi, %rbp
	movabsq	$-1606136188198331460, %rsi     # imm = 0xE9B5DBA58189DBBC
	addq	%rsi, %rbp
	movq	%r11, %rsi
	rolq	$36, %rsi
	movq	%r11, %rdi
	rolq	$30, %rdi
	xorq	%rsi, %rdi
	movq	%r11, %rsi
	rolq	$25, %rsi
	xorq	%rdi, %rsi
	movq	%r11, %r8
	xorq	%r15, %r8
	andq	%r8, %r10
	xorq	%r15, %r10
	addq	%rsi, %r10
	addq	%rbp, %r10
	movq	32(%r13), %rbx
	bswapq	%rbx
	movq	%r12, 112(%rsp)                 # 8-byte Spill
	addq	%r12, %rbp
	movq	%rbp, %rsi
	rolq	$50, %rsi
	movq	%rbp, %rdi
	rolq	$46, %rdi
	xorq	%rsi, %rdi
	movq	%rbp, %rsi
	rolq	$23, %rsi
	xorq	%rdi, %rsi
	movq	%rcx, %rdi
	xorq	%rdx, %rdi
	andq	%rbp, %rdi
	xorq	%rdx, %rdi
	movq	%rbx, -72(%rsp)                 # 8-byte Spill
	addq	%rbx, %rax
	addq	%rdi, %rax
	addq	%rsi, %rax
	movq	%r10, %rsi
	rolq	$36, %rsi
	movq	%r10, %rdi
	rolq	$30, %rdi
	xorq	%rsi, %rdi
	movq	%r10, %rsi
	rolq	$25, %rsi
	xorq	%rdi, %rsi
	movq	%r10, %rbx
	xorq	%r11, %rbx
	andq	%rbx, %r8
	xorq	%r11, %r8
	addq	%rsi, %r8
	movabsq	$4131703408338449720, %rsi      # imm = 0x3956C25BF348B538
	addq	%rsi, %rax
	movq	40(%r13), %rdi
	bswapq	%rdi
	addq	%rax, %r9
	movq	%r9, %rsi
	rolq	$50, %rsi
	addq	%rax, %r8
	movq	%r9, %rax
	rolq	$46, %rax
	xorq	%rsi, %rax
	movq	%r9, %rsi
	rolq	$23, %rsi
	xorq	%rax, %rsi
	movq	%rbp, %rax
	xorq	%rcx, %rax
	andq	%r9, %rax
	xorq	%rcx, %rax
	movq	%rdi, 24(%rsp)                  # 8-byte Spill
	addq	%rdi, %rdx
	addq	%rax, %rdx
	addq	%rsi, %rdx
	movq	%r8, %rax
	rolq	$36, %rax
	movq	%r8, %rsi
	rolq	$30, %rsi
	xorq	%rax, %rsi
	movq	%r8, %rax
	rolq	$25, %rax
	xorq	%rsi, %rax
	movq	%r8, %r14
	xorq	%r10, %r14
	andq	%r14, %rbx
	xorq	%r10, %rbx
	addq	%rax, %rbx
	movabsq	$6480981068601479193, %rax      # imm = 0x59F111F1B605D019
	addq	%rax, %rdx
	movq	48(%r13), %rsi
	bswapq	%rsi
	addq	%rdx, %r15
	movq	%r15, %rax
	rolq	$50, %rax
	addq	%rdx, %rbx
	movq	%r15, %rdx
	rolq	$46, %rdx
	xorq	%rax, %rdx
	movq	%r15, %rax
	rolq	$23, %rax
	xorq	%rdx, %rax
	movq	%r9, %rdx
	xorq	%rbp, %rdx
	andq	%r15, %rdx
	xorq	%rbp, %rdx
	movq	%rsi, 8(%rsp)                   # 8-byte Spill
	addq	%rsi, %rcx
	addq	%rdx, %rcx
	addq	%rax, %rcx
	movq	%rbx, %rax
	rolq	$36, %rax
	movq	%rbx, %rdx
	rolq	$30, %rdx
	xorq	%rax, %rdx
	movq	%rbx, %rax
	rolq	$25, %rax
	xorq	%rdx, %rax
	movq	%rbx, %rsi
	xorq	%r8, %rsi
	andq	%rsi, %r14
	xorq	%r8, %r14
	addq	%rax, %r14
	movabsq	$-7908458776815382629, %rax     # imm = 0x923F82A4AF194F9B
	addq	%rax, %rcx
	movq	56(%r13), %rdx
	bswapq	%rdx
	movq	%rdx, 40(%rsp)                  # 8-byte Spill
	addq	%rcx, %r11
	movq	%r11, %rax
	rolq	$50, %rax
	addq	%rcx, %r14
	movq	%r11, %rcx
	rolq	$46, %rcx
	xorq	%rax, %rcx
	movq	%r11, %rax
	rolq	$23, %rax
	xorq	%rcx, %rax
	movq	%r15, %rcx
	xorq	%r9, %rcx
	andq	%r11, %rcx
	xorq	%r9, %rcx
	addq	%rdx, %rbp
	addq	%rcx, %rbp
	addq	%rax, %rbp
	movq	%r14, %rax
	rolq	$36, %rax
	movq	%r14, %rcx
	rolq	$30, %rcx
	xorq	%rax, %rcx
	movq	%r14, %rax
	rolq	$25, %rax
	xorq	%rcx, %rax
	movq	%r14, %rdi
	xorq	%rbx, %rdi
	andq	%rdi, %rsi
	xorq	%rbx, %rsi
	addq	%rax, %rsi
	movabsq	$-6116909921290321640, %rax     # imm = 0xAB1C5ED5DA6D8118
	addq	%rax, %rbp
	movq	64(%r13), %rdx
	bswapq	%rdx
	movq	%rdx, 32(%rsp)                  # 8-byte Spill
	addq	%rbp, %r10
	movq	%r10, %rax
	rolq	$50, %rax
	addq	%rbp, %rsi
	movq	%r10, %rcx
	rolq	$46, %rcx
	xorq	%rax, %rcx
	movq	%r10, %rax
	rolq	$23, %rax
	xorq	%rcx, %rax
	movq	%r11, %rcx
	xorq	%r15, %rcx
	andq	%r10, %rcx
	xorq	%r15, %rcx
	addq	%rdx, %r9
	addq	%rcx, %r9
	addq	%rax, %r9
	movq	%rsi, %rax
	rolq	$36, %rax
	movq	%rsi, %rcx
	rolq	$30, %rcx
	xorq	%rax, %rcx
	movq	%rsi, %rax
	rolq	$25, %rax
	xorq	%rcx, %rax
	movq	%rsi, %rbp
	xorq	%r14, %rbp
	andq	%rbp, %rdi
	xorq	%r14, %rdi
	addq	%rax, %rdi
	movabsq	$-2880145864133508542, %rax     # imm = 0xD807AA98A3030242
	addq	%rax, %r9
	movq	72(%r13), %rax
	bswapq	%rax
	movq	%rax, %rdx
	movq	%rax, -112(%rsp)                # 8-byte Spill
	movq	%r8, %r12
	addq	%r9, %r12
	movq	%r12, %rax
	rolq	$50, %rax
	addq	%r9, %rdi
	movq	%r12, %rcx
	rolq	$46, %rcx
	xorq	%rax, %rcx
	movq	%r12, %rax
	rolq	$23, %rax
	xorq	%rcx, %rax
	movq	%r10, %rcx
	xorq	%r11, %rcx
	andq	%r12, %rcx
	xorq	%r11, %rcx
	addq	%rdx, %r15
	addq	%rcx, %r15
	addq	%rax, %r15
	movq	%rdi, %r9
	movq	%rdi, -120(%rsp)                # 8-byte Spill
	movq	%rdi, %rax
	rolq	$36, %rax
	movq	%rdi, %rcx
	rolq	$30, %rcx
	xorq	%rax, %rcx
	rolq	$25, %rdi
	xorq	%rcx, %rdi
	xorq	%rsi, %r9
	andq	%r9, %rbp
	xorq	%rsi, %rbp
	addq	%rdi, %rbp
	movabsq	$1334009975649890238, %rax      # imm = 0x12835B0145706FBE
	addq	%rax, %r15
	movq	80(%r13), %rax
	movq	%r13, %rdx
	bswapq	%rax
	movq	%rax, -104(%rsp)                # 8-byte Spill
	addq	%r15, %rbx
	movq	%rbx, %rcx
	rolq	$50, %rcx
	addq	%r15, %rbp
	movq	-40(%rsp), %r8                  # 8-byte Reload
	movq	%rbx, %rdi
	rolq	$46, %rdi
	xorq	%rcx, %rdi
	movq	%rbx, %rcx
	rolq	$23, %rcx
	xorq	%rdi, %rcx
	movq	%r12, %rdi
	xorq	%r10, %rdi
	andq	%rbx, %rdi
	xorq	%r10, %rdi
	addq	%rax, %r11
	addq	%rdi, %r11
	addq	%rcx, %r11
	movq	%rbp, %rcx
	rolq	$36, %rcx
	movq	%rbp, %rdi
	rolq	$30, %rdi
	xorq	%rcx, %rdi
	movq	%rbp, %rcx
	rolq	$25, %rcx
	xorq	%rdi, %rcx
	movq	%rbp, %r13
	movq	-120(%rsp), %rax                # 8-byte Reload
	xorq	%rax, %r13
	andq	%r13, %r9
	xorq	%rax, %r9
	addq	%rcx, %r9
	movabsq	$2608012711638119052, %rax      # imm = 0x243185BE4EE4B28C
	addq	%rax, %r11
	movq	88(%rdx), %rax
	bswapq	%rax
	movq	%rax, -80(%rsp)                 # 8-byte Spill
	addq	%r11, %r14
	movq	%r14, %rcx
	rolq	$50, %rcx
	addq	%r11, %r9
	movq	%r14, %rdi
	rolq	$46, %rdi
	xorq	%rcx, %rdi
	movq	%r14, %rcx
	rolq	$23, %rcx
	xorq	%rdi, %rcx
	movq	%rbx, %rdi
	movq	%rbx, %r11
	xorq	%r12, %rdi
	andq	%r14, %rdi
	xorq	%r12, %rdi
	addq	%rax, %r10
	addq	%rdi, %r10
	addq	%rcx, %r10
	movq	%r9, %rcx
	rolq	$36, %rcx
	movq	%r9, %rdi
	rolq	$30, %rdi
	xorq	%rcx, %rdi
	movq	%r9, %rbx
	rolq	$25, %rbx
	xorq	%rdi, %rbx
	movq	%r9, %r15
	movq	%rbp, -88(%rsp)                 # 8-byte Spill
	xorq	%rbp, %r15
	andq	%r15, %r13
	xorq	%rbp, %r13
	addq	%rbx, %r13
	movabsq	$6128411473006802146, %rax      # imm = 0x550C7DC3D5FFB4E2
	addq	%rax, %r10
	movq	96(%rdx), %rax
	movq	%rdx, %rcx
	movq	%rdx, 48(%rsp)                  # 8-byte Spill
	bswapq	%rax
	movq	%rax, -56(%rsp)                 # 8-byte Spill
	addq	%r10, %rsi
	movq	%rsi, %rdi
	rolq	$50, %rdi
	addq	%r10, %r13
	movq	%rsi, %rbp
	rolq	$46, %rbp
	xorq	%rdi, %rbp
	movq	%rsi, %rdi
	rolq	$23, %rdi
	xorq	%rbp, %rdi
	movq	%r14, %rbp
	movq	%r11, %r10
	xorq	%r11, %rbp
	andq	%rsi, %rbp
	xorq	%r11, %rbp
	addq	%rax, %r12
	addq	%rbp, %r12
	addq	%rdi, %r12
	movq	%r13, %rdi
	rolq	$36, %rdi
	movq	%r13, %rbp
	rolq	$30, %rbp
	xorq	%rdi, %rbp
	movq	%r13, %rdi
	rolq	$25, %rdi
	xorq	%rbp, %rdi
	movq	%r13, %rdx
	xorq	%r9, %rdx
	andq	%rdx, %r15
	xorq	%r9, %r15
	addq	%rdi, %r15
	movabsq	$8268148722764581231, %rax      # imm = 0x72BE5D74F27B896F
	addq	%rax, %r12
	movq	104(%rcx), %rax
	bswapq	%rax
	movq	-120(%rsp), %rbx                # 8-byte Reload
	addq	%r12, %rbx
	movq	%rbx, %rdi
	rolq	$50, %rdi
	addq	%r12, %r15
	movq	%rbx, %rbp
	rolq	$46, %rbp
	xorq	%rdi, %rbp
	movq	%rbx, %rdi
	rolq	$23, %rdi
	xorq	%rbp, %rdi
	movq	%rsi, %rbp
	xorq	%r14, %rbp
	andq	%rbx, %rbp
	movq	%rbx, -120(%rsp)                # 8-byte Spill
	xorq	%r14, %rbp
	movq	%rax, -96(%rsp)                 # 8-byte Spill
	movq	%r11, %rcx
	addq	%rax, %rcx
	addq	%rbp, %rcx
	addq	%rdi, %rcx
	movq	%rcx, %r10
	movq	%r15, %rdi
	rolq	$36, %rdi
	movq	%r15, %rbp
	rolq	$30, %rbp
	xorq	%rdi, %rbp
	movq	%r15, %rdi
	rolq	$25, %rdi
	xorq	%rbp, %rdi
	movq	%r15, %r11
	xorq	%r13, %r11
	andq	%r11, %rdx
	xorq	%r13, %rdx
	addq	%rdi, %rdx
	movabsq	$-9160688886553864527, %rax     # imm = 0x80DEB1FE3B1696B1
	addq	%rax, %r10
	movq	48(%rsp), %rax                  # 8-byte Reload
	movq	112(%rax), %rcx
	bswapq	%rcx
	movq	-88(%rsp), %r12                 # 8-byte Reload
	addq	%r10, %r12
	movq	%r12, %rdi
	rolq	$50, %rdi
	addq	%r10, %rdx
	movq	%r12, %rbp
	rolq	$46, %rbp
	xorq	%rdi, %rbp
	movq	%r12, %rdi
	rolq	$23, %rdi
	xorq	%rbp, %rdi
	xorq	%rsi, %rbx
	andq	%r12, %rbx
	xorq	%rsi, %rbx
	addq	%rcx, %r14
	addq	%rbx, %r14
	addq	%rdi, %r14
	movq	%rdx, %rdi
	rolq	$36, %rdi
	movq	%rdx, %rbp
	rolq	$30, %rbp
	xorq	%rdi, %rbp
	movq	%rdx, %rdi
	rolq	$25, %rdi
	xorq	%rbp, %rdi
	movq	%rdx, %r10
	xorq	%r15, %r10
	andq	%r10, %r11
	movq	%r15, -24(%rsp)                 # 8-byte Spill
	xorq	%r15, %r11
	addq	%rdi, %r11
	movabsq	$-7215885187991268811, %rax     # imm = 0x9BDC06A725C71235
	addq	%rax, %r14
	movq	48(%rsp), %rax                  # 8-byte Reload
	movq	120(%rax), %rbx
	bswapq	%rbx
	addq	%r14, %r9
	movq	%r9, %rdi
	rolq	$50, %rdi
	addq	%r14, %r11
	movq	%r9, %rbp
	rolq	$46, %rbp
	xorq	%rdi, %rbp
	movq	%r9, %rdi
	rolq	$23, %rdi
	xorq	%rbp, %rdi
	movq	%r12, -88(%rsp)                 # 8-byte Spill
	movq	-120(%rsp), %rax                # 8-byte Reload
	xorq	%rax, %r12
	movq	%r9, %r14
	andq	%r9, %r12
	xorq	%rax, %r12
	addq	%rbx, %rsi
	addq	%r12, %rsi
	addq	%rdi, %rsi
	movq	%r11, %rdi
	rolq	$36, %rdi
	movq	%r11, %rbp
	rolq	$30, %rbp
	xorq	%rdi, %rbp
	movq	%r11, %rdi
	rolq	$25, %rdi
	xorq	%rbp, %rdi
	movq	%r11, -64(%rsp)                 # 8-byte Spill
	movq	%r11, %rax
	xorq	%rdx, %rax
	movq	%rax, -32(%rsp)                 # 8-byte Spill
	andq	%rax, %r10
	movq	%rdx, -16(%rsp)                 # 8-byte Spill
	xorq	%rdx, %r10
	movq	-128(%rsp), %r15                # 8-byte Reload
	addq	%rdi, %r10
	movabsq	$-4495734319001033068, %rax     # imm = 0xC19BF174CF692694
	addq	%rax, %rsi
	addq	%rsi, %r13
	movq	%r13, %r12
	addq	%rsi, %r10
	movq	%r10, %r9
	leaq	_ZN7fast_io7details6sha51210k512scalarE(%rip), %rdx
	movq	-112(%rsp), %rax                # 8-byte Reload
	.p2align	4, 0x90
.LBB0_3:                                #   Parent Loop BB0_2 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movq	%rdx, -128(%rsp)                # 8-byte Spill
	movq	%rax, -112(%rsp)                # 8-byte Spill
	movq	%r8, %rsi
	rolq	$56, %rsi
	movq	%rbx, %rdi
	rolq	$45, %rdi
	movq	%rbx, %rbp
	rolq	$3, %rbp
	movq	%rbx, %r13
	movq	%rbx, 64(%rsp)                  # 8-byte Spill
	movq	%r8, %rbx
	rorq	%rbx
	xorq	%rbx, %rsi
	xorq	%rdi, %rbp
	movq	%rcx, -48(%rsp)                 # 8-byte Spill
	shrq	$6, %r13
	xorq	%rbp, %r13
	movq	%rcx, %rdi
	rolq	$45, %rdi
	movq	%rcx, %rbp
	rolq	$3, %rbp
	xorq	%rdi, %rbp
	addq	%r8, %r13
	shrq	$7, %r8
	xorq	%rsi, %r8
	shrq	$6, %rcx
	xorq	%rbp, %rcx
	addq	%r15, %rcx
	movq	%r12, %r11
	movq	%r12, %rsi
	rolq	$50, %rsi
	movq	%r12, %rbp
	rolq	$46, %rbp
	xorq	%rsi, %rbp
	movq	%r12, %rsi
	rolq	$23, %rsi
	xorq	%rbp, %rsi
	addq	%r8, %rcx
	movq	%r14, %r8
	movq	%r14, %rdi
	movq	-88(%rsp), %rdx                 # 8-byte Reload
	xorq	%rdx, %rdi
	andq	%r12, %rdi
	addq	%rax, %rcx
	movq	%rcx, 72(%rsp)                  # 8-byte Spill
	xorq	%rdx, %rdi
	movq	-120(%rsp), %r14                # 8-byte Reload
	addq	%rcx, %r14
	addq	%rdi, %r14
	addq	%rsi, %r14
	movq	%r9, -40(%rsp)                  # 8-byte Spill
	movq	%r9, %rsi
	rolq	$36, %rsi
	movq	%r9, %rdi
	rolq	$30, %rdi
	xorq	%rsi, %rdi
	movq	%r9, %rsi
	rolq	$25, %rsi
	xorq	%rdi, %rsi
	movq	-64(%rsp), %rax                 # 8-byte Reload
	xorq	%rax, %r9
	movq	%r9, %rdi
	movq	%r9, -120(%rsp)                 # 8-byte Spill
	movq	%rax, %rcx
	movq	%rax, -64(%rsp)                 # 8-byte Spill
	movq	-8(%rsp), %r9                   # 8-byte Reload
	movq	%r9, %r15
	rolq	$56, %r15
	movq	-72(%rsp), %r10                 # 8-byte Reload
	movq	%r10, %r12
	rolq	$56, %r12
	movq	-32(%rsp), %rax                 # 8-byte Reload
	andq	%rdi, %rax
	movq	24(%rsp), %rbp                  # 8-byte Reload
	movq	%rbp, %rbx
	rorq	%rbx
	xorq	%rcx, %rax
	addq	%rsi, %rax
	movq	%r9, %rsi
	rorq	%rsi
	xorq	%rsi, %r15
	movq	%r10, %rsi
	rorq	%rsi
	xorq	%rsi, %r12
	movq	%rbp, %rsi
	rolq	$56, %rsi
	xorq	%rbx, %rsi
	movq	56(%rsp), %rcx                  # 8-byte Reload
	movq	%rcx, %rbx
	rorq	%rbx
	movq	%rcx, %rdi
	rolq	$56, %rdi
	xorq	%rbx, %rdi
	movq	%rbp, %rbx
	shrq	$7, %rbx
	xorq	%rsi, %rbx
	addq	%r10, %rbx
	movq	%rbx, 16(%rsp)                  # 8-byte Spill
	shrq	$7, %r10
	xorq	%r12, %r10
	addq	%r9, %r10
	movq	%r10, -72(%rsp)                 # 8-byte Spill
	shrq	$7, %r9
	xorq	%r15, %r9
	addq	%rcx, %r9
	movq	%rcx, %rsi
	shrq	$7, %rsi
	xorq	%rdi, %rsi
	addq	%rsi, %r13
	movq	-128(%rsp), %rcx                # 8-byte Reload
	addq	(%rcx), %r14
	movq	-24(%rsp), %rbx                 # 8-byte Reload
	addq	%r14, %rbx
	addq	%r14, %rax
	movq	%rbx, %rsi
	rolq	$50, %rsi
	movq	%rbx, %rdi
	rolq	$46, %rdi
	xorq	%rsi, %rdi
	movq	%rbx, %rsi
	rolq	$23, %rsi
	xorq	%rdi, %rsi
	addq	-104(%rsp), %r13                # 8-byte Folded Reload
	movq	%r11, %rdi
	xorq	%r8, %rdi
	andq	%rbx, %rdi
	xorq	%r8, %rdi
	addq	%r13, %rdx
	addq	8(%rcx), %rdx
	addq	%rdi, %rdx
	movq	%rax, %rdi
	rolq	$36, %rdi
	movq	%rax, %rbp
	rolq	$30, %rbp
	xorq	%rdi, %rbp
	movq	%rax, %rdi
	rolq	$25, %rdi
	xorq	%rbp, %rdi
	addq	%rsi, %rdx
	movq	%rax, %rcx
	movq	%rax, %rbp
	movq	-40(%rsp), %rax                 # 8-byte Reload
	xorq	%rax, %rcx
	movq	-120(%rsp), %r10                # 8-byte Reload
	andq	%rcx, %r10
	xorq	%rax, %r10
	addq	%rdi, %r10
	movq	72(%rsp), %rax                  # 8-byte Reload
	movq	%rax, %rsi
	rolq	$3, %rsi
	movq	-16(%rsp), %r12                 # 8-byte Reload
	addq	%rdx, %r12
	movq	%rax, %r14
	rolq	$45, %r14
	addq	%rdx, %r10
	xorq	%r14, %rsi
	movq	%rax, %rdx
	shrq	$6, %rdx
	xorq	%rsi, %rdx
	movq	%r12, %rsi
	rolq	$50, %rsi
	addq	-80(%rsp), %r9                  # 8-byte Folded Reload
	movq	%r12, %rax
	rolq	$46, %rax
	addq	%rdx, %r9
	movq	%r12, %rdx
	rolq	$23, %rdx
	xorq	%rsi, %rax
	xorq	%rax, %rdx
	movq	%rbx, %rsi
	xorq	%r11, %rsi
	andq	%r12, %rsi
	xorq	%r11, %rsi
	addq	%r9, %r8
	movq	%r10, %rdi
	rolq	$36, %rdi
	movq	-128(%rsp), %rax                # 8-byte Reload
	addq	16(%rax), %r8
	addq	%rsi, %r8
	movq	%r10, %rsi
	rolq	$30, %rsi
	addq	%rdx, %r8
	movq	%r10, %rdx
	rolq	$25, %rdx
	xorq	%rdi, %rsi
	xorq	%rsi, %rdx
	movq	%r10, %r14
	movq	%rbp, -32(%rsp)                 # 8-byte Spill
	xorq	%rbp, %r14
	andq	%r14, %rcx
	movq	%r13, -8(%rsp)                  # 8-byte Spill
	movq	%r13, %rax
	rolq	$45, %rax
	movq	%r13, %rdi
	rolq	$3, %rdi
	xorq	%rbp, %rcx
	addq	%rdx, %rcx
	xorq	%rax, %rdi
	movq	-64(%rsp), %rsi                 # 8-byte Reload
	addq	%r8, %rsi
	movq	%rsi, -64(%rsp)                 # 8-byte Spill
	addq	%r8, %rcx
	movq	%r13, %rax
	shrq	$6, %rax
	xorq	%rdi, %rax
	movq	-56(%rsp), %r13                 # 8-byte Reload
	movq	-72(%rsp), %rdx                 # 8-byte Reload
	addq	%r13, %rdx
	addq	%rax, %rdx
	movq	%rdx, -72(%rsp)                 # 8-byte Spill
	movq	%r12, %rax
	xorq	%rbx, %rax
	andq	%rsi, %rax
	addq	%rdx, %r11
	movq	-128(%rsp), %rdx                # 8-byte Reload
	addq	24(%rdx), %r11
	xorq	%rbx, %rax
	addq	%rax, %r11
	movq	%rsi, %rax
	rolq	$50, %rax
	movq	%rsi, %rdx
	rolq	$46, %rdx
	xorq	%rax, %rdx
	movq	%rsi, %rax
	movq	%rsi, %r8
	rolq	$23, %rax
	xorq	%rdx, %rax
	movq	%rcx, %rbp
	movq	%rcx, %rdx
	rolq	$36, %rdx
	movq	%rcx, %rsi
	rolq	$30, %rsi
	xorq	%rdx, %rsi
	movq	%rcx, %rdx
	rolq	$25, %rdx
	xorq	%rsi, %rdx
	addq	%rax, %r11
	movq	%rcx, %r15
	movq	%r10, -120(%rsp)                # 8-byte Spill
	xorq	%r10, %r15
	movq	%r14, %rsi
	andq	%r15, %rsi
	xorq	%r10, %rsi
	addq	%rdx, %rsi
	movq	%r9, 56(%rsp)                   # 8-byte Spill
	movq	%r9, %rax
	rolq	$45, %rax
	movq	%r9, %rdx
	rolq	$3, %rdx
	movq	-40(%rsp), %r14                 # 8-byte Reload
	addq	%r11, %r14
	addq	%r11, %rsi
	movq	%rsi, %rcx
	xorq	%rax, %rdx
	shrq	$6, %r9
	xorq	%rdx, %r9
	movq	%r14, %rdx
	rolq	$50, %rdx
	movq	16(%rsp), %rdi                  # 8-byte Reload
	addq	-96(%rsp), %rdi                 # 8-byte Folded Reload
	movq	%r14, %rsi
	rolq	$46, %rsi
	addq	%r9, %rdi
	movq	%rdi, 16(%rsp)                  # 8-byte Spill
	movq	%r14, %rax
	movq	%r14, -40(%rsp)                 # 8-byte Spill
	rolq	$23, %rax
	xorq	%rdx, %rsi
	xorq	%rsi, %rax
	movq	%r12, %r10
	xorq	%r12, %r8
	addq	%rdi, %rbx
	movq	-128(%rsp), %rsi                # 8-byte Reload
	addq	32(%rsi), %rbx
	andq	%r14, %r8
	xorq	%r12, %r8
	movq	%rcx, -88(%rsp)                 # 8-byte Spill
	movq	%rcx, %rsi
	rolq	$36, %rsi
	addq	%r8, %rbx
	movq	%rcx, %rdx
	rolq	$30, %rdx
	addq	%rax, %rbx
	movq	%rcx, %rax
	rolq	$25, %rax
	xorq	%rsi, %rdx
	xorq	%rdx, %rax
	movq	%rcx, %rdx
	movq	%rbp, -16(%rsp)                 # 8-byte Spill
	xorq	%rbp, %rcx
	movq	%rcx, -24(%rsp)                 # 8-byte Spill
	movq	-80(%rsp), %r14                 # 8-byte Reload
	movq	%r13, %r11
	movq	8(%rsp), %rdx                   # 8-byte Reload
	movq	%rdx, %r9
	rorq	%r9
	andq	%rcx, %r15
	movq	%rdx, %rsi
	movq	%rdx, 8(%rsp)                   # 8-byte Spill
	rolq	$56, %rsi
	xorq	%rbp, %r15
	addq	%rax, %r15
	xorq	%r9, %rsi
	movq	-32(%rsp), %r12                 # 8-byte Reload
	addq	%rbx, %r12
	addq	%rbx, %r15
	movq	%r15, 224(%rsp)                 # 8-byte Spill
	movq	-72(%rsp), %r8                  # 8-byte Reload
	movq	%r8, %rcx
	rolq	$45, %rcx
	movq	%rdx, %rax
	shrq	$7, %rax
	movq	%r8, %rdx
	rolq	$3, %rdx
	xorq	%rsi, %rax
	xorq	%rcx, %rdx
	movq	%r8, %rcx
	shrq	$6, %rcx
	xorq	%rdx, %rcx
	movq	40(%rsp), %rsi                  # 8-byte Reload
	rolq	$56, %rsi
	movq	%rsi, (%rsp)                    # 8-byte Spill
	movq	-48(%rsp), %rbp                 # 8-byte Reload
	movq	24(%rsp), %rdi                  # 8-byte Reload
	addq	%rbp, %rdi
	movq	32(%rsp), %rsi                  # 8-byte Reload
	rolq	$56, %rsi
	movq	%rsi, 208(%rsp)                 # 8-byte Spill
	addq	%rax, %rdi
	movq	%r12, %rsi
	movq	%r12, %rax
	rolq	$50, %rax
	addq	%rcx, %rdi
	movq	%rdi, 24(%rsp)                  # 8-byte Spill
	movq	%r12, %rcx
	movq	%r12, -32(%rsp)                 # 8-byte Spill
	rolq	$46, %rcx
	xorq	%rax, %rcx
	movq	%rcx, 216(%rsp)                 # 8-byte Spill
	movq	-112(%rsp), %rax                # 8-byte Reload
	rolq	$56, %rax
	movq	%rax, %r9
	movq	-104(%rsp), %rbx                # 8-byte Reload
	rolq	$56, %rbx
	movq	%rbx, 200(%rsp)                 # 8-byte Spill
	movq	%r14, %rax
	movq	%r14, %r13
	rolq	$56, %rax
	movq	%rax, 192(%rsp)                 # 8-byte Spill
	movq	-128(%rsp), %rcx                # 8-byte Reload
	movq	40(%rcx), %rax
	addq	%rdi, %rax
	addq	%r10, %rax
	movq	%rax, %rdi
	movq	%r11, %rax
	movq	%r11, %r14
	rolq	$56, %rax
	movq	%rax, %rdx
	movq	-96(%rsp), %rax                 # 8-byte Reload
	rolq	$56, %rax
	movq	%rax, 184(%rsp)                 # 8-byte Spill
	movq	%rbp, %rax
	movq	%rbp, %rcx
	rorq	%rcx
	movq	%rcx, 168(%rsp)                 # 8-byte Spill
	movq	-40(%rsp), %rbx                 # 8-byte Reload
	movq	-64(%rsp), %rbp                 # 8-byte Reload
	xorq	%rbp, %rbx
	rolq	$56, %rax
	movq	%rax, 176(%rsp)                 # 8-byte Spill
	andq	%r12, %rbx
	movq	64(%rsp), %rcx                  # 8-byte Reload
	movq	%rcx, %rax
	rorq	%rax
	xorq	%rbp, %rbx
	addq	%rbx, %rdi
	movq	%rdi, 160(%rsp)                 # 8-byte Spill
	movq	40(%rsp), %rbx                  # 8-byte Reload
	rorq	%rbx
	movq	(%rsp), %r12                    # 8-byte Reload
	xorq	%rbx, %r12
	movq	32(%rsp), %rbx                  # 8-byte Reload
	rorq	%rbx
	movq	208(%rsp), %r15                 # 8-byte Reload
	xorq	%rbx, %r15
	movq	-112(%rsp), %rbx                # 8-byte Reload
	rorq	%rbx
	movq	%r9, %r11
	xorq	%rbx, %r11
	movq	-104(%rsp), %rbp                # 8-byte Reload
	movq	%rbp, %rbx
	rorq	%rbx
	movq	200(%rsp), %r9                  # 8-byte Reload
	xorq	%rbx, %r9
	movq	%r13, %rbx
	rorq	%rbx
	movq	192(%rsp), %r8                  # 8-byte Reload
	xorq	%rbx, %r8
	movq	%r14, %rbx
	rorq	%rbx
	movq	%rdx, %rdi
	xorq	%rbx, %rdi
	movq	-96(%rsp), %r10                 # 8-byte Reload
	movq	%r10, %rbx
	rorq	%rbx
	movq	184(%rsp), %rsi                 # 8-byte Reload
	xorq	%rbx, %rsi
	movq	%rcx, %rbx
	rolq	$56, %rbx
	movq	176(%rsp), %rdx                 # 8-byte Reload
	xorq	168(%rsp), %rdx                 # 8-byte Folded Reload
	xorq	%rax, %rbx
	movq	%rcx, %rax
	shrq	$7, %rax
	xorq	%rbx, %rax
	movq	-48(%rsp), %rcx                 # 8-byte Reload
	addq	%rcx, %rax
	movq	%rax, (%rsp)                    # 8-byte Spill
	shrq	$7, %rcx
	xorq	%rdx, %rcx
	addq	%r10, %rcx
	movq	%rcx, -48(%rsp)                 # 8-byte Spill
	shrq	$7, %r10
	xorq	%rsi, %r10
	addq	%r14, %r10
	movq	%r10, -96(%rsp)                 # 8-byte Spill
	shrq	$7, %r14
	xorq	%rdi, %r14
	addq	%r13, %r14
	movq	%r14, -56(%rsp)                 # 8-byte Spill
	shrq	$7, %r13
	xorq	%r8, %r13
	addq	%rbp, %r13
	movq	%r13, -80(%rsp)                 # 8-byte Spill
	movq	%rbp, %rax
	shrq	$7, %rax
	xorq	%r9, %rax
	movq	-112(%rsp), %rbp                # 8-byte Reload
	addq	%rbp, %rax
	movq	%rax, -104(%rsp)                # 8-byte Spill
	movq	%rbp, %rax
	shrq	$7, %rax
	xorq	%r11, %rax
	movq	32(%rsp), %rbp                  # 8-byte Reload
	addq	%rbp, %rax
	movq	%rax, -112(%rsp)                # 8-byte Spill
	shrq	$7, %rbp
	xorq	%r15, %rbp
	movq	40(%rsp), %r9                   # 8-byte Reload
	addq	%r9, %rbp
	shrq	$7, %r9
	xorq	%r12, %r9
	movq	224(%rsp), %r13                 # 8-byte Reload
	movq	%r13, %rdx
	rolq	$36, %rdx
	movq	%r13, %rbx
	rolq	$30, %rbx
	xorq	%rdx, %rbx
	movq	-32(%rsp), %rsi                 # 8-byte Reload
	movq	%rsi, %rdx
	rolq	$23, %rdx
	xorq	216(%rsp), %rdx                 # 8-byte Folded Reload
	movq	%r13, %rax
	rolq	$25, %rax
	xorq	%rbx, %rax
	movq	16(%rsp), %rcx                  # 8-byte Reload
	movq	%rcx, %r8
	rolq	$45, %r8
	movq	%rcx, %rbx
	rolq	$3, %rbx
	xorq	%r8, %rbx
	movq	160(%rsp), %rdi                 # 8-byte Reload
	addq	%rdx, %rdi
	movq	%rcx, %rdx
	shrq	$6, %rdx
	xorq	%rbx, %rdx
	movq	%r13, %r15
	movq	-88(%rsp), %rcx                 # 8-byte Reload
	xorq	%rcx, %r15
	movq	-24(%rsp), %r14                 # 8-byte Reload
	andq	%r15, %r14
	xorq	%rcx, %r14
	addq	%rax, %r14
	movq	8(%rsp), %rcx                   # 8-byte Reload
	addq	64(%rsp), %rcx                  # 8-byte Folded Reload
	addq	%r9, %rcx
	addq	%rdx, %rcx
	movq	-128(%rsp), %rax                # 8-byte Reload
	movq	48(%rax), %rax
	addq	%rcx, %rax
	movq	%rcx, %r12
	addq	-64(%rsp), %rax                 # 8-byte Folded Reload
	movq	-120(%rsp), %r10                # 8-byte Reload
	movq	%rdi, %rcx
	addq	%rdi, %r10
	addq	%rdi, %r14
	movq	%rsi, %rcx
	movq	%rsi, %r9
	movq	-40(%rsp), %rdx                 # 8-byte Reload
	xorq	%rdx, %rcx
	andq	%r10, %rcx
	xorq	%rdx, %rcx
	movq	%rdx, %rsi
	addq	%rcx, %rax
	movq	%r10, %rcx
	rolq	$50, %rcx
	movq	%r10, %rdx
	rolq	$46, %rdx
	xorq	%rcx, %rdx
	movq	%r10, %rcx
	movq	%r10, %r8
	rolq	$23, %rcx
	xorq	%rdx, %rcx
	movq	%r14, %rdx
	rolq	$36, %rdx
	movq	%r14, %rbx
	rolq	$30, %rbx
	xorq	%rdx, %rbx
	movq	%r14, %rdx
	rolq	$25, %rdx
	xorq	%rbx, %rdx
	addq	%rcx, %rax
	movq	%r14, %rdi
	xorq	%r13, %rdi
	andq	%rdi, %r15
	xorq	%r13, %r15
	addq	%rdx, %r15
	movq	24(%rsp), %rcx                  # 8-byte Reload
	movq	%rcx, %rbx
	rolq	$45, %rbx
	movq	%rcx, %rdx
	rolq	$3, %rdx
	movq	-16(%rsp), %r11                 # 8-byte Reload
	addq	%rax, %r11
	addq	%rax, %r15
	xorq	%rbx, %rdx
	movq	%rcx, %rax
	shrq	$6, %rax
	xorq	%rdx, %rax
	movq	%r11, %rcx
	rolq	$50, %rcx
	addq	72(%rsp), %rbp                  # 8-byte Folded Reload
	movq	%r11, %rdx
	rolq	$46, %rdx
	addq	%rax, %rbp
	movq	%r11, %rax
	rolq	$23, %rax
	xorq	%rcx, %rdx
	xorq	%rdx, %rax
	movq	-128(%rsp), %rcx                # 8-byte Reload
	movq	56(%rcx), %rcx
	addq	%rbp, %rcx
	addq	%rsi, %rcx
	movq	%r10, %rdx
	movq	%r9, %rsi
	xorq	%r9, %rdx
	andq	%r11, %rdx
	xorq	%r9, %rdx
	movq	%r15, %rbx
	rolq	$36, %rbx
	addq	%rdx, %rcx
	movq	%r15, %rdx
	rolq	$30, %rdx
	addq	%rax, %rcx
	movq	%r15, %rax
	rolq	$25, %rax
	xorq	%rbx, %rdx
	xorq	%rdx, %rax
	movq	%r15, %r9
	xorq	%r14, %r9
	movq	%r12, 8(%rsp)                   # 8-byte Spill
	movq	%r12, %rdx
	rolq	$45, %rdx
	andq	%r9, %rdi
	movq	%r12, %rbx
	rolq	$3, %rbx
	xorq	%r14, %rdi
	addq	%rax, %rdi
	xorq	%rdx, %rbx
	movq	%r12, %rax
	shrq	$6, %rax
	xorq	%rbx, %rax
	movq	-112(%rsp), %r12                # 8-byte Reload
	addq	-8(%rsp), %r12                  # 8-byte Folded Reload
	addq	%rax, %r12
	movq	-128(%rsp), %rax                # 8-byte Reload
	movq	64(%rax), %rax
	addq	%r12, %rax
	addq	%rsi, %rax
	movq	-88(%rsp), %rsi                 # 8-byte Reload
	addq	%rcx, %rsi
	addq	%rcx, %rdi
	movq	%r11, %rcx
	xorq	%r10, %rcx
	andq	%rsi, %rcx
	xorq	%r10, %rcx
	addq	%rcx, %rax
	movq	%rsi, %rcx
	rolq	$50, %rcx
	movq	%rsi, %rdx
	rolq	$46, %rdx
	xorq	%rcx, %rdx
	movq	%rsi, %rcx
	rolq	$23, %rcx
	xorq	%rdx, %rcx
	movq	%rdi, %rdx
	rolq	$36, %rdx
	movq	%rdi, %rbx
	rolq	$30, %rbx
	xorq	%rdx, %rbx
	movq	%rdi, %rdx
	rolq	$25, %rdx
	xorq	%rbx, %rdx
	addq	%rcx, %rax
	movq	%rdi, %r10
	xorq	%r15, %r10
	andq	%r10, %r9
	xorq	%r15, %r9
	addq	%rdx, %r9
	movq	%rbp, %rcx
	rolq	$45, %rcx
	movq	%rbp, %rdx
	rolq	$3, %rdx
	addq	%rax, %r13
	addq	%rax, %r9
	xorq	%rcx, %rdx
	movq	%rbp, %rax
	movq	%rbp, 40(%rsp)                  # 8-byte Spill
	shrq	$6, %rax
	xorq	%rdx, %rax
	movq	%r13, %rcx
	rolq	$50, %rcx
	movq	-104(%rsp), %rbx                # 8-byte Reload
	addq	56(%rsp), %rbx                  # 8-byte Folded Reload
	movq	%r13, %rdx
	rolq	$46, %rdx
	addq	%rax, %rbx
	movq	%rbx, -104(%rsp)                # 8-byte Spill
	movq	%r13, %rax
	rolq	$23, %rax
	xorq	%rcx, %rdx
	xorq	%rdx, %rax
	movq	-128(%rsp), %rcx                # 8-byte Reload
	movq	72(%rcx), %rcx
	addq	%rbx, %rcx
	addq	%r8, %rcx
	movq	%rsi, %rdx
	xorq	%r11, %rdx
	andq	%r13, %rdx
	movq	%r13, %r8
	xorq	%r11, %rdx
	movq	%r9, %rbx
	rolq	$36, %rbx
	addq	%rdx, %rcx
	movq	%r9, %rdx
	rolq	$30, %rdx
	addq	%rax, %rcx
	movq	%r9, %rax
	rolq	$25, %rax
	xorq	%rbx, %rdx
	xorq	%rdx, %rax
	movq	%r9, %r13
	xorq	%rdi, %r13
	movq	%r12, -112(%rsp)                # 8-byte Spill
	movq	%r12, %rdx
	rolq	$45, %rdx
	andq	%r13, %r10
	movq	%r12, %rbx
	rolq	$3, %rbx
	xorq	%rdi, %r10
	addq	%rax, %r10
	xorq	%rdx, %rbx
	shrq	$6, %r12
	xorq	%rbx, %r12
	movq	-80(%rsp), %rdx                 # 8-byte Reload
	addq	-72(%rsp), %rdx                 # 8-byte Folded Reload
	addq	%r12, %rdx
	movq	%rdx, -80(%rsp)                 # 8-byte Spill
	movq	-128(%rsp), %rax                # 8-byte Reload
	movq	80(%rax), %rax
	addq	%rdx, %rax
	addq	%r11, %rax
	addq	%rcx, %r14
	addq	%rcx, %r10
	movq	%r8, %rcx
	xorq	%rsi, %rcx
	andq	%r14, %rcx
	xorq	%rsi, %rcx
	addq	%rcx, %rax
	movq	%r14, %rcx
	rolq	$50, %rcx
	movq	%r14, %rdx
	rolq	$46, %rdx
	xorq	%rcx, %rdx
	movq	%r14, %rcx
	rolq	$23, %rcx
	xorq	%rdx, %rcx
	movq	%r10, %rdx
	rolq	$36, %rdx
	movq	%r10, %rbx
	rolq	$30, %rbx
	xorq	%rdx, %rbx
	movq	%r10, %rdx
	rolq	$25, %rdx
	xorq	%rbx, %rdx
	addq	%rcx, %rax
	movq	%r10, %r12
	xorq	%r9, %r12
	andq	%r12, %r13
	xorq	%r9, %r13
	addq	%rdx, %r13
	movq	-104(%rsp), %rcx                # 8-byte Reload
	movq	%rcx, %rbx
	rolq	$45, %rbx
	movq	%rcx, %rdx
	rolq	$3, %rdx
	addq	%rax, %r15
	addq	%rax, %r13
	xorq	%rbx, %rdx
	movq	%rcx, %rax
	shrq	$6, %rax
	xorq	%rdx, %rax
	movq	%r15, %rcx
	rolq	$50, %rcx
	movq	-56(%rsp), %rbx                 # 8-byte Reload
	addq	16(%rsp), %rbx                  # 8-byte Folded Reload
	movq	%r15, %rdx
	rolq	$46, %rdx
	addq	%rax, %rbx
	movq	%rbx, -56(%rsp)                 # 8-byte Spill
	movq	%r15, %rax
	rolq	$23, %rax
	xorq	%rcx, %rdx
	xorq	%rdx, %rax
	movq	-128(%rsp), %rcx                # 8-byte Reload
	movq	88(%rcx), %rdx
	addq	%rbx, %rdx
	addq	%rsi, %rdx
	movq	%r14, %rcx
	xorq	%r8, %rcx
	andq	%r15, %rcx
	xorq	%r8, %rcx
	movq	%r13, %rbx
	rolq	$36, %rbx
	addq	%rcx, %rdx
	movq	%r13, %rcx
	rolq	$30, %rcx
	addq	%rax, %rdx
	movq	%r13, %rax
	rolq	$25, %rax
	xorq	%rbx, %rcx
	xorq	%rcx, %rax
	movq	%r13, %r11
	xorq	%r10, %r11
	movq	-80(%rsp), %rsi                 # 8-byte Reload
	movq	%rsi, %rbx
	rolq	$45, %rbx
	andq	%r11, %r12
	movq	%rsi, %rcx
	rolq	$3, %rcx
	xorq	%r10, %r12
	addq	%rax, %r12
	xorq	%rbx, %rcx
	movq	%rsi, %rax
	shrq	$6, %rax
	xorq	%rcx, %rax
	movq	-96(%rsp), %rcx                 # 8-byte Reload
	addq	24(%rsp), %rcx                  # 8-byte Folded Reload
	addq	%rax, %rcx
	movq	%rcx, -96(%rsp)                 # 8-byte Spill
	movq	-128(%rsp), %rax                # 8-byte Reload
	movq	96(%rax), %rax
	addq	%rcx, %rax
	addq	%r8, %rax
	addq	%rdx, %rdi
	addq	%rdx, %r12
	movq	%r15, %rcx
	xorq	%r14, %rcx
	andq	%rdi, %rcx
	xorq	%r14, %rcx
	addq	%rcx, %rax
	movq	%rdi, %rcx
	rolq	$50, %rcx
	movq	%rdi, %rdx
	rolq	$46, %rdx
	xorq	%rcx, %rdx
	movq	%rdi, %rcx
	rolq	$23, %rcx
	xorq	%rdx, %rcx
	movq	%r12, %rdx
	rolq	$36, %rdx
	movq	%r12, %rbx
	rolq	$30, %rbx
	xorq	%rdx, %rbx
	movq	%r12, %rdx
	rolq	$25, %rdx
	xorq	%rbx, %rdx
	addq	%rcx, %rax
	movq	%r12, %r8
	xorq	%r13, %r8
	andq	%r8, %r11
	xorq	%r13, %r11
	addq	%rdx, %r11
	movq	-56(%rsp), %rsi                 # 8-byte Reload
	movq	%rsi, %rcx
	rolq	$45, %rcx
	movq	%rsi, %rdx
	rolq	$3, %rdx
	addq	%rax, %r9
	addq	%rax, %r11
	xorq	%rcx, %rdx
	movq	%rsi, %rax
	shrq	$6, %rax
	xorq	%rdx, %rax
	movq	%r9, %rcx
	rolq	$50, %rcx
	movq	-48(%rsp), %rsi                 # 8-byte Reload
	addq	8(%rsp), %rsi                   # 8-byte Folded Reload
	movq	%r9, %rdx
	rolq	$46, %rdx
	addq	%rax, %rsi
	movq	%rsi, -48(%rsp)                 # 8-byte Spill
	movq	%r9, %rax
	rolq	$23, %rax
	xorq	%rcx, %rdx
	xorq	%rdx, %rax
	movq	-128(%rsp), %rcx                # 8-byte Reload
	movq	104(%rcx), %rcx
	addq	%rsi, %rcx
	addq	%r14, %rcx
	movq	%rdi, %rdx
	xorq	%r15, %rdx
	andq	%r9, %rdx
	xorq	%r15, %rdx
	movq	%r11, %rbx
	rolq	$36, %rbx
	addq	%rdx, %rcx
	movq	%r11, %rdx
	rolq	$30, %rdx
	addq	%rax, %rcx
	movq	%r11, %rax
	rolq	$25, %rax
	xorq	%rbx, %rdx
	xorq	%rdx, %rax
	movq	%r11, %rsi
	xorq	%r12, %rsi
	movq	-96(%rsp), %rdx                 # 8-byte Reload
	movq	%rdx, %r14
	rolq	$45, %r14
	andq	%rsi, %r8
	movq	%rdx, %rbx
	rolq	$3, %rbx
	xorq	%r12, %r8
	addq	%rax, %r8
	xorq	%r14, %rbx
	movq	%rdx, %rax
	shrq	$6, %rax
	xorq	%rbx, %rax
	movq	(%rsp), %rdx                    # 8-byte Reload
	addq	%rbp, %rdx
	addq	%rax, %rdx
	movq	%rdx, (%rsp)                    # 8-byte Spill
	movq	-128(%rsp), %rax                # 8-byte Reload
	movq	112(%rax), %r14
	addq	%rdx, %r14
	addq	%r15, %r14
	addq	%rcx, %r10
	addq	%rcx, %r8
	movq	%r9, %rax
	xorq	%rdi, %rax
	andq	%r10, %rax
	xorq	%rdi, %rax
	addq	%rax, %r14
	movq	%r10, %rax
	rolq	$50, %rax
	movq	%r10, %rcx
	rolq	$46, %rcx
	xorq	%rax, %rcx
	movq	%r10, %rax
	rolq	$23, %rax
	xorq	%rcx, %rax
	movq	%r8, %rcx
	rolq	$36, %rcx
	movq	%r8, %rdx
	rolq	$30, %rdx
	xorq	%rcx, %rdx
	movq	%r8, %rcx
	rolq	$25, %rcx
	xorq	%rdx, %rcx
	addq	%rax, %r14
	movq	72(%rsp), %rbp                  # 8-byte Reload
	movq	%rbp, %rax
	rorq	%rax
	movq	%rbp, %rdx
	rolq	$56, %rdx
	xorq	%rax, %rdx
	movq	%r8, %r15
	xorq	%r11, %r15
	andq	%r15, %rsi
	movq	%r11, -24(%rsp)                 # 8-byte Spill
	xorq	%r11, %rsi
	addq	%rcx, %rsi
	movq	-48(%rsp), %rax                 # 8-byte Reload
	movq	%rax, %r11
	rolq	$45, %r11
	movq	%rbp, %rbx
	shrq	$7, %rbx
	movq	%rax, %rcx
	rolq	$3, %rcx
	xorq	%rdx, %rbx
	xorq	%r11, %rcx
	shrq	$6, %rax
	xorq	%rcx, %rax
	addq	64(%rsp), %rbx                  # 8-byte Folded Reload
	addq	%r14, %r13
	addq	%r14, %rsi
	movq	%rsi, %r11
	movq	-112(%rsp), %rsi                # 8-byte Reload
	addq	%rsi, %rbx
	addq	%rax, %rbx
	movq	-128(%rsp), %rax                # 8-byte Reload
	movq	120(%rax), %rax
	addq	%rbx, %rax
	addq	%rdi, %rax
	movq	%r13, %rcx
	rolq	$50, %rcx
	movq	%r13, %rdx
	rolq	$46, %rdx
	xorq	%rcx, %rdx
	movq	%r13, %rcx
	rolq	$23, %rcx
	xorq	%rdx, %rcx
	movq	%r10, -88(%rsp)                 # 8-byte Spill
	xorq	%r9, %r10
	movq	%r13, %r14
	andq	%r13, %r10
	movq	%r9, -120(%rsp)                 # 8-byte Spill
	xorq	%r9, %r10
	addq	%r10, %rax
	movq	%r11, %rdx
	rolq	$36, %rdx
	addq	%rcx, %rax
	movq	%r11, %rcx
	rolq	$30, %rcx
	xorq	%rdx, %rcx
	movq	%r11, %rdx
	rolq	$25, %rdx
	xorq	%rcx, %rdx
	movq	%r11, -64(%rsp)                 # 8-byte Spill
	movq	%r11, %rcx
	xorq	%r8, %rcx
	movq	%rcx, -32(%rsp)                 # 8-byte Spill
	andq	%rcx, %r15
	movq	%r8, -16(%rsp)                  # 8-byte Spill
	xorq	%r8, %r15
	addq	%rdx, %r15
	movq	-128(%rsp), %rdi                # 8-byte Reload
	addq	%rax, %r12
	addq	%rax, %r15
	movq	%r15, %r9
	movq	%rbp, %r15
	movq	-8(%rsp), %r8                   # 8-byte Reload
	movq	-72(%rsp), %rax                 # 8-byte Reload
	movq	%rax, -8(%rsp)                  # 8-byte Spill
	movq	16(%rsp), %rax                  # 8-byte Reload
	movq	%rax, -72(%rsp)                 # 8-byte Spill
	movq	%rsi, 32(%rsp)                  # 8-byte Spill
	movq	-104(%rsp), %rax                # 8-byte Reload
	movq	-80(%rsp), %rcx                 # 8-byte Reload
	movq	%rcx, -104(%rsp)                # 8-byte Spill
	movq	-56(%rsp), %rcx                 # 8-byte Reload
	movq	%rcx, -80(%rsp)                 # 8-byte Spill
	movq	-96(%rsp), %rcx                 # 8-byte Reload
	movq	%rcx, -56(%rsp)                 # 8-byte Spill
	movq	-48(%rsp), %rcx                 # 8-byte Reload
	movq	%rcx, -96(%rsp)                 # 8-byte Spill
	subq	$-128, %rdi
	movq	(%rsp), %rcx                    # 8-byte Reload
	leaq	_ZN7fast_io7details6sha51210k512scalarE+512(%rip), %rsi
	movq	%rdi, %rdx
	cmpq	%rsi, %rdi
	jne	.LBB0_3
# %bb.4:                                #   in Loop: Header=BB0_2 Depth=1
	movq	112(%rsp), %r15                 # 8-byte Reload
	addq	%r9, %r15
	movq	80(%rsp), %rdx                  # 8-byte Reload
	addq	-64(%rsp), %rdx                 # 8-byte Folded Reload
	movq	88(%rsp), %rcx                  # 8-byte Reload
	addq	-16(%rsp), %rcx                 # 8-byte Folded Reload
	movq	120(%rsp), %rax                 # 8-byte Reload
	addq	-24(%rsp), %rax                 # 8-byte Folded Reload
	movq	%rax, %r11
	movq	128(%rsp), %rbx                 # 8-byte Reload
	addq	%r12, %rbx
	movq	136(%rsp), %r10                 # 8-byte Reload
	addq	%r14, %r10
	movq	144(%rsp), %r13                 # 8-byte Reload
	addq	-88(%rsp), %r13                 # 8-byte Folded Reload
	movq	152(%rsp), %rbp                 # 8-byte Reload
	addq	-120(%rsp), %rbp                # 8-byte Folded Reload
	movq	48(%rsp), %rsi                  # 8-byte Reload
	subq	$-128, %rsi
	cmpq	104(%rsp), %rsi                 # 8-byte Folded Reload
	jne	.LBB0_2
# %bb.5:
	movq	96(%rsp), %rax                  # 8-byte Reload
	movq	%r15, (%rax)
	movq	%rdx, 8(%rax)
	movq	%rcx, 16(%rax)
	movq	%r11, 24(%rax)
	movq	%rbx, 32(%rax)
	movq	%r10, 40(%rax)
	movq	%r13, 48(%rax)
	movq	%rbp, 56(%rax)
.LBB0_6:
	addq	$232, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
.Lfunc_end0:
	.size	_Z14sha512_invokerPmPKSt4byteS2_, .Lfunc_end0-_Z14sha512_invokerPmPKSt4byteS2_
                                        # -- End function
	.type	_ZN7fast_io7details6sha51210k512scalarE,@object # @_ZN7fast_io7details6sha51210k512scalarE
	.section	.rodata._ZN7fast_io7details6sha51210k512scalarE,"aG",@progbits,_ZN7fast_io7details6sha51210k512scalarE,comdat
	.weak	_ZN7fast_io7details6sha51210k512scalarE
	.p2align	3
_ZN7fast_io7details6sha51210k512scalarE:
	.quad	-1973867731355612462            # 0xe49b69c19ef14ad2
	.quad	-1171420211273849373            # 0xefbe4786384f25e3
	.quad	1135362057144423861             # 0xfc19dc68b8cd5b5
	.quad	2597628984639134821             # 0x240ca1cc77ac9c65
	.quad	3308224258029322869             # 0x2de92c6f592b0275
	.quad	5365058923640841347             # 0x4a7484aa6ea6e483
	.quad	6679025012923562964             # 0x5cb0a9dcbd41fbd4
	.quad	8573033837759648693             # 0x76f988da831153b5
	.quad	-7476448914759557205            # 0x983e5152ee66dfab
	.quad	-6327057829258317296            # 0xa831c66d2db43210
	.quad	-5763719355590565569            # 0xb00327c898fb213f
	.quad	-4658551843659510044            # 0xbf597fc7beef0ee4
	.quad	-4116276920077217854            # 0xc6e00bf33da88fc2
	.quad	-3051310485924567259            # 0xd5a79147930aa725
	.quad	489312712824947311              # 0x6ca6351e003826f
	.quad	1452737877330783856             # 0x142929670a0e6e70
	.quad	2861767655752347644             # 0x27b70a8546d22ffc
	.quad	3322285676063803686             # 0x2e1b21385c26c926
	.quad	5560940570517711597             # 0x4d2c6dfc5ac42aed
	.quad	5996557281743188959             # 0x53380d139d95b3df
	.quad	7280758554555802590             # 0x650a73548baf63de
	.quad	8532644243296465576             # 0x766a0abb3c77b2a8
	.quad	-9096487096722542874            # 0x81c2c92e47edaee6
	.quad	-7894198246740708037            # 0x92722c851482353b
	.quad	-6719396339535248540            # 0xa2bfe8a14cf10364
	.quad	-6333637450476146687            # 0xa81a664bbc423001
	.quad	-4446306890439682159            # 0xc24b8b70d0f89791
	.quad	-4076793802049405392            # 0xc76c51a30654be30
	.quad	-3345356375505022440            # 0xd192e819d6ef5218
	.quad	-2983346525034927856            # 0xd69906245565a910
	.quad	-860691631967231958             # 0xf40e35855771202a
	.quad	1182934255886127544             # 0x106aa07032bbd1b8
	.quad	1847814050463011016             # 0x19a4c116b8d2d0c8
	.quad	2177327727835720531             # 0x1e376c085141ab53
	.quad	2830643537854262169             # 0x2748774cdf8eeb99
	.quad	3796741975233480872             # 0x34b0bcb5e19b48a8
	.quad	4115178125766777443             # 0x391c0cb3c5c95a63
	.quad	5681478168544905931             # 0x4ed8aa4ae3418acb
	.quad	6601373596472566643             # 0x5b9cca4f7763e373
	.quad	7507060721942968483             # 0x682e6ff3d6b2b8a3
	.quad	8399075790359081724             # 0x748f82ee5defb2fc
	.quad	8693463985226723168             # 0x78a5636f43172f60
	.quad	-8878714635349349518            # 0x84c87814a1f0ab72
	.quad	-8302665154208450068            # 0x8cc702081a6439ec
	.quad	-8016688836872298968            # 0x90befffa23631e28
	.quad	-6606660893046293015            # 0xa4506cebde82bde9
	.quad	-4685533653050689259            # 0xbef9a3f7b2c67915
	.quad	-4147400797238176981            # 0xc67178f2e372532b
	.quad	-3880063495543823972            # 0xca273eceea26619c
	.quad	-3348786107499101689            # 0xd186b8c721c0c207
	.quad	-1523767162380948706            # 0xeada7dd6cde0eb1e
	.quad	-757361751448694408             # 0xf57d4f7fee6ed178
	.quad	500013540394364858              # 0x6f067aa72176fba
	.quad	748580250866718886              # 0xa637dc5a2c898a6
	.quad	1242879168328830382             # 0x113f9804bef90dae
	.quad	1977374033974150939             # 0x1b710b35131c471b
	.quad	2944078676154940804             # 0x28db77f523047d84
	.quad	3659926193048069267             # 0x32caab7b40c72493
	.quad	4368137639120453308             # 0x3c9ebe0a15c9bebc
	.quad	4836135668995329356             # 0x431d67c49c100d4c
	.quad	5532061633213252278             # 0x4cc5d4becb3e42b6
	.quad	6448918945643986474             # 0x597f299cfc657e2a
	.quad	6902733635092675308             # 0x5fcb6fab3ad6faec
	.quad	7801388544844847127             # 0x6c44198c4a475817
	.size	_ZN7fast_io7details6sha51210k512scalarE, 512

	.section	".linker-options","e",@llvm_linker_options
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym _ZN7fast_io7details6sha51210k512scalarE
