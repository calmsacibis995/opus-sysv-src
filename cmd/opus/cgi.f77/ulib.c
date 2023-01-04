
/* Wizard of Opus Systems Library Generator */

/* Version 1.04 -- January 19, 1988 */

/* LIBNAME(CGI) */
/*				*/
/* TYPE(short,SINT16) */
/* TYPE(long,SINT32) */
/* TYPE(short,FD) */
/* TYPE(char,CHAR) */
/* TYPE(unsigned char,BIT8) */
/* TYPE(unsigned short,BIT16) */
/* TYPE(unsigned long,BIT32) */
/* TYPE(unsigned short,BOOLEAN) */

#include "oplib.h"

#include <sys/types.h>
#define NO_EXT_KEYS

struct libctl CGIctl = {
	"CGI",		/* name */
	BUF_DEF,	/* bufsize */
	BUF_MAX,	/* bufmax */
	ARG_DEF,	/* argsize */
	0,		/* handle */
	0L,		/* bufbase */
	0L,		/* bufptr */
	0L,		/* bufend */
	0L,		/* bufdma */
	0L,		/* dmaptr */
	0L,		/* prior */
};


typedef short SINT16;
typedef long SINT32;
typedef short FD;
typedef char CHAR;
typedef unsigned char BIT8;
typedef unsigned short BIT16;
typedef unsigned long BIT32;
typedef unsigned short BOOLEAN;

/* Application Data */

SINT16
v_appl (dev_handle,function,data_cnt,app_data)

	SINT16 dev_handle;
	CHAR function[];	/* BUFOUT[STRING] */
	SINT16 data_cnt;
	SINT16 app_data[];	/* BUFOUT[data_cnt] */
{
	short rtnval;
	LIB_DCL();

	CALL_ID(0,5,CF_BUF);
	Abufin(&rtnval,1,short);
	Ashort(dev_handle);
	Abufout(function,strlen(function)+1,char);
	Ashort(data_cnt);
	Abufout(app_data,data_cnt,short);
