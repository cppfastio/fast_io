	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zmmul1p0"
	.file	"sha512.cc"
	.section	.srodata.cst8,"aM",@progbits,8
	.p2align	3, 0x0                          # -- Begin function _Z14sha512_invokerPmPKSt4byteS2_
.LCPI0_0:
	.quad	4794697086780616226             # 0x428a2f98d728ae22
.LCPI0_1:
	.quad	8158064640168781261             # 0x7137449123ef65cd
.LCPI0_2:
	.quad	-5349999486874862801            # 0xb5c0fbcfec4d3b2f
.LCPI0_3:
	.quad	-1606136188198331460            # 0xe9b5dba58189dbbc
.LCPI0_4:
	.quad	4131703408338449720             # 0x3956c25bf348b538
.LCPI0_5:
	.quad	6480981068601479193             # 0x59f111f1b605d019
.LCPI0_6:
	.quad	-7908458776815382629            # 0x923f82a4af194f9b
.LCPI0_7:
	.quad	-6116909921290321640            # 0xab1c5ed5da6d8118
.LCPI0_8:
	.quad	-2880145864133508542            # 0xd807aa98a3030242
.LCPI0_9:
	.quad	1334009975649890238             # 0x12835b0145706fbe
.LCPI0_10:
	.quad	2608012711638119052             # 0x243185be4ee4b28c
.LCPI0_11:
	.quad	6128411473006802146             # 0x550c7dc3d5ffb4e2
.LCPI0_12:
	.quad	8268148722764581231             # 0x72be5d74f27b896f
.LCPI0_13:
	.quad	-9160688886553864527            # 0x80deb1fe3b1696b1
.LCPI0_14:
	.quad	-7215885187991268811            # 0x9bdc06a725c71235
.LCPI0_15:
	.quad	-4495734319001033068            # 0xc19bf174cf692694
	.text
	.globl	_Z14sha512_invokerPmPKSt4byteS2_
	.p2align	1
	.type	_Z14sha512_invokerPmPKSt4byteS2_,@function
_Z14sha512_invokerPmPKSt4byteS2_:       # @_Z14sha512_invokerPmPKSt4byteS2_
	.cfi_startproc
# %bb.0:
	addi	sp, sp, -432
	.cfi_def_cfa_offset 432
	sd	ra, 424(sp)                     # 8-byte Folded Spill
	sd	s0, 416(sp)                     # 8-byte Folded Spill
	sd	s1, 408(sp)                     # 8-byte Folded Spill
	sd	s2, 400(sp)                     # 8-byte Folded Spill
	sd	s3, 392(sp)                     # 8-byte Folded Spill
	sd	s4, 384(sp)                     # 8-byte Folded Spill
	sd	s5, 376(sp)                     # 8-byte Folded Spill
	sd	s6, 368(sp)                     # 8-byte Folded Spill
	sd	s7, 360(sp)                     # 8-byte Folded Spill
	sd	s8, 352(sp)                     # 8-byte Folded Spill
	sd	s9, 344(sp)                     # 8-byte Folded Spill
	sd	s10, 336(sp)                    # 8-byte Folded Spill
	sd	s11, 328(sp)                    # 8-byte Folded Spill
	.cfi_offset ra, -8
	.cfi_offset s0, -16
	.cfi_offset s1, -24
	.cfi_offset s2, -32
	.cfi_offset s3, -40
	.cfi_offset s4, -48
	.cfi_offset s5, -56
	.cfi_offset s6, -64
	.cfi_offset s7, -72
	.cfi_offset s8, -80
	.cfi_offset s9, -88
	.cfi_offset s10, -96
	.cfi_offset s11, -104
	sd	a2, 168(sp)                     # 8-byte Folded Spill
	bne	a1, a2, .LBB0_1
	j	.LBB0_6
.LBB0_1:
	mv	s7, a1
	ld	a5, 56(a0)
	ld	s0, 48(a0)
	ld	a6, 40(a0)
	ld	t4, 32(a0)
	ld	a7, 24(a0)
	ld	t2, 16(a0)
	ld	t6, 8(a0)
	sd	a0, 8(sp)                       # 8-byte Folded Spill
	ld	s2, 0(a0)
	li	a0, 255
	slli	a0, a0, 24
	sd	a0, 160(sp)                     # 8-byte Folded Spill
	lui	a0, 16
	addiw	a0, a0, -256
	sd	a0, 152(sp)                     # 8-byte Folded Spill
.Lpcrel_hi0:
	auipc	a0, %pcrel_hi(.LCPI0_0)
	ld	a0, %pcrel_lo(.Lpcrel_hi0)(a0)
	sd	a0, 144(sp)                     # 8-byte Folded Spill
.Lpcrel_hi1:
	auipc	a0, %pcrel_hi(.LCPI0_1)
	ld	a0, %pcrel_lo(.Lpcrel_hi1)(a0)
	sd	a0, 136(sp)                     # 8-byte Folded Spill
.Lpcrel_hi2:
	auipc	a0, %pcrel_hi(.LCPI0_2)
	ld	a0, %pcrel_lo(.Lpcrel_hi2)(a0)
	sd	a0, 128(sp)                     # 8-byte Folded Spill
.Lpcrel_hi3:
	auipc	a0, %pcrel_hi(.LCPI0_3)
	ld	a0, %pcrel_lo(.Lpcrel_hi3)(a0)
	sd	a0, 120(sp)                     # 8-byte Folded Spill
.Lpcrel_hi4:
	auipc	a0, %pcrel_hi(.LCPI0_4)
	ld	a0, %pcrel_lo(.Lpcrel_hi4)(a0)
	sd	a0, 112(sp)                     # 8-byte Folded Spill
.Lpcrel_hi5:
	auipc	a0, %pcrel_hi(.LCPI0_5)
	ld	a0, %pcrel_lo(.Lpcrel_hi5)(a0)
	sd	a0, 104(sp)                     # 8-byte Folded Spill
.Lpcrel_hi6:
	auipc	a0, %pcrel_hi(.LCPI0_6)
	ld	a0, %pcrel_lo(.Lpcrel_hi6)(a0)
	sd	a0, 96(sp)                      # 8-byte Folded Spill
.Lpcrel_hi7:
	auipc	a0, %pcrel_hi(.LCPI0_7)
	ld	a0, %pcrel_lo(.Lpcrel_hi7)(a0)
	sd	a0, 88(sp)                      # 8-byte Folded Spill
.Lpcrel_hi8:
	auipc	a0, %pcrel_hi(.LCPI0_8)
	ld	a0, %pcrel_lo(.Lpcrel_hi8)(a0)
	sd	a0, 80(sp)                      # 8-byte Folded Spill
.Lpcrel_hi9:
	auipc	a0, %pcrel_hi(.LCPI0_9)
	ld	a0, %pcrel_lo(.Lpcrel_hi9)(a0)
	sd	a0, 72(sp)                      # 8-byte Folded Spill
.Lpcrel_hi10:
	auipc	a0, %pcrel_hi(.LCPI0_10)
	ld	a0, %pcrel_lo(.Lpcrel_hi10)(a0)
	sd	a0, 64(sp)                      # 8-byte Folded Spill
.Lpcrel_hi11:
	auipc	a0, %pcrel_hi(.LCPI0_11)
	ld	a0, %pcrel_lo(.Lpcrel_hi11)(a0)
	sd	a0, 56(sp)                      # 8-byte Folded Spill
.Lpcrel_hi12:
	auipc	a0, %pcrel_hi(.LCPI0_12)
	ld	a0, %pcrel_lo(.Lpcrel_hi12)(a0)
	sd	a0, 48(sp)                      # 8-byte Folded Spill
.Lpcrel_hi13:
	auipc	a0, %pcrel_hi(.LCPI0_13)
	ld	a0, %pcrel_lo(.Lpcrel_hi13)(a0)
	sd	a0, 40(sp)                      # 8-byte Folded Spill
.Lpcrel_hi14:
	auipc	a0, %pcrel_hi(.LCPI0_14)
	ld	a0, %pcrel_lo(.Lpcrel_hi14)(a0)
	sd	a0, 32(sp)                      # 8-byte Folded Spill
.Lpcrel_hi15:
	auipc	a0, %pcrel_hi(.LCPI0_15)
	ld	a0, %pcrel_lo(.Lpcrel_hi15)(a0)
	sd	a0, 24(sp)                      # 8-byte Folded Spill
.Lpcrel_hi16:
	auipc	a0, %pcrel_hi(_ZN7fast_io7details6sha51210k512scalarE)
	addi	a0, a0, %pcrel_lo(.Lpcrel_hi16)
	sd	a0, 248(sp)                     # 8-byte Folded Spill
	addi	a0, a0, 64
	sd	a0, 16(sp)                      # 8-byte Folded Spill
.LBB0_2:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_3 Depth 2
	ld	a0, 0(s7)
	xor	a1, t2, t6
	srli	a2, a0, 24
	lui	s1, 4080
	and	a2, a2, s1
	srli	a3, a0, 8
	ld	s8, 160(sp)                     # 8-byte Folded Reload
	and	a3, a3, s8
	or	a2, a2, a3
	srli	a3, a0, 40
	ld	s6, 152(sp)                     # 8-byte Folded Reload
	and	a3, a3, s6
	srli	a4, a0, 56
	or	a3, a3, a4
	or	a2, a2, a3
	and	a3, a0, s1
	lui	s3, 4080
	slli	a3, a3, 24
	srliw	a4, a0, 24
	slli	a4, a4, 32
	or	a3, a3, a4
	and	a4, a0, s6
	slli	a4, a4, 40
	slli	a0, a0, 56
	or	a0, a0, a4
	or	a0, a0, a3
	or	a4, a0, a2
	sd	a4, 320(sp)                     # 8-byte Folded Spill
	srli	a0, t4, 14
	slli	a2, t4, 50
	or	a0, a0, a2
	srli	a2, t4, 18
	slli	a3, t4, 46
	or	a2, a2, a3
	xor	a0, a0, a2
	srli	a2, t4, 41
	slli	a3, t4, 23
	or	a2, a2, a3
	xor	a0, a0, a2
	xor	a2, s0, a6
	and	a2, a2, t4
	xor	a2, a2, s0
	sd	a5, 240(sp)                     # 8-byte Folded Spill
	add	a2, a2, a5
	add	a0, a0, a2
	ld	a2, 144(sp)                     # 8-byte Folded Reload
	add	a0, a0, a2
	add	a0, a0, a4
	srli	a2, s2, 28
	slli	a3, s2, 36
	or	a2, a2, a3
	srli	a3, s2, 34
	slli	a4, s2, 30
	or	a3, a3, a4
	xor	a2, a2, a3
	srli	a3, s2, 39
	slli	a4, s2, 25
	or	a3, a3, a4
	xor	a3, a3, a2
	xor	a2, t6, s2
	and	a1, a1, a2
	xor	a1, a1, t6
	ld	s1, 8(s7)
	add	a3, a3, a1
	sd	a7, 200(sp)                     # 8-byte Folded Spill
	add	t3, a0, a7
	add	t0, a3, a0
	srli	a3, s1, 24
	and	a3, a3, s3
	srli	a5, s1, 8
	and	a5, a5, s8
	or	a3, a3, a5
	srli	a5, s1, 40
	and	a5, a5, s6
	srli	a4, s1, 56
	or	a4, a4, a5
	or	a3, a3, a4
	and	a4, s1, s3
	slli	a4, a4, 24
	srliw	a5, s1, 24
	slli	a5, a5, 32
	or	a4, a4, a5
	and	a5, s1, s6
	slli	a5, a5, 40
	slli	s1, s1, 56
	or	a5, a5, s1
	or	a4, a4, a5
	or	a0, a4, a3
	srli	a3, t3, 14
	slli	a4, t3, 50
	or	a3, a3, a4
	srli	a4, t3, 18
	slli	a5, t3, 46
	or	a4, a4, a5
	xor	a3, a3, a4
	srli	a4, t3, 41
	slli	a5, t3, 23
	or	a4, a4, a5
	xor	a3, a3, a4
	xor	a4, a6, t4
	and	a4, t3, a4
	xor	a4, a4, a6
	sd	s0, 232(sp)                     # 8-byte Folded Spill
	sd	a0, 312(sp)                     # 8-byte Folded Spill
	add	a5, s0, a0
	add	a4, a4, a5
	ld	a0, 136(sp)                     # 8-byte Folded Reload
	add	a4, a4, a0
	add	a4, a4, a3
	srli	a3, t0, 28
	slli	a5, t0, 36
	or	a3, a3, a5
	srli	a5, t0, 34
	slli	a0, t0, 30
	or	a0, a0, a5
	xor	a0, a0, a3
	srli	a3, t0, 39
	slli	a5, t0, 25
	or	a3, a3, a5
	xor	a0, a0, a3
	xor	a3, t0, s2
	and	a2, a2, a3
	xor	a2, a2, s2
	ld	s0, 16(s7)
	add	a0, a0, a2
	sd	t2, 192(sp)                     # 8-byte Folded Spill
	add	t5, a4, t2
	add	t1, a0, a4
	srli	a0, s0, 24
	and	a0, a0, s3
	srli	a4, s0, 8
	and	a4, a4, s8
	or	a0, a0, a4
	srli	a4, s0, 40
	and	a4, a4, s6
	srli	s1, s0, 56
	or	a4, a4, s1
	or	a0, a0, a4
	and	a4, s0, s3
	slli	a4, a4, 24
	srliw	s1, s0, 24
	slli	s1, s1, 32
	or	a4, a4, s1
	and	s1, s0, s6
	slli	s1, s1, 40
	slli	s0, s0, 56
	or	s0, s0, s1
	or	a4, a4, s0
	or	ra, a4, a0
	srli	a0, t5, 14
	slli	a4, t5, 50
	or	a0, a0, a4
	srli	a4, t5, 18
	slli	a5, t5, 46
	or	a4, a4, a5
	xor	a0, a0, a4
	srli	a4, t5, 41
	slli	a5, t5, 23
	or	a4, a4, a5
	xor	a0, a0, a4
	xor	a4, t3, t4
	and	a4, t5, a4
	xor	a4, a4, t4
	sd	a6, 216(sp)                     # 8-byte Folded Spill
	add	a5, a6, ra
	add	a4, a4, a5
	ld	a1, 128(sp)                     # 8-byte Folded Reload
	add	a4, a4, a1
	add	a0, a0, a4
	srli	a4, t1, 28
	slli	a5, t1, 36
	or	a4, a4, a5
	srli	a5, t1, 34
	slli	s1, t1, 30
	or	a5, a5, s1
	xor	a4, a4, a5
	srli	a5, t1, 39
	slli	s1, t1, 25
	or	a5, a5, s1
	xor	a5, a5, a4
	xor	a7, t1, t0
	and	a3, a7, a3
	xor	a3, a3, t0
	ld	a4, 24(s7)
	add	a3, a3, a5
	sd	t6, 184(sp)                     # 8-byte Folded Spill
	add	s1, a0, t6
	add	t2, a3, a0
	srli	a0, a4, 24
	and	a0, a0, s3
	srli	a5, a4, 8
	and	a5, a5, s8
	or	a0, a0, a5
	srli	a5, a4, 40
	and	a5, a5, s6
	srli	s0, a4, 56
	or	a5, a5, s0
	or	a0, a0, a5
	and	a5, a4, s3
	slli	a5, a5, 24
	srliw	s0, a4, 24
	slli	s0, s0, 32
	or	a5, a5, s0
	and	s0, a4, s6
	slli	s0, s0, 40
	slli	a4, a4, 56
	or	a4, a4, s0
	or	a4, a4, a5
	or	a1, a4, a0
	srli	a0, s1, 14
	slli	a4, s1, 50
	or	a0, a0, a4
	srli	a4, s1, 18
	slli	a5, s1, 46
	or	a4, a4, a5
	xor	a0, a0, a4
	srli	a4, s1, 41
	slli	a5, s1, 23
	or	a4, a4, a5
	xor	a0, a0, a4
	xor	a4, t5, t3
	and	a4, a4, s1
	xor	a4, a4, t3
	sd	t4, 208(sp)                     # 8-byte Folded Spill
	sd	a1, 304(sp)                     # 8-byte Folded Spill
	add	a5, t4, a1
	add	a4, a4, a5
	ld	a1, 120(sp)                     # 8-byte Folded Reload
	add	a4, a4, a1
	add	a0, a0, a4
	srli	a4, t2, 28
	slli	a5, t2, 36
	or	a4, a4, a5
	srli	a5, t2, 34
	slli	a3, t2, 30
	or	a3, a3, a5
	xor	a3, a3, a4
	srli	a4, t2, 39
	slli	a5, t2, 25
	or	a4, a4, a5
	xor	a3, a3, a4
	xor	a6, t2, t1
	and	a4, a6, a7
	xor	a4, a4, t1
	ld	a1, 32(s7)
	add	a3, a3, a4
	sd	s2, 176(sp)                     # 8-byte Folded Spill
	add	s0, a0, s2
	add	t4, a3, a0
	srli	a0, a1, 24
	and	a0, a0, s3
	srli	a3, a1, 8
	and	a3, a3, s8
	or	a0, a0, a3
	srli	a3, a1, 40
	and	a3, a3, s6
	srli	a5, a1, 56
	or	a3, a3, a5
	or	a0, a0, a3
	and	a3, a1, s3
	slli	a3, a3, 24
	srliw	a5, a1, 24
	slli	a5, a5, 32
	or	a3, a3, a5
	and	a5, a1, s6
	slli	a5, a5, 40
	slli	a1, a1, 56
	or	a1, a1, a5
	or	a1, a1, a3
	or	s4, a1, a0
	srli	a0, s0, 14
	slli	a1, s0, 50
	or	a0, a0, a1
	srli	a1, s0, 18
	slli	a3, s0, 46
	or	a1, a1, a3
	xor	a0, a0, a1
	srli	a1, s0, 41
	slli	a3, s0, 23
	or	a1, a1, a3
	xor	a0, a0, a1
	xor	a1, s1, t5
	and	a1, a1, s0
	xor	a1, a1, t5
	add	t3, t3, s4
	add	a1, a1, t3
	ld	a2, 112(sp)                     # 8-byte Folded Reload
	add	a1, a1, a2
	add	a0, a0, a1
	srli	a1, t4, 28
	slli	a3, t4, 36
	or	a1, a1, a3
	srli	a3, t4, 34
	slli	a5, t4, 30
	or	a3, a3, a5
	xor	a1, a1, a3
	srli	a3, t4, 39
	slli	a5, t4, 25
	or	a3, a3, a5
	xor	a1, a1, a3
	xor	a7, t4, t2
	and	a3, a7, a6
	xor	a3, a3, t2
	ld	a2, 40(s7)
	add	a3, a3, a1
	add	t3, a0, t0
	add	t0, a3, a0
	srli	a3, a2, 24
	and	a3, a3, s3
	srli	a5, a2, 8
	and	a5, a5, s8
	or	a3, a3, a5
	srli	a5, a2, 40
	and	a5, a5, s6
	srli	a4, a2, 56
	or	a4, a4, a5
	or	a3, a3, a4
	and	a4, a2, s3
	slli	a4, a4, 24
	srliw	a5, a2, 24
	slli	a5, a5, 32
	or	a4, a4, a5
	and	a5, a2, s6
	slli	a5, a5, 40
	slli	a2, a2, 56
	or	a2, a2, a5
	or	a2, a2, a4
	or	a0, a2, a3
	srli	a2, t3, 14
	slli	a3, t3, 50
	or	a2, a2, a3
	srli	a3, t3, 18
	slli	a4, t3, 46
	or	a3, a3, a4
	xor	a2, a2, a3
	srli	a3, t3, 41
	slli	a4, t3, 23
	or	a3, a3, a4
	xor	a2, a2, a3
	xor	a3, s0, s1
	and	a3, t3, a3
	xor	a3, a3, s1
	sd	a0, 288(sp)                     # 8-byte Folded Spill
	add	t5, t5, a0
	add	a3, a3, t5
	ld	a0, 104(sp)                     # 8-byte Folded Reload
	add	a3, a3, a0
	add	a2, a2, a3
	srli	a3, t0, 28
	slli	a4, t0, 36
	or	a3, a3, a4
	srli	a4, t0, 34
	slli	a5, t0, 30
	or	a4, a4, a5
	xor	a3, a3, a4
	srli	a4, t0, 39
	slli	a5, t0, 25
	or	a4, a4, a5
	xor	a3, a3, a4
	xor	a6, t0, t4
	and	a4, a6, a7
	xor	a4, a4, t4
	ld	a1, 48(s7)
	add	a3, a3, a4
	add	a5, a2, t1
	add	t5, a3, a2
	srli	a3, a1, 24
	and	a3, a3, s3
	srli	a4, a1, 8
	and	a4, a4, s8
	or	a3, a3, a4
	srli	a4, a1, 40
	and	a4, a4, s6
	srli	a0, a1, 56
	or	a0, a0, a4
	or	a0, a0, a3
	and	a3, a1, s3
	slli	a3, a3, 24
	srliw	a4, a1, 24
	slli	a4, a4, 32
	or	a3, a3, a4
	and	a4, a1, s6
	slli	a4, a4, 40
	slli	a1, a1, 56
	or	a1, a1, a4
	or	a1, a1, a3
	or	s10, a1, a0
	srli	a0, a5, 14
	slli	a1, a5, 50
	or	a0, a0, a1
	srli	a1, a5, 18
	slli	a3, a5, 46
	or	a1, a1, a3
	xor	a0, a0, a1
	srli	a1, a5, 41
	slli	a3, a5, 23
	or	a1, a1, a3
	xor	a0, a0, a1
	xor	a1, t3, s0
	and	a1, a1, a5
	xor	a1, a1, s0
	add	s1, s1, s10
	add	a1, a1, s1
	ld	a2, 96(sp)                      # 8-byte Folded Reload
	add	a1, a1, a2
	add	a0, a0, a1
	srli	a1, t5, 28
	slli	a3, t5, 36
	or	a1, a1, a3
	srli	a3, t5, 34
	slli	a4, t5, 30
	or	a3, a3, a4
	xor	a1, a1, a3
	srli	a3, t5, 39
	slli	a4, t5, 25
	or	a3, a3, a4
	xor	a1, a1, a3
	xor	a7, t5, t0
	and	a3, a7, a6
	xor	a3, a3, t0
	ld	a2, 56(s7)
	add	a1, a1, a3
	add	a3, a0, t2
	add	t2, a1, a0
	srli	a0, a2, 24
	lui	s1, 4080
	and	a0, a0, s1
	srli	a1, a2, 8
	and	a1, a1, s8
	or	a0, a0, a1
	srli	a1, a2, 40
	and	a1, a1, s6
	srli	a4, a2, 56
	or	a1, a1, a4
	or	a0, a0, a1
	and	a1, a2, s1
	lui	s9, 4080
	slli	a1, a1, 24
	srliw	a4, a2, 24
	slli	a4, a4, 32
	or	a1, a1, a4
	and	a4, a2, s6
	slli	a4, a4, 40
	slli	a2, a2, 56
	or	a2, a2, a4
	or	a1, a1, a2
	or	a4, a1, a0
	srli	a0, a3, 14
	slli	a1, a3, 50
	or	a0, a0, a1
	srli	a1, a3, 18
	slli	a2, a3, 46
	or	a1, a1, a2
	xor	a0, a0, a1
	srli	a1, a3, 41
	slli	a2, a3, 23
	or	a1, a1, a2
	xor	a0, a0, a1
	xor	a1, a5, t3
	and	a1, a1, a3
	xor	a1, a1, t3
	sd	a4, 296(sp)                     # 8-byte Folded Spill
	add	s0, s0, a4
	add	a1, a1, s0
	ld	a2, 88(sp)                      # 8-byte Folded Reload
	add	a1, a1, a2
	add	a0, a0, a1
	srli	a1, t2, 28
	slli	a2, t2, 36
	or	a1, a1, a2
	srli	a2, t2, 34
	slli	a4, t2, 30
	or	a2, a2, a4
	xor	a1, a1, a2
	srli	a2, t2, 39
	slli	a4, t2, 25
	or	a2, a2, a4
	xor	a1, a1, a2
	xor	t1, t2, t5
	and	a2, t1, a7
	xor	a2, a2, t5
	ld	s0, 64(s7)
	add	a1, a1, a2
	add	a4, a0, t4
	add	a6, a1, a0
	srli	a0, s0, 24
	and	a0, a0, s9
	srli	a1, s0, 8
	and	a1, a1, s8
	or	a0, a0, a1
	srli	a1, s0, 40
	and	a1, a1, s6
	srli	a2, s0, 56
	or	a1, a1, a2
	or	a0, a0, a1
	and	a1, s0, s9
	slli	a1, a1, 24
	srliw	a2, s0, 24
	slli	a2, a2, 32
	or	a1, a1, a2
	and	a2, s0, s6
	slli	a2, a2, 40
	slli	s0, s0, 56
	or	a2, a2, s0
	or	a1, a1, a2
	or	s2, a1, a0
	srli	a0, a4, 14
	slli	a1, a4, 50
	or	a0, a0, a1
	srli	a1, a4, 18
	slli	a2, a4, 46
	or	a1, a1, a2
	xor	a0, a0, a1
	srli	a1, a4, 41
	slli	a2, a4, 23
	or	a1, a1, a2
	xor	a0, a0, a1
	xor	a1, a3, a5
	and	a1, a1, a4
	xor	a1, a1, a5
	add	t3, t3, s2
	add	a1, a1, t3
	ld	a2, 80(sp)                      # 8-byte Folded Reload
	add	a1, a1, a2
	add	a0, a0, a1
	srli	a1, a6, 28
	slli	a2, a6, 36
	or	a1, a1, a2
	srli	a2, a6, 34
	slli	s0, a6, 30
	or	a2, a2, s0
	xor	a1, a1, a2
	srli	a2, a6, 39
	slli	s0, a6, 25
	or	a2, a2, s0
	xor	a1, a1, a2
	xor	a7, a6, t2
	and	a2, a7, t1
	xor	a2, a2, t2
	ld	s1, 72(s7)
	add	a2, a2, a1
	add	t0, t0, a0
	add	t1, a2, a0
	srli	a0, s1, 24
	and	a0, a0, s9
	srli	a2, s1, 8
	and	a2, a2, s8
	or	a0, a0, a2
	srli	a2, s1, 40
	and	a2, a2, s6
	srli	s0, s1, 56
	or	a2, a2, s0
	or	a0, a0, a2
	and	a2, s1, s9
	slli	a2, a2, 24
	srliw	s0, s1, 24
	slli	s0, s0, 32
	or	a2, a2, s0
	and	s0, s1, s6
	slli	s0, s0, 40
	slli	s1, s1, 56
	or	s0, s0, s1
	or	a2, a2, s0
	or	s3, a2, a0
	srli	a0, t0, 14
	slli	a2, t0, 50
	or	a0, a0, a2
	srli	a2, t0, 18
	slli	a1, t0, 46
	or	a1, a1, a2
	xor	a0, a0, a1
	srli	a1, t0, 41
	slli	a2, t0, 23
	or	a1, a1, a2
	xor	a0, a0, a1
	xor	a1, a4, a3
	and	a1, t0, a1
	xor	a1, a1, a3
	add	a5, a5, s3
	add	a1, a1, a5
	ld	a2, 72(sp)                      # 8-byte Folded Reload
	add	a1, a1, a2
	add	a1, a1, a0
	srli	a0, t1, 28
	slli	a2, t1, 36
	or	a0, a0, a2
	srli	a2, t1, 34
	slli	a5, t1, 30
	or	a2, a2, a5
	xor	a0, a0, a2
	srli	a2, t1, 39
	slli	a5, t1, 25
	or	a2, a2, a5
	xor	a2, a2, a0
	xor	t4, t1, a6
	and	a5, t4, a7
	xor	a5, a5, a6
	ld	s0, 80(s7)
	add	a5, a5, a2
	add	a0, a1, t5
	add	t3, a5, a1
	srli	a1, s0, 24
	and	a1, a1, s9
	srli	a5, s0, 8
	and	a5, a5, s8
	or	a1, a1, a5
	srli	a5, s0, 40
	and	a5, a5, s6
	srli	s1, s0, 56
	or	a5, a5, s1
	or	a1, a1, a5
	and	a5, s0, s9
	slli	a5, a5, 24
	srliw	s1, s0, 24
	slli	s1, s1, 32
	or	a5, a5, s1
	and	s1, s0, s6
	slli	s1, s1, 40
	slli	s0, s0, 56
	or	s0, s0, s1
	or	a5, a5, s0
	or	t6, a5, a1
	srli	a1, a0, 14
	slli	a5, a0, 50
	or	a1, a1, a5
	srli	a5, a0, 18
	slli	a2, a0, 46
	or	a2, a2, a5
	xor	a1, a1, a2
	srli	a2, a0, 41
	slli	a5, a0, 23
	or	a2, a2, a5
	xor	a1, a1, a2
	xor	a2, t0, a4
	and	a2, a2, a0
	xor	a2, a2, a4
	add	a3, a3, t6
	add	a2, a2, a3
	ld	a3, 64(sp)                      # 8-byte Folded Reload
	add	a2, a2, a3
	add	a1, a1, a2
	srli	a2, t3, 28
	slli	a3, t3, 36
	or	a2, a2, a3
	srli	a3, t3, 34
	slli	a5, t3, 30
	or	a3, a3, a5
	xor	a2, a2, a3
	srli	a3, t3, 39
	slli	a5, t3, 25
	or	a3, a3, a5
	xor	a2, a2, a3
	xor	a3, t3, t1
	and	a5, a3, t4
	xor	a5, a5, t1
	ld	s0, 88(s7)
	add	a2, a2, a5
	add	s1, a1, t2
	add	t5, a2, a1
	srli	a1, s0, 24
	and	a1, a1, s9
	srli	a2, s0, 8
	and	a2, a2, s8
	or	a1, a1, a2
	srli	a2, s0, 40
	and	a2, a2, s6
	srli	a5, s0, 56
	or	a2, a2, a5
	or	a1, a1, a2
	and	a2, s0, s9
	slli	a2, a2, 24
	srliw	a5, s0, 24
	slli	a5, a5, 32
	or	a2, a2, a5
	and	a5, s0, s6
	slli	a5, a5, 40
	slli	s0, s0, 56
	or	a5, a5, s0
	or	a2, a2, a5
	or	s0, a2, a1
	srli	a1, s1, 14
	slli	a2, s1, 50
	or	a1, a1, a2
	srli	a2, s1, 18
	slli	a5, s1, 46
	or	a2, a2, a5
	xor	a1, a1, a2
	srli	a2, s1, 41
	slli	a5, s1, 23
	or	a2, a2, a5
	xor	a1, a1, a2
	xor	a2, a0, t0
	and	a2, a2, s1
	xor	a2, a2, t0
	add	a4, a4, s0
	add	a2, a2, a4
	ld	a4, 56(sp)                      # 8-byte Folded Reload
	add	a2, a2, a4
	add	a1, a1, a2
	srli	a2, t5, 28
	slli	a4, t5, 36
	or	a2, a2, a4
	srli	a4, t5, 34
	slli	a5, t5, 30
	or	a4, a4, a5
	xor	a2, a2, a4
	srli	a4, t5, 39
	slli	a5, t5, 25
	or	a4, a4, a5
	xor	a4, a4, a2
	xor	t2, t5, t3
	and	a3, t2, a3
	xor	a3, a3, t3
	ld	a2, 96(s7)
	add	a3, a3, a4
	add	a4, a1, a6
	mv	a6, s0
	add	a7, a3, a1
	srli	a1, a2, 24
	and	a1, a1, s9
	srli	a5, a2, 8
	and	a5, a5, s8
	or	a1, a1, a5
	srli	a5, a2, 40
	and	a5, a5, s6
	srli	s0, a2, 56
	or	a5, a5, s0
	or	a1, a1, a5
	and	a5, a2, s9
	slli	a5, a5, 24
	srliw	s0, a2, 24
	slli	s0, s0, 32
	or	a5, a5, s0
	and	s0, a2, s6
	slli	s0, s0, 40
	slli	a2, a2, 56
	or	a2, a2, s0
	or	a2, a2, a5
	or	s11, a2, a1
	srli	a1, a4, 14
	slli	a2, a4, 50
	or	a1, a1, a2
	srli	a2, a4, 18
	slli	a5, a4, 46
	or	a2, a2, a5
	xor	a1, a1, a2
	srli	a2, a4, 41
	slli	a5, a4, 23
	or	a2, a2, a5
	xor	a1, a1, a2
	xor	a2, s1, a0
	and	a2, a2, a4
	xor	a2, a2, a0
	add	t0, t0, s11
	add	a2, a2, t0
	ld	a3, 48(sp)                      # 8-byte Folded Reload
	add	a2, a2, a3
	add	a2, a2, a1
	srli	a1, a7, 28
	slli	a5, a7, 36
	or	a1, a1, a5
	srli	a5, a7, 34
	slli	a3, a7, 30
	or	a3, a3, a5
	xor	a1, a1, a3
	srli	a3, a7, 39
	slli	a5, a7, 25
	or	a3, a3, a5
	xor	a3, a3, a1
	xor	a1, a7, t5
	and	a5, a1, t2
	xor	a5, a5, t5
	ld	s0, 104(s7)
	add	a3, a3, a5
	add	t1, t1, a2
	add	t0, a3, a2
	srli	a2, s0, 24
	and	a2, a2, s9
	srli	a3, s0, 8
	and	a3, a3, s8
	or	a2, a2, a3
	srli	a3, s0, 40
	and	a3, a3, s6
	srli	a5, s0, 56
	or	a3, a3, a5
	or	a2, a2, a3
	and	a3, s0, s9
	slli	a3, a3, 24
	srliw	a5, s0, 24
	slli	a5, a5, 32
	or	a3, a3, a5
	and	a5, s0, s6
	slli	a5, a5, 40
	slli	s0, s0, 56
	or	a5, a5, s0
	or	a3, a3, a5
	or	s0, a3, a2
	srli	a2, t1, 14
	slli	a3, t1, 50
	or	a2, a2, a3
	srli	a3, t1, 18
	slli	a5, t1, 46
	or	a3, a3, a5
	xor	a2, a2, a3
	srli	a3, t1, 41
	slli	a5, t1, 23
	or	a3, a3, a5
	xor	a2, a2, a3
	xor	a3, a4, s1
	and	a3, t1, a3
	xor	a3, a3, s1
	add	a0, a0, s0
	add	a0, a0, a3
	ld	a3, 40(sp)                      # 8-byte Folded Reload
	add	a0, a0, a3
	add	a2, a2, a0
	srli	a0, t0, 28
	slli	a3, t0, 36
	or	a0, a0, a3
	srli	a3, t0, 34
	slli	a5, t0, 30
	or	a3, a3, a5
	xor	a0, a0, a3
	srli	a3, t0, 39
	slli	a5, t0, 25
	or	a3, a3, a5
	xor	a3, a3, a0
	xor	a0, t0, a7
	and	a1, a1, a0
	xor	a1, a1, a7
	ld	a5, 112(s7)
	add	a1, a1, a3
	add	t3, t3, a2
	add	t2, a1, a2
	srli	a1, a5, 24
	and	a1, a1, s9
	srli	a2, a5, 8
	and	a2, a2, s8
	or	a1, a1, a2
	srli	a2, a5, 40
	and	a2, a2, s6
	srli	a3, a5, 56
	or	a2, a2, a3
	or	a1, a1, a2
	and	a2, a5, s9
	slli	a2, a2, 24
	srliw	a3, a5, 24
	slli	a3, a3, 32
	or	a2, a2, a3
	and	a3, a5, s6
	slli	a3, a3, 40
	slli	a5, a5, 56
	or	a3, a3, a5
	or	a2, a2, a3
	or	s5, a2, a1
	srli	a1, t3, 14
	slli	a2, t3, 50
	or	a1, a1, a2
	srli	a2, t3, 18
	slli	a3, t3, 46
	or	a2, a2, a3
	xor	a1, a1, a2
	srli	a2, t3, 41
	slli	a3, t3, 23
	or	a2, a2, a3
	xor	a1, a1, a2
	xor	a2, t1, a4
	and	a2, t3, a2
	xor	a2, a2, a4
	add	s1, s1, s5
	add	a2, a2, s1
	ld	a3, 32(sp)                      # 8-byte Folded Reload
	add	a2, a2, a3
	add	a2, a2, a1
	srli	a1, t2, 28
	slli	a3, t2, 36
	or	a1, a1, a3
	srli	a3, t2, 34
	slli	a5, t2, 30
	or	a3, a3, a5
	xor	a1, a1, a3
	srli	a3, t2, 39
	slli	a5, t2, 25
	or	a3, a3, a5
	xor	a3, a3, a1
	xor	a1, t2, t0
	and	a0, a0, a1
	xor	a0, a0, t0
	sd	s7, 224(sp)                     # 8-byte Folded Spill
	ld	a5, 120(s7)
	add	a0, a0, a3
	add	t5, t5, a2
	add	t4, a0, a2
	srli	a0, a5, 24
	and	a0, a0, s9
	srli	a2, a5, 8
	and	a2, a2, s8
	ld	s8, 320(sp)                     # 8-byte Folded Reload
	or	a0, a0, a2
	srli	a2, a5, 40
	and	a2, a2, s6
	srli	a3, a5, 56
	or	a2, a2, a3
	or	a0, a0, a2
	and	a2, a5, s9
	slli	a2, a2, 24
	srliw	a3, a5, 24
	slli	a3, a3, 32
	or	a2, a2, a3
	and	a3, a5, s6
	slli	a3, a3, 40
	slli	a5, a5, 56
	or	a3, a3, a5
	or	a2, a2, a3
	or	s6, a2, a0
	srli	a0, t5, 14
	slli	a2, t5, 50
	or	a0, a0, a2
	srli	a2, t5, 18
	slli	a3, t5, 46
	or	a2, a2, a3
	xor	a0, a0, a2
	srli	a2, t5, 41
	slli	a3, t5, 23
	or	a2, a2, a3
	xor	a0, a0, a2
	xor	a2, t3, t1
	and	a2, t5, a2
	xor	a2, a2, t1
	add	a4, a4, s6
	add	a2, a2, a4
	ld	a3, 24(sp)                      # 8-byte Folded Reload
	add	a2, a2, a3
	add	a2, a2, a0
	srli	a0, t4, 28
	slli	a3, t4, 36
	or	a0, a0, a3
	srli	a3, t4, 34
	slli	a4, t4, 30
	or	a3, a3, a4
	xor	a0, a0, a3
	srli	a3, t4, 39
	slli	a4, t4, 25
	or	a3, a3, a4
	xor	a3, a3, a0
	xor	s1, t4, t2
	and	a1, a1, s1
	xor	a1, a1, t2
	add	a1, a1, a3
	add	a3, a2, a7
	add	s7, a1, a2
	ld	a7, 16(sp)                      # 8-byte Folded Reload
.LBB0_3:                                #   Parent Loop BB0_2 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	sd	a6, 256(sp)                     # 8-byte Folded Spill
	sd	s3, 264(sp)                     # 8-byte Folded Spill
	sd	s5, 272(sp)                     # 8-byte Folded Spill
	sd	s0, 320(sp)                     # 8-byte Folded Spill
	ld	a0, 312(sp)                     # 8-byte Folded Reload
	srli	a1, a0, 1
	slli	a2, a0, 63
	or	a1, a1, a2
	srli	a2, a0, 8
	slli	a5, a0, 56
	or	a2, a2, a5
	xor	a1, a1, a2
	srli	a2, a0, 7
	xor	a1, a1, a2
	srli	a2, s5, 19
	slli	a5, s5, 45
	or	a2, a2, a5
	srli	a5, s5, 61
	slli	s0, s5, 3
	or	a5, a5, s0
	xor	a2, a2, a5
	srli	a5, s5, 6
	xor	a2, a2, a5
	add	a5, s8, s3
	add	a1, a1, a5
	mv	s8, s11
	sd	s11, 280(sp)                    # 8-byte Folded Spill
	add	s11, a1, a2
	srli	a1, a3, 14
	slli	a2, a3, 50
	or	a1, a1, a2
	srli	a2, a3, 18
	slli	a5, a3, 46
	or	a2, a2, a5
	xor	a1, a1, a2
	srli	a2, a3, 41
	slli	a5, a3, 23
	or	a2, a2, a5
	ld	a5, -64(a7)
	xor	a1, a1, a2
	xor	a2, t3, t5
	and	a2, a2, a3
	xor	a2, a2, t3
	add	a2, a2, t1
	add	a1, a1, a2
	add	a1, a1, a5
	add	a5, a1, s11
	srli	a1, s7, 28
	slli	a2, s7, 36
	or	a1, a1, a2
	srli	a2, s7, 34
	slli	a4, s7, 30
	or	a2, a2, a4
	xor	a1, a1, a2
	srli	a2, s7, 39
	slli	a4, s7, 25
	or	a2, a2, a4
	xor	a2, a2, a1
	xor	a1, t4, s7
	and	s1, s1, a1
	xor	a4, s1, t4
	add	a2, a2, a4
	add	t0, t0, a5
	add	t1, a2, a5
	srli	a2, ra, 1
	slli	a4, ra, 63
	or	a2, a2, a4
	srli	a4, ra, 8
	slli	s1, ra, 56
	or	a4, a4, s1
	xor	a2, a2, a4
	srli	a4, ra, 7
	xor	a2, a2, a4
	srli	a4, s6, 19
	slli	s1, s6, 45
	or	a4, a4, s1
	srli	s1, s6, 61
	slli	s0, s6, 3
	or	s0, s0, s1
	xor	a4, a4, s0
	srli	s1, s6, 6
	xor	a4, a4, s1
	add	s1, a0, t6
	add	a2, a2, s1
	mv	s3, s2
	add	s2, a2, a4
	srli	a2, t0, 14
	slli	a4, t0, 50
	or	a2, a2, a4
	srli	a4, t0, 18
	slli	s1, t0, 46
	or	a4, a4, s1
	xor	a2, a2, a4
	srli	a4, t0, 41
	slli	s1, t0, 23
	or	a4, a4, s1
	ld	s1, -56(a7)
	xor	a2, a2, a4
	xor	a4, t5, a3
	and	a4, t0, a4
	xor	a4, a4, t5
	add	t3, t3, s1
	add	t3, t3, s2
	add	a4, a4, t3
	add	a2, a2, a4
	srli	a4, t1, 28
	slli	a5, t1, 36
	or	a4, a4, a5
	srli	a5, t1, 34
	slli	a0, t1, 30
	or	a0, a0, a5
	xor	a0, a0, a4
	srli	a4, t1, 39
	slli	a5, t1, 25
	or	a4, a4, a5
	xor	a0, a0, a4
	xor	a4, t1, s7
	and	a1, a1, a4
	xor	a1, a1, s7
	add	a0, a0, a1
	add	t2, t2, a2
	add	t3, a0, a2
	ld	s0, 304(sp)                     # 8-byte Folded Reload
	srli	a0, s0, 1
	slli	a2, s0, 63
	or	a0, a0, a2
	srli	a2, s0, 8
	slli	a5, s0, 56
	or	a2, a2, a5
	xor	a0, a0, a2
	srli	a2, s0, 7
	xor	a0, a0, a2
	srli	a2, s11, 19
	slli	a5, s11, 45
	or	a2, a2, a5
	srli	a5, s11, 61
	slli	s1, s11, 3
	or	a5, a5, s1
	xor	a2, a2, a5
	srli	a5, s11, 6
	xor	a2, a2, a5
	add	a5, ra, a6
	add	a0, a0, a5
	add	ra, a0, a2
	srli	a0, t2, 14
	slli	a2, t2, 50
	or	a0, a0, a2
	srli	a2, t2, 18
	slli	a5, t2, 46
	or	a2, a2, a5
	xor	a0, a0, a2
	srli	a2, t2, 41
	slli	a5, t2, 23
	or	a2, a2, a5
	ld	a5, -48(a7)
	xor	a0, a0, a2
	xor	a2, t0, a3
	and	a2, t2, a2
	xor	a2, a2, a3
	add	a5, a5, t5
	add	a5, a5, ra
	add	a2, a2, a5
	add	a0, a0, a2
	srli	a2, t3, 28
	slli	a5, t3, 36
	or	a2, a2, a5
	srli	a5, t3, 34
	slli	a1, t3, 30
	or	a1, a1, a5
	xor	a1, a1, a2
	srli	a2, t3, 39
	slli	a5, t3, 25
	or	a2, a2, a5
	xor	a1, a1, a2
	xor	a2, t3, t1
	and	a4, a4, a2
	xor	a4, a4, t1
	add	a1, a1, a4
	add	t4, t4, a0
	add	s1, a1, a0
	srli	a0, s4, 1
	slli	a1, s4, 63
	or	a0, a0, a1
	srli	a1, s4, 8
	slli	a4, s4, 56
	or	a1, a1, a4
	xor	a0, a0, a1
	srli	a1, s4, 7
	xor	a0, a0, a1
	srli	a1, s2, 19
	slli	a4, s2, 45
	or	a1, a1, a4
	srli	a4, s2, 61
	slli	a5, s2, 3
	or	a4, a4, a5
	xor	a1, a1, a4
	srli	a4, s2, 6
	xor	a1, a1, a4
	add	a4, s0, s8
	add	a0, a0, a4
	mv	a6, t6
	add	t6, a0, a1
	srli	a0, t4, 14
	slli	a1, t4, 50
	or	a0, a0, a1
	srli	a1, t4, 18
	slli	a4, t4, 46
	or	a1, a1, a4
	xor	a0, a0, a1
	srli	a1, t4, 41
	slli	a4, t4, 23
	or	a1, a1, a4
	ld	a4, -40(a7)
	xor	a0, a0, a1
	xor	a1, t2, t0
	and	a1, t4, a1
	xor	a1, a1, t0
	add	a3, a3, a4
	add	a3, a3, t6
	add	a1, a1, a3
	add	a0, a0, a1
	srli	a1, s1, 28
	slli	a3, s1, 36
	or	a1, a1, a3
	srli	a3, s1, 34
	slli	a4, s1, 30
	or	a3, a3, a4
	xor	a1, a1, a3
	srli	a3, s1, 39
	slli	a4, s1, 25
	or	a3, a3, a4
	xor	a1, a1, a3
	xor	s0, s1, t3
	and	a2, a2, s0
	xor	a2, a2, t3
	add	a1, a1, a2
	add	a4, a0, s7
	add	s7, a1, a0
	ld	s9, 288(sp)                     # 8-byte Folded Reload
	srli	a0, s9, 1
	slli	a1, s9, 63
	or	a0, a0, a1
	srli	a1, s9, 8
	slli	a2, s9, 56
	or	a1, a1, a2
	xor	a0, a0, a1
	srli	a1, s9, 7
	xor	a0, a0, a1
	srli	a1, ra, 19
	slli	a2, ra, 45
	or	a1, a1, a2
	srli	a2, ra, 61
	slli	a5, ra, 3
	or	a2, a2, a5
	xor	a1, a1, a2
	srli	a2, ra, 6
	xor	a1, a1, a2
	ld	s8, 320(sp)                     # 8-byte Folded Reload
	add	s8, s8, s4
	add	a0, a0, s8
	add	s4, a0, a1
	srli	a0, a4, 14
	slli	a1, a4, 50
	or	a0, a0, a1
	srli	a1, a4, 18
	slli	a2, a4, 46
	or	a1, a1, a2
	xor	a0, a0, a1
	srli	a1, a4, 41
	slli	a2, a4, 23
	or	a1, a1, a2
	ld	a2, -32(a7)
	xor	a0, a0, a1
	xor	a1, t4, t2
	and	a1, a1, a4
	xor	a1, a1, t2
	add	a2, a2, t0
	add	a2, a2, s4
	add	a1, a1, a2
	add	a0, a0, a1
	srli	a1, s7, 28
	slli	a2, s7, 36
	or	a1, a1, a2
	srli	a2, s7, 34
	slli	a5, s7, 30
	or	a2, a2, a5
	xor	a1, a1, a2
	srli	a2, s7, 39
	slli	a5, s7, 25
	or	a2, a2, a5
	xor	a1, a1, a2
	xor	t5, s7, s1
	and	s0, t5, s0
	xor	s0, s0, s1
	add	a1, a1, s0
	add	a5, a0, t1
	add	s8, a1, a0
	srli	a0, s10, 1
	slli	a1, s10, 63
	or	a0, a0, a1
	srli	a1, s10, 8
	slli	a2, s10, 56
	or	a1, a1, a2
	xor	a0, a0, a1
	srli	a1, s10, 7
	xor	a0, a0, a1
	srli	a1, t6, 19
	slli	a2, t6, 45
	or	a1, a1, a2
	srli	a2, t6, 61
	slli	a3, t6, 3
	or	a2, a2, a3
	xor	a1, a1, a2
	srli	a2, t6, 6
	xor	a1, a1, a2
	add	s9, s9, s5
	add	a0, a0, s9
	add	s5, a0, a1
	sd	s5, 288(sp)                     # 8-byte Folded Spill
	srli	a0, a5, 14
	slli	a1, a5, 50
	or	a0, a0, a1
	srli	a1, a5, 18
	slli	a2, a5, 46
	or	a1, a1, a2
	xor	a0, a0, a1
	srli	a1, a5, 41
	slli	a2, a5, 23
	or	a1, a1, a2
	ld	a2, -24(a7)
	xor	a0, a0, a1
	xor	a1, a4, t4
	and	a1, a1, a5
	xor	a1, a1, t4
	add	a2, a2, t2
	add	a2, a2, s5
	add	a1, a1, a2
	add	a0, a0, a1
	srli	a1, s8, 28
	slli	a2, s8, 36
	or	a1, a1, a2
	srli	a2, s8, 34
	slli	a3, s8, 30
	or	a2, a2, a3
	xor	a1, a1, a2
	srli	a2, s8, 39
	slli	a3, s8, 25
	or	a2, a2, a3
	xor	a1, a1, a2
	xor	t0, s8, s7
	and	a2, t0, t5
	xor	a2, a2, s7
	add	a2, a2, a1
	add	t5, a0, t3
	add	t2, a2, a0
	ld	a1, 296(sp)                     # 8-byte Folded Reload
	srli	a2, a1, 1
	slli	a3, a1, 63
	or	a2, a2, a3
	srli	a3, a1, 8
	slli	a0, a1, 56
	or	a0, a0, a3
	xor	a0, a0, a2
	srli	a2, a1, 7
	xor	a0, a0, a2
	srli	a2, s4, 19
	slli	a3, s4, 45
	or	a2, a2, a3
	srli	a3, s4, 61
	slli	s0, s4, 3
	or	a3, a3, s0
	xor	a2, a2, a3
	srli	a3, s4, 6
	xor	a2, a2, a3
	add	s10, s10, s6
	add	a0, a0, s10
	add	s10, a0, a2
	srli	a0, t5, 14
	slli	a2, t5, 50
	or	a0, a0, a2
	srli	a2, t5, 18
	slli	a3, t5, 46
	or	a2, a2, a3
	xor	a0, a0, a2
	srli	a2, t5, 41
	slli	a3, t5, 23
	or	a2, a2, a3
	ld	a3, -16(a7)
	xor	a0, a0, a2
	xor	a2, a5, a4
	and	a2, t5, a2
	xor	a2, a2, a4
	add	a3, a3, t4
	add	a3, a3, s10
	add	a2, a2, a3
	add	a0, a0, a2
	srli	a2, t2, 28
	slli	a3, t2, 36
	or	a2, a2, a3
	srli	a3, t2, 34
	slli	s0, t2, 30
	or	a3, a3, s0
	xor	a2, a2, a3
	srli	a3, t2, 39
	slli	s0, t2, 25
	or	a3, a3, s0
	xor	a2, a2, a3
	xor	t1, t2, s8
	and	a3, t1, t0
	xor	a3, a3, s8
	add	a2, a2, a3
	add	s1, s1, a0
	add	t4, a2, a0
	srli	a0, s3, 1
	slli	a3, s3, 63
	or	a0, a0, a3
	srli	a3, s3, 8
	slli	s0, s3, 56
	or	a3, a3, s0
	xor	a0, a0, a3
	srli	a3, s3, 7
	xor	a0, a0, a3
	srli	a3, s5, 19
	slli	s0, s5, 45
	or	a3, a3, s0
	srli	s0, s5, 61
	slli	a2, s5, 3
	or	a2, a2, s0
	xor	a2, a2, a3
	srli	a3, s5, 6
	xor	a2, a2, a3
	add	a0, a0, a1
	add	a0, a0, s11
	add	s9, a0, a2
	srli	a0, s1, 14
	slli	a2, s1, 50
	or	a0, a0, a2
	srli	a2, s1, 18
	slli	a3, s1, 46
	or	a2, a2, a3
	xor	a0, a0, a2
	srli	a2, s1, 41
	slli	a3, s1, 23
	or	a2, a2, a3
	ld	a3, -8(a7)
	xor	a0, a0, a2
	xor	a2, t5, a5
	and	a2, a2, s1
	xor	a2, a2, a5
	add	a3, a3, s9
	add	a3, a3, a4
	add	a2, a2, a3
	add	a0, a0, a2
	srli	a2, t4, 28
	slli	a3, t4, 36
	or	a2, a2, a3
	srli	a3, t4, 34
	slli	a4, t4, 30
	or	a3, a3, a4
	xor	a2, a2, a3
	srli	a3, t4, 39
	slli	a4, t4, 25
	or	a3, a3, a4
	xor	a2, a2, a3
	xor	t0, t4, t2
	and	a3, t0, t1
	xor	a3, a3, t2
	add	a2, a2, a3
	add	a4, a0, s7
	add	s7, a2, a0
	ld	a1, 264(sp)                     # 8-byte Folded Reload
	srli	a0, a1, 1
	slli	a2, a1, 63
	or	a0, a0, a2
	srli	a2, a1, 8
	slli	s0, a1, 56
	or	a2, a2, s0
	xor	a0, a0, a2
	srli	a2, a1, 7
	xor	a0, a0, a2
	srli	a2, s10, 19
	slli	s0, s10, 45
	or	a2, a2, s0
	srli	s0, s10, 61
	slli	a3, s10, 3
	or	a3, a3, s0
	xor	a2, a2, a3
	srli	a3, s10, 6
	xor	a2, a2, a3
	add	a0, a0, s3
	sd	s2, 312(sp)                     # 8-byte Folded Spill
	add	a0, a0, s2
	add	s2, a0, a2
	srli	a0, a4, 14
	slli	a2, a4, 50
	or	a0, a0, a2
	srli	a2, a4, 18
	slli	a3, a4, 46
	or	a2, a2, a3
	xor	a0, a0, a2
	srli	a2, a4, 41
	slli	a3, a4, 23
	or	a2, a2, a3
	ld	a3, 0(a7)
	xor	a0, a0, a2
	xor	a2, s1, t5
	and	a2, a2, a4
	xor	a2, a2, t5
	add	a3, a3, s2
	add	a3, a3, a5
	add	a2, a2, a3
	add	a0, a0, a2
	srli	a2, s7, 28
	slli	a3, s7, 36
	or	a2, a2, a3
	srli	a3, s7, 34
	slli	a5, s7, 30
	or	a3, a3, a5
	xor	a2, a2, a3
	srli	a3, s7, 39
	slli	a5, s7, 25
	or	a3, a3, a5
	xor	a2, a2, a3
	xor	t3, s7, t4
	and	a3, t3, t0
	xor	a3, a3, t4
	add	a2, a2, a3
	add	s0, a0, s8
	mv	s8, s11
	add	t1, a2, a0
	srli	a0, a6, 1
	slli	a2, a6, 63
	or	a0, a0, a2
	srli	a2, a6, 8
	slli	a3, a6, 56
	or	a2, a2, a3
	xor	a0, a0, a2
	srli	a2, a6, 7
	xor	a0, a0, a2
	srli	a2, s9, 19
	slli	a3, s9, 45
	or	a2, a2, a3
	srli	a3, s9, 61
	slli	a5, s9, 3
	or	a3, a3, a5
	xor	a2, a2, a3
	srli	a3, s9, 6
	xor	a2, a2, a3
	add	a0, a0, a1
	add	a0, a0, ra
	add	s3, a0, a2
	srli	a0, s0, 14
	slli	a2, s0, 50
	or	a0, a0, a2
	srli	a2, s0, 18
	slli	a3, s0, 46
	or	a2, a2, a3
	xor	a0, a0, a2
	srli	a2, s0, 41
	slli	a3, s0, 23
	or	a2, a2, a3
	ld	a3, 8(a7)
	xor	a0, a0, a2
	xor	a2, a4, s1
	and	a2, a2, s0
	xor	a2, a2, s1
	add	a3, a3, s3
	add	a1, a3, t5
	add	a1, a1, a2
	add	a2, a1, a0
	srli	a0, t1, 28
	slli	a1, t1, 36
	or	a0, a0, a1
	srli	a1, t1, 34
	slli	a3, t1, 30
	or	a1, a1, a3
	xor	a0, a0, a1
	srli	a1, t1, 39
	slli	a3, t1, 25
	or	a1, a1, a3
	xor	a0, a0, a1
	xor	t0, t1, s7
	and	a3, t0, t3
	xor	a3, a3, s7
	add	a3, a3, a0
	add	a0, a2, t2
	add	t3, a3, a2
	ld	s11, 256(sp)                    # 8-byte Folded Reload
	srli	a2, s11, 1
	slli	a3, s11, 63
	or	a2, a2, a3
	srli	a3, s11, 8
	slli	a5, s11, 56
	or	a3, a3, a5
	xor	a2, a2, a3
	srli	a3, s11, 7
	xor	a2, a2, a3
	srli	a3, s2, 19
	slli	a5, s2, 45
	or	a3, a3, a5
	srli	a5, s2, 61
	slli	a1, s2, 3
	or	a1, a1, a5
	xor	a1, a1, a3
	srli	a3, s2, 6
	xor	a1, a1, a3
	add	a2, a2, a6
	sd	t6, 304(sp)                     # 8-byte Folded Spill
	add	a2, a2, t6
	add	t6, a2, a1
	srli	a1, a0, 14
	slli	a2, a0, 50
	or	a1, a1, a2
	srli	a2, a0, 18
	slli	a3, a0, 46
	or	a2, a2, a3
	xor	a1, a1, a2
	srli	a2, a0, 41
	slli	a3, a0, 23
	or	a2, a2, a3
	ld	a3, 16(a7)
	xor	a1, a1, a2
	xor	a2, s0, a4
	and	a2, a2, a0
	xor	a2, a2, a4
	add	a3, a3, t6
	add	a3, a3, s1
	add	a2, a2, a3
	add	a1, a1, a2
	srli	a2, t3, 28
	slli	a3, t3, 36
	or	a2, a2, a3
	srli	a3, t3, 34
	slli	a5, t3, 30
	or	a3, a3, a5
	xor	a2, a2, a3
	srli	a3, t3, 39
	slli	a5, t3, 25
	or	a3, a3, a5
	xor	a2, a2, a3
	xor	t2, t3, t1
	and	a3, t2, t0
	xor	a3, a3, t1
	add	a3, a3, a2
	add	a2, a1, t4
	add	t5, a3, a1
	ld	t4, 280(sp)                     # 8-byte Folded Reload
	srli	a1, t4, 1
	slli	a3, t4, 63
	or	a1, a1, a3
	srli	a3, t4, 8
	slli	s1, t4, 56
	or	a3, a3, s1
	xor	a1, a1, a3
	srli	a3, t4, 7
	xor	a1, a1, a3
	srli	a3, s3, 19
	slli	s1, s3, 45
	or	a3, a3, s1
	srli	s1, s3, 61
	slli	a5, s3, 3
	or	a5, a5, s1
	xor	a3, a3, a5
	srli	a5, s3, 6
	xor	a3, a3, a5
	add	a1, a1, s11
	add	a1, a1, s4
	add	a6, a1, a3
	srli	a1, a2, 14
	slli	a3, a2, 50
	or	a1, a1, a3
	srli	a3, a2, 18
	slli	a5, a2, 46
	or	a3, a3, a5
	xor	a1, a1, a3
	srli	a3, a2, 41
	slli	a5, a2, 23
	or	a3, a3, a5
	ld	a5, 24(a7)
	xor	a1, a1, a3
	xor	a3, a0, s0
	and	a3, a3, a2
	xor	a3, a3, s0
	add	a5, a5, a6
	add	a4, a4, a5
	add	a3, a3, a4
	add	a3, a3, a1
	srli	a1, t5, 28
	slli	a4, t5, 36
	or	a1, a1, a4
	srli	a4, t5, 34
	slli	a5, t5, 30
	or	a4, a4, a5
	xor	a1, a1, a4
	srli	a4, t5, 39
	slli	a5, t5, 25
	or	a4, a4, a5
	xor	a1, a1, a4
	xor	t0, t5, t3
	and	a5, t0, t2
	xor	a5, a5, t3
	add	a5, a5, a1
	add	a1, a3, s7
	add	s7, a5, a3
	ld	t2, 320(sp)                     # 8-byte Folded Reload
	srli	a5, t2, 1
	slli	s1, t2, 63
	or	a5, a5, s1
	srli	s1, t2, 8
	slli	a4, t2, 56
	or	a4, a4, s1
	xor	a4, a4, a5
	srli	a5, t2, 7
	xor	a4, a4, a5
	srli	a5, t6, 19
	slli	s1, t6, 45
	or	a5, a5, s1
	srli	s1, t6, 61
	slli	a3, t6, 3
	or	a3, a3, s1
	xor	a3, a3, a5
	srli	a5, t6, 6
	xor	a3, a3, a5
	add	a4, a4, t4
	add	a4, a4, s5
	add	s11, a4, a3
	srli	a3, a1, 14
	slli	a4, a1, 50
	or	a3, a3, a4
	srli	a4, a1, 18
	slli	a5, a1, 46
	or	a4, a4, a5
	xor	a3, a3, a4
	srli	a4, a1, 41
	slli	a5, a1, 23
	or	a4, a4, a5
	ld	a5, 32(a7)
	xor	a3, a3, a4
	xor	a4, a2, a0
	and	a4, a4, a1
	xor	a4, a4, a0
	add	a5, a5, s11
	add	a5, a5, s0
	add	a4, a4, a5
	add	a3, a3, a4
	srli	a4, s7, 28
	slli	a5, s7, 36
	or	a4, a4, a5
	srli	a5, s7, 34
	slli	s1, s7, 30
	or	a5, a5, s1
	xor	a4, a4, a5
	srli	a5, s7, 39
	slli	s1, s7, 25
	or	a5, a5, s1
	xor	a4, a4, a5
	xor	a5, s7, t5
	and	s1, a5, t0
	xor	s1, s1, t5
	add	a4, a4, s1
	add	t1, t1, a3
	add	t0, a4, a3
	ld	t4, 272(sp)                     # 8-byte Folded Reload
	srli	a3, t4, 1
	slli	a4, t4, 63
	or	a3, a3, a4
	srli	a4, t4, 8
	slli	s1, t4, 56
	or	a4, a4, s1
	xor	a3, a3, a4
	srli	a4, t4, 7
	xor	a3, a3, a4
	srli	a4, a6, 19
	slli	s1, a6, 45
	or	a4, a4, s1
	srli	s1, a6, 61
	slli	s0, a6, 3
	or	s0, s0, s1
	xor	a4, a4, s0
	srli	s1, a6, 6
	xor	a4, a4, s1
	add	a3, a3, t2
	add	a3, a3, s10
	add	s0, a3, a4
	srli	a3, t1, 14
	slli	a4, t1, 50
	or	a3, a3, a4
	srli	a4, t1, 18
	slli	s1, t1, 46
	or	a4, a4, s1
	xor	a3, a3, a4
	srli	a4, t1, 41
	slli	s1, t1, 23
	or	a4, a4, s1
	ld	s1, 40(a7)
	xor	a3, a3, a4
	xor	a4, a1, a2
	and	a4, t1, a4
	xor	a4, a4, a2
	add	s1, s1, s0
	add	a0, a0, s1
	add	a0, a0, a4
	add	a3, a3, a0
	srli	a0, t0, 28
	slli	a4, t0, 36
	or	a0, a0, a4
	srli	a4, t0, 34
	slli	s1, t0, 30
	or	a4, a4, s1
	xor	a0, a0, a4
	srli	a4, t0, 39
	slli	s1, t0, 25
	or	a4, a4, s1
	xor	a4, a4, a0
	xor	a0, t0, s7
	and	a5, a5, a0
	xor	a5, a5, s7
	add	a4, a4, a5
	add	t3, t3, a3
	add	t2, a4, a3
	srli	a3, s6, 1
	slli	a4, s6, 63
	or	a3, a3, a4
	srli	a4, s6, 8
	slli	a5, s6, 56
	or	a4, a4, a5
	xor	a3, a3, a4
	srli	a4, s6, 7
	xor	a3, a3, a4
	srli	a4, s11, 19
	slli	a5, s11, 45
	or	a4, a4, a5
	srli	a5, s11, 61
	slli	s1, s11, 3
	or	a5, a5, s1
	xor	a4, a4, a5
	srli	a5, s11, 6
	xor	a4, a4, a5
	add	a3, a3, t4
	sd	s9, 296(sp)                     # 8-byte Folded Spill
	add	a3, a3, s9
	add	s5, a3, a4
	srli	a3, t3, 14
	slli	a4, t3, 50
	or	a3, a3, a4
	srli	a4, t3, 18
	slli	a5, t3, 46
	or	a4, a4, a5
	xor	a3, a3, a4
	srli	a4, t3, 41
	slli	a5, t3, 23
	or	a4, a4, a5
	ld	a5, 48(a7)
	xor	a3, a3, a4
	xor	a4, t1, a1
	and	a4, t3, a4
	xor	a4, a4, a1
	add	a5, a5, s5
	add	a2, a2, a5
	add	a2, a2, a4
	add	a3, a3, a2
	srli	a2, t2, 28
	slli	a4, t2, 36
	or	a2, a2, a4
	srli	a4, t2, 34
	slli	a5, t2, 30
	or	a4, a4, a5
	xor	a2, a2, a4
	srli	a4, t2, 39
	slli	a5, t2, 25
	or	a4, a4, a5
	xor	a4, a4, a2
	xor	a2, t2, t0
	and	a0, a0, a2
	xor	a0, a0, t0
	add	a0, a0, a4
	add	t5, t5, a3
	add	t4, a0, a3
	srli	a0, s8, 1
	slli	a3, s8, 63
	or	a0, a0, a3
	srli	a3, s8, 8
	slli	a4, s8, 56
	or	a3, a3, a4
	xor	a0, a0, a3
	srli	a3, s8, 7
	xor	a0, a0, a3
	srli	a3, s0, 19
	slli	a4, s0, 45
	or	a3, a3, a4
	srli	a4, s0, 61
	slli	a5, s0, 3
	or	a4, a4, a5
	xor	a3, a3, a4
	srli	a4, s0, 6
	xor	a3, a3, a4
	add	a0, a0, s6
	add	a0, a0, s2
	add	s6, a0, a3
	srli	a0, t5, 14
	slli	a3, t5, 50
	or	a0, a0, a3
	srli	a3, t5, 18
	slli	a4, t5, 46
	or	a3, a3, a4
	xor	a0, a0, a3
	srli	a3, t5, 41
	slli	a4, t5, 23
	or	a3, a3, a4
	ld	a4, 56(a7)
	xor	a0, a0, a3
	xor	a3, t3, t1
	and	a3, t5, a3
	xor	a3, a3, t1
	add	a4, a4, s6
	add	a1, a1, a4
	add	a1, a1, a3
	add	a0, a0, a1
	srli	a1, t4, 28
	slli	a3, t4, 36
	or	a1, a1, a3
	srli	a3, t4, 34
	slli	a4, t4, 30
	or	a3, a3, a4
	xor	a1, a1, a3
	srli	a3, t4, 39
	slli	a4, t4, 25
	or	a3, a3, a4
	xor	a1, a1, a3
	xor	s1, t4, t2
	and	a2, a2, s1
	xor	a2, a2, t2
	add	a1, a1, a2
	add	a3, a0, s7
	addi	a7, a7, 128
	ld	a2, 248(sp)                     # 8-byte Folded Reload
	addi	a2, a2, 576
	add	s7, a1, a0
	bne	a7, a2, .LBB0_3
# %bb.4:                                #   in Loop: Header=BB0_2 Depth=1
	ld	s2, 176(sp)                     # 8-byte Folded Reload
	add	s2, s2, s7
	ld	t6, 184(sp)                     # 8-byte Folded Reload
	add	t6, t6, t4
	ld	a0, 192(sp)                     # 8-byte Folded Reload
	add	t2, t2, a0
	ld	a7, 200(sp)                     # 8-byte Folded Reload
	add	a7, a7, t0
	ld	t4, 208(sp)                     # 8-byte Folded Reload
	add	t4, t4, a3
	ld	a6, 216(sp)                     # 8-byte Folded Reload
	add	a6, a6, t5
	ld	s0, 232(sp)                     # 8-byte Folded Reload
	add	s0, s0, t3
	ld	s7, 224(sp)                     # 8-byte Folded Reload
	addi	s7, s7, 128
	ld	a5, 240(sp)                     # 8-byte Folded Reload
	add	a5, a5, t1
	ld	a0, 168(sp)                     # 8-byte Folded Reload
	beq	s7, a0, .LBB0_5
	j	.LBB0_2
.LBB0_5:
	ld	a0, 8(sp)                       # 8-byte Folded Reload
	sd	s2, 0(a0)
	sd	t6, 8(a0)
	sd	t2, 16(a0)
	sd	a7, 24(a0)
	sd	t4, 32(a0)
	sd	a6, 40(a0)
	sd	s0, 48(a0)
	sd	a5, 56(a0)
.LBB0_6:
	ld	ra, 424(sp)                     # 8-byte Folded Reload
	ld	s0, 416(sp)                     # 8-byte Folded Reload
	ld	s1, 408(sp)                     # 8-byte Folded Reload
	ld	s2, 400(sp)                     # 8-byte Folded Reload
	ld	s3, 392(sp)                     # 8-byte Folded Reload
	ld	s4, 384(sp)                     # 8-byte Folded Reload
	ld	s5, 376(sp)                     # 8-byte Folded Reload
	ld	s6, 368(sp)                     # 8-byte Folded Reload
	ld	s7, 360(sp)                     # 8-byte Folded Reload
	ld	s8, 352(sp)                     # 8-byte Folded Reload
	ld	s9, 344(sp)                     # 8-byte Folded Reload
	ld	s10, 336(sp)                    # 8-byte Folded Reload
	ld	s11, 328(sp)                    # 8-byte Folded Reload
	addi	sp, sp, 432
	ret
.Lfunc_end0:
	.size	_Z14sha512_invokerPmPKSt4byteS2_, .Lfunc_end0-_Z14sha512_invokerPmPKSt4byteS2_
	.cfi_endproc
                                        # -- End function
	.type	_ZN7fast_io7details6sha51210k512scalarE,@object # @_ZN7fast_io7details6sha51210k512scalarE
	.section	.rodata._ZN7fast_io7details6sha51210k512scalarE,"aG",@progbits,_ZN7fast_io7details6sha51210k512scalarE,comdat
	.weak	_ZN7fast_io7details6sha51210k512scalarE
	.p2align	3, 0x0
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
	.ident	"clang version 20.0.0git (git@github.com:trcrsired/llvm-project.git 1ccbabd27406055793ca2fd73a6fd2f0d9474828)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
