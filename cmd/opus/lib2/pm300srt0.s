#@(#)CLIPsrt0.s	1.2 10/31/86
# Startup code for standalone programs

	.set	TOPSYS,4096*255		# TOP of system memory, at 1MegaByte
					# less 4k.
					# set stack to here, grow toward _end

	.set	HIGH,0x0		# Initial value for SSW
	.set	PRIV,0x0		# Initial value for PSW
	.set	SS_M,0x4000000		# Virtual memory

	.set	UNUSED,4096*4		# Space consumed by I/O, boot pages
	.set	SEGLEN,4096		# Length of segment table
	.set	PGLEN,4096		# Length of page table

# Memory mapped I/O locations
	.set	TR_SV,0x4		# Invalidate Supervisor TLB
	.set	TR_UV,0x8		# Invalidate User TLB
	.set	RESETD,0x4980		# Reset register, data cache
	.set	RESETI,0x4b80		# Reset register, instruction cache
	.set	SSTOD,0x4904		# Supervisor Seg Table, data cache
	.set	SSTOI,0x4b04		# Supervisor Seg Table, instr. cache
	.set	DFAULT,0x4910		# D-CAMMU fault register
	.set	IFAULT,0x4b10		# I-CAMMU fault register
	.set	USTOD,0x4908
	.set	USTOI,0x4b08
	.set	CNTLD,0x4940
	.set	CNTLI,0x4b40

# Page table entry definitions
	.set	PG_PF,0x1		# Page fault
# Caching schemes
	.set	PG_CWT,0x200		# Common, Write Thru
#	.set	PG_PCB,0x400		# Private, Copy Back
	.set	PG_PCB,0x600		# Force to Non Cachable
	.set	PG_NCA,0x600		# Non Cachable
	.set	PG_MMIO,0x800		# Non Cachable, Memory Mapped I/O
	.set	PG_BSTR,0xa00		# Non Cachable, Bootstrap Area
	.set	PG_OPNCA,0x60000000	# opmon NCA bits

	.set	CUST_NCA,0x30
	.set	CNTL_EWIR,8
	.set	CNTL_EWIW,4
	.set	CNTL_EWCW,2
	.set	CNTL_EP,0

# Data bits in reset register
	.set	CR_LVW,0x1	# Reset all LVw bits in cache
	.set	CR_LVX,0x2	# Reset all LVx bits in cache
	.set	TR_SV,0x4	# Reset all SV bits in TLB
	.set	TR_UV,0x8	# Reset all UV bits in TLB
	.set	TR_D,0x10	# Reset all D bits in TLB
	.set	TR_R,0x20	# Reset all R bits in TLB
	.set	CR_U,0x40	# Reset all U bits in cache


# Protections.  We assume that the K bit is zero in the SSW.
	.set	PG_RE,0x60		# Read, Execute
	.set	PG_RWE,0x38		# Read, Write, Execute
	.set	PG_RW,0x18		# Read, Write

# Opus specific
	.set	COMMPAGE,0x2000		# communications page
	.set	MAGIC_PM300,0x4321
	.set	HPAGE,0x5ffff000	# ppn of hardware registers
	.set	F_AT,0x16
	.set	CLR_AT,6
	.set	CLR_CP,4
	.set	CR0,0x9ffc		# hardware register page
	.set	SET,1
	.set	AT_IO_SPACE,0x60000000
	.set	AT_MEM_SPACE,0x80000000
	.set	MAPINITWORD,0x91a	# PG_REW|PG_LOCK|PGC_MMIO|PM_RW

	.globl	start
	.globl	done
	.globl	_edata
	.globl	_end
	.globl	_main
	.globl	__init
	.globl	_exit
	.globl	__exit
	.globl	_trap
	.globl	__r
	.globl	__opus
	.globl	_environ
	.globl	__mempte
	.globl	__memsize
	.globl	__mapit
	.globl	__pagetable
	.globl	__dmapage
	.globl	__maxmem

	.text

# PAGE 0
#	The vector table

vectors:
	b	0x2400			# branch to startup code
	bpt
	.align	0x8
	.long	_trap_008		# trap 8 (1)
	.long	0
	.long	_trap_010		# trap 10 (2)
	.long	0
	.long	_trap_018		# trap 18 (3)
	.long	0
	.long	_trap_020		# trap 20 (4)
	.long	0
	.long	_trap_028		# trap 28 (5)
	.long	0
	.long	_trap_030		# trap 30 (6)
	.long	0
	.long	_trap_038		# trap 38 (7)
	.long	0
	.long	_trap_040		# trap 40 (8)
	.long	0
	.long	_trap_048		# trap 48 (9)
	.long	0
	.long	_trap_050		# trap 50 (10)
	.long	0
	.long	_trap_058		# trap 58 (11)
	.long	0
	.long	_trap_060		# trap 60 (12)
	.long	0
	.long	_trap_068		# trap 68 (13)
	.long	0
	.long	_trap_070		# trap 70 (14)
	.long	0
	.long	_trap_078		# trap 78 (15)
	.long	0
	.long	_trap_080		# trap 80 (16)
	.long	0
	.long	_trap_088		# trap 88 (17)
	.long	0
	.long	_trap_090		# trap 90 (18)
	.long	0
	.long	_trap_098		# trap 98 (19)
	.long	0
	.long	_trap_0a0		# trap a0 (20)
	.long	0
	.long	_trap_0a8		# trap a8 (21)
	.long	0
	.long	_trap_0b0		# trap b0 (22)
	.long	0
	.long	_trap_0b8		# trap b8 (23)
	.long	0
	.long	_trap_0c0		# trap c0 (24)
	.long	0
	.long	_trap_0c8		# trap c8 (25)
	.long	0
	.long	_trap_0d0		# trap d0 (26)
	.long	0
	.long	_trap_0d8		# trap d8 (27)
	.long	0
	.long	_trap_0e0		# trap e0 (28)
	.long	0
	.long	_trap_0e8		# trap e8 (29)
	.long	0
	.long	_trap_0f0		# trap f0 (30)
	.long	0
	.long	_trap_0f8		# trap f8 (31)
	.long	0
	.long	_trap_100		# trap 100 (32)
	.long	0
	.long	_trap_108		# trap 108 (33)
	.long	0
	.long	_trap_110		# trap 110 (34)
	.long	0
	.long	_trap_118		# trap 118 (35)
	.long	0
	.long	_trap_120		# trap 120 (36)
	.long	0
	.long	_trap_128		# trap 128 (37)
	.long	0
	.long	_trap_130		# trap 130 (38)
	.long	0
	.long	_trap_138		# trap 138 (39)
	.long	0
	.long	_trap_140		# trap 140 (40)
	.long	0
	.long	_trap_148		# trap 148 (41)
	.long	0
	.long	_trap_150		# trap 150 (42)
	.long	0
	.long	_trap_158		# trap 158 (43)
	.long	0
	.long	_trap_160		# trap 160 (44)
	.long	0
	.long	_trap_168		# trap 168 (45)
	.long	0
	.long	_trap_170		# trap 170 (46)
	.long	0
	.long	_trap_178		# trap 178 (47)
	.long	0
	.long	_trap_180		# trap 180 (48)
	.long	0
	.long	_trap_188		# trap 188 (49)
	.long	0
	.long	_trap_190		# trap 190 (50)
	.long	0
	.long	_trap_198		# trap 198 (51)
	.long	0
	.long	_trap_1a0		# trap 1a0 (52)
	.long	0
	.long	_trap_1a8		# trap 1a8 (53)
	.long	0
	.long	_trap_1b0		# trap 1b0 (54)
	.long	0
	.long	_trap_1b8		# trap 1b8 (55)
	.long	0
	.long	_trap_1c0		# trap 1c0 (56)
	.long	0
	.long	_trap_1c8		# trap 1c8 (57)
	.long	0
	.long	_trap_1d0		# trap 1d0 (58)
	.long	0
	.long	_trap_1d8		# trap 1d8 (59)
	.long	0
	.long	_trap_1e0		# trap 1e0 (60)
	.long	0
	.long	_trap_1e8		# trap 1e8 (61)
	.long	0
	.long	_trap_1f0		# trap 1f0 (62)
	.long	0
	.long	_trap_1f8		# trap 1f8 (63)
	.long	0
	.long	_trap_200		# trap 200 (64)
	.long	0
	.long	_trap_208		# trap 208 (65)
	.long	0
	.long	_trap_210		# trap 210 (66)
	.long	0
	.long	_trap_218		# trap 218 (67)
	.long	0
	.long	_trap_220		# trap 220 (68)
	.long	0
	.long	_trap_228		# trap 228 (69)
	.long	0
	.long	_trap_230		# trap 230 (70)
	.long	0
	.long	_trap_238		# trap 238 (71)
	.long	0
	.long	_trap_240		# trap 240 (72)
	.long	0
	.long	_trap_248		# trap 248 (73)
	.long	0
	.long	_trap_250		# trap 250 (74)
	.long	0
	.long	_trap_258		# trap 258 (75)
	.long	0
	.long	_trap_260		# trap 260 (76)
	.long	0
	.long	_trap_268		# trap 268 (77)
	.long	0
	.long	_trap_270		# trap 270 (78)
	.long	0
	.long	_trap_278		# trap 278 (79)
	.long	0
	.long	_trap_280		# trap 280 (80)
	.long	0
	.long	_trap_288		# trap 288 (81)
	.long	0
	.long	_trap_290		# trap 290 (82)
	.long	0
	.long	_trap_298		# trap 298 (83)
	.long	0
	.long	_trap_2a0		# trap 2a0 (84)
	.long	0
	.long	_trap_2a8		# trap 2a8 (85)
	.long	0
	.long	_trap_2b0		# trap 2b0 (86)
	.long	0
	.long	_trap_2b8		# trap 2b8 (87)
	.long	0
	.long	_trap_2c0		# trap 2c0 (88)
	.long	0
	.long	_trap_2c8		# trap 2c8 (89)
	.long	0
	.long	_trap_2d0		# trap 2d0 (90)
	.long	0
	.long	_trap_2d8		# trap 2d8 (91)
	.long	0
	.long	_trap_2e0		# trap 2e0 (92)
	.long	0
	.long	_trap_2e8		# trap 2e8 (93)
	.long	0
	.long	_trap_2f0		# trap 2f0 (94)
	.long	0
	.long	_trap_2f8		# trap 2f8 (95)
	.long	0
	.long	_trap_300		# trap 300 (96)
	.long	0
	.long	_trap_308		# trap 308 (97)
	.long	0
	.long	_trap_310		# trap 310 (98)
	.long	0
	.long	_trap_318		# trap 318 (99)
	.long	0
	.long	_trap_320		# trap 320 (100)
	.long	0
	.long	_trap_328		# trap 328 (101)
	.long	0
	.long	_trap_330		# trap 330 (102)
	.long	0
	.long	_trap_338		# trap 338 (103)
	.long	0
	.long	_trap_340		# trap 340 (104)
	.long	0
	.long	_trap_348		# trap 348 (105)
	.long	0
	.long	_trap_350		# trap 350 (106)
	.long	0
	.long	_trap_358		# trap 358 (107)
	.long	0
	.long	_trap_360		# trap 360 (108)
	.long	0
	.long	_trap_368		# trap 368 (109)
	.long	0
	.long	_trap_370		# trap 370 (110)
	.long	0
	.long	_trap_378		# trap 378 (111)
	.long	0
	.long	_trap_380		# trap 380 (112)
	.long	0
	.long	_trap_388		# trap 388 (113)
	.long	0
	.long	_trap_390		# trap 390 (114)
	.long	0
	.long	_trap_398		# trap 398 (115)
	.long	0
	.long	_trap_3a0		# trap 3a0 (116)
	.long	0
	.long	_trap_3a8		# trap 3a8 (117)
	.long	0
	.long	_trap_3b0		# trap 3b0 (118)
	.long	0
	.long	_trap_3b8		# trap 3b8 (119)
	.long	0
	.long	_trap_3c0		# trap 3c0 (120)
	.long	0
	.long	_trap_3c8		# trap 3c8 (121)
	.long	0
	.long	_trap_3d0		# trap 3d0 (122)
	.long	0
	.long	_trap_3d8		# trap 3d8 (123)
	.long	0
	.long	_trap_3e0		# trap 3e0 (124)
	.long	0
	.long	_trap_3e8		# trap 3e8 (125)
	.long	0
	.long	_trap_3f0		# trap 3f0 (126)
	.long	0
	.long	_trap_3f8		# trap 3f8 (127)
	.long	0
	.long	_trap_400		# trap 400 (128)
	.long	0
	.long	_trap_408		# trap 408 (129)
	.long	0
	.long	_trap_410		# trap 410 (130)
	.long	0
	.long	_trap_418		# trap 418 (131)
	.long	0
	.long	_trap_420		# trap 420 (132)
	.long	0
	.long	_trap_428		# trap 428 (133)
	.long	0
	.long	_trap_430		# trap 430 (134)
	.long	0
	.long	_trap_438		# trap 438 (135)
	.long	0
	.long	_trap_440		# trap 440 (136)
	.long	0
	.long	_trap_448		# trap 448 (137)
	.long	0
	.long	_trap_450		# trap 450 (138)
	.long	0
	.long	_trap_458		# trap 458 (139)
	.long	0
	.long	_trap_460		# trap 460 (140)
	.long	0
	.long	_trap_468		# trap 468 (141)
	.long	0
	.long	_trap_470		# trap 470 (142)
	.long	0
	.long	_trap_478		# trap 478 (143)
	.long	0
	.long	_trap_480		# trap 480 (144)
	.long	0
	.long	_trap_488		# trap 488 (145)
	.long	0
	.long	_trap_490		# trap 490 (146)
	.long	0
	.long	_trap_498		# trap 498 (147)
	.long	0
	.long	_trap_4a0		# trap 4a0 (148)
	.long	0
	.long	_trap_4a8		# trap 4a8 (149)
	.long	0
	.long	_trap_4b0		# trap 4b0 (150)
	.long	0
	.long	_trap_4b8		# trap 4b8 (151)
	.long	0
	.long	_trap_4c0		# trap 4c0 (152)
	.long	0
	.long	_trap_4c8		# trap 4c8 (153)
	.long	0
	.long	_trap_4d0		# trap 4d0 (154)
	.long	0
	.long	_trap_4d8		# trap 4d8 (155)
	.long	0
	.long	_trap_4e0		# trap 4e0 (156)
	.long	0
	.long	_trap_4e8		# trap 4e8 (157)
	.long	0
	.long	_trap_4f0		# trap 4f0 (158)
	.long	0
	.long	_trap_4f8		# trap 4f8 (159)
	.long	0
	.long	_trap_500		# trap 500 (160)
	.long	0
	.long	_trap_508		# trap 508 (161)
	.long	0
	.long	_trap_510		# trap 510 (162)
	.long	0
	.long	_trap_518		# trap 518 (163)
	.long	0
	.long	_trap_520		# trap 520 (164)
	.long	0
	.long	_trap_528		# trap 528 (165)
	.long	0
	.long	_trap_530		# trap 530 (166)
	.long	0
	.long	_trap_538		# trap 538 (167)
	.long	0
	.long	_trap_540		# trap 540 (168)
	.long	0
	.long	_trap_548		# trap 548 (169)
	.long	0
	.long	_trap_550		# trap 550 (170)
	.long	0
	.long	_trap_558		# trap 558 (171)
	.long	0
	.long	_trap_560		# trap 560 (172)
	.long	0
	.long	_trap_568		# trap 568 (173)
	.long	0
	.long	_trap_570		# trap 570 (174)
	.long	0
	.long	_trap_578		# trap 578 (175)
	.long	0
	.long	_trap_580		# trap 580 (176)
	.long	0
	.long	_trap_588		# trap 588 (177)
	.long	0
	.long	_trap_590		# trap 590 (178)
	.long	0
	.long	_trap_598		# trap 598 (179)
	.long	0
	.long	_trap_5a0		# trap 5a0 (180)
	.long	0
	.long	_trap_5a8		# trap 5a8 (181)
	.long	0
	.long	_trap_5b0		# trap 5b0 (182)
	.long	0
	.long	_trap_5b8		# trap 5b8 (183)
	.long	0
	.long	_trap_5c0		# trap 5c0 (184)
	.long	0
	.long	_trap_5c8		# trap 5c8 (185)
	.long	0
	.long	_trap_5d0		# trap 5d0 (186)
	.long	0
	.long	_trap_5d8		# trap 5d8 (187)
	.long	0
	.long	_trap_5e0		# trap 5e0 (188)
	.long	0
	.long	_trap_5e8		# trap 5e8 (189)
	.long	0
	.long	_trap_5f0		# trap 5f0 (190)
	.long	0
	.long	_trap_5f8		# trap 5f8 (191)
	.long	0
	.long	_trap_600		# trap 600 (192)
	.long	0
	.long	_trap_608		# trap 608 (193)
	.long	0
	.long	_trap_610		# trap 610 (194)
	.long	0
	.long	_trap_618		# trap 618 (195)
	.long	0
	.long	_trap_620		# trap 620 (196)
	.long	0
	.long	_trap_628		# trap 628 (197)
	.long	0
	.long	_trap_630		# trap 630 (198)
	.long	0
	.long	_trap_638		# trap 638 (199)
	.long	0
	.long	_trap_640		# trap 640 (200)
	.long	0
	.long	_trap_648		# trap 648 (201)
	.long	0
	.long	_trap_650		# trap 650 (202)
	.long	0
	.long	_trap_658		# trap 658 (203)
	.long	0
	.long	_trap_660		# trap 660 (204)
	.long	0
	.long	_trap_668		# trap 668 (205)
	.long	0
	.long	_trap_670		# trap 670 (206)
	.long	0
	.long	_trap_678		# trap 678 (207)
	.long	0
	.long	_trap_680		# trap 680 (208)
	.long	0
	.long	_trap_688		# trap 688 (209)
	.long	0
	.long	_trap_690		# trap 690 (210)
	.long	0
	.long	_trap_698		# trap 698 (211)
	.long	0
	.long	_trap_6a0		# trap 6a0 (212)
	.long	0
	.long	_trap_6a8		# trap 6a8 (213)
	.long	0
	.long	_trap_6b0		# trap 6b0 (214)
	.long	0
	.long	_trap_6b8		# trap 6b8 (215)
	.long	0
	.long	_trap_6c0		# trap 6c0 (216)
	.long	0
	.long	_trap_6c8		# trap 6c8 (217)
	.long	0
	.long	_trap_6d0		# trap 6d0 (218)
	.long	0
	.long	_trap_6d8		# trap 6d8 (219)
	.long	0
	.long	_trap_6e0		# trap 6e0 (220)
	.long	0
	.long	_trap_6e8		# trap 6e8 (221)
	.long	0
	.long	_trap_6f0		# trap 6f0 (222)
	.long	0
	.long	_trap_6f8		# trap 6f8 (223)
	.long	0
	.long	_trap_700		# trap 700 (224)
	.long	0
	.long	_trap_708		# trap 708 (225)
	.long	0
	.long	_trap_710		# trap 710 (226)
	.long	0
	.long	_trap_718		# trap 718 (227)
	.long	0
	.long	_trap_720		# trap 720 (228)
	.long	0
	.long	_trap_728		# trap 728 (229)
	.long	0
	.long	_trap_730		# trap 730 (230)
	.long	0
	.long	_trap_738		# trap 738 (231)
	.long	0
	.long	_trap_740		# trap 740 (232)
	.long	0
	.long	_trap_748		# trap 748 (233)
	.long	0
	.long	_trap_750		# trap 750 (234)
	.long	0
	.long	_trap_758		# trap 758 (235)
	.long	0
	.long	_trap_760		# trap 760 (236)
	.long	0
	.long	_trap_768		# trap 768 (237)
	.long	0
	.long	_trap_770		# trap 770 (238)
	.long	0
	.long	_trap_778		# trap 778 (239)
	.long	0
	.long	_trap_780		# trap 780 (240)
	.long	0
	.long	_trap_788		# trap 788 (241)
	.long	0
	.long	_trap_790		# trap 790 (242)
	.long	0
	.long	_trap_798		# trap 798 (243)
	.long	0
	.long	_trap_7a0		# trap 7a0 (244)
	.long	0
	.long	_trap_7a8		# trap 7a8 (245)
	.long	0
	.long	_trap_7b0		# trap 7b0 (246)
	.long	0
	.long	_trap_7b8		# trap 7b8 (247)
	.long	0
	.long	_trap_7c0		# trap 7c0 (248)
	.long	0
	.long	_trap_7c8		# trap 7c8 (249)
	.long	0
	.long	_trap_7d0		# trap 7d0 (250)
	.long	0
	.long	_trap_7d8		# trap 7d8 (251)
	.long	0
	.long	_trap_7e0		# trap 7e0 (252)
	.long	0
	.long	_trap_7e8		# trap 7e8 (253)
	.long	0
	.long	_trap_7f0		# trap 7f0 (254)
	.long	0
	.long	_trap_7f8		# trap 7f8 (255)
	.long	0
	.long	_intr_800		# interrupt 800 (256)
	.long	0
	.long	_intr_808		# interrupt 808 (257)
	.long	0
	.long	_intr_810		# interrupt 810 (258)
	.long	0
	.long	_intr_818		# interrupt 818 (259)
	.long	0
	.long	_intr_820		# interrupt 820 (260)
	.long	0
	.long	_intr_828		# interrupt 828 (261)
	.long	0
	.long	_intr_830		# interrupt 830 (262)
	.long	0
	.long	_intr_838		# interrupt 838 (263)
	.long	0
	.long	_intr_840		# interrupt 840 (264)
	.long	0
	.long	_intr_848		# interrupt 848 (265)
	.long	0
	.long	_intr_850		# interrupt 850 (266)
	.long	0
	.long	_intr_858		# interrupt 858 (267)
	.long	0
	.long	_intr_860		# interrupt 860 (268)
	.long	0
	.long	_intr_868		# interrupt 868 (269)
	.long	0
	.long	_intr_870		# interrupt 870 (270)
	.long	0
	.long	_intr_878		# interrupt 878 (271)
	.long	0
	.long	_intr_880		# interrupt 880 (272)
	.long	0
	.long	_intr_888		# interrupt 888 (273)
	.long	0
	.long	_intr_890		# interrupt 890 (274)
	.long	0
	.long	_intr_898		# interrupt 898 (275)
	.long	0
	.long	_intr_8a0		# interrupt 8a0 (276)
	.long	0
	.long	_intr_8a8		# interrupt 8a8 (277)
	.long	0
	.long	_intr_8b0		# interrupt 8b0 (278)
	.long	0
	.long	_intr_8b8		# interrupt 8b8 (279)
	.long	0
	.long	_intr_8c0		# interrupt 8c0 (280)
	.long	0
	.long	_intr_8c8		# interrupt 8c8 (281)
	.long	0
	.long	_intr_8d0		# interrupt 8d0 (282)
	.long	0
	.long	_intr_8d8		# interrupt 8d8 (283)
	.long	0
	.long	_intr_8e0		# interrupt 8e0 (284)
	.long	0
	.long	_intr_8e8		# interrupt 8e8 (285)
	.long	0
	.long	_intr_8f0		# interrupt 8f0 (286)
	.long	0
	.long	_intr_8f8		# interrupt 8f8 (287)
	.long	0
	.long	_intr_900		# interrupt 900 (288)
	.long	0
	.long	_intr_908		# interrupt 908 (289)
	.long	0
	.long	_intr_910		# interrupt 910 (290)
	.long	0
	.long	_intr_918		# interrupt 918 (291)
	.long	0
	.long	_intr_920		# interrupt 920 (292)
	.long	0
	.long	_intr_928		# interrupt 928 (293)
	.long	0
	.long	_intr_930		# interrupt 930 (294)
	.long	0
	.long	_intr_938		# interrupt 938 (295)
	.long	0
	.long	_intr_940		# interrupt 940 (296)
	.long	0
	.long	_intr_948		# interrupt 948 (297)
	.long	0
	.long	_intr_950		# interrupt 950 (298)
	.long	0
	.long	_intr_958		# interrupt 958 (299)
	.long	0
	.long	_intr_960		# interrupt 960 (300)
	.long	0
	.long	_intr_968		# interrupt 968 (301)
	.long	0
	.long	_intr_970		# interrupt 970 (302)
	.long	0
	.long	_intr_978		# interrupt 978 (303)
	.long	0
	.long	_intr_980		# interrupt 980 (304)
	.long	0
	.long	_intr_988		# interrupt 988 (305)
	.long	0
	.long	_intr_990		# interrupt 990 (306)
	.long	0
	.long	_intr_998		# interrupt 998 (307)
	.long	0
	.long	_intr_9a0		# interrupt 9a0 (308)
	.long	0
	.long	_intr_9a8		# interrupt 9a8 (309)
	.long	0
	.long	_intr_9b0		# interrupt 9b0 (310)
	.long	0
	.long	_intr_9b8		# interrupt 9b8 (311)
	.long	0
	.long	_intr_9c0		# interrupt 9c0 (312)
	.long	0
	.long	_intr_9c8		# interrupt 9c8 (313)
	.long	0
	.long	_intr_9d0		# interrupt 9d0 (314)
	.long	0
	.long	_intr_9d8		# interrupt 9d8 (315)
	.long	0
	.long	_intr_9e0		# interrupt 9e0 (316)
	.long	0
	.long	_intr_9e8		# interrupt 9e8 (317)
	.long	0
	.long	_intr_9f0		# interrupt 9f0 (318)
	.long	0
	.long	_intr_9f8		# interrupt 9f8 (319)
	.long	0
	.long	_intr_a00		# interrupt a00 (320)
	.long	0
	.long	_intr_a08		# interrupt a08 (321)
	.long	0
	.long	_intr_a10		# interrupt a10 (322)
	.long	0
	.long	_intr_a18		# interrupt a18 (323)
	.long	0
	.long	_intr_a20		# interrupt a20 (324)
	.long	0
	.long	_intr_a28		# interrupt a28 (325)
	.long	0
	.long	_intr_a30		# interrupt a30 (326)
	.long	0
	.long	_intr_a38		# interrupt a38 (327)
	.long	0
	.long	_intr_a40		# interrupt a40 (328)
	.long	0
	.long	_intr_a48		# interrupt a48 (329)
	.long	0
	.long	_intr_a50		# interrupt a50 (330)
	.long	0
	.long	_intr_a58		# interrupt a58 (331)
	.long	0
	.long	_intr_a60		# interrupt a60 (332)
	.long	0
	.long	_intr_a68		# interrupt a68 (333)
	.long	0
	.long	_intr_a70		# interrupt a70 (334)
	.long	0
	.long	_intr_a78		# interrupt a78 (335)
	.long	0
	.long	_intr_a80		# interrupt a80 (336)
	.long	0
	.long	_intr_a88		# interrupt a88 (337)
	.long	0
	.long	_intr_a90		# interrupt a90 (338)
	.long	0
	.long	_intr_a98		# interrupt a98 (339)
	.long	0
	.long	_intr_aa0		# interrupt aa0 (340)
	.long	0
	.long	_intr_aa8		# interrupt aa8 (341)
	.long	0
	.long	_intr_ab0		# interrupt ab0 (342)
	.long	0
	.long	_intr_ab8		# interrupt ab8 (343)
	.long	0
	.long	_intr_ac0		# interrupt ac0 (344)
	.long	0
	.long	_intr_ac8		# interrupt ac8 (345)
	.long	0
	.long	_intr_ad0		# interrupt ad0 (346)
	.long	0
	.long	_intr_ad8		# interrupt ad8 (347)
	.long	0
	.long	_intr_ae0		# interrupt ae0 (348)
	.long	0
	.long	_intr_ae8		# interrupt ae8 (349)
	.long	0
	.long	_intr_af0		# interrupt af0 (350)
	.long	0
	.long	_intr_af8		# interrupt af8 (351)
	.long	0
	.long	_intr_b00		# interrupt b00 (352)
	.long	0
	.long	_intr_b08		# interrupt b08 (353)
	.long	0
	.long	_intr_b10		# interrupt b10 (354)
	.long	0
	.long	_intr_b18		# interrupt b18 (355)
	.long	0
	.long	_intr_b20		# interrupt b20 (356)
	.long	0
	.long	_intr_b28		# interrupt b28 (357)
	.long	0
	.long	_intr_b30		# interrupt b30 (358)
	.long	0
	.long	_intr_b38		# interrupt b38 (359)
	.long	0
	.long	_intr_b40		# interrupt b40 (360)
	.long	0
	.long	_intr_b48		# interrupt b48 (361)
	.long	0
	.long	_intr_b50		# interrupt b50 (362)
	.long	0
	.long	_intr_b58		# interrupt b58 (363)
	.long	0
	.long	_intr_b60		# interrupt b60 (364)
	.long	0
	.long	_intr_b68		# interrupt b68 (365)
	.long	0
	.long	_intr_b70		# interrupt b70 (366)
	.long	0
	.long	_intr_b78		# interrupt b78 (367)
	.long	0
	.long	_intr_b80		# interrupt b80 (368)
	.long	0
	.long	_intr_b88		# interrupt b88 (369)
	.long	0
	.long	_intr_b90		# interrupt b90 (370)
	.long	0
	.long	_intr_b98		# interrupt b98 (371)
	.long	0
	.long	_intr_ba0		# interrupt ba0 (372)
	.long	0
	.long	_intr_ba8		# interrupt ba8 (373)
	.long	0
	.long	_intr_bb0		# interrupt bb0 (374)
	.long	0
	.long	_intr_bb8		# interrupt bb8 (375)
	.long	0
	.long	_intr_bc0		# interrupt bc0 (376)
	.long	0
	.long	_intr_bc8		# interrupt bc8 (377)
	.long	0
	.long	_intr_bd0		# interrupt bd0 (378)
	.long	0
	.long	_intr_bd8		# interrupt bd8 (379)
	.long	0
	.long	_intr_be0		# interrupt be0 (380)
	.long	0
	.long	_intr_be8		# interrupt be8 (381)
	.long	0
	.long	_intr_bf0		# interrupt bf0 (382)
	.long	0
	.long	_intr_bf8		# interrupt bf8 (383)
	.long	0
	.long	_intr_c00		# interrupt c00 (384)
	.long	0
	.long	_intr_c08		# interrupt c08 (385)
	.long	0
	.long	_intr_c10		# interrupt c10 (386)
	.long	0
	.long	_intr_c18		# interrupt c18 (387)
	.long	0
	.long	_intr_c20		# interrupt c20 (388)
	.long	0
	.long	_intr_c28		# interrupt c28 (389)
	.long	0
	.long	_intr_c30		# interrupt c30 (390)
	.long	0
	.long	_intr_c38		# interrupt c38 (391)
	.long	0
	.long	_intr_c40		# interrupt c40 (392)
	.long	0
	.long	_intr_c48		# interrupt c48 (393)
	.long	0
	.long	_intr_c50		# interrupt c50 (394)
	.long	0
	.long	_intr_c58		# interrupt c58 (395)
	.long	0
	.long	_intr_c60		# interrupt c60 (396)
	.long	0
	.long	_intr_c68		# interrupt c68 (397)
	.long	0
	.long	_intr_c70		# interrupt c70 (398)
	.long	0
	.long	_intr_c78		# interrupt c78 (399)
	.long	0
	.long	_intr_c80		# interrupt c80 (400)
	.long	0
	.long	_intr_c88		# interrupt c88 (401)
	.long	0
	.long	_intr_c90		# interrupt c90 (402)
	.long	0
	.long	_intr_c98		# interrupt c98 (403)
	.long	0
	.long	_intr_ca0		# interrupt ca0 (404)
	.long	0
	.long	_intr_ca8		# interrupt ca8 (405)
	.long	0
	.long	_intr_cb0		# interrupt cb0 (406)
	.long	0
	.long	_intr_cb8		# interrupt cb8 (407)
	.long	0
	.long	_intr_cc0		# interrupt cc0 (408)
	.long	0
	.long	_intr_cc8		# interrupt cc8 (409)
	.long	0
	.long	_intr_cd0		# interrupt cd0 (410)
	.long	0
	.long	_intr_cd8		# interrupt cd8 (411)
	.long	0
	.long	_intr_ce0		# interrupt ce0 (412)
	.long	0
	.long	_intr_ce8		# interrupt ce8 (413)
	.long	0
	.long	_intr_cf0		# interrupt cf0 (414)
	.long	0
	.long	_intr_cf8		# interrupt cf8 (415)
	.long	0
	.long	_intr_d00		# interrupt d00 (416)
	.long	0
	.long	_intr_d08		# interrupt d08 (417)
	.long	0
	.long	_intr_d10		# interrupt d10 (418)
	.long	0
	.long	_intr_d18		# interrupt d18 (419)
	.long	0
	.long	_intr_d20		# interrupt d20 (420)
	.long	0
	.long	_intr_d28		# interrupt d28 (421)
	.long	0
	.long	_intr_d30		# interrupt d30 (422)
	.long	0
	.long	_intr_d38		# interrupt d38 (423)
	.long	0
	.long	_intr_d40		# interrupt d40 (424)
	.long	0
	.long	_intr_d48		# interrupt d48 (425)
	.long	0
	.long	_intr_d50		# interrupt d50 (426)
	.long	0
	.long	_intr_d58		# interrupt d58 (427)
	.long	0
	.long	_intr_d60		# interrupt d60 (428)
	.long	0
	.long	_intr_d68		# interrupt d68 (429)
	.long	0
	.long	_intr_d70		# interrupt d70 (430)
	.long	0
	.long	_intr_d78		# interrupt d78 (431)
	.long	0
	.long	_intr_d80		# interrupt d80 (432)
	.long	0
	.long	_intr_d88		# interrupt d88 (433)
	.long	0
	.long	_intr_d90		# interrupt d90 (434)
	.long	0
	.long	_intr_d98		# interrupt d98 (435)
	.long	0
	.long	_intr_da0		# interrupt da0 (436)
	.long	0
	.long	_intr_da8		# interrupt da8 (437)
	.long	0
	.long	_intr_db0		# interrupt db0 (438)
	.long	0
	.long	_intr_db8		# interrupt db8 (439)
	.long	0
	.long	_intr_dc0		# interrupt dc0 (440)
	.long	0
	.long	_intr_dc8		# interrupt dc8 (441)
	.long	0
	.long	_intr_dd0		# interrupt dd0 (442)
	.long	0
	.long	_intr_dd8		# interrupt dd8 (443)
	.long	0
	.long	_intr_de0		# interrupt de0 (444)
	.long	0
	.long	_intr_de8		# interrupt de8 (445)
	.long	0
	.long	_intr_df0		# interrupt df0 (446)
	.long	0
	.long	_intr_df8		# interrupt df8 (447)
	.long	0
	.long	_intr_e00		# interrupt e00 (448)
	.long	0
	.long	_intr_e08		# interrupt e08 (449)
	.long	0
	.long	_intr_e10		# interrupt e10 (450)
	.long	0
	.long	_intr_e18		# interrupt e18 (451)
	.long	0
	.long	_intr_e20		# interrupt e20 (452)
	.long	0
	.long	_intr_e28		# interrupt e28 (453)
	.long	0
	.long	_intr_e30		# interrupt e30 (454)
	.long	0
	.long	_intr_e38		# interrupt e38 (455)
	.long	0
	.long	_intr_e40		# interrupt e40 (456)
	.long	0
	.long	_intr_e48		# interrupt e48 (457)
	.long	0
	.long	_intr_e50		# interrupt e50 (458)
	.long	0
	.long	_intr_e58		# interrupt e58 (459)
	.long	0
	.long	_intr_e60		# interrupt e60 (460)
	.long	0
	.long	_intr_e68		# interrupt e68 (461)
	.long	0
	.long	_intr_e70		# interrupt e70 (462)
	.long	0
	.long	_intr_e78		# interrupt e78 (463)
	.long	0
	.long	_intr_e80		# interrupt e80 (464)
	.long	0
	.long	_intr_e88		# interrupt e88 (465)
	.long	0
	.long	_intr_e90		# interrupt e90 (466)
	.long	0
	.long	_intr_e98		# interrupt e98 (467)
	.long	0
	.long	_intr_ea0		# interrupt ea0 (468)
	.long	0
	.long	_intr_ea8		# interrupt ea8 (469)
	.long	0
	.long	_intr_eb0		# interrupt eb0 (470)
	.long	0
	.long	_intr_eb8		# interrupt eb8 (471)
	.long	0
	.long	_intr_ec0		# interrupt ec0 (472)
	.long	0
	.long	_intr_ec8		# interrupt ec8 (473)
	.long	0
	.long	_intr_ed0		# interrupt ed0 (474)
	.long	0
	.long	_intr_ed8		# interrupt ed8 (475)
	.long	0
	.long	_intr_ee0		# interrupt ee0 (476)
	.long	0
	.long	_intr_ee8		# interrupt ee8 (477)
	.long	0
	.long	_intr_ef0		# interrupt ef0 (478)
	.long	0
	.long	_intr_ef8		# interrupt ef8 (479)
	.long	0
	.long	_intr_f00		# interrupt f00 (480)
	.long	0
	.long	_intr_f08		# interrupt f08 (481)
	.long	0
	.long	_intr_f10		# interrupt f10 (482)
	.long	0
	.long	_intr_f18		# interrupt f18 (483)
	.long	0
	.long	_intr_f20		# interrupt f20 (484)
	.long	0
	.long	_intr_f28		# interrupt f28 (485)
	.long	0
	.long	_intr_f30		# interrupt f30 (486)
	.long	0
	.long	_intr_f38		# interrupt f38 (487)
	.long	0
	.long	_intr_f40		# interrupt f40 (488)
	.long	0
	.long	_intr_f48		# interrupt f48 (489)
	.long	0
	.long	_intr_f50		# interrupt f50 (490)
	.long	0
	.long	_intr_f58		# interrupt f58 (491)
	.long	0
	.long	_intr_f60		# interrupt f60 (492)
	.long	0
	.long	_intr_f68		# interrupt f68 (493)
	.long	0
	.long	_intr_f70		# interrupt f70 (494)
	.long	0
	.long	_intr_f78		# interrupt f78 (495)
	.long	0
	.long	_intr_f80		# interrupt f80 (496)
	.long	0
	.long	_intr_f88		# interrupt f88 (497)
	.long	0
	.long	_intr_f90		# interrupt f90 (498)
	.long	0
	.long	_intr_f98		# interrupt f98 (499)
	.long	0
	.long	_intr_fa0		# interrupt fa0 (500)
	.long	0
	.long	_intr_fa8		# interrupt fa8 (501)
	.long	0
	.long	_intr_fb0		# interrupt fb0 (502)
	.long	0
	.long	_intr_fb8		# interrupt fb8 (503)
	.long	0
	.long	_intr_fc0		# interrupt fc0 (504)
	.long	0
	.long	_intr_fc8		# interrupt fc8 (505)
	.long	0
	.long	_intr_fd0		# interrupt fd0 (506)
	.long	0
	.long	_intr_fd8		# interrupt fd8 (507)
	.long	0
	.long	_intr_fe0		# interrupt fe0 (508)
	.long	0
	.long	_intr_fe8		# interrupt fe8 (509)
	.long	0
	.long	_intr_ff0		# interrupt ff0 (510)
	.long	0
	.long	_intr_ff8		# interrupt ff8 (511)
	.long	0

# PAGE 1
#	The trap handlers

_trap_000:	#call	sp,trapc
_trap_008:	#call	sp,trapc
_trap_010:	#call	sp,trapc
_trap_018:	#call	sp,trapc
_trap_020:	#call	sp,trapc
_trap_028:	#call	sp,trapc
_trap_030:	#call	sp,trapc
_trap_038:	#call	sp,trapc
_trap_040:	#call	sp,trapc
_trap_048:	#call	sp,trapc
_trap_050:	#call	sp,trapc
_trap_058:	#call	sp,trapc
_trap_060:	#call	sp,trapc
_trap_068:	#call	sp,trapc
_trap_070:	#call	sp,trapc
_trap_078:	#call	sp,trapc
_trap_080:	#call	sp,trapc
_trap_088:	#call	sp,trapc
_trap_090:	#call	sp,trapc
_trap_098:	#call	sp,trapc
_trap_0a0:	#call	sp,trapc
_trap_0a8:	#call	sp,trapc
_trap_0b0:	#call	sp,trapc
_trap_0b8:	#call	sp,trapc
_trap_0c0:	#call	sp,trapc
_trap_0c8:	#call	sp,trapc
_trap_0d0:	#call	sp,trapc
_trap_0d8:	#call	sp,trapc
_trap_0e0:	#call	sp,trapc
_trap_0e8:	#call	sp,trapc
_trap_0f0:	#call	sp,trapc
_trap_0f8:	#call	sp,trapc
_trap_100:	#call	sp,trapc
_trap_108:	#call	sp,trapc
_trap_110:	#call	sp,trapc
_trap_118:	#call	sp,trapc
_trap_120:	#call	sp,trapc
_trap_128:	#call	sp,trapc
_trap_130:	#call	sp,trapc
_trap_138:	#call	sp,trapc
_trap_140:	#call	sp,trapc
_trap_148:	#call	sp,trapc
_trap_150:	#call	sp,trapc
_trap_158:	#call	sp,trapc
_trap_160:	#call	sp,trapc
_trap_168:	#call	sp,trapc
_trap_170:	#call	sp,trapc
_trap_178:	#call	sp,trapc
_trap_180:	#call	sp,trapc
_trap_188:	#call	sp,trapc
_trap_190:	#call	sp,trapc
_trap_198:	#call	sp,trapc
_trap_1a0:	#call	sp,trapc
_trap_1a8:	#call	sp,trapc
_trap_1b0:	#call	sp,trapc
_trap_1b8:	#call	sp,trapc
_trap_1c0:	#call	sp,trapc
_trap_1c8:	#call	sp,trapc
_trap_1d0:	#call	sp,trapc
_trap_1d8:	#call	sp,trapc
_trap_1e0:	#call	sp,trapc
_trap_1e8:	#call	sp,trapc
_trap_1f0:	#call	sp,trapc
_trap_1f8:	#call	sp,trapc
_trap_200:	#call	sp,trapc
_trap_208:	#call	sp,trapc
_trap_210:	#call	sp,trapc
_trap_218:	#call	sp,trapc
_trap_220:	#call	sp,trapc
_trap_228:	#call	sp,trapc
_trap_230:	#call	sp,trapc
_trap_238:	#call	sp,trapc
_trap_240:	#call	sp,trapc
_trap_248:	#call	sp,trapc
_trap_250:	#call	sp,trapc
_trap_258:	#call	sp,trapc
_trap_260:	#call	sp,trapc
_trap_268:	#call	sp,trapc
_trap_270:	#call	sp,trapc
_trap_278:	#call	sp,trapc
_trap_280:	#call	sp,trapc
_trap_288:	#call	sp,trapc
_trap_290:	#call	sp,trapc
_trap_298:	#call	sp,trapc
_trap_2a0:	#call	sp,trapc
_trap_2a8:	#call	sp,trapc
_trap_2b0:	#call	sp,trapc
_trap_2b8:	#call	sp,trapc
_trap_2c0:	#call	sp,trapc
_trap_2c8:	#call	sp,trapc
_trap_2d0:	#call	sp,trapc
_trap_2d8:	#call	sp,trapc
_trap_2e0:	#call	sp,trapc
_trap_2e8:	#call	sp,trapc
_trap_2f0:	#call	sp,trapc
_trap_2f8:	#call	sp,trapc
_trap_300:	#call	sp,trapc
_trap_308:	#call	sp,trapc
_trap_310:	#call	sp,trapc
_trap_318:	#call	sp,trapc
_trap_320:	#call	sp,trapc
_trap_328:	#call	sp,trapc
_trap_330:	#call	sp,trapc
_trap_338:	#call	sp,trapc
_trap_340:	#call	sp,trapc
_trap_348:	#call	sp,trapc
_trap_350:	#call	sp,trapc
_trap_358:	#call	sp,trapc
_trap_360:	#call	sp,trapc
_trap_368:	#call	sp,trapc
_trap_370:	#call	sp,trapc
_trap_378:	#call	sp,trapc
_trap_380:	#call	sp,trapc
_trap_388:	#call	sp,trapc
_trap_390:	#call	sp,trapc
_trap_398:	#call	sp,trapc
_trap_3a0:	#call	sp,trapc
_trap_3a8:	#call	sp,trapc
_trap_3b0:	#call	sp,trapc
_trap_3b8:	#call	sp,trapc
_trap_3c0:	#call	sp,trapc
_trap_3c8:	#call	sp,trapc
_trap_3d0:	#call	sp,trapc
_trap_3d8:	#call	sp,trapc
_trap_3e0:	#call	sp,trapc
_trap_3e8:	#call	sp,trapc
_trap_3f0:	#call	sp,trapc
_trap_3f8:	#call	sp,trapc
_trap_400:	#call	sp,trapc
_trap_408:	#call	sp,trapc
_trap_410:	#call	sp,trapc
_trap_418:	#call	sp,trapc
_trap_420:	#call	sp,trapc
_trap_428:	#call	sp,trapc
_trap_430:	#call	sp,trapc
_trap_438:	#call	sp,trapc
_trap_440:	#call	sp,trapc
_trap_448:	#call	sp,trapc
_trap_450:	#call	sp,trapc
_trap_458:	#call	sp,trapc
_trap_460:	#call	sp,trapc
_trap_468:	#call	sp,trapc
_trap_470:	#call	sp,trapc
_trap_478:	#call	sp,trapc
_trap_480:	#call	sp,trapc
_trap_488:	#call	sp,trapc
_trap_490:	#call	sp,trapc
_trap_498:	#call	sp,trapc
_trap_4a0:	#call	sp,trapc
_trap_4a8:	#call	sp,trapc
_trap_4b0:	#call	sp,trapc
_trap_4b8:	#call	sp,trapc
_trap_4c0:	#call	sp,trapc
_trap_4c8:	#call	sp,trapc
_trap_4d0:	#call	sp,trapc
_trap_4d8:	#call	sp,trapc
_trap_4e0:	#call	sp,trapc
_trap_4e8:	#call	sp,trapc
_trap_4f0:	#call	sp,trapc
_trap_4f8:	#call	sp,trapc
_trap_500:	#call	sp,trapc
_trap_508:	#call	sp,trapc
_trap_510:	#call	sp,trapc
_trap_518:	#call	sp,trapc
_trap_520:	#call	sp,trapc
_trap_528:	#call	sp,trapc
_trap_530:	#call	sp,trapc
_trap_538:	#call	sp,trapc
_trap_540:	#call	sp,trapc
_trap_548:	#call	sp,trapc
_trap_550:	#call	sp,trapc
_trap_558:	#call	sp,trapc
_trap_560:	#call	sp,trapc
_trap_568:	#call	sp,trapc
_trap_570:	#call	sp,trapc
_trap_578:	#call	sp,trapc
_trap_580:	#call	sp,trapc
_trap_588:	#call	sp,trapc
_trap_590:	#call	sp,trapc
_trap_598:	#call	sp,trapc
_trap_5a0:	#call	sp,trapc
_trap_5a8:	#call	sp,trapc
_trap_5b0:	#call	sp,trapc
_trap_5b8:	#call	sp,trapc
_trap_5c0:	#call	sp,trapc
_trap_5c8:	#call	sp,trapc
_trap_5d0:	#call	sp,trapc
_trap_5d8:	#call	sp,trapc
_trap_5e0:	#call	sp,trapc
_trap_5e8:	#call	sp,trapc
_trap_5f0:	#call	sp,trapc
_trap_5f8:	#call	sp,trapc
_trap_600:	#call	sp,trapc
_trap_608:	#call	sp,trapc
_trap_610:	#call	sp,trapc
_trap_618:	#call	sp,trapc
_trap_620:	#call	sp,trapc
_trap_628:	#call	sp,trapc
_trap_630:	#call	sp,trapc
_trap_638:	#call	sp,trapc
_trap_640:	#call	sp,trapc
_trap_648:	#call	sp,trapc
_trap_650:	#call	sp,trapc
_trap_658:	#call	sp,trapc
_trap_660:	#call	sp,trapc
_trap_668:	#call	sp,trapc
_trap_670:	#call	sp,trapc
_trap_678:	#call	sp,trapc
_trap_680:	#call	sp,trapc
_trap_688:	#call	sp,trapc
_trap_690:	#call	sp,trapc
_trap_698:	#call	sp,trapc
_trap_6a0:	#call	sp,trapc
_trap_6a8:	#call	sp,trapc
_trap_6b0:	#call	sp,trapc
_trap_6b8:	#call	sp,trapc
_trap_6c0:	#call	sp,trapc
_trap_6c8:	#call	sp,trapc
_trap_6d0:	#call	sp,trapc
_trap_6d8:	#call	sp,trapc
_trap_6e0:	#call	sp,trapc
_trap_6e8:	#call	sp,trapc
_trap_6f0:	#call	sp,trapc
_trap_6f8:	#call	sp,trapc
_trap_700:	#call	sp,trapc
_trap_708:	#call	sp,trapc
_trap_710:	#call	sp,trapc
_trap_718:	#call	sp,trapc
_trap_720:	#call	sp,trapc
_trap_728:	#call	sp,trapc
_trap_730:	#call	sp,trapc
_trap_738:	#call	sp,trapc
_trap_740:	#call	sp,trapc
_trap_748:	#call	sp,trapc
_trap_750:	#call	sp,trapc
_trap_758:	#call	sp,trapc
_trap_760:	#call	sp,trapc
_trap_768:	#call	sp,trapc
_trap_770:	#call	sp,trapc
_trap_778:	#call	sp,trapc
_trap_780:	#call	sp,trapc
_trap_788:	#call	sp,trapc
_trap_790:	#call	sp,trapc
_trap_798:	#call	sp,trapc
_trap_7a0:	#call	sp,trapc
_trap_7a8:	#call	sp,trapc
_trap_7b0:	#call	sp,trapc
_trap_7b8:	#call	sp,trapc
_trap_7c0:	#call	sp,trapc
_trap_7c8:	#call	sp,trapc
_trap_7d0:	#call	sp,trapc
_trap_7d8:	#call	sp,trapc
_trap_7e0:	#call	sp,trapc
_trap_7e8:	#call	sp,trapc
_trap_7f0:	#call	sp,trapc
_trap_7f8:	#call	sp,trapc
_intr_800:	#call	sp,trapc
_intr_808:	#call	sp,trapc
_intr_810:	#call	sp,trapc
_intr_818:	#call	sp,trapc
_intr_820:	#call	sp,trapc
_intr_828:	#call	sp,trapc
_intr_830:	#call	sp,trapc
_intr_838:	#call	sp,trapc
_intr_840:	#call	sp,trapc
_intr_848:	#call	sp,trapc
_intr_850:	#call	sp,trapc
_intr_858:	#call	sp,trapc
_intr_860:	#call	sp,trapc
_intr_868:	#call	sp,trapc
_intr_870:	#call	sp,trapc
_intr_878:	#call	sp,trapc
_intr_880:	#call	sp,trapc
_intr_888:	#call	sp,trapc
_intr_890:	#call	sp,trapc
_intr_898:	#call	sp,trapc
_intr_8a0:	#call	sp,trapc
_intr_8a8:	#call	sp,trapc
_intr_8b0:	#call	sp,trapc
_intr_8b8:	#call	sp,trapc
_intr_8c0:	#call	sp,trapc
_intr_8c8:	#call	sp,trapc
_intr_8d0:	#call	sp,trapc
_intr_8d8:	#call	sp,trapc
_intr_8e0:	#call	sp,trapc
_intr_8e8:	#call	sp,trapc
_intr_8f0:	#call	sp,trapc
_intr_8f8:	#call	sp,trapc
_intr_900:	#call	sp,trapc
_intr_908:	#call	sp,trapc
_intr_910:	#call	sp,trapc
_intr_918:	#call	sp,trapc
_intr_920:	#call	sp,trapc
_intr_928:	#call	sp,trapc
_intr_930:	#call	sp,trapc
_intr_938:	#call	sp,trapc
_intr_940:	#call	sp,trapc
_intr_948:	#call	sp,trapc
_intr_950:	#call	sp,trapc
_intr_958:	#call	sp,trapc
_intr_960:	#call	sp,trapc
_intr_968:	#call	sp,trapc
_intr_970:	#call	sp,trapc
_intr_978:	#call	sp,trapc
_intr_980:	#call	sp,trapc
_intr_988:	#call	sp,trapc
_intr_990:	#call	sp,trapc
_intr_998:	#call	sp,trapc
_intr_9a0:	#call	sp,trapc
_intr_9a8:	#call	sp,trapc
_intr_9b0:	#call	sp,trapc
_intr_9b8:	#call	sp,trapc
_intr_9c0:	#call	sp,trapc
_intr_9c8:	#call	sp,trapc
_intr_9d0:	#call	sp,trapc
_intr_9d8:	#call	sp,trapc
_intr_9e0:	#call	sp,trapc
_intr_9e8:	#call	sp,trapc
_intr_9f0:	#call	sp,trapc
_intr_9f8:	#call	sp,trapc
_intr_a00:	#call	sp,trapc
_intr_a08:	#call	sp,trapc
_intr_a10:	#call	sp,trapc
_intr_a18:	#call	sp,trapc
_intr_a20:	#call	sp,trapc
_intr_a28:	#call	sp,trapc
_intr_a30:	#call	sp,trapc
_intr_a38:	#call	sp,trapc
_intr_a40:	#call	sp,trapc
_intr_a48:	#call	sp,trapc
_intr_a50:	#call	sp,trapc
_intr_a58:	#call	sp,trapc
_intr_a60:	#call	sp,trapc
_intr_a68:	#call	sp,trapc
_intr_a70:	#call	sp,trapc
_intr_a78:	#call	sp,trapc
_intr_a80:	#call	sp,trapc
_intr_a88:	#call	sp,trapc
_intr_a90:	#call	sp,trapc
_intr_a98:	#call	sp,trapc
_intr_aa0:	#call	sp,trapc
_intr_aa8:	#call	sp,trapc
_intr_ab0:	#call	sp,trapc
_intr_ab8:	#call	sp,trapc
_intr_ac0:	#call	sp,trapc
_intr_ac8:	#call	sp,trapc
_intr_ad0:	#call	sp,trapc
_intr_ad8:	#call	sp,trapc
_intr_ae0:	#call	sp,trapc
_intr_ae8:	#call	sp,trapc
_intr_af0:	#call	sp,trapc
_intr_af8:	#call	sp,trapc
_intr_b00:	#call	sp,trapc
_intr_b08:	#call	sp,trapc
_intr_b10:	#call	sp,trapc
_intr_b18:	#call	sp,trapc
_intr_b20:	#call	sp,trapc
_intr_b28:	#call	sp,trapc
_intr_b30:	#call	sp,trapc
_intr_b38:	#call	sp,trapc
_intr_b40:	#call	sp,trapc
_intr_b48:	#call	sp,trapc
_intr_b50:	#call	sp,trapc
_intr_b58:	#call	sp,trapc
_intr_b60:	#call	sp,trapc
_intr_b68:	#call	sp,trapc
_intr_b70:	#call	sp,trapc
_intr_b78:	#call	sp,trapc
_intr_b80:	#call	sp,trapc
_intr_b88:	#call	sp,trapc
_intr_b90:	#call	sp,trapc
_intr_b98:	#call	sp,trapc
_intr_ba0:	#call	sp,trapc
_intr_ba8:	#call	sp,trapc
_intr_bb0:	#call	sp,trapc
_intr_bb8:	#call	sp,trapc
_intr_bc0:	#call	sp,trapc
_intr_bc8:	#call	sp,trapc
_intr_bd0:	#call	sp,trapc
_intr_bd8:	#call	sp,trapc
_intr_be0:	#call	sp,trapc
_intr_be8:	#call	sp,trapc
_intr_bf0:	#call	sp,trapc
_intr_bf8:	#call	sp,trapc
_intr_c00:	#call	sp,trapc
_intr_c08:	#call	sp,trapc
_intr_c10:	#call	sp,trapc
_intr_c18:	#call	sp,trapc
_intr_c20:	#call	sp,trapc
_intr_c28:	#call	sp,trapc
_intr_c30:	#call	sp,trapc
_intr_c38:	#call	sp,trapc
_intr_c40:	#call	sp,trapc
_intr_c48:	#call	sp,trapc
_intr_c50:	#call	sp,trapc
_intr_c58:	#call	sp,trapc
_intr_c60:	#call	sp,trapc
_intr_c68:	#call	sp,trapc
_intr_c70:	#call	sp,trapc
_intr_c78:	#call	sp,trapc
_intr_c80:	#call	sp,trapc
_intr_c88:	#call	sp,trapc
_intr_c90:	#call	sp,trapc
_intr_c98:	#call	sp,trapc
_intr_ca0:	#call	sp,trapc
_intr_ca8:	#call	sp,trapc
_intr_cb0:	#call	sp,trapc
_intr_cb8:	#call	sp,trapc
_intr_cc0:	#call	sp,trapc
_intr_cc8:	#call	sp,trapc
_intr_cd0:	#call	sp,trapc
_intr_cd8:	#call	sp,trapc
_intr_ce0:	#call	sp,trapc
_intr_ce8:	#call	sp,trapc
_intr_cf0:	#call	sp,trapc
_intr_cf8:	#call	sp,trapc
_intr_d00:	#call	sp,trapc
_intr_d08:	#call	sp,trapc
_intr_d10:	#call	sp,trapc
_intr_d18:	#call	sp,trapc
_intr_d20:	#call	sp,trapc
_intr_d28:	#call	sp,trapc
_intr_d30:	#call	sp,trapc
_intr_d38:	#call	sp,trapc
_intr_d40:	#call	sp,trapc
_intr_d48:	#call	sp,trapc
_intr_d50:	#call	sp,trapc
_intr_d58:	#call	sp,trapc
_intr_d60:	#call	sp,trapc
_intr_d68:	#call	sp,trapc
_intr_d70:	#call	sp,trapc
_intr_d78:	#call	sp,trapc
_intr_d80:	#call	sp,trapc
_intr_d88:	#call	sp,trapc
_intr_d90:	#call	sp,trapc
_intr_d98:	#call	sp,trapc
_intr_da0:	#call	sp,trapc
_intr_da8:	#call	sp,trapc
_intr_db0:	#call	sp,trapc
_intr_db8:	#call	sp,trapc
_intr_dc0:	#call	sp,trapc
_intr_dc8:	#call	sp,trapc
_intr_dd0:	#call	sp,trapc
_intr_dd8:	#call	sp,trapc
_intr_de0:	#call	sp,trapc
_intr_de8:	#call	sp,trapc
_intr_df0:	#call	sp,trapc
_intr_df8:	#call	sp,trapc
_intr_e00:	#call	sp,trapc
_intr_e08:	#call	sp,trapc
_intr_e10:	#call	sp,trapc
_intr_e18:	#call	sp,trapc
_intr_e20:	#call	sp,trapc
_intr_e28:	#call	sp,trapc
_intr_e30:	#call	sp,trapc
_intr_e38:	#call	sp,trapc
_intr_e40:	#call	sp,trapc
_intr_e48:	#call	sp,trapc
_intr_e50:	#call	sp,trapc
_intr_e58:	#call	sp,trapc
_intr_e60:	#call	sp,trapc
_intr_e68:	#call	sp,trapc
_intr_e70:	#call	sp,trapc
_intr_e78:	#call	sp,trapc
_intr_e80:	#call	sp,trapc
_intr_e88:	#call	sp,trapc
_intr_e90:	#call	sp,trapc
_intr_e98:	#call	sp,trapc
_intr_ea0:	#call	sp,trapc
_intr_ea8:	#call	sp,trapc
_intr_eb0:	#call	sp,trapc
_intr_eb8:	#call	sp,trapc
_intr_ec0:	#call	sp,trapc
_intr_ec8:	#call	sp,trapc
_intr_ed0:	#call	sp,trapc
_intr_ed8:	#call	sp,trapc
_intr_ee0:	#call	sp,trapc
_intr_ee8:	#call	sp,trapc
_intr_ef0:	#call	sp,trapc
_intr_ef8:	#call	sp,trapc
_intr_f00:	#call	sp,trapc
_intr_f08:	#call	sp,trapc
_intr_f10:	#call	sp,trapc
_intr_f18:	#call	sp,trapc
_intr_f20:	#call	sp,trapc
_intr_f28:	#call	sp,trapc
_intr_f30:	#call	sp,trapc
_intr_f38:	#call	sp,trapc
_intr_f40:	#call	sp,trapc
_intr_f48:	#call	sp,trapc
_intr_f50:	#call	sp,trapc
_intr_f58:	#call	sp,trapc
_intr_f60:	#call	sp,trapc
_intr_f68:	#call	sp,trapc
_intr_f70:	#call	sp,trapc
_intr_f78:	#call	sp,trapc
_intr_f80:	#call	sp,trapc
_intr_f88:	#call	sp,trapc
_intr_f90:	#call	sp,trapc
_intr_f98:	#call	sp,trapc
_intr_fa0:	#call	sp,trapc
_intr_fa8:	#call	sp,trapc
_intr_fb0:	#call	sp,trapc
_intr_fb8:	#call	sp,trapc
_intr_fc0:	#call	sp,trapc
_intr_fc8:	#call	sp,trapc
_intr_fd0:	#call	sp,trapc
_intr_fd8:	#call	sp,trapc
_intr_fe0:	#call	sp,trapc
_intr_fe8:	#call	sp,trapc
_intr_ff0:	#call	sp,trapc
_intr_ff8:	#call	sp,trapc
trapc:		# save all registers in __r
	movw	r0,__r
	movw	r1,__r+0x4
	movw	r2,__r+0x8
	movw	r3,__r+0xC
	movw	r4,__r+0x10
	movw	r5,__r+0x14
	movw	r6,__r+0x18
	movw	r7,__r+0x1C
	movw	r8,__r+0x20
	movw	r9,__r+0x24
	movw	r10,__r+0x28
	movw	r11,__r+0x2C
	movw	r12,__r+0x30
	movw	r13,__r+0x34
	movw	r14,__r+0x38
	movw	r15,r0
	addw	$12,r0
	movw	r0,__r+0x3C		# sp before interrupt/trap
	movw	(sp),r0
	movw	r0,__r+0x40		# psw
	movw	4(sp),r0
	movw	r0,__r+0x44		# ssw
	movw	8(sp),r0
	movw	r0,__r+0x48		# pc
	movw	SSTOI,r0
	movw	r0,__r+0x4C		# I-MMU PDO
	movw	SSTOD,r0
	movw	r0,__r+0x50		# D-MMU PDO
	movw	IFAULT,r0
	movw	r0,__r+0x54		# I-MMU PDO
	movw	DFAULT,r0
	movw	r0,__r+0x58		# D-MMU PDO

# note savew0 unaligns stack

	movw	$4,r0
	andw	sp,r0
	bceq	dbl
	movw	__r,r0			# restore r0
	savew0
	call	sp,_trap
	restw0
	reti	sp

dbl:
	subw	$4,sp
	movw	__r,r0			# restore r0
	savew0
	call	sp,_trap
	restw0
	addw	$4,sp
	reti	sp

#	rest of page 2

	.align	0x400
	.space	0x400
	.space	0x400
	.space	0x400
	

# PAGE 2 
# Communication area for opmon
commpage:
	.space	0x400
# 0x2400
# startup code
startup:
	loadq	$0,r0			# clear all registers
	movw	r0,r1
	movw	r0,r2
	movw	r0,r3
	movw	r0,r4
	movw	r0,r5
	movw	r0,r6
	movw	r0,r7
	movw	r0,r8
	movw	r0,r9
	movw	r0,r10
	movw	r0,r11
	movw	r0,r12
	movw	r0,r13
	movw	r0,r14
	movw	r0,r15
	movld	r0,f0
	movld	r0,f1
	movld	r0,f2
	movld	r0,f3
	movld	r0,f4
	movld	r0,f5
	movld	r0,f6
	movld	r0,f7
	movwp	r0,ssw			# reset ssw
	movwp	r0,psw			#  and psw
	movw	$CUST_NCA+CNTL_EWIR+CNTL_EWIW+CNTL_EWCW+CNTL_EP,r0
	storb	r0,CNTLD		# Initialize data cache settings
# following line fix for I-CAMMU 1/4 bclk bug - JK
	movw	USTOD,r1
	storb	r0,CNTLI		#   and instruction cache
	movw	$CR_LVW+CR_LVX+TR_SV+TR_UV+TR_D+TR_R+CR_U,r0
	movw	r0,RESETD		# Reset data cache
# following line fix for I-CAMMU 1/4 bclk bug - JK
	movw	USTOD,r1
	movw	r0,RESETI		#   and instruction cache

# Now turn on mapping

	loada	__segt,r0	# get address of segment table
	movw	r0,SSTOD	# set data segment table register
	movw	r0,SSTOI	# set instruction segment table register

	movpw	ssw,r0		# get old SSW
	orw	$SS_M,r0	# turn on mapping bit
	movwp	r0,ssw		# set new SSW
	movw	$0,r0
	storw	r0,(r0)		# set address 0 - 5 to noop's
	storh	r0,4(r0)
	b	start

	.align	0x400
	.space	0x400
	.space	0x400

# PAGE 3
# BUS-MASTER I/O 2nd level page table entries
# NOT AVAILABLE FROM LOGICAL MEMORY
pagebm:
	.long	0x0000+AT_IO_SPACE+MAPINITWORD
	.space	0xffc

# Skip over I/O and boot pages
# PAGE 4
# BUS-MASTER MEMORY 2nd level page table entries
# NOT AVAILABLE FROM LOGICAL MEMORY
pagebm0:
	.long	0x0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x10000+AT_MEM_SPACE+MAPINITWORD
	.long	0x11000+AT_MEM_SPACE+MAPINITWORD
	.long	0x12000+AT_MEM_SPACE+MAPINITWORD
	.long	0x13000+AT_MEM_SPACE+MAPINITWORD
	.long	0x14000+AT_MEM_SPACE+MAPINITWORD
	.long	0x15000+AT_MEM_SPACE+MAPINITWORD
	.long	0x16000+AT_MEM_SPACE+MAPINITWORD
	.long	0x17000+AT_MEM_SPACE+MAPINITWORD
	.long	0x18000+AT_MEM_SPACE+MAPINITWORD
	.long	0x19000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x20000+AT_MEM_SPACE+MAPINITWORD
	.long	0x21000+AT_MEM_SPACE+MAPINITWORD
	.long	0x22000+AT_MEM_SPACE+MAPINITWORD
	.long	0x23000+AT_MEM_SPACE+MAPINITWORD
	.long	0x24000+AT_MEM_SPACE+MAPINITWORD
	.long	0x25000+AT_MEM_SPACE+MAPINITWORD
	.long	0x26000+AT_MEM_SPACE+MAPINITWORD
	.long	0x27000+AT_MEM_SPACE+MAPINITWORD
	.long	0x28000+AT_MEM_SPACE+MAPINITWORD
	.long	0x29000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x30000+AT_MEM_SPACE+MAPINITWORD
	.long	0x31000+AT_MEM_SPACE+MAPINITWORD
	.long	0x32000+AT_MEM_SPACE+MAPINITWORD
	.long	0x33000+AT_MEM_SPACE+MAPINITWORD
	.long	0x34000+AT_MEM_SPACE+MAPINITWORD
	.long	0x35000+AT_MEM_SPACE+MAPINITWORD
	.long	0x36000+AT_MEM_SPACE+MAPINITWORD
	.long	0x37000+AT_MEM_SPACE+MAPINITWORD
	.long	0x38000+AT_MEM_SPACE+MAPINITWORD
	.long	0x39000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x40000+AT_MEM_SPACE+MAPINITWORD
	.long	0x41000+AT_MEM_SPACE+MAPINITWORD
	.long	0x42000+AT_MEM_SPACE+MAPINITWORD
	.long	0x43000+AT_MEM_SPACE+MAPINITWORD
	.long	0x44000+AT_MEM_SPACE+MAPINITWORD
	.long	0x45000+AT_MEM_SPACE+MAPINITWORD
	.long	0x46000+AT_MEM_SPACE+MAPINITWORD
	.long	0x47000+AT_MEM_SPACE+MAPINITWORD
	.long	0x48000+AT_MEM_SPACE+MAPINITWORD
	.long	0x49000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x50000+AT_MEM_SPACE+MAPINITWORD
	.long	0x51000+AT_MEM_SPACE+MAPINITWORD
	.long	0x52000+AT_MEM_SPACE+MAPINITWORD
	.long	0x53000+AT_MEM_SPACE+MAPINITWORD
	.long	0x54000+AT_MEM_SPACE+MAPINITWORD
	.long	0x55000+AT_MEM_SPACE+MAPINITWORD
	.long	0x56000+AT_MEM_SPACE+MAPINITWORD
	.long	0x57000+AT_MEM_SPACE+MAPINITWORD
	.long	0x58000+AT_MEM_SPACE+MAPINITWORD
	.long	0x59000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x60000+AT_MEM_SPACE+MAPINITWORD
	.long	0x61000+AT_MEM_SPACE+MAPINITWORD
	.long	0x62000+AT_MEM_SPACE+MAPINITWORD
	.long	0x63000+AT_MEM_SPACE+MAPINITWORD
	.long	0x64000+AT_MEM_SPACE+MAPINITWORD
	.long	0x65000+AT_MEM_SPACE+MAPINITWORD
	.long	0x66000+AT_MEM_SPACE+MAPINITWORD
	.long	0x67000+AT_MEM_SPACE+MAPINITWORD
	.long	0x68000+AT_MEM_SPACE+MAPINITWORD
	.long	0x69000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x70000+AT_MEM_SPACE+MAPINITWORD
	.long	0x71000+AT_MEM_SPACE+MAPINITWORD
	.long	0x72000+AT_MEM_SPACE+MAPINITWORD
	.long	0x73000+AT_MEM_SPACE+MAPINITWORD
	.long	0x74000+AT_MEM_SPACE+MAPINITWORD
	.long	0x75000+AT_MEM_SPACE+MAPINITWORD
	.long	0x76000+AT_MEM_SPACE+MAPINITWORD
	.long	0x77000+AT_MEM_SPACE+MAPINITWORD
	.long	0x78000+AT_MEM_SPACE+MAPINITWORD
	.long	0x79000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x80000+AT_MEM_SPACE+MAPINITWORD
	.long	0x81000+AT_MEM_SPACE+MAPINITWORD
	.long	0x82000+AT_MEM_SPACE+MAPINITWORD
	.long	0x83000+AT_MEM_SPACE+MAPINITWORD
	.long	0x84000+AT_MEM_SPACE+MAPINITWORD
	.long	0x85000+AT_MEM_SPACE+MAPINITWORD
	.long	0x86000+AT_MEM_SPACE+MAPINITWORD
	.long	0x87000+AT_MEM_SPACE+MAPINITWORD
	.long	0x88000+AT_MEM_SPACE+MAPINITWORD
	.long	0x89000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x90000+AT_MEM_SPACE+MAPINITWORD
	.long	0x91000+AT_MEM_SPACE+MAPINITWORD
	.long	0x92000+AT_MEM_SPACE+MAPINITWORD
	.long	0x93000+AT_MEM_SPACE+MAPINITWORD
	.long	0x94000+AT_MEM_SPACE+MAPINITWORD
	.long	0x95000+AT_MEM_SPACE+MAPINITWORD
	.long	0x96000+AT_MEM_SPACE+MAPINITWORD
	.long	0x97000+AT_MEM_SPACE+MAPINITWORD
	.long	0x98000+AT_MEM_SPACE+MAPINITWORD
	.long	0x99000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xab000+AT_MEM_SPACE+MAPINITWORD
	.long	0xac000+AT_MEM_SPACE+MAPINITWORD
	.long	0xad000+AT_MEM_SPACE+MAPINITWORD
	.long	0xae000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xba000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xca000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xce000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xda000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xde000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xea000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xec000+AT_MEM_SPACE+MAPINITWORD
	.long	0xed000+AT_MEM_SPACE+MAPINITWORD
	.long	0xee000+AT_MEM_SPACE+MAPINITWORD
	.long	0xef000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xff000+AT_MEM_SPACE+MAPINITWORD
	.long	0x100000+AT_MEM_SPACE+MAPINITWORD
	.long	0x101000+AT_MEM_SPACE+MAPINITWORD
	.long	0x102000+AT_MEM_SPACE+MAPINITWORD
	.long	0x103000+AT_MEM_SPACE+MAPINITWORD
	.long	0x104000+AT_MEM_SPACE+MAPINITWORD
	.long	0x105000+AT_MEM_SPACE+MAPINITWORD
	.long	0x106000+AT_MEM_SPACE+MAPINITWORD
	.long	0x107000+AT_MEM_SPACE+MAPINITWORD
	.long	0x108000+AT_MEM_SPACE+MAPINITWORD
	.long	0x109000+AT_MEM_SPACE+MAPINITWORD
	.long	0x10a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x10b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x10c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x10d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x10e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x10f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x110000+AT_MEM_SPACE+MAPINITWORD
	.long	0x111000+AT_MEM_SPACE+MAPINITWORD
	.long	0x112000+AT_MEM_SPACE+MAPINITWORD
	.long	0x113000+AT_MEM_SPACE+MAPINITWORD
	.long	0x114000+AT_MEM_SPACE+MAPINITWORD
	.long	0x115000+AT_MEM_SPACE+MAPINITWORD
	.long	0x116000+AT_MEM_SPACE+MAPINITWORD
	.long	0x117000+AT_MEM_SPACE+MAPINITWORD
	.long	0x118000+AT_MEM_SPACE+MAPINITWORD
	.long	0x119000+AT_MEM_SPACE+MAPINITWORD
	.long	0x11a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x11b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x11c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x11d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x11e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x11f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x120000+AT_MEM_SPACE+MAPINITWORD
	.long	0x121000+AT_MEM_SPACE+MAPINITWORD
	.long	0x122000+AT_MEM_SPACE+MAPINITWORD
	.long	0x123000+AT_MEM_SPACE+MAPINITWORD
	.long	0x124000+AT_MEM_SPACE+MAPINITWORD
	.long	0x125000+AT_MEM_SPACE+MAPINITWORD
	.long	0x126000+AT_MEM_SPACE+MAPINITWORD
	.long	0x127000+AT_MEM_SPACE+MAPINITWORD
	.long	0x128000+AT_MEM_SPACE+MAPINITWORD
	.long	0x129000+AT_MEM_SPACE+MAPINITWORD
	.long	0x12a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x12b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x12c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x12d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x12e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x12f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x130000+AT_MEM_SPACE+MAPINITWORD
	.long	0x131000+AT_MEM_SPACE+MAPINITWORD
	.long	0x132000+AT_MEM_SPACE+MAPINITWORD
	.long	0x133000+AT_MEM_SPACE+MAPINITWORD
	.long	0x134000+AT_MEM_SPACE+MAPINITWORD
	.long	0x135000+AT_MEM_SPACE+MAPINITWORD
	.long	0x136000+AT_MEM_SPACE+MAPINITWORD
	.long	0x137000+AT_MEM_SPACE+MAPINITWORD
	.long	0x138000+AT_MEM_SPACE+MAPINITWORD
	.long	0x139000+AT_MEM_SPACE+MAPINITWORD
	.long	0x13a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x13b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x13c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x13d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x13e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x13f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x140000+AT_MEM_SPACE+MAPINITWORD
	.long	0x141000+AT_MEM_SPACE+MAPINITWORD
	.long	0x142000+AT_MEM_SPACE+MAPINITWORD
	.long	0x143000+AT_MEM_SPACE+MAPINITWORD
	.long	0x144000+AT_MEM_SPACE+MAPINITWORD
	.long	0x145000+AT_MEM_SPACE+MAPINITWORD
	.long	0x146000+AT_MEM_SPACE+MAPINITWORD
	.long	0x147000+AT_MEM_SPACE+MAPINITWORD
	.long	0x148000+AT_MEM_SPACE+MAPINITWORD
	.long	0x149000+AT_MEM_SPACE+MAPINITWORD
	.long	0x14a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x14b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x14c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x14d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x14e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x14f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x150000+AT_MEM_SPACE+MAPINITWORD
	.long	0x151000+AT_MEM_SPACE+MAPINITWORD
	.long	0x152000+AT_MEM_SPACE+MAPINITWORD
	.long	0x153000+AT_MEM_SPACE+MAPINITWORD
	.long	0x154000+AT_MEM_SPACE+MAPINITWORD
	.long	0x155000+AT_MEM_SPACE+MAPINITWORD
	.long	0x156000+AT_MEM_SPACE+MAPINITWORD
	.long	0x157000+AT_MEM_SPACE+MAPINITWORD
	.long	0x158000+AT_MEM_SPACE+MAPINITWORD
	.long	0x159000+AT_MEM_SPACE+MAPINITWORD
	.long	0x15a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x15b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x15c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x15d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x15e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x15f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x160000+AT_MEM_SPACE+MAPINITWORD
	.long	0x161000+AT_MEM_SPACE+MAPINITWORD
	.long	0x162000+AT_MEM_SPACE+MAPINITWORD
	.long	0x163000+AT_MEM_SPACE+MAPINITWORD
	.long	0x164000+AT_MEM_SPACE+MAPINITWORD
	.long	0x165000+AT_MEM_SPACE+MAPINITWORD
	.long	0x166000+AT_MEM_SPACE+MAPINITWORD
	.long	0x167000+AT_MEM_SPACE+MAPINITWORD
	.long	0x168000+AT_MEM_SPACE+MAPINITWORD
	.long	0x169000+AT_MEM_SPACE+MAPINITWORD
	.long	0x16a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x16b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x16c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x16d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x16e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x16f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x170000+AT_MEM_SPACE+MAPINITWORD
	.long	0x171000+AT_MEM_SPACE+MAPINITWORD
	.long	0x172000+AT_MEM_SPACE+MAPINITWORD
	.long	0x173000+AT_MEM_SPACE+MAPINITWORD
	.long	0x174000+AT_MEM_SPACE+MAPINITWORD
	.long	0x175000+AT_MEM_SPACE+MAPINITWORD
	.long	0x176000+AT_MEM_SPACE+MAPINITWORD
	.long	0x177000+AT_MEM_SPACE+MAPINITWORD
	.long	0x178000+AT_MEM_SPACE+MAPINITWORD
	.long	0x179000+AT_MEM_SPACE+MAPINITWORD
	.long	0x17a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x17b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x17c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x17d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x17e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x17f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x180000+AT_MEM_SPACE+MAPINITWORD
	.long	0x181000+AT_MEM_SPACE+MAPINITWORD
	.long	0x182000+AT_MEM_SPACE+MAPINITWORD
	.long	0x183000+AT_MEM_SPACE+MAPINITWORD
	.long	0x184000+AT_MEM_SPACE+MAPINITWORD
	.long	0x185000+AT_MEM_SPACE+MAPINITWORD
	.long	0x186000+AT_MEM_SPACE+MAPINITWORD
	.long	0x187000+AT_MEM_SPACE+MAPINITWORD
	.long	0x188000+AT_MEM_SPACE+MAPINITWORD
	.long	0x189000+AT_MEM_SPACE+MAPINITWORD
	.long	0x18a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x18b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x18c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x18d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x18e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x18f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x190000+AT_MEM_SPACE+MAPINITWORD
	.long	0x191000+AT_MEM_SPACE+MAPINITWORD
	.long	0x192000+AT_MEM_SPACE+MAPINITWORD
	.long	0x193000+AT_MEM_SPACE+MAPINITWORD
	.long	0x194000+AT_MEM_SPACE+MAPINITWORD
	.long	0x195000+AT_MEM_SPACE+MAPINITWORD
	.long	0x196000+AT_MEM_SPACE+MAPINITWORD
	.long	0x197000+AT_MEM_SPACE+MAPINITWORD
	.long	0x198000+AT_MEM_SPACE+MAPINITWORD
	.long	0x199000+AT_MEM_SPACE+MAPINITWORD
	.long	0x19a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x19b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x19c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x19d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x19e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x19f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1a0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1a1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1a2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1a3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1a4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1a5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1a6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1a7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1a8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1a9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1aa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1ab000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1ac000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1ad000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1ae000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1af000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1b0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1b1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1b2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1b3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1b4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1b5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1b6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1b7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1b8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1b9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1ba000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1bb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1bc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1bd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1be000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1bf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1c0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1c1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1c2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1c3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1c4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1c5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1c6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1c7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1c8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1c9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1ca000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1cb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1cc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1cd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1ce000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1cf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1d0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1d1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1d2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1d3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1d4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1d5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1d6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1d7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1d8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1d9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1da000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1db000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1dc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1dd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1de000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1df000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1e0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1e1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1e2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1e3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1e4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1e5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1e6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1e7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1e8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1e9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1ea000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1eb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1ec000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1ed000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1ee000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1ef000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1f0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1f1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1f2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1f3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1f4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1f5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1f6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1f7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1f8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1f9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1fa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1fb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1fc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1fd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1fe000+AT_MEM_SPACE+MAPINITWORD
	.long	0x1ff000+AT_MEM_SPACE+MAPINITWORD
	.long	0x200000+AT_MEM_SPACE+MAPINITWORD
	.long	0x201000+AT_MEM_SPACE+MAPINITWORD
	.long	0x202000+AT_MEM_SPACE+MAPINITWORD
	.long	0x203000+AT_MEM_SPACE+MAPINITWORD
	.long	0x204000+AT_MEM_SPACE+MAPINITWORD
	.long	0x205000+AT_MEM_SPACE+MAPINITWORD
	.long	0x206000+AT_MEM_SPACE+MAPINITWORD
	.long	0x207000+AT_MEM_SPACE+MAPINITWORD
	.long	0x208000+AT_MEM_SPACE+MAPINITWORD
	.long	0x209000+AT_MEM_SPACE+MAPINITWORD
	.long	0x20a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x20b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x20c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x20d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x20e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x20f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x210000+AT_MEM_SPACE+MAPINITWORD
	.long	0x211000+AT_MEM_SPACE+MAPINITWORD
	.long	0x212000+AT_MEM_SPACE+MAPINITWORD
	.long	0x213000+AT_MEM_SPACE+MAPINITWORD
	.long	0x214000+AT_MEM_SPACE+MAPINITWORD
	.long	0x215000+AT_MEM_SPACE+MAPINITWORD
	.long	0x216000+AT_MEM_SPACE+MAPINITWORD
	.long	0x217000+AT_MEM_SPACE+MAPINITWORD
	.long	0x218000+AT_MEM_SPACE+MAPINITWORD
	.long	0x219000+AT_MEM_SPACE+MAPINITWORD
	.long	0x21a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x21b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x21c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x21d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x21e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x21f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x220000+AT_MEM_SPACE+MAPINITWORD
	.long	0x221000+AT_MEM_SPACE+MAPINITWORD
	.long	0x222000+AT_MEM_SPACE+MAPINITWORD
	.long	0x223000+AT_MEM_SPACE+MAPINITWORD
	.long	0x224000+AT_MEM_SPACE+MAPINITWORD
	.long	0x225000+AT_MEM_SPACE+MAPINITWORD
	.long	0x226000+AT_MEM_SPACE+MAPINITWORD
	.long	0x227000+AT_MEM_SPACE+MAPINITWORD
	.long	0x228000+AT_MEM_SPACE+MAPINITWORD
	.long	0x229000+AT_MEM_SPACE+MAPINITWORD
	.long	0x22a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x22b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x22c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x22d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x22e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x22f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x230000+AT_MEM_SPACE+MAPINITWORD
	.long	0x231000+AT_MEM_SPACE+MAPINITWORD
	.long	0x232000+AT_MEM_SPACE+MAPINITWORD
	.long	0x233000+AT_MEM_SPACE+MAPINITWORD
	.long	0x234000+AT_MEM_SPACE+MAPINITWORD
	.long	0x235000+AT_MEM_SPACE+MAPINITWORD
	.long	0x236000+AT_MEM_SPACE+MAPINITWORD
	.long	0x237000+AT_MEM_SPACE+MAPINITWORD
	.long	0x238000+AT_MEM_SPACE+MAPINITWORD
	.long	0x239000+AT_MEM_SPACE+MAPINITWORD
	.long	0x23a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x23b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x23c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x23d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x23e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x23f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x240000+AT_MEM_SPACE+MAPINITWORD
	.long	0x241000+AT_MEM_SPACE+MAPINITWORD
	.long	0x242000+AT_MEM_SPACE+MAPINITWORD
	.long	0x243000+AT_MEM_SPACE+MAPINITWORD
	.long	0x244000+AT_MEM_SPACE+MAPINITWORD
	.long	0x245000+AT_MEM_SPACE+MAPINITWORD
	.long	0x246000+AT_MEM_SPACE+MAPINITWORD
	.long	0x247000+AT_MEM_SPACE+MAPINITWORD
	.long	0x248000+AT_MEM_SPACE+MAPINITWORD
	.long	0x249000+AT_MEM_SPACE+MAPINITWORD
	.long	0x24a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x24b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x24c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x24d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x24e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x24f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x250000+AT_MEM_SPACE+MAPINITWORD
	.long	0x251000+AT_MEM_SPACE+MAPINITWORD
	.long	0x252000+AT_MEM_SPACE+MAPINITWORD
	.long	0x253000+AT_MEM_SPACE+MAPINITWORD
	.long	0x254000+AT_MEM_SPACE+MAPINITWORD
	.long	0x255000+AT_MEM_SPACE+MAPINITWORD
	.long	0x256000+AT_MEM_SPACE+MAPINITWORD
	.long	0x257000+AT_MEM_SPACE+MAPINITWORD
	.long	0x258000+AT_MEM_SPACE+MAPINITWORD
	.long	0x259000+AT_MEM_SPACE+MAPINITWORD
	.long	0x25a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x25b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x25c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x25d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x25e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x25f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x260000+AT_MEM_SPACE+MAPINITWORD
	.long	0x261000+AT_MEM_SPACE+MAPINITWORD
	.long	0x262000+AT_MEM_SPACE+MAPINITWORD
	.long	0x263000+AT_MEM_SPACE+MAPINITWORD
	.long	0x264000+AT_MEM_SPACE+MAPINITWORD
	.long	0x265000+AT_MEM_SPACE+MAPINITWORD
	.long	0x266000+AT_MEM_SPACE+MAPINITWORD
	.long	0x267000+AT_MEM_SPACE+MAPINITWORD
	.long	0x268000+AT_MEM_SPACE+MAPINITWORD
	.long	0x269000+AT_MEM_SPACE+MAPINITWORD
	.long	0x26a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x26b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x26c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x26d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x26e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x26f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x270000+AT_MEM_SPACE+MAPINITWORD
	.long	0x271000+AT_MEM_SPACE+MAPINITWORD
	.long	0x272000+AT_MEM_SPACE+MAPINITWORD
	.long	0x273000+AT_MEM_SPACE+MAPINITWORD
	.long	0x274000+AT_MEM_SPACE+MAPINITWORD
	.long	0x275000+AT_MEM_SPACE+MAPINITWORD
	.long	0x276000+AT_MEM_SPACE+MAPINITWORD
	.long	0x277000+AT_MEM_SPACE+MAPINITWORD
	.long	0x278000+AT_MEM_SPACE+MAPINITWORD
	.long	0x279000+AT_MEM_SPACE+MAPINITWORD
	.long	0x27a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x27b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x27c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x27d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x27e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x27f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x280000+AT_MEM_SPACE+MAPINITWORD
	.long	0x281000+AT_MEM_SPACE+MAPINITWORD
	.long	0x282000+AT_MEM_SPACE+MAPINITWORD
	.long	0x283000+AT_MEM_SPACE+MAPINITWORD
	.long	0x284000+AT_MEM_SPACE+MAPINITWORD
	.long	0x285000+AT_MEM_SPACE+MAPINITWORD
	.long	0x286000+AT_MEM_SPACE+MAPINITWORD
	.long	0x287000+AT_MEM_SPACE+MAPINITWORD
	.long	0x288000+AT_MEM_SPACE+MAPINITWORD
	.long	0x289000+AT_MEM_SPACE+MAPINITWORD
	.long	0x28a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x28b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x28c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x28d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x28e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x28f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x290000+AT_MEM_SPACE+MAPINITWORD
	.long	0x291000+AT_MEM_SPACE+MAPINITWORD
	.long	0x292000+AT_MEM_SPACE+MAPINITWORD
	.long	0x293000+AT_MEM_SPACE+MAPINITWORD
	.long	0x294000+AT_MEM_SPACE+MAPINITWORD
	.long	0x295000+AT_MEM_SPACE+MAPINITWORD
	.long	0x296000+AT_MEM_SPACE+MAPINITWORD
	.long	0x297000+AT_MEM_SPACE+MAPINITWORD
	.long	0x298000+AT_MEM_SPACE+MAPINITWORD
	.long	0x299000+AT_MEM_SPACE+MAPINITWORD
	.long	0x29a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x29b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x29c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x29d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x29e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x29f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2a0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2a1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2a2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2a3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2a4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2a5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2a6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2a7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2a8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2a9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2aa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2ab000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2ac000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2ad000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2ae000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2af000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2b0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2b1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2b2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2b3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2b4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2b5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2b6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2b7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2b8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2b9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2ba000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2bb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2bc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2bd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2be000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2bf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2c0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2c1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2c2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2c3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2c4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2c5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2c6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2c7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2c8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2c9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2ca000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2cb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2cc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2cd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2ce000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2cf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2d0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2d1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2d2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2d3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2d4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2d5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2d6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2d7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2d8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2d9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2da000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2db000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2dc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2dd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2de000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2df000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2e0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2e1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2e2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2e3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2e4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2e5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2e6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2e7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2e8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2e9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2ea000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2eb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2ec000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2ed000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2ee000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2ef000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2f0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2f1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2f2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2f3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2f4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2f5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2f6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2f7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2f8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2f9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2fa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2fb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2fc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2fd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2fe000+AT_MEM_SPACE+MAPINITWORD
	.long	0x2ff000+AT_MEM_SPACE+MAPINITWORD
	.long	0x300000+AT_MEM_SPACE+MAPINITWORD
	.long	0x301000+AT_MEM_SPACE+MAPINITWORD
	.long	0x302000+AT_MEM_SPACE+MAPINITWORD
	.long	0x303000+AT_MEM_SPACE+MAPINITWORD
	.long	0x304000+AT_MEM_SPACE+MAPINITWORD
	.long	0x305000+AT_MEM_SPACE+MAPINITWORD
	.long	0x306000+AT_MEM_SPACE+MAPINITWORD
	.long	0x307000+AT_MEM_SPACE+MAPINITWORD
	.long	0x308000+AT_MEM_SPACE+MAPINITWORD
	.long	0x309000+AT_MEM_SPACE+MAPINITWORD
	.long	0x30a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x30b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x30c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x30d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x30e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x30f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x310000+AT_MEM_SPACE+MAPINITWORD
	.long	0x311000+AT_MEM_SPACE+MAPINITWORD
	.long	0x312000+AT_MEM_SPACE+MAPINITWORD
	.long	0x313000+AT_MEM_SPACE+MAPINITWORD
	.long	0x314000+AT_MEM_SPACE+MAPINITWORD
	.long	0x315000+AT_MEM_SPACE+MAPINITWORD
	.long	0x316000+AT_MEM_SPACE+MAPINITWORD
	.long	0x317000+AT_MEM_SPACE+MAPINITWORD
	.long	0x318000+AT_MEM_SPACE+MAPINITWORD
	.long	0x319000+AT_MEM_SPACE+MAPINITWORD
	.long	0x31a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x31b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x31c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x31d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x31e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x31f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x320000+AT_MEM_SPACE+MAPINITWORD
	.long	0x321000+AT_MEM_SPACE+MAPINITWORD
	.long	0x322000+AT_MEM_SPACE+MAPINITWORD
	.long	0x323000+AT_MEM_SPACE+MAPINITWORD
	.long	0x324000+AT_MEM_SPACE+MAPINITWORD
	.long	0x325000+AT_MEM_SPACE+MAPINITWORD
	.long	0x326000+AT_MEM_SPACE+MAPINITWORD
	.long	0x327000+AT_MEM_SPACE+MAPINITWORD
	.long	0x328000+AT_MEM_SPACE+MAPINITWORD
	.long	0x329000+AT_MEM_SPACE+MAPINITWORD
	.long	0x32a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x32b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x32c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x32d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x32e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x32f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x330000+AT_MEM_SPACE+MAPINITWORD
	.long	0x331000+AT_MEM_SPACE+MAPINITWORD
	.long	0x332000+AT_MEM_SPACE+MAPINITWORD
	.long	0x333000+AT_MEM_SPACE+MAPINITWORD
	.long	0x334000+AT_MEM_SPACE+MAPINITWORD
	.long	0x335000+AT_MEM_SPACE+MAPINITWORD
	.long	0x336000+AT_MEM_SPACE+MAPINITWORD
	.long	0x337000+AT_MEM_SPACE+MAPINITWORD
	.long	0x338000+AT_MEM_SPACE+MAPINITWORD
	.long	0x339000+AT_MEM_SPACE+MAPINITWORD
	.long	0x33a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x33b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x33c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x33d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x33e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x33f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x340000+AT_MEM_SPACE+MAPINITWORD
	.long	0x341000+AT_MEM_SPACE+MAPINITWORD
	.long	0x342000+AT_MEM_SPACE+MAPINITWORD
	.long	0x343000+AT_MEM_SPACE+MAPINITWORD
	.long	0x344000+AT_MEM_SPACE+MAPINITWORD
	.long	0x345000+AT_MEM_SPACE+MAPINITWORD
	.long	0x346000+AT_MEM_SPACE+MAPINITWORD
	.long	0x347000+AT_MEM_SPACE+MAPINITWORD
	.long	0x348000+AT_MEM_SPACE+MAPINITWORD
	.long	0x349000+AT_MEM_SPACE+MAPINITWORD
	.long	0x34a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x34b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x34c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x34d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x34e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x34f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x350000+AT_MEM_SPACE+MAPINITWORD
	.long	0x351000+AT_MEM_SPACE+MAPINITWORD
	.long	0x352000+AT_MEM_SPACE+MAPINITWORD
	.long	0x353000+AT_MEM_SPACE+MAPINITWORD
	.long	0x354000+AT_MEM_SPACE+MAPINITWORD
	.long	0x355000+AT_MEM_SPACE+MAPINITWORD
	.long	0x356000+AT_MEM_SPACE+MAPINITWORD
	.long	0x357000+AT_MEM_SPACE+MAPINITWORD
	.long	0x358000+AT_MEM_SPACE+MAPINITWORD
	.long	0x359000+AT_MEM_SPACE+MAPINITWORD
	.long	0x35a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x35b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x35c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x35d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x35e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x35f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x360000+AT_MEM_SPACE+MAPINITWORD
	.long	0x361000+AT_MEM_SPACE+MAPINITWORD
	.long	0x362000+AT_MEM_SPACE+MAPINITWORD
	.long	0x363000+AT_MEM_SPACE+MAPINITWORD
	.long	0x364000+AT_MEM_SPACE+MAPINITWORD
	.long	0x365000+AT_MEM_SPACE+MAPINITWORD
	.long	0x366000+AT_MEM_SPACE+MAPINITWORD
	.long	0x367000+AT_MEM_SPACE+MAPINITWORD
	.long	0x368000+AT_MEM_SPACE+MAPINITWORD
	.long	0x369000+AT_MEM_SPACE+MAPINITWORD
	.long	0x36a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x36b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x36c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x36d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x36e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x36f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x370000+AT_MEM_SPACE+MAPINITWORD
	.long	0x371000+AT_MEM_SPACE+MAPINITWORD
	.long	0x372000+AT_MEM_SPACE+MAPINITWORD
	.long	0x373000+AT_MEM_SPACE+MAPINITWORD
	.long	0x374000+AT_MEM_SPACE+MAPINITWORD
	.long	0x375000+AT_MEM_SPACE+MAPINITWORD
	.long	0x376000+AT_MEM_SPACE+MAPINITWORD
	.long	0x377000+AT_MEM_SPACE+MAPINITWORD
	.long	0x378000+AT_MEM_SPACE+MAPINITWORD
	.long	0x379000+AT_MEM_SPACE+MAPINITWORD
	.long	0x37a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x37b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x37c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x37d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x37e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x37f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x380000+AT_MEM_SPACE+MAPINITWORD
	.long	0x381000+AT_MEM_SPACE+MAPINITWORD
	.long	0x382000+AT_MEM_SPACE+MAPINITWORD
	.long	0x383000+AT_MEM_SPACE+MAPINITWORD
	.long	0x384000+AT_MEM_SPACE+MAPINITWORD
	.long	0x385000+AT_MEM_SPACE+MAPINITWORD
	.long	0x386000+AT_MEM_SPACE+MAPINITWORD
	.long	0x387000+AT_MEM_SPACE+MAPINITWORD
	.long	0x388000+AT_MEM_SPACE+MAPINITWORD
	.long	0x389000+AT_MEM_SPACE+MAPINITWORD
	.long	0x38a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x38b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x38c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x38d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x38e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x38f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x390000+AT_MEM_SPACE+MAPINITWORD
	.long	0x391000+AT_MEM_SPACE+MAPINITWORD
	.long	0x392000+AT_MEM_SPACE+MAPINITWORD
	.long	0x393000+AT_MEM_SPACE+MAPINITWORD
	.long	0x394000+AT_MEM_SPACE+MAPINITWORD
	.long	0x395000+AT_MEM_SPACE+MAPINITWORD
	.long	0x396000+AT_MEM_SPACE+MAPINITWORD
	.long	0x397000+AT_MEM_SPACE+MAPINITWORD
	.long	0x398000+AT_MEM_SPACE+MAPINITWORD
	.long	0x399000+AT_MEM_SPACE+MAPINITWORD
	.long	0x39a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x39b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x39c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x39d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x39e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x39f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3a0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3a1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3a2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3a3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3a4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3a5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3a6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3a7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3a8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3a9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3aa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3ab000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3ac000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3ad000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3ae000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3af000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3b0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3b1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3b2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3b3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3b4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3b5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3b6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3b7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3b8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3b9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3ba000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3bb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3bc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3bd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3be000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3bf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3c0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3c1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3c2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3c3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3c4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3c5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3c6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3c7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3c8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3c9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3ca000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3cb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3cc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3cd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3ce000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3cf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3d0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3d1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3d2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3d3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3d4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3d5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3d6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3d7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3d8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3d9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3da000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3db000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3dc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3dd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3de000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3df000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3e0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3e1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3e2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3e3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3e4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3e5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3e6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3e7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3e8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3e9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3ea000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3eb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3ec000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3ed000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3ee000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3ef000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3f0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3f1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3f2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3f3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3f4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3f5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3f6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3f7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3f8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3f9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3fa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3fb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3fc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3fd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3fe000+AT_MEM_SPACE+MAPINITWORD
	.long	0x3ff000+AT_MEM_SPACE+MAPINITWORD

# PAGE 5
# NOT AVAILABLE FROM LOGICAL MEMORY
# BUS-MASTER 2nd level page table entries
pagebm1:
	.long	0x400000+AT_MEM_SPACE+MAPINITWORD
	.long	0x401000+AT_MEM_SPACE+MAPINITWORD
	.long	0x402000+AT_MEM_SPACE+MAPINITWORD
	.long	0x403000+AT_MEM_SPACE+MAPINITWORD
	.long	0x404000+AT_MEM_SPACE+MAPINITWORD
	.long	0x405000+AT_MEM_SPACE+MAPINITWORD
	.long	0x406000+AT_MEM_SPACE+MAPINITWORD
	.long	0x407000+AT_MEM_SPACE+MAPINITWORD
	.long	0x408000+AT_MEM_SPACE+MAPINITWORD
	.long	0x409000+AT_MEM_SPACE+MAPINITWORD
	.long	0x40a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x40b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x40c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x40d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x40e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x40f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x410000+AT_MEM_SPACE+MAPINITWORD
	.long	0x411000+AT_MEM_SPACE+MAPINITWORD
	.long	0x412000+AT_MEM_SPACE+MAPINITWORD
	.long	0x413000+AT_MEM_SPACE+MAPINITWORD
	.long	0x414000+AT_MEM_SPACE+MAPINITWORD
	.long	0x415000+AT_MEM_SPACE+MAPINITWORD
	.long	0x416000+AT_MEM_SPACE+MAPINITWORD
	.long	0x417000+AT_MEM_SPACE+MAPINITWORD
	.long	0x418000+AT_MEM_SPACE+MAPINITWORD
	.long	0x419000+AT_MEM_SPACE+MAPINITWORD
	.long	0x41a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x41b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x41c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x41d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x41e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x41f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x420000+AT_MEM_SPACE+MAPINITWORD
	.long	0x421000+AT_MEM_SPACE+MAPINITWORD
	.long	0x422000+AT_MEM_SPACE+MAPINITWORD
	.long	0x423000+AT_MEM_SPACE+MAPINITWORD
	.long	0x424000+AT_MEM_SPACE+MAPINITWORD
	.long	0x425000+AT_MEM_SPACE+MAPINITWORD
	.long	0x426000+AT_MEM_SPACE+MAPINITWORD
	.long	0x427000+AT_MEM_SPACE+MAPINITWORD
	.long	0x428000+AT_MEM_SPACE+MAPINITWORD
	.long	0x429000+AT_MEM_SPACE+MAPINITWORD
	.long	0x42a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x42b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x42c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x42d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x42e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x42f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x430000+AT_MEM_SPACE+MAPINITWORD
	.long	0x431000+AT_MEM_SPACE+MAPINITWORD
	.long	0x432000+AT_MEM_SPACE+MAPINITWORD
	.long	0x433000+AT_MEM_SPACE+MAPINITWORD
	.long	0x434000+AT_MEM_SPACE+MAPINITWORD
	.long	0x435000+AT_MEM_SPACE+MAPINITWORD
	.long	0x436000+AT_MEM_SPACE+MAPINITWORD
	.long	0x437000+AT_MEM_SPACE+MAPINITWORD
	.long	0x438000+AT_MEM_SPACE+MAPINITWORD
	.long	0x439000+AT_MEM_SPACE+MAPINITWORD
	.long	0x43a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x43b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x43c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x43d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x43e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x43f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x440000+AT_MEM_SPACE+MAPINITWORD
	.long	0x441000+AT_MEM_SPACE+MAPINITWORD
	.long	0x442000+AT_MEM_SPACE+MAPINITWORD
	.long	0x443000+AT_MEM_SPACE+MAPINITWORD
	.long	0x444000+AT_MEM_SPACE+MAPINITWORD
	.long	0x445000+AT_MEM_SPACE+MAPINITWORD
	.long	0x446000+AT_MEM_SPACE+MAPINITWORD
	.long	0x447000+AT_MEM_SPACE+MAPINITWORD
	.long	0x448000+AT_MEM_SPACE+MAPINITWORD
	.long	0x449000+AT_MEM_SPACE+MAPINITWORD
	.long	0x44a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x44b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x44c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x44d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x44e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x44f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x450000+AT_MEM_SPACE+MAPINITWORD
	.long	0x451000+AT_MEM_SPACE+MAPINITWORD
	.long	0x452000+AT_MEM_SPACE+MAPINITWORD
	.long	0x453000+AT_MEM_SPACE+MAPINITWORD
	.long	0x454000+AT_MEM_SPACE+MAPINITWORD
	.long	0x455000+AT_MEM_SPACE+MAPINITWORD
	.long	0x456000+AT_MEM_SPACE+MAPINITWORD
	.long	0x457000+AT_MEM_SPACE+MAPINITWORD
	.long	0x458000+AT_MEM_SPACE+MAPINITWORD
	.long	0x459000+AT_MEM_SPACE+MAPINITWORD
	.long	0x45a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x45b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x45c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x45d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x45e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x45f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x460000+AT_MEM_SPACE+MAPINITWORD
	.long	0x461000+AT_MEM_SPACE+MAPINITWORD
	.long	0x462000+AT_MEM_SPACE+MAPINITWORD
	.long	0x463000+AT_MEM_SPACE+MAPINITWORD
	.long	0x464000+AT_MEM_SPACE+MAPINITWORD
	.long	0x465000+AT_MEM_SPACE+MAPINITWORD
	.long	0x466000+AT_MEM_SPACE+MAPINITWORD
	.long	0x467000+AT_MEM_SPACE+MAPINITWORD
	.long	0x468000+AT_MEM_SPACE+MAPINITWORD
	.long	0x469000+AT_MEM_SPACE+MAPINITWORD
	.long	0x46a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x46b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x46c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x46d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x46e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x46f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x470000+AT_MEM_SPACE+MAPINITWORD
	.long	0x471000+AT_MEM_SPACE+MAPINITWORD
	.long	0x472000+AT_MEM_SPACE+MAPINITWORD
	.long	0x473000+AT_MEM_SPACE+MAPINITWORD
	.long	0x474000+AT_MEM_SPACE+MAPINITWORD
	.long	0x475000+AT_MEM_SPACE+MAPINITWORD
	.long	0x476000+AT_MEM_SPACE+MAPINITWORD
	.long	0x477000+AT_MEM_SPACE+MAPINITWORD
	.long	0x478000+AT_MEM_SPACE+MAPINITWORD
	.long	0x479000+AT_MEM_SPACE+MAPINITWORD
	.long	0x47a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x47b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x47c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x47d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x47e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x47f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x480000+AT_MEM_SPACE+MAPINITWORD
	.long	0x481000+AT_MEM_SPACE+MAPINITWORD
	.long	0x482000+AT_MEM_SPACE+MAPINITWORD
	.long	0x483000+AT_MEM_SPACE+MAPINITWORD
	.long	0x484000+AT_MEM_SPACE+MAPINITWORD
	.long	0x485000+AT_MEM_SPACE+MAPINITWORD
	.long	0x486000+AT_MEM_SPACE+MAPINITWORD
	.long	0x487000+AT_MEM_SPACE+MAPINITWORD
	.long	0x488000+AT_MEM_SPACE+MAPINITWORD
	.long	0x489000+AT_MEM_SPACE+MAPINITWORD
	.long	0x48a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x48b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x48c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x48d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x48e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x48f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x490000+AT_MEM_SPACE+MAPINITWORD
	.long	0x491000+AT_MEM_SPACE+MAPINITWORD
	.long	0x492000+AT_MEM_SPACE+MAPINITWORD
	.long	0x493000+AT_MEM_SPACE+MAPINITWORD
	.long	0x494000+AT_MEM_SPACE+MAPINITWORD
	.long	0x495000+AT_MEM_SPACE+MAPINITWORD
	.long	0x496000+AT_MEM_SPACE+MAPINITWORD
	.long	0x497000+AT_MEM_SPACE+MAPINITWORD
	.long	0x498000+AT_MEM_SPACE+MAPINITWORD
	.long	0x499000+AT_MEM_SPACE+MAPINITWORD
	.long	0x49a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x49b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x49c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x49d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x49e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x49f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4a0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4a1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4a2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4a3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4a4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4a5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4a6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4a7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4a8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4a9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4aa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4ab000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4ac000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4ad000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4ae000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4af000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4b0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4b1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4b2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4b3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4b4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4b5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4b6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4b7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4b8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4b9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4ba000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4bb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4bc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4bd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4be000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4bf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4c0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4c1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4c2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4c3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4c4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4c5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4c6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4c7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4c8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4c9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4ca000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4cb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4cc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4cd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4ce000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4cf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4d0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4d1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4d2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4d3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4d4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4d5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4d6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4d7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4d8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4d9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4da000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4db000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4dc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4dd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4de000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4df000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4e0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4e1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4e2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4e3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4e4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4e5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4e6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4e7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4e8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4e9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4ea000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4eb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4ec000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4ed000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4ee000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4ef000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4f0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4f1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4f2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4f3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4f4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4f5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4f6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4f7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4f8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4f9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4fa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4fb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4fc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4fd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4fe000+AT_MEM_SPACE+MAPINITWORD
	.long	0x4ff000+AT_MEM_SPACE+MAPINITWORD
	.long	0x500000+AT_MEM_SPACE+MAPINITWORD
	.long	0x501000+AT_MEM_SPACE+MAPINITWORD
	.long	0x502000+AT_MEM_SPACE+MAPINITWORD
	.long	0x503000+AT_MEM_SPACE+MAPINITWORD
	.long	0x504000+AT_MEM_SPACE+MAPINITWORD
	.long	0x505000+AT_MEM_SPACE+MAPINITWORD
	.long	0x506000+AT_MEM_SPACE+MAPINITWORD
	.long	0x507000+AT_MEM_SPACE+MAPINITWORD
	.long	0x508000+AT_MEM_SPACE+MAPINITWORD
	.long	0x509000+AT_MEM_SPACE+MAPINITWORD
	.long	0x50a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x50b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x50c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x50d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x50e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x50f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x510000+AT_MEM_SPACE+MAPINITWORD
	.long	0x511000+AT_MEM_SPACE+MAPINITWORD
	.long	0x512000+AT_MEM_SPACE+MAPINITWORD
	.long	0x513000+AT_MEM_SPACE+MAPINITWORD
	.long	0x514000+AT_MEM_SPACE+MAPINITWORD
	.long	0x515000+AT_MEM_SPACE+MAPINITWORD
	.long	0x516000+AT_MEM_SPACE+MAPINITWORD
	.long	0x517000+AT_MEM_SPACE+MAPINITWORD
	.long	0x518000+AT_MEM_SPACE+MAPINITWORD
	.long	0x519000+AT_MEM_SPACE+MAPINITWORD
	.long	0x51a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x51b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x51c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x51d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x51e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x51f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x520000+AT_MEM_SPACE+MAPINITWORD
	.long	0x521000+AT_MEM_SPACE+MAPINITWORD
	.long	0x522000+AT_MEM_SPACE+MAPINITWORD
	.long	0x523000+AT_MEM_SPACE+MAPINITWORD
	.long	0x524000+AT_MEM_SPACE+MAPINITWORD
	.long	0x525000+AT_MEM_SPACE+MAPINITWORD
	.long	0x526000+AT_MEM_SPACE+MAPINITWORD
	.long	0x527000+AT_MEM_SPACE+MAPINITWORD
	.long	0x528000+AT_MEM_SPACE+MAPINITWORD
	.long	0x529000+AT_MEM_SPACE+MAPINITWORD
	.long	0x52a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x52b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x52c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x52d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x52e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x52f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x530000+AT_MEM_SPACE+MAPINITWORD
	.long	0x531000+AT_MEM_SPACE+MAPINITWORD
	.long	0x532000+AT_MEM_SPACE+MAPINITWORD
	.long	0x533000+AT_MEM_SPACE+MAPINITWORD
	.long	0x534000+AT_MEM_SPACE+MAPINITWORD
	.long	0x535000+AT_MEM_SPACE+MAPINITWORD
	.long	0x536000+AT_MEM_SPACE+MAPINITWORD
	.long	0x537000+AT_MEM_SPACE+MAPINITWORD
	.long	0x538000+AT_MEM_SPACE+MAPINITWORD
	.long	0x539000+AT_MEM_SPACE+MAPINITWORD
	.long	0x53a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x53b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x53c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x53d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x53e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x53f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x540000+AT_MEM_SPACE+MAPINITWORD
	.long	0x541000+AT_MEM_SPACE+MAPINITWORD
	.long	0x542000+AT_MEM_SPACE+MAPINITWORD
	.long	0x543000+AT_MEM_SPACE+MAPINITWORD
	.long	0x544000+AT_MEM_SPACE+MAPINITWORD
	.long	0x545000+AT_MEM_SPACE+MAPINITWORD
	.long	0x546000+AT_MEM_SPACE+MAPINITWORD
	.long	0x547000+AT_MEM_SPACE+MAPINITWORD
	.long	0x548000+AT_MEM_SPACE+MAPINITWORD
	.long	0x549000+AT_MEM_SPACE+MAPINITWORD
	.long	0x54a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x54b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x54c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x54d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x54e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x54f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x550000+AT_MEM_SPACE+MAPINITWORD
	.long	0x551000+AT_MEM_SPACE+MAPINITWORD
	.long	0x552000+AT_MEM_SPACE+MAPINITWORD
	.long	0x553000+AT_MEM_SPACE+MAPINITWORD
	.long	0x554000+AT_MEM_SPACE+MAPINITWORD
	.long	0x555000+AT_MEM_SPACE+MAPINITWORD
	.long	0x556000+AT_MEM_SPACE+MAPINITWORD
	.long	0x557000+AT_MEM_SPACE+MAPINITWORD
	.long	0x558000+AT_MEM_SPACE+MAPINITWORD
	.long	0x559000+AT_MEM_SPACE+MAPINITWORD
	.long	0x55a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x55b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x55c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x55d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x55e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x55f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x560000+AT_MEM_SPACE+MAPINITWORD
	.long	0x561000+AT_MEM_SPACE+MAPINITWORD
	.long	0x562000+AT_MEM_SPACE+MAPINITWORD
	.long	0x563000+AT_MEM_SPACE+MAPINITWORD
	.long	0x564000+AT_MEM_SPACE+MAPINITWORD
	.long	0x565000+AT_MEM_SPACE+MAPINITWORD
	.long	0x566000+AT_MEM_SPACE+MAPINITWORD
	.long	0x567000+AT_MEM_SPACE+MAPINITWORD
	.long	0x568000+AT_MEM_SPACE+MAPINITWORD
	.long	0x569000+AT_MEM_SPACE+MAPINITWORD
	.long	0x56a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x56b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x56c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x56d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x56e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x56f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x570000+AT_MEM_SPACE+MAPINITWORD
	.long	0x571000+AT_MEM_SPACE+MAPINITWORD
	.long	0x572000+AT_MEM_SPACE+MAPINITWORD
	.long	0x573000+AT_MEM_SPACE+MAPINITWORD
	.long	0x574000+AT_MEM_SPACE+MAPINITWORD
	.long	0x575000+AT_MEM_SPACE+MAPINITWORD
	.long	0x576000+AT_MEM_SPACE+MAPINITWORD
	.long	0x577000+AT_MEM_SPACE+MAPINITWORD
	.long	0x578000+AT_MEM_SPACE+MAPINITWORD
	.long	0x579000+AT_MEM_SPACE+MAPINITWORD
	.long	0x57a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x57b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x57c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x57d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x57e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x57f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x580000+AT_MEM_SPACE+MAPINITWORD
	.long	0x581000+AT_MEM_SPACE+MAPINITWORD
	.long	0x582000+AT_MEM_SPACE+MAPINITWORD
	.long	0x583000+AT_MEM_SPACE+MAPINITWORD
	.long	0x584000+AT_MEM_SPACE+MAPINITWORD
	.long	0x585000+AT_MEM_SPACE+MAPINITWORD
	.long	0x586000+AT_MEM_SPACE+MAPINITWORD
	.long	0x587000+AT_MEM_SPACE+MAPINITWORD
	.long	0x588000+AT_MEM_SPACE+MAPINITWORD
	.long	0x589000+AT_MEM_SPACE+MAPINITWORD
	.long	0x58a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x58b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x58c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x58d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x58e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x58f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x590000+AT_MEM_SPACE+MAPINITWORD
	.long	0x591000+AT_MEM_SPACE+MAPINITWORD
	.long	0x592000+AT_MEM_SPACE+MAPINITWORD
	.long	0x593000+AT_MEM_SPACE+MAPINITWORD
	.long	0x594000+AT_MEM_SPACE+MAPINITWORD
	.long	0x595000+AT_MEM_SPACE+MAPINITWORD
	.long	0x596000+AT_MEM_SPACE+MAPINITWORD
	.long	0x597000+AT_MEM_SPACE+MAPINITWORD
	.long	0x598000+AT_MEM_SPACE+MAPINITWORD
	.long	0x599000+AT_MEM_SPACE+MAPINITWORD
	.long	0x59a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x59b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x59c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x59d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x59e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x59f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5a0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5a1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5a2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5a3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5a4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5a5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5a6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5a7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5a8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5a9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5aa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5ab000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5ac000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5ad000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5ae000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5af000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5b0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5b1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5b2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5b3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5b4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5b5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5b6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5b7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5b8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5b9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5ba000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5bb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5bc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5bd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5be000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5bf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5c0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5c1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5c2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5c3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5c4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5c5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5c6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5c7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5c8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5c9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5ca000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5cb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5cc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5cd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5ce000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5cf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5d0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5d1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5d2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5d3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5d4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5d5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5d6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5d7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5d8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5d9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5da000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5db000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5dc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5dd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5de000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5df000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5e0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5e1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5e2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5e3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5e4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5e5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5e6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5e7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5e8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5e9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5ea000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5eb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5ec000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5ed000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5ee000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5ef000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5f0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5f1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5f2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5f3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5f4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5f5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5f6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5f7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5f8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5f9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5fa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5fb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5fc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5fd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5fe000+AT_MEM_SPACE+MAPINITWORD
	.long	0x5ff000+AT_MEM_SPACE+MAPINITWORD
	.long	0x600000+AT_MEM_SPACE+MAPINITWORD
	.long	0x601000+AT_MEM_SPACE+MAPINITWORD
	.long	0x602000+AT_MEM_SPACE+MAPINITWORD
	.long	0x603000+AT_MEM_SPACE+MAPINITWORD
	.long	0x604000+AT_MEM_SPACE+MAPINITWORD
	.long	0x605000+AT_MEM_SPACE+MAPINITWORD
	.long	0x606000+AT_MEM_SPACE+MAPINITWORD
	.long	0x607000+AT_MEM_SPACE+MAPINITWORD
	.long	0x608000+AT_MEM_SPACE+MAPINITWORD
	.long	0x609000+AT_MEM_SPACE+MAPINITWORD
	.long	0x60a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x60b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x60c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x60d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x60e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x60f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x610000+AT_MEM_SPACE+MAPINITWORD
	.long	0x611000+AT_MEM_SPACE+MAPINITWORD
	.long	0x612000+AT_MEM_SPACE+MAPINITWORD
	.long	0x613000+AT_MEM_SPACE+MAPINITWORD
	.long	0x614000+AT_MEM_SPACE+MAPINITWORD
	.long	0x615000+AT_MEM_SPACE+MAPINITWORD
	.long	0x616000+AT_MEM_SPACE+MAPINITWORD
	.long	0x617000+AT_MEM_SPACE+MAPINITWORD
	.long	0x618000+AT_MEM_SPACE+MAPINITWORD
	.long	0x619000+AT_MEM_SPACE+MAPINITWORD
	.long	0x61a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x61b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x61c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x61d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x61e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x61f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x620000+AT_MEM_SPACE+MAPINITWORD
	.long	0x621000+AT_MEM_SPACE+MAPINITWORD
	.long	0x622000+AT_MEM_SPACE+MAPINITWORD
	.long	0x623000+AT_MEM_SPACE+MAPINITWORD
	.long	0x624000+AT_MEM_SPACE+MAPINITWORD
	.long	0x625000+AT_MEM_SPACE+MAPINITWORD
	.long	0x626000+AT_MEM_SPACE+MAPINITWORD
	.long	0x627000+AT_MEM_SPACE+MAPINITWORD
	.long	0x628000+AT_MEM_SPACE+MAPINITWORD
	.long	0x629000+AT_MEM_SPACE+MAPINITWORD
	.long	0x62a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x62b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x62c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x62d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x62e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x62f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x630000+AT_MEM_SPACE+MAPINITWORD
	.long	0x631000+AT_MEM_SPACE+MAPINITWORD
	.long	0x632000+AT_MEM_SPACE+MAPINITWORD
	.long	0x633000+AT_MEM_SPACE+MAPINITWORD
	.long	0x634000+AT_MEM_SPACE+MAPINITWORD
	.long	0x635000+AT_MEM_SPACE+MAPINITWORD
	.long	0x636000+AT_MEM_SPACE+MAPINITWORD
	.long	0x637000+AT_MEM_SPACE+MAPINITWORD
	.long	0x638000+AT_MEM_SPACE+MAPINITWORD
	.long	0x639000+AT_MEM_SPACE+MAPINITWORD
	.long	0x63a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x63b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x63c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x63d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x63e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x63f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x640000+AT_MEM_SPACE+MAPINITWORD
	.long	0x641000+AT_MEM_SPACE+MAPINITWORD
	.long	0x642000+AT_MEM_SPACE+MAPINITWORD
	.long	0x643000+AT_MEM_SPACE+MAPINITWORD
	.long	0x644000+AT_MEM_SPACE+MAPINITWORD
	.long	0x645000+AT_MEM_SPACE+MAPINITWORD
	.long	0x646000+AT_MEM_SPACE+MAPINITWORD
	.long	0x647000+AT_MEM_SPACE+MAPINITWORD
	.long	0x648000+AT_MEM_SPACE+MAPINITWORD
	.long	0x649000+AT_MEM_SPACE+MAPINITWORD
	.long	0x64a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x64b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x64c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x64d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x64e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x64f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x650000+AT_MEM_SPACE+MAPINITWORD
	.long	0x651000+AT_MEM_SPACE+MAPINITWORD
	.long	0x652000+AT_MEM_SPACE+MAPINITWORD
	.long	0x653000+AT_MEM_SPACE+MAPINITWORD
	.long	0x654000+AT_MEM_SPACE+MAPINITWORD
	.long	0x655000+AT_MEM_SPACE+MAPINITWORD
	.long	0x656000+AT_MEM_SPACE+MAPINITWORD
	.long	0x657000+AT_MEM_SPACE+MAPINITWORD
	.long	0x658000+AT_MEM_SPACE+MAPINITWORD
	.long	0x659000+AT_MEM_SPACE+MAPINITWORD
	.long	0x65a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x65b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x65c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x65d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x65e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x65f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x660000+AT_MEM_SPACE+MAPINITWORD
	.long	0x661000+AT_MEM_SPACE+MAPINITWORD
	.long	0x662000+AT_MEM_SPACE+MAPINITWORD
	.long	0x663000+AT_MEM_SPACE+MAPINITWORD
	.long	0x664000+AT_MEM_SPACE+MAPINITWORD
	.long	0x665000+AT_MEM_SPACE+MAPINITWORD
	.long	0x666000+AT_MEM_SPACE+MAPINITWORD
	.long	0x667000+AT_MEM_SPACE+MAPINITWORD
	.long	0x668000+AT_MEM_SPACE+MAPINITWORD
	.long	0x669000+AT_MEM_SPACE+MAPINITWORD
	.long	0x66a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x66b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x66c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x66d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x66e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x66f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x670000+AT_MEM_SPACE+MAPINITWORD
	.long	0x671000+AT_MEM_SPACE+MAPINITWORD
	.long	0x672000+AT_MEM_SPACE+MAPINITWORD
	.long	0x673000+AT_MEM_SPACE+MAPINITWORD
	.long	0x674000+AT_MEM_SPACE+MAPINITWORD
	.long	0x675000+AT_MEM_SPACE+MAPINITWORD
	.long	0x676000+AT_MEM_SPACE+MAPINITWORD
	.long	0x677000+AT_MEM_SPACE+MAPINITWORD
	.long	0x678000+AT_MEM_SPACE+MAPINITWORD
	.long	0x679000+AT_MEM_SPACE+MAPINITWORD
	.long	0x67a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x67b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x67c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x67d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x67e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x67f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x680000+AT_MEM_SPACE+MAPINITWORD
	.long	0x681000+AT_MEM_SPACE+MAPINITWORD
	.long	0x682000+AT_MEM_SPACE+MAPINITWORD
	.long	0x683000+AT_MEM_SPACE+MAPINITWORD
	.long	0x684000+AT_MEM_SPACE+MAPINITWORD
	.long	0x685000+AT_MEM_SPACE+MAPINITWORD
	.long	0x686000+AT_MEM_SPACE+MAPINITWORD
	.long	0x687000+AT_MEM_SPACE+MAPINITWORD
	.long	0x688000+AT_MEM_SPACE+MAPINITWORD
	.long	0x689000+AT_MEM_SPACE+MAPINITWORD
	.long	0x68a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x68b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x68c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x68d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x68e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x68f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x690000+AT_MEM_SPACE+MAPINITWORD
	.long	0x691000+AT_MEM_SPACE+MAPINITWORD
	.long	0x692000+AT_MEM_SPACE+MAPINITWORD
	.long	0x693000+AT_MEM_SPACE+MAPINITWORD
	.long	0x694000+AT_MEM_SPACE+MAPINITWORD
	.long	0x695000+AT_MEM_SPACE+MAPINITWORD
	.long	0x696000+AT_MEM_SPACE+MAPINITWORD
	.long	0x697000+AT_MEM_SPACE+MAPINITWORD
	.long	0x698000+AT_MEM_SPACE+MAPINITWORD
	.long	0x699000+AT_MEM_SPACE+MAPINITWORD
	.long	0x69a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x69b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x69c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x69d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x69e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x69f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6a0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6a1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6a2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6a3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6a4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6a5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6a6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6a7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6a8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6a9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6aa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6ab000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6ac000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6ad000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6ae000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6af000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6b0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6b1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6b2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6b3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6b4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6b5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6b6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6b7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6b8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6b9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6ba000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6bb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6bc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6bd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6be000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6bf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6c0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6c1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6c2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6c3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6c4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6c5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6c6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6c7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6c8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6c9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6ca000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6cb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6cc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6cd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6ce000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6cf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6d0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6d1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6d2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6d3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6d4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6d5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6d6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6d7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6d8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6d9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6da000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6db000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6dc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6dd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6de000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6df000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6e0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6e1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6e2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6e3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6e4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6e5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6e6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6e7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6e8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6e9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6ea000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6eb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6ec000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6ed000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6ee000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6ef000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6f0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6f1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6f2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6f3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6f4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6f5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6f6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6f7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6f8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6f9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6fa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6fb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6fc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6fd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6fe000+AT_MEM_SPACE+MAPINITWORD
	.long	0x6ff000+AT_MEM_SPACE+MAPINITWORD
	.long	0x700000+AT_MEM_SPACE+MAPINITWORD
	.long	0x701000+AT_MEM_SPACE+MAPINITWORD
	.long	0x702000+AT_MEM_SPACE+MAPINITWORD
	.long	0x703000+AT_MEM_SPACE+MAPINITWORD
	.long	0x704000+AT_MEM_SPACE+MAPINITWORD
	.long	0x705000+AT_MEM_SPACE+MAPINITWORD
	.long	0x706000+AT_MEM_SPACE+MAPINITWORD
	.long	0x707000+AT_MEM_SPACE+MAPINITWORD
	.long	0x708000+AT_MEM_SPACE+MAPINITWORD
	.long	0x709000+AT_MEM_SPACE+MAPINITWORD
	.long	0x70a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x70b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x70c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x70d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x70e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x70f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x710000+AT_MEM_SPACE+MAPINITWORD
	.long	0x711000+AT_MEM_SPACE+MAPINITWORD
	.long	0x712000+AT_MEM_SPACE+MAPINITWORD
	.long	0x713000+AT_MEM_SPACE+MAPINITWORD
	.long	0x714000+AT_MEM_SPACE+MAPINITWORD
	.long	0x715000+AT_MEM_SPACE+MAPINITWORD
	.long	0x716000+AT_MEM_SPACE+MAPINITWORD
	.long	0x717000+AT_MEM_SPACE+MAPINITWORD
	.long	0x718000+AT_MEM_SPACE+MAPINITWORD
	.long	0x719000+AT_MEM_SPACE+MAPINITWORD
	.long	0x71a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x71b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x71c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x71d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x71e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x71f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x720000+AT_MEM_SPACE+MAPINITWORD
	.long	0x721000+AT_MEM_SPACE+MAPINITWORD
	.long	0x722000+AT_MEM_SPACE+MAPINITWORD
	.long	0x723000+AT_MEM_SPACE+MAPINITWORD
	.long	0x724000+AT_MEM_SPACE+MAPINITWORD
	.long	0x725000+AT_MEM_SPACE+MAPINITWORD
	.long	0x726000+AT_MEM_SPACE+MAPINITWORD
	.long	0x727000+AT_MEM_SPACE+MAPINITWORD
	.long	0x728000+AT_MEM_SPACE+MAPINITWORD
	.long	0x729000+AT_MEM_SPACE+MAPINITWORD
	.long	0x72a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x72b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x72c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x72d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x72e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x72f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x730000+AT_MEM_SPACE+MAPINITWORD
	.long	0x731000+AT_MEM_SPACE+MAPINITWORD
	.long	0x732000+AT_MEM_SPACE+MAPINITWORD
	.long	0x733000+AT_MEM_SPACE+MAPINITWORD
	.long	0x734000+AT_MEM_SPACE+MAPINITWORD
	.long	0x735000+AT_MEM_SPACE+MAPINITWORD
	.long	0x736000+AT_MEM_SPACE+MAPINITWORD
	.long	0x737000+AT_MEM_SPACE+MAPINITWORD
	.long	0x738000+AT_MEM_SPACE+MAPINITWORD
	.long	0x739000+AT_MEM_SPACE+MAPINITWORD
	.long	0x73a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x73b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x73c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x73d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x73e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x73f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x740000+AT_MEM_SPACE+MAPINITWORD
	.long	0x741000+AT_MEM_SPACE+MAPINITWORD
	.long	0x742000+AT_MEM_SPACE+MAPINITWORD
	.long	0x743000+AT_MEM_SPACE+MAPINITWORD
	.long	0x744000+AT_MEM_SPACE+MAPINITWORD
	.long	0x745000+AT_MEM_SPACE+MAPINITWORD
	.long	0x746000+AT_MEM_SPACE+MAPINITWORD
	.long	0x747000+AT_MEM_SPACE+MAPINITWORD
	.long	0x748000+AT_MEM_SPACE+MAPINITWORD
	.long	0x749000+AT_MEM_SPACE+MAPINITWORD
	.long	0x74a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x74b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x74c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x74d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x74e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x74f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x750000+AT_MEM_SPACE+MAPINITWORD
	.long	0x751000+AT_MEM_SPACE+MAPINITWORD
	.long	0x752000+AT_MEM_SPACE+MAPINITWORD
	.long	0x753000+AT_MEM_SPACE+MAPINITWORD
	.long	0x754000+AT_MEM_SPACE+MAPINITWORD
	.long	0x755000+AT_MEM_SPACE+MAPINITWORD
	.long	0x756000+AT_MEM_SPACE+MAPINITWORD
	.long	0x757000+AT_MEM_SPACE+MAPINITWORD
	.long	0x758000+AT_MEM_SPACE+MAPINITWORD
	.long	0x759000+AT_MEM_SPACE+MAPINITWORD
	.long	0x75a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x75b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x75c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x75d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x75e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x75f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x760000+AT_MEM_SPACE+MAPINITWORD
	.long	0x761000+AT_MEM_SPACE+MAPINITWORD
	.long	0x762000+AT_MEM_SPACE+MAPINITWORD
	.long	0x763000+AT_MEM_SPACE+MAPINITWORD
	.long	0x764000+AT_MEM_SPACE+MAPINITWORD
	.long	0x765000+AT_MEM_SPACE+MAPINITWORD
	.long	0x766000+AT_MEM_SPACE+MAPINITWORD
	.long	0x767000+AT_MEM_SPACE+MAPINITWORD
	.long	0x768000+AT_MEM_SPACE+MAPINITWORD
	.long	0x769000+AT_MEM_SPACE+MAPINITWORD
	.long	0x76a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x76b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x76c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x76d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x76e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x76f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x770000+AT_MEM_SPACE+MAPINITWORD
	.long	0x771000+AT_MEM_SPACE+MAPINITWORD
	.long	0x772000+AT_MEM_SPACE+MAPINITWORD
	.long	0x773000+AT_MEM_SPACE+MAPINITWORD
	.long	0x774000+AT_MEM_SPACE+MAPINITWORD
	.long	0x775000+AT_MEM_SPACE+MAPINITWORD
	.long	0x776000+AT_MEM_SPACE+MAPINITWORD
	.long	0x777000+AT_MEM_SPACE+MAPINITWORD
	.long	0x778000+AT_MEM_SPACE+MAPINITWORD
	.long	0x779000+AT_MEM_SPACE+MAPINITWORD
	.long	0x77a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x77b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x77c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x77d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x77e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x77f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x780000+AT_MEM_SPACE+MAPINITWORD
	.long	0x781000+AT_MEM_SPACE+MAPINITWORD
	.long	0x782000+AT_MEM_SPACE+MAPINITWORD
	.long	0x783000+AT_MEM_SPACE+MAPINITWORD
	.long	0x784000+AT_MEM_SPACE+MAPINITWORD
	.long	0x785000+AT_MEM_SPACE+MAPINITWORD
	.long	0x786000+AT_MEM_SPACE+MAPINITWORD
	.long	0x787000+AT_MEM_SPACE+MAPINITWORD
	.long	0x788000+AT_MEM_SPACE+MAPINITWORD
	.long	0x789000+AT_MEM_SPACE+MAPINITWORD
	.long	0x78a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x78b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x78c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x78d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x78e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x78f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x790000+AT_MEM_SPACE+MAPINITWORD
	.long	0x791000+AT_MEM_SPACE+MAPINITWORD
	.long	0x792000+AT_MEM_SPACE+MAPINITWORD
	.long	0x793000+AT_MEM_SPACE+MAPINITWORD
	.long	0x794000+AT_MEM_SPACE+MAPINITWORD
	.long	0x795000+AT_MEM_SPACE+MAPINITWORD
	.long	0x796000+AT_MEM_SPACE+MAPINITWORD
	.long	0x797000+AT_MEM_SPACE+MAPINITWORD
	.long	0x798000+AT_MEM_SPACE+MAPINITWORD
	.long	0x799000+AT_MEM_SPACE+MAPINITWORD
	.long	0x79a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x79b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x79c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x79d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x79e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x79f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7a0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7a1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7a2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7a3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7a4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7a5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7a6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7a7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7a8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7a9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7aa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7ab000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7ac000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7ad000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7ae000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7af000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7b0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7b1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7b2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7b3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7b4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7b5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7b6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7b7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7b8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7b9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7ba000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7bb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7bc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7bd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7be000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7bf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7c0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7c1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7c2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7c3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7c4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7c5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7c6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7c7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7c8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7c9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7ca000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7cb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7cc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7cd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7ce000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7cf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7d0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7d1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7d2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7d3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7d4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7d5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7d6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7d7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7d8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7d9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7da000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7db000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7dc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7dd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7de000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7df000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7e0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7e1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7e2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7e3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7e4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7e5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7e6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7e7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7e8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7e9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7ea000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7eb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7ec000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7ed000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7ee000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7ef000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7f0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7f1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7f2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7f3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7f4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7f5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7f6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7f7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7f8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7f9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7fa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7fb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7fc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7fd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7fe000+AT_MEM_SPACE+MAPINITWORD
	.long	0x7ff000+AT_MEM_SPACE+MAPINITWORD

# PAGE 6
# NOT AVAILABLE FROM LOGICAL MEMORY
# BUS-MASTER 2nd level page table entries
pagebm2:
	.long	0x800000+AT_MEM_SPACE+MAPINITWORD
	.long	0x801000+AT_MEM_SPACE+MAPINITWORD
	.long	0x802000+AT_MEM_SPACE+MAPINITWORD
	.long	0x803000+AT_MEM_SPACE+MAPINITWORD
	.long	0x804000+AT_MEM_SPACE+MAPINITWORD
	.long	0x805000+AT_MEM_SPACE+MAPINITWORD
	.long	0x806000+AT_MEM_SPACE+MAPINITWORD
	.long	0x807000+AT_MEM_SPACE+MAPINITWORD
	.long	0x808000+AT_MEM_SPACE+MAPINITWORD
	.long	0x809000+AT_MEM_SPACE+MAPINITWORD
	.long	0x80a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x80b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x80c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x80d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x80e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x80f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x810000+AT_MEM_SPACE+MAPINITWORD
	.long	0x811000+AT_MEM_SPACE+MAPINITWORD
	.long	0x812000+AT_MEM_SPACE+MAPINITWORD
	.long	0x813000+AT_MEM_SPACE+MAPINITWORD
	.long	0x814000+AT_MEM_SPACE+MAPINITWORD
	.long	0x815000+AT_MEM_SPACE+MAPINITWORD
	.long	0x816000+AT_MEM_SPACE+MAPINITWORD
	.long	0x817000+AT_MEM_SPACE+MAPINITWORD
	.long	0x818000+AT_MEM_SPACE+MAPINITWORD
	.long	0x819000+AT_MEM_SPACE+MAPINITWORD
	.long	0x81a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x81b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x81c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x81d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x81e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x81f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x820000+AT_MEM_SPACE+MAPINITWORD
	.long	0x821000+AT_MEM_SPACE+MAPINITWORD
	.long	0x822000+AT_MEM_SPACE+MAPINITWORD
	.long	0x823000+AT_MEM_SPACE+MAPINITWORD
	.long	0x824000+AT_MEM_SPACE+MAPINITWORD
	.long	0x825000+AT_MEM_SPACE+MAPINITWORD
	.long	0x826000+AT_MEM_SPACE+MAPINITWORD
	.long	0x827000+AT_MEM_SPACE+MAPINITWORD
	.long	0x828000+AT_MEM_SPACE+MAPINITWORD
	.long	0x829000+AT_MEM_SPACE+MAPINITWORD
	.long	0x82a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x82b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x82c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x82d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x82e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x82f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x830000+AT_MEM_SPACE+MAPINITWORD
	.long	0x831000+AT_MEM_SPACE+MAPINITWORD
	.long	0x832000+AT_MEM_SPACE+MAPINITWORD
	.long	0x833000+AT_MEM_SPACE+MAPINITWORD
	.long	0x834000+AT_MEM_SPACE+MAPINITWORD
	.long	0x835000+AT_MEM_SPACE+MAPINITWORD
	.long	0x836000+AT_MEM_SPACE+MAPINITWORD
	.long	0x837000+AT_MEM_SPACE+MAPINITWORD
	.long	0x838000+AT_MEM_SPACE+MAPINITWORD
	.long	0x839000+AT_MEM_SPACE+MAPINITWORD
	.long	0x83a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x83b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x83c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x83d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x83e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x83f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x840000+AT_MEM_SPACE+MAPINITWORD
	.long	0x841000+AT_MEM_SPACE+MAPINITWORD
	.long	0x842000+AT_MEM_SPACE+MAPINITWORD
	.long	0x843000+AT_MEM_SPACE+MAPINITWORD
	.long	0x844000+AT_MEM_SPACE+MAPINITWORD
	.long	0x845000+AT_MEM_SPACE+MAPINITWORD
	.long	0x846000+AT_MEM_SPACE+MAPINITWORD
	.long	0x847000+AT_MEM_SPACE+MAPINITWORD
	.long	0x848000+AT_MEM_SPACE+MAPINITWORD
	.long	0x849000+AT_MEM_SPACE+MAPINITWORD
	.long	0x84a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x84b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x84c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x84d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x84e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x84f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x850000+AT_MEM_SPACE+MAPINITWORD
	.long	0x851000+AT_MEM_SPACE+MAPINITWORD
	.long	0x852000+AT_MEM_SPACE+MAPINITWORD
	.long	0x853000+AT_MEM_SPACE+MAPINITWORD
	.long	0x854000+AT_MEM_SPACE+MAPINITWORD
	.long	0x855000+AT_MEM_SPACE+MAPINITWORD
	.long	0x856000+AT_MEM_SPACE+MAPINITWORD
	.long	0x857000+AT_MEM_SPACE+MAPINITWORD
	.long	0x858000+AT_MEM_SPACE+MAPINITWORD
	.long	0x859000+AT_MEM_SPACE+MAPINITWORD
	.long	0x85a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x85b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x85c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x85d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x85e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x85f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x860000+AT_MEM_SPACE+MAPINITWORD
	.long	0x861000+AT_MEM_SPACE+MAPINITWORD
	.long	0x862000+AT_MEM_SPACE+MAPINITWORD
	.long	0x863000+AT_MEM_SPACE+MAPINITWORD
	.long	0x864000+AT_MEM_SPACE+MAPINITWORD
	.long	0x865000+AT_MEM_SPACE+MAPINITWORD
	.long	0x866000+AT_MEM_SPACE+MAPINITWORD
	.long	0x867000+AT_MEM_SPACE+MAPINITWORD
	.long	0x868000+AT_MEM_SPACE+MAPINITWORD
	.long	0x869000+AT_MEM_SPACE+MAPINITWORD
	.long	0x86a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x86b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x86c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x86d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x86e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x86f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x870000+AT_MEM_SPACE+MAPINITWORD
	.long	0x871000+AT_MEM_SPACE+MAPINITWORD
	.long	0x872000+AT_MEM_SPACE+MAPINITWORD
	.long	0x873000+AT_MEM_SPACE+MAPINITWORD
	.long	0x874000+AT_MEM_SPACE+MAPINITWORD
	.long	0x875000+AT_MEM_SPACE+MAPINITWORD
	.long	0x876000+AT_MEM_SPACE+MAPINITWORD
	.long	0x877000+AT_MEM_SPACE+MAPINITWORD
	.long	0x878000+AT_MEM_SPACE+MAPINITWORD
	.long	0x879000+AT_MEM_SPACE+MAPINITWORD
	.long	0x87a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x87b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x87c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x87d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x87e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x87f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x880000+AT_MEM_SPACE+MAPINITWORD
	.long	0x881000+AT_MEM_SPACE+MAPINITWORD
	.long	0x882000+AT_MEM_SPACE+MAPINITWORD
	.long	0x883000+AT_MEM_SPACE+MAPINITWORD
	.long	0x884000+AT_MEM_SPACE+MAPINITWORD
	.long	0x885000+AT_MEM_SPACE+MAPINITWORD
	.long	0x886000+AT_MEM_SPACE+MAPINITWORD
	.long	0x887000+AT_MEM_SPACE+MAPINITWORD
	.long	0x888000+AT_MEM_SPACE+MAPINITWORD
	.long	0x889000+AT_MEM_SPACE+MAPINITWORD
	.long	0x88a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x88b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x88c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x88d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x88e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x88f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x890000+AT_MEM_SPACE+MAPINITWORD
	.long	0x891000+AT_MEM_SPACE+MAPINITWORD
	.long	0x892000+AT_MEM_SPACE+MAPINITWORD
	.long	0x893000+AT_MEM_SPACE+MAPINITWORD
	.long	0x894000+AT_MEM_SPACE+MAPINITWORD
	.long	0x895000+AT_MEM_SPACE+MAPINITWORD
	.long	0x896000+AT_MEM_SPACE+MAPINITWORD
	.long	0x897000+AT_MEM_SPACE+MAPINITWORD
	.long	0x898000+AT_MEM_SPACE+MAPINITWORD
	.long	0x899000+AT_MEM_SPACE+MAPINITWORD
	.long	0x89a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x89b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x89c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x89d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x89e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x89f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8a0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8a1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8a2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8a3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8a4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8a5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8a6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8a7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8a8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8a9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8aa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8ab000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8ac000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8ad000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8ae000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8af000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8b0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8b1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8b2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8b3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8b4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8b5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8b6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8b7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8b8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8b9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8ba000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8bb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8bc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8bd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8be000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8bf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8c0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8c1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8c2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8c3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8c4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8c5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8c6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8c7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8c8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8c9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8ca000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8cb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8cc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8cd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8ce000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8cf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8d0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8d1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8d2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8d3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8d4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8d5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8d6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8d7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8d8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8d9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8da000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8db000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8dc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8dd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8de000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8df000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8e0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8e1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8e2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8e3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8e4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8e5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8e6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8e7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8e8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8e9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8ea000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8eb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8ec000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8ed000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8ee000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8ef000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8f0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8f1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8f2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8f3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8f4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8f5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8f6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8f7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8f8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8f9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8fa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8fb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8fc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8fd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8fe000+AT_MEM_SPACE+MAPINITWORD
	.long	0x8ff000+AT_MEM_SPACE+MAPINITWORD
	.long	0x900000+AT_MEM_SPACE+MAPINITWORD
	.long	0x901000+AT_MEM_SPACE+MAPINITWORD
	.long	0x902000+AT_MEM_SPACE+MAPINITWORD
	.long	0x903000+AT_MEM_SPACE+MAPINITWORD
	.long	0x904000+AT_MEM_SPACE+MAPINITWORD
	.long	0x905000+AT_MEM_SPACE+MAPINITWORD
	.long	0x906000+AT_MEM_SPACE+MAPINITWORD
	.long	0x907000+AT_MEM_SPACE+MAPINITWORD
	.long	0x908000+AT_MEM_SPACE+MAPINITWORD
	.long	0x909000+AT_MEM_SPACE+MAPINITWORD
	.long	0x90a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x90b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x90c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x90d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x90e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x90f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x910000+AT_MEM_SPACE+MAPINITWORD
	.long	0x911000+AT_MEM_SPACE+MAPINITWORD
	.long	0x912000+AT_MEM_SPACE+MAPINITWORD
	.long	0x913000+AT_MEM_SPACE+MAPINITWORD
	.long	0x914000+AT_MEM_SPACE+MAPINITWORD
	.long	0x915000+AT_MEM_SPACE+MAPINITWORD
	.long	0x916000+AT_MEM_SPACE+MAPINITWORD
	.long	0x917000+AT_MEM_SPACE+MAPINITWORD
	.long	0x918000+AT_MEM_SPACE+MAPINITWORD
	.long	0x919000+AT_MEM_SPACE+MAPINITWORD
	.long	0x91a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x91b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x91c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x91d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x91e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x91f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x920000+AT_MEM_SPACE+MAPINITWORD
	.long	0x921000+AT_MEM_SPACE+MAPINITWORD
	.long	0x922000+AT_MEM_SPACE+MAPINITWORD
	.long	0x923000+AT_MEM_SPACE+MAPINITWORD
	.long	0x924000+AT_MEM_SPACE+MAPINITWORD
	.long	0x925000+AT_MEM_SPACE+MAPINITWORD
	.long	0x926000+AT_MEM_SPACE+MAPINITWORD
	.long	0x927000+AT_MEM_SPACE+MAPINITWORD
	.long	0x928000+AT_MEM_SPACE+MAPINITWORD
	.long	0x929000+AT_MEM_SPACE+MAPINITWORD
	.long	0x92a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x92b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x92c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x92d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x92e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x92f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x930000+AT_MEM_SPACE+MAPINITWORD
	.long	0x931000+AT_MEM_SPACE+MAPINITWORD
	.long	0x932000+AT_MEM_SPACE+MAPINITWORD
	.long	0x933000+AT_MEM_SPACE+MAPINITWORD
	.long	0x934000+AT_MEM_SPACE+MAPINITWORD
	.long	0x935000+AT_MEM_SPACE+MAPINITWORD
	.long	0x936000+AT_MEM_SPACE+MAPINITWORD
	.long	0x937000+AT_MEM_SPACE+MAPINITWORD
	.long	0x938000+AT_MEM_SPACE+MAPINITWORD
	.long	0x939000+AT_MEM_SPACE+MAPINITWORD
	.long	0x93a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x93b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x93c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x93d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x93e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x93f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x940000+AT_MEM_SPACE+MAPINITWORD
	.long	0x941000+AT_MEM_SPACE+MAPINITWORD
	.long	0x942000+AT_MEM_SPACE+MAPINITWORD
	.long	0x943000+AT_MEM_SPACE+MAPINITWORD
	.long	0x944000+AT_MEM_SPACE+MAPINITWORD
	.long	0x945000+AT_MEM_SPACE+MAPINITWORD
	.long	0x946000+AT_MEM_SPACE+MAPINITWORD
	.long	0x947000+AT_MEM_SPACE+MAPINITWORD
	.long	0x948000+AT_MEM_SPACE+MAPINITWORD
	.long	0x949000+AT_MEM_SPACE+MAPINITWORD
	.long	0x94a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x94b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x94c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x94d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x94e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x94f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x950000+AT_MEM_SPACE+MAPINITWORD
	.long	0x951000+AT_MEM_SPACE+MAPINITWORD
	.long	0x952000+AT_MEM_SPACE+MAPINITWORD
	.long	0x953000+AT_MEM_SPACE+MAPINITWORD
	.long	0x954000+AT_MEM_SPACE+MAPINITWORD
	.long	0x955000+AT_MEM_SPACE+MAPINITWORD
	.long	0x956000+AT_MEM_SPACE+MAPINITWORD
	.long	0x957000+AT_MEM_SPACE+MAPINITWORD
	.long	0x958000+AT_MEM_SPACE+MAPINITWORD
	.long	0x959000+AT_MEM_SPACE+MAPINITWORD
	.long	0x95a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x95b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x95c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x95d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x95e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x95f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x960000+AT_MEM_SPACE+MAPINITWORD
	.long	0x961000+AT_MEM_SPACE+MAPINITWORD
	.long	0x962000+AT_MEM_SPACE+MAPINITWORD
	.long	0x963000+AT_MEM_SPACE+MAPINITWORD
	.long	0x964000+AT_MEM_SPACE+MAPINITWORD
	.long	0x965000+AT_MEM_SPACE+MAPINITWORD
	.long	0x966000+AT_MEM_SPACE+MAPINITWORD
	.long	0x967000+AT_MEM_SPACE+MAPINITWORD
	.long	0x968000+AT_MEM_SPACE+MAPINITWORD
	.long	0x969000+AT_MEM_SPACE+MAPINITWORD
	.long	0x96a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x96b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x96c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x96d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x96e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x96f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x970000+AT_MEM_SPACE+MAPINITWORD
	.long	0x971000+AT_MEM_SPACE+MAPINITWORD
	.long	0x972000+AT_MEM_SPACE+MAPINITWORD
	.long	0x973000+AT_MEM_SPACE+MAPINITWORD
	.long	0x974000+AT_MEM_SPACE+MAPINITWORD
	.long	0x975000+AT_MEM_SPACE+MAPINITWORD
	.long	0x976000+AT_MEM_SPACE+MAPINITWORD
	.long	0x977000+AT_MEM_SPACE+MAPINITWORD
	.long	0x978000+AT_MEM_SPACE+MAPINITWORD
	.long	0x979000+AT_MEM_SPACE+MAPINITWORD
	.long	0x97a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x97b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x97c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x97d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x97e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x97f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x980000+AT_MEM_SPACE+MAPINITWORD
	.long	0x981000+AT_MEM_SPACE+MAPINITWORD
	.long	0x982000+AT_MEM_SPACE+MAPINITWORD
	.long	0x983000+AT_MEM_SPACE+MAPINITWORD
	.long	0x984000+AT_MEM_SPACE+MAPINITWORD
	.long	0x985000+AT_MEM_SPACE+MAPINITWORD
	.long	0x986000+AT_MEM_SPACE+MAPINITWORD
	.long	0x987000+AT_MEM_SPACE+MAPINITWORD
	.long	0x988000+AT_MEM_SPACE+MAPINITWORD
	.long	0x989000+AT_MEM_SPACE+MAPINITWORD
	.long	0x98a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x98b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x98c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x98d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x98e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x98f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x990000+AT_MEM_SPACE+MAPINITWORD
	.long	0x991000+AT_MEM_SPACE+MAPINITWORD
	.long	0x992000+AT_MEM_SPACE+MAPINITWORD
	.long	0x993000+AT_MEM_SPACE+MAPINITWORD
	.long	0x994000+AT_MEM_SPACE+MAPINITWORD
	.long	0x995000+AT_MEM_SPACE+MAPINITWORD
	.long	0x996000+AT_MEM_SPACE+MAPINITWORD
	.long	0x997000+AT_MEM_SPACE+MAPINITWORD
	.long	0x998000+AT_MEM_SPACE+MAPINITWORD
	.long	0x999000+AT_MEM_SPACE+MAPINITWORD
	.long	0x99a000+AT_MEM_SPACE+MAPINITWORD
	.long	0x99b000+AT_MEM_SPACE+MAPINITWORD
	.long	0x99c000+AT_MEM_SPACE+MAPINITWORD
	.long	0x99d000+AT_MEM_SPACE+MAPINITWORD
	.long	0x99e000+AT_MEM_SPACE+MAPINITWORD
	.long	0x99f000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9a0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9a1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9a2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9a3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9a4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9a5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9a6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9a7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9a8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9a9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9aa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9ab000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9ac000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9ad000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9ae000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9af000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9b0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9b1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9b2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9b3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9b4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9b5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9b6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9b7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9b8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9b9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9ba000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9bb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9bc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9bd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9be000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9bf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9c0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9c1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9c2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9c3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9c4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9c5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9c6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9c7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9c8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9c9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9ca000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9cb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9cc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9cd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9ce000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9cf000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9d0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9d1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9d2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9d3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9d4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9d5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9d6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9d7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9d8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9d9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9da000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9db000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9dc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9dd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9de000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9df000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9e0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9e1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9e2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9e3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9e4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9e5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9e6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9e7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9e8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9e9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9ea000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9eb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9ec000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9ed000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9ee000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9ef000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9f0000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9f1000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9f2000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9f3000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9f4000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9f5000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9f6000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9f7000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9f8000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9f9000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9fa000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9fb000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9fc000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9fd000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9fe000+AT_MEM_SPACE+MAPINITWORD
	.long	0x9ff000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa00000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa01000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa02000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa03000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa04000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa05000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa06000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa07000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa08000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa09000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa0a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa0b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa0c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa0d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa0e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa0f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa10000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa11000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa12000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa13000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa14000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa15000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa16000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa17000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa18000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa19000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa1a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa1b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa1c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa1d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa1e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa1f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa20000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa21000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa22000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa23000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa24000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa25000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa26000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa27000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa28000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa29000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa2a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa2b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa2c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa2d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa2e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa2f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa30000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa31000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa32000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa33000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa34000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa35000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa36000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa37000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa38000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa39000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa3a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa3b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa3c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa3d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa3e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa3f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa40000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa41000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa42000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa43000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa44000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa45000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa46000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa47000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa48000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa49000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa4a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa4b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa4c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa4d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa4e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa4f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa50000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa51000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa52000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa53000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa54000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa55000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa56000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa57000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa58000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa59000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa5a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa5b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa5c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa5d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa5e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa5f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa60000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa61000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa62000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa63000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa64000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa65000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa66000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa67000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa68000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa69000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa6a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa6b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa6c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa6d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa6e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa6f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa70000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa71000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa72000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa73000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa74000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa75000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa76000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa77000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa78000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa79000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa7a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa7b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa7c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa7d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa7e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa7f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa80000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa81000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa82000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa83000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa84000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa85000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa86000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa87000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa88000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa89000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa8a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa8b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa8c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa8d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa8e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa8f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa90000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa91000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa92000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa93000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa94000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa95000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa96000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa97000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa98000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa99000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa9a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa9b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa9c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa9d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa9e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xa9f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaa0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaa1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaa2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaa3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaa4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaa5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaa6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaa7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaa8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaa9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaaa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaab000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaac000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaad000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaae000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaaf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xab0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xab1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xab2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xab3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xab4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xab5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xab6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xab7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xab8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xab9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaba000+AT_MEM_SPACE+MAPINITWORD
	.long	0xabb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xabc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xabd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xabe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xabf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xac0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xac1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xac2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xac3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xac4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xac5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xac6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xac7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xac8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xac9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaca000+AT_MEM_SPACE+MAPINITWORD
	.long	0xacb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xacc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xacd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xace000+AT_MEM_SPACE+MAPINITWORD
	.long	0xacf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xad0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xad1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xad2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xad3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xad4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xad5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xad6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xad7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xad8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xad9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xada000+AT_MEM_SPACE+MAPINITWORD
	.long	0xadb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xadc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xadd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xade000+AT_MEM_SPACE+MAPINITWORD
	.long	0xadf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xae0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xae1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xae2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xae3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xae4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xae5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xae6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xae7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xae8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xae9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaea000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaeb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaec000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaed000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaee000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaef000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaf0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaf1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaf2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaf3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaf4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaf5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaf6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaf7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaf8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaf9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xafa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xafb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xafc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xafd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xafe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xaff000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb00000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb01000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb02000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb03000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb04000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb05000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb06000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb07000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb08000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb09000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb0a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb0b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb0c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb0d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb0e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb0f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb10000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb11000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb12000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb13000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb14000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb15000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb16000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb17000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb18000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb19000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb1a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb1b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb1c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb1d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb1e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb1f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb20000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb21000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb22000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb23000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb24000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb25000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb26000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb27000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb28000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb29000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb2a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb2b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb2c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb2d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb2e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb2f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb30000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb31000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb32000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb33000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb34000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb35000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb36000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb37000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb38000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb39000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb3a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb3b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb3c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb3d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb3e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb3f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb40000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb41000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb42000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb43000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb44000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb45000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb46000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb47000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb48000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb49000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb4a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb4b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb4c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb4d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb4e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb4f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb50000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb51000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb52000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb53000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb54000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb55000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb56000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb57000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb58000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb59000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb5a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb5b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb5c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb5d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb5e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb5f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb60000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb61000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb62000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb63000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb64000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb65000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb66000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb67000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb68000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb69000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb6a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb6b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb6c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb6d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb6e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb6f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb70000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb71000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb72000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb73000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb74000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb75000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb76000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb77000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb78000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb79000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb7a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb7b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb7c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb7d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb7e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb7f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb80000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb81000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb82000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb83000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb84000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb85000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb86000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb87000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb88000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb89000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb8a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb8b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb8c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb8d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb8e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb8f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb90000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb91000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb92000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb93000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb94000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb95000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb96000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb97000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb98000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb99000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb9a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb9b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb9c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb9d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb9e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xb9f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xba0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xba1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xba2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xba3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xba4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xba5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xba6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xba7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xba8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xba9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbaa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbab000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbac000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbad000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbae000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbaf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbb0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbb1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbb2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbb3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbb4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbb5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbb6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbb7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbb8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbb9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbba000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbbb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbbc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbbd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbbe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbbf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbc0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbc1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbc2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbc3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbc4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbc5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbc6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbc7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbc8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbc9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbca000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbcb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbcc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbcd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbce000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbcf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbd0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbd1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbd2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbd3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbd4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbd5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbd6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbd7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbd8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbd9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbda000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbdb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbdc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbdd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbde000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbdf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbe0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbe1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbe2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbe3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbe4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbe5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbe6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbe7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbe8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbe9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbea000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbeb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbec000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbed000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbee000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbef000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbf0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbf1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbf2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbf3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbf4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbf5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbf6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbf7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbf8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbf9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbfa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbfb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbfc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbfd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbfe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xbff000+AT_MEM_SPACE+MAPINITWORD

# PAGE 7
# NOT AVAILABLE FROM LOGICAL MEMORY
# BUS-MASTER 2nd level page table entries
pagebm3:
	.long	0xc00000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc01000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc02000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc03000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc04000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc05000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc06000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc07000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc08000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc09000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc0a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc0b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc0c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc0d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc0e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc0f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc10000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc11000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc12000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc13000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc14000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc15000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc16000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc17000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc18000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc19000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc1a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc1b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc1c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc1d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc1e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc1f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc20000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc21000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc22000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc23000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc24000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc25000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc26000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc27000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc28000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc29000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc2a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc2b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc2c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc2d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc2e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc2f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc30000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc31000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc32000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc33000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc34000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc35000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc36000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc37000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc38000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc39000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc3a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc3b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc3c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc3d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc3e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc3f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc40000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc41000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc42000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc43000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc44000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc45000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc46000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc47000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc48000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc49000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc4a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc4b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc4c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc4d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc4e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc4f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc50000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc51000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc52000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc53000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc54000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc55000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc56000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc57000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc58000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc59000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc5a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc5b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc5c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc5d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc5e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc5f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc60000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc61000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc62000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc63000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc64000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc65000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc66000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc67000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc68000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc69000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc6a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc6b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc6c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc6d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc6e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc6f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc70000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc71000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc72000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc73000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc74000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc75000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc76000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc77000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc78000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc79000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc7a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc7b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc7c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc7d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc7e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc7f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc80000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc81000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc82000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc83000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc84000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc85000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc86000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc87000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc88000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc89000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc8a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc8b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc8c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc8d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc8e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc8f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc90000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc91000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc92000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc93000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc94000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc95000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc96000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc97000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc98000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc99000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc9a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc9b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc9c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc9d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc9e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xc9f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xca0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xca1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xca2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xca3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xca4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xca5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xca6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xca7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xca8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xca9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcaa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcab000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcac000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcad000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcae000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcaf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcb0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcb1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcb2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcb3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcb4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcb5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcb6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcb7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcb8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcb9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcba000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcbb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcbc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcbd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcbe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcbf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcc0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcc1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcc2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcc3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcc4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcc5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcc6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcc7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcc8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcc9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcca000+AT_MEM_SPACE+MAPINITWORD
	.long	0xccb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xccc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xccd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcce000+AT_MEM_SPACE+MAPINITWORD
	.long	0xccf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcd0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcd1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcd2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcd3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcd4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcd5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcd6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcd7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcd8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcd9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcda000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcdb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcdc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcdd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcde000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcdf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xce0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xce1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xce2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xce3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xce4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xce5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xce6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xce7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xce8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xce9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcea000+AT_MEM_SPACE+MAPINITWORD
	.long	0xceb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcec000+AT_MEM_SPACE+MAPINITWORD
	.long	0xced000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcee000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcef000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcf0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcf1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcf2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcf3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcf4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcf5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcf6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcf7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcf8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcf9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcfa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcfb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcfc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcfd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcfe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xcff000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd00000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd01000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd02000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd03000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd04000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd05000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd06000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd07000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd08000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd09000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd0a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd0b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd0c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd0d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd0e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd0f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd10000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd11000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd12000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd13000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd14000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd15000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd16000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd17000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd18000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd19000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd1a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd1b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd1c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd1d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd1e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd1f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd20000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd21000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd22000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd23000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd24000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd25000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd26000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd27000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd28000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd29000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd2a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd2b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd2c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd2d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd2e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd2f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd30000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd31000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd32000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd33000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd34000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd35000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd36000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd37000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd38000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd39000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd3a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd3b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd3c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd3d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd3e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd3f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd40000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd41000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd42000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd43000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd44000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd45000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd46000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd47000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd48000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd49000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd4a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd4b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd4c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd4d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd4e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd4f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd50000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd51000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd52000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd53000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd54000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd55000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd56000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd57000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd58000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd59000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd5a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd5b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd5c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd5d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd5e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd5f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd60000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd61000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd62000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd63000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd64000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd65000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd66000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd67000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd68000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd69000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd6a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd6b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd6c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd6d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd6e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd6f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd70000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd71000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd72000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd73000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd74000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd75000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd76000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd77000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd78000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd79000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd7a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd7b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd7c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd7d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd7e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd7f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd80000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd81000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd82000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd83000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd84000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd85000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd86000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd87000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd88000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd89000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd8a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd8b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd8c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd8d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd8e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd8f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd90000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd91000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd92000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd93000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd94000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd95000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd96000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd97000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd98000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd99000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd9a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd9b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd9c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd9d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd9e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xd9f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xda0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xda1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xda2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xda3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xda4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xda5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xda6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xda7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xda8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xda9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdaa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdab000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdac000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdad000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdae000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdaf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdb0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdb1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdb2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdb3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdb4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdb5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdb6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdb7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdb8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdb9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdba000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdbb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdbc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdbd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdbe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdbf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdc0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdc1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdc2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdc3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdc4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdc5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdc6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdc7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdc8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdc9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdca000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdcb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdcc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdcd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdce000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdcf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdd0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdd1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdd2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdd3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdd4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdd5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdd6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdd7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdd8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdd9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdda000+AT_MEM_SPACE+MAPINITWORD
	.long	0xddb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xddc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xddd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdde000+AT_MEM_SPACE+MAPINITWORD
	.long	0xddf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xde0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xde1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xde2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xde3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xde4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xde5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xde6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xde7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xde8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xde9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdea000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdeb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdec000+AT_MEM_SPACE+MAPINITWORD
	.long	0xded000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdee000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdef000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdf0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdf1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdf2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdf3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdf4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdf5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdf6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdf7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdf8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdf9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdfa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdfb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdfc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdfd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdfe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xdff000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe00000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe01000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe02000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe03000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe04000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe05000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe06000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe07000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe08000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe09000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe0a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe0b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe0c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe0d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe0e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe0f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe10000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe11000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe12000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe13000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe14000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe15000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe16000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe17000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe18000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe19000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe1a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe1b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe1c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe1d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe1e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe1f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe20000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe21000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe22000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe23000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe24000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe25000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe26000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe27000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe28000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe29000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe2a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe2b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe2c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe2d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe2e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe2f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe30000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe31000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe32000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe33000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe34000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe35000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe36000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe37000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe38000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe39000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe3a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe3b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe3c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe3d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe3e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe3f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe40000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe41000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe42000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe43000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe44000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe45000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe46000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe47000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe48000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe49000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe4a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe4b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe4c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe4d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe4e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe4f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe50000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe51000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe52000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe53000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe54000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe55000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe56000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe57000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe58000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe59000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe5a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe5b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe5c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe5d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe5e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe5f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe60000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe61000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe62000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe63000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe64000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe65000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe66000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe67000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe68000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe69000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe6a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe6b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe6c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe6d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe6e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe6f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe70000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe71000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe72000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe73000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe74000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe75000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe76000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe77000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe78000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe79000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe7a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe7b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe7c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe7d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe7e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe7f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe80000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe81000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe82000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe83000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe84000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe85000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe86000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe87000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe88000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe89000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe8a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe8b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe8c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe8d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe8e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe8f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe90000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe91000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe92000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe93000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe94000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe95000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe96000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe97000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe98000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe99000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe9a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe9b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe9c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe9d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe9e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xe9f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xea0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xea1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xea2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xea3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xea4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xea5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xea6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xea7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xea8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xea9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeaa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeab000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeac000+AT_MEM_SPACE+MAPINITWORD
	.long	0xead000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeae000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeaf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeb0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeb1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeb2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeb3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeb4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeb5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeb6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeb7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeb8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeb9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeba000+AT_MEM_SPACE+MAPINITWORD
	.long	0xebb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xebc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xebd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xebe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xebf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xec0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xec1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xec2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xec3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xec4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xec5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xec6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xec7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xec8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xec9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeca000+AT_MEM_SPACE+MAPINITWORD
	.long	0xecb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xecc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xecd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xece000+AT_MEM_SPACE+MAPINITWORD
	.long	0xecf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xed0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xed1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xed2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xed3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xed4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xed5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xed6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xed7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xed8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xed9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeda000+AT_MEM_SPACE+MAPINITWORD
	.long	0xedb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xedc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xedd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xede000+AT_MEM_SPACE+MAPINITWORD
	.long	0xedf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xee0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xee1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xee2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xee3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xee4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xee5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xee6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xee7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xee8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xee9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeea000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeeb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeec000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeed000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeee000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeef000+AT_MEM_SPACE+MAPINITWORD
	.long	0xef0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xef1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xef2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xef3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xef4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xef5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xef6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xef7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xef8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xef9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xefa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xefb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xefc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xefd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xefe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xeff000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf00000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf01000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf02000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf03000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf04000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf05000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf06000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf07000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf08000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf09000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf0a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf0b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf0c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf0d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf0e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf0f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf10000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf11000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf12000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf13000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf14000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf15000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf16000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf17000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf18000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf19000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf1a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf1b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf1c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf1d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf1e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf1f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf20000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf21000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf22000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf23000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf24000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf25000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf26000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf27000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf28000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf29000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf2a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf2b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf2c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf2d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf2e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf2f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf30000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf31000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf32000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf33000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf34000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf35000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf36000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf37000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf38000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf39000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf3a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf3b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf3c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf3d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf3e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf3f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf40000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf41000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf42000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf43000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf44000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf45000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf46000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf47000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf48000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf49000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf4a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf4b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf4c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf4d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf4e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf4f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf50000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf51000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf52000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf53000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf54000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf55000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf56000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf57000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf58000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf59000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf5a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf5b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf5c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf5d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf5e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf5f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf60000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf61000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf62000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf63000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf64000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf65000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf66000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf67000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf68000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf69000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf6a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf6b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf6c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf6d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf6e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf6f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf70000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf71000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf72000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf73000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf74000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf75000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf76000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf77000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf78000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf79000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf7a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf7b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf7c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf7d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf7e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf7f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf80000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf81000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf82000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf83000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf84000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf85000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf86000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf87000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf88000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf89000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf8a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf8b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf8c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf8d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf8e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf8f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf90000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf91000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf92000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf93000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf94000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf95000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf96000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf97000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf98000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf99000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf9a000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf9b000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf9c000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf9d000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf9e000+AT_MEM_SPACE+MAPINITWORD
	.long	0xf9f000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfa0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfa1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfa2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfa3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfa4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfa5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfa6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfa7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfa8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfa9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfaa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfab000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfac000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfad000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfae000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfaf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfb0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfb1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfb2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfb3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfb4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfb5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfb6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfb7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfb8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfb9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfba000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfbb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfbc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfbd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfbe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfbf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfc0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfc1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfc2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfc3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfc4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfc5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfc6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfc7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfc8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfc9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfca000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfcb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfcc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfcd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfce000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfcf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfd0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfd1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfd2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfd3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfd4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfd5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfd6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfd7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfd8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfd9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfda000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfdb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfdc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfdd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfde000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfdf000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfe0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfe1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfe2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfe3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfe4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfe5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfe6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfe7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfe8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfe9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfea000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfeb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfec000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfed000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfee000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfef000+AT_MEM_SPACE+MAPINITWORD
	.long	0xff0000+AT_MEM_SPACE+MAPINITWORD
	.long	0xff1000+AT_MEM_SPACE+MAPINITWORD
	.long	0xff2000+AT_MEM_SPACE+MAPINITWORD
	.long	0xff3000+AT_MEM_SPACE+MAPINITWORD
	.long	0xff4000+AT_MEM_SPACE+MAPINITWORD
	.long	0xff5000+AT_MEM_SPACE+MAPINITWORD
	.long	0xff6000+AT_MEM_SPACE+MAPINITWORD
	.long	0xff7000+AT_MEM_SPACE+MAPINITWORD
	.long	0xff8000+AT_MEM_SPACE+MAPINITWORD
	.long	0xff9000+AT_MEM_SPACE+MAPINITWORD
	.long	0xffa000+AT_MEM_SPACE+MAPINITWORD
	.long	0xffb000+AT_MEM_SPACE+MAPINITWORD
	.long	0xffc000+AT_MEM_SPACE+MAPINITWORD
	.long	0xffd000+AT_MEM_SPACE+MAPINITWORD
	.long	0xffe000+AT_MEM_SPACE+MAPINITWORD
	.long	0xfff000+AT_MEM_SPACE+MAPINITWORD

# PAGE 8
# 1st level page table entries
__segt:
	.long	0xb000+PG_NCA+PG_RW	# first segment valid
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	0x3000+PG_NCA+PG_RW	# PC BUS-MASTER I/O MEMORY (0xa0000000)
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	0x4000+PG_NCA+PG_RW	# PC BUS-MASTER MEMORY (0xb0000000)
	.long	0x5000+PG_NCA+PG_RW	# PC BUS-MASTER MEMORY (4-8MB)
	.long	0x6000+PG_NCA+PG_RW	# PC BUS-MASTER MEMORY (8-12MB)
	.long	0x7000+PG_NCA+PG_RW	# PC BUS-MASTER MEMORY (12-16MB)
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment
	.long	PG_PF			# invalid segment

# PAGE 9
# Hardware register page
	.align	0x1000
page9:
	.space	0x1000

# PAGE A
# rest of startup code
# We align the start of routines to a 4-byte boundary.  A 2-byte boundary
# would be enough for the architecture, but this insures that we will get
# two parcels on the first fetch.

	.align	4
done:	# break here (loc a000) on program completion
	movw	$-1,r0
	call	sp,__pause

# Initialize stack pointer.  We know we will be pushing a frame pointer
# and one stack argument, so if we leave the stack on an even word
# boundary, then the alignment will work out.

	.align	4
start:
	movw	$__temptos,sp	# use end of this page for temp top of stack
	andi	$0xfffffff8,sp	# align to an even word boundary
	call	sp,__memsize	# determine physical memory for top of stack
	loadw	__maxmem,r1	# leave room above stack for page tables > 4MB
	addi	$0x3fffff,r1	# add 4MB-1 to round up to next page boundary
	shli	$-10,r1	
	andw    $0xfffff000,r1	# mask off extraneous bits
	subw	r1,r0		# r0 has tos as returned by memsize
	storw	r0,__pagetable
	addi	$0x1000,r0	# skip first page, already allocated
	subw	r1,r0
	storw	r0,__dmapage
	movw	r0,sp		# use value from _memsize for top of stack
	andi	$0xfffffff8,sp	# align to an even word boundary

	movpw	ssw,r0		# get old SSW
	xorw	$SS_M,r0	# turn off mapping bit
	movwp	r0,ssw		# set new SSW

	movw	$PG_PCB+PG_RWE,r0
	pushw	r0,sp
	loadw	__maxmem,r0	# __maxmem set by __memsize() call above
	pushw	r0,sp
	movw	$0x400000,r0
	pushw	r0,sp
	loadw	__pagetable,r1	# map second level page tables above stack
	movw	$__segt,r0	#  except for first one which is in low mem
	call	sp,__mapit	# mapit(__segt,__pagetable,4MB,__maxmem,MASK)
	addw	$12,sp

	movw	$PG_OPNCA,r0
	pushw	r0,sp
	loadw	__maxmem,r0
	pushw	r0,sp
	movw	$0,r0
	pushw	r0,sp
	loadw	__dmapage,r1	# map dma tables up at top above stack
	movw	$0,r0
	call	sp,__mapit	# mapit(0,__dmapage,0,__maxmem,MASK)
	addw	$12,sp

	movpw	ssw,r0		# get old SSW
	orw	$SS_M,r0	# turn on mapping bit
	movwp	r0,ssw		# set new SSW

	movw	$0,fp		# initialize frame pointer
	pushw	fp,sp		# save fp for stack traces
	movw	sp,fp		# pointer to saved copy


# Clear bss area.
# Usually the kernel would do this for us, but no kernel here.

	movw	sp,r0		# clear till sp
	subw	$_edata,r0
	loada	_edata,r1	# get address of area
	movw	$0,r2		# get pattern - zero to clear memory
	initc			# clear memory

# bring pc back to life
	loadw	__dmapage,r0
	storw	r0,COMMPAGE+0x14
	movw	__maxmem,r0
	storw	r0,COMMPAGE+0x18
	movw	$MAGIC_PM300,r0
	storh	r0,COMMPAGE+0x10
	movw	$CLR_AT,r0
	storb	r0,CR0
	movw	$CLR_CP,r0
	storb	r0,CR0
	movw	$F_AT+SET,r0
	storb	r0,CR0

# call opus intialization routines
	call	sp,__opus

# call device init routine
	call	sp,__init

# Set up arguments to main()
	movw	_environ,r0
	pushw	r0,sp		# Push **envp on stack
	movw	__argv0,r1	# Get **argv
	movw	__argc,r0	#   and argc
	call	sp,_main	# Call main()
	addw	$4,sp		# Pop third arg off stack

# Here at _exit, either by call, or by falling through from return
# from main().  Either way, status of program is in r0 and stack
# is aligned on an odd word boundary.

_exit:
	# Put any desired cleanup here
__exit:
	b	done		# Just branch to 8000 where we will bpt
	.align	0x1000
__temptos:

# PAGE B
# 2nd level page table entries (0MB-4MB)
__mempte:
paget0:
	.long	0x0000+PG_CWT+PG_RWE	# Map hardware-wired pages
	.long	0x1000+PG_NCA+PG_RWE	#   to match what hardware will enforce
	.long	0x2000+PG_NCA+PG_RWE
	.long	0x3000+PG_NCA+PG_RWE	# (Bus-master I/O page table)
	.long	0x0000+PG_MMIO+PG_RWE	# Cache I/O page (B-M MEM page table)
	.long	0x1000+PG_MMIO+PG_RWE	# Page for I/O device registers (")
	.long	0x0000+PG_BSTR+PG_RE	# Bootstrap area                (")
	.long	0x1000+PG_BSTR+PG_RE	#				(")
	.long	0x8000+PG_NCA+PG_RW	# Map segment table
	.long	HPAGE+PG_NCA+PG_RWE	# Hardware register page
	.long	0xa000+PG_PCB+PG_RWE	# Map ordinary memory
	.long	0xb000+PG_PCB+PG_RWE	# 2nd-level page table (0-4MB)
	.long	0xc000+PG_PCB+PG_RWE	# memory page
	.long	0xd000+PG_PCB+PG_RWE	# memory page
	.long	0xe000+PG_PCB+PG_RWE	# memory page
	.long	0xf000+PG_PCB+PG_RWE	# memory page
	.long	0x10000+PG_PCB+PG_RWE	# memory page
	.long	0x11000+PG_PCB+PG_RWE	# memory page
	.long	0x12000+PG_PCB+PG_RWE	# memory page
	.long	0x13000+PG_PCB+PG_RWE	# memory page
	.long	0x14000+PG_PCB+PG_RWE	# memory page
	.long	0x15000+PG_PCB+PG_RWE	# memory page
	.long	0x16000+PG_PCB+PG_RWE	# memory page
	.long	0x17000+PG_PCB+PG_RWE	# memory page
	.long	0x18000+PG_PCB+PG_RWE	# memory page
	.long	0x19000+PG_PCB+PG_RWE	# memory page
	.long	0x1a000+PG_PCB+PG_RWE	# memory page
	.long	0x1b000+PG_PCB+PG_RWE	# memory page
	.long	0x1c000+PG_PCB+PG_RWE	# memory page
	.long	0x1d000+PG_PCB+PG_RWE	# memory page
	.long	0x1e000+PG_PCB+PG_RWE	# memory page
	.long	0x1f000+PG_PCB+PG_RWE	# memory page
	.long	0x20000+PG_PCB+PG_RWE	# memory page
	.long	0x21000+PG_PCB+PG_RWE	# memory page
	.long	0x22000+PG_PCB+PG_RWE	# memory page
	.long	0x23000+PG_PCB+PG_RWE	# memory page
	.long	0x24000+PG_PCB+PG_RWE	# memory page
	.long	0x25000+PG_PCB+PG_RWE	# memory page
	.long	0x26000+PG_PCB+PG_RWE	# memory page
	.long	0x27000+PG_PCB+PG_RWE	# memory page
	.long	0x28000+PG_PCB+PG_RWE	# memory page
	.long	0x29000+PG_PCB+PG_RWE	# memory page
	.long	0x2a000+PG_PCB+PG_RWE	# memory page
	.long	0x2b000+PG_PCB+PG_RWE	# memory page
	.long	0x2c000+PG_PCB+PG_RWE	# memory page
	.long	0x2d000+PG_PCB+PG_RWE	# memory page
	.long	0x2e000+PG_PCB+PG_RWE	# memory page
	.long	0x2f000+PG_PCB+PG_RWE	# memory page
	.long	0x30000+PG_PCB+PG_RWE	# memory page
	.long	0x31000+PG_PCB+PG_RWE	# memory page
	.long	0x32000+PG_PCB+PG_RWE	# memory page
	.long	0x33000+PG_PCB+PG_RWE	# memory page
	.long	0x34000+PG_PCB+PG_RWE	# memory page
	.long	0x35000+PG_PCB+PG_RWE	# memory page
	.long	0x36000+PG_PCB+PG_RWE	# memory page
	.long	0x37000+PG_PCB+PG_RWE	# memory page
	.long	0x38000+PG_PCB+PG_RWE	# memory page
	.long	0x39000+PG_PCB+PG_RWE	# memory page
	.long	0x3a000+PG_PCB+PG_RWE	# memory page
	.long	0x3b000+PG_PCB+PG_RWE	# memory page
	.long	0x3c000+PG_PCB+PG_RWE	# memory page
	.long	0x3d000+PG_PCB+PG_RWE	# memory page
	.long	0x3e000+PG_PCB+PG_RWE	# memory page
	.long	0x3f000+PG_PCB+PG_RWE	# memory page
	.long	0x40000+PG_PCB+PG_RWE	# memory page
	.long	0x41000+PG_PCB+PG_RWE	# memory page
	.long	0x42000+PG_PCB+PG_RWE	# memory page
	.long	0x43000+PG_PCB+PG_RWE	# memory page
	.long	0x44000+PG_PCB+PG_RWE	# memory page
	.long	0x45000+PG_PCB+PG_RWE	# memory page
	.long	0x46000+PG_PCB+PG_RWE	# memory page
	.long	0x47000+PG_PCB+PG_RWE	# memory page
	.long	0x48000+PG_PCB+PG_RWE	# memory page
	.long	0x49000+PG_PCB+PG_RWE	# memory page
	.long	0x4a000+PG_PCB+PG_RWE	# memory page
	.long	0x4b000+PG_PCB+PG_RWE	# memory page
	.long	0x4c000+PG_PCB+PG_RWE	# memory page
	.long	0x4d000+PG_PCB+PG_RWE	# memory page
	.long	0x4e000+PG_PCB+PG_RWE	# memory page
	.long	0x4f000+PG_PCB+PG_RWE	# memory page
	.long	0x50000+PG_PCB+PG_RWE	# memory page
	.long	0x51000+PG_PCB+PG_RWE	# memory page
	.long	0x52000+PG_PCB+PG_RWE	# memory page
	.long	0x53000+PG_PCB+PG_RWE	# memory page
	.long	0x54000+PG_PCB+PG_RWE	# memory page
	.long	0x55000+PG_PCB+PG_RWE	# memory page
	.long	0x56000+PG_PCB+PG_RWE	# memory page
	.long	0x57000+PG_PCB+PG_RWE	# memory page
	.long	0x58000+PG_PCB+PG_RWE	# memory page
	.long	0x59000+PG_PCB+PG_RWE	# memory page
	.long	0x5a000+PG_PCB+PG_RWE	# memory page
	.long	0x5b000+PG_PCB+PG_RWE	# memory page
	.long	0x5c000+PG_PCB+PG_RWE	# memory page
	.long	0x5d000+PG_PCB+PG_RWE	# memory page
	.long	0x5e000+PG_PCB+PG_RWE	# memory page
	.long	0x5f000+PG_PCB+PG_RWE	# memory page
	.long	0x60000+PG_PCB+PG_RWE	# memory page
	.long	0x61000+PG_PCB+PG_RWE	# memory page
	.long	0x62000+PG_PCB+PG_RWE	# memory page
	.long	0x63000+PG_PCB+PG_RWE	# memory page
	.long	0x64000+PG_PCB+PG_RWE	# memory page
	.long	0x65000+PG_PCB+PG_RWE	# memory page
	.long	0x66000+PG_PCB+PG_RWE	# memory page
	.long	0x67000+PG_PCB+PG_RWE	# memory page
	.long	0x68000+PG_PCB+PG_RWE	# memory page
	.long	0x69000+PG_PCB+PG_RWE	# memory page
	.long	0x6a000+PG_PCB+PG_RWE	# memory page
	.long	0x6b000+PG_PCB+PG_RWE	# memory page
	.long	0x6c000+PG_PCB+PG_RWE	# memory page
	.long	0x6d000+PG_PCB+PG_RWE	# memory page
	.long	0x6e000+PG_PCB+PG_RWE	# memory page
	.long	0x6f000+PG_PCB+PG_RWE	# memory page
	.long	0x70000+PG_PCB+PG_RWE	# memory page
	.long	0x71000+PG_PCB+PG_RWE	# memory page
	.long	0x72000+PG_PCB+PG_RWE	# memory page
	.long	0x73000+PG_PCB+PG_RWE	# memory page
	.long	0x74000+PG_PCB+PG_RWE	# memory page
	.long	0x75000+PG_PCB+PG_RWE	# memory page
	.long	0x76000+PG_PCB+PG_RWE	# memory page
	.long	0x77000+PG_PCB+PG_RWE	# memory page
	.long	0x78000+PG_PCB+PG_RWE	# memory page
	.long	0x79000+PG_PCB+PG_RWE	# memory page
	.long	0x7a000+PG_PCB+PG_RWE	# memory page
	.long	0x7b000+PG_PCB+PG_RWE	# memory page
	.long	0x7c000+PG_PCB+PG_RWE	# memory page
	.long	0x7d000+PG_PCB+PG_RWE	# memory page
	.long	0x7e000+PG_PCB+PG_RWE	# memory page
	.long	0x7f000+PG_PCB+PG_RWE	# memory page
	.long	0x80000+PG_PCB+PG_RWE	# memory page
	.long	0x81000+PG_PCB+PG_RWE	# memory page
	.long	0x82000+PG_PCB+PG_RWE	# memory page
	.long	0x83000+PG_PCB+PG_RWE	# memory page
	.long	0x84000+PG_PCB+PG_RWE	# memory page
	.long	0x85000+PG_PCB+PG_RWE	# memory page
	.long	0x86000+PG_PCB+PG_RWE	# memory page
	.long	0x87000+PG_PCB+PG_RWE	# memory page
	.long	0x88000+PG_PCB+PG_RWE	# memory page
	.long	0x89000+PG_PCB+PG_RWE	# memory page
	.long	0x8a000+PG_PCB+PG_RWE	# memory page
	.long	0x8b000+PG_PCB+PG_RWE	# memory page
	.long	0x8c000+PG_PCB+PG_RWE	# memory page
	.long	0x8d000+PG_PCB+PG_RWE	# memory page
	.long	0x8e000+PG_PCB+PG_RWE	# memory page
	.long	0x8f000+PG_PCB+PG_RWE	# memory page
	.long	0x90000+PG_PCB+PG_RWE	# memory page
	.long	0x91000+PG_PCB+PG_RWE	# memory page
	.long	0x92000+PG_PCB+PG_RWE	# memory page
	.long	0x93000+PG_PCB+PG_RWE	# memory page
	.long	0x94000+PG_PCB+PG_RWE	# memory page
	.long	0x95000+PG_PCB+PG_RWE	# memory page
	.long	0x96000+PG_PCB+PG_RWE	# memory page
	.long	0x97000+PG_PCB+PG_RWE	# memory page
	.long	0x98000+PG_PCB+PG_RWE	# memory page
	.long	0x99000+PG_PCB+PG_RWE	# memory page
	.long	0x9a000+PG_PCB+PG_RWE	# memory page
	.long	0x9b000+PG_PCB+PG_RWE	# memory page
	.long	0x9c000+PG_PCB+PG_RWE	# memory page
	.long	0x9d000+PG_PCB+PG_RWE	# memory page
	.long	0x9e000+PG_PCB+PG_RWE	# memory page
	.long	0x9f000+PG_PCB+PG_RWE	# memory page
	.long	0xa0000+PG_PCB+PG_RWE	# memory page
	.long	0xa1000+PG_PCB+PG_RWE	# memory page
	.long	0xa2000+PG_PCB+PG_RWE	# memory page
	.long	0xa3000+PG_PCB+PG_RWE	# memory page
	.long	0xa4000+PG_PCB+PG_RWE	# memory page
	.long	0xa5000+PG_PCB+PG_RWE	# memory page
	.long	0xa6000+PG_PCB+PG_RWE	# memory page
	.long	0xa7000+PG_PCB+PG_RWE	# memory page
	.long	0xa8000+PG_PCB+PG_RWE	# memory page
	.long	0xa9000+PG_PCB+PG_RWE	# memory page
	.long	0xaa000+PG_PCB+PG_RWE	# memory page
	.long	0xab000+PG_PCB+PG_RWE	# memory page
	.long	0xac000+PG_PCB+PG_RWE	# memory page
	.long	0xad000+PG_PCB+PG_RWE	# memory page
	.long	0xae000+PG_PCB+PG_RWE	# memory page
	.long	0xaf000+PG_PCB+PG_RWE	# memory page
	.long	0xb0000+PG_PCB+PG_RWE	# memory page
	.long	0xb1000+PG_PCB+PG_RWE	# memory page
	.long	0xb2000+PG_PCB+PG_RWE	# memory page
	.long	0xb3000+PG_PCB+PG_RWE	# memory page
	.long	0xb4000+PG_PCB+PG_RWE	# memory page
	.long	0xb5000+PG_PCB+PG_RWE	# memory page
	.long	0xb6000+PG_PCB+PG_RWE	# memory page
	.long	0xb7000+PG_PCB+PG_RWE	# memory page
	.long	0xb8000+PG_PCB+PG_RWE	# memory page
	.long	0xb9000+PG_PCB+PG_RWE	# memory page
	.long	0xba000+PG_PCB+PG_RWE	# memory page
	.long	0xbb000+PG_PCB+PG_RWE	# memory page
	.long	0xbc000+PG_PCB+PG_RWE	# memory page
	.long	0xbd000+PG_PCB+PG_RWE	# memory page
	.long	0xbe000+PG_PCB+PG_RWE	# memory page
	.long	0xbf000+PG_PCB+PG_RWE	# memory page
	.long	0xc0000+PG_PCB+PG_RWE	# memory page
	.long	0xc1000+PG_PCB+PG_RWE	# memory page
	.long	0xc2000+PG_PCB+PG_RWE	# memory page
	.long	0xc3000+PG_PCB+PG_RWE	# memory page
	.long	0xc4000+PG_PCB+PG_RWE	# memory page
	.long	0xc5000+PG_PCB+PG_RWE	# memory page
	.long	0xc6000+PG_PCB+PG_RWE	# memory page
	.long	0xc7000+PG_PCB+PG_RWE	# memory page
	.long	0xc8000+PG_PCB+PG_RWE	# memory page
	.long	0xc9000+PG_PCB+PG_RWE	# memory page
	.long	0xca000+PG_PCB+PG_RWE	# memory page
	.long	0xcb000+PG_PCB+PG_RWE	# memory page
	.long	0xcc000+PG_PCB+PG_RWE	# memory page
	.long	0xcd000+PG_PCB+PG_RWE	# memory page
	.long	0xce000+PG_PCB+PG_RWE	# memory page
	.long	0xcf000+PG_PCB+PG_RWE	# memory page
	.long	0xd0000+PG_PCB+PG_RWE	# memory page
	.long	0xd1000+PG_PCB+PG_RWE	# memory page
	.long	0xd2000+PG_PCB+PG_RWE	# memory page
	.long	0xd3000+PG_PCB+PG_RWE	# memory page
	.long	0xd4000+PG_PCB+PG_RWE	# memory page
	.long	0xd5000+PG_PCB+PG_RWE	# memory page
	.long	0xd6000+PG_PCB+PG_RWE	# memory page
	.long	0xd7000+PG_PCB+PG_RWE	# memory page
	.long	0xd8000+PG_PCB+PG_RWE	# memory page
	.long	0xd9000+PG_PCB+PG_RWE	# memory page
	.long	0xda000+PG_PCB+PG_RWE	# memory page
	.long	0xdb000+PG_PCB+PG_RWE	# memory page
	.long	0xdc000+PG_PCB+PG_RWE	# memory page
	.long	0xdd000+PG_PCB+PG_RWE	# memory page
	.long	0xde000+PG_PCB+PG_RWE	# memory page
	.long	0xdf000+PG_PCB+PG_RWE	# memory page
	.long	0xe0000+PG_PCB+PG_RWE	# memory page
	.long	0xe1000+PG_PCB+PG_RWE	# memory page
	.long	0xe2000+PG_PCB+PG_RWE	# memory page
	.long	0xe3000+PG_PCB+PG_RWE	# memory page
	.long	0xe4000+PG_PCB+PG_RWE	# memory page
	.long	0xe5000+PG_PCB+PG_RWE	# memory page
	.long	0xe6000+PG_PCB+PG_RWE	# memory page
	.long	0xe7000+PG_PCB+PG_RWE	# memory page
	.long	0xe8000+PG_PCB+PG_RWE	# memory page
	.long	0xe9000+PG_PCB+PG_RWE	# memory page
	.long	0xea000+PG_PCB+PG_RWE	# memory page
	.long	0xeb000+PG_PCB+PG_RWE	# memory page
	.long	0xec000+PG_PCB+PG_RWE	# memory page
	.long	0xed000+PG_PCB+PG_RWE	# memory page
	.long	0xee000+PG_PCB+PG_RWE	# memory page
	.long	0xef000+PG_PCB+PG_RWE	# memory page
	.long	0xf0000+PG_PCB+PG_RWE	# memory page
	.long	0xf1000+PG_PCB+PG_RWE	# memory page
	.long	0xf2000+PG_PCB+PG_RWE	# memory page
	.long	0xf3000+PG_PCB+PG_RWE	# memory page
	.long	0xf4000+PG_PCB+PG_RWE	# memory page
	.long	0xf5000+PG_PCB+PG_RWE	# memory page
	.long	0xf6000+PG_PCB+PG_RWE	# memory page
	.long	0xf7000+PG_PCB+PG_RWE	# memory page
	.long	0xf8000+PG_PCB+PG_RWE	# memory page
	.long	0xf9000+PG_PCB+PG_RWE	# memory page
	.long	0xfa000+PG_PCB+PG_RWE	# memory page
	.long	0xfb000+PG_PCB+PG_RWE	# memory page
	.long	0xfc000+PG_PCB+PG_RWE	# memory page
	.long	0xfd000+PG_PCB+PG_RWE	# memory page
	.long	0xfe000+PG_PCB+PG_RWE	# memory page
	.long	0xff000+PG_PCB+PG_RWE	# memory page
	.long	0x100000+PG_PCB+PG_RWE	# memory page
	.long	0x101000+PG_PCB+PG_RWE	# memory page
	.long	0x102000+PG_PCB+PG_RWE	# memory page
	.long	0x103000+PG_PCB+PG_RWE	# memory page
	.long	0x104000+PG_PCB+PG_RWE	# memory page
	.long	0x105000+PG_PCB+PG_RWE	# memory page
	.long	0x106000+PG_PCB+PG_RWE	# memory page
	.long	0x107000+PG_PCB+PG_RWE	# memory page
	.long	0x108000+PG_PCB+PG_RWE	# memory page
	.long	0x109000+PG_PCB+PG_RWE	# memory page
	.long	0x10a000+PG_PCB+PG_RWE	# memory page
	.long	0x10b000+PG_PCB+PG_RWE	# memory page
	.long	0x10c000+PG_PCB+PG_RWE	# memory page
	.long	0x10d000+PG_PCB+PG_RWE	# memory page
	.long	0x10e000+PG_PCB+PG_RWE	# memory page
	.long	0x10f000+PG_PCB+PG_RWE	# memory page
	.long	0x110000+PG_PCB+PG_RWE	# memory page
	.long	0x111000+PG_PCB+PG_RWE	# memory page
	.long	0x112000+PG_PCB+PG_RWE	# memory page
	.long	0x113000+PG_PCB+PG_RWE	# memory page
	.long	0x114000+PG_PCB+PG_RWE	# memory page
	.long	0x115000+PG_PCB+PG_RWE	# memory page
	.long	0x116000+PG_PCB+PG_RWE	# memory page
	.long	0x117000+PG_PCB+PG_RWE	# memory page
	.long	0x118000+PG_PCB+PG_RWE	# memory page
	.long	0x119000+PG_PCB+PG_RWE	# memory page
	.long	0x11a000+PG_PCB+PG_RWE	# memory page
	.long	0x11b000+PG_PCB+PG_RWE	# memory page
	.long	0x11c000+PG_PCB+PG_RWE	# memory page
	.long	0x11d000+PG_PCB+PG_RWE	# memory page
	.long	0x11e000+PG_PCB+PG_RWE	# memory page
	.long	0x11f000+PG_PCB+PG_RWE	# memory page
	.long	0x120000+PG_PCB+PG_RWE	# memory page
	.long	0x121000+PG_PCB+PG_RWE	# memory page
	.long	0x122000+PG_PCB+PG_RWE	# memory page
	.long	0x123000+PG_PCB+PG_RWE	# memory page
	.long	0x124000+PG_PCB+PG_RWE	# memory page
	.long	0x125000+PG_PCB+PG_RWE	# memory page
	.long	0x126000+PG_PCB+PG_RWE	# memory page
	.long	0x127000+PG_PCB+PG_RWE	# memory page
	.long	0x128000+PG_PCB+PG_RWE	# memory page
	.long	0x129000+PG_PCB+PG_RWE	# memory page
	.long	0x12a000+PG_PCB+PG_RWE	# memory page
	.long	0x12b000+PG_PCB+PG_RWE	# memory page
	.long	0x12c000+PG_PCB+PG_RWE	# memory page
	.long	0x12d000+PG_PCB+PG_RWE	# memory page
	.long	0x12e000+PG_PCB+PG_RWE	# memory page
	.long	0x12f000+PG_PCB+PG_RWE	# memory page
	.long	0x130000+PG_PCB+PG_RWE	# memory page
	.long	0x131000+PG_PCB+PG_RWE	# memory page
	.long	0x132000+PG_PCB+PG_RWE	# memory page
	.long	0x133000+PG_PCB+PG_RWE	# memory page
	.long	0x134000+PG_PCB+PG_RWE	# memory page
	.long	0x135000+PG_PCB+PG_RWE	# memory page
	.long	0x136000+PG_PCB+PG_RWE	# memory page
	.long	0x137000+PG_PCB+PG_RWE	# memory page
	.long	0x138000+PG_PCB+PG_RWE	# memory page
	.long	0x139000+PG_PCB+PG_RWE	# memory page
	.long	0x13a000+PG_PCB+PG_RWE	# memory page
	.long	0x13b000+PG_PCB+PG_RWE	# memory page
	.long	0x13c000+PG_PCB+PG_RWE	# memory page
	.long	0x13d000+PG_PCB+PG_RWE	# memory page
	.long	0x13e000+PG_PCB+PG_RWE	# memory page
	.long	0x13f000+PG_PCB+PG_RWE	# memory page
	.long	0x140000+PG_PCB+PG_RWE	# memory page
	.long	0x141000+PG_PCB+PG_RWE	# memory page
	.long	0x142000+PG_PCB+PG_RWE	# memory page
	.long	0x143000+PG_PCB+PG_RWE	# memory page
	.long	0x144000+PG_PCB+PG_RWE	# memory page
	.long	0x145000+PG_PCB+PG_RWE	# memory page
	.long	0x146000+PG_PCB+PG_RWE	# memory page
	.long	0x147000+PG_PCB+PG_RWE	# memory page
	.long	0x148000+PG_PCB+PG_RWE	# memory page
	.long	0x149000+PG_PCB+PG_RWE	# memory page
	.long	0x14a000+PG_PCB+PG_RWE	# memory page
	.long	0x14b000+PG_PCB+PG_RWE	# memory page
	.long	0x14c000+PG_PCB+PG_RWE	# memory page
	.long	0x14d000+PG_PCB+PG_RWE	# memory page
	.long	0x14e000+PG_PCB+PG_RWE	# memory page
	.long	0x14f000+PG_PCB+PG_RWE	# memory page
	.long	0x150000+PG_PCB+PG_RWE	# memory page
	.long	0x151000+PG_PCB+PG_RWE	# memory page
	.long	0x152000+PG_PCB+PG_RWE	# memory page
	.long	0x153000+PG_PCB+PG_RWE	# memory page
	.long	0x154000+PG_PCB+PG_RWE	# memory page
	.long	0x155000+PG_PCB+PG_RWE	# memory page
	.long	0x156000+PG_PCB+PG_RWE	# memory page
	.long	0x157000+PG_PCB+PG_RWE	# memory page
	.long	0x158000+PG_PCB+PG_RWE	# memory page
	.long	0x159000+PG_PCB+PG_RWE	# memory page
	.long	0x15a000+PG_PCB+PG_RWE	# memory page
	.long	0x15b000+PG_PCB+PG_RWE	# memory page
	.long	0x15c000+PG_PCB+PG_RWE	# memory page
	.long	0x15d000+PG_PCB+PG_RWE	# memory page
	.long	0x15e000+PG_PCB+PG_RWE	# memory page
	.long	0x15f000+PG_PCB+PG_RWE	# memory page
	.long	0x160000+PG_PCB+PG_RWE	# memory page
	.long	0x161000+PG_PCB+PG_RWE	# memory page
	.long	0x162000+PG_PCB+PG_RWE	# memory page
	.long	0x163000+PG_PCB+PG_RWE	# memory page
	.long	0x164000+PG_PCB+PG_RWE	# memory page
	.long	0x165000+PG_PCB+PG_RWE	# memory page
	.long	0x166000+PG_PCB+PG_RWE	# memory page
	.long	0x167000+PG_PCB+PG_RWE	# memory page
	.long	0x168000+PG_PCB+PG_RWE	# memory page
	.long	0x169000+PG_PCB+PG_RWE	# memory page
	.long	0x16a000+PG_PCB+PG_RWE	# memory page
	.long	0x16b000+PG_PCB+PG_RWE	# memory page
	.long	0x16c000+PG_PCB+PG_RWE	# memory page
	.long	0x16d000+PG_PCB+PG_RWE	# memory page
	.long	0x16e000+PG_PCB+PG_RWE	# memory page
	.long	0x16f000+PG_PCB+PG_RWE	# memory page
	.long	0x170000+PG_PCB+PG_RWE	# memory page
	.long	0x171000+PG_PCB+PG_RWE	# memory page
	.long	0x172000+PG_PCB+PG_RWE	# memory page
	.long	0x173000+PG_PCB+PG_RWE	# memory page
	.long	0x174000+PG_PCB+PG_RWE	# memory page
	.long	0x175000+PG_PCB+PG_RWE	# memory page
	.long	0x176000+PG_PCB+PG_RWE	# memory page
	.long	0x177000+PG_PCB+PG_RWE	# memory page
	.long	0x178000+PG_PCB+PG_RWE	# memory page
	.long	0x179000+PG_PCB+PG_RWE	# memory page
	.long	0x17a000+PG_PCB+PG_RWE	# memory page
	.long	0x17b000+PG_PCB+PG_RWE	# memory page
	.long	0x17c000+PG_PCB+PG_RWE	# memory page
	.long	0x17d000+PG_PCB+PG_RWE	# memory page
	.long	0x17e000+PG_PCB+PG_RWE	# memory page
	.long	0x17f000+PG_PCB+PG_RWE	# memory page
	.long	0x180000+PG_PCB+PG_RWE	# memory page
	.long	0x181000+PG_PCB+PG_RWE	# memory page
	.long	0x182000+PG_PCB+PG_RWE	# memory page
	.long	0x183000+PG_PCB+PG_RWE	# memory page
	.long	0x184000+PG_PCB+PG_RWE	# memory page
	.long	0x185000+PG_PCB+PG_RWE	# memory page
	.long	0x186000+PG_PCB+PG_RWE	# memory page
	.long	0x187000+PG_PCB+PG_RWE	# memory page
	.long	0x188000+PG_PCB+PG_RWE	# memory page
	.long	0x189000+PG_PCB+PG_RWE	# memory page
	.long	0x18a000+PG_PCB+PG_RWE	# memory page
	.long	0x18b000+PG_PCB+PG_RWE	# memory page
	.long	0x18c000+PG_PCB+PG_RWE	# memory page
	.long	0x18d000+PG_PCB+PG_RWE	# memory page
	.long	0x18e000+PG_PCB+PG_RWE	# memory page
	.long	0x18f000+PG_PCB+PG_RWE	# memory page
	.long	0x190000+PG_PCB+PG_RWE	# memory page
	.long	0x191000+PG_PCB+PG_RWE	# memory page
	.long	0x192000+PG_PCB+PG_RWE	# memory page
	.long	0x193000+PG_PCB+PG_RWE	# memory page
	.long	0x194000+PG_PCB+PG_RWE	# memory page
	.long	0x195000+PG_PCB+PG_RWE	# memory page
	.long	0x196000+PG_PCB+PG_RWE	# memory page
	.long	0x197000+PG_PCB+PG_RWE	# memory page
	.long	0x198000+PG_PCB+PG_RWE	# memory page
	.long	0x199000+PG_PCB+PG_RWE	# memory page
	.long	0x19a000+PG_PCB+PG_RWE	# memory page
	.long	0x19b000+PG_PCB+PG_RWE	# memory page
	.long	0x19c000+PG_PCB+PG_RWE	# memory page
	.long	0x19d000+PG_PCB+PG_RWE	# memory page
	.long	0x19e000+PG_PCB+PG_RWE	# memory page
	.long	0x19f000+PG_PCB+PG_RWE	# memory page
	.long	0x1a0000+PG_PCB+PG_RWE	# memory page
	.long	0x1a1000+PG_PCB+PG_RWE	# memory page
	.long	0x1a2000+PG_PCB+PG_RWE	# memory page
	.long	0x1a3000+PG_PCB+PG_RWE	# memory page
	.long	0x1a4000+PG_PCB+PG_RWE	# memory page
	.long	0x1a5000+PG_PCB+PG_RWE	# memory page
	.long	0x1a6000+PG_PCB+PG_RWE	# memory page
	.long	0x1a7000+PG_PCB+PG_RWE	# memory page
	.long	0x1a8000+PG_PCB+PG_RWE	# memory page
	.long	0x1a9000+PG_PCB+PG_RWE	# memory page
	.long	0x1aa000+PG_PCB+PG_RWE	# memory page
	.long	0x1ab000+PG_PCB+PG_RWE	# memory page
	.long	0x1ac000+PG_PCB+PG_RWE	# memory page
	.long	0x1ad000+PG_PCB+PG_RWE	# memory page
	.long	0x1ae000+PG_PCB+PG_RWE	# memory page
	.long	0x1af000+PG_PCB+PG_RWE	# memory page
	.long	0x1b0000+PG_PCB+PG_RWE	# memory page
	.long	0x1b1000+PG_PCB+PG_RWE	# memory page
	.long	0x1b2000+PG_PCB+PG_RWE	# memory page
	.long	0x1b3000+PG_PCB+PG_RWE	# memory page
	.long	0x1b4000+PG_PCB+PG_RWE	# memory page
	.long	0x1b5000+PG_PCB+PG_RWE	# memory page
	.long	0x1b6000+PG_PCB+PG_RWE	# memory page
	.long	0x1b7000+PG_PCB+PG_RWE	# memory page
	.long	0x1b8000+PG_PCB+PG_RWE	# memory page
	.long	0x1b9000+PG_PCB+PG_RWE	# memory page
	.long	0x1ba000+PG_PCB+PG_RWE	# memory page
	.long	0x1bb000+PG_PCB+PG_RWE	# memory page
	.long	0x1bc000+PG_PCB+PG_RWE	# memory page
	.long	0x1bd000+PG_PCB+PG_RWE	# memory page
	.long	0x1be000+PG_PCB+PG_RWE	# memory page
	.long	0x1bf000+PG_PCB+PG_RWE	# memory page
	.long	0x1c0000+PG_PCB+PG_RWE	# memory page
	.long	0x1c1000+PG_PCB+PG_RWE	# memory page
	.long	0x1c2000+PG_PCB+PG_RWE	# memory page
	.long	0x1c3000+PG_PCB+PG_RWE	# memory page
	.long	0x1c4000+PG_PCB+PG_RWE	# memory page
	.long	0x1c5000+PG_PCB+PG_RWE	# memory page
	.long	0x1c6000+PG_PCB+PG_RWE	# memory page
	.long	0x1c7000+PG_PCB+PG_RWE	# memory page
	.long	0x1c8000+PG_PCB+PG_RWE	# memory page
	.long	0x1c9000+PG_PCB+PG_RWE	# memory page
	.long	0x1ca000+PG_PCB+PG_RWE	# memory page
	.long	0x1cb000+PG_PCB+PG_RWE	# memory page
	.long	0x1cc000+PG_PCB+PG_RWE	# memory page
	.long	0x1cd000+PG_PCB+PG_RWE	# memory page
	.long	0x1ce000+PG_PCB+PG_RWE	# memory page
	.long	0x1cf000+PG_PCB+PG_RWE	# memory page
	.long	0x1d0000+PG_PCB+PG_RWE	# memory page
	.long	0x1d1000+PG_PCB+PG_RWE	# memory page
	.long	0x1d2000+PG_PCB+PG_RWE	# memory page
	.long	0x1d3000+PG_PCB+PG_RWE	# memory page
	.long	0x1d4000+PG_PCB+PG_RWE	# memory page
	.long	0x1d5000+PG_PCB+PG_RWE	# memory page
	.long	0x1d6000+PG_PCB+PG_RWE	# memory page
	.long	0x1d7000+PG_PCB+PG_RWE	# memory page
	.long	0x1d8000+PG_PCB+PG_RWE	# memory page
	.long	0x1d9000+PG_PCB+PG_RWE	# memory page
	.long	0x1da000+PG_PCB+PG_RWE	# memory page
	.long	0x1db000+PG_PCB+PG_RWE	# memory page
	.long	0x1dc000+PG_PCB+PG_RWE	# memory page
	.long	0x1dd000+PG_PCB+PG_RWE	# memory page
	.long	0x1de000+PG_PCB+PG_RWE	# memory page
	.long	0x1df000+PG_PCB+PG_RWE	# memory page
	.long	0x1e0000+PG_PCB+PG_RWE	# memory page
	.long	0x1e1000+PG_PCB+PG_RWE	# memory page
	.long	0x1e2000+PG_PCB+PG_RWE	# memory page
	.long	0x1e3000+PG_PCB+PG_RWE	# memory page
	.long	0x1e4000+PG_PCB+PG_RWE	# memory page
	.long	0x1e5000+PG_PCB+PG_RWE	# memory page
	.long	0x1e6000+PG_PCB+PG_RWE	# memory page
	.long	0x1e7000+PG_PCB+PG_RWE	# memory page
	.long	0x1e8000+PG_PCB+PG_RWE	# memory page
	.long	0x1e9000+PG_PCB+PG_RWE	# memory page
	.long	0x1ea000+PG_PCB+PG_RWE	# memory page
	.long	0x1eb000+PG_PCB+PG_RWE	# memory page
	.long	0x1ec000+PG_PCB+PG_RWE	# memory page
	.long	0x1ed000+PG_PCB+PG_RWE	# memory page
	.long	0x1ee000+PG_PCB+PG_RWE	# memory page
	.long	0x1ef000+PG_PCB+PG_RWE	# memory page
	.long	0x1f0000+PG_PCB+PG_RWE	# memory page
	.long	0x1f1000+PG_PCB+PG_RWE	# memory page
	.long	0x1f2000+PG_PCB+PG_RWE	# memory page
	.long	0x1f3000+PG_PCB+PG_RWE	# memory page
	.long	0x1f4000+PG_PCB+PG_RWE	# memory page
	.long	0x1f5000+PG_PCB+PG_RWE	# memory page
	.long	0x1f6000+PG_PCB+PG_RWE	# memory page
	.long	0x1f7000+PG_PCB+PG_RWE	# memory page
	.long	0x1f8000+PG_PCB+PG_RWE	# memory page
	.long	0x1f9000+PG_PCB+PG_RWE	# memory page
	.long	0x1fa000+PG_PCB+PG_RWE	# memory page
	.long	0x1fb000+PG_PCB+PG_RWE	# memory page
	.long	0x1fc000+PG_PCB+PG_RWE	# memory page
	.long	0x1fd000+PG_PCB+PG_RWE	# memory page
	.long	0x1fe000+PG_PCB+PG_RWE	# memory page
	.long	0x1ff000+PG_PCB+PG_RWE	# memory page
	.long	0x200000+PG_PCB+PG_RWE	# memory page
	.long	0x201000+PG_PCB+PG_RWE	# memory page
	.long	0x202000+PG_PCB+PG_RWE	# memory page
	.long	0x203000+PG_PCB+PG_RWE	# memory page
	.long	0x204000+PG_PCB+PG_RWE	# memory page
	.long	0x205000+PG_PCB+PG_RWE	# memory page
	.long	0x206000+PG_PCB+PG_RWE	# memory page
	.long	0x207000+PG_PCB+PG_RWE	# memory page
	.long	0x208000+PG_PCB+PG_RWE	# memory page
	.long	0x209000+PG_PCB+PG_RWE	# memory page
	.long	0x20a000+PG_PCB+PG_RWE	# memory page
	.long	0x20b000+PG_PCB+PG_RWE	# memory page
	.long	0x20c000+PG_PCB+PG_RWE	# memory page
	.long	0x20d000+PG_PCB+PG_RWE	# memory page
	.long	0x20e000+PG_PCB+PG_RWE	# memory page
	.long	0x20f000+PG_PCB+PG_RWE	# memory page
	.long	0x210000+PG_PCB+PG_RWE	# memory page
	.long	0x211000+PG_PCB+PG_RWE	# memory page
	.long	0x212000+PG_PCB+PG_RWE	# memory page
	.long	0x213000+PG_PCB+PG_RWE	# memory page
	.long	0x214000+PG_PCB+PG_RWE	# memory page
	.long	0x215000+PG_PCB+PG_RWE	# memory page
	.long	0x216000+PG_PCB+PG_RWE	# memory page
	.long	0x217000+PG_PCB+PG_RWE	# memory page
	.long	0x218000+PG_PCB+PG_RWE	# memory page
	.long	0x219000+PG_PCB+PG_RWE	# memory page
	.long	0x21a000+PG_PCB+PG_RWE	# memory page
	.long	0x21b000+PG_PCB+PG_RWE	# memory page
	.long	0x21c000+PG_PCB+PG_RWE	# memory page
	.long	0x21d000+PG_PCB+PG_RWE	# memory page
	.long	0x21e000+PG_PCB+PG_RWE	# memory page
	.long	0x21f000+PG_PCB+PG_RWE	# memory page
	.long	0x220000+PG_PCB+PG_RWE	# memory page
	.long	0x221000+PG_PCB+PG_RWE	# memory page
	.long	0x222000+PG_PCB+PG_RWE	# memory page
	.long	0x223000+PG_PCB+PG_RWE	# memory page
	.long	0x224000+PG_PCB+PG_RWE	# memory page
	.long	0x225000+PG_PCB+PG_RWE	# memory page
	.long	0x226000+PG_PCB+PG_RWE	# memory page
	.long	0x227000+PG_PCB+PG_RWE	# memory page
	.long	0x228000+PG_PCB+PG_RWE	# memory page
	.long	0x229000+PG_PCB+PG_RWE	# memory page
	.long	0x22a000+PG_PCB+PG_RWE	# memory page
	.long	0x22b000+PG_PCB+PG_RWE	# memory page
	.long	0x22c000+PG_PCB+PG_RWE	# memory page
	.long	0x22d000+PG_PCB+PG_RWE	# memory page
	.long	0x22e000+PG_PCB+PG_RWE	# memory page
	.long	0x22f000+PG_PCB+PG_RWE	# memory page
	.long	0x230000+PG_PCB+PG_RWE	# memory page
	.long	0x231000+PG_PCB+PG_RWE	# memory page
	.long	0x232000+PG_PCB+PG_RWE	# memory page
	.long	0x233000+PG_PCB+PG_RWE	# memory page
	.long	0x234000+PG_PCB+PG_RWE	# memory page
	.long	0x235000+PG_PCB+PG_RWE	# memory page
	.long	0x236000+PG_PCB+PG_RWE	# memory page
	.long	0x237000+PG_PCB+PG_RWE	# memory page
	.long	0x238000+PG_PCB+PG_RWE	# memory page
	.long	0x239000+PG_PCB+PG_RWE	# memory page
	.long	0x23a000+PG_PCB+PG_RWE	# memory page
	.long	0x23b000+PG_PCB+PG_RWE	# memory page
	.long	0x23c000+PG_PCB+PG_RWE	# memory page
	.long	0x23d000+PG_PCB+PG_RWE	# memory page
	.long	0x23e000+PG_PCB+PG_RWE	# memory page
	.long	0x23f000+PG_PCB+PG_RWE	# memory page
	.long	0x240000+PG_PCB+PG_RWE	# memory page
	.long	0x241000+PG_PCB+PG_RWE	# memory page
	.long	0x242000+PG_PCB+PG_RWE	# memory page
	.long	0x243000+PG_PCB+PG_RWE	# memory page
	.long	0x244000+PG_PCB+PG_RWE	# memory page
	.long	0x245000+PG_PCB+PG_RWE	# memory page
	.long	0x246000+PG_PCB+PG_RWE	# memory page
	.long	0x247000+PG_PCB+PG_RWE	# memory page
	.long	0x248000+PG_PCB+PG_RWE	# memory page
	.long	0x249000+PG_PCB+PG_RWE	# memory page
	.long	0x24a000+PG_PCB+PG_RWE	# memory page
	.long	0x24b000+PG_PCB+PG_RWE	# memory page
	.long	0x24c000+PG_PCB+PG_RWE	# memory page
	.long	0x24d000+PG_PCB+PG_RWE	# memory page
	.long	0x24e000+PG_PCB+PG_RWE	# memory page
	.long	0x24f000+PG_PCB+PG_RWE	# memory page
	.long	0x250000+PG_PCB+PG_RWE	# memory page
	.long	0x251000+PG_PCB+PG_RWE	# memory page
	.long	0x252000+PG_PCB+PG_RWE	# memory page
	.long	0x253000+PG_PCB+PG_RWE	# memory page
	.long	0x254000+PG_PCB+PG_RWE	# memory page
	.long	0x255000+PG_PCB+PG_RWE	# memory page
	.long	0x256000+PG_PCB+PG_RWE	# memory page
	.long	0x257000+PG_PCB+PG_RWE	# memory page
	.long	0x258000+PG_PCB+PG_RWE	# memory page
	.long	0x259000+PG_PCB+PG_RWE	# memory page
	.long	0x25a000+PG_PCB+PG_RWE	# memory page
	.long	0x25b000+PG_PCB+PG_RWE	# memory page
	.long	0x25c000+PG_PCB+PG_RWE	# memory page
	.long	0x25d000+PG_PCB+PG_RWE	# memory page
	.long	0x25e000+PG_PCB+PG_RWE	# memory page
	.long	0x25f000+PG_PCB+PG_RWE	# memory page
	.long	0x260000+PG_PCB+PG_RWE	# memory page
	.long	0x261000+PG_PCB+PG_RWE	# memory page
	.long	0x262000+PG_PCB+PG_RWE	# memory page
	.long	0x263000+PG_PCB+PG_RWE	# memory page
	.long	0x264000+PG_PCB+PG_RWE	# memory page
	.long	0x265000+PG_PCB+PG_RWE	# memory page
	.long	0x266000+PG_PCB+PG_RWE	# memory page
	.long	0x267000+PG_PCB+PG_RWE	# memory page
	.long	0x268000+PG_PCB+PG_RWE	# memory page
	.long	0x269000+PG_PCB+PG_RWE	# memory page
	.long	0x26a000+PG_PCB+PG_RWE	# memory page
	.long	0x26b000+PG_PCB+PG_RWE	# memory page
	.long	0x26c000+PG_PCB+PG_RWE	# memory page
	.long	0x26d000+PG_PCB+PG_RWE	# memory page
	.long	0x26e000+PG_PCB+PG_RWE	# memory page
	.long	0x26f000+PG_PCB+PG_RWE	# memory page
	.long	0x270000+PG_PCB+PG_RWE	# memory page
	.long	0x271000+PG_PCB+PG_RWE	# memory page
	.long	0x272000+PG_PCB+PG_RWE	# memory page
	.long	0x273000+PG_PCB+PG_RWE	# memory page
	.long	0x274000+PG_PCB+PG_RWE	# memory page
	.long	0x275000+PG_PCB+PG_RWE	# memory page
	.long	0x276000+PG_PCB+PG_RWE	# memory page
	.long	0x277000+PG_PCB+PG_RWE	# memory page
	.long	0x278000+PG_PCB+PG_RWE	# memory page
	.long	0x279000+PG_PCB+PG_RWE	# memory page
	.long	0x27a000+PG_PCB+PG_RWE	# memory page
	.long	0x27b000+PG_PCB+PG_RWE	# memory page
	.long	0x27c000+PG_PCB+PG_RWE	# memory page
	.long	0x27d000+PG_PCB+PG_RWE	# memory page
	.long	0x27e000+PG_PCB+PG_RWE	# memory page
	.long	0x27f000+PG_PCB+PG_RWE	# memory page
	.long	0x280000+PG_PCB+PG_RWE	# memory page
	.long	0x281000+PG_PCB+PG_RWE	# memory page
	.long	0x282000+PG_PCB+PG_RWE	# memory page
	.long	0x283000+PG_PCB+PG_RWE	# memory page
	.long	0x284000+PG_PCB+PG_RWE	# memory page
	.long	0x285000+PG_PCB+PG_RWE	# memory page
	.long	0x286000+PG_PCB+PG_RWE	# memory page
	.long	0x287000+PG_PCB+PG_RWE	# memory page
	.long	0x288000+PG_PCB+PG_RWE	# memory page
	.long	0x289000+PG_PCB+PG_RWE	# memory page
	.long	0x28a000+PG_PCB+PG_RWE	# memory page
	.long	0x28b000+PG_PCB+PG_RWE	# memory page
	.long	0x28c000+PG_PCB+PG_RWE	# memory page
	.long	0x28d000+PG_PCB+PG_RWE	# memory page
	.long	0x28e000+PG_PCB+PG_RWE	# memory page
	.long	0x28f000+PG_PCB+PG_RWE	# memory page
	.long	0x290000+PG_PCB+PG_RWE	# memory page
	.long	0x291000+PG_PCB+PG_RWE	# memory page
	.long	0x292000+PG_PCB+PG_RWE	# memory page
	.long	0x293000+PG_PCB+PG_RWE	# memory page
	.long	0x294000+PG_PCB+PG_RWE	# memory page
	.long	0x295000+PG_PCB+PG_RWE	# memory page
	.long	0x296000+PG_PCB+PG_RWE	# memory page
	.long	0x297000+PG_PCB+PG_RWE	# memory page
	.long	0x298000+PG_PCB+PG_RWE	# memory page
	.long	0x299000+PG_PCB+PG_RWE	# memory page
	.long	0x29a000+PG_PCB+PG_RWE	# memory page
	.long	0x29b000+PG_PCB+PG_RWE	# memory page
	.long	0x29c000+PG_PCB+PG_RWE	# memory page
	.long	0x29d000+PG_PCB+PG_RWE	# memory page
	.long	0x29e000+PG_PCB+PG_RWE	# memory page
	.long	0x29f000+PG_PCB+PG_RWE	# memory page
	.long	0x2a0000+PG_PCB+PG_RWE	# memory page
	.long	0x2a1000+PG_PCB+PG_RWE	# memory page
	.long	0x2a2000+PG_PCB+PG_RWE	# memory page
	.long	0x2a3000+PG_PCB+PG_RWE	# memory page
	.long	0x2a4000+PG_PCB+PG_RWE	# memory page
	.long	0x2a5000+PG_PCB+PG_RWE	# memory page
	.long	0x2a6000+PG_PCB+PG_RWE	# memory page
	.long	0x2a7000+PG_PCB+PG_RWE	# memory page
	.long	0x2a8000+PG_PCB+PG_RWE	# memory page
	.long	0x2a9000+PG_PCB+PG_RWE	# memory page
	.long	0x2aa000+PG_PCB+PG_RWE	# memory page
	.long	0x2ab000+PG_PCB+PG_RWE	# memory page
	.long	0x2ac000+PG_PCB+PG_RWE	# memory page
	.long	0x2ad000+PG_PCB+PG_RWE	# memory page
	.long	0x2ae000+PG_PCB+PG_RWE	# memory page
	.long	0x2af000+PG_PCB+PG_RWE	# memory page
	.long	0x2b0000+PG_PCB+PG_RWE	# memory page
	.long	0x2b1000+PG_PCB+PG_RWE	# memory page
	.long	0x2b2000+PG_PCB+PG_RWE	# memory page
	.long	0x2b3000+PG_PCB+PG_RWE	# memory page
	.long	0x2b4000+PG_PCB+PG_RWE	# memory page
	.long	0x2b5000+PG_PCB+PG_RWE	# memory page
	.long	0x2b6000+PG_PCB+PG_RWE	# memory page
	.long	0x2b7000+PG_PCB+PG_RWE	# memory page
	.long	0x2b8000+PG_PCB+PG_RWE	# memory page
	.long	0x2b9000+PG_PCB+PG_RWE	# memory page
	.long	0x2ba000+PG_PCB+PG_RWE	# memory page
	.long	0x2bb000+PG_PCB+PG_RWE	# memory page
	.long	0x2bc000+PG_PCB+PG_RWE	# memory page
	.long	0x2bd000+PG_PCB+PG_RWE	# memory page
	.long	0x2be000+PG_PCB+PG_RWE	# memory page
	.long	0x2bf000+PG_PCB+PG_RWE	# memory page
	.long	0x2c0000+PG_PCB+PG_RWE	# memory page
	.long	0x2c1000+PG_PCB+PG_RWE	# memory page
	.long	0x2c2000+PG_PCB+PG_RWE	# memory page
	.long	0x2c3000+PG_PCB+PG_RWE	# memory page
	.long	0x2c4000+PG_PCB+PG_RWE	# memory page
	.long	0x2c5000+PG_PCB+PG_RWE	# memory page
	.long	0x2c6000+PG_PCB+PG_RWE	# memory page
	.long	0x2c7000+PG_PCB+PG_RWE	# memory page
	.long	0x2c8000+PG_PCB+PG_RWE	# memory page
	.long	0x2c9000+PG_PCB+PG_RWE	# memory page
	.long	0x2ca000+PG_PCB+PG_RWE	# memory page
	.long	0x2cb000+PG_PCB+PG_RWE	# memory page
	.long	0x2cc000+PG_PCB+PG_RWE	# memory page
	.long	0x2cd000+PG_PCB+PG_RWE	# memory page
	.long	0x2ce000+PG_PCB+PG_RWE	# memory page
	.long	0x2cf000+PG_PCB+PG_RWE	# memory page
	.long	0x2d0000+PG_PCB+PG_RWE	# memory page
	.long	0x2d1000+PG_PCB+PG_RWE	# memory page
	.long	0x2d2000+PG_PCB+PG_RWE	# memory page
	.long	0x2d3000+PG_PCB+PG_RWE	# memory page
	.long	0x2d4000+PG_PCB+PG_RWE	# memory page
	.long	0x2d5000+PG_PCB+PG_RWE	# memory page
	.long	0x2d6000+PG_PCB+PG_RWE	# memory page
	.long	0x2d7000+PG_PCB+PG_RWE	# memory page
	.long	0x2d8000+PG_PCB+PG_RWE	# memory page
	.long	0x2d9000+PG_PCB+PG_RWE	# memory page
	.long	0x2da000+PG_PCB+PG_RWE	# memory page
	.long	0x2db000+PG_PCB+PG_RWE	# memory page
	.long	0x2dc000+PG_PCB+PG_RWE	# memory page
	.long	0x2dd000+PG_PCB+PG_RWE	# memory page
	.long	0x2de000+PG_PCB+PG_RWE	# memory page
	.long	0x2df000+PG_PCB+PG_RWE	# memory page
	.long	0x2e0000+PG_PCB+PG_RWE	# memory page
	.long	0x2e1000+PG_PCB+PG_RWE	# memory page
	.long	0x2e2000+PG_PCB+PG_RWE	# memory page
	.long	0x2e3000+PG_PCB+PG_RWE	# memory page
	.long	0x2e4000+PG_PCB+PG_RWE	# memory page
	.long	0x2e5000+PG_PCB+PG_RWE	# memory page
	.long	0x2e6000+PG_PCB+PG_RWE	# memory page
	.long	0x2e7000+PG_PCB+PG_RWE	# memory page
	.long	0x2e8000+PG_PCB+PG_RWE	# memory page
	.long	0x2e9000+PG_PCB+PG_RWE	# memory page
	.long	0x2ea000+PG_PCB+PG_RWE	# memory page
	.long	0x2eb000+PG_PCB+PG_RWE	# memory page
	.long	0x2ec000+PG_PCB+PG_RWE	# memory page
	.long	0x2ed000+PG_PCB+PG_RWE	# memory page
	.long	0x2ee000+PG_PCB+PG_RWE	# memory page
	.long	0x2ef000+PG_PCB+PG_RWE	# memory page
	.long	0x2f0000+PG_PCB+PG_RWE	# memory page
	.long	0x2f1000+PG_PCB+PG_RWE	# memory page
	.long	0x2f2000+PG_PCB+PG_RWE	# memory page
	.long	0x2f3000+PG_PCB+PG_RWE	# memory page
	.long	0x2f4000+PG_PCB+PG_RWE	# memory page
	.long	0x2f5000+PG_PCB+PG_RWE	# memory page
	.long	0x2f6000+PG_PCB+PG_RWE	# memory page
	.long	0x2f7000+PG_PCB+PG_RWE	# memory page
	.long	0x2f8000+PG_PCB+PG_RWE	# memory page
	.long	0x2f9000+PG_PCB+PG_RWE	# memory page
	.long	0x2fa000+PG_PCB+PG_RWE	# memory page
	.long	0x2fb000+PG_PCB+PG_RWE	# memory page
	.long	0x2fc000+PG_PCB+PG_RWE	# memory page
	.long	0x2fd000+PG_PCB+PG_RWE	# memory page
	.long	0x2fe000+PG_PCB+PG_RWE	# memory page
	.long	0x2ff000+PG_PCB+PG_RWE	# memory page
	.long	0x300000+PG_PCB+PG_RWE	# memory page
	.long	0x301000+PG_PCB+PG_RWE	# memory page
	.long	0x302000+PG_PCB+PG_RWE	# memory page
	.long	0x303000+PG_PCB+PG_RWE	# memory page
	.long	0x304000+PG_PCB+PG_RWE	# memory page
	.long	0x305000+PG_PCB+PG_RWE	# memory page
	.long	0x306000+PG_PCB+PG_RWE	# memory page
	.long	0x307000+PG_PCB+PG_RWE	# memory page
	.long	0x308000+PG_PCB+PG_RWE	# memory page
	.long	0x309000+PG_PCB+PG_RWE	# memory page
	.long	0x30a000+PG_PCB+PG_RWE	# memory page
	.long	0x30b000+PG_PCB+PG_RWE	# memory page
	.long	0x30c000+PG_PCB+PG_RWE	# memory page
	.long	0x30d000+PG_PCB+PG_RWE	# memory page
	.long	0x30e000+PG_PCB+PG_RWE	# memory page
	.long	0x30f000+PG_PCB+PG_RWE	# memory page
	.long	0x310000+PG_PCB+PG_RWE	# memory page
	.long	0x311000+PG_PCB+PG_RWE	# memory page
	.long	0x312000+PG_PCB+PG_RWE	# memory page
	.long	0x313000+PG_PCB+PG_RWE	# memory page
	.long	0x314000+PG_PCB+PG_RWE	# memory page
	.long	0x315000+PG_PCB+PG_RWE	# memory page
	.long	0x316000+PG_PCB+PG_RWE	# memory page
	.long	0x317000+PG_PCB+PG_RWE	# memory page
	.long	0x318000+PG_PCB+PG_RWE	# memory page
	.long	0x319000+PG_PCB+PG_RWE	# memory page
	.long	0x31a000+PG_PCB+PG_RWE	# memory page
	.long	0x31b000+PG_PCB+PG_RWE	# memory page
	.long	0x31c000+PG_PCB+PG_RWE	# memory page
	.long	0x31d000+PG_PCB+PG_RWE	# memory page
	.long	0x31e000+PG_PCB+PG_RWE	# memory page
	.long	0x31f000+PG_PCB+PG_RWE	# memory page
	.long	0x320000+PG_PCB+PG_RWE	# memory page
	.long	0x321000+PG_PCB+PG_RWE	# memory page
	.long	0x322000+PG_PCB+PG_RWE	# memory page
	.long	0x323000+PG_PCB+PG_RWE	# memory page
	.long	0x324000+PG_PCB+PG_RWE	# memory page
	.long	0x325000+PG_PCB+PG_RWE	# memory page
	.long	0x326000+PG_PCB+PG_RWE	# memory page
	.long	0x327000+PG_PCB+PG_RWE	# memory page
	.long	0x328000+PG_PCB+PG_RWE	# memory page
	.long	0x329000+PG_PCB+PG_RWE	# memory page
	.long	0x32a000+PG_PCB+PG_RWE	# memory page
	.long	0x32b000+PG_PCB+PG_RWE	# memory page
	.long	0x32c000+PG_PCB+PG_RWE	# memory page
	.long	0x32d000+PG_PCB+PG_RWE	# memory page
	.long	0x32e000+PG_PCB+PG_RWE	# memory page
	.long	0x32f000+PG_PCB+PG_RWE	# memory page
	.long	0x330000+PG_PCB+PG_RWE	# memory page
	.long	0x331000+PG_PCB+PG_RWE	# memory page
	.long	0x332000+PG_PCB+PG_RWE	# memory page
	.long	0x333000+PG_PCB+PG_RWE	# memory page
	.long	0x334000+PG_PCB+PG_RWE	# memory page
	.long	0x335000+PG_PCB+PG_RWE	# memory page
	.long	0x336000+PG_PCB+PG_RWE	# memory page
	.long	0x337000+PG_PCB+PG_RWE	# memory page
	.long	0x338000+PG_PCB+PG_RWE	# memory page
	.long	0x339000+PG_PCB+PG_RWE	# memory page
	.long	0x33a000+PG_PCB+PG_RWE	# memory page
	.long	0x33b000+PG_PCB+PG_RWE	# memory page
	.long	0x33c000+PG_PCB+PG_RWE	# memory page
	.long	0x33d000+PG_PCB+PG_RWE	# memory page
	.long	0x33e000+PG_PCB+PG_RWE	# memory page
	.long	0x33f000+PG_PCB+PG_RWE	# memory page
	.long	0x340000+PG_PCB+PG_RWE	# memory page
	.long	0x341000+PG_PCB+PG_RWE	# memory page
	.long	0x342000+PG_PCB+PG_RWE	# memory page
	.long	0x343000+PG_PCB+PG_RWE	# memory page
	.long	0x344000+PG_PCB+PG_RWE	# memory page
	.long	0x345000+PG_PCB+PG_RWE	# memory page
	.long	0x346000+PG_PCB+PG_RWE	# memory page
	.long	0x347000+PG_PCB+PG_RWE	# memory page
	.long	0x348000+PG_PCB+PG_RWE	# memory page
	.long	0x349000+PG_PCB+PG_RWE	# memory page
	.long	0x34a000+PG_PCB+PG_RWE	# memory page
	.long	0x34b000+PG_PCB+PG_RWE	# memory page
	.long	0x34c000+PG_PCB+PG_RWE	# memory page
	.long	0x34d000+PG_PCB+PG_RWE	# memory page
	.long	0x34e000+PG_PCB+PG_RWE	# memory page
	.long	0x34f000+PG_PCB+PG_RWE	# memory page
	.long	0x350000+PG_PCB+PG_RWE	# memory page
	.long	0x351000+PG_PCB+PG_RWE	# memory page
	.long	0x352000+PG_PCB+PG_RWE	# memory page
	.long	0x353000+PG_PCB+PG_RWE	# memory page
	.long	0x354000+PG_PCB+PG_RWE	# memory page
	.long	0x355000+PG_PCB+PG_RWE	# memory page
	.long	0x356000+PG_PCB+PG_RWE	# memory page
	.long	0x357000+PG_PCB+PG_RWE	# memory page
	.long	0x358000+PG_PCB+PG_RWE	# memory page
	.long	0x359000+PG_PCB+PG_RWE	# memory page
	.long	0x35a000+PG_PCB+PG_RWE	# memory page
	.long	0x35b000+PG_PCB+PG_RWE	# memory page
	.long	0x35c000+PG_PCB+PG_RWE	# memory page
	.long	0x35d000+PG_PCB+PG_RWE	# memory page
	.long	0x35e000+PG_PCB+PG_RWE	# memory page
	.long	0x35f000+PG_PCB+PG_RWE	# memory page
	.long	0x360000+PG_PCB+PG_RWE	# memory page
	.long	0x361000+PG_PCB+PG_RWE	# memory page
	.long	0x362000+PG_PCB+PG_RWE	# memory page
	.long	0x363000+PG_PCB+PG_RWE	# memory page
	.long	0x364000+PG_PCB+PG_RWE	# memory page
	.long	0x365000+PG_PCB+PG_RWE	# memory page
	.long	0x366000+PG_PCB+PG_RWE	# memory page
	.long	0x367000+PG_PCB+PG_RWE	# memory page
	.long	0x368000+PG_PCB+PG_RWE	# memory page
	.long	0x369000+PG_PCB+PG_RWE	# memory page
	.long	0x36a000+PG_PCB+PG_RWE	# memory page
	.long	0x36b000+PG_PCB+PG_RWE	# memory page
	.long	0x36c000+PG_PCB+PG_RWE	# memory page
	.long	0x36d000+PG_PCB+PG_RWE	# memory page
	.long	0x36e000+PG_PCB+PG_RWE	# memory page
	.long	0x36f000+PG_PCB+PG_RWE	# memory page
	.long	0x370000+PG_PCB+PG_RWE	# memory page
	.long	0x371000+PG_PCB+PG_RWE	# memory page
	.long	0x372000+PG_PCB+PG_RWE	# memory page
	.long	0x373000+PG_PCB+PG_RWE	# memory page
	.long	0x374000+PG_PCB+PG_RWE	# memory page
	.long	0x375000+PG_PCB+PG_RWE	# memory page
	.long	0x376000+PG_PCB+PG_RWE	# memory page
	.long	0x377000+PG_PCB+PG_RWE	# memory page
	.long	0x378000+PG_PCB+PG_RWE	# memory page
	.long	0x379000+PG_PCB+PG_RWE	# memory page
	.long	0x37a000+PG_PCB+PG_RWE	# memory page
	.long	0x37b000+PG_PCB+PG_RWE	# memory page
	.long	0x37c000+PG_PCB+PG_RWE	# memory page
	.long	0x37d000+PG_PCB+PG_RWE	# memory page
	.long	0x37e000+PG_PCB+PG_RWE	# memory page
	.long	0x37f000+PG_PCB+PG_RWE	# memory page
	.long	0x380000+PG_PCB+PG_RWE	# memory page
	.long	0x381000+PG_PCB+PG_RWE	# memory page
	.long	0x382000+PG_PCB+PG_RWE	# memory page
	.long	0x383000+PG_PCB+PG_RWE	# memory page
	.long	0x384000+PG_PCB+PG_RWE	# memory page
	.long	0x385000+PG_PCB+PG_RWE	# memory page
	.long	0x386000+PG_PCB+PG_RWE	# memory page
	.long	0x387000+PG_PCB+PG_RWE	# memory page
	.long	0x388000+PG_PCB+PG_RWE	# memory page
	.long	0x389000+PG_PCB+PG_RWE	# memory page
	.long	0x38a000+PG_PCB+PG_RWE	# memory page
	.long	0x38b000+PG_PCB+PG_RWE	# memory page
	.long	0x38c000+PG_PCB+PG_RWE	# memory page
	.long	0x38d000+PG_PCB+PG_RWE	# memory page
	.long	0x38e000+PG_PCB+PG_RWE	# memory page
	.long	0x38f000+PG_PCB+PG_RWE	# memory page
	.long	0x390000+PG_PCB+PG_RWE	# memory page
	.long	0x391000+PG_PCB+PG_RWE	# memory page
	.long	0x392000+PG_PCB+PG_RWE	# memory page
	.long	0x393000+PG_PCB+PG_RWE	# memory page
	.long	0x394000+PG_PCB+PG_RWE	# memory page
	.long	0x395000+PG_PCB+PG_RWE	# memory page
	.long	0x396000+PG_PCB+PG_RWE	# memory page
	.long	0x397000+PG_PCB+PG_RWE	# memory page
	.long	0x398000+PG_PCB+PG_RWE	# memory page
	.long	0x399000+PG_PCB+PG_RWE	# memory page
	.long	0x39a000+PG_PCB+PG_RWE	# memory page
	.long	0x39b000+PG_PCB+PG_RWE	# memory page
	.long	0x39c000+PG_PCB+PG_RWE	# memory page
	.long	0x39d000+PG_PCB+PG_RWE	# memory page
	.long	0x39e000+PG_PCB+PG_RWE	# memory page
	.long	0x39f000+PG_PCB+PG_RWE	# memory page
	.long	0x3a0000+PG_PCB+PG_RWE	# memory page
	.long	0x3a1000+PG_PCB+PG_RWE	# memory page
	.long	0x3a2000+PG_PCB+PG_RWE	# memory page
	.long	0x3a3000+PG_PCB+PG_RWE	# memory page
	.long	0x3a4000+PG_PCB+PG_RWE	# memory page
	.long	0x3a5000+PG_PCB+PG_RWE	# memory page
	.long	0x3a6000+PG_PCB+PG_RWE	# memory page
	.long	0x3a7000+PG_PCB+PG_RWE	# memory page
	.long	0x3a8000+PG_PCB+PG_RWE	# memory page
	.long	0x3a9000+PG_PCB+PG_RWE	# memory page
	.long	0x3aa000+PG_PCB+PG_RWE	# memory page
	.long	0x3ab000+PG_PCB+PG_RWE	# memory page
	.long	0x3ac000+PG_PCB+PG_RWE	# memory page
	.long	0x3ad000+PG_PCB+PG_RWE	# memory page
	.long	0x3ae000+PG_PCB+PG_RWE	# memory page
	.long	0x3af000+PG_PCB+PG_RWE	# memory page
	.long	0x3b0000+PG_PCB+PG_RWE	# memory page
	.long	0x3b1000+PG_PCB+PG_RWE	# memory page
	.long	0x3b2000+PG_PCB+PG_RWE	# memory page
	.long	0x3b3000+PG_PCB+PG_RWE	# memory page
	.long	0x3b4000+PG_PCB+PG_RWE	# memory page
	.long	0x3b5000+PG_PCB+PG_RWE	# memory page
	.long	0x3b6000+PG_PCB+PG_RWE	# memory page
	.long	0x3b7000+PG_PCB+PG_RWE	# memory page
	.long	0x3b8000+PG_PCB+PG_RWE	# memory page
	.long	0x3b9000+PG_PCB+PG_RWE	# memory page
	.long	0x3ba000+PG_PCB+PG_RWE	# memory page
	.long	0x3bb000+PG_PCB+PG_RWE	# memory page
	.long	0x3bc000+PG_PCB+PG_RWE	# memory page
	.long	0x3bd000+PG_PCB+PG_RWE	# memory page
	.long	0x3be000+PG_PCB+PG_RWE	# memory page
	.long	0x3bf000+PG_PCB+PG_RWE	# memory page
	.long	0x3c0000+PG_PCB+PG_RWE	# memory page
	.long	0x3c1000+PG_PCB+PG_RWE	# memory page
	.long	0x3c2000+PG_PCB+PG_RWE	# memory page
	.long	0x3c3000+PG_PCB+PG_RWE	# memory page
	.long	0x3c4000+PG_PCB+PG_RWE	# memory page
	.long	0x3c5000+PG_PCB+PG_RWE	# memory page
	.long	0x3c6000+PG_PCB+PG_RWE	# memory page
	.long	0x3c7000+PG_PCB+PG_RWE	# memory page
	.long	0x3c8000+PG_PCB+PG_RWE	# memory page
	.long	0x3c9000+PG_PCB+PG_RWE	# memory page
	.long	0x3ca000+PG_PCB+PG_RWE	# memory page
	.long	0x3cb000+PG_PCB+PG_RWE	# memory page
	.long	0x3cc000+PG_PCB+PG_RWE	# memory page
	.long	0x3cd000+PG_PCB+PG_RWE	# memory page
	.long	0x3ce000+PG_PCB+PG_RWE	# memory page
	.long	0x3cf000+PG_PCB+PG_RWE	# memory page
	.long	0x3d0000+PG_PCB+PG_RWE	# memory page
	.long	0x3d1000+PG_PCB+PG_RWE	# memory page
	.long	0x3d2000+PG_PCB+PG_RWE	# memory page
	.long	0x3d3000+PG_PCB+PG_RWE	# memory page
	.long	0x3d4000+PG_PCB+PG_RWE	# memory page
	.long	0x3d5000+PG_PCB+PG_RWE	# memory page
	.long	0x3d6000+PG_PCB+PG_RWE	# memory page
	.long	0x3d7000+PG_PCB+PG_RWE	# memory page
	.long	0x3d8000+PG_PCB+PG_RWE	# memory page
	.long	0x3d9000+PG_PCB+PG_RWE	# memory page
	.long	0x3da000+PG_PCB+PG_RWE	# memory page
	.long	0x3db000+PG_PCB+PG_RWE	# memory page
	.long	0x3dc000+PG_PCB+PG_RWE	# memory page
	.long	0x3dd000+PG_PCB+PG_RWE	# memory page
	.long	0x3de000+PG_PCB+PG_RWE	# memory page
	.long	0x3df000+PG_PCB+PG_RWE	# memory page
	.long	0x3e0000+PG_PCB+PG_RWE	# memory page
	.long	0x3e1000+PG_PCB+PG_RWE	# memory page
	.long	0x3e2000+PG_PCB+PG_RWE	# memory page
	.long	0x3e3000+PG_PCB+PG_RWE	# memory page
	.long	0x3e4000+PG_PCB+PG_RWE	# memory page
	.long	0x3e5000+PG_PCB+PG_RWE	# memory page
	.long	0x3e6000+PG_PCB+PG_RWE	# memory page
	.long	0x3e7000+PG_PCB+PG_RWE	# memory page
	.long	0x3e8000+PG_PCB+PG_RWE	# memory page
	.long	0x3e9000+PG_PCB+PG_RWE	# memory page
	.long	0x3ea000+PG_PCB+PG_RWE	# memory page
	.long	0x3eb000+PG_PCB+PG_RWE	# memory page
	.long	0x3ec000+PG_PCB+PG_RWE	# memory page
	.long	0x3ed000+PG_PCB+PG_RWE	# memory page
	.long	0x3ee000+PG_PCB+PG_RWE	# memory page
	.long	0x3ef000+PG_PCB+PG_RWE	# memory page
	.long	0x3f0000+PG_PCB+PG_RWE	# memory page
	.long	0x3f1000+PG_PCB+PG_RWE	# memory page
	.long	0x3f2000+PG_PCB+PG_RWE	# memory page
	.long	0x3f3000+PG_PCB+PG_RWE	# memory page
	.long	0x3f4000+PG_PCB+PG_RWE	# memory page
	.long	0x3f5000+PG_PCB+PG_RWE	# memory page
	.long	0x3f6000+PG_PCB+PG_RWE	# memory page
	.long	0x3f7000+PG_PCB+PG_RWE	# memory page
	.long	0x3f8000+PG_PCB+PG_RWE	# memory page
	.long	0x3f9000+PG_PCB+PG_RWE	# memory page
	.long	0x3fa000+PG_PCB+PG_RWE	# memory page
	.long	0x3fb000+PG_PCB+PG_RWE	# memory page
	.long	0x3fc000+PG_PCB+PG_RWE	# memory page
	.long	0x3fd000+PG_PCB+PG_RWE	# memory page
	.long	0x3fe000+PG_PCB+PG_RWE	# memory page
	.long	0x3ff000+PG_PCB+PG_RWE	# memory page

	.data
	.align	4
	.globl cerror
cerror: .double 0			# used by libc
	.globl	_S_CR0
_S_CR0:
	.long	CR0
__maxpmem: .long 0
__pagetable: .long 0
__dmapage: .long 0
	.globl	__argc
	.globl	__argv00
__argc:	.long	0
__argv0: .long	__argv00
__argv00: .long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
env00:	.long	0
_environ:
	.long	env00
