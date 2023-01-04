#ident "@(#) $Header: itoa.c 6.0 88/07/06 14:36:50 root Exp $ RISC Source Base"
itoa(n, s)

char 	s[];
int 	n;

{
	int i, sign;
	if ( (sign = n) <0 )
  	   n = -n;
           i = 0;
	do {
	   s[i++] = n %10 + '0';
	} while ( (n /= 10) > 0 );
	if ( sign < 0 ) {
	   s[i++] = '-';
        }
        s[i] = '\0';
      	reverse(s);
}

reverse(s)
char 	s[];
{
	int c, i, j;
	for ( i=0, j=strlen(s)-1; i<j; i++, j-- ) {
		c = s[i];
		s[i] = s[j];
	 	s[j] = c;
	}
}

