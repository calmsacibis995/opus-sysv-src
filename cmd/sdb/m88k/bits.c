/* 	Dissambler for MC88000 sdb
 */

#define mneu mnem

#include	"../../sgs/dis/m88k/dis.c"

int errlev;
extern dot;
extern loc;
extern char operands[];

static savfmt;

dis_dot(loccntr,idsp,format) {
	savfmt = format;
	errlev = 0;
	loc = dot + 4;
	idis(get(loccntr,idsp));
	return(loc);
}

extern char sl_name[];
extern short sl_scnum;

void *extsympr(val, sym)
char **sym;
{
	register long diff;
	char *namptr;

	if (savfmt == 'I') {	/* don't print symbolically */
		if (val >= 10)
			sprintf(*sym, "0x%x\n", val);
		else
			sprintf(*sym, "%x\n", val);
		return;
	}
	diff = adrtoext(val);
	if (diff >= 0 && sl_scnum >= 0) {
		namptr = &sl_name[0];
		if (*sl_name == '_')
			namptr++;
		if (diff > 0)
			if (diff >= 10)
				sprintf(*sym, "%s+0x%x", namptr, diff);
			else
				sprintf(*sym, "%s+%x", namptr, diff);
		else
			sprintf(*sym, "%s", namptr);
	}
}
