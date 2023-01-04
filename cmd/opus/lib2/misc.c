signal(arg1,arg2) {
	printf("signal(%x,%x)\n",arg1,arg2);
}

unlink(arg1,arg2) {
	printf("unlink(%x,%x)\n",arg1,arg2);
}

fork(arg1,arg2) {
	printf("fork(%x,%x)\n",arg1,arg2);
}

execvp(arg1,arg2) {
	printf("execvp(%x,%x)\n",arg1,arg2);
}

wait(arg1,arg2) {
	printf("wait(%x,%x)\n",arg1,arg2);
}

system(arg1,arg2) {
	printf("system(%s)\n",arg1,arg2);
}

asm(".globl cerror");
asm("cerror: .long 0");
