

prttystate(tp)
register struct tty *tp;
{
	printf("tty%d st %x flg(i %x, o %x, c %x, l %x)", 
		tp->t_minor, tp->t_state, 
		tp->t_iflag, tp->t_oflag, tp->t_cflag, tp->t_lflag);
	printf(" qsz(r %x, c %x, o %x)\n", 
			tp->t_rawq.c_cc, tp->t_canq.c_cc, tp->t_outq.c_cc);
}
