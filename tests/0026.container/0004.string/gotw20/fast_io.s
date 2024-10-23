	.text
	.file	"fast_iostring.cc"
	.globl	_Z27EvaluateSalaryAndReturnName8Employee # -- Begin function _Z27EvaluateSalaryAndReturnName8Employee
	.p2align	4, 0x90
	.type	_Z27EvaluateSalaryAndReturnName8Employee,@function
_Z27EvaluateSalaryAndReturnName8Employee: # @_Z27EvaluateSalaryAndReturnName8Employee
.Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, DW.ref.__gxx_personality_v0
	.cfi_lsda 27, .Lexception0
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	pushq	%r15
	.cfi_def_cfa_offset 24
	pushq	%r14
	.cfi_def_cfa_offset 32
	pushq	%r13
	.cfi_def_cfa_offset 40
	pushq	%r12
	.cfi_def_cfa_offset 48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	subq	$88, %rsp
	.cfi_def_cfa_offset 144
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	.cfi_offset %rbp, -16
	movq	%rdi, 16(%rsp)                  # 8-byte Spill
	movq	144(%rsp), %r12
	movq	152(%rsp), %r14
	cmpq	%r14, %r12
	je	.LBB0_1
# %bb.2:
	subq	%r12, %r14
	leaq	1(%r14), %rax
	cmpq	$1, %rax
	movq	%r14, %rdi
	adcq	$1, %rdi
	callq	malloc@PLT
	testq	%rax, %rax
	je	.LBB0_26
# %bb.3:
	movq	%rax, %r15
	movq	%rax, %rdi
	callq	malloc_usable_size@PLT
	movq	%rax, %rbx
	movq	%r15, %rdi
	movq	%r12, %rsi
	movq	%r14, %rdx
	callq	memcpy@PLT
	leaq	(%r15,%r14), %rax
	movb	$0, (%r15,%r14)
	leaq	(%r15,%rbx), %rcx
	decq	%rcx
	movq	%r15, 40(%rsp)
	movq	%rax, 48(%rsp)
	movq	%rcx, 56(%rsp)
	movq	%rax, %rcx
	subq	%r15, %rcx
	cmpq	$3, %rcx
	jne	.LBB0_5
.LBB0_6:
	cmpq	%r15, %rax
	je	.LBB0_7
# %bb.8:
	movzwl	(%r15), %eax
	xorl	$17731, %eax                    # imm = 0x4543
	movzbl	2(%r15), %ecx
	xorl	$79, %ecx
	orw	%ax, %cx
	sete	%bl
	jmp	.LBB0_9
.LBB0_1:
	leaq	64(%rsp), %rax
	movq	%rax, 40(%rsp)
	movq	%rax, 48(%rsp)
	movq	%rax, 56(%rsp)
	movb	$0, 64(%rsp)
	movq	%rax, %r15
	movq	%rax, %rcx
	subq	%r15, %rcx
	cmpq	$3, %rcx
	je	.LBB0_6
.LBB0_5:
	xorl	%ebx, %ebx
	jmp	.LBB0_9
.LBB0_7:
	movb	$1, %bl
.LBB0_9:
	leaq	144(%rsp), %r14
	movq	%r15, %rdi
	callq	free@PLT
	testb	%bl, %bl
	jne	.LBB0_11
# %bb.10:
	cmpq	$100001, 32(%r14)               # imm = 0x186A1
	jl	.LBB0_12
.LBB0_11:
	movq	40(%r14), %rsi
	movq	48(%r14), %rdx
	subq	%rsi, %rdx
	movq	72(%r14), %r8
	movq	80(%r14), %r9
	subq	%r8, %r9
	leaq	.L.str.2(%rip), %rax
	movq	%rax, 72(%rsp)
	movq	$13, 80(%rsp)
	movq	stdout@GOTPCREL(%rip), %rax
	movq	(%rax), %rdi
.Ltmp0:
	movups	72(%rsp), %xmm0
	movups	%xmm0, (%rsp)
	movl	$32, %ecx
	callq	_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_
.Ltmp1:
.LBB0_12:
	movq	40(%r14), %rbp
	movq	48(%r14), %r12
	movq	%r12, %r13
	subq	%rbp, %r13
	movq	72(%r14), %rdx
	movq	80(%r14), %rsi
	movq	%rsi, %r15
	subq	%rdx, %r15
	movq	16(%rsp), %rcx                  # 8-byte Reload
	leaq	24(%rcx), %rax
	movq	%rax, (%rcx)
	movq	%rax, 8(%rcx)
	movq	%rax, 16(%rcx)
	movb	$0, 24(%rcx)
	movq	%r13, %rdi
	addq	%r15, %rdi
	jb	.LBB0_26
# %bb.13:
	incq	%rdi
	je	.LBB0_26
# %bb.14:
	movq	%rsi, 24(%rsp)                  # 8-byte Spill
	movq	%rdx, 32(%rsp)                  # 8-byte Spill
	leaq	1(%rdi), %rax
	cmpq	$1, %rax
	adcq	$1, %rdi
	callq	malloc@PLT
	testq	%rax, %rax
	je	.LBB0_26
# %bb.15:
	movq	%rax, %rbx
	movq	%rax, %rdi
	callq	malloc_usable_size@PLT
	movb	$0, (%rbx)
	addq	%rbx, %rax
	decq	%rax
	movq	16(%rsp), %rcx                  # 8-byte Reload
	movq	%rbx, (%rcx)
	movq	%rax, 16(%rcx)
	cmpq	%rbp, %r12
	je	.LBB0_17
# %bb.16:
	movq	%rbx, %rdi
	movq	%rbp, %rsi
	movq	%r13, %rdx
	callq	memcpy@PLT
.LBB0_17:
	movb	$32, (%rbx,%r13)
	addq	%r13, %rbx
	incq	%rbx
	movq	32(%rsp), %rsi                  # 8-byte Reload
	cmpq	%rsi, 24(%rsp)                  # 8-byte Folded Reload
	je	.LBB0_19
# %bb.18:
	movq	%rbx, %rdi
	movq	%r15, %rdx
	callq	memcpy@PLT
.LBB0_19:
	leaq	(%rbx,%r15), %rax
	movq	16(%rsp), %rcx                  # 8-byte Reload
	movq	%rax, 8(%rcx)
	movb	$0, (%rbx,%r15)
	movq	72(%r14), %rdi
	leaq	96(%r14), %rax
	cmpq	%rax, %rdi
	sete	%al
	testq	%rdi, %rdi
	sete	%cl
	orb	%al, %cl
	jne	.LBB0_21
# %bb.20:
	callq	free@PLT
.LBB0_21:
	movq	40(%r14), %rdi
	leaq	64(%r14), %rax
	cmpq	%rax, %rdi
	sete	%al
	testq	%rdi, %rdi
	sete	%cl
	orb	%al, %cl
	jne	.LBB0_23
# %bb.22:
	callq	free@PLT
.LBB0_23:
	movq	(%r14), %rdi
	addq	$24, %r14
	cmpq	%r14, %rdi
	sete	%al
	testq	%rdi, %rdi
	sete	%cl
	orb	%al, %cl
	jne	.LBB0_25
# %bb.24:
	callq	free@PLT
.LBB0_25:
	movq	16(%rsp), %rax                  # 8-byte Reload
	addq	$88, %rsp
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%r12
	.cfi_def_cfa_offset 40
	popq	%r13
	.cfi_def_cfa_offset 32
	popq	%r14
	.cfi_def_cfa_offset 24
	popq	%r15
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	retq
.LBB0_26:
	.cfi_def_cfa_offset 144
	ud2
.LBB0_27:
.Ltmp2:
	movq	%rax, %rbx
	movq	%r14, %rdi
	callq	_ZN8EmployeeD2Ev
	movq	%rbx, %rdi
	callq	_Unwind_Resume@PLT
.Lfunc_end0:
	.size	_Z27EvaluateSalaryAndReturnName8Employee, .Lfunc_end0-_Z27EvaluateSalaryAndReturnName8Employee
	.cfi_endproc
	.section	.gcc_except_table,"a",@progbits
	.p2align	2, 0x0
GCC_except_table0:
.Lexception0:
	.byte	255                             # @LPStart Encoding = omit
	.byte	255                             # @TType Encoding = omit
	.byte	1                               # Call site Encoding = uleb128
	.uleb128 .Lcst_end0-.Lcst_begin0
.Lcst_begin0:
	.uleb128 .Lfunc_begin0-.Lfunc_begin0    # >> Call Site 1 <<
	.uleb128 .Ltmp0-.Lfunc_begin0           #   Call between .Lfunc_begin0 and .Ltmp0
	.byte	0                               #     has no landing pad
	.byte	0                               #   On action: cleanup
	.uleb128 .Ltmp0-.Lfunc_begin0           # >> Call Site 2 <<
	.uleb128 .Ltmp1-.Ltmp0                  #   Call between .Ltmp0 and .Ltmp1
	.uleb128 .Ltmp2-.Lfunc_begin0           #     jumps to .Ltmp2
	.byte	0                               #   On action: cleanup
	.uleb128 .Ltmp1-.Lfunc_begin0           # >> Call Site 3 <<
	.uleb128 .Lfunc_end0-.Ltmp1             #   Call between .Ltmp1 and .Lfunc_end0
	.byte	0                               #     has no landing pad
	.byte	0                               #   On action: cleanup
.Lcst_end0:
	.p2align	2, 0x0
                                        # -- End function
	.section	.text._ZN8EmployeeD2Ev,"axG",@progbits,_ZN8EmployeeD2Ev,comdat
	.weak	_ZN8EmployeeD2Ev                # -- Begin function _ZN8EmployeeD2Ev
	.p2align	4, 0x90
	.type	_ZN8EmployeeD2Ev,@function
_ZN8EmployeeD2Ev:                       # @_ZN8EmployeeD2Ev
	.cfi_startproc
# %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset %rbx, -16
	movq	%rdi, %rbx
	movq	72(%rdi), %rdi
	leaq	96(%rbx), %rax
	cmpq	%rax, %rdi
	sete	%al
	testq	%rdi, %rdi
	sete	%cl
	orb	%al, %cl
	jne	.LBB1_2
# %bb.1:
	callq	free@PLT
.LBB1_2:
	movq	40(%rbx), %rdi
	leaq	64(%rbx), %rax
	cmpq	%rax, %rdi
	sete	%al
	testq	%rdi, %rdi
	sete	%cl
	orb	%al, %cl
	jne	.LBB1_4
# %bb.3:
	callq	free@PLT
.LBB1_4:
	movq	(%rbx), %rdi
	addq	$24, %rbx
	cmpq	%rbx, %rdi
	sete	%al
	testq	%rdi, %rdi
	sete	%cl
	orb	%al, %cl
	je	.LBB1_6
# %bb.5:
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.LBB1_6:
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	jmp	free@PLT                        # TAILCALL
.Lfunc_end1:
	.size	_ZN8EmployeeD2Ev, .Lfunc_end1-_ZN8EmployeeD2Ev
	.cfi_endproc
                                        # -- End function
	.text
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	xorl	%eax, %eax
	retq
.Lfunc_end2:
	.size	main, .Lfunc_end2-main
	.cfi_endproc
                                        # -- End function
	.section	.text._ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_,"axG",@progbits,_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_,comdat
	.weak	_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_ # -- Begin function _ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_
	.p2align	4, 0x90
	.type	_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_,@function
_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_: # @_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_
.Lfunc_begin1:
	.cfi_startproc
	.cfi_personality 155, DW.ref.__gxx_personality_v0
	.cfi_lsda 27, .Lexception1
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	pushq	%r15
	.cfi_def_cfa_offset 24
	pushq	%r14
	.cfi_def_cfa_offset 32
	pushq	%r13
	.cfi_def_cfa_offset 40
	pushq	%r12
	.cfi_def_cfa_offset 48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	subq	$88, %rsp
	.cfi_def_cfa_offset 144
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	.cfi_offset %rbp, -16
	movq	%r9, %r12
	movq	%r8, 24(%rsp)                   # 8-byte Spill
	movl	%ecx, 12(%rsp)                  # 4-byte Spill
	movq	%rdx, %r13
	movq	%rsi, %r15
	movq	%rdi, %rbx
	callq	flockfile@PLT
	movq	144(%rsp), %rax
	movq	%rax, 16(%rsp)                  # 8-byte Spill
	movq	152(%rsp), %r14
	movq	%r15, 64(%rsp)
	movq	%r13, 72(%rsp)
	movq	40(%rbx), %rbp
	movq	48(%rbx), %rax
	subq	%rbp, %rax
	xorl	%ecx, %ecx
	testq	%rax, %rax
	cmovgq	%rax, %rcx
	cmpq	%rcx, %r13
	jae	.LBB3_4
# %bb.1:
	testq	%r13, %r13
	je	.LBB3_3
# %bb.2:
	movq	%rbp, %rdi
	movq	%r15, %rsi
	movq	%r13, %rdx
	callq	memcpy@PLT
.LBB3_3:
	addq	%r13, %rbp
	movq	%rbp, 40(%rbx)
	movq	16(%rsp), %r13                  # 8-byte Reload
	movq	48(%rbx), %rax
	subq	%rbp, %rax
	cmpq	$2, %rax
	jl	.LBB3_8
.LBB3_7:
	movl	12(%rsp), %eax                  # 4-byte Reload
	movb	%al, (%rbp)
	incq	%rbp
	movq	%rbp, 40(%rbx)
.LBB3_10:
	movq	24(%rsp), %rax                  # 8-byte Reload
	movq	%rax, 32(%rsp)
	movq	%r12, 40(%rsp)
	movq	%r13, 48(%rsp)
	movq	%r14, 56(%rsp)
	movq	48(%rbx), %rax
	subq	%rbp, %rax
	xorl	%r15d, %r15d
	testq	%rax, %rax
	cmovgq	%rax, %r15
	subq	%r12, %r15
	jbe	.LBB3_11
# %bb.13:
	testq	%r12, %r12
	je	.LBB3_15
# %bb.14:
	movq	%rbp, %rdi
	movq	24(%rsp), %rsi                  # 8-byte Reload
	movq	%r12, %rdx
	callq	memcpy@PLT
.LBB3_15:
	addq	%r12, %rbp
	cmpq	%r15, %r14
	jae	.LBB3_16
# %bb.17:
	testq	%r14, %r14
	je	.LBB3_19
# %bb.18:
	movq	%rbp, %rdi
	movq	%r13, %rsi
	movq	%r14, %rdx
	callq	memcpy@PLT
.LBB3_19:
	addq	%r14, %rbp
	movq	%rbp, 40(%rbx)
.LBB3_20:
	movq	%rbx, %rdi
	callq	funlockfile@PLT
	addq	$88, %rsp
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%r12
	.cfi_def_cfa_offset 40
	popq	%r13
	.cfi_def_cfa_offset 32
	popq	%r14
	.cfi_def_cfa_offset 24
	popq	%r15
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	retq
.LBB3_4:
	.cfi_def_cfa_offset 144
.Ltmp3:
	leaq	64(%rsp), %rsi
	movl	$1, %edx
	movq	%rbx, %rdi
	callq	_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm
.Ltmp4:
	movq	16(%rsp), %r13                  # 8-byte Reload
# %bb.5:
	movq	40(%rbx), %rbp
	movq	48(%rbx), %rax
	subq	%rbp, %rax
	cmpq	$2, %rax
	jge	.LBB3_7
.LBB3_8:
	movl	12(%rsp), %eax                  # 4-byte Reload
	movb	%al, 32(%rsp)
	leaq	33(%rsp), %rdx
.Ltmp5:
	leaq	32(%rsp), %rsi
	movq	%rbx, %rdi
	callq	_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_
.Ltmp6:
# %bb.9:
	movq	40(%rbx), %rbp
	jmp	.LBB3_10
.LBB3_11:
	leaq	32(%rsp), %rsi
	movl	$2, %edx
	jmp	.LBB3_12
.LBB3_16:
	leaq	48(%rsp), %rsi
	movl	$1, %edx
.LBB3_12:
	movq	%rbp, 40(%rbx)
.Ltmp7:
	movq	%rbx, %rdi
	callq	_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm
.Ltmp8:
	jmp	.LBB3_20
.LBB3_21:
.Ltmp9:
	movq	%rax, %r14
	movq	%rbx, %rdi
	callq	funlockfile@PLT
	movq	%r14, %rdi
	callq	_Unwind_Resume@PLT
.Lfunc_end3:
	.size	_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_, .Lfunc_end3-_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_
	.cfi_endproc
	.section	.gcc_except_table._ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_,"aG",@progbits,_ZN7fast_io10operations5decay24print_freestanding_decayILb0ENS_26basic_c_family_io_observerILNS_8c_familyE0EcEEJNS_18basic_io_scatter_tIcEENS_12manipulators6chvw_tIcEES7_S7_EEEDcT0_DpT1_,comdat
	.p2align	2, 0x0
GCC_except_table3:
.Lexception1:
	.byte	255                             # @LPStart Encoding = omit
	.byte	255                             # @TType Encoding = omit
	.byte	1                               # Call site Encoding = uleb128
	.uleb128 .Lcst_end1-.Lcst_begin1
.Lcst_begin1:
	.uleb128 .Lfunc_begin1-.Lfunc_begin1    # >> Call Site 1 <<
	.uleb128 .Ltmp3-.Lfunc_begin1           #   Call between .Lfunc_begin1 and .Ltmp3
	.byte	0                               #     has no landing pad
	.byte	0                               #   On action: cleanup
	.uleb128 .Ltmp3-.Lfunc_begin1           # >> Call Site 2 <<
	.uleb128 .Ltmp8-.Ltmp3                  #   Call between .Ltmp3 and .Ltmp8
	.uleb128 .Ltmp9-.Lfunc_begin1           #     jumps to .Ltmp9
	.byte	0                               #   On action: cleanup
	.uleb128 .Ltmp8-.Lfunc_begin1           # >> Call Site 3 <<
	.uleb128 .Lfunc_end3-.Ltmp8             #   Call between .Ltmp8 and .Lfunc_end3
	.byte	0                               #     has no landing pad
	.byte	0                               #   On action: cleanup
.Lcst_end1:
	.p2align	2, 0x0
                                        # -- End function
	.section	.text.unlikely._ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm,"axG",@progbits,_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm,comdat
	.weak	_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm # -- Begin function _ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm
	.type	_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm,@function
_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm: # @_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm
	.cfi_startproc
# %bb.0:
	testq	%rdx, %rdx
	je	.LBB4_9
# %bb.1:
	pushq	%r15
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%r13
	.cfi_def_cfa_offset 32
	pushq	%r12
	.cfi_def_cfa_offset 40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset %rbx, -48
	.cfi_offset %r12, -40
	.cfi_offset %r13, -32
	.cfi_offset %r14, -24
	.cfi_offset %r15, -16
	movq	%rdx, %rbx
	movq	%rsi, %r14
	movq	%rdi, %r15
	shlq	$4, %rbx
	addq	%rsi, %rbx
.LBB4_2:                                # =>This Inner Loop Header: Depth=1
	movq	(%r14), %rsi
	movq	8(%r14), %r12
	movq	40(%r15), %r13
	movq	48(%r15), %rax
	subq	%r13, %rax
	cmpq	%rax, %r12
	jge	.LBB4_6
# %bb.3:                                #   in Loop: Header=BB4_2 Depth=1
	testq	%r12, %r12
	je	.LBB4_5
# %bb.4:                                #   in Loop: Header=BB4_2 Depth=1
	movq	%r13, %rdi
	movq	%r12, %rdx
	callq	memcpy@PLT
.LBB4_5:                                #   in Loop: Header=BB4_2 Depth=1
	addq	%r12, %r13
	movq	%r13, 40(%r15)
.LBB4_7:                                #   in Loop: Header=BB4_2 Depth=1
	addq	$16, %r14
	cmpq	%rbx, %r14
	jne	.LBB4_2
	jmp	.LBB4_8
.LBB4_6:                                #   in Loop: Header=BB4_2 Depth=1
	addq	%rsi, %r12
	movq	%r15, %rdi
	movq	%r12, %rdx
	callq	_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_
	jmp	.LBB4_7
.LBB4_8:
	popq	%rbx
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	.cfi_restore %rbx
	.cfi_restore %r12
	.cfi_restore %r13
	.cfi_restore %r14
	.cfi_restore %r15
.LBB4_9:
	retq
.Lfunc_end4:
	.size	_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm, .Lfunc_end4-_ZN7fast_io7details33scatter_write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKNS_18basic_io_scatter_tIvEEm
	.cfi_endproc
                                        # -- End function
	.section	.text.unlikely._ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_,"axG",@progbits,_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_,comdat
	.weak	_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_ # -- Begin function _ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_
	.type	_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_,@function
_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_: # @_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_
	.cfi_startproc
# %bb.0:
	pushq	%r15
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%r13
	.cfi_def_cfa_offset 32
	pushq	%r12
	.cfi_def_cfa_offset 40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset %rbx, -48
	.cfi_offset %r12, -40
	.cfi_offset %r13, -32
	.cfi_offset %r14, -24
	.cfi_offset %r15, -16
	movq	%rdx, %r15
	movq	%rsi, %r14
	movq	%rdi, %rbx
.LBB5_1:                                # =>This Inner Loop Header: Depth=1
	movq	%r15, %rdx
	subq	%r14, %rdx
	je	.LBB5_2
# %bb.3:                                #   in Loop: Header=BB5_1 Depth=1
	movl	$1, %esi
	movq	%r14, %rdi
	movq	%rbx, %rcx
	callq	fwrite_unlocked@PLT
	testq	%rax, %rax
	jne	.LBB5_4
	jmp	.LBB5_8
.LBB5_2:                                #   in Loop: Header=BB5_1 Depth=1
	xorl	%eax, %eax
.LBB5_4:                                #   in Loop: Header=BB5_1 Depth=1
	addq	%rax, %r14
	cmpq	%r15, %r14
	je	.LBB5_7
# %bb.5:                                #   in Loop: Header=BB5_1 Depth=1
	movq	40(%rbx), %r12
	movq	48(%rbx), %rax
	subq	%r12, %rax
	movq	%r15, %r13
	subq	%r14, %r13
	cmpq	%rax, %r13
	jge	.LBB5_1
# %bb.6:
	movq	%r12, %rdi
	movq	%r14, %rsi
	movq	%r13, %rdx
	callq	memcpy@PLT
	addq	%r13, %r12
	movq	%r12, 40(%rbx)
.LBB5_7:
	popq	%rbx
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	retq
.LBB5_8:
	.cfi_def_cfa_offset 48
	callq	_ZN7fast_io17throw_posix_errorEv
.Lfunc_end5:
	.size	_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_, .Lfunc_end5-_ZN7fast_io7details25write_all_bytes_cold_implINS_26basic_c_family_io_observerILNS_8c_familyE1EcEEEEvT_PKSt4byteS8_
	.cfi_endproc
                                        # -- End function
	.section	.text._ZN7fast_io17throw_posix_errorEv,"axG",@progbits,_ZN7fast_io17throw_posix_errorEv,comdat
	.weak	_ZN7fast_io17throw_posix_errorEv # -- Begin function _ZN7fast_io17throw_posix_errorEv
	.p2align	4, 0x90
	.type	_ZN7fast_io17throw_posix_errorEv,@function
_ZN7fast_io17throw_posix_errorEv:       # @_ZN7fast_io17throw_posix_errorEv
	.cfi_startproc
# %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset %rbx, -16
	movl	$16, %edi
	callq	__cxa_allocate_exception@PLT
	movq	%rax, %rbx
	movabsq	$-7308014004222350952, %rax     # imm = 0x9A94B7FB90BF2998
	movq	%rax, (%rbx)
	callq	__errno_location@PLT
	movl	(%rax), %eax
	movq	%rax, 8(%rbx)
	leaq	_ZTIN7fast_io5errorE(%rip), %rsi
	movq	%rbx, %rdi
	xorl	%edx, %edx
	callq	__cxa_throw@PLT
.Lfunc_end6:
	.size	_ZN7fast_io17throw_posix_errorEv, .Lfunc_end6-_ZN7fast_io17throw_posix_errorEv
	.cfi_endproc
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

	.type	_ZTSN7fast_io5errorE,@object    # @_ZTSN7fast_io5errorE
	.section	.rodata._ZTSN7fast_io5errorE,"aG",@progbits,_ZTSN7fast_io5errorE,comdat
	.weak	_ZTSN7fast_io5errorE
_ZTSN7fast_io5errorE:
	.asciz	"N7fast_io5errorE"
	.size	_ZTSN7fast_io5errorE, 17

	.type	_ZTIN7fast_io5errorE,@object    # @_ZTIN7fast_io5errorE
	.section	.data.rel.ro._ZTIN7fast_io5errorE,"awG",@progbits,_ZTIN7fast_io5errorE,comdat
	.weak	_ZTIN7fast_io5errorE
	.p2align	3, 0x0
_ZTIN7fast_io5errorE:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSN7fast_io5errorE
	.size	_ZTIN7fast_io5errorE, 16

	.section	".linker-options","e",@llvm_linker_options
	.hidden	DW.ref.__gxx_personality_v0
	.weak	DW.ref.__gxx_personality_v0
	.section	.data.DW.ref.__gxx_personality_v0,"awG",@progbits,DW.ref.__gxx_personality_v0,comdat
	.p2align	3, 0x0
	.type	DW.ref.__gxx_personality_v0,@object
	.size	DW.ref.__gxx_personality_v0, 8
DW.ref.__gxx_personality_v0:
	.quad	__gxx_personality_v0
	.ident	"clang version 20.0.0git (git@github.com:trcrsired/llvm-project.git 3b9afec33ad48c7b4ddfc51cf9e1b286ccf9ac5c)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym __gxx_personality_v0
	.addrsig_sym _Unwind_Resume
	.addrsig_sym _ZTVN10__cxxabiv117__class_type_infoE
	.addrsig_sym _ZTSN7fast_io5errorE
	.addrsig_sym _ZTIN7fast_io5errorE
