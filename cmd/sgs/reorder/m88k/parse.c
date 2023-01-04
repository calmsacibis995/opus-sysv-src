#ident "@(#) $Header: parse.c 4.1 88/04/26 17:22:04 root Exp $ RISC Source Base"
/****************************************************************************/
/* parse.c      this file contains the routines which convert the input     */
/*              format from ascii to the reorganization control structure   */
/****************************************************************************/
/*      The Text and algorithms contained herein are the sole unpublished   */
/*      property of Motorola Inc.  Copyright  1987  All Rights Reserved     */
/****************************************************************************/

# include "types.h"

/****************************************************************************/
/*      data    process data section looking for text directives            */
/****************************************************************************/
/*      data and text parse the input file in a straight forward manner     */
/*--------------------------------------------------------------------------*/
/*      the data section is subject to comment stripping only, and thus the */
/*      processing looks only for comments and a text directive             */
/*--------------------------------------------------------------------------*/
/*      upon encountering a text directive, control is transfered to the    */
/*      text section handler.  By returning the last character parsed,      */
/*      EOF processing is enhanced                                          */
/*--------------------------------------------------------------------------*/
/*******************************************************************	
Modification history:
	WWV7		03/24/88	data
		Do not require a data directive to end scheduling. Allow
		EOF to do this also.
*********************************************************************/


data()
{
	int     c;      /* int for getchar EOF reasons */

	do
	{
		while( iswhite( c = getchar() ) )
			putchar( c );
		if( c == ';' )
		{       /* throw comments away */
			while( ! term( c = getchar() ) );
			continue;
		}

		/* non white--look for text directive */
		/* a direct state machine parser      */
		if( c == 't' )
		{
			putchar( c );
			if( ( c = getchar() ) == 'e' )
			{
				putchar( c );
				if( ( c = getchar() ) == 'x' )
				{
					putchar( c );
					if( ( c = getchar() ) == 't' )
					{
						putchar( c );
						if( ! continues_name( c = getchar() ) )
						{       /* it is a text directive */
							while( ! term( c ) )
							{       /* emit  to end of line */
								putchar( c );
								c = getchar();
							}
							putchar( '\n' );
							c = text( );
						}
					}
				}
			}
		}
		while( ! term( c ) )
		{       /* copy to end of line */
			putchar( c );
			c = getchar();
		}
		putchar( '\n' );
	}
	while( c != EOF );
}

/****************************************************************************/
/*      text    process text section looking for data directives            */
/*                                               labels                     */
/*                                           and instrutions                */
/****************************************************************************/
/*      This parser accepts the M88000 text section assembly language       */
/*      In particular:  a label is    <name>:                               */
/*                  an instruction    <name> [<operand>[delimiter]]*        */
/*--------------------------------------------------------------------------*/

static char buffer[128];

text()
{
	int     c;      /* int for getchar EOF reasons */
	char    *buf,
		*white;

	init_graph();
	do
	{
		buf = &buffer[0];

		/* skip white space */
		while( iswhite( c = getchar() ) );
		if ( c == EOF )
		{       /* its the end of text section */
			reorder();
			printf( "\t%s", buffer );
			return c;
		}

		if( starts_name( c ) )
		{
			do
				*buf++ = c;
			while( continues_name( c = getchar() ) );
			if( c == ':' )
			{       /* its a label */
				*buf++ = '\0';
				label( buffer );
			}
			else if( iswhite( c ) || c == '\n' )
			{       /* smells like an opcode to me */
				white = buf+1;
				if( c != '\n' )
				{
					c = '\0';  /* decouple fields */
					do
					{       /* pack white space out of fields */
						*buf++ = c;
						while( iswhite( c = getchar() ) );
					}
					while( ! term_or_semi( c ) );
				}
				*buf++ = '\0';
				if( buffer[0] == 'd' && buffer[1] == 'a' &&
				    buffer[2] == 't' && buffer[3] == 'a' &&
				    buffer[4] == '\0' )
				{       /* its the end of text section */
					reorder();
					printf( "\t%s", buffer );
					return c;
				}
				operator( buffer, white, buf - white );
			}
		}
		if( c == ';' )
		{
			while( ! term( c ) )
				c = getchar();
		}
	}
	while( c != EOF );
	return c;
}

