.\"	@(#)asg1	7.5	
.nr H1 17
.H 1 "SYSTEM ASSEMBLER"
.sp 2

.H 2 "Introduction"
This is a reference manual for 
the \*(5) resident assembler, \f2as\f1.
Programmers familiar
with the M88000 family of processors
should be able to program in \f2as\f1
by referring to this manual, but this is not a manual for the
processor itself.  Details about the effects of instructions,
meanings of status register bits, handling of interrupts, and
many other issues are not dealt with here.  This manual, therefore,
should be used in conjunction with the following reference
manuals:
.BL
.LI
MC88000 User's Manual, Advanced Information, Revision 0.3, September 24,
1987.
.LI
MC88000 Procedure Interface Standard, Revision 2.0.
.LI
MC88000 System Debugger Interface Specification.
.LE
.SP 1

.H 2 "Warnings"
The programming environment for a RISC machine like the MC88000 is generally
far more complex than that for traditional CISC type machines.
In particular, the RISC programming tools are far more likely to make
optimizations that the assembly language programmer needs to be aware
of to avoid surprises.
One example is the restrictions on what instructions may go into the
delay slot of a branch instruction.
Another example is the reordering of instructions that may be done by
various standalone optimizers or even by the linker.

.H 2 "Use Of The Assembler"
The command \f2as\f1 invokes the
assembler and has the following syntax:
.DS I
\f3as [ \-o \f2output\f3 ]\f2 file\f1
.DE
When \f2as\f1 is invoked with the \f3\-o\f2 output\f1 flag,
the output of the assembly is put in the file \f2output\f1.
If the \f3\-o\f1 flag is not specified, the output is
left in a file whose name is formed by removing the \f3.s\f1 
suffix, if there is one, from the input filename and
appending a \f3.o\f1 suffix.

.H 2 "General Syntax Rules"
.H 3 "Format of Assembly Language Line"
Typical lines of \f2as\f1 assembly code look like
these:
.DS I
.nf
.sp
;  Compilation of C library function memset().
	def		call1.,0
	def		aut1.,0
	def		arg1.,8
	global		_memset
	text
_memset:
	subu		r31,r31,8
	st		r2,r31,arg1.+0
	st.b		r3,r31,arg1.+4
	st		r4,r31,arg1.+8
	ld		r10,r31,arg1.+0
	st		r10,r31,aut1.+0
@L13:
	ld		r10,r31,arg1.+8
	subu		r11,r10,1
	st		r11,r31,arg1.+8
	cmp		r12,r11,0
	bb0		ge,r12,@L14
	ld		r10,r31,arg1.+0
	ld.b		r11,r31,arg1.+4
	st.b		r11,r10,0
	ld		r10,r31,arg1.+0
	addu		r11,r10,1
	st		r11,r31,arg1.+0
	br		@L13
@L14:
	ld		r2,r31,aut1.+0
@L12:
	addu		r31,r31,8
	jmp		r1
.DE
These general points about the example should be noted:
.BL
.LI
An identifier occurring at the beginning of a line and
followed by a colon (\f3:\f1) is a \f2label\f1.  One
or more \f2label\f1s may precede any assembly language
instruction or pseudo-operation.  Refer to ``Location Counters and Labels.''
later in this chapter
.LI
A line of assembly code need not include an instruction.
It may consist of a comment alone (introduced by ;), a
label alone (terminated by \f3:\f1), or it may be
entirely blank.
.LI
It is good practice to use tabs to align assembly language
operations and their operands into columns, but this is not
a requirement of the assembler.  An opcode may appear at
the beginning of the line, if desired, and spaces may precede
a label.  A single blank or tab suffices to separate an
opcode from its operands.  Additional blanks and tabs are
ignored by the assembler.
.LE
.SP 1
.H 3 "Comments"
Comments are introduced by the character ';' and continue to
the end of the line.  Comments may appear anywhere and are
completely disregarded by the assembler.
.H 3 "Identifiers"
An identifier is a string of characters taken from the
set \f3a-z, A-Z, \(ru\|\|, %\|, @\|, $, \.\|\|,\f1 and\f3 0-9\f1.
However,\f3 0-9\f1 are only valid identifier characters when
they are \f3not\f1 the first character.
Uppercase and
lowercase letters are distinguished; for example,
\f3con35\f1 and \f3\s-1CON\s+135\f1 are two distinct
identifiers.
.P
There is no limit on the length of an identifier.
.P
The value of an identifier is established by the \f3def\f1
pseudo-operation (refer to ``Symbol Definition Operations'')
or by using it as a label (refer to ``Location Counters and Labels'').
.P
The period character (\f3.\|\f1) has special significance
to the assembler.  A \f3.\f1\|\| used alone, as an
identifier, means ``the current location''.  A \f3.\f1\|\|
used as the first character in an identifier has no special
significance, allowing symbols
such as \f3.eos\f1 and \f3.0fake\f1 to be entered into
the symbol table, as required by the Common Object File
Format (COFF).
Information about file formats is provided in
the \f2\*(6) User's Reference Manual\f1.
.H 3 "Register Identifiers"
The predefined register identifiers are:
.BL
.LI
r0 to r31
.LI
cr0 to cr63
.LI
sp
.LE
.SP 1
\f3Notes:\f1
.SP 1
High level language compilers should prepend a '_' character to
symbol names seen by the assembler to avoid nameclashes.
The assembler programmer needs to be aware of this convention.
.SP 1
The identifiers \f3r31\f1 and \f3sp\f1 represent
the same machine register. Use of both \f3r31\f1 and \f3sp\f1
in the same program may result in confusion.

.H 3 "Constants"
\f2As\f1 deals with integer and floating point constants.  They
may be entered in decimal, octal, or hexadecimal, or
they may be entered as character constants.  Internally,
\f2as\f1 treats all integer constants as signed 32-bit binary
two's complement quantities.

.H 4 "Integral Constants.\|\|"
A decimal constant is a string of digits beginning with a
non-zero digit.
An octal constant is a string of digits beginning with
zero.
A hexadecimal constant consists of the characters \f30x\f1
or \f30X\f1 followed by a string of characters from the set
\f30-9\f1, \f3a-f\f1, and \f3A-F\f1.  In hexadecimal
constants, uppercase and lowercase letters are not
distinguished.
.P
Examples:
.DS
.TS
center;
l l l.
def	const,35	; Decimal 35
ld	r4,r28,035	; Octal 35 (decimal 29)
def	const,0x35	; Hex 35 (decimal 53)
ld	r4,r28,0xff	; Hex ff (decimal 255)
.TE
.DE
.H 4 "Character Constants.\|\|"
An ordinary character constant consists of a single-quote
character (\f3'\f1) followed by an arbitrary \s-1ASCII\s+1
character other than the backslash (\f3\e\|\|\f1).  The
value of the constant is equal to the \s-1ASCII\s+1 code for
the character.  Special meanings of characters are
overridden when used in character constants; for example,
if \f3';\f1 is used, the ; is not treated as introducing a
comment.
.P
A special character constant consists of \f3'\e\|\|\f1
followed by another character.  All the special character
constants and examples of ordinary character constants are
listed in the following table.
.TS
center box;
l l c
c c l.
CONSTANT	VALUE	MEANING
_
'\e\|b	0x08	Backspace
'\e\|t	0x09	Horizontal Tab
'\e\|n	0x0a	Newline (Line Feed)
'\e\|v	0x0b	Vertical Tab
'\e\|f	0x0c	Form Feed
'\e\|r	0x0d	Carriage Return
'\e\e	0x5c	Backslash
' '	0x27	Single Quote
'0	0x30	Zero
'A	0x41	Uppercase A
'a	0x61	Lowercase a
.TE
.H 3 "Other Syntactic Details"
For a discussion of expression syntax, see ``Expressions'' in
this chapter.  For information about the syntax of specific
components of \f2as\f1 instructions and
pseudo-operations see ``Pseudo-Operations''.
.H 2 "Segments, Location Counters, And Labels"
.H 3 "Segments"
A program in \f2as\f1 assembly language may be
broken into segments known as \f2text\f1, \f2data\f1, and
\f2bss\f1 segments.  The convention regarding the use of these
segments is to place instructions in \f2text\f1 segments,
initialized data in \f2data\f1 segments, and uninitialized data
in \f2bss\f1 segments.  However, the assembler does not
enforce this convention; for example, it permits intermixing
of instructions and data in a \f2text\f1 segment.

.H 3 "Location Counters and Labels"
The assembler maintains separate \f2location counters\f1
for each of the \f2text\f1, \f2data\f1, and \f2bss\f1 segments.
The location counter for a given segment
is incremented by one for each byte generated in that segment.
.P
The location counters allow values to be assigned to labels.
When an identifier is used as a label in the assembly language
input, the current value of the current location counter
is assigned to the identifier.  The assembler also keeps
track of which segment the label appeared in.  Thus, the
identifier represents a memory location relative to the beginning
of a particular segment.
Any label relative to the location counter should be within
the text segment.

.H 2 "Types"
Identifiers and expressions may have values of different types.
.BL
.LI
In the simplest case, an expression (or identifier) may have an
\f2absolute\f1 value, such as 29, -5000, or 262143.
.LI
An expression (or identifier) may have a value relative to the
start of a particular segment.  Such a value is known as a
\f2relocatable\f1 value.  The memory location represented by
such an expression cannot be known at assembly time, but the
relative values of two such expressions (i.e., the difference
between them)
can be known if they refer to the same segment.
However, use of values computed by arithmetic on relocatable 
expressions is strongly discouraged on the MC88000 because of
optimizations which may be performed by the linker.
.P
Identifiers which appear as labels have \f2relocatable\f1 values.
.LI
If an identifier is never assigned a value, it is assumed to be
an \f2undefined external\f1.  Such identifiers may be used with
the expectation that their values will be defined in another 
program, and therefore known at link time; but the relative
values of \f2undefined externals\f1 cannot be known.
.LE
.SP 1

.H 2 "Expressions"
For conciseness, the following abbreviations are useful:
.TS
center;
l l.
\f3abs\f1	absolute expression
\f3rel\f1	relocatable expression
\f3ext\f1	undefined external
.TE
All constants are absolute expressions.  An identifier may be
thought of as an expression having the identifier's type.
Expressions may be built up from lesser expressions using the
operators \f3+\f1, \f3\-\f1, \f3\(**\f1, and \f3/\|\f1,
according to the following type rules:
.DS I
\f3abs + abs = abs\f1
.br
\f3abs + rel = rel + abs = rel\f1
.br
\f3abs + ext = ext + abs = ext\f1
.br
.sp .5v
\f3abs \- abs = abs\f1
.br
\f3rel \- abs = rel\f1
.br
\f3ext \- abs = ext\f1
.br
\f3rel \- rel = abs\f1
.br
(provided that the two relocatable expressions are relative to the same segment)
.sp .5v
.br
\f3abs \(** abs = abs\f1
.br
.sp .5v
.br
\f3abs / abs = abs\f1
.br
.sp .5v
\f3\- abs = abs\f1
.br
.DE
.SP 1
\f3Note:\f1 Use
of a \f3rel \- rel\f1 expression is dangerous, particularly
when dealing with identifiers from \f2text\f1 segments.
The problem is that the assembler will determine the
value of the expression before any rearrangement of the code to
optimize for minimum pipeline stalls or for 32 bit literal synthesis.
.P
The unary minus operator takes the highest precedence; the
next highest precedence is given to \f3\(**\f1 and \f3/\f1,
and lowest precedence is given to \f3+\f1 and binary
\f3\-\f1.  Parentheses may be used to coerce the order
of evaluation.
.SP 1
.DS I F 5
.ce
\f3NOTE\f1
.SP 1
The above precedence rules are incorrect for the current
implementation, which does not have any precedence rules for the
binary operators.  Operators are evaluated in the order encountered.
Thus (1 + 2 * 3) is equal to 9 and not 7.
It is recommended, for upward compatibility
with future assemblers, that parentheses be used to coerce the order of
evaluation for all expressions containing more than one binary operand.
.DE
.SP 1
.P
If the result of a division is a positive non-integer,
it will be truncated toward zero.  If the result is a
negative non-integer, the direction of truncation cannot
be guaranteed.

./"	@(#)asg2	7.8	
.H 2 "Pseudo-Operations"
.H 3 "Data Initialization Operations"
.VL 15
.LI "\f3align  \f2abs\f1"
.br
Forces alignment to the value given by the absolute expression.
After processing of this pseudo, the current location counter will
be a integral multiple of the value of \f2abs\f1.
.LI "\f3byte   \f2abs\f1,\f2abs\f1,..."
.br
One or more arguments, separated by commas, may be given.
The values of the arguments are computed to produce
successive bytes in the assembly output.
.LI "\f3half   \f2abs\f1,\f2abs\f1,..."
.br
One or more arguments, separated by commas, may be given.
The values of the arguments are computed to produce
successive 16-bit words in the assembly output.
.LI "\f3word   \f2expr\f1,\f2expr\f1,..."
.br
One or more arguments, separated by commas, may be given.
Each expression may be \f2absolute\f1, \f2relocatable\f1,
or \f2undefined external\f1.  A 32-bit quantity is generated
for each such argument (in the case of \f2relocatable\f1
or \f2undefined external\f1 expressions, the actual value
may not be filled in until link time.)
.sp
Alternatively,
the arguments may be bit-field expressions.  A bit-field
expression has the form
.sp
.ti +5
\f2n\f3 :\f2 value\f1
.sp
where both \f2n\f1 and \f2value\f1 denote \f2absolute\f1
expressions.  The quantity \f2n\f1 represents a field width;
the low-order \f2n\f1 bits of \f2value\f1 become the contents
of the bit-field.  Successive bit-fields fill up 32-bit word
quantities starting with the high-order part.  If the
sum of the lengths of the bit-fields is less than 32 bits, the
assembler creates a 32-bit word with zeroes filling out the 
low-order bits.  For example,
.sp
.ti +5
.na
.nf
	\f3word	4: -1, 16: 0x7f,  12:0, 5000\f1
.fi
.sp
and
.sp
.ti +5
.nf
	\f3word	4: -1, 16: 0x7f, 5000\f1
.fi
.sp
are equivalent to
.sp
.ti +5
.nf
	\f3word	0xf007f000, 5000\f1
.fi
.sp
Bit-fields may not span pairs of 32-bit words.  Thus,
.sp
.ti +5
.nf
	\f3word	24: 0xa, 24: 0xb, 24:0xc\f1
.fi
.sp
yields the same thing as
.sp
.ti +5
.nf
	\f3word	0x00000a00, 0x00000b00, 0x00000c00\f1
.fi
.LI "\f3float   \f2abs\f1,\f2abs\f1,..."
.br
One or more arguments, separated by commas, may be given.
The values of the arguments are computed to produce
successive 32-bit IEEE floating point values in the assembly output.
.LI "\f3double   \f2abs\f1,\f2abs\f1,..."
.br
One or more arguments, separated by commas, may be given.
The values of the arguments are computed to produce
successive 64-bit IEEE floating point values in the assembly output.
.LI "\f3string    \f2qstring\f1"
.br
The quoted string \f2qstring\f1 is evaluated as a C style string to
produce successive bytes in the assembly output.
The leading and trailing double quotes are discarded.
.LI "\f3zero\f1   \f2abs\f1"
.br
The value of \f2abs\f1 is computed, and the resultant number
of bytes of zero data is generated.  For example,
.sp
.ti +5
.nf
	\f3zero	6\f1
.fi
.sp
is equivalent to
.sp
.ti +5
.nf
	\f3byte	0,0,0,0,0,0\f1
.fi
.ad
.LE
.SP 1
.H 3 "Symbol Definition Operations"
.VL 15
.LI "\f3def \f2identifier\f1,\f2expr\f1"
.br
The value of \f2identifier\f1 is set equal to \f2expr\f1, which
may be absolute or relocatable.
.LI "\f3comm   \f2identifier,abs\f1"
.br
The named identifier is to be assigned to a common area of
size \f2abs\f1 bytes.  If \f2identifier\f1 is not defined by
another program, the linker will allocate space for it.
.sp
The type of \f2identifier\f1 becomes \f2relocatable\f1.
.LI "\f3global   \f2identifier\f1"
.br
This causes \f2identifier\f1 to be externally visible.  If
\f2identifier\f1 is defined in the current program,
then declaring it global allows the linker to resolve
references to \f2identifier\f1 in other programs.
.sp
If \f2identifier\f1 is not defined in the current
program, the assembler expects an external
resolution; in this case, therefore, \f2identifier\f1 is
global by default.
.LI "\f3static  \f2identifier\f1"
.br
This causes \f2identifier\f1 to be externally invisible.
It may be used to undo the effects of \f3global\f1 (and vice-versa).
.LE
.SP 1
.H 3 "Location Counter Control Operations"
.VL 15
.LI "\f3text\f1"
Direct assembly into the \f2text\f1 segment.
Before the first \f3text\f1 or \f3data\f1 operation is
encountered, assembly is by default directed into the \f2text\f1
segment.
.LI "\f3data\f1"
Direct assembly into the \f2data\f1 (initialized data) segment.
.LI "\f3bss\f1"
Direct assembly into the \f2bss\f1 (uninitialized data) segment.
.LI "\f3previous\f1"
Direct assembly into the previously active segment.
.LE
.SP 1
.H 3 "Symbolic Debugging Operations"
The assembler allows for symbolic debugging information to
be placed into the object code file with special pseudo-operations.
The information typically includes line numbers and information
about C language symbols, such as their type and storage class.
The C compiler (\f2cc\f1(1)) generates symbolic
debugging information when the \f3-g\f1 option is used.
Assembler programmers may also include such information in
source files.
.H 4 "File and Ln.\|\|"
.VL 15
.LI "\f3file   \f2filename\f1"
The \f3file\f1 pseudo-operation passes the name of the source
file into the object file symbol table, where
\f2filename\f1 consists of one to 14 characters
enclosed in quotation marks.
.LI "\f3ln \f2line\f1[,\f2value\f1]"
The \f3ln\f1 pseudo-operation makes a line number table
entry in the object file.  That is, it associates a
line number with a memory location.  Usually the memory
location is the current location in text, and
\f2line\f1 is the line number.  The optional value
is the address in \f2text\f1, \f2data\f1, or \f2bss\f1 to associate with
the line number.  The default when \f2value\f1 is omitted
(which is usually the case) is the current location in \f2text\f1.
.LE
.SP 1
.H 4 "Symbol Attribute Operations.\|\|"
The basic symbolic testing pseudo-operations are \f3sdef\f1
and \f3endef\f1.  These operations enclose other
pseudo-operations that assign attributes to a symbol and
must be paired.
.TS
center;
lB lB lB.
sdef	name	\&
\.	\&	; Attribute
\.	\&	; Assigning
\.	\&	; Operations
endef	\&	\&
.TE
.ce
\f3NOTES\f1
.BL
.LI
\f3sdef\f1 does not define the symbol, although it does create a 
symbol table entry.  Because an undefined symbol is treated as
external, a symbol which appears in a \f3sdef\f1, but which
never acquires a value, will ultimately result in an error
at link edit time.
.LI
To allow the assembler to calculate the sizes of functions for
other tools, each \f3sdef/endef\f1 pair that defines a function
name must be matched by a \f3sdef/endef\f1 pair after the
function in which a storage class of \f3\-1\f1 is assigned.
.LE
.SP 1
.P
The paragraphs below describe the attribute-assigning operations.
Keep in mind that all of these operations apply to symbol \f2name\f1
which appeared in the opening \f3def\f1 pseudo-operation.
.VL 15
.LI "\f3val   \f2expr\f1"
Assigns the value \f2expr\f1 to \f2name\f1.  The type of the
expression \f2expr\f1 determines with which section \f2name\f1
is associated.  If value is \f3~\f1\|\|, the current location
in the \f2text\f1 section is used.
.LI "\f3scl   \f2expr\f1"
Declares a storage class for \f2name\f1.  The expression \f2expr\f1
must yield an \f3\s-1ABSOLUTE\s+1\f1 value that corresponds to
the C compiler's internal representation of a storage class.
The special value \f3\-1\f1 designates the physical end of a
function.
.LI "\f3type   \f2expr\f1"
Declares the C language type of \f2name\f1.  The expression
\f2expr\f1 must yield an \f3\s-1ABSOLUTE\s+1\f1 value that
corresponds to the C compiler's internal representation of a basic
or derived type.
.LI "\f3tag   \f2str\f1"
Associates \f2name\f1 with the structure, enumeration, or
union named \f2str\f1 which must have already been declared
with a \f3def/endef\f1 pair.
.LI "\f3line   \f2expr\f1"
Provides the line number of \f2name\f1, where \f2name\f1 is
a block symbol.  The expression \f2expr\f1 should yield an
\f3\s-1ABSOLUTE\s+1\f1 value that represents a line number.
.LI "\f3size   \f2expr\f1"
Gives a size for \f2name\f1.  The expression \f2expr\f1 must
yield an \f3\s-1ABSOLUTE\s+1\f1 value.  When \f2name\f1 is
a structure or an array with a predetermined extent, \f2expr\f1
gives the size in bytes.  For bit fields, the size is in bits.
.LI "\f3dim   \f2expr1\f1,\f2expr2:expr3\f1,..."
Indicates that \f2name\f1 is an array.  Each of the expressions
must yield an \f3\s-1ABSOLUTE\s+1\f1 value that provides the
corresponding array dimension.
A colon separated pair of expressions, such as "1:10", indicates
an array dimension with specified lower and upper bounds.
.LE
.SP 1
.H 3 "Miscellaneous Pseudos"
.VL 15
.LI "\f3version\f1    \f2qstring\f1"
.br
The quoted string \f2qstring\f1 is evaluated as a C style string
and compared to a builtin version string in the assembler, using strcmp.
A warning message is issued if \f2qstring\f1 is lexically greater than
the builtin version string.
For example:
.sp
.ti +5
.na
.nf
\f3version\f1  "02.02"
.fi
.ad
.sp
would issue a warning if the builtin version string was "02.01" but not
if the builtin version string was "02.03".
This feature allows setting of a minimum assembler revision level that
is capable of assembling a given file, when there has been an assembler
enhancement which has added new features that are used in that file.
Attempts to assemble the file with earlier revisions of the assembler will
trigger a warning.
.LI "\f3ident\f1    \f2qstring\f1"
The quoted string \f2qstring\f1 is evaluated as a C style string
and added to a special section called ".comment"
in the COFF object file, after stripping off the leading and trailing quotes.
The string is null terminated in the ".comment" section.
The strings are typically used to track revision levels of modules used
in building a given executable.
.LE
.SP 1
.H 3 "Undefined Semantics Pseudos"
The following pseudos are vestigial remnants from the port of the
assembler from the original base source code.
The semantics are currently undefined.
They may eventually either be removed or defined.
.VL 15
.LI "\f3il\f1"
Pseudo instruction with currently undefined semantics.
.LI "\f3section\f1"
Pseudo instruction with currently undefined semantics.
.LI "\f3decfp\f1"
Pseudo instruction with currently undefined semantics.
.LI "\f3decint\f1"
Pseudo instruction with currently undefined semantics.
.LI "\f3doublx\f1"
Pseudo instruction with currently undefined semantics.
.LE
.SP 1
.H 2 "Pseudo-Functions"
The assembler implements several operations that syntactically look
like C function calls, but provide specialized relocation or link time
functionality.
.H 3 "Bit Field Extractions"
.VL 15
.LI "\f3lo16(\f2expr\f3)\f1"
Compute the value of \f2expr\f1 at link time, using 32-bit signed arithmetic,
and discard all bits except the least significant 16-bits.
The resulting field is treated as a 16-bit unsigned bit field.
.LI "\f3hi16(\f2expr\f3)\f1"
Compute the value of \f2expr\f1 at link time, using 32-bit signed arithmetic,
and discard all bits except the most significant 16-bits.
The resulting field is treated as a 16-bit unsigned bit field and is
right shifted as necessary to move the lsb down to bit zero.
.LE
.SP 1

.H 2 "Machine Instructions"
.H 3 "Instructions For The MC88000"
The following table shows how MC88000 instructions
should be written in order to be understood correctly by
the \f2as\f1 assembler.
.P
Abbreviations used in the table are:
.VL 15
.LI "\f3rs1\f1"
Register containing the first source operand.
.LI "\f3rs2\f1"
Register containing the second source operand.
.LI "\f3rd\f1"
Register destination that will be modified by the operation.
.LI "\f3rsd\f1"
Register source and/or destination, depending upon the context.
.LI "\f3crd\f1"
Control register destination.
.LI "\f3off16\f1"
Sixteen bit signed instruction (word) offset from the address in the
instruction pointer.
.LI "\f3off26\f1"
Twenty-six bit signed instruction (word) offset from the address in the
instruction pointer.
.LI "\f3lit16\f1"
Unsigned sixteen bit literal value.
.LI "\f3vec9\f1"
Trap vector offset (double word) from the page address contained in the Vector
Base Register.
Vectors are formed by concatenating the 20-bit Vector Base Register
with the 9-bit vec9 field, followed by a 3-bit field of zeros.
.LI "\f3m5\f1"
Five bit condition match field.
The bits indicate the following conditions:
.br
.sp
<0>:  (not S) and (not Z)
.br
<1>:  (not S) and Z
.br
<2>:  S and (not Z)
.br
<3>:  S and Z
.br
<4>:  Reserved; not used by the condition logic.
.br
.sp
Where:
.br
.sp
S:  Sign bit (bit 31 of the tested register).
.br
Z:  Zero bit (logical NOR of bits 30 through 0 of the tested register).
.br
.sp
The following symbolic constants are recognized for an \f3m5\f1 field:
.br
.TS
center box;
l l l c
c c c l.
SYMBOL	ALTERNATE	VALUE	MEANING
_
gt0	s0z0	0x1	greater than zero
eq0	s0z1	0x2	equal to zero
ge0	(none)	0x3	greater than or equal to zero
(none)	s1z0	0x4	negative but not max negative
(none)	s1z1	0x8	maximum negative
lt0	(none)	0xc	less than zero
ne0	(none)	0xd	not equal to zero
le0	(none)	0xe	less than or equal to zero
.TE
.sp
.LI "\f3b5\f1"
Unsigned 5-bit integer denoting a bit number.
The following symbolic constants are recognized for an \f3b5\f1 field
and are typically used in a branch or trap instruction after a cmp:
.br
.TS
center box;
l l c
c c l.
SYMBOL	VALUE	MEANING
_
eq	0x2	equal
ne	0x3	not equal
gt	0x4	greater than
le	0x5	less than or equal
lt	0x6	less than
ge	0x7	greater than or equal
hi	0x8	xxxx
ls	0x9	xxxx
lo	0xa	xxxx
hs	0xb	xxxx
.TE
.LI "\f3o5\f1"
Unsigned 5-bit integer denoting a bit field offset.
.LI "\f3w5\f1"
Unsigned 5-bit integer denoting a field width, with 0 denoting a width
of 32.
.LI "\f3.n\f1"
Pipeline control option.
If specified, execute the next sequential instruction before the branch
target instruction.
.LI "\f3.c\f1"
Data control option.
If specified, the second operand is complemented before it is used in
the operation.
.LI "\f3.u\f1"
Select control operation.
If specified, the 16-bit logical operation is to be performed with the
upper 16-bits of the source or destination register.
.LI "\f3.ci\f1"
Carry in option.
If specified, include the PSR Carry bit in the arithmetic operation.
.LI "\f3.co\f1"
Carry out option.
If specified, set or clear the PSR Carry bit based on the result of the
arithmetic operation.
.LI "\f3.cio\f1"
Carry in, carry out option.
If specified, include the PSR Carry bit in the arithmetic operation and
set or clear the carry bit based on the result.
.LI "\f3.usr\f1"
Specifies that the memory access must be in the user address space
regardless of the mode bit (user or supervisor) in the Processor
Status Register.
.LI "\f3.b\f1"
Byte (8 bits).
.LI "\f3.bu\f1"
Unsigned byte (8 bits).
.LI "\f3.h\f1"
Half word (16 bits).
.LI "\f3.hu\f1"
Unsigned half word (16 bits).
.LI "\f3.s\f1"
Single word (32 bits).
.LI "\f3.d\f1"
Double word (64 bits).
.LI "\f3.fsz\f1"
Floating point operand size.
Floating point operations support mixed operand sizes; two or three
register operands will use two or three of the \f2.s\f1 or \f2.d\f1
qualifiers in any combination to support the operand size mix.
.LE

.bp
.S -2
.nf
.ad
.TS
center box expand;
c s s s
cw(1i) | cw(.75i) sw(1i) | cw(3.5i)
lBw(1i) | lBw(.75i) lBw(1i) | ltw(3.5i).
MC88000 INSTRUCTION FORMATS
_
INSTRUCTION	ASSEMBLER SYNTAX	OPERATION
=
ADD	add	T{
rd,rs1,rs2
rd,rs1,lit16
T}	T{
.fi
.ad
Signed add, without carry.
.nf
.na
T}
.sp
\^	add.ci	rd,rs1,rs2	T{
.fi
.ad
Signed add, with carry-in bit added to result.
.nf
.na
T}
.sp
\^	add.co	rd,rs1,rs2	T{
.fi
.ad
Signed add, with generated carry-out bit written to psr.
.nf
.na
T}
.sp
\^	add.cio	rd,rs1,rs2	T{
.fi
.ad
Signed add, with carry-in bit added to result and generated carry-out bit
written to psr.
.nf
.na
T}
_
ADDU	addu	T{
rd,rs1,rs2
rd,rs1,lit16
T}	T{
.fi
.ad
Unsigned add, without carry.
.nf
.na
T}
.sp
\^	addu.ci	rd,rs1,rs2	T{
.fi
.ad
Unsigned add, with carry-in bit added to result.
.nf
.na
T}
.sp
\^	addu.co	rd,rs1,rs2	T{
.fi
.ad
Unsigned add, with generated carry-out bit written to psr.
.nf
.na
T}
.sp
\^	addu.cio	rd,rs1,rs2	T{
.fi
.ad
Unsigned add, with carry-in bit added to result and generated carry-out bit
written to psr.
.nf
.na
T}
_
AND	and	rd,rs1,rs2	T{
.fi
.ad
Rs1 and rs2 are logically ANDed and results placed in rd.
.nf
.na
T}
.sp
\^	and	rd,rs1,lit16	T{
.fi
.ad
Lower 16 bits of rs1 are logically ANDed with 16 bit literal value and
result placed in lower 16 bits of rd.
The upper 16 bits of rd and rs1 remain unchanged.
.nf
.na
T}
.sp
\^	and.c	rd,rs1,rs2	T{
.fi
.ad
Rs1 and complement of rs2 are logically ANDed and results placed in rd.
.nf
.na
T}
.sp
\^	and.u	rd,rs1,lit16	T{
.fi
.ad
Upper 16 bits of rs1 are logically ANDed with 16 bit literal value and
result placed in upper 16 bits of rd.
The lower 16 bits of rd and rs1 remain unchanged.
.nf
.na
T}
_
BB0	T{
bb0
bb0.n
T}	b5,rs1,off16	T{
.fi
.ad
If bit in rs1 specified by b5 field is clear, a branch is
taken to the instruction off16 words from the current
instruction.  Off16 is signed.  For \f2.n\f1 form, the
next instruction is always executed before the branch.
.nf
.na
T}
_
BB1	T{
bb1
bb1.n
T}	b5,rs1,off16	T{
.fi
.ad
If bit in rs1 specified by b5 field is set, a branch is
taken to the instruction off16 words from the current
instruction.  Off16 is signed.  For \f2.n\f1 form, the
next instruction is always executed before the branch.
.nf
.na
T}
_
BCND	T{
bcnd
bcnd.n
T}	m5,rs1,off16	T{
.fi
.ad
If data in rs1 register meets conditions specified by m5 field
a branch is taken to the instruction off16 words from the current
instruction.  Off16 is signed.
For \f2.n\f1 form, the next instruction is always executed before the branch.
.nf
.na
T}
_
BR	T{
br
br.n
T}	off26	T{
.fi
.ad
Unconditional branch to instruction off26 words from the current instruction.
Off26 is signed.
For \f2.n\f1 form, the next instruction is always executed before the branch.
.nf
.na
T}
_
BSR	T{
bsr
bsr.n
T}	off26	T{
.fi
.ad
Branch to subroutine at off26 words from the current instruction.
Off26 is signed.
For \f2.n\f1 form, the next instruction is always executed before the branch.
.nf
.na
T}
_
CLR	clr	T{
rd,rs1,w5<o5>
rd,rs1,rs2
T}	T{
.fi
.ad
Set bit field of all zeros, in data from rs1, according to width and offset
specification in w5<o5>, or lower 10 bits of rs2, and place result in rd.
Rs1 is unchanged.
.nf
.na
T}
_
CMP	cmp	T{
rd,rs1,lit16
rd,rs1,rs2
T}	T{
.fi
.ad
Integer comparison of rs1 with either unsigned,
zero extended 16-bit literal or rs2, returning evaluated conditions
as bit string in rd.
.nf
.na
T}
_
DIV	div	T{
rd,rs1,lit16
rd,rs1,rs2
T}	T{
.fi
.ad
Signed 32-bit twos complement integer divide of rs1 by either unsigned,
zero-extended 16-bit literal value or rs2, with quotient stored in rd.
A divide error exception occurs if divisor is zero or either operand
is less than zero.
.nf
.na
T}
_
DIVU	divu	T{
rd,rs1,lit16
rd,rs1,rs2
T}	T{
.fi
.ad
Unsigned 32-bit twos complement integer divide of rs1 by either unsigned,
zero-extended 16-bit literal value or rs2, with quotient stored in rd.
A divide error exception occurs if divisor is zero.
.nf
.na
T}
_
.TE
.S +2
.bp
.S -2
.TS
center box expand;
c s s s
cw(1i) | cw(.75i) sw(1i) | cw(3.5i)
lBw(1i) | lBw(.75i) lBw(1i) | ltw(3.5i).
MC88000 INSTRUCTION FORMATS
_
INSTRUCTION	ASSEMBLER SYNTAX	OPERATION
=
EXT	ext	T{
rd,rs1,w5<o5>
rd,rs1,rs2
T}	T{
.fi
.ad
Extract signed bit field from rs1 according to width and offset
specification in w5<o5>, or lower 10 bits of rs2, and place 
sign-extended result in rd.
.nf
.na
T}
_
EXTU	extu	T{
rd,rs1,w5<o5>
rd,rs1,rs2
T}	T{
.fi
.ad
Extract unsigned bit field from rs1 according to width and offset
specification in w5<o5>, or lower 10 bits of rs2, and place 
zero-extended result in rd.
.nf
.na
T}
_
FADD	T{
fadd.sss
fadd.ssd
fadd.sds
fadd.sdd
fadd.dss
fadd.dsd
fadd.dds
fadd.ddd
T}	rd,rs1,rs2	T{
.fi
.ad
Floating point addition of rs1 and rs2, with result placed in rd.
The size of each operand is determined by the corresponding
character, \f2s\f1 for single precision and \f2d\f1 for double precision,
in the assembler instruction extension.
Arithmetic is performed according to the IEEE P754 Binary Floating
Point Standard.
.nf
.na
T}
_
FCMP	T{
fcmp.sss
fcmp.ssd
fcmp.sds
fcmp.sdd
T}	rd,rs1,rs2	T{
Subtracts rs2 from rs1 and evaluates a number of conditions using the
IEEE P754 Binary Floating Point Standard.  Returns evaluation result
as a bit string in rd.
The size of each source operand is determined by the corresponding
character, \f2s\f1 for single precision and \f2d\f1 for double precision,
in the assembler instruction extension.
T}
_
FDIV	T{
fdiv.sss
fdiv.ssd
fdiv.sds
fdiv.sdd
fdiv.dss
fdiv.dsd
fdiv.dds
fdiv.ddd
T}	rd,rs1,rs2	T{
.fi
.ad
Floating point division of rs1 by rs2, with result placed in rd.
The size of each operand is determined by the corresponding
character, \f2s\f1 for single precision and \f2d\f1 for double precision,
in the assembler instruction extension.
Arithmetic is performed according to the IEEE P754 Binary Floating
Point Standard.
.nf
.na
T}
_
FF0	ff0	rd,rs2	T{
.fi
.ad
Scan rs2 from MSB to LSB and place in rd the bit number of first bit
found clear, zero for LSB, 31 for MSB, or 32 if no bits found clear.
.nf
.na
T}
_
FF1	ff1	rd,rs2	T{
.fi
.ad
Scan rs2 from MSB to LSB and place in rd the bit number of first bit
found set, zero for LSB, 31 for MSB, or 32 if no bits found set.
.nf
.na
T}
_
FLDCR	fldcr	rd,fcrs	T{
.fi
.ad
Load rd with value from floating point control register fcrs.
Control register 0-8 are privileged, register 62 and 63 are
user floating point control and status registers.
.nf
.na
T}
_
FLT	T{
flt.ss
flt.ds
T}	rd,rs2	T{
.fi
.ad
The signed integer in rs2 is converted to floating point and the
result placed in rd.
The size of the floating point result is determined by
the corresponding character, \f2s\f1 for single precision
and \f2d\f1 for double precision, in the assembler
instruction extension.
.nf
.na
T}
_
FMUL	T{
fmul.sss
fmul.ssd
fmul.sds
fmul.sdd
fmul.dss
fmul.dsd
fmul.dds
fmul.ddd
T}	rd,rs1,rs2	T{
.fi
.ad
Floating point multiplication of rs1 and rs2, with result placed in rd.
The size of each operand is determined by the corresponding
character, \f2s\f1 for single precision and \f2d\f1 for double precision,
in the assembler instruction extension.
Arithmetic is performed according to the IEEE P754 Binary Floating
Point Standard.
.nf
.na
T}
_
FSTCR	fstcr	rs,fcrd	T{
.fi
.ad
Store value in rs to floating point control register fcrd.
Control register 0-8 are privileged, register 62 and 63 are
user floating point control and status registers.
.nf
.na
T}
_
FSUB	T{
fsub.sss
fsub.ssd
fsub.sds
fsub.sdd
fsub.dss
fsub.dsd
fsub.dds
fsub.ddd
T}	rd,rs1,rs2	T{
.fi
.ad
Floating point subtraction of rs2 from rs1, with result placed in rd.
The size of each operand is determined by the corresponding
character, \f2s\f1 for single precision and \f2d\f1 for double precision,
in the assembler instruction extension.
Arithmetic is performed according to the IEEE P754 Binary Floating
Point Standard.
.nf
.na
T}
.TE
.S +2
.bp
.S -2
.TS
center box expand;
c s s s
cw(1i) | cw(.75i) sw(1i) | cw(3.5i)
lBw(1i) | lBw(.75i) lBw(1i) | ltw(3.5i).
MC88000 INSTRUCTION FORMATS
_
INSTRUCTION	ASSEMBLER SYNTAX	OPERATION
=
FXCR	fxcr	rd,rs1,fcrsd	T{
.fi
.ad
Store value in rs1 into floating point control register and load
rd with previous value of floating point control register.
Control register 0-8 are privileged, register 62 and 63 are
user floating point control and status registers.
.nf
.na
T}
_
INT	T{
int.ss
int.sd
T}	rd,rs2	T{
.fi
.ad
Convert floating point value in rs2 to integral value in rd.
The size of the floating point source operand is determined by
the corresponding character, \f2s\f1 for single precision
and \f2d\f1 for double precision, in the assembler
instruction extension.
.nf
.na
T}
_
JMP	T{
jmp
jmp.n
T}	rs2	T{
.fi
.ad
Unconditional jump to address in rs2.
The two LSB's of rs2 are ignored.
For \f2.n\f1 form, the next instruction is always executed before the branch.
.nf
.na
T}
_
JSR	T{
jsr
jsr.n
T}	rs2	T{
.fi
.ad
Jump to subroutine at address in rs2.
The two LSB's of rs2 are ignored.
For \f2.n\f1 form, the next instruction is always executed before the branch.
.nf
.na
T}
_
LD	ld.b	T{
rd,rs1,lit16
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Load signed byte from memory into destination register rd.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for byte sized objects.
.nf
.na
T}
.sp
\^	ld.bu	T{
rd,rs1,lit16
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Load unsigned byte from memory into destination register rd.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for byte sized objects.
.nf
.na
T}
.sp
\^	ld.h	T{
rd,rs1,lit16
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Load signed half word from memory into destination register rd.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for half word sized objects.
.nf
.na
T}
.sp
\^	ld.hu	T{
rd,rs1,lit16
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Load unsigned half word from memory into destination register rd.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for half word sized objects.
.nf
.na
T}
.sp
\^	ld	T{
rd,rs1,lit16
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Load word from memory into destination register rd.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for word sized objects.
.nf
.na
T}
.sp
\^	ld.d	T{
rd,rs1,lit16
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Load double word from memory into destination register rd.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for double word sized objects.
.nf
.na
T}
.sp
\^	ld.b.usr	rd,rs1,rs2	T{
.fi
.ad
Load signed byte from user address space memory into destination register rd.
Effective address is rs1 plus rs2.
.nf
.na
T}
.sp
\^	ld.bu.usr	rd,rs1,rs2	T{
.fi
.ad
Load unsigned byte from user address space memory into destination register rd.
Effective address is rs1 plus rs2.
.nf
.na
T}
.sp
\^	ld.h.usr	T{
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Load signed half word from user address space memory into destination register
rd.
Effective address is rs1 plus rs2.
Rs2 can be unscaled or scaled for half word sized objects.
.nf
.na
T}
.sp
\^	ld.hu.usr	T{
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Load signed half word from user address space memory into destination register
rd.
Effective address is rs1 plus rs2.
Rs2 can be unscaled or scaled for half word sized objects.
.nf
.na
T}
.sp
\^	ld.usr	T{
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Load word from user address space memory into destination register rd.
Effective address is rs1 plus rs2.
Rs2 can be unscaled or scaled for word sized objects.
.nf
.na
T}
.sp
\^	ld.d.usr	T{
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Load double word from user address space memory into destination register rd.
Effective address is rs1 plus rs2.
Rs2 can be unscaled or scaled for double word sized objects.
.nf
.na
T}
.TE
.S +2
.bp
.S -2
.TS
center box expand;
c s s s
cw(1i) | cw(.75i) sw(1i) | cw(3.5i)
lBw(1i) | lBw(.75i) lBw(1i) | ltw(3.5i).
MC88000 INSTRUCTION FORMATS
_
INSTRUCTION	ASSEMBLER SYNTAX	OPERATION
=
LDA	lda.b	T{
rd,rs1,lit16
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Form effective address in destination register rd by adding contents of
rs1 to either unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for byte sized objects.
.nf
.na
T}
.sp
\^	lda.h	T{
rd,rs1,lit16
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Form effective address in destination register rd by adding contents of
rs1 to either unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for half word sized objects.
.nf
.na
T}
.sp
\^	lda	T{
rd,rs1,lit16
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Form effective byte address in destination register rd by adding contents of
rs1 to either unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for word sized objects.
.nf
.na
T}
.sp
\^	lda.d	T{
rd,rs1,lit16
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Form effective byte address in destination register rd by adding contents of
rs1 to either unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for double word sized objects.
.nf
.na
T}
_
LDCR	ldcr	rd,crs	T{
.fi
.ad
Load destination register rd from Integer Unit control register crs.
This is a priviledged instruction.
.nf
.na
T}
_
MAK	mak	T{
rd,rs1,w5<o5>
rd,rs1,rs2
T}	T{
.fi
.ad
Use width and offset specification in w5<o5>, or lower 10 bits of rs2,
to extract bit field from LSB's of rs1 and place in rd at the specified
offset.
All other bits in rd are set to zero.
.na
.nf
T}
_
MASK	T{
mask
mask.u
T}	rd,rs1,lit16	T{
.fi
.ad
Lower 16 bits of rs1 are logically ANDed with 16 bit literal value and
result placed in lower 16-bits of rd, with upper 16-bits of rd zeroed.
For \f2.u\f1 form, the result is placed in the upper 16-bits of rd with
the lower 16-bits zeroed.
.nf
.na
T}
_
MUL	mul	T{
rd,rs1,lit16
rd,rs1,rs2
T}	T{
.fi
.ad
Multiply value in rs1 by either unsigned, zero extended 16-bit literal value,
or value in rs2, and place least significant 32 bits of product in rd.
.nf
.na
T}
_
NINT	T{
nint.ss
nint.sd
T}	rd,rs2	T{
.fi
.ad
Convert floating point value in rs2 to integral value in rd,
using IEEE "round to nearest" rounding method.
The size of the floating point source operand is determined by
the corresponding character, \f2s\f1 for single precision
and \f2d\f1 for double precision, in the assembler
instruction extension.
.nf
.na
T}
_
OR	or	rd,rs1,rs2	T{
.fi
.ad
Rs1 and rs2 are logically ORed and results placed in rd.
.nf
.na
T}
.sp
\^	or	rd,rs1,lit16	T{
.fi
.ad
Lower 16 bits of rs1 are logically ORed with 16 bit literal value and
result placed in lower 16 bits of rd.
The upper 16 bits of rd and rs1 remain unchanged.
.nf
.na
T}
.sp
\^	or.c	rd,rs1,rs2	T{
.fi
.ad
Rs1 and complement of rs2 are logically ORed and results placed in rd.
.nf
.na
T}
.sp
\^	or.u	rd,rs1,lit16	T{
.fi
.ad
Upper 16 bits of rs1 are logically ORed with 16 bit literal value and
result placed in upper 16 bits of rd.
The lower 16 bits of rd and rs1 remain unchanged.
.nf
.na
T}
_
ROT	rot	T{
rd,rs1,<o5>
rd,rs1,rs2
T}	T{
.fi
.ad
Rotate bits in rs1 to the right by the number of bits specified by
the <o5> field, or lower five bits of rs2.
.nf
.na
T}
_
RTE	rte	\ 	T{
.fi
.ad
Return from exception.
This instruction is priviledged.
.nf
.na
T}
_
SET	set	T{
rd,rs1,w5<o5>
rd,rs1,rs2
T}	T{
.fi
.ad
Set bit field of all ones, in data from rs1, according to width and offset
specification in w5<o5>, or lower 10 bits of rs2, and place result in rd.
Rs1 is unchanged.
.nf
.na
T}
.TE
.S +2
.bp
.S -2
.TS
center box expand;
c s s s
cw(1i) | cw(.75i) sw(1i) | cw(3.5i)
lBw(1i) | lBw(.75i) lBw(1i) | ltw(3.5i).
MC88000 INSTRUCTION FORMATS
_
INSTRUCTION	ASSEMBLER SYNTAX	OPERATION
=
ST	st.b	T{
rs,rs1,lit16
rs,rs1,rs2
rs,rs1[rs2]
T}	T{
.fi
.ad
Store signed byte to memory from source register rs.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for byte sized objects.
.nf
.na
T}
.sp
\^	st.bu	T{
rs,rs1,lit16
rs,rs1,rs2
rs,rs1[rs2]
T}	T{
.fi
.ad
Store unsigned byte to memory from source register rs.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for byte sized objects.
.nf
.na
T}
.sp
\^	st.h	T{
rs,rs1,lit16
rs,rs1,rs2
rs,rs1[rs2]
T}	T{
.fi
.ad
Store signed half wors to memory from source register rs.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for half words sized objects.
.nf
.na
T}
.sp
\^	st.hu	T{
rs,rs1,lit16
rs,rs1,rs2
rs,rs1[rs2]
T}	T{
.fi
.ad
Store unsigned half word to memory from source register rs.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for half word sized objects.
.nf
.na
T}
.sp
\^	st	T{
rs,rs1,lit16
rs,rs1,rs2
rs,rs1[rs2]
T}	T{
.fi
.ad
Store word to memory from source register rs.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for word sized objects.
.nf
.na
T}
.sp
\^	st.d	T{
rs,rs1,lit16
rs,rs1,rs2
rs,rs1[rs2]
T}	T{
.fi
.ad
Store double word to memory from source register rs.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for double word sized objects.
.nf
.na
T}
.sp
\^	st.b.usr	rs,rs1,rs2	T{
.fi
.ad
Store signed byte to user address space memory from source register rs.
Effective address is rs1 plus rs2.
.nf
.na
T}
.sp
\^	st.h.usr	T{
rs,rs1,rs2
rs,rs1[rs2]
T}	T{
.fi
.ad
Store signed half word to user address space memory from source register rs.
Effective address is rs1 plus rs2.
Rs2 can be unscaled or scaled for half word sized objects.
.nf
.na
T}
.sp
\^	st.usr	T{
rs,rs1,rs2
rs,rs1[rs2]
T}	T{
.fi
.ad
Store word to user address space memory from source register rs.
Effective address is rs1 plus rs2.
Rs2 can be unscaled or scaled for word sized objects.
.nf
.na
T}
.sp
\^	st.d.usr	T{
rs,rs1,rs2
rs,rs1[rs2]
T}	T{
.fi
.ad
Store double word to user address space memory from source register rs.
Effective address is rs1 plus rs2.
Rs2 can be unscaled or scaled for double word sized objects.
.nf
.na
T}
_
STCR	stcr	rs,crd	T{
.fi
.ad
Store source register rs to Integer Unit control register crd.
This is a priviledged instruction.
.nf
.na
T}
_
SUB	sub	T{
rd,rs1,rs2
rd,rs1,lit16
T}	T{
.fi
.ad
Signed subtraction, without carry.
.nf
.na
T}
.sp
\^	sub.ci	rd,rs1,rs2	T{
.fi
.ad
Signed subtraction, with carry-in bit added to result.
.nf
.na
T}
.sp
\^	sub.co	rd,rs1,rs2	T{
.fi
.ad
Signed subtraction, with generated carry-out bit written to psr.
.nf
.na
T}
.sp
\^	sub.cio	rd,rs1,rs2	T{
.fi
.ad
Signed subtraction, with carry-in bit added to result and generated 
carry-out bit written to psr.
.nf
.na
T}
.TE
.S +2
.bp
.S -2
.TS
center box expand;
c s s s
cw(1i) | cw(.75i) sw(1i) | cw(3.5i)
lBw(1i) | lBw(.75i) lBw(1i) | ltw(3.5i).
MC88000 INSTRUCTION FORMATS
_
INSTRUCTION	ASSEMBLER SYNTAX	OPERATION
=
SUBU	subu	T{
rd,rs1,rs2
rd,rs1,lit16
T}	T{
.fi
.ad
Unsigned subtraction, without carry.
.nf
.na
T}
.sp
\^	subu.ci	rd,rs1,rs2	T{
.fi
.ad
Unsigned subtraction, with carry-in bit added to result.
.nf
.na
T}
.sp
\^	subu.co	rd,rs1,rs2	T{
.fi
.ad
Unsigned subtraction, with generated carry-out bit written to psr.
.nf
.na
T}
.sp
\^	subu.cio	rd,rs1,rs2	T{
.fi
.ad
Unsigned subtraction, with carry-in bit added to result and generated 
carry-out bit written to psr.
.nf
.na
T}
_
TB0	tb0	b5,rs1,vec9	T{
.fi
.ad
Trap on bit clear.
Examine bit in rs1 specified by b5 field and if clear, trap to
vector given by vec9.
Vectors are formed by concatenating the 20-bit Vector Base Register
with the 9-bit vec9 field, followed by a 3-bit field of zeros..
nf
.na
T}
_
TB1	tb1	b5,rs1,vec9	T{
.fi
.ad
Trap on bit set.
Examine bit in rs1 specified by b5 field and if set, trap to
vector given by vec9.
Vectors are formed by concatenating the 20-bit Vector Base Register
with the 9-bit vec9 field, followed by a 3-bit field of zeros.
.nf
.na
T}
_
TBND	tbnd	T{
rs1,lit16
rs1,rs2
T}	T{
.fi
.ad
Trap on bounds check.
Compare rs1 to either an unsigned 16-bit literal, or rs2, using unsigned
arithmetic.
Take bounds check trap if rs1 is larger.
.nf
.na
T}
_
TCND	tcnd	m5,rs1,vec9	T{
.fi
.ad
Conditional trap.
If data in rs1 register meets conditions specified by m5 field
a trap is taken to the vector given by vec9.
Vectors are formed by concatenating the 20-bit Vector Base Register
with the 9-bit vec9 field, followed by a 3-bit field of zeros.
.nf
.na
T}
_
TRNC	T{
trnc.ss
trnc.sd
T}	rd,rs2	T{
.fi
.ad
Convert floating point value in rs2 to integral value in rd,
using IEEE "round to zero" rounding method.
The size of the floating point source operand is determined by
the corresponding character, \f2s\f1 for single precision
and \f2d\f1 for double precision, in the assembler
instruction extension.
.nf
.na
T}
_
XCR	xcr	rd,rs1,crsd	T{
.fi
.ad
Exchange control register.
Store value in rs1 into control register and load
rd with previous value of control register.
.nf
.na
T}
_
XMEM	xmem.bu	T{
rd,rs1,lit16
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Exchange unsigned byte from memory with destination register rd.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for byte sized objects.
.nf
.na
T}
.sp
\^	xmem	T{
rd,rs1,lit16
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Exchange word from memory with destination register rd.
Effective address is rs1 plus unsigned 16-bit literal or signed value in rs2.
Rs2 can be unscaled or scaled for word sized objects.
.nf
.na
T}
.sp
\^	xmem.bu.usr	T{
rd,rs1,rs2
T}	T{
.fi
.ad
Exchange unsigned byte from user address space memory with destination 
register rd.
Effective address is rs1 plus signed value in rs2.
.nf
.na
T}
.sp
\^	xmem.usr	T{
rd,rs1,rs2
rd,rs1[rs2]
T}	T{
.fi
.ad
Exchange word from user address space memory with destination register rd.
Effective address is rs1 plus signed value in rs2.
Rs2 can be unscaled or scaled for word sized objects.
.nf
.na
T}
_
XOR	xor	rd,rs1,rs2	T{
.fi
.ad
Rs1 and rs2 are logically XORed and results placed in rd.
.nf
.na
T}
.sp
\^	xor	rd,rs1,lit16	T{
.fi
.ad
Lower 16 bits of rs1 are logically XORed with 16 bit literal value and
result placed in lower 16 bits of rd.
The upper 16 bits of rd and rs1 remain unchanged.
.nf
.na
T}
.sp
\^	xor.c	rd,rs1,rs2	T{
.fi
.ad
Rs1 and complement of rs2 are logically XORed and results placed in rd.
.nf
.na
T}
.sp
\^	xor.u	rd,rs1,lit16	T{
.fi
.ad
Upper 16 bits of rs1 are logically XORed with 16 bit literal value and
result placed in upper 16 bits of rd.
The lower 16 bits of rd and rs1 remain unchanged.
.nf
.na
T}
.TE
.na
.fi
.S +2
.bp

