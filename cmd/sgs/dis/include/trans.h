/*
*	"trans.h"		Date Created: Oct. 29, 1986
*
*	This is the transaction include file for the VBED debugger
*/

	/* Transaction packet structure */

struct transaction
{
	unsigned long	adr1;
	unsigned long	adr2;
	unsigned long	size;
	unsigned long	cnt;
	unsigned long	expr;
	unsigned long	type;
	unsigned long	page;
	unsigned long	am;
};
