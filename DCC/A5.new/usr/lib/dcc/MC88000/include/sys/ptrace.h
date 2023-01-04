#ifndef __Iptrace
#define __Iptrace

struct pt_mem_desc {
	long		pt_mem_type;
	char		*pt_mem_offset;
	char		*pt_mem_va;
	unsigned long	pt_mem_size;
};

#define	PTM_SHDATA	1
#define PTM_SHLIBDATA	2
#define	PTM_SHLIBTEXT	3

struct ptrace_user {
	unsigned long	pt_magic;
	unsigned long	pt_rev;
	long		pt_timdat;
	long		pt_vendor;
	long		pt_r0;
	long		pt_r1;
	long		pt_r2;
	long		pt_r3;
	long		pt_r4;
	long		pt_r5;
	long		pt_r6;
	long		pt_r7;
	long		pt_r8;
	long		pt_r9;
	long		pt_r10;
	long		pt_r11;
	long		pt_r12;
	long		pt_r13;
	long		pt_r14;
	long		pt_r15;
	long		pt_r16;
	long		pt_r17;
	long		pt_r18;
	long		pt_r19;
	long		pt_r20;
	long		pt_r21;
	long		pt_r22;
	long		pt_r23;
	long		pt_r24;
	long		pt_r25;
	long		pt_r26;
	long		pt_r27;
	long		pt_r28;
	long		pt_r29;
	long		pt_r30;
	long		pt_r31;
	long		pt_psr;
	long		pt_fpsr;
	long		pt_fpcr;
	long		pt_ppid;
	char		pt_comm[256];
	long		pt_arg[10];
	long		pt_dsize;
	long		pt_ssize;
	void		(*pt_sigtbl[64]) ();
	short		pt_o_magic;
	short		pt_o_vstamp;
	long		pt_o_tsize;
	long		pt_o_dsize;
	long		pt_o_bsize;
	long		pt_o_entry;
	long		pt_o_text_start;
	long		pt_o_data_start;
	char		*pt_dataptr;
	long		pt_nmem;
	struct		pt_mem_desc	*pt_memptr;
	char		pt_local[256];
	long	pt_sigframe[258];
};

#define PT_TRACE_ME	0
#define PT_READ_I	1
#define PT_READ_D	2
#define PT_READ_U	3
#define PT_WRITE_I	4
#define PT_WRITE_D	5
#define PT_WRITE_U	6
#define PT_CONTINUE	7
#define PT_KILL		8
#define PT_STEP		9

#define PT_TRACE_PROC	128
#define PT_RELEASE_PROC	129
#define PT_TRACE_CHILD	130

#endif
