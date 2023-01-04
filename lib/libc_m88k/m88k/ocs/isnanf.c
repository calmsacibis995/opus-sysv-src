#include <values.h>
#include <nan.h>

_isnanf(f)
long f;		/* fake float */
{
	return(NaF(f));
}

