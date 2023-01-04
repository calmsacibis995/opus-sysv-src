static char copyright12[] = "Copyright (c) Motorola, Inc. 1985,1986,1987,1988";

/*
*			Disassembler Instruction Table
*
*	File:	instab.h	11/08/85
*
*	Copyright:  Motorola, Inc. 1985,1986,1987,1988
*
*				Functional Description
*
*	The first field of the table is the opcode field. If an opcode
*	is specified which has any non-opcode bits on, a system error
*	will occur when the system attempts the install it into the
*	instruction table.  The second parameter is a pointer to the
*	instruction mnemonic. Each operand is specified by offset, width,
*	and type. The offset is the bit number of the least significant
*	bit of the operand with bit 0 being the least significant bit of
*	the instruction. The width is the number of bits used to specify
*	the operand. The type specifies the output format to be used for
*	the operand. The valid formats are: register, register indirect,
*	hex constant, and bit field specification.  The last field is a
*	pointer to the next instruction in the linked list.  These pointers
*	are initialized by init_disasm().
*
*				Structure Format
*
*       struct INSTAB {
*          UPINT opcode;
*          char *mnemonic;
*          struct OPSPEC op1,op2,op3;
*          struct SIM_FLAGS flgs;
*          struct INSTAB *next;
*       }
*
*       struct OPSPEC {
*          UPINT offset:5;
*          UPINT width:6;
*          UPINT type:5;
*       }
*
*				Revision History
*
*	Revision 1.0	11/08/85	Creation date
*		 1.1	02/05/86	Updated instruction mnemonic table MD
*		 1.2	06/16/86	Updated SIM_FLAGS for floating point
*		 1.3	09/20/86	Updated for new encoding
*/

#include <stdio.h>
#include "disasm.h"

/* Opcode     Mnemonic       Op 1 Spec     Op 2 Spec    Op 3 Spec         Simflags             Next  */

static INSTAB  instructions[] =
{0xf400c800,"jsr      ",{0,5,REG}   ,{0,0,0}      ,{0,0,0}   , {2,2,NA,JSR ,          0,0,1,0,0,0,0,1,0,0,0,0}, NULL,
 0xf400cc00,"jsr.n    ",{0,5,REG}   ,{0,0,0}      ,{0,0,0}   , {1,1,NA,JSR ,          0,0,1,0,0,0,1,1,0,0,0,0}, NULL,
 0xf400c000,"jmp      ",{0,5,REG}   ,{0,0,0}      ,{0,0,0}   , {2,2,NA,JMP ,          0,0,1,0,0,0,0,1,0,0,0,0}, NULL,
 0xf400c400,"jmp.n    ",{0,5,REG}   ,{0,0,0}      ,{0,0,0}   , {1,1,NA,JMP ,          0,0,1,0,0,0,1,1,0,0,0,0}, NULL,
 0xc8000000,"bsr      ",{0,26,HEX}  ,{0,0,0}      ,{0,0,0}   , {2,2,NA,BSR ,     i26bit,0,0,0,0,0,0,1,0,0,0,0}, NULL,
 0xcc000000,"bsr.n    ",{0,26,HEX}  ,{0,0,0}      ,{0,0,0}   , {1,1,NA,BSR ,     i26bit,0,0,0,0,0,1,1,0,0,0,0}, NULL,
 0xc0000000,"br       ",{0,26,HEX}  ,{0,0,0}      ,{0,0,0}   , {2,2,NA,BR  ,     i26bit,0,0,0,0,0,0,1,0,0,0,0}, NULL,
 0xc4000000,"br.n     ",{0,26,HEX}  ,{0,0,0}      ,{0,0,0}   , {1,1,NA,BR  ,     i26bit,0,0,0,0,0,1,1,0,0,0,0}, NULL,
 0xd0000000,"bb0      ",{21,5,HEX}  ,{16,5,REG}   ,{0,16,HEX}, {2,2,NA,BB0 ,     i16bit,0,1,0,0,0,0,1,0,0,0,0}, NULL,
 0xd4000000,"bb0.n    ",{21,5,HEX}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,NA,BB0 ,     i16bit,0,1,0,0,0,1,1,0,0,0,0}, NULL,
 0xd8000000,"bb1      ",{21,5,HEX}  ,{16,5,REG}   ,{0,16,HEX}, {2,2,NA,BB1 ,     i16bit,0,1,0,0,0,0,1,0,0,0,0}, NULL,
 0xdc000000,"bb1.n    ",{21,5,HEX}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,NA,BB1 ,     i16bit,0,1,0,0,0,1,1,0,0,0,0}, NULL,
 0xf000d000,"tb0      ",{21,5,HEX}  ,{16,5,REG}   ,{0,10,HEX}, {2,2,NA,TB0 ,     i10bit,0,1,0,0,0,0,1,0,0,0,0}, NULL,
 0xf000d800,"tb1      ",{21,5,HEX}  ,{16,5,REG}   ,{0,10,HEX}, {2,2,NA,TB1 ,     i10bit,0,1,0,0,0,0,1,0,0,0,0}, NULL,
 0xe8000000,"bcnd     ",{21,5,HEX}  ,{16,5,REG}   ,{0,16,HEX}, {2,2,NA,BCND,     i16bit,0,1,0,0,0,0,1,0,0,0,0}, NULL,
 0xec000000,"bcnd.n   ",{21,5,HEX}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,NA,BCND,     i16bit,0,1,0,0,0,1,1,0,0,0,0}, NULL,
 0xf000e800,"tcnd     ",{21,5,HEX}  ,{16,5,REG}   ,{0,10,HEX}, {2,2,NA,TCND,     i10bit,0,1,0,0,0,0,1,0,0,0,0}, NULL,
 0xf8000000,"tbnd     ",{16,5,REG}  ,{0,16,HEX}   ,{0,0,0}   , {2,2,NA,TBND,     i10bit,1,0,0,0,0,0,1,0,0,0,0}, NULL,
 0xf400f800,"tbnd     ",{16,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {2,2,NA,TBND,          0,1,1,0,0,0,0,1,0,0,0,0}, NULL,
 0xf400fc00,"rte      ",{0,0,0}     ,{0,0,0}      ,{0,0,0}   , {2,2,NA,RTE ,          0,0,0,0,0,0,0,1,0,0,0,0}, NULL,
 0x1c000000,"ld.b     ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {3,1,PMEM,LDB    ,i16bit,1,0,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4001c00,"ld.b     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {3,1,PMEM,LDB     ,    0,1,1,1,0,0,0,1,0,0,0,0}, NULL,
 0x0c000000,"ld.bu    ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {3,1,PMEM,LDBU,   i16bit,1,0,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4000c00,"ld.bu    ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {3,1,PMEM,LDBU        ,0,1,1,1,0,0,0,1,0,0,0,0}, NULL,
 0x18000000,"ld.h     ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {3,1,PMEM,LDH    ,i16bit,1,0,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4001800,"ld.h     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {3,1,PMEM,LDH         ,0,1,1,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4001a00,"ld.h     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{3,1,PMEM,LDH         ,0,1,1,1,0,0,0,1,0,0,0,1}, NULL,
 0x08000000,"ld.hu    ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {3,1,PMEM,LDHU,   i16bit,1,0,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4000800,"ld.hu    ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {3,1,PMEM,LDHU        ,0,1,1,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4000a00,"ld.hu    ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{3,1,PMEM,LDHU        ,0,1,1,1,0,0,0,1,0,0,0,1}, NULL,
 0x14000000,"ld       ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {3,1,PMEM,LD     ,i16bit,1,0,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4001400,"ld       ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {3,1,PMEM,LD          ,0,1,1,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4001600,"ld       ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{3,1,PMEM,LD          ,0,1,1,1,0,0,0,1,0,0,0,1}, NULL,
 0x10000000,"ld.d     ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {3,1,PMEM,LDD    ,i16bit,1,0,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4001000,"ld.d     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {3,1,PMEM,LDD         ,0,1,1,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4001200,"ld.d     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{3,1,PMEM,LDD         ,0,1,1,1,0,0,0,1,0,0,0,1}, NULL,
 0xf4001500,"ld.usr   ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {3,1,PMEM,LD          ,0,1,1,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4001700,"ld.usr   ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{3,1,PMEM,LD          ,0,1,1,1,0,0,0,1,0,0,0,1}, NULL,
 0x2c000000,"st.b     ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,NA,STB      ,i16bit,1,0,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4002c00,"st.b     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,NA,STB           ,0,1,1,1,0,0,0,1,0,0,0,0}, NULL,
 0x28000000,"st.h     ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,NA,STH      ,i16bit,1,0,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4002800,"st.h     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,NA,STH           ,0,1,1,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4002a00,"st.h     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{1,1,NA,STH           ,0,1,1,1,0,0,0,1,0,0,0,1}, NULL,
 0x24000000,"st       ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,NA,ST       ,i16bit,1,0,1,0,0,0,1,0,0,0,0}, NULL,
 0xf4002400,"st       ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,NA,ST            ,0,1,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0xf4002600,"st       ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{1,1,NA,ST            ,0,1,1,1,0,0,0,1,0,0,0,1}   ,NULL,
 0x20000000,"st.d     ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,NA,STD      ,i16bit,0,1,0,0,0,0,1,0,0,0,0}   ,NULL,
 0xf4002000,"st.d     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,NA,STD           ,0,1,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0xf4002200,"st.d     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{1,1,NA,STD           ,0,1,1,1,0,0,0,1,0,0,0,1}   ,NULL,
 0xf4002500,"st.usr   ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,NA,ST            ,0,1,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0xf4002700,"st.usr   ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{1,1,NA,ST            ,0,1,1,1,0,0,0,1,0,0,0,1}   ,NULL,
 0x00000000,"xmem.bu  ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {3,1,PMEM,XMEMBU ,i16bit,1,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0xf4000000,"xmem.bu  ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {3,1,PMEM,XMEM        ,0,1,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x04000000,"xmem     ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {3,1,PMEM,XMEM   ,i16bit,1,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0xf4000400,"xmem     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {3,1,PMEM,XMEM        ,0,1,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0xf4000600,"xmem     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{3,1,PMEM,XMEM        ,0,1,1,1,0,0,0,1,0,0,0,1}   ,NULL,
 0xf4000500,"xmem.usr ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {3,1,PMEM,XMEM        ,0,1,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0xf4000700,"xmem.usr ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{3,1,PMEM,XMEM        ,0,1,1,1,0,0,0,1,0,0,0,1}   ,NULL,
 0xf4003e00,"lda.b    ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{1,1,PINT,LDAH,        0,1,1,1,0,0,0,0,0,0,0,1}   ,NULL,
 0xf4003a00,"lda.h    ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{1,1,PINT,LDAH,        0,1,1,1,0,0,0,0,0,0,0,1}   ,NULL,
 0xf4003600,"lda      ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{1,1,PINT,LDA ,        0,1,1,1,0,0,0,0,0,0,0,1}   ,NULL,
 0xf4003200,"lda.d    ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REGSC},{1,1,PINT,LDAD,        0,1,1,1,0,0,0,0,0,0,0,1}   ,NULL,

 0x80004000,"ldcr     ",{21,5,REG}  ,{5,6,CRREG}  ,{0,0,0}    ,{1,1,PINT,LDCR,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0x80008000,"stcr     ",{16,5,REG}  ,{5,6,CRREG}  ,{0,0,0}    ,{1,1,PINT,STCR,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0x8000c000,"xcr      ",{21,5,REG}  ,{16,5,REG}   ,{5,6,CRREG},{1,1,PINT,XCR,         0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,

 0xf4006000,"addu     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,ADDU,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4006200,"addu.ci  ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,ADDU,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4006100,"addu.co  ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,ADDU,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4006300,"addu.cio ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,ADDU,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4006400,"subu     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,SUBU,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4006600,"subu.ci  ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,SUBU,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4006500,"subu.co  ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,SUBU,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4006700,"subu.cio ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,SUBU,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4006900,"divu     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {32,32,PINT,DIVU,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4006d00,"mul      ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,4,PINT,MUL,      0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4007000,"add      ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,ADD ,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4007200,"add.ci   ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,ADD ,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4007100,"add.co   ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,ADD ,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4007300,"add.cio  ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,ADD ,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4007400,"sub      ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,SUB ,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4007600,"sub.ci   ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,SUB ,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4007500,"sub.co   ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,SUB ,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4007700,"sub.cio  ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,SUB ,        0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4007900,"div      ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {32,32,PINT,DIV ,      0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4007d00,"cmp      ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,CMP,         0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,

 0x60000000,"addu     ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,PINT,ADDU,   i16bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0x64000000,"subu     ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,PINT,SUBU,   i16bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,

 0x68000000,"divu     ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {32,32,PINT,DIVU, i16bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0x6c000000,"mul      ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {4,1,PINT,MUL,    i16bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0x70000000,"add      ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,PINT,ADD,    i16bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0x74000000,"sub      ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,PINT,SUB,    i16bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0x78000000,"div      ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {32,32,PINT,DIV,  i16bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0x7c000000,"cmp      ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,PINT,CMP,    i16bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,

 0xf4004000,"and      ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,AND         ,0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4004400,"and.c    ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,AND         ,0,1,1,1,1,0,0,0,0,0,0,0}   ,NULL,
 0xf4005800,"or       ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,OR          ,0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4005c00,"or.c     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,OR          ,0,1,1,1,1,0,0,0,0,0,0,0}   ,NULL,
 0xf4005000,"xor      ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,XOR         ,0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4005400,"xor.c    ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,XOR         ,0,1,1,1,1,0,0,0,0,0,0,0}   ,NULL,
 0x40000000,"and      ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,PINT,AND    ,i16bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0x44000000,"and.u    ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,PINT,AND    ,i16bit,1,0,1,0,1,0,0,0,0,0,0}   ,NULL,
 0x58000000,"or       ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,PINT,OR     ,i16bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0x5c000000,"or.u     ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,PINT,OR     ,i16bit,1,0,1,0,1,0,0,0,0,0,0}   ,NULL,
 0x50000000,"xor      ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,PINT,XOR    ,i16bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0x54000000,"xor.u    ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,PINT,XOR    ,i16bit,1,0,1,0,1,0,0,0,0,0,0}   ,NULL,
 0x48000000,"mask     ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,PINT,MASK   ,i16bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0x4c000000,"mask.u   ",{21,5,REG}  ,{16,5,REG}   ,{0,16,HEX}, {1,1,PINT,MASK   ,i16bit,1,0,1,0,1,0,0,0,0,0,0}   ,NULL,
 0xf400ec00,"ff0      ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {1,1,PINT,FF0         ,0,0,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf400e800,"ff1      ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {1,1,PINT,FF1         ,0,0,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf0008000,"clr      ",{21,5,REG}  ,{16,5,REG}   ,{0,10,BF} , {1,1,PINT,CLR    ,i10bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf0008800,"set      ",{21,5,REG}  ,{16,5,REG}   ,{0,10,BF} , {1,1,PINT,SET    ,i10bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf0009000,"ext      ",{21,5,REG}  ,{16,5,REG}   ,{0,10,BF} , {1,1,PINT,EXT    ,i10bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf0009800,"extu     ",{21,5,REG}  ,{16,5,REG}   ,{0,10,BF} , {1,1,PINT,EXTU   ,i10bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf000a000,"mak      ",{21,5,REG}  ,{16,5,REG}   ,{0,10,BF} , {1,1,PINT,MAK    ,i10bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf000a800,"rot      ",{21,5,REG}  ,{16,5,REG}   ,{0,10,BF} , {1,1,PINT,ROT    ,i10bit,1,0,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4008000,"clr      ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,CLR         ,0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4008800,"set      ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,SET         ,0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4009000,"ext      ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,EXT         ,0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf4009800,"extu     ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,EXTU        ,0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf400a000,"mak      ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,MAK         ,0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,
 0xf400a800,"rot      ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {1,1,PINT,ROT         ,0,1,1,1,0,0,0,0,0,0,0,0}   ,NULL,

 0x84002800,"fadd.sss ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {5,1,PFLT,FADD        ,0,1,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x84002880,"fadd.ssd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FADD        ,0,1,1,1,0,0,0,1,0,0,1,0}   ,NULL,
 0x84002a00,"fadd.sds ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FADD        ,0,1,1,1,0,0,0,1,0,1,0,0}   ,NULL,
 0x84002a80,"fadd.sdd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FADD        ,0,1,1,1,0,0,0,1,0,1,1,0}   ,NULL,
 0x84002820,"fadd.dss ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FADD        ,0,1,1,1,0,0,0,1,1,0,0,0}   ,NULL,
 0x840028a0,"fadd.dsd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FADD        ,0,1,1,1,0,0,0,1,1,0,1,0}   ,NULL,
 0x84002a20,"fadd.dds ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FADD        ,0,1,1,1,0,0,0,1,1,1,0,0}   ,NULL,
 0x84002aa0,"fadd.ddd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FADD        ,0,1,1,1,0,0,0,1,1,1,1,0}   ,NULL,
 0x84003000,"fsub.sss ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {5,1,PFLT,FSUB        ,0,1,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x84003080,"fsub.ssd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FSUB        ,0,1,1,1,0,0,0,1,0,0,1,0}   ,NULL,
 0x84003200,"fsub.sds ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FSUB        ,0,1,1,1,0,0,0,1,0,1,0,0}   ,NULL,
 0x84003280,"fsub.sdd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FSUB        ,0,1,1,1,0,0,0,1,0,1,1,0}   ,NULL,
 0x84003020,"fsub.dss ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FSUB        ,0,1,1,1,0,0,0,1,1,0,0,0}   ,NULL,
 0x840030a0,"fsub.dsd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FSUB        ,0,1,1,1,0,0,0,1,1,0,1,0}   ,NULL,
 0x84003220,"fsub.dds ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FSUB        ,0,1,1,1,0,0,0,1,1,1,0,0}   ,NULL,
 0x840032a0,"fsub.ddd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,2,PFLT,FSUB        ,0,1,1,1,0,0,0,1,1,1,1,0}   ,NULL,
 0x84000000,"fmul.sss ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,1,PFLT,FMUL        ,0,1,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x84000080,"fmul.ssd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {9,2,PFLT,FMUL        ,0,1,1,1,0,0,0,1,0,0,1,0}   ,NULL,
 0x84000200,"fmul.sds ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {9,2,PFLT,FMUL        ,0,1,1,1,0,0,0,1,0,1,0,0}   ,NULL,
 0x84000280,"fmul.sdd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {9,2,PFLT,FMUL        ,0,1,1,1,0,0,0,1,0,1,1,0}   ,NULL,
 0x84000020,"fmul.dss ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {9,2,PFLT,FMUL        ,0,1,1,1,0,0,0,1,1,0,0,0}   ,NULL,
 0x840000a0,"fmul.dsd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {9,2,PFLT,FMUL        ,0,1,1,1,0,0,0,1,1,0,1,0}   ,NULL,
 0x84000220,"fmul.dds ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {9,2,PFLT,FMUL        ,0,1,1,1,0,0,0,1,1,1,0,0}   ,NULL,
 0x840002a0,"fmul.ddd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {9,2,PFLT,FMUL        ,0,1,1,1,0,0,0,1,1,1,1,0}   ,NULL,
 0x84007000,"fdiv.sss ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {30,30,PFLT,FDIV      ,0,1,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x84007080,"fdiv.ssd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {60,60,PFLT,FDIV      ,0,1,1,1,0,0,0,1,0,0,1,0}   ,NULL,
 0x84007200,"fdiv.sds ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {60,60,PFLT,FDIV      ,0,1,1,1,0,0,0,1,0,1,0,0}   ,NULL,
 0x84007280,"fdiv.sdd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {60,60,PFLT,FDIV      ,0,1,1,1,0,0,0,1,0,1,1,0}   ,NULL,
 0x84007020,"fdiv.dss ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {60,60,PFLT,FDIV      ,0,1,1,1,0,0,0,1,1,0,0,0}   ,NULL,
 0x840070a0,"fdiv.dsd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {60,60,PFLT,FDIV      ,0,1,1,1,0,0,0,1,1,0,1,0}   ,NULL,
 0x84007220,"fdiv.dds ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {60,60,PFLT,FDIV      ,0,1,1,1,0,0,0,1,1,1,0,0}   ,NULL,
 0x840072a0,"fdiv.ddd ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {60,60,PFLT,FDIV      ,0,1,1,1,0,0,0,1,1,1,1,0}   ,NULL,
 0x84007800,"fsqrt.ss ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {5,1,PFLT,FLT         ,0,0,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x84007880,"fsqrt.sd ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {5,1,PFLT,FLT         ,0,0,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x84007820,"fsqrt.ds ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {5,1,PFLT,FLT         ,0,0,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x840078a0,"fsqrt.dd ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {6,1,PFLT,FLT         ,0,0,1,1,0,0,0,1,1,0,0,0}   ,NULL,
 0x84003800,"fcmp.ss  ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {5,1,PFLT,FCMP        ,0,1,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x84003880,"fcmp.sd  ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,1,PFLT,FCMP        ,0,1,1,1,0,0,0,1,0,1,0,0}   ,NULL,
 0x84003a00,"fcmp.ds  ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,1,PFLT,FCMP        ,0,1,1,1,0,0,0,1,1,0,0,0}   ,NULL,
 0x84003a80,"fcmp.dd  ",{21,5,REG}  ,{16,5,REG}   ,{0,5,REG} , {6,1,PFLT,FCMP        ,0,1,1,1,0,0,0,1,1,1,0,0}   ,NULL,
 0x84002000,"flt.s    ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {5,1,PFLT,FLT         ,0,0,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x84002020,"flt.d    ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {6,1,PFLT,FLT         ,0,0,1,1,0,0,0,1,1,0,0,0}   ,NULL,
 0x84004800,"int.s    ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {5,1,PFLT,INT         ,0,0,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x84004880,"int.d    ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {6,1,PFLT,INT         ,0,0,1,1,0,0,0,1,1,0,0,0}   ,NULL,
 0x84005000,"nint.s   ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {5,1,PFLT,INT         ,0,0,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x84005080,"nint.d   ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {6,1,PFLT,INT         ,0,0,1,1,0,0,0,1,1,0,0,0}   ,NULL,
 0x84005800,"trnc.s   ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {5,1,PFLT,TRNC        ,0,0,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x84005880,"trnc.d   ",{21,5,REG}  ,{0,5,REG}    ,{0,0,0}   , {6,1,PFLT,TRNC        ,0,0,1,1,0,0,0,1,1,0,0,0}   ,NULL,

 0x80004800,"fldcr    ",{21,5,REG}  ,{5,6,FCRREG} ,{0,0,0}   , {1,1,PFLT,FLDC        ,0,0,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x80008800,"fstcr    ",{16,5,REG}  ,{5,6,FCRREG} ,{0,0,0}   , {1,1,PFLT,FSTC        ,0,0,1,1,0,0,0,1,0,0,0,0}   ,NULL,
 0x8000c800,"fxcr     ",{21,5,REG}  ,{16,5,REG}   ,{5,6,FCRREG} , {1,1,PFLT,FXC         ,0,0,1,1,0,0,0,1,0,0,0,0}   ,NULL};

