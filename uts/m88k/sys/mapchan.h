/*  @(#)mapchan.h	1.1 */
#ifndef _SYS_MAPCHAN_H_
#define _SYS_MAPCHAN_H_
/*
 * mapchan.h 
*/

struct mcinput {
	u_char typed;
	u_char xlate;
};

struct mcdeadkeys {
	u_char typed;
	u_char xlate;
};
struct mcdead {
	u_char deadkey;
	u_char count;
	struct mcdeadkeys mcdeadkeys[1];	/* variable length */
};


struct mccompose {
	u_char typed1;
	u_char typed2;
	u_char xlate;
};

struct mcoutput {
	u_char outchar;
	u_char xlate[1];		/* null termination */
};

struct mapchan_hdr {
	int refcount;		/* reference count for sharing */
	int version;
	int ninput;
	int input_offset;
	int ndead;
	int dead_offset;
	char pad[2];
	u_char compose_char;
	u_char beep;		/* beep on translation error */
	int ncompose;
	int compose_offset;
	int noutput;
	int output_offset;
};

struct mapchan {		/* each channel map is 1024 bytes */
	struct mapchan_hdr hdr;
	u_char data[1024 - sizeof(struct mapchan_hdr)];
};

struct mctbl {
	int mapping;		/* 0 = no, 1 = mapping on */
	struct mapchan *mapchan_p;
	int input_state[2];	/* state of input translation */
	int output_state;	/* state of output translation */
	u_char *next_out_xlate;	/* next translated character to go out */
	u_char lastchar[2];
};

#define	GET_INPUT_PTR(m) ((struct mcinput *) &(m)->data[(m)->hdr.input_offset])
#define	GET_COMPOSE_PTR(m) ((struct mccompose *) &(m)->data[(m)->hdr.compose_offset])
#define	GET_DEAD_PTR(m) ((struct mcdead *) &(m)->data[(m)->hdr.dead_offset])
#define	GET_OUTPUT_PTR(m) ((struct mcoutput *) &(m)->data[(m)->hdr.output_offset])

/* input states */
#define MC_INPUT_STATE_IDLE 0		/* no input translation occuring */ 
#define MC_INPUT_STATE_DEAD 1		/* got one of dead keys (in lastchar) */
#define MC_INPUT_STATE_COMP 2		/* got compose character */
#define MC_INPUT_STATE_CMP1 3		/* got compose first char */

/* output states */
#define MC_OUTPUT_STATE_IDLE 	0	/* no output translation occuring */
#define MC_OUTPUT_STATE_BUSY 	0	/* output translation in progress */
#endif /* ! _SYS_MAPCHAN_H_ */
