	.text
	.file	"fast_iostring.cc"
	.globl	_Z27EvaluateSalaryAndReturnName8Employee # -- Begin function _Z27EvaluateSalaryAndReturnName8Employee
	.p2align	4, 0x90
	.type	_Z27EvaluateSalaryAndReturnName8Employee,@function
_Z27EvaluateSalaryAndReturnName8Employee: # @_Z27EvaluateSalaryAndReturnName8Employee
# %bb.0:
	pushq	%rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$104, %rsp
	movq	%rdi, 24(%rsp)                  # 8-byte Spill
	movq	160(%rsp), %rbx
	movq	168(%rsp), %r12
	cmpq	%r12, %rbx
	je	.LBB0_1
# %bb.2:
	subq	%rbx, %r12
	leaq	1(%r12), %rax
	cmpq	$1, %rax
	movq	%r12, %rdi
	adcq	$1, %rdi
	callq	malloc@PLT
	testq	%rax, %rax
	je	.LBB0_27
# %bb.3:
	movq	%rax, %r15
	movq	%rax, %rdi
	callq	malloc_usable_size@PLT
	movq	%rax, %r14
	movq	%r15, %rdi
	movq	%rbx, %rsi
	movq	%r12, %rdx
	callq	memcpy@PLT
	leaq	(%r15,%r12), %rax
	movb	$0, (%r15,%r12)
	leaq	(%r15,%r14), %rcx
	decq	%rcx
	movq	%r15, 56(%rsp)
	movq	%rax, 64(%rsp)
	movq	%rcx, 72(%rsp)
	jmp	.LBB0_4
.LBB0_1:
	leaq	80(%rsp), %rax
	movq	%rax, 56(%rsp)
	movq	%rax, 64(%rsp)
	movq	%rax, 72(%rsp)
	movb	$0, 80(%rsp)
	movq	%rax, %r15
.LBB0_4:
	movq	%rax, %rcx
	subq	%r15, %rcx
	cmpq	$3, %rcx
	movq	%rbx, 48(%rsp)                  # 8-byte Spill
	jne	.LBB0_5
# %bb.6:
	cmpq	%r15, %rax
	je	.LBB0_7
# %bb.8:
	movzwl	(%r15), %eax
	xorl	$17731, %eax                    # imm = 0x4543
	movzbl	2(%r15), %ecx
	xorl	$79, %ecx
	orw	%ax, %cx
	sete	%bpl
	jmp	.LBB0_9
.LBB0_5:
	xorl	%ebp, %ebp
	jmp	.LBB0_9
.LBB0_7:
	movb	$1, %bpl
.LBB0_9:
	leaq	160(%rsp), %rbx
	movq	%r15, %rdi
	callq	free@PLT
	testb	%bpl, %bpl
	jne	.LBB0_12
# %bb.10:
	cmpq	$100000, 32(%rbx)               # imm = 0x186A0
	jg	.LBB0_12
# %bb.11:
	movq	40(%rbx), %r15
	movq	48(%rbx), %rbp
	movq	72(%rbx), %r12
	movq	80(%rbx), %r14
	movq	%rbp, 32(%rsp)                  # 8-byte Spill
	subq	%r15, %rbp
	movq	%r14, %r13
	subq	%r12, %r13
	jmp	.LBB0_13
.LBB0_12:
	movq	40(%rbx), %r15
	movq	48(%rbx), %rbp
	movq	%rbp, 32(%rsp)                  # 8-byte Spill
	subq	%r15, %rbp
	movq	72(%rbx), %r12
	movq	80(%rbx), %r14
	movq	%r14, %r13
	subq	%r12, %r13
	leaq	.L.str.2(%rip), %rax
	movq	%rax, 88(%rsp)
	movq	$13, 96(%rsp)
	movq	stdout@GOTPCREL(%rip), %rax
	movq	(%rax), %rdi
	movups	88(%rsp), %xmm0
	movups	%xmm0, (%rsp)
	movq	%r15, %rsi
	movq	%rbp, %rdx
	movl	$32, %ecx
	movq	%r12, %r8
	movq	%r13, %r9
	callq	_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_
.LBB0_13:
	movq	24(%rsp), %rcx                  # 8-byte Reload
	leaq	24(%rcx), %rax
	movq	%rax, (%rcx)
	movq	%rax, 8(%rcx)
	movq	%rax, 16(%rcx)
	movb	$0, 24(%rcx)
	movq	%rbp, %rdi
	addq	%r13, %rdi
	jb	.LBB0_27
# %bb.14:
	incq	%rdi
	je	.LBB0_27
# %bb.15:
	movq	%r14, 40(%rsp)                  # 8-byte Spill
	leaq	1(%rdi), %rax
	cmpq	$1, %rax
	adcq	$1, %rdi
	callq	malloc@PLT
	testq	%rax, %rax
	je	.LBB0_27
# %bb.16:
	movq	%rax, %r14
	movq	%rax, %rdi
	callq	malloc_usable_size@PLT
	movb	$0, (%r14)
	addq	%r14, %rax
	decq	%rax
	movq	24(%rsp), %rcx                  # 8-byte Reload
	movq	%r14, (%rcx)
	movq	%rax, 16(%rcx)
	cmpq	%r15, 32(%rsp)                  # 8-byte Folded Reload
	je	.LBB0_18
# %bb.17:
	movq	%r14, %rdi
	movq	%r15, %rsi
	movq	%rbp, %rdx
	callq	memcpy@PLT
.LBB0_18:
	movb	$32, (%r14,%rbp)
	addq	%rbp, %r14
	incq	%r14
	cmpq	%r12, 40(%rsp)                  # 8-byte Folded Reload
	je	.LBB0_20
# %bb.19:
	movq	%r14, %rdi
	movq	%r12, %rsi
	movq	%r13, %rdx
	callq	memcpy@PLT
.LBB0_20:
	leaq	(%r14,%r13), %rax
	movq	24(%rsp), %rcx                  # 8-byte Reload
	movq	%rax, 8(%rcx)
	movb	$0, (%r14,%r13)
	leaq	96(%rbx), %rax
	cmpq	%rax, %r12
	sete	%al
	testq	%r12, %r12
	sete	%cl
	orb	%al, %cl
	jne	.LBB0_22
# %bb.21:
	movq	%r12, %rdi
	callq	free@PLT
.LBB0_22:
	leaq	64(%rbx), %rax
	cmpq	%rax, %r15
	sete	%al
	testq	%r15, %r15
	sete	%cl
	orb	%al, %cl
	jne	.LBB0_24
# %bb.23:
	movq	%r15, %rdi
	callq	free@PLT
.LBB0_24:
	addq	$24, %rbx
	movq	48(%rsp), %rdi                  # 8-byte Reload
	cmpq	%rbx, %rdi
	sete	%al
	testq	%rdi, %rdi
	sete	%cl
	orb	%al, %cl
	jne	.LBB0_26
# %bb.25:
	callq	free@PLT
.LBB0_26:
	movq	24(%rsp), %rax                  # 8-byte Reload
	addq	$104, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
.LBB0_27:
	ud2
.Lfunc_end0:
	.size	_Z27EvaluateSalaryAndReturnName8Employee, .Lfunc_end0-_Z27EvaluateSalaryAndReturnName8Employee
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
# %bb.0:
	xorl	%eax, %eax
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
                                        # -- End function
	.section	.text._ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_,"axG",@progbits,_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_,comdat
	.weak	_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_ # -- Begin function _ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_
	.p2align	4, 0x90
	.type	_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_,@function
_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_: # @_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_
# %bb.0:
	pushq	%rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$88, %rsp
	movq	%r9, %r13
	movq	%r8, 8(%rsp)                    # 8-byte Spill
	movl	%ecx, 4(%rsp)                   # 4-byte Spill
	movq	%rdx, %rbp
	movq	%rsi, %r12
	movq	%rdi, %rbx
	callq	flockfile@PLT
	movq	144(%rsp), %rax
	movq	%rax, 56(%rsp)                  # 8-byte Spill
	movq	152(%rsp), %r14
	movq	%r12, 64(%rsp)
	movq	%rbp, 72(%rsp)
	movq	40(%rbx), %r15
	movq	48(%rbx), %rax
	subq	%r15, %rax
	xorl	%ecx, %ecx
	testq	%rax, %rax
	cmovgq	%rax, %rcx
	cmpq	%rcx, %rbp
	jae	.LBB2_4
# %bb.1:
	testq	%rbp, %rbp
	je	.LBB2_3
# %bb.2:
	movq	%r15, %rdi
	movq	%r12, %rsi
	movq	%rbp, %rdx
	callq	memcpy@PLT
.LBB2_3:
	addq	%rbp, %r15
	movq	%r15, 40(%rbx)
.LBB2_5:
	movq	48(%rbx), %rax
	subq	%r15, %rax
	cmpq	$2, %rax
	movq	56(%rsp), %rbp                  # 8-byte Reload
	movq	8(%rsp), %rsi                   # 8-byte Reload
	jl	.LBB2_7
# %bb.6:
	movl	4(%rsp), %eax                   # 4-byte Reload
	movb	%al, (%r15)
	incq	%r15
	movq	%r15, 40(%rbx)
.LBB2_8:
	movq	%rsi, 16(%rsp)
	movq	%r13, 24(%rsp)
	movq	%rbp, 32(%rsp)
	movq	%r14, 40(%rsp)
	movq	48(%rbx), %rax
	subq	%r15, %rax
	xorl	%r12d, %r12d
	testq	%rax, %rax
	cmovgq	%rax, %r12
	subq	%r13, %r12
	jbe	.LBB2_9
# %bb.11:
	testq	%r13, %r13
	je	.LBB2_13
# %bb.12:
	movq	%r15, %rdi
	movq	%r13, %rdx
	callq	memcpy@PLT
.LBB2_13:
	addq	%r13, %r15
	cmpq	%r12, %r14
	jae	.LBB2_14
# %bb.15:
	testq	%r14, %r14
	je	.LBB2_17
# %bb.16:
	movq	%r15, %rdi
	movq	%rbp, %rsi
	movq	%r14, %rdx
	callq	memcpy@PLT
.LBB2_17:
	addq	%r14, %r15
	movq	%r15, 40(%rbx)
.LBB2_18:
	movq	%rbx, %rdi
	callq	funlockfile@PLT
	addq	$88, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
.LBB2_4:
	leaq	64(%rsp), %rsi
	movl	$1, %edx
	movq	%rbx, %rdi
	callq	_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm
	movq	40(%rbx), %r15
	jmp	.LBB2_5
.LBB2_7:
	movl	4(%rsp), %eax                   # 4-byte Reload
	movb	%al, 16(%rsp)
	leaq	17(%rsp), %rdx
	leaq	16(%rsp), %rsi
	movq	%rbx, %rdi
	callq	_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_
	movq	8(%rsp), %rsi                   # 8-byte Reload
	movq	40(%rbx), %r15
	jmp	.LBB2_8
.LBB2_9:
	leaq	16(%rsp), %rsi
	movl	$2, %edx
	jmp	.LBB2_10
.LBB2_14:
	leaq	32(%rsp), %rsi
	movl	$1, %edx
.LBB2_10:
	movq	%r15, 40(%rbx)
	movq	%rbx, %rdi
	callq	_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm
	jmp	.LBB2_18
.Lfunc_end2:
	.size	_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_, .Lfunc_end2-_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_
                                        # -- End function
	.section	.text.unlikely._ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm,"axG",@progbits,_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm,comdat
	.weak	_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm # -- Begin function _ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm
	.type	_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm,@function
_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm: # @_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm
# %bb.0:
	testq	%rdx, %rdx
	je	.LBB3_9
# %bb.1:
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	movq	%rdx, %rbx
	movq	%rsi, %r14
	movq	%rdi, %r15
	shlq	$4, %rbx
	addq	%rsi, %rbx
.LBB3_2:                                # =>This Inner Loop Header: Depth=1
	movq	(%r14), %rsi
	movq	8(%r14), %r12
	movq	40(%r15), %r13
	movq	48(%r15), %rax
	subq	%r13, %rax
	cmpq	%rax, %r12
	jge	.LBB3_6
# %bb.3:                                #   in Loop: Header=BB3_2 Depth=1
	testq	%r12, %r12
	je	.LBB3_5
# %bb.4:                                #   in Loop: Header=BB3_2 Depth=1
	movq	%r13, %rdi
	movq	%r12, %rdx
	callq	memcpy@PLT
.LBB3_5:                                #   in Loop: Header=BB3_2 Depth=1
	addq	%r12, %r13
	movq	%r13, 40(%r15)
.LBB3_7:                                #   in Loop: Header=BB3_2 Depth=1
	addq	$16, %r14
	cmpq	%rbx, %r14
	jne	.LBB3_2
	jmp	.LBB3_8
.LBB3_6:                                #   in Loop: Header=BB3_2 Depth=1
	addq	%rsi, %r12
	movq	%r15, %rdi
	movq	%r12, %rdx
	callq	_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_
	jmp	.LBB3_7
.LBB3_8:
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
.LBB3_9:
	retq
.Lfunc_end3:
	.size	_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm, .Lfunc_end3-_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm
                                        # -- End function
	.section	.text.unlikely._ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_,"axG",@progbits,_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_,comdat
	.weak	_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_ # -- Begin function _ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_
	.type	_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_,@function
_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_: # @_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_
# %bb.0:
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	movq	%rdx, %r15
	movq	%rsi, %r14
	movq	%rdi, %rbx
.LBB4_1:                                # =>This Inner Loop Header: Depth=1
	movq	%r15, %rdx
	subq	%r14, %rdx
	je	.LBB4_2
# %bb.3:                                #   in Loop: Header=BB4_1 Depth=1
	movl	$1, %esi
	movq	%r14, %rdi
	movq	%rbx, %rcx
	callq	fwrite_unlocked@PLT
	testq	%rax, %rax
	jne	.LBB4_4
	jmp	.LBB4_8
.LBB4_2:                                #   in Loop: Header=BB4_1 Depth=1
	xorl	%eax, %eax
.LBB4_4:                                #   in Loop: Header=BB4_1 Depth=1
	addq	%rax, %r14
	cmpq	%r15, %r14
	je	.LBB4_7
# %bb.5:                                #   in Loop: Header=BB4_1 Depth=1
	movq	40(%rbx), %r12
	movq	48(%rbx), %rax
	subq	%r12, %rax
	movq	%r15, %r13
	subq	%r14, %r13
	cmpq	%rax, %r13
	jge	.LBB4_1
# %bb.6:
	movq	%r12, %rdi
	movq	%r14, %rsi
	movq	%r13, %rdx
	callq	memcpy@PLT
	addq	%r13, %r12
	movq	%r12, 40(%rbx)
.LBB4_7:
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	retq
.LBB4_8:
	ud2
.Lfunc_end4:
	.size	_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_, .Lfunc_end4-_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_
                                        # -- End function
	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"CEO"
	.size	.L.str, 4

	.type	.L.str.2,@object                # @.str.2
.L.str.2:
	.asciz	" is overpaid\n"
	.size	.L.str.2, 14

	.section	".linker-options","e",@llvm_linker_options
	.section	".note.GNU-stack","",@progbits
	.addrsig
