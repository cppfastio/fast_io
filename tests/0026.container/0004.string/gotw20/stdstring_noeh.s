	.text
	.file	"stdstring.cc"
                                        # Start of file scope inline assembly
	.globl	_ZSt21ios_base_library_initv

                                        # End of file scope inline assembly
	.globl	_Z27EvaluateSalaryAndReturnNameB5cxx118Employee # -- Begin function _Z27EvaluateSalaryAndReturnNameB5cxx118Employee
	.p2align	4, 0x90
	.type	_Z27EvaluateSalaryAndReturnNameB5cxx118Employee,@function
_Z27EvaluateSalaryAndReturnNameB5cxx118Employee: # @_Z27EvaluateSalaryAndReturnNameB5cxx118Employee
# %bb.0:
	pushq	%r15
	pushq	%r14
	pushq	%r12
	pushq	%rbx
	subq	$40, %rsp
	movq	%rsi, %r14
	movq	%rdi, %rbx
	cmpq	$3, 8(%rsi)
	jne	.LBB0_2
# %bb.1:
	movq	(%r14), %rax
	movzwl	(%rax), %ecx
	xorl	$17731, %ecx                    # imm = 0x4543
	movzbl	2(%rax), %eax
	xorl	$79, %eax
	orw	%cx, %ax
	je	.LBB0_3
.LBB0_2:
	cmpq	$100001, 32(%r14)               # imm = 0x186A1
	jl	.LBB0_8
.LBB0_3:
	movq	40(%r14), %rsi
	movq	48(%r14), %rdx
	movq	_ZSt4cout@GOTPCREL(%rip), %rdi
	callq	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movq	%rax, %r15
	leaq	.L.str.1(%rip), %rsi
	movl	$1, %edx
	movq	%rax, %rdi
	callq	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movq	72(%r14), %rsi
	movq	80(%r14), %rdx
	movq	%r15, %rdi
	callq	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movq	%rax, %r15
	leaq	.L.str.2(%rip), %rsi
	movl	$12, %edx
	movq	%rax, %rdi
	callq	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movq	(%r15), %rax
	movq	-24(%rax), %rax
	movq	240(%r15,%rax), %r12
	testq	%r12, %r12
	je	.LBB0_21
# %bb.4:
	cmpb	$0, 56(%r12)
	je	.LBB0_6
# %bb.5:
	movzbl	67(%r12), %eax
	jmp	.LBB0_7
.LBB0_6:
	movq	%r12, %rdi
	callq	_ZNKSt5ctypeIcE13_M_widen_initEv@PLT
	movq	(%r12), %rax
	movq	%r12, %rdi
	movl	$10, %esi
	callq	*48(%rax)
.LBB0_7:
	movsbl	%al, %esi
	movq	%r15, %rdi
	callq	_ZNSo3putEc@PLT
	movq	%rax, %rdi
	callq	_ZNSo5flushEv@PLT
.LBB0_8:
	movq	40(%r14), %rsi
	movq	48(%r14), %rdx
	leaq	.L.str.1(%rip), %rcx
	leaq	8(%rsp), %rdi
	leaq	7(%rsp), %r9
	movl	$1, %r8d
	callq	_ZSt12__str_concatINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEET_PKNS6_10value_typeENS6_9size_typeES9_SA_RKNS6_14allocator_typeE
	movq	80(%r14), %r8
	movq	16(%rsp), %rsi
	movabsq	$9223372036854775807, %rax      # imm = 0x7FFFFFFFFFFFFFFF
	xorq	%rsi, %rax
	cmpq	%r8, %rax
	jb	.LBB0_22
# %bb.9:
	leaq	(%rsi,%r8), %r12
	movq	8(%rsp), %rdi
	leaq	24(%rsp), %r15
	movl	$15, %eax
	cmpq	%r15, %rdi
	je	.LBB0_11
# %bb.10:
	movq	24(%rsp), %rax
.LBB0_11:
	movq	72(%r14), %rcx
	cmpq	%rax, %r12
	jbe	.LBB0_12
# %bb.16:
	leaq	8(%rsp), %rdi
	xorl	%edx, %edx
	callq	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_mutateEmmPKcm
	jmp	.LBB0_17
.LBB0_12:
	testq	%r8, %r8
	je	.LBB0_17
# %bb.13:
	addq	%rsi, %rdi
	cmpq	$1, %r8
	jne	.LBB0_15
# %bb.14:
	movzbl	(%rcx), %eax
	movb	%al, (%rdi)
	jmp	.LBB0_17
.LBB0_15:
	movq	%rcx, %rsi
	movq	%r8, %rdx
	callq	memcpy@PLT
.LBB0_17:
	movq	%r12, 16(%rsp)
	movq	8(%rsp), %rax
	movb	$0, (%rax,%r12)
	leaq	16(%rbx), %rdi
	movq	%rdi, (%rbx)
	movq	8(%rsp), %rax
	cmpq	%r15, %rax
	je	.LBB0_18
# %bb.19:
	movq	%rax, (%rbx)
	movq	24(%rsp), %rax
	movq	%rax, 16(%rbx)
	movq	16(%rsp), %r14
	jmp	.LBB0_20
.LBB0_18:
	movq	16(%rsp), %r14
	leaq	1(%r14), %rdx
	movq	%r15, %rsi
	callq	memcpy@PLT
.LBB0_20:
	movq	%r14, 8(%rbx)
	movq	%rbx, %rax
	addq	$40, %rsp
	popq	%rbx
	popq	%r12
	popq	%r14
	popq	%r15
	retq
.LBB0_22:
	leaq	.L.str.3(%rip), %rdi
	callq	_ZSt20__throw_length_errorPKc@PLT
.LBB0_21:
	callq	_ZSt16__throw_bad_castv@PLT
.Lfunc_end0:
	.size	_Z27EvaluateSalaryAndReturnNameB5cxx118Employee, .Lfunc_end0-_Z27EvaluateSalaryAndReturnNameB5cxx118Employee
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
	.section	.text._ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_mutateEmmPKcm,"axG",@progbits,_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_mutateEmmPKcm,comdat
	.weak	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_mutateEmmPKcm # -- Begin function _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_mutateEmmPKcm
	.p2align	4, 0x90
	.type	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_mutateEmmPKcm,@function
_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_mutateEmmPKcm: # @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_mutateEmmPKcm
# %bb.0:
	pushq	%rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$56, %rsp
	movq	%rcx, 16(%rsp)                  # 8-byte Spill
	movq	(%rdi), %r15
	movq	8(%rdi), %r13
	movq	%r8, %rbp
	movq	%rdx, 32(%rsp)                  # 8-byte Spill
	subq	%rdx, %rbp
	leaq	16(%rdi), %rdx
	cmpq	%rdx, %r15
	movq	16(%rdi), %rcx
	movl	$15, %eax
	cmovneq	%rcx, %rax
	addq	%r13, %rbp
	js	.LBB2_21
# %bb.1:
	movq	%r8, %r14
	movq	%rsi, %r12
	movq	%rdi, %rbx
	cmpq	%rax, %rbp
	movq	%rcx, 40(%rsp)                  # 8-byte Spill
	movq	%rdx, 48(%rsp)                  # 8-byte Spill
	jbe	.LBB2_4
# %bb.2:
	addq	%rax, %rax
	cmpq	%rax, %rbp
	jae	.LBB2_4
# %bb.3:
	movabsq	$9223372036854775807, %rbp      # imm = 0x7FFFFFFFFFFFFFFF
	cmpq	%rbp, %rax
	cmovbq	%rax, %rbp
.LBB2_4:
	movq	%rbp, %rdi
	incq	%rdi
	js	.LBB2_22
# %bb.5:
	callq	_Znwm@PLT
	movq	%rax, %rdx
	testq	%r12, %r12
	movq	%rax, 8(%rsp)                   # 8-byte Spill
	je	.LBB2_9
# %bb.6:
	cmpq	$1, %r12
	jne	.LBB2_8
# %bb.7:
	movzbl	(%r15), %eax
	movb	%al, (%rdx)
	jmp	.LBB2_9
.LBB2_8:
	movq	%rdx, %rdi
	movq	%r15, %rsi
	movq	%r12, %rdx
	callq	memcpy@PLT
	movq	8(%rsp), %rdx                   # 8-byte Reload
.LBB2_9:
	movq	%r15, 24(%rsp)                  # 8-byte Spill
	movq	32(%rsp), %rax                  # 8-byte Reload
	leaq	(%rax,%r12), %r15
	cmpq	$0, 16(%rsp)                    # 8-byte Folded Reload
	sete	%al
	testq	%r14, %r14
	sete	%cl
	orb	%al, %cl
	jne	.LBB2_13
# %bb.10:
	leaq	(%rdx,%r12), %rdi
	cmpq	$1, %r14
	jne	.LBB2_12
# %bb.11:
	movq	16(%rsp), %rax                  # 8-byte Reload
	movzbl	(%rax), %eax
	movb	%al, (%rdi)
.LBB2_13:
	cmpq	%r15, %r13
	je	.LBB2_18
.LBB2_14:
	subq	%r15, %r13
	je	.LBB2_18
# %bb.15:
	movq	%rdx, %rdi
	addq	%r12, %rdi
	addq	%r14, %rdi
	addq	24(%rsp), %r12                  # 8-byte Folded Reload
	addq	32(%rsp), %r12                  # 8-byte Folded Reload
	cmpq	$1, %r13
	jne	.LBB2_17
# %bb.16:
	movzbl	(%r12), %eax
	movb	%al, (%rdi)
	movq	24(%rsp), %rdi                  # 8-byte Reload
	cmpq	48(%rsp), %rdi                  # 8-byte Folded Reload
	jne	.LBB2_19
	jmp	.LBB2_20
.LBB2_12:
	movq	16(%rsp), %rsi                  # 8-byte Reload
	movq	%r14, %rdx
	callq	memcpy@PLT
	movq	8(%rsp), %rdx                   # 8-byte Reload
	cmpq	%r15, %r13
	jne	.LBB2_14
.LBB2_18:
	movq	24(%rsp), %rdi                  # 8-byte Reload
	cmpq	48(%rsp), %rdi                  # 8-byte Folded Reload
	je	.LBB2_20
.LBB2_19:
	movq	40(%rsp), %rsi                  # 8-byte Reload
	incq	%rsi
	callq	_ZdlPvm@PLT
	movq	8(%rsp), %rdx                   # 8-byte Reload
.LBB2_20:
	movq	%rdx, (%rbx)
	movq	%rbp, 16(%rbx)
	addq	$56, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
.LBB2_17:
	movq	%r12, %rsi
	movq	%r13, %rdx
	callq	memcpy@PLT
	movq	8(%rsp), %rdx                   # 8-byte Reload
	movq	24(%rsp), %rdi                  # 8-byte Reload
	cmpq	48(%rsp), %rdi                  # 8-byte Folded Reload
	jne	.LBB2_19
	jmp	.LBB2_20
.LBB2_22:
	callq	_ZSt17__throw_bad_allocv@PLT
.LBB2_21:
	leaq	.L.str.4(%rip), %rdi
	callq	_ZSt20__throw_length_errorPKc@PLT
.Lfunc_end2:
	.size	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_mutateEmmPKcm, .Lfunc_end2-_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_mutateEmmPKcm
                                        # -- End function
	.section	.text._ZSt12__str_concatINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEET_PKNS6_10value_typeENS6_9size_typeES9_SA_RKNS6_14allocator_typeE,"axG",@progbits,_ZSt12__str_concatINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEET_PKNS6_10value_typeENS6_9size_typeES9_SA_RKNS6_14allocator_typeE,comdat
	.weak	_ZSt12__str_concatINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEET_PKNS6_10value_typeENS6_9size_typeES9_SA_RKNS6_14allocator_typeE # -- Begin function _ZSt12__str_concatINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEET_PKNS6_10value_typeENS6_9size_typeES9_SA_RKNS6_14allocator_typeE
	.p2align	4, 0x90
	.type	_ZSt12__str_concatINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEET_PKNS6_10value_typeENS6_9size_typeES9_SA_RKNS6_14allocator_typeE,@function
_ZSt12__str_concatINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEET_PKNS6_10value_typeENS6_9size_typeES9_SA_RKNS6_14allocator_typeE: # @_ZSt12__str_concatINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEET_PKNS6_10value_typeENS6_9size_typeES9_SA_RKNS6_14allocator_typeE
# %bb.0:
	pushq	%rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$24, %rsp
	movq	%r8, %r14
	movq	%rcx, 8(%rsp)                   # 8-byte Spill
	movq	%rdx, %r12
	movq	%rsi, 16(%rsp)                  # 8-byte Spill
	movq	%rdi, %rbx
	movabsq	$9223372036854775807, %r15      # imm = 0x7FFFFFFFFFFFFFFF
	leaq	16(%rdi), %rbp
	movq	%rbp, (%rdi)
	movq	$0, 8(%rdi)
	movb	$0, 16(%rdi)
	leaq	(%r8,%rdx), %rsi
	callq	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7reserveEm
	movq	8(%rbx), %rsi
	movq	%r15, %rax
	subq	%rsi, %rax
	cmpq	%r12, %rax
	jb	.LBB3_19
# %bb.1:
	leaq	(%rsi,%r12), %r13
	movq	(%rbx), %rdi
	movl	$15, %eax
	cmpq	%rbp, %rdi
	je	.LBB3_3
# %bb.2:
	movq	(%rbp), %rax
.LBB3_3:
	cmpq	%rax, %r13
	jbe	.LBB3_4
# %bb.8:
	movq	%rbx, %rdi
	xorl	%edx, %edx
	movq	16(%rsp), %rcx                  # 8-byte Reload
	movq	%r12, %r8
	callq	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_mutateEmmPKcm
	jmp	.LBB3_9
.LBB3_4:
	testq	%r12, %r12
	je	.LBB3_9
# %bb.5:
	addq	%rsi, %rdi
	cmpq	$1, %r12
	jne	.LBB3_7
# %bb.6:
	movq	16(%rsp), %rax                  # 8-byte Reload
	movzbl	(%rax), %eax
	movb	%al, (%rdi)
	jmp	.LBB3_9
.LBB3_7:
	movq	16(%rsp), %rsi                  # 8-byte Reload
	movq	%r12, %rdx
	callq	memcpy@PLT
.LBB3_9:
	movq	%r13, 8(%rbx)
	movq	(%rbx), %rax
	movb	$0, (%rax,%r13)
	movq	8(%rbx), %rsi
	subq	%rsi, %r15
	cmpq	%r14, %r15
	jb	.LBB3_19
# %bb.10:
	leaq	(%rsi,%r14), %r15
	movq	(%rbx), %rdi
	movl	$15, %eax
	cmpq	%rbp, %rdi
	je	.LBB3_12
# %bb.11:
	movq	(%rbp), %rax
.LBB3_12:
	cmpq	%rax, %r15
	jbe	.LBB3_13
# %bb.17:
	movq	%rbx, %rdi
	xorl	%edx, %edx
	movq	8(%rsp), %rcx                   # 8-byte Reload
	movq	%r14, %r8
	callq	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_mutateEmmPKcm
	jmp	.LBB3_18
.LBB3_13:
	testq	%r14, %r14
	je	.LBB3_18
# %bb.14:
	addq	%rsi, %rdi
	cmpq	$1, %r14
	jne	.LBB3_16
# %bb.15:
	movq	8(%rsp), %rax                   # 8-byte Reload
	movzbl	(%rax), %eax
	movb	%al, (%rdi)
	jmp	.LBB3_18
.LBB3_16:
	movq	8(%rsp), %rsi                   # 8-byte Reload
	movq	%r14, %rdx
	callq	memcpy@PLT
.LBB3_18:
	movq	%r15, 8(%rbx)
	movq	(%rbx), %rax
	movb	$0, (%rax,%r15)
	movq	%rbx, %rax
	addq	$24, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
.LBB3_19:
	leaq	.L.str.3(%rip), %rdi
	callq	_ZSt20__throw_length_errorPKc@PLT
.Lfunc_end3:
	.size	_ZSt12__str_concatINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEET_PKNS6_10value_typeENS6_9size_typeES9_SA_RKNS6_14allocator_typeE, .Lfunc_end3-_ZSt12__str_concatINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEET_PKNS6_10value_typeENS6_9size_typeES9_SA_RKNS6_14allocator_typeE
                                        # -- End function
	.section	.text._ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7reserveEm,"axG",@progbits,_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7reserveEm,comdat
	.weak	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7reserveEm # -- Begin function _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7reserveEm
	.p2align	4, 0x90
	.type	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7reserveEm,@function
_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7reserveEm: # @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7reserveEm
# %bb.0:
	pushq	%rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	pushq	%rax
	movq	(%rdi), %r14
	movq	16(%rdi), %r15
	leaq	16(%rdi), %r13
	cmpq	%r13, %r14
	movl	$15, %eax
	cmovneq	%r15, %rax
	cmpq	%rax, %rsi
	jbe	.LBB4_10
# %bb.1:
	testq	%rsi, %rsi
	js	.LBB4_11
# %bb.2:
	movq	%rdi, %rbx
	addq	%rax, %rax
	movabsq	$9223372036854775807, %rbp      # imm = 0x7FFFFFFFFFFFFFFF
	cmpq	%rbp, %rax
	cmovbq	%rax, %rbp
	cmpq	%rax, %rsi
	cmovaeq	%rsi, %rbp
	movq	%rbp, %rdi
	incq	%rdi
	js	.LBB4_12
# %bb.3:
	callq	_Znwm@PLT
	movq	%rax, %r12
	movq	8(%rbx), %rdx
	cmpq	$-1, %rdx
	je	.LBB4_7
# %bb.4:
	testq	%rdx, %rdx
	jne	.LBB4_6
# %bb.5:
	movzbl	(%r14), %eax
	movb	%al, (%r12)
.LBB4_7:
	cmpq	%r13, %r14
	je	.LBB4_9
.LBB4_8:
	incq	%r15
	movq	%r14, %rdi
	movq	%r15, %rsi
	callq	_ZdlPvm@PLT
.LBB4_9:
	movq	%r12, (%rbx)
	movq	%rbp, 16(%rbx)
.LBB4_10:
	addq	$8, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
.LBB4_6:
	incq	%rdx
	movq	%r12, %rdi
	movq	%r14, %rsi
	callq	memcpy@PLT
	cmpq	%r13, %r14
	jne	.LBB4_8
	jmp	.LBB4_9
.LBB4_12:
	callq	_ZSt17__throw_bad_allocv@PLT
.LBB4_11:
	leaq	.L.str.4(%rip), %rdi
	callq	_ZSt20__throw_length_errorPKc@PLT
.Lfunc_end4:
	.size	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7reserveEm, .Lfunc_end4-_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7reserveEm
                                        # -- End function
	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"CEO"
	.size	.L.str, 4

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.asciz	" "
	.size	.L.str.1, 2

	.type	.L.str.2,@object                # @.str.2
.L.str.2:
	.asciz	" is overpaid"
	.size	.L.str.2, 13

	.type	.L.str.3,@object                # @.str.3
.L.str.3:
	.asciz	"basic_string::append"
	.size	.L.str.3, 21

	.type	.L.str.4,@object                # @.str.4
.L.str.4:
	.asciz	"basic_string::_M_create"
	.size	.L.str.4, 24

	.section	".linker-options","e",@llvm_linker_options
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym _ZSt4cout
