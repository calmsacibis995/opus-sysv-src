/*	@(#)fort.h	1.2	*/
/*	machine dependent file  */

label( n ){
	printf( "@L%d:\n", n );
	set_r13(0);
	}

tlabel(){
	cerror("code generator asked to make label via tlabel\n");
	}
