
/*
 * fcons.c: version 1.3 of 12/11/85
 * Floating point constants for NS32000
 */


#ifdef SCCS
static char *sccsid = "@(#)fcons.c	1.3 (NSC) 12/11/85";
#endif

#include <local/fpops.h>
/*  The following are a list of important boundary constants for floating point
    variables.  The boundary constants are:
    maxint and minint - The largest positive and negative integers respectively
    maxintreal and minintreal - Same as above but in NS32000 long float format
    maxfloat and maxnegfloat  - Most positive and negative short float in 
    				long float format.
    leafloat and leanegfloat  - least positive and least negative float
    leadouble and leanegdouble - least positive and least negative
    maxdouble and maxnegdouble - most positive and negative double floating
    				 floating point numbers. */ 
   
				
				quad_wrd	  /* d_lo,  d_hi*/
   maxintplusone = {0, 0x41e00000}, /*2^31 or 2,147,483,648 */
   minintreal    = {0, 0xc1e00000 },/*-2^31 or -2,147,483,648 */
   minintminusone= {0x00200000,0xc1e00000},/*-2^31 or -2,147,483,649 */
   maxintreal	 = {0xffc00000,0x41dfffff},/*2^31-1 or 2,147,483,647 */
   maxfloat	 = {0xe0000000,0x47efffff},/*2^127*(2-2^-23)or 3.4028235e38 */
   maxnegfloat	 = {0xe0000000,0xc7efffff},/*-2^127*(2-2^-23)or-3.4028235e38*/
   leadouble	 = {0,0x0010000},
   leanegdouble	 = {0,0x8010000},
   leafloat	 = {0,0x38100000},
   leanegfloat	 = {0,0xb8100000},
   maxdouble	 = {0xffffffff,0x7fefffff},
   maxnegdouble  = {0xffffffff,0xffefffff},
   zerodouble    = {0x00000000,0x00000000},
   negzerodouble = {0x80000000,0x00000000};

#ifdef vms
/* This dummy is needed on vms because the vms auto initialize the
 * above variables w/o it. fpops must have fp_dummy(), which calls
 * fc_dummy() for this to work.
 */
fc_dummy()
{
}
#endif vms
